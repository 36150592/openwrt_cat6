#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "request.h"
#include "log.h"
#include "util.h"

/* decs public variables */
struct list_head request_list;

/* defs static functions */
static int request_open_socket(struct request *request);
static void request_connect(struct request *request);
static void request_process(struct request *request);
static void request_process_send(struct request *request);
static void request_process_recv(struct request *request);

/*
 * decs static functions
 */
static int request_open_socket(struct request *request)
{
	int flags;
	struct sockaddr_in sockname;

        /* create socket */
        request->s = socket(PF_INET, SOCK_STREAM, 0);
        if(request->s < 0)
        {
                log_error("socket(): %s", strerror(errno));
                goto exit_error;
        }

        log_debug("&request: %p, open socket %d",
                  request, request->s);

        if((flags = fcntl(request->s, F_GETFL, 0)) < 0)
        {
		log_error("fcntl(..F_GETFL..): %s", strerror(errno));
		goto exit_error;
	}

        /* no blockant */
	if(fcntl(request->s, F_SETFL, flags | O_NONBLOCK) < 0)
        {
		log_error("fcntl(..F_SETFL..): %s", strerror(errno));
		goto exit_error;
        }

        /* bind ? */
        if(request->opt.mask & REQ_OPT_BIND_ADDR)
        {
                memset(&sockname, 0, sizeof(struct sockaddr_in));
                sockname.sin_family = AF_INET;
                sockname.sin_addr.s_addr = request->opt.bind_addr.s_addr;

                log_debug("&request: %p, bind to %s",
                          request, inet_ntoa(sockname.sin_addr));

                if(bind(request->s,
                        (struct sockaddr *)&sockname,
                        (socklen_t)sizeof(struct sockaddr_in)) < 0)
                {
                        log_error("bind(): %s", strerror(errno));
                        goto exit_error;
                }
        }

        return 0;

exit_error:
        if(request->s >= 0)
        {
                close(request->s);
                request->s = -1;
        }

        return -1;
}

static void request_connect(struct request *request)
{
        struct addrinfo hints;
        struct addrinfo *res = NULL, *rp = NULL;
        int e;
        char serv[6];
        int ret;

        snprintf(serv, sizeof(serv),
                 "%u", request->host.port);

        memset(&hints, '\0', sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
#if defined(AI_ADDRCONFIG)
	/* some OS (like netbsd) doesn't have AI_ADDRCONFIG flag */
        hints.ai_flags = AI_ADDRCONFIG;
#endif

        e = getaddrinfo(request->host.addr,
                        serv,
                        &hints,
                        &res);
        if(e != 0)
        {
                log_error("getaddrinfo(%s, %u) failed: %s\n",
                          request->host.addr,
                          request->host.port,
                          gai_strerror(e));
                request->state = FSError;
                request->errcode = REQ_ERR_SYSTEM;
	  	echo_status_info("errorOccupy");
                return;
        }

        log_debug("&request:%p, connecting to %s:%u",
                  request,
                  request->host.addr,
                  request->host.port);

        for (rp = res; rp != NULL; rp = rp->ai_next)
        {
                log_debug("&request:%p, try to connect to %s:%u ...",
                          request,
                          inet_ntoa(((struct sockaddr_in*)rp->ai_addr)->sin_addr),
                          ntohs(((struct sockaddr_in*)rp->ai_addr)->sin_port));

                ret = connect(request->s,
                              rp->ai_addr, rp->ai_addrlen);
                if(ret == 0)
                {
                        request->state = FSConnected;
                        break;
                }
                else if(ret < 0)
                {
                        if(errno == EINPROGRESS)
                        {
                                request->state = FSConnecting;
                                request->last_pending_action.tv_sec =
                                        util_getuptime();
                                break;
                        }

                        /* big error */
                        log_notice("connect(%s:%u) failed: %s",
                                   inet_ntoa(((struct sockaddr_in*)rp->ai_addr)->sin_addr),
                                   ntohs(((struct sockaddr_in*)rp->ai_addr)->sin_port),
                                   strerror(errno));
                }
        }

        freeaddrinfo(res);

        if(request->state != FSConnecting && request->state != FSConnected)
        {
                log_error("Unable to connect !");
                request->state = FSError;
                request->errcode = REQ_ERR_CONNECT_FAILED;
        }
}

static void request_process(struct request *request)
{
        int err;
        socklen_t errsize = sizeof(int);

        /* first, process FSConnecting request state */
        if(request->state == FSConnecting)
        {
                log_debug("&request:%p, FSConnecting - check connect status",
                          request);

                if(getsockopt(request->s, SOL_SOCKET,
                              SO_ERROR, &err, &errsize) != 0)
                {
                        log_error("getsockopt(%d, SO_ERROR) failed: %s",
                                  request->s,
                                  strerror(errno));
                        request->state = FSError;
                        request->errcode = REQ_ERR_SYSTEM;
                        return;
                }

                if(err != 0)
                {
                        log_error("connect(): %s", strerror(err));
                        request->state = FSError;
                        request->errcode = REQ_ERR_CONNECT_FAILED;
                        return;
                }

                /* yes, we are connected ! */
                log_debug("&request:%p, connected !", request);
                request->state = FSConnected;
        }

        /* next, process the other states */
        if(request->state == FSConnected
           || request->state == FSSending)
        {
                log_debug("&request:%p, FSConnected or FSSending"
                          " - perform send",
                          request);
		request_process_send(request);
        }
        else if(request->state == FSFinished)
        {
                log_debug("&request:%p, FSFinished - close socket.",
                          request);
		close(request->s);
		request->s = -1;
        }
        else if(request->state == FSWaitingResponse)
        {
                log_debug("&request:%p, FSWaitingResponse"
                          " - perform recv",
                          request);
		request_process_recv(request);
	}
        else
        {
		log_critical("Unknown state");
	}
}

static void request_process_send(struct request *request)
{
        ssize_t i;
        size_t remain = (size_t)(request->buff.data_size
                                 - request->buff.data_ack);

        log_debug("&request:%p, send on %d: %.*s",
                  request,
                  request->s,
                  remain,
                  request->buff.data + request->buff.data_ack);

        i = send(request->s,
                 request->buff.data + request->buff.data_ack,
                 remain,
                 0);
        if(i < 0)
        {
                log_error("send(): %s", strerror(errno));
                request->state = FSError;
                request->errcode = REQ_ERR_SYSTEM;
                return;
        }

        if((size_t)i != remain)
        {
                log_notice("%zi bytes send out of %zu",
                           i, remain);
        }

        log_debug("&request:%p, sent %zi bytes", request, i);

        request->buff.data_ack += (size_t)i;
        if(request->buff.data_ack == request->buff.data_size)
        {
                request->state = FSWaitingResponse;
        }
        else
        {
                request->state = FSSending;
        }

        request->last_pending_action.tv_sec = util_getuptime();

        return;
}

static void request_process_recv(struct request *request)
{
        ssize_t n;

        n = recv(request->s,
                 request->buff.data,
                 sizeof(request->buff.data) - 1,
                 0);
        if(n < 0)
        {
                log_error("Error when reading socket %d: %s",
                          request->s,
                          strerror(errno));
                request->state = FSError;
                request->errcode = REQ_ERR_SYSTEM;
                return;
        }

        /* put the \0 at end */
        request->buff.data[n] = '\0';
        log_debug("&request:%p, recv on %d (%zi bytes): %s",
                  request, request->s, n, request->buff.data);

        request->buff.data_size = (size_t)n;
        request->buff.data_ack = (size_t)n;

        request->state = FSResponseReceived;

        /* call hook func */
        request->ctl.hook_func(request, request->ctl.hook_data);

        request->state = FSFinished;

        return;
}

/*
 * decs API functions
 */
void request_ctl_init(void)
{
        INIT_LIST_HEAD(&request_list);
}

void request_ctl_cleanup(void)
{
        struct request *request = NULL,
                *safe_request = NULL;

        list_for_each_entry_safe(request, safe_request,
                                 &(request_list), list)
        {
                list_del(&(request->list));
                free(request);
        }
}

int request_send(struct request_host *host,
                 struct request_ctl *ctl,
                 struct request_buff *buff,
                 struct request_opt *opt)
{
        struct request *request = NULL;

        request = calloc(1, sizeof(struct request));

        /* fill host structure */
        snprintf(request->host.addr, sizeof(request->host.addr),
                 "%s", host->addr);
        request->host.port = host->port;

        /* fill ctl structure */
        request->ctl.hook_func = ctl->hook_func;
        request->ctl.hook_data = ctl->hook_data;

        /* fill buf */
        snprintf(request->buff.data, sizeof(request->buff.data),
                 "%s", buff->data);
        request->buff.data_size = buff->data_size;
        request->buff.data_ack = 0;

        /* request options */
        if(opt != NULL)
        {
                memcpy(&(request->opt), opt, sizeof(request->opt));
        }

        /* open socket */
        if(request_open_socket(request) != 0)
        {
                log_error("Error opening socket. Abort request.");
                free(request);
                return -1;
        }

        /* all is ok, add to request list */
        request->state = FSCreated;
        list_add(&(request->list), &request_list);

        return 0;
}

void request_ctl_selectfds(fd_set *readset, fd_set *writeset, int *max_fd)
{
        struct request *request = NULL;

        log_debug("--------------------- selectfds ---------------------");

        list_for_each_entry(request, &(request_list), list)
        {
                if(request->state == FSCreated)
                {
                        log_debug("&request:%p, FSCreated - execute connect",
                                  request);

                        request_connect(request);
                }

                if(request->state == FSConnecting
                   || request->state == FSConnected
                   || request->state == FSSending)
                {
                        log_debug("&request:%p, FSConnect(ing|ed)|FSSending"
                                  " - FD_SET(%d, writeset)",
                                  request, request->s);

                        FD_SET(request->s, writeset);
                        *max_fd = MAX(request->s, *max_fd);
                }
                else if(request->state == FSWaitingResponse)
                {
                        log_debug("&request:%p, FSWaitingForResponse"
                                  " - FD_SET(%d, readset)",
                                  request, request->s);

                        FD_SET(request->s, readset);
                        *max_fd = MAX(request->s, *max_fd);
                }
        }
}

void request_ctl_processfds(fd_set *readset, fd_set *writeset)
{
        struct request *request = NULL,
                *safe = NULL;
        time_t uptime = util_getuptime();

        log_debug("--------------------- processfds ---------------------");

        list_for_each_entry_safe(request, safe,
                                 &request_list, list)
        {
                /* process request with fd marked*/
                if(FD_ISSET(request->s, readset)
                   || FD_ISSET(request->s, writeset))
                {
                        log_debug("&request:%p, FD_ISSET(%d) == 1",
                                  request, request->s);

                        request_process(request);
                }

                /* remove finished, error or timeout request */
                if((request->state == FSConnecting
                    || request->state == FSWaitingResponse
                    || request->state == FSSending)
                   && (uptime - request->last_pending_action.tv_sec
                       >= REQUEST_PENDING_ACTION_TIMEOUT))
                {
                        log_debug("Pending request on %s:%d timeout (> %d sec)",
                                   request->host.addr, request->host.port,
                                   REQUEST_PENDING_ACTION_TIMEOUT);

                        /* set appropriated errcode */
                        switch(request->state)
                        {
                        case FSConnecting:
                                request->errcode = REQ_ERR_CONNECT_TIMEOUT;
                                break;

                        case FSWaitingResponse:
                                request->errcode = REQ_ERR_RESPONSE_TIMEOUT;
                                break;

                        case FSSending:
                                request->errcode = REQ_ERR_SENDING_TIMEOUT;
                                break;

                        default:
                                request->errcode = REQ_ERR_UNKNOWN;
                                break;
                        }

                        request->state = FSError;
                }

                if(request->state == FSError
                   || request->state == FSFinished)
                {
                        log_debug("&request:%p, FSFinished|FSError|timeout"
                                  " - remove it", request);

                        /* call hook func */
                        request->ctl.hook_func(request, request->ctl.hook_data);

                        close(request->s);
                        request->s = -1;

                        list_del(&(request->list));
                        free(request);
                }
        }
}

int request_ctl_remove_by_hook_data(const void *hook_data)
{
        struct request *request = NULL,
                *safe = NULL;
        int i = 0;

        list_for_each_entry_safe(request, safe,
                                 &request_list, list)
        {
                if(request->ctl.hook_data == hook_data)
                {
                        ++i;

                        log_debug("&request:%p, remove it",
                                  request);

                        close(request->s);
                        request->s = -1;

                        list_del(&(request->list));
                        free(request);
                }
        }

        return i;
}

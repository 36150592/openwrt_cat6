

#include "tz_socket.h"

int encode_data_to_sock_msg(char *app_name, int msgId, char *data, char *sock_msg)
{
	sprintf(sock_msg, "%s %d %s\r\n", app_name, msgId, data);
	return 1;
}

int decode_sock_msg_to_data(const char *raw_msg, char *app_name, int *msgId, char *data)
{
	int id=0;
	char name[64]={'\0'};
	char info[256]={'\0'};
	char *p1, *p2;

	memset(name, 0, sizeof(name));
	memset(info, 0, sizeof(info));

	if(raw_msg==NULL || raw_msg[0]=='\0')
		return 0;
		
	p1 = (char *)raw_msg;
	p2 = strchr(p1, ' ');
	if(p2==NULL)
		return 0;

	if((unsigned int)p2-(unsigned int)p1 >= sizeof(name))
		return 0;
		
	memcpy(name, p1, p2-p1);

	p1 = p2+1;
	if(p1==NULL ||  p1[0]=='\0')
		return 0;

	p2 = strchr(p1, ' ');
	if(p2==NULL)
		return 0;

	id = atoi(p1);
	if(id <= 0)
		return 0;

	p2++;
	strncpy(info, p2, sizeof(info)-1);
	
	strcpy(app_name, name);
	*msgId = id;
	strcpy(data, info);
	return 1;
}


int send_data_to_app_by_sock_path(char *sock_path, char *data, int data_size)
{
	int sockfd;
	int result;
	int len;

	struct sockaddr_un address;

	 if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		 tz_sock_dbg ("send_data_to %s socket open fail\n", sock_path);
		 return 0;
	 }
	
	 address.sun_family = AF_UNIX;
	 strcpy (address.sun_path, sock_path);
	 len = sizeof (address);
	
	 result = connect (sockfd, (struct sockaddr *)&address, len);
	 if (result == -1) {
		 close (sockfd);
		 tz_sock_dbg ("send_data_to %s connect error\n", sock_path);
		 return 0;
	 }

	 result = write(sockfd, data, data_size);
	 if (result <= 0) {
		 close (sockfd);
		 tz_sock_dbg ("send_data_to %s write error\n", sock_path);
		 return 0;
	}
	
	 close (sockfd);
	
	 return result;
} 


static tSOCK_SRV_PARAMS srv_info;

void *running_socket_client_thread(void *param)
{
	int bytes;
	char buff[1024];
	int client_sockfd = (int)param;

	char app_name[256]={'\0'};
	int msgId=0;
	char msg_info[256]={'\0'};
	
	pthread_detach(pthread_self());

	bytes = read (client_sockfd, buff, sizeof(buff)-1);
	if (bytes == -1) {
		close (client_sockfd);
		tz_sock_dbg ("read client_sockfd=%d fail\n", client_sockfd);
		return NULL ;
	}

	buff[bytes] = '\0';
	
	close (client_sockfd);

	if(decode_sock_msg_to_data(buff, app_name, &msgId, msg_info)!=0)
	{
		tz_sock_dbg("main get data=%s\n",buff);
		tz_sock_dbg("decode msgId=%d info=%s\n", msgId, msg_info);
		srv_info.cb(app_name, msgId, msg_info);
	}
	
	return NULL;
}


void *running_socket_srv_thread(void *param)
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_un server_address;		
	struct sockaddr_un client_address;

	int rt;
	pthread_t pid;

	clear_compile_warning(param);
	unlink (srv_info.sock_path);		
	
	server_sockfd = socket (AF_UNIX, SOCK_STREAM, 0);
	server_address.sun_family = AF_UNIX;
	strcpy (server_address.sun_path, srv_info.sock_path);
	server_len = sizeof (server_address);
	bind (server_sockfd, (struct sockaddr *)&server_address, server_len);
	listen (server_sockfd, 1);
	
	tz_sock_dbg("Server is waiting for client connect...");
	
	client_len = sizeof (client_address);

	while(1){
		fd_set fds;
    	struct timeval tv;

    	FD_ZERO(&fds);
        FD_SET(server_sockfd, &fds);
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        int ret = select(server_sockfd+1, &fds, NULL, NULL, &tv);
        switch (ret) {
	        case -1:
	        	break;
	        case 0:
	    		// timeout
	            break;
	        default:
	            if (FD_ISSET(server_sockfd, &fds)) {
					client_sockfd = accept (server_sockfd, (struct sockaddr *)&server_address, (socklen_t *)&client_len);
					if (client_sockfd == -1) {
						tz_sock_dbg ("accept client_sockfd fail");
						continue ;
					}

					rt = pthread_create(&pid, NULL, running_socket_client_thread, (void *)client_sockfd);
	    			if(rt!=0)
	    			{
	    				tz_sock_dbg("create thread %d fail", client_sockfd);
	    				close(client_sockfd);
	    				continue ;
	    			}
	            }
        }
	}
	
    return NULL;
}


int create_socket_srv_thread(tSOCK_SRV_PARAMS *srv)
{
	int rt;
	pthread_t pid;

	if(srv==NULL)
		return 0;

	if(srv->sock_path==NULL || 
		strlen(srv->sock_path)==0 ||
		srv->cb==NULL)
		return 0;

	rt = pthread_create(&pid, NULL, running_socket_srv_thread, NULL);
	if(rt!=0)
	{
		tz_sock_dbg("create create_socket_srv_thread fail");
		return 0;
	}

	memcpy(&srv_info, srv, sizeof(srv_info));
	return 1;
}



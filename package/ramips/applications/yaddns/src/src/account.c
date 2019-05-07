#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "account.h"
#include "yaddns.h"
#include "request.h"
#include "services.h"
#include "log.h"
#include "util.h"

/* sleep REQ_SLEEPTIME_ON_ERROR when got an request error */
#define REQ_SLEEPTIME_ON_ERROR 5

/* sleep FREEZETIME_ON_TEMP_ERROR when received temporary error from service */
#define FREEZETIME_ON_TEMP_ERROR 1800


/* decs public variables */
struct list_head account_list;

/* defs static functions */
static void account_reqhook_readresponse(struct account *account,
                                         struct request_buff *buff);
static void account_reqhook_error(struct account *account,
                                  unsigned int errcode);
static void account_reqhook(struct request *request, void *data);

/*
 * Decs static functions
 */
static void account_reqhook_readresponse(struct account *account,
                                         struct request_buff *buff)
{
        int ret;
        struct rc_report report = {
                .code = up_unknown_error,
                .proprio_return = "unknown",
                .proprio_return_info = "Unknown return",
        };

        ret = account->def->read_resp(buff,
                                      &report);
        if(ret != 0)
        {
                log_error("Service %s read failed (critical error)",
                          account->def->name);
                account->locked = 1;
                account->status = ASError;
                return;
        }

        log_debug("Service %s (account '%s') return=%s (%s), code=%d",
                  account->def->name,
                  cfgstr_get(&(account->cfg->name)),
                  report.proprio_return,
                  report.proprio_return_info,
                  report.code);
                
        if(report.code == up_success)
        {
                log_info("Update success for account '%s'",
                         cfgstr_get(&(account->cfg->name)));
				echo_status_info("okConnected");
                account->status = ASOk;
                account->updated = 1;
                account->last_update.tv_sec = util_getuptime();
        }
        else
        {
                log_notice("Update failed for account '%s' (%s, %s)",
                           cfgstr_get(&(account->cfg->name)),
                           report.proprio_return,
                           report.proprio_return_info);

                if ( strncmp(report.proprio_return,"badauth",sizeof("badauth") ) == 0 )
                {
                	echo_status_info("errorAuthFailed");
                }
                else if ( strncmp(report.proprio_return,"nohost",sizeof("nohost") ) == 0 )
                {
                	echo_status_info("errorDomainListFailed");
                }
                else
                {
                	echo_status_info("errorOccupy");
                }
                account->status = ASError;

                if(report.code == up_server_error
                   || report.code == up_unknown_error)
                {
                        log_notice("Freeze account '%s' for %d sec.",
                                   cfgstr_get(&(account->cfg->name)),
                                   FREEZETIME_ON_TEMP_ERROR);

                        account->freezed = 1;
                        account->freeze_time.tv_sec = util_getuptime();
                        account->freeze_interval.tv_sec =
                                FREEZETIME_ON_TEMP_ERROR;
                }
                else
                {
                        log_notice("Lock account '%s'",
                                   cfgstr_get(&(account->cfg->name)));
                        account->locked = 1;
                }
        }
}

static void account_reqhook_error(struct account *account,
                                  unsigned int errcode)
{
        account->status = ASError;
        log_error("account '%s' update failed (%s). Retry in %d seconds.",
                  cfgstr_get(&(account->cfg->name)),
                  strreqerr(errcode),
                  REQ_SLEEPTIME_ON_ERROR);
	echo_status_info("errorOccupy");
        account->freezed = 1;
        account->freeze_time.tv_sec = util_getuptime();
        account->freeze_interval.tv_sec = REQ_SLEEPTIME_ON_ERROR;
}

static void account_reqhook(struct request *request, void *data)
{
        struct account *account = data;

        if(request->state == FSError)
        {
                account_reqhook_error(account, request->errcode);
        }
        else if(request->state == FSResponseReceived)
        {
                account_reqhook_readresponse(account, &(request->buff));
        }
}

void account_ctl_init(void)
{
        INIT_LIST_HEAD(&account_list);
}

void account_ctl_cleanup(void)
{
        struct account *account = NULL,
                *safe_act = NULL;

        list_for_each_entry_safe(account, safe_act,
                                 &(account_list), list)
        {
                list_del(&(account->list));
                free(account);
        }
}

/*
 * ctl manage of account:
 * - unfreeze account if the freezetime is over;
 * - force reupdate for account which have done their update 28 days ago:
 *    otherwise dyndns server don't know we are still alive...;
 * - if get wan ip addr, launch update procedure for accounts not updated;
 */
void account_ctl_manage(const struct cfg *cfg)
{
        struct request_host req_host;
        struct request_ctl req_ctl = {
                .hook_func = account_reqhook,
        };
        struct request_buff req_buff;
        struct request_opt req_opt = {
                .mask = 0,
        };
        char buf_wanip[32];
        struct account *account = NULL;
        time_t uptime = util_getuptime();

        /* transform wan ip raw in ascii char */
        if(!inet_ntop(AF_INET, &wanip, buf_wanip, sizeof(buf_wanip)))
        {
                log_error("inet_ntop(): %s", strerror(errno));
                return;
        }

        /* start update processus for service which need to update */
        list_for_each_entry(account,
                            &(account_list), list)
        {
                /* unfreeze account ? */
                if(account->freezed)
                {
                        if(uptime - account->freeze_time.tv_sec
                           >= account->freeze_interval.tv_sec)
                        {
                                /* unfreeze ! */
                                account->freezed = 0;
                        }
                }

                if(account->locked || account->freezed)
                {
                        /* no deal with locked or freezed account
                         * even if needs to be updated
                         */
                        continue;
                }

                if(account->updated
                   && uptime - account->last_update.tv_sec >= 2419200)
                {
                        /*
                         * 28 days after last update, we need to send an
                         * updatepkt otherwise dyndns server desactive
                         * the account because he don't know we are still alive
                         */
                        log_notice("re-update service after 28 beautiful days");
                        account->updated = 0;
                }

                if(have_wanip
                   && !account->updated
                   && account->status != ASWorking)
                {
                        log_notice("Account '%s' service '%s'"
                                   " need to be updated !",
                                   cfgstr_get(&(account->cfg->name)),
                                   cfgstr_get(&(account->cfg->service)));

                        /* req_host structure */
                        snprintf(req_host.addr, sizeof(req_host.addr),
                                 "%s", account->def->ipserv);
                        req_host.port = account->def->portserv;

                        /* req_ctl structure */
                        req_ctl.hook_data = account;

                        /* req_buff structure, tell to service to fill it */
                        memset(&req_buff, 0, sizeof(req_buff));

                        if(account->def->make_query(account->cfg,
                                                    buf_wanip,
                                                    &req_buff) != 0)
                        {
                                account->status = ASError;
                                continue;
                        }

                        /* req opt */
                        if(cfg->wan_cnt_type == wan_cnt_direct)
                        {
                                req_opt.mask = REQ_OPT_BIND_ADDR;
                                req_opt.bind_addr = wanip;
                        }

                        /* send request */
                        if(request_send(&req_host, &req_ctl,
                                        &req_buff, &req_opt) != 0)
                        {
                                account->status = ASError;
                                continue;
                        }

                        /* all is ok */
                        account->status = ASWorking;
                }
        }
}

void account_ctl_needupdate(void)
{
        struct account *account = NULL;

        list_for_each_entry(account,
                            &(account_list), list)
        {
                account->updated = 0;
        }
}

void account_ctl_unfreeze_all(void)
{
        struct account *account = NULL;

        list_for_each_entry(account,
                            &(account_list), list)
        {
                account->freezed = 0;
        }
}

int account_ctl_mapcfg(struct cfg *cfg)
{
        struct service *service = NULL;
        struct cfg_account *accountcfg = NULL;
        struct account *account = NULL,
                *safe = NULL;
        int ismapped = 0;
        int ret = 0;

        list_for_each_entry(accountcfg,
                            &(cfg->account_list), list)
        {
                ismapped = 0;

                list_for_each_entry(service,
                                    &(service_list), list)
                {
                        if(strcmp(service->name,
                                  cfgstr_get(&(accountcfg->service))) == 0)
                        {
                                account = calloc(1,
                                                 sizeof(struct account));
                                account->def = service;
                                account->cfg = accountcfg;

                                list_add(&(account->list),
                                         &(account_list));

                                ismapped = 1;
                                break;
                        }
                }

                if(!ismapped)
                {
                        log_error("No service named '%s' available !",
                                  cfgstr_get(&(accountcfg->service)));

                        list_for_each_entry_safe(account, safe,
                                                 &(account_list), list)
                        {
                                list_del(&(account->list));
                                free(account);
                        }

                        ret = -1;
                        break;
                }
        }

        return ret;
}

int account_ctl_mapnewcfg(const struct cfg *newcfg)
{
        int ret = 0, found = 0;

        struct list_head entry_tomap_list;
        struct {
                struct cfg_account *newcfg;
                struct service *service;
                struct list_head list;
        } *entry_tomap = NULL,
                  *entry_tomap_safe = NULL;

        struct cfg_account *new_actcfg = NULL;
        struct account *accountctl = NULL,
                *accountctl_safe = NULL;
        struct service *service = NULL;

        INIT_LIST_HEAD(&entry_tomap_list);

        /* - make a list with all account cfg from newcfg (because
         *   we need to know which account cfg haven't account
         *   struct already defined).
         * - check service exist for all account cfg.
         */
        list_for_each_entry(new_actcfg, &(newcfg->account_list), list)
        {
                /* check the service exist */
                found = 0;
                list_for_each_entry(service,
                                    &(service_list), list)
                {
                        if(strcmp(service->name,
                                  cfgstr_get(&(new_actcfg->service))) == 0)
                        {
                                found = 1;

                                /* create a new entry and add to the list */
                                entry_tomap = alloca(sizeof(*entry_tomap));
                                entry_tomap->newcfg = new_actcfg;
                                entry_tomap->service = service;

                                list_add(&(entry_tomap->list),
                                         &entry_tomap_list);
                        }
                }

                if(!found)
                {
                        log_error("No service named '%s' available !"
                                  " Abort the new config file.",
                                  cfgstr_get(&(new_actcfg->service)));
                        ret = -1;
                        goto out;
                }
        }

        /* for each account already registered, check is defined in the
         * new config. If not, remove account reference. Otherwise, compare
         * the new config with old one. If account cfg is different,
         * reset the account (to try a new update).
         * And finally, link the new account config to the account.
         */
        list_for_each_entry_safe(accountctl, accountctl_safe,
                                 &(account_list), list)
        {
                found = 0;
                list_for_each_entry_safe(entry_tomap, entry_tomap_safe,
                                         &(entry_tomap_list), list)
                {
                        if(strcmp(cfgstr_get(&(accountctl->cfg->name)),
                                  cfgstr_get(&(entry_tomap->newcfg->name))) == 0)
                        {
                                /* found reference in new cfg */
                                found = 1;

                                /* view it cfg has changed */
                                if(strcmp(cfgstr_get(&(entry_tomap->newcfg->service)),
                                          cfgstr_get(&(accountctl->cfg->service))) != 0
                                   || strcmp(cfgstr_get(&(entry_tomap->newcfg->username)),
                                             cfgstr_get(&(accountctl->cfg->username))) != 0
                                   || strcmp(cfgstr_get(&(entry_tomap->newcfg->passwd)),
                                             cfgstr_get(&(accountctl->cfg->passwd))) != 0
                                   || strcmp(cfgstr_get(&(entry_tomap->newcfg->hostname)),
                                             cfgstr_get(&(accountctl->cfg->hostname))) != 0)
                                {
                                        /* cfg has changed */
                                        log_debug("account cfg for '%s'"
                                                  " has changed",
                                                  cfgstr_get(&(entry_tomap->newcfg->name)));

                                        accountctl->updated = 0;
                                        accountctl->locked = 0;
                                        accountctl->freezed = 0;
                                }

                                /* link the new cfg to account ctl struct */
                                accountctl->cfg = entry_tomap->newcfg;
                                accountctl->def = entry_tomap->service;

                                /* the entry was mapped */
                                list_del(&(entry_tomap->list));
                        }
                }

                if(!found)
                {
                        /* remove reference of account */
                        log_debug("Remove unused account ctl '%s'",
                                  cfgstr_get(&(accountctl->cfg->name)));

                        /* This old reference must not be used anymore,
                         * this is why we remove all request which can
                         * reference accountctl.
                         */
                        request_ctl_remove_by_hook_data(accountctl);

                        list_del(&(accountctl->list));
                        free(accountctl);
                }
        }

        /* all entries not already mapped need a new account ctl struct */
        list_for_each_entry(entry_tomap, &(entry_tomap_list), list)
        {
                log_debug("New account '%s'",
                          cfgstr_get(&(entry_tomap->newcfg->name)));

                accountctl = calloc(1, sizeof(struct account));
                accountctl->cfg = entry_tomap->newcfg;
                accountctl->def = entry_tomap->service;

                list_add(&(accountctl->list), &(account_list));
        }

out:
        return ret;
}

struct account *account_ctl_get(const char *accountname)
{
        struct account *account = NULL;

        list_for_each_entry(account,
                            &(account_list), list)
        {
                if(strcmp(cfgstr_get(&(account->cfg->name)), accountname) == 0)
                {
                        return account;
                }
        }

        return NULL;
}

/************************************************************************
 *                                                                      *
 * Netcwmp/Opencwmp Project                                             *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                            *
 *                                                                      *
 * This program is free software; you can redistribute it and/or        *
 * modify it under the terms of the GNU General Public License          *
 * as published by the Free Software Foundation; either version 2       *
 * of the License, or (at your option) any later version.               *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU Lesser General Public     *
 * License along with this library; if not, write to the                *
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,          *
 * Boston, MA  02111-1307 USA                                           *
 *                                                                      *
 * Copyright 2013-2014  Mr.x(Mr.x) <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#include "cwmpd.h"
#include <unistd.h>
#include <signal.h>

#define CWMP_VALUE_UNSET -1



int              cwmp_argc;
char           **cwmp_argv;


static pool_t * cwmp_global_pool;



void cwmp_daemon()
{
    //daemon(0, 1);
}




void cwmp_getopt(int argc, char **argv)
{
    
}


static int cwmp_save_argv( int argc, char *const *argv)
{
    cwmp_argv = (char **) argv;
    cwmp_argc = argc;

    return 0;
}


int cwmp_set_var(cwmp_t * cwmp)
{
    FUNCTION_TRACE();

    cwmp_event_init(cwmp);

    return CWMP_OK;
}




#ifdef USE_CWMP_OPENSSL
void cwmp_init_ssl(cwmp_t * cwmp)
{
    char * cafile = CWMP_CA_FILENAME;
    char * capasswd = CWMP_CA_PASSWORD;   
    cwmp->ssl_ctx = openssl_initialize_ctx(cafile, capasswd);
}
#endif

int read_file_data(const char *fileName, char *buffer, int buf_size)
{
	int n;
	FILE *fp;

	if(buffer==NULL || buf_size<=0)
		return 0;

	buffer[0]='\0';

	fp = fopen((char *)fileName, "rb");
	if(fp == NULL)
		return 0;
	else{
		n = fread(buffer, 1, buf_size-1, fp);
		fclose(fp);
	}

	buffer[n]='\0';

	return n;
}

int get_cwmp_log_level(void)
{
	int n;
	char buf[64];

	n = read_file_data("/tmp/cwmp_log_level", buf, sizeof(buf)-1);
	if(n <= 0)
		return CWMP_LOG_NOTICE;

	return atoi(buf);
}

static cwmp_t *main_cwmp=NULL;

cwmp_t *get_root_cwmp(void)
{
	return main_cwmp;
}

void *allow_acs_ping_handler(void *param)
{
	int rt;
	int len;
	
	char ipv4[64];
	char ipv6[64];

	char buff[256];
	char host[100];

	char *p1, *p2;
	
	pthread_detach(pthread_self());

    p1 = strstr(main_cwmp->acs_url, "//");
    if(p1==NULL)
    {
		cwmp_log_debug("get_acs_dns_handler get host no // \n");
		return NULL;
    }
    else
    	p1 += 2;

    p2 = strchr(p1, '/');
    if(p2==NULL)
    {
		cwmp_log_debug("get_acs_dns_handler get host next / \n");
		return NULL;
    }

    len = p2-p1;
    if(len>=sizeof(host))
    {
		cwmp_log_debug("get_acs_dns_handler host len=%d too loog \n", len);
		return NULL;
    }

    memcpy(host, p1, len);
    host[len]='\0';

    p1=strchr(host, ':');	// skip port when host like rm.wiz.co.zm:8080 
    if(p1!=NULL)
    	p1[0]='\0';

	cwmp_log_debug("get_acs_dns_handler host=%s\n", host);
    rt = util_resolv_domain_name(host, ipv4, ipv6, &len);
	if(rt==FALSE || strlen(ipv4)==0)
	{
		cwmp_log_debug("get_acs_dns_handler host=%s rt=%d ipv4=%s failed\n", 
						host, rt, ipv4);
		return NULL;
	}

	sprintf(buff, "iptables -t nat -D PREROUTING -i eth0.2 -p icmp -s %s -j ACCEPT", ipv4);
	system(buff);
	cwmp_log_debug("allow_acs_ping_handler remove old rule: %s\n", buff);

	sprintf(buff, "iptables -t nat -I PREROUTING 1 -i eth0.2 -p icmp -s %s -j ACCEPT", ipv4);
	system(buff);
	cwmp_log_debug("allow_acs_ping_handler add new rule: %s\n", buff);

	return NULL;
}


void allow_acs_ping_thread(void)
{
	int rt;
	pthread_t pid;

	rt = pthread_create(&pid, NULL, allow_acs_ping_handler, NULL);
	if(rt!=0)
		cwmp_log_debug("allow_acs_ping_thread failed\n");
}
extern void cwmp_model_load(cwmp_t * cwmp, const char * xmlfile);
extern int clear_and_cwmp_event_set_value(cwmp_t *cwmp,  int event,   int value, const char * cmd_key, int fault_code, time_t start, time_t end);
extern void cwmp_process_start_master(cwmp_t * cwmp);


int main(int argc, char **argv)
{
    cwmp_pid_t pid;
    cwmp_t * cwmp;

    //int syslog_enable = 0;
    int cwmp_enable = 0;
	//int interval = 0;

	char network_dev_name[ 64 ];

	int is_ipv4_addr_exist=FALSE;
	int is_ipv6_addr_exist=FALSE;

#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    
    signal(SIGPIPE, SIG_IGN);
    pid = getpid();

	printf("cwmpd built at %s %s\r\n", __DATE__, __TIME__);

	// CHECK WAN IP is ok
	//strcpy( network_dev_name, util_get_wan_interface_name() );
	//strcpy( network_dev_name, "eth0.2" );

	while( TRUE )
	{
 //               read_memory("getinfacestatus -i wan -p l3_device",network_dev_name,sizeof(network_dev_name));
                read_memory("route -n|awk  '{print $1,$8}'|grep 0.0.0.0|awk  '{print $2}'|head -1",network_dev_name,sizeof(network_dev_name));
                util_strip_traling_spaces(network_dev_name);
				if(strlen(network_dev_name)==0)
				{
				   strcpy(network_dev_name,"eth0.2");
				}
				printf("jiangyibo network_dev_name %s\n",network_dev_name);
		while( cmd_netdev_exist( network_dev_name ) )
		{
			//check if ipv4 addr exist
			is_ipv4_addr_exist=cmd_ip_exist( network_dev_name );
			//check if ipv6 addr exist
			is_ipv6_addr_exist=cmd_ipv6_exist( network_dev_name );

			//ipv4 or ipv6 addr exist
			if( is_ipv4_addr_exist || is_ipv6_addr_exist )
			{
				sleep( 5 );
				goto NETWORK_IS_OK;
			}

			sleep( 1 );
			printf("cwmpd is checking ip status ...\r\n");
		}
		
		sleep( 5 );
		printf("cwmpd is checking networking status ...\r\n");
	}

NETWORK_IS_OK:

    cwmp_log_init("/tmp/cwmpd.log", CWMP_LOG_DEBUG);
    //cwmp_log_init("/tmp/cwmpd.log", CWMP_LOG_DEBUG);
    //cwmp_log_init(NULL, CWMP_LOG_ERROR);
    //cwmp_log_init(NULL, CWMP_LOG_DEBUG);
    //cwmp_log_init(NULL, get_cwmp_log_level());
	
    cwmp_global_pool = pool_create(POOL_DEFAULT_SIZE);
    cwmp = pool_palloc(cwmp_global_pool, sizeof(cwmp_t));
	main_cwmp = cwmp;
	
	cwmp_bzero(cwmp, sizeof(cwmp_t));
	cwmp->new_request = CWMP_TRUE;
	pool_t * _pool = pool_create(POOL_DEFAULT_SIZE);
	cwmp->pool = _pool;
	cwmp->queue = queue_create(_pool);
	
    cwmp_conf_open("/etc/cwmp.conf");
    
    //cwmp_enable=cwmp_conf_get_int("cwmp:enable");
    //if(!cwmp_enable)
    //{
        //exit(-1);    
    //}

	//interval=cwmp_conf_get_int("cwmp:interval");
	//printf("interval = %d\n", interval);

    cwmp_getopt(argc, argv);
    
    //cwmp_init_db();    
    cwmp_daemon();
    
    cwmp_conf_init(cwmp);
    cwmp_set_var(cwmp);

#ifdef USE_CWMP_OPENSSL
    cwmp_init_ssl(cwmp);
#endif

#if 0
	allow_acs_ping_thread();
#ifdef  PERIODREADDEVICE
	refresh_device_info_porcess_thread_init();
#endif 
#endif

    //cwmp_model_load(cwmp, "/etc/device.xml");
    cwmp_model_load(cwmp, "/etc/device.xml");

	if(access(TR069_UPDATE_REBOOT_FLAG, F_OK) < 0 &&
		access(TR069_REBOOT_FLAG, F_OK) < 0){
		cwmp_log_debug("Not tr069_update_then_reboot and tr069_reboot");
		
	    if (access(TR069_BOOTSTRAP_FLAG, F_OK) < 0)
	    {
	    	cwmp_log_debug("TR069 INFORM_BOOTSTRAP.");
	        //clear_and_cwmp_event_set_value(cwmp, INFORM_BOOTSTRAP, 1, NULL, 0, 0, 0);
	    }
	    else if (access(TR069_REG_PASSWD, F_OK) >= 0)
	    {
	    	cwmp_log_debug("TR069 INFORM_XCMCC_BIND.");
	        clear_and_cwmp_event_set_value(cwmp, INFORM_XCMCC_BIND, 1, NULL, 0, 0, 0);
	    }
	    else
	    {
	    	cwmp_log_debug("TR069 INFORM_BOOT.");
	        //clear_and_cwmp_event_set_value(cwmp, INFORM_BOOT, 1, NULL, 0, 0, 0);
	    }
    }
    else {
	    cwmp_log_debug("Is tr069_update_then_reboot or tr069_reboot");
    	if (access(TR069_REBOOT_FLAG, F_OK) >= 0) {
    		remove(TR069_REBOOT_FLAG);
    	}
    	//cwmp_log_debug("Got tr069_update_then_reboot");
		//
    }
	
    cwmp_process_start_master(cwmp);

    return 0;
}




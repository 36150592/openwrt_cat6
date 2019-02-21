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

#include "cwmp_module.h"
#include "cmdlib.h"
#include <cwmp/cfg.h>

extern void load_params(void);
extern InfoStruct system_version_info;
extern WanInfo wan_info;

/*
void cwmp_conf_init(cwmp_t * cwmp)
{
	MacAddr mac;
    pool_t * pool;
    FUNCTION_TRACE();
	char value[128];

    pool = cwmp->pool;

	load_params();
	
	memset(value, 0, sizeof(value));
	get_single_config_attr("TZ_TR069_CPE_PORT", value);
	if(strlen(value) > 0 && !is_digit(value))
    	cwmp->httpd_port = atoi(value);
	else
		cwmp->httpd_port = 5400;
    
	memset(value, 0, sizeof(value));
    get_single_config_attr("TZ_TR069_ACS_AUTH_ENABLED", value);
	cwmp->acs_auth = atoi(value); 

	memset(value, 0, sizeof(value));
    get_single_config_attr("TZ_TR069_CPE_AUTH_ENABLED", value);
	cwmp->cpe_auth = atoi(value); 

    if(cwmp->acs_auth)
    {
		memset(value, 0, sizeof(value));
		get_single_config_attr("TZ_TR069_USERNAME", value);
		cwmp->acs_user = pool_palloc(pool, strlen(value)+1);
		strcpy(cwmp->acs_user, value);

		memset(value, 0, sizeof(value));
		get_single_config_attr("TZ_TR069_PASSWD", value);
        cwmp->acs_pwd = pool_palloc(pool, strlen(value)+1);
		strcpy(cwmp->acs_pwd, value);
    }

	if(cwmp->cpe_auth)
	{
		memset(value, 0, sizeof(value));
		get_single_config_attr("TZ_TR069_LINK_USERNAME", value);
		cwmp->cpe_user = pool_palloc(pool, strlen(value)+1);
		strcpy(cwmp->cpe_user, value);

		memset(value, 0, sizeof(value));
		get_single_config_attr("TZ_TR069_LINK_PASSWD", value);
        cwmp->cpe_pwd = pool_palloc(pool, strlen(value)+1);
		strcpy(cwmp->cpe_pwd, value);
	}

	memset(value, 0, sizeof(value));
	strcpy(value,"http://113.98.195.202:7547/");
//	get_single_config_attr("TZ_TR069_URL", value);
    cwmp->acs_url = pool_palloc(pool, strlen(value)+1);
	strcpy(cwmp->acs_url, value);
	
	cwmp->cpe_mf = pool_palloc(pool, strlen(CWMP_CPE_MF)+1);
	strcpy(cwmp->cpe_mf, CWMP_CPE_MF);

	cwmp->cpe_oui = pool_palloc(pool, 6+1);
	if (!cmd_netdev_get_mac_addr("ath0", &mac))
	{
		sprintf(cwmp->cpe_oui, "%02X%02X%02X", mac.value[0], mac.value[1], mac.value[2]);
	}
	else
	{
		strcpy(cwmp->cpe_oui, "D8D866");
	}

	cwmp->cpe_sn = system_version_info.imei;

	get_single_config_attr("TZ_DEVICE_TYPE", value);
    if(strlen(value) == 0)
    	cwmp->cpe_name = system_version_info.device;
	else
	{
		cwmp->cpe_name = pool_palloc(pool, strlen(value)+1);
		strcpy(cwmp->cpe_name, value);
	}

    cwmp->cpe_pc = cwmp->cpe_name;

    //cwmp->cpe_cmcc_pwd   =   cwmp_conf_pool_get(pool, "cwmp:cmcc_password"); 
    cwmp->cpe_httpd_ip = wan_info.ExternalIPAddress; 

    cwmp_log_debug("url:%s\nmf:%s\noui:%s\nsn:%s\nname:%s\npc:%s\nhttpd ip=%s port:%d\n",
		cwmp->acs_url,
		cwmp->cpe_mf,
		cwmp->cpe_oui,
		cwmp->cpe_sn,
		cwmp->cpe_name,
		cwmp->cpe_pc,
		cwmp->cpe_httpd_ip,
        cwmp->httpd_port);

	cwmp->event_filename = pool_palloc(pool, strlen(CWMP_EVENT_FILENAME)+1);
	strcpy(cwmp->event_filename, CWMP_EVENT_FILENAME);
	//cwmp_log_debug("event_filename=%s", cwmp->event_filename);
}
*/
void cwmp_conf_init(cwmp_t * cwmp)
{
    pool_t * pool;
    FUNCTION_TRACE();

    pool = cwmp->pool;
    cwmp->httpd_port =  cwmp_conf_get_int("cwmpd:httpd_port");

    cwmp->acs_auth  =   cwmp_conf_get_int("cwmp:acs_auth");
    cwmp->cpe_auth  =   cwmp_conf_get_int("cwmp:cpe_auth");

    if(cwmp->acs_auth)
    {
        cwmp->acs_user = cwmp_conf_pool_get(pool, "cwmp:acs_username");
        cwmp->acs_pwd = cwmp_conf_pool_get(pool, "cwmp:acs_password");
    }

	if(cwmp->cpe_auth)
	{
	    cwmp->cpe_user = cwmp_conf_pool_get(pool, "cwmp:cpe_username");
    	cwmp->cpe_pwd = cwmp_conf_pool_get(pool, "cwmp:cpe_password");
	}

    cwmp->acs_url   =   cwmp_conf_pool_get(pool, "cwmp:acs_url");

    cwmp->cpe_mf    =   cwmp_conf_pool_get(pool, "cwmp:cpe_manufacture"); 

    cwmp->cpe_oui   =   cwmp_conf_pool_get(pool, "cwmp:cpe_oui"); 

    cwmp->cpe_sn    =   cwmp_conf_pool_get(pool, "cwmp:cpe_sn");

    cwmp->cpe_name  =   cwmp_conf_pool_get(pool, "cwmp:cpe_name");

    cwmp->cpe_pc    =   cwmp_conf_pool_get(pool, "cwmp:cpe_pc");

    cwmp->cpe_httpd_ip  =   cwmp_conf_pool_get(pool, "cwmpd:httpd_ip"); 

    cwmp_log_debug("url:%s\nmf:%s\noui:%s\nsn:%s\nname:%s\npc:%s\nhttpd ip=%s port:%d\n",
                    cwmp->acs_url, cwmp->cpe_mf, cwmp->cpe_oui, 
                    cwmp->cpe_sn, cwmp->cpe_name, cwmp->cpe_pc,
					cwmp->cpe_httpd_ip,
                    cwmp->httpd_port);


    cwmp->event_filename = cwmp_conf_pool_get(pool, "cwmp:event_filename");
	cwmp_log_debug("event_filename=%s", cwmp->event_filename);

    read_wan_info();
    cmd_netdev_flowstat_ex(wan_info.Name,rx_bytes_counter,tx_bytes_counter,rx_packages_counter,tx_packages_counter);
}


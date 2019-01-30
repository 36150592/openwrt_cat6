#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <sys/time.h>

#include "common.h"
#include "include.h"
#include "logger.h"
#include "bm906.h"
#include "bm916.h"
#include "bm806.h"
#include "ec20.h"
#include "serial.h"
#include "sim7000.h"
#include "a9500.h"
#include "lt10.h"

struct Queue at_recv_buff;  //define at recv queue

GD global_dialtool;
GSI global_system_info;
GIP global_init_parms;
GDV global_dial_vars;
CPS configs;
CSA scan_att_configs;
int sendAtFlag = 0;
int global_sleep_interval_long=0;

const MDI* modules_info_summary[]=
{
	&bm806_moduleinfo,
	&bm906_moduleinfo,
	&bm916_moduleinfo,
	&ec20_moduleinfo,
	&sim7000c_moduleinfo,
	&ar9500_moduleinfo,
	&lt10_moduleinfo,
	NULL
};
extern MP* project_info_summary[];
extern int project_info_len(void);


void init_parms(PARMS *cfg,int len)
{
	strcpy(configs.enable_pin.Name,"TZ_CONFIG_ENABLED_PIN");
	strcpy(configs.pin.Name,"TZ_CONFIG_PIN_CODE");
	strcpy(configs.puk.Name,"TZ_CONFIG_PUK_CODE");
	strcpy(configs.apn.Name,"TZ_CONFIG_APN_NAME");
	strcpy(configs.network_mode_web.Name,"TZ_LTE_MODULE_MODE");
	strcpy(configs.ppp_auth_type.Name,"TZ_PPP_AUTH_TYPE");
	strcpy(configs.ppp_username.Name,"TZ_PPP_USERNAME");
	strcpy(configs.ppp_password.Name,"TZ_PPP_PASSWORD");
	strcpy(configs.lte_band_choose.Name,"TZ_BAND_PREF");
	strcpy(configs.plmn_mode.Name,"TZ_PLMN_MODE");
	strcpy(configs.plmn.Name,"TZ_PLMN_NUMBER");
	strcpy(configs.plmn_act.Name,"TZ_PLMN_ACT");
	strcpy(configs.ipstack.Name,"TZ_IP_STACK_MODE");
	strcpy(configs.network_card_mtu.Name,"TZ_MTU_USB0");
	//check scan att sim card
	strcpy(scan_att_configs.scan_att_check_enable.Name,"TZ_SCAN_ATT_CHECK_ENABLE");
	strcpy(scan_att_configs.scan_att_mcc_mnc.Name,"TZ_SCAN_ATT_MCC_MNC");
	strcpy(scan_att_configs.scan_att_apn_name.Name,"TZ_SCAN_ATT_APN_NAME");

	get_config_specified(cfg,len,configs.enable_pin.Name,configs.enable_pin.Value);
	get_config_specified(cfg,len,configs.pin.Name,configs.pin.Value);
	get_config_specified(cfg,len,configs.puk.Name,configs.puk.Value);

	get_config_specified(cfg,len,configs.apn.Name,configs.apn.Value);
	//if(configs.apn.Value[0] =='\0')
	//{
		//strcpy(configs.apn.Value,"0");
	//}
	get_config_specified(cfg,len,configs.network_mode_web.Name,configs.network_mode_web.Value);
	if(configs.network_mode_web.Value[0] =='\0')
	{
		strcpy(configs.network_mode_web.Value,"0");
	}
	get_config_specified(cfg,len,configs.ppp_auth_type.Name,configs.ppp_auth_type.Value);
	if(configs.ppp_auth_type.Value[0] =='\0')
	{
		strcpy(configs.ppp_auth_type.Value,"0");
	}
	get_config_specified(cfg,len,configs.ppp_username.Name,configs.ppp_username.Value);
	if(configs.ppp_username.Value[0] =='\0')
	{
		strcpy(configs.ppp_username.Value,"0");
	}
	get_config_specified(cfg,len,configs.ppp_password.Name,configs.ppp_password.Value);
	if(configs.ppp_password.Value[0] =='\0')
	{
		strcpy(configs.ppp_password.Value,"0");
	}
	get_config_specified(cfg,len,configs.lte_band_choose.Name,configs.lte_band_choose.Value);

	get_config_specified(cfg,len,configs.plmn_mode.Name,configs.plmn_mode.Value);
	if(configs.plmn_mode.Value[0] =='\0')
	{
		strcpy(configs.plmn_mode.Value,"0");
	}
	get_config_specified(cfg,len,configs.plmn.Name,configs.plmn.Value);
	if(configs.plmn.Value[0] =='\0')
	{
		strcpy(configs.plmn.Value,"0");
	}
	get_config_specified(cfg,len,configs.plmn_act.Name,configs.plmn_act.Value);
	if(configs.plmn_act.Value[0] =='\0')
	{
		strcpy(configs.plmn_act.Value,"0");
	}
	get_config_specified(cfg,len,configs.ipstack.Name,configs.ipstack.Value);
	if(configs.ipstack.Value[0] =='\0')
	{
		strcpy(configs.ipstack.Value,"IP");
	}
	get_config_specified(cfg,len,configs.network_card_mtu.Name,configs.network_card_mtu.Value);
	if(configs.network_card_mtu.Value[0] =='\0')
	{
		strcpy(configs.network_card_mtu.Value,"1500");
	}

	get_config_specified(cfg,len,scan_att_configs.scan_att_check_enable.Name,scan_att_configs.scan_att_check_enable.Value);
	if(strcmp(scan_att_configs.scan_att_check_enable.Value, "yes") == 0)
	{
		get_config_specified(cfg,len,scan_att_configs.scan_att_mcc_mnc.Name,scan_att_configs.scan_att_mcc_mnc.Value);
		get_config_specified(cfg,len,scan_att_configs.scan_att_apn_name.Name,scan_att_configs.scan_att_apn_name.Value);
	}

	char value[64] = {0};
	int *p = NULL;
	get_config_specified(cfg,len,"TZ_CONFIG_SIGNAL_RSRP_LVL", value);
	p = global_init_parms.signal_rssi_lvl;
	p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0; p[4] = 0;
	p = global_init_parms.signal_rsrp_lvl;
	p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0; p[4] = 0;
	if(strlen(value) > 0 )
	{
		p = global_init_parms.signal_rsrp_lvl;
		sscanf(value, "%d:%d:%d:%d:%d\n", p, p+1,p+2,p+3,p+4);
		if(!(p[0] < p[1] && p[1] < p[2] && p[2] < p[3] && p[3] < p[4]))
		{
			p[0] = -115; p[1] = -110; p[2] = -105; p[3] = -97; p[4] = -84;
		}
	}
	else
	{
		get_config_specified(cfg,len,"TZ_CONFIG_SIGNAL_RSSI_LVL", value);
		p = global_init_parms.signal_rssi_lvl;
		p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0; p[4] = 0;
		if(strlen(value) > 0 )
		{
			sscanf(value, "%d:%d:%d:%d:%d\n", p, p+1,p+2,p+3,p+4);
			if(!(p[0] < p[1] && p[1] < p[2] && p[2] < p[3] && p[3] < p[4]))
			{
				p[0] = 58; p[1] = 78; p[2] = 98; p[3] = 108; p[4] = 118;
			}
		}
		else
		{
			p = global_init_parms.signal_rsrp_lvl;
			p[0] = -115; p[1] = -110; p[2] = -105; p[3] = -97; p[4] = -84;
		}
	}

	
	strcpy(global_init_parms.enable_pin,configs.enable_pin.Value);
	strcpy(global_init_parms.apn,configs.apn.Value);
	strcpy(global_init_parms.puk,configs.puk.Value);
	strcpy(global_init_parms.pin,configs.pin.Value);
	strcpy(global_init_parms.puk,configs.puk.Value);
	strcpy(global_init_parms.lte_band_choose,configs.lte_band_choose.Value);
	global_init_parms.network_mode_web=atoi(configs.network_mode_web.Value);
	strcpy(global_init_parms.plmn,configs.plmn.Value);
	global_init_parms.plmn_mode=atoi(configs.plmn_mode.Value);
	global_init_parms.plmn_act=atoi(configs.plmn_act.Value);
	strcpy(global_init_parms.ppp_auth_type,configs.ppp_auth_type.Value);
	strcpy(global_init_parms.ppp_username,configs.ppp_username.Value);
	strcpy(global_init_parms.ppp_password,configs.ppp_password.Value);
	strcpy(global_init_parms.ipstack,configs.ipstack.Value);
	strcpy(global_init_parms.network_card_mtu,configs.network_card_mtu.Value);


	log_info("%s_%d:[%s %s] [%s %s] [%s %s] [%s %s] [%s %s] [%s %s] \n"
		,__FUNCTION__,__LINE__,	configs.pin.Name,configs.puk.Value,
		configs.puk.Name,configs.puk.Value,configs.apn.Name,configs.apn.Value,
		configs.ppp_auth_type.Name,configs.ppp_auth_type.Value,
		configs.ppp_username.Name,configs.ppp_username.Value,
		configs.ppp_password.Name,configs.ppp_password.Value);
	log_info("%s_%d:[%s %s] [%s %s] [%s %s] [%s %s] [%s %s] [%s %s] [%s %s]\n"
		,__FUNCTION__,__LINE__,
		configs.network_mode_web.Name,configs.network_mode_web.Value,
		configs.lte_band_choose.Name,configs.lte_band_choose.Value,
		configs.plmn.Name,configs.plmn.Value,
		configs.plmn_mode.Name,configs.plmn_mode.Value,
		configs.plmn_act.Name,configs.plmn_act.Value,
		configs.ipstack.Name,configs.ipstack.Value,
		configs.network_card_mtu.Name,configs.network_card_mtu.Value);
}

int get_version(char* file_name,GSI* p)
{
	FILE* f;
	char buffer[1024];
	char* tmp_ptr=NULL;
	char* tmp_ptr_1=NULL;
	char* tmp_ptr_2=NULL;
	if(NULL == (f=fopen(file_name,"r")))
	{
		return FALSE;
	}
	while(!feof(f))
	{
		memset(buffer,0,sizeof(buffer));
		fgets(buffer,sizeof(buffer),f);
		if(NULL!=(tmp_ptr=strstr(buffer,"hwversion:")))
		{
			tmp_ptr_1=tmp_ptr+strlen("hwversion:");
			tmp_ptr_2=strip_head_tail_space(tmp_ptr_1);
			strncpy(p->sys_hardware,tmp_ptr_2,strlen(tmp_ptr_2));
			log_info("%s,%s",__FUNCTION__,p->sys_hardware);
		}else if(NULL!=(tmp_ptr=strstr(buffer,"version:")))
		{
			tmp_ptr_1=tmp_ptr+strlen("version:");
			tmp_ptr_2=strip_head_tail_space(tmp_ptr_1);
			strncpy(p->sys_softver,tmp_ptr_2,strlen(tmp_ptr_2));
//			log_info("%s,%s",__FUNCTION__,p->sys_softver);
		}
		else if(NULL!=(tmp_ptr=strstr(buffer,"device:")))
		{
			tmp_ptr_1=tmp_ptr+strlen("device:");
			tmp_ptr_2=strip_head_tail_space(tmp_ptr_1);
			strncpy(p->device_name,tmp_ptr_2,strlen(tmp_ptr_2));
//			log_info("%s,%s",__FUNCTION__,p->device_name);
		}else if(NULL!=(tmp_ptr=strstr(buffer,"sha1:")))
		{
			tmp_ptr_1=tmp_ptr+strlen("sha1:");
			tmp_ptr_2=strip_head_tail_space(tmp_ptr_1);
			strncpy(p->sys_sha,tmp_ptr_2,strlen(tmp_ptr_2));
//			log_info("%s,%s",__FUNCTION__,p->sys_sha);
		}else if(NULL!=(tmp_ptr=strstr(buffer,"build:")))
		{
			tmp_ptr_1=tmp_ptr+strlen("build:");
			tmp_ptr_2=strip_head_tail_space(tmp_ptr_1);
			strncpy(p->build_time,tmp_ptr_2,strlen(tmp_ptr_2));
//			log_info("%s,%s",__FUNCTION__,p->build_time);
		}else
		{
			continue;
		}
	}
	return TRUE;
		
}

int write_sysinfo_static(GSI* p)
{
	FILE* f;
	f=fopen(SYSTEM_INFO_STATIC,"w+");
	if(NULL != f)
	{
		fprintf(f,"device_name:%s\n",p->device_name);
		fprintf(f,"hardware_version:%s\n",p->sys_hardware);
		fprintf(f,"software_version:%s\n",p->sys_softver);
		fprintf(f,"sha:%s\n",p->sys_sha);
		fprintf(f,"module_manufacturer:%s\n",p->module_info.manufacturer);
		fprintf(f,"module_hardware:%s\n",p->module_info.hardver);
		fprintf(f,"module_softver:%s\n",p->module_info.softver);
		fprintf(f,"module_imei:%s\n",p->module_info.imei);
		fclose(f);
		return TRUE;
	}
	else
		return  FALSE;
	
}
int update_web_4g_status(void)
{
	char buffer[1024] = "";
	FILE* f;
	f=fopen(DIAL_STATUS_INDICATOR,"w+");
	if(NULL!=f)
	{
		fprintf(f,"%d %d %d %d %d %d %d %d %d %d ",
			global_dial_vars.is_sim_exist,
			global_dial_vars.device_lock_pin,
			global_dial_vars.device_lock_puk,
			global_dial_vars.signal_rssi_level,
			0,				//mesg indicator
			global_dial_vars.reg_status,
			global_dial_vars.greg_status,
			global_dial_vars.ereg_status,
			global_dial_vars.act,
			0);
		fclose(f);
		
		snprintf(buffer,sizeof(buffer),"rssi:%s",global_dial_vars.signal_rsrp);
		cmd_echo(buffer,RSSI_DISPLAY);

		return TRUE;
	}
	
	snprintf(buffer,sizeof(buffer),"rssi:%s",global_dial_vars.signal_rsrp);
	cmd_echo(buffer,RSSI_DISPLAY);
	
	return FALSE;
}

int write_sysinfo_dynamic(GDV* p)
{
	FILE* f;
	f=fopen(SYSTEM_INFO_DYNAMIC,"w+");
	if(NULL != f)
	{
		fprintf(f,"iccid:%s\n",p->iccid);
		fprintf(f,"imsi:%s\n",p->imsi);
		fprintf(f,"band:%s\n",p->band);
		fprintf(f,"act:%d\n",p->act);
		fprintf(f,"service_status:%d\n",p->service_status);
		fprintf(f,"ps_cs_status:%d\n",p->ps_cs_region);
		fprintf(f,"sim_real_lock_pin:%d\n",p->sim_real_lock_pin);	
		fprintf(f,"lock_pin_flag:%d\n",p->device_lock_pin);
		fprintf(f,"lock_puk_flag:%d\n",p->device_lock_puk);
		fprintf(f,"pinlock_enable_flag:%d\n",p->pinlock_enable_flag);
		fprintf(f,"is_sim_exist:%d\n",p->is_sim_exist);
		fprintf(f,"2g_register_status:%d\n",p->reg_status);
		fprintf(f,"3g_register_status:%d\n",p->greg_status);
		fprintf(f,"4g_register_stauts:%d\n",p->ereg_status);
		fprintf(f,"roam_status:%d\n",p->roam_status);
		fprintf(f,"pin_left_times:%d\n",p->pin_left_times);
		fprintf(f,"puk_left_times:%d\n",p->puk_left_times);
		fprintf(f,"network_type_num:%d\n",p->network_mode_bmrat);
		fprintf(f,"network_type_str:%s\n",p->network_mode_detail);
		fprintf(f,"ipstack_current:%d\n",p->ipstack_registered);
		fprintf(f,"signal_lvl:%d\n",p->signal_rssi_level);
		fprintf(f,"rssi:%s\n",p->signal_rssi_value);
		fprintf(f,"network_link_stauts:%d\n",p->network_link);
		fprintf(f,"CELL_ID:%s\n",p->cell_id);
		fprintf(f,"PCI:%s\n",p->pci);
		fprintf(f,"RSRP:%s\n",p->signal_rsrp);
		fprintf(f,"RSRQ:%s\n",p->signal_rsrq);
		fprintf(f,"SINR:%s\n",p->signal_sinr);
		fprintf(f,"LAC_ID:%s\n",p->lac_id);
		fprintf(f,"ACTIVE_BAND:%s\n",p->active_band);
		fprintf(f,"ACTIVE_CHANNEL:%s\n",p->active_channel);
		fprintf(f,"EARFCN:%s\n",p->earfcn);
		fprintf(f,"ENODEBID:%s\n",p->enodebid);
		fprintf(f,"TAC:%s\n",p->tac);
		fprintf(f,"RRC_STATUS:%s\n",p->rrc_status);
		//BAND 
		fprintf(f,"GW_ALL_BAND:%s\n",p->gw_all_band);
		fprintf(f,"LTE_ALL_BAND:%s\n",p->lte_all_band); 
		fprintf(f,"TDS_ALL_BAND:%s\n",p->tds_all_band);
		fprintf(f,"GW_LOCK_BAND:%s\n",p->gw_lock_band);
		fprintf(f,"LTE_LOCK_BAND:%s\n",p->lte_lock_band);
		fprintf(f,"TDS_LOCK_BAND:%s\n",p->tds_lock_band);
		
		fprintf(f,"OPERATOR:%s\n",p->operator_current);
		fprintf(f,"PLMN:%s\n",p->operator_plmn);
		//moduleType
		fprintf(f,"ModuleType:%s\n",p->moduleType);
		fclose(f);
		return TRUE;
	}
	else
		return  FALSE;
	
}



int set_dial_func_set(GD* p,GSI* q)
{
	int i;
	char tmp[200];
	memset(tmp,0,sizeof(tmp));
	for(i=0;i<project_info_len();i++)
	{
		/*sprintf(tmp,"echo %s >> /tmp/test",q->sys_hardware);
		system(tmp);
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp,"echo %s >> /tmp/test",project_info_summary[i]->hardware_type);
		system(tmp);
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp,"echo %s >> /tmp/test",q->module_info.hardver);
		system(tmp);
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp,"echo %s >> /tmp/test",project_info_summary[i]->module_name);
		system(tmp);*/
		if(A_CMP_B_ACCORDING_B(q->sys_hardware,project_info_summary[i]->hardware_type)&&\
			A_CMP_B_ACCORDING_B(q->module_info.hardver,project_info_summary[i]->module_name))
		{
			p->boardtype_module_combine=project_info_summary[i];
			p->board_func_set=project_info_summary[i]->process_functions;
			strncpy(p->web_display_flag,project_info_summary[i]->flag_file,strlen(project_info_summary[i]->flag_file));
			return TRUE;
		}
	}
	return FALSE;
}

int install_module_driver(char* driver_name)
{
#ifdef DEBUG
	return 0;
#endif
	char buffer[1024];
	int status;
	//snprintf(buffer,sizeof(buffer),"modprobe %s",driver_name);
	snprintf(buffer,sizeof(buffer),"modprobe %s",driver_name);
	status=system(buffer);
	if(-1==status)
		return status;
	else
	{
		if(WIFEXITED(status))
		{
			if(WEXITSTATUS(status)==0)
				return 0;
			else
				return WEXITSTATUS(status);
		}
		else
			return WEXITSTATUS(status);
	}
}
int up_module_networkcard(char* card_name,char* mtu)
{
	char cmd_buffer[1024];
	int status;
	snprintf(cmd_buffer,sizeof(cmd_buffer),"ifconfig %s down ; ifconfig %s mtu %s && ifconfig %s up",
		global_system_info.module_info.network_card_name,global_system_info.module_info.network_card_name,
		global_init_parms.network_card_mtu,global_system_info.module_info.network_card_name);
	status=system(cmd_buffer);
	if(-1==status)
		return status;
	else
	{
		if(WIFEXITED(status))
		{
			if(WEXITSTATUS(status)==0)
				return 0;
			else
				return WEXITSTATUS(status);
		}
		else
			return WEXITSTATUS(status);
	}
}
void thr_recv(void* args)
{
	fd_set readfds;
	char *buffer_recv=NULL;
	int maxfd;
	int out_flag;
	int bytes_n = 0;
	int bState = 0;
	int timeout_counter = 0;

	log_info("go into:%s\n",__FUNCTION__);
	while(1)
	{
		out_flag=1;
		FD_ZERO(&readfds);
		FD_SET(global_dialtool.dev_handle,&readfds);
		maxfd=global_dialtool.dev_handle+1;
		//log_info("%s_%d:bState:%d timeout_counter:%d buffer_recv:%s\n",__FUNCTION__,__LINE__,bState,timeout_counter,(char*)buffer_recv);
		switch(bState)
		{
			case 0:
				{
					buffer_recv=(char *)m_malloc(RECV_BUFF_SIZE);
					bytes_n=0;

					out_flag=select(maxfd,&readfds,NULL,NULL,NULL);
					if(out_flag>0)
					{
						out_flag=read(global_dialtool.dev_handle,buffer_recv+bytes_n,RECV_BUFF_SIZE-bytes_n);
						bytes_n+=out_flag;

						//log_info("%s_%d:sendAtFlag:%d\n",__FUNCTION__,__LINE__,sendAtFlag);
						if(sendAtFlag == 1)
						{
							sendAtFlag = 0;
							if(NULL != strstr(buffer_recv,CMD_EXE_OK)  ||
								NULL != strstr(buffer_recv,CMD_EXE_ERROR)  ||
								NULL != strstr(buffer_recv,CMD_RESULT_CME_ERROR) ||
								NULL != strstr(buffer_recv,CMD_RESULT_CMS_ERROR) ||
								NULL != strstr(buffer_recv,CMD_RESULT_CONNECT) ||
								NULL != strstr(buffer_recv,CMD_RESULT_NO_CARRIER))
							{
								log_info("%s_%d:%s\n",__FUNCTION__,__LINE__,buffer_recv);
								enqueue(&at_recv_buff,(void *)buffer_recv);
								//	log_info("address:%lu,%lu\n",&at_recv_buff,at_recv_buff.head);			
							}
							else
							{
								bState = 1;
							}
						}
						else//sendAtFlag:0 report
						{
							if(NULL != strstr(buffer_recv,"\r\n"))
							{
								log_info("%s_%d:%s\n",__FUNCTION__,__LINE__,buffer_recv);
								enqueue(&at_recv_buff,(void *)buffer_recv);
								//	log_info("address:%lu,%lu\n",&at_recv_buff,at_recv_buff.head);
							}
							else
							{
								bState = 2;
							}
						}
					}
					else if(out_flag == 0)
						free(buffer_recv);
				}
				break;

			case 1://wait ok error...
				{
					struct timeval tv1;
					tv1.tv_sec=1;
					tv1.tv_usec=0;

					out_flag=select(maxfd,&readfds,NULL,NULL,&tv1);
					if(out_flag > 0)
					{
						out_flag=read(global_dialtool.dev_handle,buffer_recv+bytes_n,RECV_BUFF_SIZE-bytes_n);
						bytes_n+=out_flag;
					}
					else if(out_flag == 0)
						timeout_counter++;

					if(NULL != strstr(buffer_recv,CMD_EXE_OK)  ||
						NULL != strstr(buffer_recv,CMD_EXE_ERROR)  ||
						NULL != strstr(buffer_recv,CMD_RESULT_CME_ERROR) ||
						NULL != strstr(buffer_recv,CMD_RESULT_CMS_ERROR) ||
						NULL != strstr(buffer_recv,CMD_RESULT_CONNECT) ||
						NULL != strstr(buffer_recv,CMD_RESULT_NO_CARRIER))
					{
						timeout_counter = 0;
						bState = 0;
						log_info("%s_%d:%s\n",__FUNCTION__,__LINE__,buffer_recv);
						enqueue(&at_recv_buff,(void *)buffer_recv);
						//	log_info("address:%lu,%lu\n",&at_recv_buff,at_recv_buff.head);			
					}
					else
					{
						if(timeout_counter > 6)
						{
							log_info("%s_%d:bState1 Timeout\n",__FUNCTION__,__LINE__);
							timeout_counter = 0;
							bState = 0;
							free(buffer_recv);
						}
					}
				}
				break;

			case 2://report wait \r\n
				{
					struct timeval tv2;
					tv2.tv_sec=1;
					tv2.tv_usec=0;

					out_flag=select(maxfd,&readfds,NULL,NULL,&tv2);
					if(out_flag > 0)
					{
						out_flag=read(global_dialtool.dev_handle,buffer_recv+bytes_n,RECV_BUFF_SIZE-bytes_n);
						bytes_n+=out_flag;
					}
					else if(out_flag == 0)
						timeout_counter++;

					if(NULL != strstr(buffer_recv,"\r\n"))
					{
						timeout_counter = 0;
						bState = 0;
						log_info("%s_%d:%s\n",__FUNCTION__,__LINE__,buffer_recv);
						enqueue(&at_recv_buff,(void *)buffer_recv);
						//	log_info("address:%lu,%lu\n",&at_recv_buff,at_recv_buff.head);			
					}
					else
					{
						if(timeout_counter > 6)
						{
							log_info("%s_%d:bState2 Timeout\n",__FUNCTION__,__LINE__);
							timeout_counter = 0;
							bState = 0;
							free(buffer_recv);
						}
					}
				}
				break;

			default:
				break;
		}
	}
}
void thr_process(void *args)
{
	log_info("go into:%s\n",__FUNCTION__);
	MP *b=global_dialtool.boardtype_module_combine;
	DialProc *p=global_dialtool.board_func_set;
	void* queue_tail=NULL;
//	void* queue_tail_sub=NULL;
	while(1)
	{
//		usleep(500);
		//log_info("address:%lu,%lu\n",&at_recv_buff,at_recv_buff.head);
		if(isempty(&at_recv_buff))
		{
			if(global_sleep_interval_long)
				sleep(1);
			else
				usleep(500000);
//			log_info("queue is empty\n");
			continue;
		}
		else
		{
			log_info("%s_%d:+++++\n",__FUNCTION__,__LINE__);
			queue_tail=(void* )dequeue(&at_recv_buff);
			log_info("dequeue:%s\n",(char *) queue_tail);
		}
//		queue_tail_sub=strip_head_tail_space((void*)queue_tail);
//		log_info("%s_%d:%s\n",__FUNCTION__,__LINE__,(char*)queue_tail);
		if(NULL== queue_tail)
			free(queue_tail);
		else if(NULL != strstr(queue_tail,CMD_EXE_OK)  ||
			NULL != strstr(queue_tail,CMD_EXE_ERROR)  ||
			NULL != strstr(queue_tail,CMD_RESULT_CME_ERROR) ||
			NULL != strstr(queue_tail,CMD_RESULT_CMS_ERROR) ||
			NULL != strstr(queue_tail,CMD_RESULT_CONNECT) ||
			NULL != strstr(queue_tail,CMD_RESULT_NO_CARRIER))
		{
			memset(global_dialtool.buffer_at_sponse,0,sizeof(global_dialtool.buffer_at_sponse));
			strncpy(global_dialtool.buffer_at_sponse,queue_tail,strlen(queue_tail));
			/*different state,different treat*/
			switch(global_dialtool.Dial_Lvl_1)
			{
				case DIAL_INIT:
						b->init_boardtype();
						p->init_proc_func(&global_dialtool.Dial_proc_state);
						break;
				case DIAL_DIAL:
						p->dial_proc_func(&global_dialtool.Dial_proc_state);
						break;
				case DIAL_DEAMON:
						p->deamon_proc_func(&global_dialtool.Dial_proc_state);
						b->update_boardtype();
						sleep(2);
						break;
				default:
						p->init_proc_func(&global_dialtool.Dial_proc_state);
						break;
			}
			free(queue_tail);
			global_dialtool.refresh_timer_flag=1;
			raise(SIGALRM);
		}
		else
		{
			p->reporter_handle_func(queue_tail);
			free(queue_tail);  //now report data abandoned
		}

	}
	
}

void common_sendat(int num)
{
	sendAtFlag = 1;
	(global_dialtool.board_func_set->sendat)(num);
}



static int usb_id2dev(char* dev_name, char *id, int len)
{
	char buff[50];
	char result_str[50];
	if(strstr(id, "ttyUSB") != 0)
	{
		log_info("as your wish :%s:%s %d\n",dev_name, __FUNCTION__,__LINE__);
		strcpy(dev_name, id);
		return 0;
	}	
	FILE *f; 
	sprintf(buff, "ls -l /sys/bus/usb-serial/drivers/option1 | grep %s | cut -b 58-64", id);
	 f=popen(buff,"r");
	 if( NULL != f ) 
    	{   
        	 if(fgets(result_str,sizeof(result_str),f))
        	 {
		 	sprintf(dev_name, "/dev/%s", result_str);
		 	dev_name[strlen(dev_name)-1] = 0; //have \r in it.
		 	//strcpy(dev_name, "/dev/ttyUSB2");
		 }
		 pclose(f);
		 log_info("%s:%s %d\n",dev_name, __FUNCTION__,__LINE__);
		 return 0;
	 }
	 len = len;
	 return 1;
}


static int read_memory(char *shellcmd, char *out, int size) {

	FILE *stream;
	char buffer[size];
    memset(buffer, 0, sizeof(buffer));

	stream = popen(shellcmd, "r");
	if(stream != NULL){
        fread(buffer, sizeof(char), sizeof(buffer), stream);
        pclose(stream);

		memcpy(out, buffer, strlen(buffer) + 1);

		return 0;
	} else {
		out[0] = '\0';
	}

	return -1;
}


#define modem_in_usb2_0 "1"
#define modem_in_usb3_0 "2"

int main(int argc,char *argv[] )
{
	IDPV * ids_module=&global_system_info.module_info.module_serial;		//store module pid and vid
	initqueue(&at_recv_buff);
	pthread_t  thread_recv,thread_process;
/* first read configs into program */
	extern char *optarg;
	extern int optind,opterr,optopt;
	int ret;
	unsigned int baudrate=115200;
	char config_file_name[64]={0};
	int i;

	char* vendor_id_file = NULL;
	char* product_id_file = NULL;
	
	//printf("compile at %s %s\n", __DATE__,__TIME__);
	openlog(argv[0],  LOG_PID, 0);  
	log_info("program start:%s %d\n",__FUNCTION__,__LINE__);
	while((ret= getopt(argc,argv,"s:")) != -1)
	{
		switch(ret)
		{
			case 's':
				strncpy(config_file_name,optarg,sizeof(config_file_name));
				break;
			default:
				log_error("missing config file\n");
				write_str_file(DIAL_INDICATOR,"missing config file","w+");
				exit(-1);
		}
	}
	log_info("%s %d\n",__FUNCTION__,__LINE__);
	fill_config(config_file_name,cfg);
	init_parms(cfg,sizeof(cfg)/sizeof(PARMS));

	while(1)
	{
		char cmd[512] = "dmesg | grep ttyUSB | grep -v DT | grep ttyUSB0 | awk '{print $4}' | awk -F \"-\" '{print $1}'";
		char usbx_flag[8] = "";
		read_memory(cmd, usbx_flag,sizeof(usbx_flag));
		if(usbx_flag[0])
		{
			if( strncmp(usbx_flag, modem_in_usb2_0, 1) == 0 )
			{
				//usb2.0
				product_id_file = PRODUCTID_MT7621_FILE_2_0;
				vendor_id_file = VENDORID_MT7621_FILE_2_0;
				break;
			}
			else if( strncmp(usbx_flag, modem_in_usb3_0, 1) == 0 )
			{
				//usb3.0 
				product_id_file = PRODUCTID_MT7621_FILE_3_0;
				vendor_id_file = VENDORID_MT7621_FILE_3_0;
				break;
			}
			
			log_info("%s %d: wait ttyUSBx...\n",__FUNCTION__,__LINE__);
			sleep(3);
		}
		
	}

	
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>enable_pin = %s\n",configs.enable_pin.Value);
	while(1)
	{
		static int checkPvCounts = 0;
		static int outWhile = 0;
		//get  pid and vid of the device
		ids_module->idVendor=get_value(vendor_id_file,16);
		ids_module->idProduct=get_value(product_id_file,16);
		/*
		** make sure we get right idProduct and idVendor
		**/
		if(-1==ids_module->idProduct ||-1==ids_module->idVendor)
		{
			char buffer_tmp[1024];
			snprintf(buffer_tmp,sizeof(buffer_tmp),"get pid or vid error,pid:%d,vid:%d\n",ids_module->idProduct,ids_module->idVendor);
			log_error(buffer_tmp);
			write_str_file(DIAL_INDICATOR,buffer_tmp,"w+");
			//exit(-1);				//maybe we shoud reset module		
		}
		else
		{
			for(i = 0; i < ARRAY_MEMBER_COUNT(modules_info_summary); i++ )
			{
				if(modules_info_summary[i] == NULL)
					break;
				if(ids_module->idVendor==modules_info_summary[i]->module_serial.idVendor && 
					ids_module->idProduct==modules_info_summary[i]->module_serial.idProduct)
				{
					outWhile = 1;
				}
			}
		}
		
		if(outWhile)
			break;
		
		//if 1 min no get eth idVendor and idProduct,restart modem and exit
		checkPvCounts++;
		if(checkPvCounts >= 30)
		{
			system("/etc/rc.d/rc.reset_module");
			printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> will go to reset modem\n");
			exit(-1);
		}
		
		sleep(2);
	}
		//printf("ids_module->idVendor=%x, ids_module->idProduct=%x\n", ids_module->idVendor, ids_module->idProduct);
	for(i=0;i<ARRAY_MEMBER_COUNT(modules_info_summary);i++)
	{
		//printf("modules_info_summary[i]->module_serial.idVendor=%x, modules_info_summary[i]->module_serial.idProduct=%x\n", 
		//	modules_info_summary[i]->module_serial.idVendor, modules_info_summary[i]->module_serial.idProduct);
		if(modules_info_summary[i] == NULL)
			break;
		if(ids_module->idVendor==modules_info_summary[i]->module_serial.idVendor && 
			ids_module->idProduct==modules_info_summary[i]->module_serial.idProduct)
		{
				//printf("founded\n");
			usb_id2dev(global_system_info.module_info.dev_name,(char *)modules_info_summary[i]->dev_name,
				strlen(modules_info_summary[i]->dev_name));
			global_system_info.module_info.get_moduleinfo=modules_info_summary[i]->get_moduleinfo;
			strncpy(global_system_info.module_info.driver_name,modules_info_summary[i]->driver_name,
				strlen(modules_info_summary[i]->driver_name));
			strncpy(global_system_info.module_info.network_card_name,modules_info_summary[i]->network_card_name,
				strlen(modules_info_summary[i]->network_card_name));
			break;
		}
	}
	if(i>=ARRAY_MEMBER_COUNT(modules_info_summary))
	{
		log_error("dialtool dont support the module\n");
		write_str_file(DIAL_INDICATOR,"don't support the module","w+");
		exit(-1);
	}
	
	if(global_system_info.module_info.driver_name[0]) //some module do not need extra ko.
	if(0!=install_module_driver(global_system_info.module_info.driver_name))
	{
		char buffer_tmp[1024];
		snprintf(buffer_tmp,sizeof(buffer_tmp),"module driver load failed:%s\n",global_system_info.module_info.driver_name);
		log_error(buffer_tmp);
		write_str_file(DIAL_INDICATOR,buffer_tmp,"w+");
		exit(-1);
	}
	/*ifconfig up usb0 or other*/
	if(global_system_info.module_info.network_card_name[0])
	if(0!=up_module_networkcard(global_system_info.module_info.network_card_name,global_init_parms.network_card_mtu))
	{
		char buffer_tmp[1024];
		snprintf(buffer_tmp,sizeof(buffer_tmp),"up module network card  failed:%s\n",global_system_info.module_info.driver_name);
		log_error(buffer_tmp);
		write_str_file(DIAL_INDICATOR,buffer_tmp,"w+");
		exit(-1);
	}
	global_dialtool.dev_handle=open(global_system_info.module_info.dev_name,O_RDWR);
	if(-1== global_dialtool.dev_handle)
	{
		char buffer_tmp[1024];
		snprintf(buffer_tmp,sizeof(buffer_tmp),"open %s failed!\n",global_system_info.module_info.dev_name);
		log_error("%s %d", buffer_tmp, errno);
		write_str_file(DIAL_INDICATOR,buffer_tmp,"w+");
		return TRUE;
	}

	config_device_mode(global_dialtool.dev_handle,baudrate,8,1,'s');
	log_info("open %s,fd:%d\n",global_system_info.module_info.dev_name,global_dialtool.dev_handle);
	//system("echo to here crash >> /tmp/test");
	global_system_info.module_info.get_moduleinfo(&global_system_info.module_info);
	cmd_echo("0",USB_SERIAL_STATUS);
	//system("echo ready >> /tmp/test");
	log_info("imei:%s\n",global_system_info.module_info.imei);
	if(FALSE==get_version("/etc/dialogtool2_version",&global_system_info))
	{
		char buffer_tmp[1024];
		snprintf(buffer_tmp,sizeof(buffer_tmp),"%s:get version error\n",__FUNCTION__);
		log_error(buffer_tmp);
		write_str_file(DIAL_INDICATOR,buffer_tmp,"w+");
		close(global_dialtool.dev_handle);
		exit(-1);
	}
	log_info("%s,%s",__FUNCTION__,global_system_info.sys_hardware);
	if(FALSE == set_dial_func_set(&global_dialtool,&global_system_info))
	{
		char buffer_tmp[1024];
		snprintf(buffer_tmp,sizeof(buffer_tmp),"not support the conbination of the board and module,sys_hardware:%s module_hardware:%s\n",
			global_system_info.sys_hardware,global_system_info.module_info.hardver);
		log_error(buffer_tmp);
		write_str_file(DIAL_INDICATOR,buffer_tmp,"w+");	
		close(global_dialtool.dev_handle);
		exit(-1);
	}
	/*here we touch a flag file for web*/
	cmd_touch(global_dialtool.web_display_flag);
	write_sysinfo_static(&global_system_info);
	global_dialtool.Dial_Lvl_1=DIAL_INIT;
	global_dialtool.Dial_proc_state=Dial_State_initialized;

/* create a pthread to recv AT result and not-asked 	AT code,insert to the received queue*/
	if(0 != pthread_create(&thread_recv,NULL,(void* )thr_recv,NULL))
	{
		log_error("thr_recv create failed\n");
		write_str_file(DIAL_INDICATOR,"thr_recv create failed","w+");	
		close(global_dialtool.dev_handle);
		exit(-1);
	}
/*create a pthread deal with recevied queue,
take member of receved queue,and compare with system report info(include led about hardware ),
then compare with AT sended ,if yes keep result, else abandon the result */
	if(0 != pthread_create(&thread_process,NULL,(void*)thr_process,NULL))
	{
		log_error("thr_process create failed\n");
		write_str_file(DIAL_INDICATOR,"thr_process create failed","w+");	
		close(global_dialtool.dev_handle);
		exit(-1);
	}

/*we use a interupt to send AT,check return result then to next state,
 write a dial process for every module,and we invoke the function pointer*/
 	int res;
	
	struct itimerval tick;
	memset(&tick,0,sizeof(tick));
	tick.it_value.tv_sec = 2;
	tick.it_value.tv_usec =0;
	tick.it_interval.tv_sec = TIMER_DELAY;
	tick.it_interval.tv_usec =0;

	signal(SIGALRM,common_sendat);
	res=setitimer(ITIMER_REAL,&tick,NULL);

	if(res!=0)
	{
		log_error("settitimer error\n");
		write_str_file(DIAL_INDICATOR,"settitimer error","w+");
		close(global_dialtool.dev_handle);
		exit(-1);
	}



	while(1)
	{
		/*can detect config changed ,then redial module */
		sleep(1);
		if(global_dialtool.board_func_set->timer_proc_func)
			global_dialtool.board_func_set->timer_proc_func(&global_dialtool.Dial_proc_state);
		if(global_dialtool.pthread_moniter_flag == 1)
		{
			pthread_join(thread_recv,NULL);
			pthread_join(thread_process,NULL);
			global_dialtool.pthread_moniter_flag = 0;
		}
		if(global_dialtool.refresh_timer_flag==1)
		{
			if(global_dialtool.board_func_set->timer_proc_func)
				global_dialtool.board_func_set->timer_proc_func(&global_dialtool.Dial_proc_state);
			global_dialtool.refresh_timer_flag=0;
			refresh_timer(TIMER_DELAY);
		}
		update_web_4g_status();
		write_sysinfo_dynamic(&global_dial_vars);	
	}

}

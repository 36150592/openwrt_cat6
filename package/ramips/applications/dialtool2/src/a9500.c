

#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "common.h"
#include "a9500.h"
#include "include.h"

#define AR9500_DIALST_INIT   0
#define AR9500_DIALST_CMEE2 1
#define AR9500_DIALST_CRC1  2
#define AR9500_DIALST_CREG2  3
#define AR9500_DIALST_CGREG2  4
#define AR9500_DIALST_CEREG2  5
#define AR9500_DIALST_COPS3  6
#define AR9500_DIALST_ICCID  7
#define AR9500_DIALST_CIMI   8

#define AR9500_DIALST_SYSINFO 9
#define AR9500_DIALST_PSRAT 10
#define AR9500_DIALST_CSQ 11

#define AR9500_State_CFUN_DISABLE 12
#define AR9500_State_CFUN_ENABLE 13

#define AR9500_DIALST_MODODR 14

#define AR9500_DIALST_CPNNUM 15
#define AR9500_DIALST_CPIN_SET 16

#define AR9500_DIALST_LCTCELLINFO 17
#define AR9500_DIALST_LSCELLINFO 18

#define AR9500_DIALST_QUERYCFUN 19


#define AR9500_DIALST_CPIN  100

#define AR9500_DIALST_CHECKPPP0 300

#define AR9500_DIALST_IDLE 999



char tempString[32] = {0};

int getTmpFileValue(char* file_name)
{
	
	FILE* f;
	char buffer[20];
	int len;
	int ret;
	if(NULL == (f=fopen(file_name,"r")))
	{
		log_info("%s_%d,can't open %s\n",__FUNCTION__,__LINE__,file_name);
		return 0;
	}
	memset(buffer,0,sizeof(buffer));
	len = fread(buffer,1,10,f);
	fclose(f);
	if(len == -1)
	{
		log_info("%s_%d\n,can't read %s",__FUNCTION__,__LINE__,file_name);
		return 0;
	}
	ret = atoi(buffer);
	log_info("%s_%d,%s value = %d\n,",__FUNCTION__,__LINE__,file_name,ret);

	return ret;

}
static void ar9500_get_moduleinfo(MDI* p)
{
	char *buffer_recv=NULL;

	int length;
	char buffer[1024]={0};
	char* str_ptr=NULL;
	char* str_sub_ptr=NULL;
	char* result_ptr=NULL;
	char * tmp_ptr;
	
	buffer_recv=(char *)m_malloc(RECV_BUFF_SIZE);
	
howitpossible:
	util_send_cmd(global_dialtool.dev_handle,"ATE1\r",&global_dialtool.pthread_moniter_flag);
	sleep(1);
	util_send_cmd(global_dialtool.dev_handle,"ATI\r",&global_dialtool.pthread_moniter_flag);
	sleep(1);
	//util_send_cmd(global_dialtool.dev_handle,"AT+GSN\r",&global_dialtool.pthread_moniter_flag);
	//sleep(2);
	read(global_dialtool.dev_handle,buffer_recv,RECV_BUFF_SIZE);
	//printf(buffer_recv);

	str_ptr=strstr(buffer_recv,"Manufacturer:");
	if(NULL!=str_ptr)
	{
		tmp_ptr=str_ptr+strlen("Manufacturer:");
		str_sub_ptr=strstr(tmp_ptr,"\n");
		if(NULL != str_sub_ptr)
		{
			length=str_sub_ptr-tmp_ptr;
			strncpy(buffer,tmp_ptr,length);
			buffer[length]='\0';
			result_ptr=strip_head_tail_space(buffer);
			strncpy(p->manufacturer,result_ptr,strlen(result_ptr));
		}
	}
	str_ptr=strstr(buffer_recv,"Model:");
	if(NULL!=str_ptr)
	{
		tmp_ptr=str_ptr+strlen("Model:");
		str_sub_ptr=strstr(tmp_ptr,"\n");
		if(NULL != str_sub_ptr)
		{
			length=str_sub_ptr-tmp_ptr;
			strncpy(buffer,tmp_ptr,length);
			buffer[length]='\0';
			result_ptr=strip_head_tail_space(buffer);
			strncpy(p->hardver,result_ptr,strlen(result_ptr));
		}
	}
	str_ptr=strstr(buffer_recv,"Revision:");
	if(NULL!=str_ptr)
	{
		tmp_ptr=str_ptr+strlen("Revision:");
		str_sub_ptr=strstr(tmp_ptr,"\n");
		if(NULL != str_sub_ptr)
		{
			length=str_sub_ptr-tmp_ptr;
			strncpy(buffer,tmp_ptr,length);
			buffer[length]='\0';
			result_ptr=strip_head_tail_space(buffer);
			strncpy(p->softver,result_ptr,strlen(result_ptr));
		}
	}
	str_ptr=strstr(buffer_recv,"IMEI:");
	if(NULL!=str_ptr)
	{
		tmp_ptr=str_ptr+strlen("IMEI:");
		str_sub_ptr=strstr(tmp_ptr,"\n");
		if(NULL != str_sub_ptr)
		{
			length=str_sub_ptr-tmp_ptr;
			strncpy(buffer,tmp_ptr,length);
			buffer[length]='\0';
			result_ptr=strip_head_tail_space(buffer);
			strncpy(p->imei,result_ptr,strlen(result_ptr));

		}
	}

	if(p->imei[0] == '\0')
		goto howitpossible;
	m_free(buffer_recv);

}



MDI ar9500_moduleinfo=
{
	{TZ_VENNOR_AR9500, TZ_PRODUCT_AR9500},
	"/dev/ttyUSB3",
	ar9500_get_moduleinfo,
	"0",
	"0",
	"0",
	"0",
	"\0",
	"\0",
};

void ar9500_306a_deamon(int* Dial_proc_state);

void ar9500_sendat(int num)
{
	//printf("%s %d, global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__, global_dialtool.Dial_proc_state);
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
	switch(global_dialtool.Dial_proc_state)
	{
		case AR9500_DIALST_INIT:
			util_send_cmd(global_dialtool.dev_handle,"ATE0\r",&global_dialtool.pthread_moniter_flag);			//test AT
			break; 
			
		case AR9500_DIALST_CMEE2:
			util_send_cmd(global_dialtool.dev_handle,"AT+CMEE=2\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case AR9500_DIALST_QUERYCFUN:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case AR9500_DIALST_ICCID:
			util_send_cmd(global_dialtool.dev_handle,"AT+ICCID\r",&global_dialtool.pthread_moniter_flag);
			break;
		case AR9500_DIALST_CIMI:
			util_send_cmd(global_dialtool.dev_handle,"AT+CIMI\r",&global_dialtool.pthread_moniter_flag);  //mode 3gpp2 can use also
			break;
		case AR9500_DIALST_CRC1:
			util_send_cmd(global_dialtool.dev_handle,"AT+CRC=1\r",&global_dialtool.pthread_moniter_flag);		//report ERROR
			break;
		case AR9500_DIALST_CGREG2:
			util_send_cmd(global_dialtool.dev_handle,"AT+CGREG=2\r",&global_dialtool.pthread_moniter_flag);		//report ERROR
			break;
		case AR9500_DIALST_CEREG2:
			util_send_cmd(global_dialtool.dev_handle,"AT+CEREG=2\r",&global_dialtool.pthread_moniter_flag); 	//report ERROR
			break;
		case AR9500_DIALST_CREG2:
			util_send_cmd(global_dialtool.dev_handle,"AT+CREG=2\r",&global_dialtool.pthread_moniter_flag); 	//report ERROR
			break;
		case AR9500_DIALST_MODODR:
			//read mododrSys
			{
				char cmd_buffer[64];
				int mododrValue;
				mododrValue = getTmpFileValue("/tmp/.mododrValue");
				if(!mododrValue)
					mododrValue = 2;
				snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+MODODR=%d\r",mododrValue);
				log_info(">>>>>>>>>>>>%s_%d\n,cmd_buffer=%s,len = %d\n",__FUNCTION__,__LINE__,cmd_buffer,strlen(cmd_buffer));
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag); 	//search 2g only
			}
			
			break;
		case AR9500_DIALST_COPS3:
			util_send_cmd(global_dialtool.dev_handle,"AT+COPS=3,2\r",&global_dialtool.pthread_moniter_flag);	//report ERROR
			break;
		case AR9500_DIALST_CPIN:
			util_send_cmd(global_dialtool.dev_handle,"AT+CPIN?\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case AR9500_DIALST_CPNNUM:
			util_send_cmd(global_dialtool.dev_handle,"AT+CPNNUM\r",&global_dialtool.pthread_moniter_flag);
			break;
		case AR9500_DIALST_CPIN_SET:
			{
				char* pin_code=global_init_parms.pin;
				char cmd_buffer[64];
				if(global_dial_vars.device_lock_pin)
				{
					if(strlen(pin_code))
					{
						snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+CPIN=\"%s\"\r",pin_code);
						util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
					}
				}
				break;
			}
		case AR9500_State_CFUN_DISABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN=0\r",&global_dialtool.pthread_moniter_flag);
			break;
		case AR9500_State_CFUN_ENABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN=1\r",&global_dialtool.pthread_moniter_flag);
			break;
		case AR9500_DIALST_SYSINFO:
			util_send_cmd(global_dialtool.dev_handle,"AT^SYSINFO\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case AR9500_DIALST_LCTCELLINFO:
			util_send_cmd(global_dialtool.dev_handle,"AT+LCTCELLINFO\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case AR9500_DIALST_LSCELLINFO:
			util_send_cmd(global_dialtool.dev_handle,"AT+LSCELLINFO\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case AR9500_DIALST_PSRAT:
			util_send_cmd(global_dialtool.dev_handle,"AT+PSRAT\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case AR9500_DIALST_CSQ:
			util_send_cmd(global_dialtool.dev_handle,"AT+CSQ\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case AR9500_DIALST_CHECKPPP0:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		default:
			break;
	}
}



void ar9500_306a_init(int*  Dial_proc_state)
{
	//printf("%s %d, global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__, global_dialtool.Dial_proc_state);
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
		//init led
	switch(*Dial_proc_state)
	{
		//write query function is  troublesome  ,maybe it not safe only judge by OK or ERROR,i just try it
		case AR9500_DIALST_INIT:
			*Dial_proc_state=AR9500_DIALST_CMEE2;
			break;

		case AR9500_DIALST_CMEE2:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = AR9500_DIALST_QUERYCFUN;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = AR9500_DIALST_QUERYCFUN;
			break;

		case AR9500_DIALST_QUERYCFUN:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"1");
				if(NULL!=ptr_tmp)
				{
					*Dial_proc_state = AR9500_DIALST_ICCID;
				}
				else
				{
					*Dial_proc_state = AR9500_State_CFUN_ENABLE;
				}
			}
			break;
		
		case AR9500_DIALST_ICCID:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^SCID:");
				if(NULL!=ptr_tmp)
				{
					char* ptr_tmp1=NULL;
					ptr_tmp1=ptr_tmp+strlen("^SCID:");
					char* ptr_tmp2=strstr(ptr_tmp1,"OK");
					if(NULL!= ptr_tmp2)
						ptr_tmp2[0]='\0';
					ptr_tmp=strip_head_tail_space(ptr_tmp1);
					if(strlen(ptr_tmp)<64)
						strncpy(global_dial_vars.iccid,ptr_tmp,strlen(ptr_tmp));
					*Dial_proc_state = AR9500_DIALST_CPIN;
				}
			}		
			break;
		case AR9500_DIALST_CPIN:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
				NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_READY))
			{
				global_dial_vars.is_sim_exist=1;
				global_dial_vars.device_lock_pin=0;
				global_dial_vars.device_lock_puk=0;
				*Dial_proc_state=AR9500_DIALST_CIMI; 
				break;
			}
			else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
				NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_NEED_PIN))
			{
				global_dial_vars.is_sim_exist=1;
				global_dial_vars.device_lock_pin=1;
				global_dial_vars.device_lock_puk=0;
				*Dial_proc_state=AR9500_DIALST_CPNNUM;
				break;
			}
			else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
				NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_NEED_PUK))
			{
				global_dial_vars.is_sim_exist=1;
				global_dial_vars.device_lock_pin=0;
				global_dial_vars.device_lock_puk=1;
				*Dial_proc_state=AR9500_DIALST_CPNNUM;
				break;
			}
			else
			{
				global_dial_vars.is_sim_exist=0;
				global_dial_vars.device_lock_pin=0;
				global_dial_vars.device_lock_puk=0;
				*Dial_proc_state=AR9500_DIALST_CPIN;
				break;
			}
			break;
		case  AR9500_DIALST_CPNNUM:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"PIN1");
				if(NULL != ptr_tmp )
				{
					//remember pin times and puk times left
					ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"PIN1=");
					if(NULL != ptr_tmp)
						global_dial_vars.pin_left_times=atoi(ptr_tmp+strlen("PIN1="));
					ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"PUK1=");
					if(NULL != ptr_tmp)
						global_dial_vars.puk_left_times=atoi(ptr_tmp+strlen("PUK1="));
					//we only try first time cpin,so if cpin times not equal to 3,we wait web change pin code,pin default value is 3
					if( global_dial_vars.device_lock_pin ==1)
					{
						if(global_dial_vars.pin_left_times == 3 && global_dial_vars.device_lock_pin == 1&& strlen(global_init_parms.pin) )
						{
							*Dial_proc_state=AR9500_DIALST_CPIN_SET;
							break;
						}
						else 				//we already input pin error one time,or need input puk,so we just query and wait
						{
							*Dial_proc_state=AR9500_DIALST_CPIN;
							log_info(">>>>>>>>>pin error, %d, pin_times = %d\n",__LINE__,global_dial_vars.pin_left_times);
							break;
						}
					}
					else
					{
						*Dial_proc_state=AR9500_DIALST_CPIN;
					}
				}
				break;
			}
			break;
		case AR9500_DIALST_CPIN_SET:
			*Dial_proc_state=AR9500_DIALST_CPIN;
			sleep(5);
			break;
		case AR9500_DIALST_CIMI:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"\n")+strlen("\n");
				char* ptr_tmp2=strstr(ptr_tmp,"OK");
				if(NULL!= ptr_tmp2)
					ptr_tmp2[0]='\0';
		
				ptr_tmp=strip_head_tail_space(ptr_tmp);
				if(strlen(ptr_tmp)<64)
				{
					strncpy(global_dial_vars.imsi,ptr_tmp,strlen(ptr_tmp));
					log_info("imsi:%s,%d\n",ptr_tmp,__LINE__);
					memcpy(tempString,ptr_tmp, 5);
					tempString[5] = '\0';
					if( !strncmp(tempString,"46000",5)  	|| 	!strncmp(tempString,"46002",5)  ||
					    !strncmp(tempString,"46004",5)  	||  	!strncmp(tempString,"46007",5) )
					{
						memcpy(global_dial_vars.operator_current, "China Mobile",strlen("China Mobile"));
					}
					else if( !strncmp(tempString,"46003",5)  	|| 	!strncmp(tempString,"46005",5) ||
					    !strncmp(tempString,"46011",5))
					{
						memcpy(global_dial_vars.operator_current, "China Telecom", strlen("China Telecom"));
					}
					else if( !strncmp(tempString,"46020",5) )
					{
						memcpy(global_dial_vars.operator_current, "China TieTong", strlen("China TieTong"));
					}
					else if( !strncmp(tempString,"46001",5) )
					{
						memcpy(global_dial_vars.operator_current, "China Unicom", strlen("China Unicom"));
					}
					*Dial_proc_state=AR9500_DIALST_CRC1;
				}
			}				
			break;
			
		case AR9500_DIALST_CRC1:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = AR9500_DIALST_CREG2;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = AR9500_DIALST_CREG2;
			break;
			
		case AR9500_DIALST_CREG2:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = AR9500_DIALST_CGREG2;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = AR9500_DIALST_CGREG2;
			break;

		case AR9500_DIALST_CGREG2:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = AR9500_DIALST_CEREG2;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = AR9500_DIALST_CEREG2;
			break;

		case AR9500_DIALST_CEREG2:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = AR9500_DIALST_MODODR;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = AR9500_DIALST_MODODR;
			break;

		case AR9500_DIALST_MODODR:
			*Dial_proc_state=AR9500_DIALST_COPS3;
			break;
			
		case AR9500_DIALST_COPS3:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = AR9500_DIALST_SYSINFO;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = AR9500_DIALST_SYSINFO;
			global_dialtool.Dial_Lvl_1=DIAL_DIAL;
			break;
		case AR9500_State_CFUN_DISABLE:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				*Dial_proc_state=AR9500_State_CFUN_ENABLE;
				break;
			}
			break;
		case AR9500_State_CFUN_ENABLE:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				sleep(3);
				*Dial_proc_state=AR9500_DIALST_INIT;
				break;
			}
			break;
		default:
			break;
	}
}



void ar9500_dial(int* Dial_proc_state )
{
	static int exception_count = 0;
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
	switch(*Dial_proc_state)
	{
		case AR9500_DIALST_SYSINFO:  //if  connect net ok
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)&& NULL!=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:"))
			{
				int allow_exception_times = 20;
				int srv_status,srv_domain;
				int roam_status,sys_mode,sim_state;
				char** result_useful=(char**)m_malloc_two(5,8);
				log_info(">>>>>>>>>>>>>>>>%s_%d\n",__FUNCTION__,__LINE__);
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:");
				if(NULL != ptr_tmp)
					ptr_tmp=ptr_tmp+strlen("^SYSINFO:");
				char* ptr_tmp_1=strstr(ptr_tmp,CMD_EXE_OK);
				if(NULL!=ptr_tmp_1)
					ptr_tmp_1[0]='\0';
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				separate_str(ptr_tmp,",",result_useful);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				srv_status=atoi(result_useful[0]);
				srv_domain=atoi(result_useful[1]);
				roam_status=atoi(result_useful[2]);
				sys_mode=atoi(result_useful[3]);
				sim_state=atoi(result_useful[4]);
				free_two(result_useful,5,8);
				global_dial_vars.service_status=srv_status;
				global_dial_vars.ps_cs_region=srv_domain;
				if(sim_state == 255 )
				{
					global_dial_vars.is_sim_exist=0;
				}
				else
				{
					global_dial_vars.is_sim_exist=1;
					if(roam_status == 1)
						global_dial_vars.roam_status=1;
					else
						global_dial_vars.roam_status=0;
					if(sys_mode != 0 && srv_status!=0 && srv_status!=4 && srv_domain!=0)
					{
						*Dial_proc_state=AR9500_DIALST_PSRAT;
						exception_count = 0;
						break;
					}
					else
					{
						sleep(2);
					}
				}
				exception_count++;
				if( !(allow_exception_times = getTmpFileValue("/tmp/.sysinfoTime")))
					allow_exception_times = 20;
				if(exception_count>allow_exception_times)
				{
					global_dialtool.Dial_Lvl_1=DIAL_INIT;
					global_dial_vars.network_link=0;
					exception_count=0;
					*Dial_proc_state=AR9500_State_CFUN_DISABLE;
				}
			}
			break;
		case AR9500_DIALST_PSRAT:	// check the type of network
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				char *network_type_string=strstr(global_dialtool.buffer_at_sponse,"+PSRAT:");
				if(NULL!= network_type_string)
					network_type_string=strip_head_tail_space(network_type_string+strlen("+PSRAT:"));
				else
				{
					break;
				}
				/*
				LTE
				TDS
				HSPA+
				HSUPA
				HSDPA
				HSPA
				HSDPA+
				DC HSDPA+
				UMTS
				HDR RevA
				HDR RevB
				HDR Rev0
				GPRS
				EDGE
				GSM
				1x
				NONE
				
				+PSRAT: HDR - EMPA EHRPD
				
				+NWTYPEIND:31 //"no service",
				+NWTYPEIND:32 //"network:gsm",
				+NWTYPEIND:33 //"network:gprs",
				+NWTYPEIND:34 //"network:edge",
				+NWTYPEIND:35 //"network:wcdma",
				+NWTYPEIND:36 //"network:hsdpa",
				+NWTYPEIND:37 //"network:hsupa"
				+NWTYPEIND:38 //"network (hsdpa_plus)"
				+NWTYPEIND:39 //"network (td_scdma)"
				+NWTYPEIND:40 //"network (lte_fdd)"
				+NWTYPEIND:41 //"network (lte_tdd)"
				+NWTYPEIND:42 //"network (EVDO)"
				+NWTYPEIND:43 //CDMA
				+NWTYPEIND:44 //CDMA&EVDO
				+NWTYPEIND:45 //EVDO
				*/
				if( A_CMP_B_ACCORDING_B( network_type_string,"TD-LTE" ) )
				{
					global_dial_vars.network_mode_bmrat=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//TDD LTE
				else if(A_CMP_B_ACCORDING_B( network_type_string,"TDD" ) )
				{
					global_dial_vars.network_mode_bmrat=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//FDD LTE
				//FDD-LTE
				else if( A_CMP_B_ACCORDING_B( network_type_string,"FDD" ) )
				{
					global_dial_vars.network_mode_bmrat=40;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//LTE
				//LTE TDD
				//LTE FDD
				else if( A_CMP_B_ACCORDING_B( network_type_string,"LTE" ) )
				{
					global_dial_vars.network_mode_bmrat=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status == 1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//TDSCDMA
				else if( A_CMP_B_ACCORDING_B( network_type_string,"TDS" ) )
				{
					global_dial_vars.network_mode_bmrat=39;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				//HSPA+
				//HSUPA
				//HSDPA
				//HSPA
				//HSDPA+
				else if( A_CMP_B_ACCORDING_B( network_type_string,"HS" ) )
				{
					global_dial_vars.network_mode_bmrat=38;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"DC HSDPA+" ) )
				{
					global_dial_vars.network_mode_bmrat=36;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"WCDMA" ) )
				{
					global_dial_vars.network_mode_bmrat=35;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"UMTS" ) )
				{
					global_dial_vars.network_mode_bmrat=35;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"EDGE" ) )
				{
					global_dial_vars.network_mode_bmrat=33;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"GPRS" ) )
				{
					global_dial_vars.network_mode_bmrat=33;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"GSM" ) )
				{
					global_dial_vars.network_mode_bmrat=32;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( strstr( network_type_string,"EHRPD" ) )
				{
					global_dial_vars.network_mode_bmrat=44;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"EVDO" ) )
				{
					global_dial_vars.network_mode_bmrat=45;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				//HDR RevA
				//HDR RevB
				//HDR Rev0
				else if( A_CMP_B_ACCORDING_B( network_type_string,"HDR" ) )
				{
					global_dial_vars.network_mode_bmrat=45;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				//CDMA
				//CDMA&EVDO
				else if( A_CMP_B_ACCORDING_B( network_type_string,"CDMA" ) )
				{
					global_dial_vars.network_mode_bmrat=43;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"1x" ) )
				{
					global_dial_vars.network_mode_bmrat=43;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"NONE" ) )
				{
					global_dial_vars.network_mode_bmrat=31;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=-1;
				}
				else
				{
					global_dial_vars.network_mode_bmrat=32;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}

				char * ptr_tmp=strstr(network_type_string,"OK");
				if(NULL != ptr_tmp)
				{
					ptr_tmp[0]='\0';
					char * ptr_tmp1=strip_head_tail_space(network_type_string);
//					printf("network:%s %s\n",ptr_tmp1,network_type_string);
					strlcpy(global_dial_vars.network_mode_detail,ptr_tmp1,strlen(ptr_tmp1)+1);
				}
				//sometimes have no signal,and we search nothing,but we must be sure wihich network we are on 
				if(global_dial_vars.network_mode_bmrat!=31)
				{
					*Dial_proc_state=AR9500_DIALST_CSQ;
					exception_count=0;
				}
				else
					exception_count++;;
				log_info("networktype:%s %d\n",network_type_string,global_dial_vars.evdo_cdma_flag);
				if(exception_count>60)
				{
					global_dialtool.Dial_Lvl_1=DIAL_DIAL;
					global_dial_vars.network_link=0;
					exception_count=0;
					*Dial_proc_state=AR9500_DIALST_SYSINFO;
				}
			}
			break;
		case AR9500_DIALST_CSQ:		//get signal num
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+CSQ:"))
			{
				double rssi;
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+CSQ:")+strlen("+CSQ:");
				char* tmp_ptr_1=strstr(tmp_ptr,CMD_EXE_OK);
				if(NULL!=tmp_ptr_1)
					tmp_ptr_1[0]='\0';
				char** result_useful=(char**)m_malloc_two(2,8);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				separate_str(tmp_ptr,",",result_useful);
				rssi=atoi(result_useful[0]);
				free_two(result_useful,2,8);
				snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"%0.f",rssi);

				//supplier provide 
				if(rssi >= 26 && rssi <= 31)
					global_dial_vars.signal_rssi_level=5;
				else if(rssi >= 22 && rssi <= 25)
					global_dial_vars.signal_rssi_level=4;
				else if(rssi >= 16 && rssi <= 21)
					global_dial_vars.signal_rssi_level=3;
				else if(rssi >= 10 && rssi <= 15)
					global_dial_vars.signal_rssi_level=2;
				else if(rssi >= 1 && rssi <= 9)
					global_dial_vars.signal_rssi_level=1;
				else
					global_dial_vars.signal_rssi_level=0;
				
				global_dialtool.Dial_Lvl_1=DIAL_DEAMON;
				*Dial_proc_state=AR9500_DIALST_CHECKPPP0;	
			}
			break;
	}	
}

void ar9500_306a_deamon(int* Dial_proc_state)
{
	static int exception_count = 0;
	static int checkpppTime = 0;
	int network_mode = 0;
	//printf("%s %d, global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__, global_dialtool.Dial_proc_state);
	switch(*Dial_proc_state)
	{
		case AR9500_DIALST_SYSINFO:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)&& NULL!=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:"))
			{
				int allow_exception_times = 20;
				int srv_status,srv_domain;
				int roam_status,sys_mode,sim_state;
				char** result_useful=(char**)m_malloc_two(5,8);
				log_info(">>>>>>>>>>>>>>>>%s_%d\n",__FUNCTION__,__LINE__);
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:");
				if(NULL != ptr_tmp)
					ptr_tmp=ptr_tmp+strlen("^SYSINFO:");
				char* ptr_tmp_1=strstr(ptr_tmp,CMD_EXE_OK);
				if(NULL!=ptr_tmp_1)
					ptr_tmp_1[0]='\0';
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				separate_str(ptr_tmp,",",result_useful);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				srv_status=atoi(result_useful[0]);
				srv_domain=atoi(result_useful[1]);
				roam_status=atoi(result_useful[2]);
				sys_mode=atoi(result_useful[3]);
				sim_state=atoi(result_useful[4]);
				global_dial_vars.service_status=srv_status;
				global_dial_vars.ps_cs_region=srv_domain;
				if(sim_state == 255 )
				{
					global_dial_vars.is_sim_exist=0;
				}
				else
				{
					global_dial_vars.is_sim_exist=1;
					if(roam_status == 1)
						global_dial_vars.roam_status=1;
					else
						global_dial_vars.roam_status=0;
					if(sys_mode != 0 && srv_status!=0 && srv_status!=4 && srv_domain!=0)
					{
						*Dial_proc_state=AR9500_DIALST_LCTCELLINFO;
						break;
					}
					else
					{
						checkpppTime = 0;
						sleep(2);
					}
				}
				exception_count++;
				free_two(result_useful,5,8);
				if( !(allow_exception_times = getTmpFileValue("/tmp/.sysinfoTime")))
					allow_exception_times = 20;
				if(exception_count>allow_exception_times)
				{
					global_dialtool.Dial_Lvl_1=DIAL_INIT;
					global_dial_vars.network_link=0;
					exception_count=0;
					*Dial_proc_state=AR9500_State_CFUN_DISABLE;
					checkpppTime = 0;
				}
			}
			break;

		case AR9500_DIALST_LCTCELLINFO:
			*Dial_proc_state=AR9500_DIALST_LSCELLINFO;
			log_info("%s_%d,LCTCELLINFO =\n %s\n",__FUNCTION__,__LINE__,global_dialtool.buffer_at_sponse);
			break;
		case AR9500_DIALST_LSCELLINFO:
			*Dial_proc_state=AR9500_DIALST_PSRAT;
			log_info("%s_%d,LCTCELLINFO =\n %s\n",__FUNCTION__,__LINE__,global_dialtool.buffer_at_sponse);
			break;
		case AR9500_DIALST_PSRAT:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				char *network_type_string=strstr(global_dialtool.buffer_at_sponse,"+PSRAT:");
				if(NULL!= network_type_string)
					network_type_string=strip_head_tail_space(network_type_string+strlen("+PSRAT:"));
				else
				{
					break;
				}
				/*
				LTE
				TDS
				HSPA+
				HSUPA
				HSDPA
				HSPA
				HSDPA+
				DC HSDPA+
				UMTS
				HDR RevA
				HDR RevB
				HDR Rev0
				GPRS
				EDGE
				GSM
				1x
				NONE
				
				+PSRAT: HDR - EMPA EHRPD
				
				+NWTYPEIND:31 //"no service",
				+NWTYPEIND:32 //"network:gsm",
				+NWTYPEIND:33 //"network:gprs",
				+NWTYPEIND:34 //"network:edge",
				+NWTYPEIND:35 //"network:wcdma",
				+NWTYPEIND:36 //"network:hsdpa",
				+NWTYPEIND:37 //"network:hsupa"
				+NWTYPEIND:38 //"network (hsdpa_plus)"
				+NWTYPEIND:39 //"network (td_scdma)"
				+NWTYPEIND:40 //"network (lte_fdd)"
				+NWTYPEIND:41 //"network (lte_tdd)"
				+NWTYPEIND:42 //"network (EVDO)"
				+NWTYPEIND:43 //CDMA
				+NWTYPEIND:44 //CDMA&EVDO
				+NWTYPEIND:45 //EVDO
				*/
				if( A_CMP_B_ACCORDING_B( network_type_string,"TD-LTE" ) )
				{
					network_mode=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//TDD LTE
				else if(A_CMP_B_ACCORDING_B( network_type_string,"TDD" ) )
				{
					network_mode=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//FDD LTE
				//FDD-LTE
				else if( A_CMP_B_ACCORDING_B( network_type_string,"FDD" ) )
				{
					network_mode=40;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//LTE
				//LTE TDD
				//LTE FDD
				else if( A_CMP_B_ACCORDING_B( network_type_string,"LTE" ) )
				{
					network_mode=41;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					if(global_dial_vars.roam_status == 1)
						global_dial_vars.ereg_status=5;
					else
						global_dial_vars.ereg_status=1;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=7;
				}
				//TDSCDMA
				else if( A_CMP_B_ACCORDING_B( network_type_string,"TDS" ) )
				{
					network_mode=39;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				//HSPA+
				//HSUPA
				//HSDPA
				//HSPA
				//HSDPA+
				else if( A_CMP_B_ACCORDING_B( network_type_string,"HS" ) )
				{
					network_mode=38;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"DC HSDPA+" ) )
				{
					network_mode=36;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"WCDMA" ) )
				{
					network_mode=35;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"UMTS" ) )
				{
					network_mode=35;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"EDGE" ) )
				{
					network_mode=33;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"GPRS" ) )
				{
					network_mode=33;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"GSM" ) )
				{
					network_mode=32;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}
				else if( strstr( network_type_string,"EHRPD" ) )
				{
					network_mode=44;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"EVDO" ) )
				{
					network_mode=45;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				//HDR RevA
				//HDR RevB
				//HDR Rev0
				else if( A_CMP_B_ACCORDING_B( network_type_string,"HDR" ) )
				{
					network_mode=45;
					global_dial_vars.reg_status=0;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.greg_status=5;
					else
						global_dial_vars.greg_status=1;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=2;
				}
				//CDMA
				//CDMA&EVDO
				else if( A_CMP_B_ACCORDING_B( network_type_string,"CDMA" ) )
				{
					network_mode=43;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"1x" ) )
				{
					network_mode=43;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=1;
					global_dial_vars.act=0;
				}
				else if( A_CMP_B_ACCORDING_B( network_type_string,"NONE" ) )
				{
					network_mode=31;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=-1;
				}
				else
				{
					network_mode=32;
					if(global_dial_vars.roam_status==1)
						global_dial_vars.reg_status=5;
					else
						global_dial_vars.reg_status=1;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=0;
				}

				char * ptr_tmp=strstr(network_type_string,"OK");
				if(NULL != ptr_tmp)
				{
					ptr_tmp[0]='\0';
					char * ptr_tmp1=strip_head_tail_space(network_type_string);
//					printf("network:%s %s\n",ptr_tmp1,network_type_string);
					strlcpy(global_dial_vars.network_mode_detail,ptr_tmp1,strlen(ptr_tmp1)+1);
				}
				//sometimes have no signal,and we search nothing,but we must be sure wihich network we are on 
				if(network_mode!=31)
				{
					*Dial_proc_state=AR9500_DIALST_CSQ;
					exception_count=0;
				}
				else
					exception_count++;;
				if( network_mode  != global_dial_vars.network_mode_bmrat )
				{
					checkpppTime = 0;
				}
				log_info("networktype:%s %d\n",network_type_string,global_dial_vars.evdo_cdma_flag);
				if(exception_count>10)
				{
					global_dialtool.Dial_Lvl_1=DIAL_DIAL;
					global_dial_vars.network_link=0;
					exception_count=0;
					*Dial_proc_state=AR9500_DIALST_SYSINFO;
					checkpppTime = 0;
				}
			}
			break;
		case AR9500_DIALST_CSQ:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+CSQ:"))
			{
				double rssi;
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+CSQ:")+strlen("+CSQ:");
				char* tmp_ptr_1=strstr(tmp_ptr,CMD_EXE_OK);
				if(NULL!=tmp_ptr_1)
					tmp_ptr_1[0]='\0';
				char** result_useful=(char**)m_malloc_two(2,8);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				separate_str(tmp_ptr,",",result_useful);
				rssi=atoi(result_useful[0]);
				free_two(result_useful,2,8);
				snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"%0.f",rssi);

				//supplier provide 
				if(rssi >= 26 && rssi <= 31)
					global_dial_vars.signal_rssi_level=5;
				else if(rssi >= 22 && rssi <= 25)
					global_dial_vars.signal_rssi_level=4;
				else if(rssi >= 16 && rssi <= 21)
					global_dial_vars.signal_rssi_level=3;
				else if(rssi >= 10 && rssi <= 15)
					global_dial_vars.signal_rssi_level=2;
				else if(rssi >= 1 && rssi <= 9)
					global_dial_vars.signal_rssi_level=1;
				else
					global_dial_vars.signal_rssi_level=0;
				
				//global_dialtool.Dial_Lvl_1=DIAL_DEAMON;
				*Dial_proc_state=AR9500_DIALST_CHECKPPP0;
				sleep(5);
			}
			break;
		case AR9500_DIALST_CHECKPPP0:
			if(checkpppTime-- <=  0 )
			{
				system("/etc/rc.d/a9500_check-nb.sh");
				checkpppTime = 4;
			}
			*Dial_proc_state=AR9500_DIALST_SYSINFO;	
			sleep(5);
			break;
	}
}

void ar9500_306a_timer(int* Dial_proc_state)
{
/*
	static int loop = 0;
	loop ++;
	//printf("%s %d, global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__, global_dialtool.Dial_proc_state);
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
	switch(*Dial_proc_state)
	{
		case AR9500_DIALST_CHECKPPP0:
			if( loop % 20 == 1)
			{
				system("/etc/rc.d/check-nb.sh");
				log_info("global_dial_vars.network_mode_bmrat = %d \n",global_dial_vars.network_mode_bmrat);
				log_info("global_dial_vars.reg_status = %d \n",global_dial_vars.reg_status);
				log_info("global_dial_vars.greg_status = %d \n",global_dial_vars.greg_status);
				log_info("global_dial_vars.ereg_status = %d \n",global_dial_vars.ereg_status);
				log_info("tempString = %s\n",tempString);
			}
			break;
	}
*/
}


void ar9500_report_handle(char* buffer)
{
	//printf("%s %d\n",__FUNCTION__,__LINE__);
}


DialProc process_ar9500=
{
	ar9500_306a_init,
	ar9500_dial,
	ar9500_306a_deamon,
	ar9500_sendat,
	ar9500_report_handle,
	ar9500_306a_timer,
};





#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "common.h"
#include "sim7000.h"
#include "include.h"


#define SIM7000C_DIALST_INIT   0
#define SIM7000C_DIALST_CMEE2 1
#define SIM7000C_DIALST_CRC1  2
#define SIM7000C_DIALST_CREG2  3
#define SIM7000C_DIALST_CGREG2  4
#define SIM7000C_DIALST_CEREG2  5
#define SIM7000C_DIALST_COPS3  6


#define SIM7000C_DIALST_CPIN  100

#define SIM600C_DIALST_CHECKPPP0 300

#define SIM7000C_DIALST_IDLE 999


void sim7000c_get_moduleinfo(MDI* p)
{
	char *buffer_recv=NULL;

	int length;
	char buffer[1024]={0};
	char* str_ptr=NULL;
	char* str_sub_ptr=NULL;
	char* result_ptr=NULL;

	buffer_recv=(char *)m_malloc(RECV_BUFF_SIZE);
		
howitpossible:
	util_send_cmd(global_dialtool.dev_handle,"ATE1\r",&global_dialtool.pthread_moniter_flag);
	sleep(1);
	util_send_cmd(global_dialtool.dev_handle,"ATI\r",&global_dialtool.pthread_moniter_flag);
	sleep(1);
	util_send_cmd(global_dialtool.dev_handle,"AT+GSN\r",&global_dialtool.pthread_moniter_flag);
	sleep(2);
	read(global_dialtool.dev_handle,buffer_recv,RECV_BUFF_SIZE);
	log_info(buffer_recv);

	str_ptr=strstr(buffer_recv,"SIM7000");
	if(NULL!=str_ptr)
	{
		str_sub_ptr=strstr(str_ptr,"\n");
		if(NULL != str_sub_ptr)
		{
			length=str_sub_ptr-str_ptr;
			strncpy(buffer,str_ptr,length);
			buffer[length]='\0';
			result_ptr=strip_head_tail_space(buffer);
			strncpy(p->manufacturer,result_ptr,strlen(result_ptr));
			strcpy(p->hardver, "sim7000c");
			log_info("get ati:%s \n",p->manufacturer);
			//printf("get ati:");
			//printf(p->manufacturer);
			//printf("\n");
		}
	}
	str_ptr=strstr(buffer_recv,"AT+GSN");
	if(NULL!=str_ptr)
	{
		str_ptr += 9;
		str_sub_ptr=strstr(str_ptr,"\n");
		if(NULL != str_sub_ptr)
		{
			length=str_sub_ptr-str_ptr;
			strncpy(buffer,str_ptr,length);
			buffer[length]='\0';
			result_ptr=strip_head_tail_space(buffer);
			strncpy(p->imei,result_ptr,strlen(result_ptr));
			log_info("get imei:%s \n",p->imei);
			//printf("get imei");
			//printf(p->imei);
			//printf("\n");
		}
	}

	if(p->imei[0] == '\0')
		goto howitpossible;
	m_free(buffer_recv);

}


MDI sim7000c_moduleinfo=
{
	{TZ_VENDOR_SIM7000C, TZ_PRODUCT_SIM7000C},
	"/dev/ttyUSB2",
	sim7000c_get_moduleinfo,
	"0",
	"0",
	"0",
	"0",
	"\0",
	"\0",
};


void sim7000c_sendat(int num)
{
	//printf("%s %d, global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__, global_dialtool.Dial_proc_state);
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
	switch(global_dialtool.Dial_proc_state)
	{
		case SIM7000C_DIALST_INIT:
			util_send_cmd(global_dialtool.dev_handle,"ATE0\r",&global_dialtool.pthread_moniter_flag);			//test AT
			break;
		case SIM7000C_DIALST_CMEE2:
			util_send_cmd(global_dialtool.dev_handle,"AT+CMEE=2\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case SIM7000C_DIALST_CRC1:
			util_send_cmd(global_dialtool.dev_handle,"AT+CRC=1\r",&global_dialtool.pthread_moniter_flag);		//report ERROR
			break;
		case SIM7000C_DIALST_CGREG2:
			util_send_cmd(global_dialtool.dev_handle,"AT+CGREG=2\r",&global_dialtool.pthread_moniter_flag);		//report ERROR
			break;
		case SIM7000C_DIALST_CEREG2:
			util_send_cmd(global_dialtool.dev_handle,"AT+CEREG=2\r",&global_dialtool.pthread_moniter_flag); 	//report ERROR
			break;
		case SIM7000C_DIALST_CREG2:
			util_send_cmd(global_dialtool.dev_handle,"AT+CREG=2\r",&global_dialtool.pthread_moniter_flag); 	//report ERROR
			break;
		case SIM7000C_DIALST_COPS3:
			util_send_cmd(global_dialtool.dev_handle,"AT+COPS=3,2\r",&global_dialtool.pthread_moniter_flag);	//report ERROR
			break;
		case SIM7000C_DIALST_CPIN:
			util_send_cmd(global_dialtool.dev_handle,"AT+CPIN?\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
		default:
			break;
	}
}



void sim7000c_306a_init(int*  Dial_proc_state)
{
	//printf("%s %d, global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__, global_dialtool.Dial_proc_state);
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
		//init led
	switch(*Dial_proc_state)
	{
		//write query function is  troublesome  ,maybe it not safe only judge by OK or ERROR,i just try it
		case SIM7000C_DIALST_INIT:
			*Dial_proc_state=SIM7000C_DIALST_CMEE2;
			break;

		case SIM7000C_DIALST_CMEE2:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = SIM7000C_DIALST_CRC1;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = SIM7000C_DIALST_CRC1;
			break;
			
		case SIM7000C_DIALST_CRC1:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = SIM7000C_DIALST_CREG2;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = SIM7000C_DIALST_CREG2;
			break;
			
		case SIM7000C_DIALST_CREG2:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = SIM7000C_DIALST_CGREG2;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = SIM7000C_DIALST_CGREG2;
			break;

		case SIM7000C_DIALST_CGREG2:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = SIM7000C_DIALST_CEREG2;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = SIM7000C_DIALST_CEREG2;
			break;

		case SIM7000C_DIALST_CEREG2:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = SIM7000C_DIALST_COPS3;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = SIM7000C_DIALST_COPS3;
			break;
			
		case SIM7000C_DIALST_COPS3:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state = SIM7000C_DIALST_CPIN;
			else if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				*Dial_proc_state = SIM7000C_DIALST_CPIN;
			break;
				
		case SIM7000C_DIALST_CPIN:
			global_dial_vars.pin_qpin_flag=0;		//set flag ,so we can choose which cmd to input pin 
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
				NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_READY))
			{
				global_dial_vars.is_sim_exist=1;
				global_dial_vars.device_lock_pin=0;
				global_dial_vars.device_lock_puk=0;
				global_dialtool.Dial_Lvl_1=DIAL_DIAL;
				*Dial_proc_state=SIM600C_DIALST_CHECKPPP0;
				break;
			}
			else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
				NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_NEED_PIN))
			{
				global_dial_vars.is_sim_exist=1;
				global_dial_vars.device_lock_pin=1;
				global_dial_vars.device_lock_puk=0;
				*Dial_proc_state=SIM7000C_DIALST_IDLE;
				break;
			}
			else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
				NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_NEED_PUK))
			{
				global_dial_vars.is_sim_exist=1;
				global_dial_vars.device_lock_pin=0;
				global_dial_vars.device_lock_puk=1;
				*Dial_proc_state=SIM7000C_DIALST_IDLE;
				break;
			}
			else
			{
				global_dial_vars.is_sim_exist=0;
				global_dial_vars.device_lock_pin=0;
				global_dial_vars.device_lock_puk=0;
				*Dial_proc_state=SIM7000C_DIALST_IDLE;
				break;
			}
			break;

		default:
			break;
	}
}

void sim7000c_dial(int* Dial_proc_state )
{
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
	switch(*Dial_proc_state)
	{
		case SIM600C_DIALST_CHECKPPP0:
			break;
	}	
}

void sim7000c_306a_deamon(int* Dial_proc_state)
{
	//printf("%s %d, global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__, global_dialtool.Dial_proc_state);
}

void sim7000c_306a_timer(int* Dial_proc_state)
{
	static int loop = 0;
	loop ++;
	//printf("%s %d, global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__, global_dialtool.Dial_proc_state);
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
	switch(*Dial_proc_state)
	{
		case SIM600C_DIALST_CHECKPPP0:
			if( loop % 20 == 1)
			{
				system("/etc/rc.d/check-nb.sh");
			}
			break;
	}
}


void sim7000c_report_handle(char* buffer)
{
	//printf("%s %d\n",__FUNCTION__,__LINE__);
}

DialProc process_sim7000c=
{
	sim7000c_306a_init,
	sim7000c_dial,
	sim7000c_306a_deamon,
	sim7000c_sendat,
	sim7000c_report_handle,
	sim7000c_306a_timer,
};



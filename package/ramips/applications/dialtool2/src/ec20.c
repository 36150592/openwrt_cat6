/*
	This file define all about module ,depend on bm817c at  command set  V1.0.4
*/
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "common.h"
#include "bm806.h"
#include "include.h"
//#include "boardtype_306A.h"


/*reporter indicator define*/
MCT ec20_mode_table[]=
{
	{"China",	"China Mobile",	"46000",	{{3,5},	{0,2},	{13,7},	{18,8},	{20,21}},},
	{"China",	"China Mobile",	"46002",	{{3,5},	{0,2},	{13,7},	{18,8},	{20,21}},},
	{"China",	"China Mobile",	"46004",	{{3,5},	{0,2},	{13,7},	{18,8},	{20,21}},},
	{"China",	"China Mobile",	"46007",	{{3,5},	{0,2},	{13,7},	{18,8},	{20,21}},},
	{"China",	"China Telecom",	"46003",	{{3,5},	{0,11},	{13,10},	{18,14},	{20,15}},},
	{"China",	"China Telecom",	"46005",	{{3,5},	{0,11},	{13,10},	{18,14},	{20,15}},	},
	{"China",	"China Telecom",	"46011",	{{3,5},	{0,11},	{13,10},	{18,14},	{20,15}},},
	{"China",	"China TieTong",	"46020",	{{3,5},	{0,2},	{13,7},	{18,8},	{20,21}},},			//TieTong card can not query,use China Mobile
	{"China",	"China Unicom",	"46001",	{{3,5},	{0,17},	{13,1},	{18,21},	{20,3}},},
//	{"Sri Lanka",	"Airtel",	"41305",	{{3,5},	{0,17},	{13,1},	{18,21},	{20,3}},},
	{"default",	"default",	"default",	{{3,5},	{0,17},	{13,6},	{18,7},	{20,21}},},					//default use China Mobile mode
};
#if 0
MCT bm806_mode_table[]=
{
	{"China",	"China Mobile",	"46000",	{{3,5},	{0,2},	{13,7},	{18,8},	{20,21}},},
	{"China",	"China Mobile",	"46002",	{{3,5},	{0,2},	{13,7},	{18,8},	{20,21}},},
	{"China",	"China Mobile",	"46004",	{{3,5},	{0,2},	{13,7},	{18,8},	{20,21}},},
	{"China",	"China Mobile",	"46007",	{{3,5},	{0,2},	{13,7},	{18,8},	{20,21}},},
	{"China",	"China Telecom",	"46003",	{{3,5},	{0,5},	{13,5},	{18,5},	{20,5}},},
	{"China",	"China Telecom",	"46005",	{{3,5},	{0,5},	{13,5},	{18,5},	{20,5}},},
	{"China",	"China Telecom",	"46011",	{{3,5},	{0,5},	{13,5},	{18,5},	{20,5}},},
	{"China",	"China TieTong",	"46020",	{{3,5},	{0,2},	{13,7},	{18,8},	{20,21}},},			//TieTong card can not query,use China Mobile
	{"China",	"China Unicom",	"46001",	{{3,5},	{0,17},	{13,1},	{18,21},	{20,3}},},
//	{"Sri Lanka",	"Airtel",	"41305",	{{3,5},	{0,17},	{13,1},	{18,21},	{20,3}},},
	{"default",	"default",	"default",	{{3,5},	{0,17},	{13,7},	{18,8},	{20,21}},},					//default use China Mobile mode
};

#endif
static char* ec20_hardware_summary[]=
{
	"EC20",
};


MDI ec20_moduleinfo=
{
	{TZ_VENDOR_EC20_ID,TZ_PRODUCT_EC20},
	"1-1.4:1.2",
	bm806_get_moduleinfo,
	"\0",
	"0",
	"0",
	"0",
	"EC20GobiNet.ko",
	"usb0",
};

static int get_network_mode_module(int network_mode_web,char* module_hardware)
{
	int i,j;
	MCT *p=NULL;
	int len=0;
	for(i=0;i<ARRAY_MEMBER_COUNT(ec20_hardware_summary);i++)
	{
		if(!strncmp(ec20_hardware_summary[i],module_hardware,strlen(ec20_hardware_summary[i])))
			break;
	}
	switch(i)
	{
		default:
			p=ec20_mode_table;
			len=ARRAY_MEMBER_COUNT(ec20_mode_table);
			break;
	}
	for(j=0;j<len;j++)
	{
		if(A_CMP_B_ACCORDING_B(global_dial_vars.imsi,(p+j)->operator_num))
		{
			for(i=0;i<5;i++)
			{
				if(((p+j)->web_mode_table+i)->web_mode == network_mode_web)
					break;					
			}
			break;
		}
	}
	if(j>=len)
	{
		for(i=0;i<5;i++)
		{
			if(((p+len-1)->web_mode_table+i)->web_mode == network_mode_web)
				break;					
		}
		if(i>=5)
		{
			return ((p+len-1)->web_mode_table+1)->real_mode;
//			return FALSE;
		}
		else
		{
			char buffer[8];
			snprintf(buffer,6,"%s",global_dial_vars.imsi);
			strncpy(global_dial_vars.operator_current,buffer,6);
			return ((p+len-1)->web_mode_table+i)->real_mode;
		}
	}
	else
	{
		if(i>=5)
		{
			return ((p+len-1)->web_mode_table+1)->real_mode;
//			return FALSE;
		}
		else
		{
			strncpy(global_dial_vars.operator_current,(p+j)->operator_str,strlen((p+j)->operator_str));
			return ((p+j)->web_mode_table+i)->real_mode;
		}
	}
	
}

void ec20_sendat(int num)
{
	switch(global_dialtool.Dial_proc_state)
	{
		case Dial_State_initialized:
			util_send_cmd(global_dialtool.dev_handle,"ATE0\r",&global_dialtool.pthread_moniter_flag);			//test AT
			break;
		case Dial_State_CMEE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CMEE=1\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case Dial_State_ICCID:
			util_send_cmd(global_dialtool.dev_handle,"AT+ICCID\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CPIN_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+CPIN?\r",&global_dialtool.pthread_moniter_flag);
			break;			
		case Dial_State_QCPIN_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+QCPIN?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_ATI:
			util_send_cmd(global_dialtool.dev_handle,"ATI\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMSIMTYPE:
			util_send_cmd(global_dialtool.dev_handle,"AT+bmsimtype?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CPIN_SET:
			{
				char* pin_code=global_init_parms.pin;
				char cmd_buffer[64];
				if(global_dial_vars.device_lock_pin)
				{
					if(strlen(pin_code))
					{
						snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+CPIN=%s\r",pin_code);
						util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
					}
				}
				break;
			}
		case Dial_State_QCPIN_SET:
			{
				char* pin_code=global_init_parms.pin;
				char cmd_buffer[64];
				if(global_dial_vars.device_lock_pin)
				{
					if(strlen(pin_code))
					{
						snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+QCPIN=%s\r",pin_code);
						util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
					}
				}
				break;
			}
		case Dial_State_CIMI:
			util_send_cmd(global_dialtool.dev_handle,"AT+CIMI\r",&global_dialtool.pthread_moniter_flag);  //mode 3gpp2 can use also
			break;
		case Dial_State_QCIMI:
			util_send_cmd(global_dialtool.dev_handle,"AT+QCIMI\r",&global_dialtool.pthread_moniter_flag);  //mode 3gpp2 can use also
			break;			
		case Dial_State_CPUK_SET:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CLCK_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CLCK_ENABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CPWD:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMDATASTATUSEN:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMDATASTATUSEN=1\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_PSDIALIND:
			util_send_cmd(global_dialtool.dev_handle,"AT+PSDIALIND=1\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMTCELLINFO:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMTCELLINFO\r",&global_dialtool.pthread_moniter_flag);
			break;	
		case Dial_State_CFUN_ENABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN=1\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CFUN_DISABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN=0\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CFUN_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMMODODR:
			{
				char cmd_buffer[64];
				int network_mode_module=get_network_mode_module(global_init_parms.network_mode_web,global_system_info.module_info.hardver);
				if(FALSE == network_mode_module)
				{
					log_error("module dont support the network mode\n");
					write_str_file(DIAL_INDICATOR,"module don't support the network mode","w+");
					exit(-1);					
				}
				snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BMMODODR=%d\r",network_mode_module);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_BMMODODR_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMMODODR?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGDCONT:
			{
				char cmd_buffer[64];
				snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+CGDCONT=1,\"%s\",\"%s\",,0,0\r",global_init_parms.ipstack,global_init_parms.apn);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_CGDCONT2:
			{
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				char cmd_buffer[256];
				int ipstack=2;

				if( !strncmp( global_init_parms.ipstack,"IPV4V6",6 ) )
				{
					ipstack=2;
				}
				else if( !strncmp( global_init_parms.ipstack,"IPV6",4 ) )
				{
					ipstack=1;
				}
				else if( !strncmp( global_init_parms.ipstack,"IP",2) )
				{
					ipstack=0;
				}
				if(A_CMP_B_ACCORDING_B(global_init_parms.apn,"0"))
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BM3GPP2CGDCONT=0,3,ctnet@mycdma.cn,vnet.mobi,ctnet,2,3,0\r");
				else
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BM3GPP2CGDCONT=0,%s,%s,%s,%s,%d,3,0\r",
						global_init_parms.ppp_auth_type,global_init_parms.ppp_username,
						global_init_parms.ppp_password,global_init_parms.apn,ipstack);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_QCPDPP:
			{
				char cmd_buffer[64];
				if(A_CMP_B_ACCORDING_B(global_init_parms.ppp_username,"0") || 
					A_CMP_B_ACCORDING_B(global_init_parms.ppp_auth_type,"0") ||
					A_CMP_B_ACCORDING_B(global_init_parms.ppp_password,"0"))
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT\r");
				else
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT$QCPDPP=1,%s,\"%s\",\"%s\"\r",
						global_init_parms.ppp_auth_type,global_init_parms.ppp_password,global_init_parms.ppp_username);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_BM3GPP2AUTHINFO:
			{
				char cmd_buffer[256];
				if(A_CMP_B_ACCORDING_B(global_init_parms.ppp_username,"0") || 
					A_CMP_B_ACCORDING_B(global_init_parms.ppp_password,"0"))
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT\r");
				else
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BM3GPP2AUTHINFO=%s,%s\r",
						global_init_parms.ppp_username,global_init_parms.ppp_password);
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_BMBANDPREF_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMBANDPREF?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMBANDPREF:
			{
				char cmd_buffer[256];
				char str[12];
				char* lte_band_lock=global_init_parms.lte_band_choose;
				if(strlen(lte_band_lock)<11)
				{
					snprintf(str,sizeof(str),"%0*d%s",11-strlen(lte_band_lock),0,lte_band_lock);
				}
				else
				{
					snprintf(str,sizeof(str),"%s",lte_band_lock);
				}
				lte_band_lock=str;
				log_info("lte_lock_band:%s\n",lte_band_lock);
				snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+BMBANDPREF=%s,%s,%s\r",
					"0002000005e80380",lte_band_lock,"00000021");
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_BMRAT:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMRAT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_COPS_SET:
			{
				char cmd_buffer[64];
				if(global_init_parms.plmn_mode == PLMN_SELECTION_MODE_MANUAL && strlen(global_init_parms.plmn) != 0)
				{
					if(global_init_parms.plmn_act != -1 )
						snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+COPS=1,2,\"%s\",%d\r",global_init_parms.plmn,global_init_parms.plmn_act);
					else
						snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+COPS=1,2,\"%s\"\r",global_init_parms.plmn);
				}
				else
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+COPS=0\r");
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_QCRMCALL:
			{
				char cmd_buffer[64];
				if(global_dial_vars.evdo_cdma_flag!=0)
				{
					if(!strncmp("IP",global_init_parms.ipstack,strlen(global_init_parms.ipstack)))
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,1,1,1,0\r");
					else if(!strncmp("IPV6",global_init_parms.ipstack,strlen(global_init_parms.ipstack)))
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,2,1,1,0\r");
					else
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,3,1,1,0\r");
				}
				else
				{
					if(!strncmp("IP",global_init_parms.ipstack,strlen(global_init_parms.ipstack)))
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,1,2,1\r");
					else if(!strncmp("IPV6",global_init_parms.ipstack,strlen(global_init_parms.ipstack)))
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,2,2,1\r");
					else
						snprintf(cmd_buffer,sizeof(cmd_buffer ),"AT$QCRMCALL=1,1,3,2,1\r"); //occur a mistake
				}
				util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				break;
			}
		case Dial_State_QCRMCALL_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT$QCRMCALL?\r",&global_dialtool.pthread_moniter_flag);
			break;	
		case Dial_State_CPMS_SM:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGL_SM:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CPMS_ME:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGL_ME:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGD:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGL:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_SYSINFO:
			util_send_cmd(global_dialtool.dev_handle,"AT^SYSINFO\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_COPS_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+COPS?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CSCS:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CSCS_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CNMI:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGSMS:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CSCA_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CSCA_SET:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGF:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGF_DM:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;

		case Dial_State_CMGS:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CMGS_DM:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGEQREQ:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGACT_ENABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGCONTRDP:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CGACT_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_SIGNALIND:
			util_send_cmd(global_dialtool.dev_handle,"AT+SIGNALIND=1\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_QCRMCALL_DISCONNECT:
			util_send_cmd(global_dialtool.dev_handle,"AT$QCRMCALL=0,1\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_CSQ:
			util_send_cmd(global_dialtool.dev_handle,"AT+CSQ\r",&global_dialtool.pthread_moniter_flag);
			break;	
		case Dial_State_CSQ_SET:
			util_send_cmd(global_dialtool.dev_handle,"AT+CSQ=1\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_HDRCSQ:
			util_send_cmd(global_dialtool.dev_handle,"AT^HDRCSQ\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMDATASTATUS:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMDATASTATUS\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Dial_State_BMCPNCNT:
			util_send_cmd(global_dialtool.dev_handle,"AT+BMCPNCNT\r",&global_dialtool.pthread_moniter_flag);
			break;	
		default:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
	}	
}

void ec20_init(int*  Dial_proc_state)
{
	log_info("%s %d\n",__FUNCTION__,__LINE__);
		//init led
	switch(*Dial_proc_state)
	{
		//write query function is  troublesome  ,maybe it not safe only judge by OK or ERROR,i just try it
		case Dial_State_initialized:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
					*Dial_proc_state=Dial_State_CMEE;
				break;
		case Dial_State_CMEE:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Dial_State_BMBANDPREF_QUERY;
				}
				break;
		case Dial_State_BMBANDPREF_QUERY:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+BMBANDPREF:");
					if(NULL !=ptr_tmp)
					{
						char* ptr_tmp1=NULL;
						ptr_tmp1=strstr(global_dialtool.buffer_at_sponse,"+BMBANDPREF:")+strlen("+BMBANDPREF:");
						char* ptr_tmp3=strstr(ptr_tmp1,",");
						if(NULL!= ptr_tmp3)
							ptr_tmp3=ptr_tmp3+1;
						char* ptr_tmp4=strstr(ptr_tmp3,",");
						if(NULL!=ptr_tmp4)
							ptr_tmp4[0]='\0';
						ptr_tmp=strip_head_tail_space(ptr_tmp3);
						if(strlen(ptr_tmp)<64)
							strncpy(global_dial_vars.lte_lock_band,ptr_tmp,strlen(ptr_tmp));
						if(global_init_parms.lte_band_choose[0]=='\0')
							*Dial_proc_state=Dial_State_BMDATASTATUSEN;
						else
							*Dial_proc_state=Dial_State_BMBANDPREF;
					}
				}
				break;			
		case Dial_State_BMBANDPREF:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
					*Dial_proc_state=Dial_State_BMDATASTATUSEN;
				break;
		case Dial_State_BMDATASTATUSEN:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
					*Dial_proc_state=Dial_State_PSDIALIND;
				break;
		case Dial_State_PSDIALIND:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
					*Dial_proc_state=Dial_State_SIGNALIND;
				break;
		case Dial_State_SIGNALIND:
				if(NULL!= strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
					*Dial_proc_state=Dial_State_CPIN_QUERY;
				break;
		case Dial_State_CPIN_QUERY:
				global_dial_vars.pin_qpin_flag=0;		//set flag ,so we can choose which cmd to input pin 
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_READY))
				{
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					break;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_NEED_PIN))
				{
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=1;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					break;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_NEED_PUK))
				{
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=1;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					break;
				}
				else
				{
					global_dial_vars.is_sim_exist=0;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Dial_State_QCPIN_QUERY;
					break;
				}
		case Dial_State_QCPIN_QUERY:
				global_dial_vars.pin_qpin_flag=1;
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_READY))
				{
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					break;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_QCPIN_NEED_PIN))
				{
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=1;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					break;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_QCPIN_NEED_PUK))
				{
					global_dial_vars.is_sim_exist=1;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=1;
					*Dial_proc_state=Dial_State_BMCPNCNT;
					break;
				}
				else
				{
					global_dial_vars.is_sim_exist=0;
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Dial_State_CPIN_QUERY;
					break;
				}
		case Dial_State_BMCPNCNT:
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+BMCPNCNT:");
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
								if(global_dial_vars.pin_qpin_flag == 1 )
									*Dial_proc_state=Dial_State_QCPIN_SET;
								else
									*Dial_proc_state=Dial_State_CPIN_SET;
								break;
							}
							else 				//we already input pin error one time,or need input puk,so we just query and wait
							{
								if(global_dial_vars.pin_qpin_flag == 1)
									*Dial_proc_state=Dial_State_QCPIN_QUERY;
								else
									*Dial_proc_state=Dial_State_CPIN_QUERY;
								break;
							}
						}
						else
						{
							*Dial_proc_state=Dial_State_ICCID;
						}
					}
				}
				break;
		case Dial_State_CPIN_SET:
				*Dial_proc_state=Dial_State_CPIN_QUERY;
				sleep(5);
				break;
		case Dial_State_QCPIN_SET:
				*Dial_proc_state=Dial_State_QCPIN_QUERY;	
				sleep(5);
				break;
		case Dial_State_ICCID:
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"ICCID:");
					if(NULL!=ptr_tmp)
					{
						char* ptr_tmp1=NULL;
						ptr_tmp1=ptr_tmp+strlen("ICCID:");
						char* ptr_tmp2=strstr(ptr_tmp1,"OK");
						if(NULL!= ptr_tmp2)
							ptr_tmp2[0]='\0';
						ptr_tmp=strip_head_tail_space(ptr_tmp1);
						if(strlen(ptr_tmp)<64)
							strncpy(global_dial_vars.iccid,ptr_tmp,strlen(ptr_tmp));
						if(global_dial_vars.pin_qpin_flag==1)
							*Dial_proc_state=Dial_State_QCIMI;
						else
							*Dial_proc_state=Dial_State_CIMI;
					}
				}		
				break;
		case Dial_State_CIMI:
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
					*Dial_proc_state=Dial_State_BMMODODR;
				}
			}				
			break;
		case Dial_State_QCIMI:
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
					*Dial_proc_state=Dial_State_BMMODODR;
				}
			}				
			break;

		case Dial_State_BMMODODR:
			if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_CGDCONT;
			break;

		case Dial_State_CGDCONT:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_QCPDPP;
			break;
		case Dial_State_QCPDPP:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_BM3GPP2AUTHINFO;
			break;
		case Dial_State_BM3GPP2AUTHINFO:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				*Dial_proc_state=Dial_State_CGDCONT2;
			break;		
		case Dial_State_CGDCONT2:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				global_dialtool.Dial_Lvl_1=DIAL_DIAL;
				*Dial_proc_state=Dial_State_SYSINFO;
			}
			break;
		case Dial_State_CFUN_DISABLE:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				*Dial_proc_state=Dial_State_CFUN_ENABLE;
				break;
			}
		case Dial_State_CFUN_ENABLE:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				sleep(10);
				*Dial_proc_state=Dial_State_CPIN_QUERY;
				break;
			}	

				
	#if 0
		case Dial_State_CLCK_QUERY:
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) )
				{
					char* ptr_tmp= strstr(global_dialtool.buffer_at_sponse,"+CLCK:");
					if(NULL!= ptr_tmp)
					{
						global_dial_vars.pinlock_enable_flag=atoi(ptr_tmp+strlen("+CLCK:"));
						global_dialtool.Dial_Lvl_1=DIAL_DIAL;
						*Dial_proc_state=Dial_State_SYSINFO;
					}					
				}
				break;	
		case Dial_State_QCLCK_QUERY:	
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) )
				{
					char* ptr_tmp= strstr(global_dialtool.buffer_at_sponse,"+QCLCK:");
					if(NULL!= ptr_tmp)
					{
						global_dial_vars.pinlock_enable_flag=atoi(ptr_tmp+strlen("+QCLCK:"));
						global_dialtool.Dial_Lvl_1=DIAL_DIAL;
						*Dial_proc_state=Dial_State_SYSINFO;
					}					
				}
				break;
	#endif
		default:
			break;
	}
	log_info("%s_%d\n",__FUNCTION__,__LINE__);
}

void ec20_dial(int* Dial_proc_state )
{
	static int exception_count=0;      //can not search  valid  network count,if more than 20 times,we go into exception mode:reboot module
	static int data_link_count=0;
	log_info("%s_%d\n",__FUNCTION__,__LINE__);
	switch(*Dial_proc_state)
	{
	
		case Dial_State_QCRMCALL_DISCONNECT:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)||NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR) )
			{
				if(check_file_exist(MANUAL_DISCONNECT))
				{
					sleep(1);
					break;
				}
				else
				{
					*Dial_proc_state=Dial_State_SYSINFO;
				}
				break;
			}
		case Dial_State_SYSINFO:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)&& NULL!=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:"))
			{
				int srv_status,srv_domain;
				int roam_status,sys_mode,sim_state;
				char** result_useful=(char**)m_malloc_two(5,8);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
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
						*Dial_proc_state=Dial_State_BMRAT;
						break;
					}
				}
				exception_count++;
				free_two(result_useful,5,8);
				if(exception_count>60)
				{
					global_dialtool.Dial_Lvl_1=DIAL_INIT;
					global_dial_vars.network_link=0;
					exception_count=0;
					*Dial_proc_state=Dial_State_CFUN_DISABLE;
				}
			}
			break;
		case Dial_State_BMRAT:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char *network_type_string=strstr(global_dialtool.buffer_at_sponse,"+BMRAT:");
				if(NULL!= network_type_string)
					network_type_string=strip_head_tail_space(network_type_string+strlen("+BMRAT:"));
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
					*Dial_proc_state=Dial_State_QCRMCALL;
					exception_count=0;
				}
				else
					exception_count++;;
				log_info("networktype:%s %d\n",network_type_string,global_dial_vars.evdo_cdma_flag);
				if(exception_count>60)
				{
					global_dialtool.Dial_Lvl_1=DIAL_INIT;
					global_dial_vars.network_link=0;
					exception_count=0;
					*Dial_proc_state=Dial_State_CFUN_DISABLE;
				}
				break;
			}
			break;
		case Dial_State_QCRMCALL:
				*Dial_proc_state=Dial_State_QCRMCALL_QUERY;
				break;
		case Dial_State_QCRMCALL_QUERY:		
			if(NULL!= strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				int ip_type=3;
				int udhcpc_pid;
				char buffer_cmd[1024];
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"$QCRMCALL:");
				if(NULL!= ptr_tmp)
				{
					static int udhcpc_sig_count=0;
					ptr_tmp=strip_head_tail_space(ptr_tmp+strlen("$QCRMCALL:"));
					ip_type=atoi(ptr_tmp);
					global_dial_vars.ipstack_registered=ip_type;
					//here we dial successful,so we should told udhcpc to get ip and dns
					snprintf(buffer_cmd,sizeof(buffer_cmd),"udhcpc -b -i %s -p %s -s /etc/udhcpc.script",
					global_system_info.module_info.network_card_name,UDHCPC_PIDFILE_PATH);
			REGET_IP_BY_UDHCPC:
					udhcpc_pid=getpid_by_pidfile(UDHCPC_PIDFILE_PATH);
					if(FALSE!=udhcpc_pid)
					{
						if(0!=kill(udhcpc_pid,SIGUSR1))
						{
							sleep(1);
							udhcpc_sig_count++;
							if(udhcpc_sig_count>3)
							{
								system("killall udhcpc");
							}
							log_info("udhcpc send USR1 ERROR:%s\n",strerror(errno));
							goto REGET_IP_BY_UDHCPC;
						}
						udhcpc_sig_count=0;
					}
					else
						system(buffer_cmd);

					exception_count=0;
					sleep(5);
					*Dial_proc_state=Dial_State_BMDATASTATUS;
					
				}
				else
				{
					exception_count++;
					global_dial_vars.ipstack_registered=0;
					*Dial_proc_state=Dial_State_QCRMCALL;
				}			
			}
			else
			{
				exception_count++;
			}
			if(exception_count>60)
			{
				global_dialtool.Dial_Lvl_1=DIAL_INIT;
				global_dial_vars.network_link=0;
				exception_count=0;
				*Dial_proc_state=Dial_State_CFUN_DISABLE;
			}
			break;
		case Dial_State_BMDATASTATUS:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMDATASTATUS:"))
			{
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+BMDATASTATUS:")+strlen("+BMDATASTATUS:");
				global_dial_vars.network_link=atoi(tmp_ptr);
				if(global_dial_vars.network_link!=0)
				{
					data_link_count=0;
					if(global_dial_vars.evdo_cdma_flag == 1)
						*Dial_proc_state=Dial_State_HDRCSQ;
					else
						*Dial_proc_state=Dial_State_CSQ_SET;
				}
				else
				{
					data_link_count++;
					if(data_link_count >10)
						*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
					if(data_link_count >30)
					{
						global_dialtool.Dial_Lvl_1=DIAL_INIT;
						global_dial_vars.network_link=0;
						exception_count=0;
						*Dial_proc_state=Dial_State_CFUN_DISABLE;
					}
				}
			}
			break;

	//set  init status after dial sucessful,and status update by reporter
		case Dial_State_CSQ_SET:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) )
			{
				*Dial_proc_state=Dial_State_CSQ;
			}
			break;
		case Dial_State_HDRCSQ:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL != strstr(global_dialtool.buffer_at_sponse,"^HDRCSQ:"))
			{
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"^HDRCSQ:")+strlen("^HDRCSQ:");
				int rssi_hdr=atoi(tmp_ptr);
				if(rssi_hdr==99)
				{
					global_dial_vars.signal_rssi_level=5;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"60");
				}
				else if(rssi_hdr ==80)
				{
					global_dial_vars.signal_rssi_level=4;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"65");
				}
				else if(rssi_hdr ==60)
				{
					global_dial_vars.signal_rssi_level=3;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"80");
				}
				else if(rssi_hdr ==40)
				{
					global_dial_vars.signal_rssi_level=2;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"95");
				}
				else if(rssi_hdr ==20)
				{
					global_dial_vars.signal_rssi_level=1;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"105");
				}
				else
				{
					global_dial_vars.signal_rssi_level=0;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"0");
				}
				global_dialtool.Dial_Lvl_1=DIAL_DEAMON;
				*Dial_proc_state=Dial_State_BMTCELLINFO;	
			}
		case Dial_State_CSQ:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+CSQ:"))
			{
				double rssi,rsrp,rsrq,snr;
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+CSQ:")+strlen("+CSQ:");
				char* tmp_ptr_1=strstr(tmp_ptr,CMD_EXE_OK);
				if(NULL!=tmp_ptr_1)
					tmp_ptr_1[0]='\0';
				char** result_useful=(char**)m_malloc_two(5,8);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				separate_str(tmp_ptr,",",result_useful);
				rssi=atof(result_useful[0]);
				rsrp=atof(result_useful[2]);
				rsrq=atof(result_useful[3]);
				snr=atof(result_useful[4]);
				free_two(result_useful,5,8);
				snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"%0.f",rssi);
				snprintf(global_dial_vars.signal_rsrp,sizeof(global_dial_vars.signal_rsrp),"%0.f",rsrp);
				snprintf(global_dial_vars.signal_rsrq,sizeof(global_dial_vars.signal_rsrq),"%0.f",rsrq);
				snprintf(global_dial_vars.signal_sinr,sizeof(global_dial_vars.signal_sinr),"%0.f",snr);

				//supplier provide 
				if(rssi<=58)
					global_dial_vars.signal_rssi_level=4;
				else if(rssi>58 && rssi<=78)
					global_dial_vars.signal_rssi_level=3;
				else if(rssi>78 && rssi <=98)
					global_dial_vars.signal_rssi_level=2;
				else if(rssi>98&& rssi<=108)
					global_dial_vars.signal_rssi_level=1;
				else
					global_dial_vars.signal_rssi_level=0;
				
				global_dialtool.Dial_Lvl_1=DIAL_DEAMON;
				*Dial_proc_state=Dial_State_BMTCELLINFO;	
			}
			break;
		default:
			break;			
	}

}

void ec20_deamon(int* Dial_proc_state)
{
	static int data_link_status_count=0;
	log_info("%s_%d\n",__FUNCTION__,__LINE__);
	
	if(check_file_exist(MANUAL_DISCONNECT))
	{
		global_dialtool.Dial_Lvl_1=DIAL_DIAL;
		data_link_status_count=0;
		global_dial_vars.network_link=0;
		*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
		return;
	}
	int network_type=0;

	switch(*Dial_proc_state)
	{
		case Dial_State_BMTCELLINFO:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)&& NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMTCELLINFO:"))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+BMTCELLINFO:")+strlen("+BMTCELLINFO:");
				char* ptr_tmp1=strstr(ptr_tmp,CMD_EXE_OK);
				char* ptr_tmp2=NULL;
				char* ptr_tmp3=NULL;
				if(NULL!=ptr_tmp1)
					ptr_tmp1='\0';
				char buffer_tmp[RECV_BUFF_SIZE];
	#if 0
				int i=0;
				char** str_affer=(char**)m_malloc_two(64,128);
				separate_str(ptr_tmp,"\n",str_affer);

				for(i=0;i<64;i++)
				{
					if(str_affer[i][0] == '\0')
						continue;
	#endif
					if(NULL!= strstr(ptr_tmp,"CELL_ID:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"CELL_ID:")+strlen("CELL_ID:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);
						strncpy(global_dial_vars.cell_id,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"PCI:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"PCI:")+strlen("PCI:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);
						strncpy(global_dial_vars.pci,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"LAC_ID:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"LAC_ID:")+strlen("LAC_ID:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);
						strncpy(global_dial_vars.lac_id,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"RSSI:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"RSSI:")+strlen("RSSI:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						strncpy(global_dial_vars.signal_rssi_value,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"RSRP:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"RSRP:")+strlen("RSRP:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						strncpy(global_dial_vars.signal_rsrp,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"RSRQ:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"RSRQ:")+strlen("RSRQ:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						strncpy(global_dial_vars.signal_rsrq,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"SINR:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"SINR:")+strlen("SINR:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						strncpy(global_dial_vars.signal_sinr,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"ACTIVE BAND:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"ACTIVE BAND:")+strlen("ACTIVE BAND:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						strncpy(global_dial_vars.active_band,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"ACTIVE CHANNEL:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"ACTIVE CHANNEL:")+strlen("ACTIVE CHANNEL:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						strncpy(global_dial_vars.active_channel,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"EARFCN:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"EARFCN:")+strlen("EARFCN:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						strncpy(global_dial_vars.earfcn,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"ENODEBID:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"ENODEBID:")+strlen("ENODEBID:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						strncpy(global_dial_vars.enodebid,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"TAC:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"TAC:")+strlen("TAC:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						strncpy(global_dial_vars.tac,ptr_tmp3,strlen(ptr_tmp3));

					}
					if(NULL!= strstr(ptr_tmp,"RRC_STATUS:"))
					{
						ptr_tmp2=strstr(ptr_tmp,"RRC_STATUS:")+strlen("RRC_STATUS:");
						memset(buffer_tmp,0,RECV_BUFF_SIZE);
						strncpy(buffer_tmp,ptr_tmp2,RECV_BUFF_SIZE);
						
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,",")) )
						{
							ptr_tmp3[0]='\0';
						}
						if(NULL!=(ptr_tmp3=strstr(buffer_tmp,"\n")) )
						{
							ptr_tmp3[0]='\0';
						}			
						ptr_tmp3=strip_head_tail_space(buffer_tmp);

						strncpy(global_dial_vars.rrc_status,ptr_tmp3,strlen(ptr_tmp3));

					}
	#if 0
				}
	#endif				
			}
			*Dial_proc_state=Dial_State_SYSINFO;
			break;
		case Dial_State_SYSINFO:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK)&& NULL!=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:"))
			{
			
				int srv_status,srv_domain;
				int roam_status,sys_mode,sim_state;
				char** result_useful=(char**)m_malloc_two(5,8);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
	
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO:");
				if(NULL != ptr_tmp)
					ptr_tmp=ptr_tmp+strlen("^SYSINFO:");
				char* ptr_tmp1=strstr(ptr_tmp,CMD_EXE_OK);
				if(NULL!=ptr_tmp1)
					ptr_tmp1[0]='\0';
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				separate_str(ptr_tmp,",",result_useful);
				
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				srv_status=atoi(result_useful[0]);
				srv_domain=atoi(result_useful[1]);
				roam_status=atoi(result_useful[2]);
				sys_mode=atoi(result_useful[3]);
				sim_state=atoi(result_useful[4]);

				global_dial_vars.service_status=srv_status;
				global_dial_vars.ps_cs_region=srv_status;

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
				}
				
				free_two(result_useful,5,8);
				if(srv_status==0||srv_status==4||srv_domain==0)
				{
					global_dialtool.Dial_Lvl_1=DIAL_DIAL;
					global_dial_vars.network_link=0;
					*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
				}
				else
				{
					*Dial_proc_state=Dial_State_BMRAT;		
				}
				sys_mode = sys_mode;
			}
			break;
		case Dial_State_BMRAT:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMRAT:"))
			{
				char *network_type_string=strstr(global_dialtool.buffer_at_sponse,"+BMRAT:");
				if(NULL!= network_type_string)
					network_type_string=strip_head_tail_space(network_type_string+strlen("+BMRAT:"));
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
					network_type=41;
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
					network_type=41;
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
					network_type=40;
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
					network_type=41;
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
					network_type=39;
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
					network_type=38;
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
					network_type=36;
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
					network_type=35;
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
					network_type=35;
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
					network_type=33;
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
					network_type=33;
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
					network_type=32;
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
					network_type=44;
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
					network_type=45;
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
					network_type=45;
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
					network_type=43;
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
					network_type=43;
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
					network_type=31;
					global_dial_vars.reg_status=0;
					global_dial_vars.greg_status=0;
					global_dial_vars.ereg_status=0;
					global_dial_vars.evdo_cdma_flag=0;
					global_dial_vars.act=-1;
					global_dial_vars.network_link=0;
				}
				else
				{
					network_type=32;
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
					strlcpy(global_dial_vars.network_mode_detail,ptr_tmp1,strlen(ptr_tmp1)+1);
				}
			}
			if(network_type != global_dial_vars.network_mode_bmrat &&
				(network_type ==45 ||network_type==43 ||global_dial_vars.network_mode_bmrat ==45 
				|| global_dial_vars.network_mode_bmrat ==43) )
			{
				global_dial_vars.network_mode_bmrat=network_type;
				global_dial_vars.network_link=0;
				global_dialtool.Dial_Lvl_1=DIAL_DIAL;
				*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
			}
			else
			{
				if(global_dial_vars.evdo_cdma_flag == 1)
				{
					if(global_dial_vars.network_mode_bmrat ==43)
					{
						static char init_cdma_rssi_flag=0;
						if(init_cdma_rssi_flag==0)
						{
							sprintf(global_dial_vars.signal_rssi_value,"65");
							global_dial_vars.signal_rssi_level=4;
							init_cdma_rssi_flag=1;
						}		
						*Dial_proc_state=Dial_State_BMDATASTATUS;
					}
					else
						*Dial_proc_state=Dial_State_HDRCSQ;
				}
				else
					*Dial_proc_state=Dial_State_CSQ;
			}


			break;
		case Dial_State_CSQ:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+CSQ:"))
			{
				float rssi,rsrp,rsrq,snr;
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+CSQ:")+strlen("+CSQ:");
				char* tmp_ptr_1=strstr(tmp_ptr,CMD_EXE_OK);
				if(NULL!=tmp_ptr_1)
					tmp_ptr_1[0]='\0';
				char** result_useful=(char**)m_malloc_two(5,8);
				log_info("%s_%d\n",__FUNCTION__,__LINE__);
				separate_str(tmp_ptr,",",result_useful);
				rssi=atof(result_useful[0]);
				rsrp=atof(result_useful[2]);
				rsrq=atof(result_useful[3]);
				snr=atof(result_useful[4]);
				free_two(result_useful,5,8);
				snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"%0.f",rssi);
				snprintf(global_dial_vars.signal_rsrp,sizeof(global_dial_vars.signal_rsrp),"%0.f",rsrp);
				snprintf(global_dial_vars.signal_rsrq,sizeof(global_dial_vars.signal_rsrq),"%0.f",rsrq);
				snprintf(global_dial_vars.signal_sinr,sizeof(global_dial_vars.signal_sinr),"%0.f",snr);

				//supplier provide 
				if(rssi<=58)
					global_dial_vars.signal_rssi_level=4;
				else if(rssi>58 && rssi<=78)
					global_dial_vars.signal_rssi_level=3;
				else if(rssi>78 && rssi <=98)
					global_dial_vars.signal_rssi_level=2;
				else if(rssi>98&& rssi<=108)
					global_dial_vars.signal_rssi_level=1;
				else
					global_dial_vars.signal_rssi_level=0;	

//				log_info("csq:%.1f,%.1f,%.1f,%.1f\n",rssi,rsrp,rsrq,snr);
			}

			*Dial_proc_state=Dial_State_BMDATASTATUS;
			break;
		case Dial_State_HDRCSQ:
			if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL != strstr(global_dialtool.buffer_at_sponse,"^HDRCSQ:"))
			{
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"^HDRCSQ:")+strlen("^HDRCSQ:");
				int rssi_hdr=atoi(tmp_ptr);
				if(rssi_hdr==99)
				{
					global_dial_vars.signal_rssi_level=5;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"60");
				}
				else if(rssi_hdr ==80)
				{
					global_dial_vars.signal_rssi_level=4;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"65");
				}
				else if(rssi_hdr ==60)
				{
					global_dial_vars.signal_rssi_level=3;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"80");
				}
				else if(rssi_hdr ==40)
				{
					global_dial_vars.signal_rssi_level=2;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"95");
				}
				else if(rssi_hdr ==20)
				{
					global_dial_vars.signal_rssi_level=1;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"105");
				}
				else
				{
					global_dial_vars.signal_rssi_level=0;
					snprintf(global_dial_vars.signal_rssi_value,sizeof(global_dial_vars.signal_rssi_value),"0");
				}
				*Dial_proc_state=Dial_State_BMDATASTATUS;	
			}
		case Dial_State_BMDATASTATUS:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMDATASTATUS:"))
			{
				char* tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+BMDATASTATUS:")+strlen("+BMDATASTATUS:");
				global_dial_vars.network_link=atoi(tmp_ptr);
				if(global_dial_vars.network_link!=1)
					data_link_status_count++;
				else
					data_link_status_count=0;
				if(data_link_status_count>5)
				{
					global_dialtool.Dial_Lvl_1=DIAL_DIAL;
					data_link_status_count=0;
					*Dial_proc_state=Dial_State_QCRMCALL_DISCONNECT;
				}
				else
				{
					*Dial_proc_state=Dial_State_SYSINFO;
				}
			}
			break;
#if 0
		case Dial_State_BMTCELLINFO:
			if(NULL!=strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMTCELLINFO:"))
			{
				char tmp_ptr=strstr(global_dialtool.buffer_at_sponse,"+BMTCELLINFO:")+strlen("+BMTCLELLINFO:");
				
			}
			*Dial_proc_state=Dial_State_SYSINFO;
#endif
		default:
			break;
	}
}

void ec20_report_handle(char* buffer)
{
	if(NULL!=strstr(buffer,"+SIGNALIND:"))
	{
		char* ptr_tmp=strstr(buffer,"+SIGNALIND:")+strlen("+SIGNALIND:");
		global_dial_vars.signal_rssi_level=atoi(ptr_tmp);
	}
	if(NULL!=strstr(buffer,"^DATACON:"))
	{
		char* ptr_tmp=strstr(buffer,"^DATACON:")+strlen("^DATACON:");
		global_dial_vars.network_link=atoi(ptr_tmp);
	}
	if(NULL!=strstr(buffer,"^DSDORMANT:"))
	{
		char* ptr_tmp=strstr(buffer,"^DSDORMANT:")+strlen("^DSDORMANT:");
		global_dial_vars.module_dormant=atoi(ptr_tmp);
	}
	if(NULL!=strstr(buffer,"^RSSILVL:"))
	{
		char* ptr_tmp=strstr(buffer,"^RSSILVL:")+strlen("^RSSILVL:");
		int signal_lvl=atoi(ptr_tmp);
		if(signal_lvl==0)
		{
			sprintf(global_dial_vars.signal_rssi_value,"0");
			global_dial_vars.signal_rssi_level=0;
		}
		else if(signal_lvl==20)
		{
			sprintf(global_dial_vars.signal_rssi_value,"105");
			global_dial_vars.signal_rssi_level=1;
		}
		else if(signal_lvl==40)
		{
			sprintf(global_dial_vars.signal_rssi_value,"100");
			global_dial_vars.signal_rssi_level=2;
		}
		else if(signal_lvl==60)
		{
			sprintf(global_dial_vars.signal_rssi_value,"80");
			global_dial_vars.signal_rssi_level=3;
		}
		else if(signal_lvl==80)
		{
			sprintf(global_dial_vars.signal_rssi_value,"65");
			global_dial_vars.signal_rssi_level=4;
		}
		else
		{
			sprintf(global_dial_vars.signal_rssi_value,"999");
			global_dial_vars.signal_rssi_level=5;
		}
	}
	if(NULL!=strstr(buffer,"^ HRSSILVL:"))
	{
		char* ptr_tmp=strstr(buffer,"^HRSSILVL:")+strlen("^HRSSILVL:");
		int signal_lvl=atoi(ptr_tmp);
		if(signal_lvl==0)
			global_dial_vars.signal_rssi_level=0;
		else if(signal_lvl==20)
			global_dial_vars.signal_rssi_level=1;
		else if(signal_lvl==40)
			global_dial_vars.signal_rssi_level=2;
		else if(signal_lvl==60)
			global_dial_vars.signal_rssi_level=3;
		else if(signal_lvl==80)
			global_dial_vars.signal_rssi_level=4;
		else
			global_dial_vars.signal_rssi_level=5;

	}
	if(NULL!=strstr(global_dialtool.buffer_at_sponse,"ICCID:"))
	{
		char* ptr_tmp1=strstr(global_dialtool.buffer_at_sponse,"ICCID:")+strlen("ICCID:");
		char* ptr_tmp2=strstr(ptr_tmp1,"OK");
		if(NULL!= ptr_tmp2)
			ptr_tmp2[0]='\0';
		char* ptr_tmp=strip_head_tail_space(ptr_tmp1);
		if(strlen(ptr_tmp) < 64)
			strncpy(global_dial_vars.iccid,ptr_tmp,strlen(ptr_tmp));
	}
	if(NULL!=strstr(global_dialtool.buffer_at_sponse,"+BMBANDPREF:"))
	{
		char* ptr_tmp1=strstr(global_dialtool.buffer_at_sponse,"+BMBANDPREF:")+strlen("+BMBANDPREF:");
		char* ptr_tmp3=strstr(ptr_tmp1,",");
		if(NULL!= ptr_tmp3)
			ptr_tmp3=ptr_tmp3+1;
		char* ptr_tmp4=strstr(ptr_tmp3,",");
		if(NULL!=ptr_tmp4)
			ptr_tmp4='\0';
		char* ptr_tmp=strip_head_tail_space(ptr_tmp3);
		strncpy(global_dial_vars.lte_lock_band,ptr_tmp,strlen(ptr_tmp));
	}
	/*
	if(NULL!=strstr(buffer,"^MODE:"))
	{
	}
	*/

#if 0
	/*short mesg dealwith*/
	if(NULL!=strstr(buffer,"^UIMST:"))
	{
	}
	if(NULL!=strstr(buffer,"^HCMT:"))
	{
	}
	if(NULL!=strstr(buffer,"^SMMEMFULL:"))
	{
	}
	if(NULL!=strstr(buffer,"^HCMGSS:"))
	{
	}
	if(NULL!=strstr(buffer,"^HCMGSF:"))
	{
	}
	if(NULL!=strstr(buffer,"^HCMT:"))
	{
	}

#endif


}
void ec20_get_moduleinfo(MDI* p)
{
	int flag_cmd;
	int bytes_n=0;
	fd_set readfds;
	char *buffer_recv=NULL;
	int maxfd;
	int out_flag;
	int reply_count=0;	//we must make sure all data received,we judge by CMD_EXE_OK and so on
	struct timeval tv;
	int imei_check_flag=0;
	int length;
	char buffer[1024]={0};
	char* str_ptr=NULL;
	char* str_sub_ptr=NULL;
	char* tmp_ptr=NULL;
	char* result_ptr=NULL;	
	buffer_recv=(char *)m_malloc(RECV_BUFF_SIZE);
	char* ptr_tmp=buffer_recv;
	char* ptr_tmp1=NULL;
	
	while(1)
	{
		memset(buffer_recv,0,RECV_BUFF_SIZE);
		flag_cmd=util_send_cmd(global_dialtool.dev_handle,"ATI\r",&global_dialtool.pthread_moniter_flag);
		reply_count++;
		log_info("we are sending ati ,and get module info detailed:%d\n",flag_cmd);
		if(TRUE!=flag_cmd)
		{
			sleep(2);
			continue;
		}
		do
		{
			tv.tv_sec=6;
			tv.tv_usec=0;

			FD_ZERO(&readfds);
			FD_SET(global_dialtool.dev_handle,&readfds);
			maxfd=global_dialtool.dev_handle+1;
			if((out_flag=select(maxfd,&readfds,NULL,NULL,&tv))>0)
			{
				if(out_flag>0)
				{
					out_flag=read(global_dialtool.dev_handle,buffer_recv+bytes_n,RECV_BUFF_SIZE-bytes_n-1);
					log_info("%s_%d:%s %d\n",__FUNCTION__,__LINE__,buffer_recv,strlen(buffer_recv));
					bytes_n+=out_flag;
				}
			}
			if(NULL!=(ptr_tmp1=strstr(ptr_tmp,CMD_EXE_OK)))
			{
				reply_count--;
				ptr_tmp=ptr_tmp+strlen(CMD_EXE_OK);
				
			}else if(NULL!=(ptr_tmp1=strstr(ptr_tmp,CMD_EXE_ERROR)))
			{
				reply_count--;
				ptr_tmp=ptr_tmp+strlen(CMD_EXE_ERROR);
			}else if(NULL!=(ptr_tmp1=strstr(ptr_tmp,CMD_RESULT_CME_ERROR)))
			{
				reply_count--;
				ptr_tmp=ptr_tmp+strlen(CMD_RESULT_CME_ERROR);
			}else if(NULL!=(ptr_tmp1=strstr(ptr_tmp,CMD_RESULT_CMS_ERROR)))
			{
				reply_count--;
				ptr_tmp=ptr_tmp+strlen(CMD_RESULT_CMS_ERROR);
			}
		}while(reply_count!=0);
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
				if(imei_check_flag!=1)
				{
					strncpy(p->imei,result_ptr,strlen(result_ptr));
				}
				else
				{
					char tmp_buffer[48]={0};
					num_asciistr_to_decimalstr(result_ptr,tmp_buffer);
					if(tmp_buffer[0] != '\0')
						strncpy(p->imei,tmp_buffer,strlen(tmp_ptr));
				}
			}
		}
		if(p->imei[0] == '\0' )
		{
			util_send_cmd(global_dialtool.dev_handle,"AT+BMIMEI\r",&global_dialtool.pthread_moniter_flag);
			reply_count++;
			imei_check_flag=1;
			sleep(1);
			continue;
		}

		log_info("%s_%d:%s\n",__FUNCTION__,__LINE__,buffer_recv);
		m_free(buffer_recv);
		break;	
	}		
}

DialProc process_ec20=
{
	ec20_init,
	ec20_dial,
	ec20_deamon,
	ec20_sendat,
	ec20_report_handle,
};



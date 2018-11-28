#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "common.h"
#include "lt10.h"
#include "include.h"

char tmpString[32] = {0};
int not_searching_state_counter=0;
int cereg_to_cgreg_counter=0;
extern int global_sleep_interval_long;

/*reporter indicator define*/
MCT lt10_mode_table[]=
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

void lt10_get_moduleinfo(MDI* p)
{
	char *buffer_recv=NULL;

	int length;
	char buffer[1024]={0};
	char* str_ptr=NULL;
	char* str_sub_ptr=NULL;
	char* result_ptr=NULL;
	char * tmp_ptr;
	int ret=0;

	buffer_recv=(char *)m_malloc(RECV_BUFF_SIZE);

	//cfun=0, 1 to reset modem
	memset(buffer_recv,0,RECV_BUFF_SIZE);
	util_send_cmd(global_dialtool.dev_handle,"at+cfun=0\r",&global_dialtool.pthread_moniter_flag);
	sleep(1);
	util_send_cmd(global_dialtool.dev_handle,"at+cfun=1\r",&global_dialtool.pthread_moniter_flag);
	sleep(1);
	read(global_dialtool.dev_handle,buffer_recv,RECV_BUFF_SIZE);
	printf(">>>>>>>>>>>>>>>>>>>>>>cfun: receive: %s\n",buffer_recv);
	if(!strstr(buffer_recv,"OK"))
	{
		system("/etc/rc.d/rc.reset_module");
		exit(-1);
	}

	while(1)
	{
		ret=util_send_cmd(global_dialtool.dev_handle,"AT+TZSPEC=0\r",&global_dialtool.pthread_moniter_flag);
		if(!ret)
		{
			sleep(1);
			continue;
		}
		read(global_dialtool.dev_handle,buffer_recv,RECV_BUFF_SIZE);
		util_send_cmd(global_dialtool.dev_handle,"AT+TZVER?\r",&global_dialtool.pthread_moniter_flag);
		sleep(1);
		read(global_dialtool.dev_handle,buffer_recv,RECV_BUFF_SIZE);
		str_ptr=strstr(buffer_recv,"TZVER:");
		if(NULL==str_ptr)
		{
			continue;
		}
		tmp_ptr=str_ptr+strlen("TZVER:");
		str_sub_ptr=strstr(tmp_ptr,"\n");
		if(NULL != str_sub_ptr)
		{
			//system("echo find-n1 >> /tmp/test");
			length=str_sub_ptr-tmp_ptr;
			strncpy(buffer,tmp_ptr,length);
			buffer[length]='\0';
			result_ptr=strip_head_tail_space(buffer);
			strncpy(p->softver,result_ptr,strlen(result_ptr));
			system("echo nofind-n >> /tmp/test");
		}
		util_send_cmd(global_dialtool.dev_handle,"AT^DGSN?\r",&global_dialtool.pthread_moniter_flag);
		sleep(1);
		memset(buffer_recv,0,RECV_BUFF_SIZE);
		read(global_dialtool.dev_handle,buffer_recv,RECV_BUFF_SIZE);
		str_ptr=strstr(buffer_recv,"DGSN:");
		if(NULL==str_ptr)
		{
			continue;
		}
		tmp_ptr=str_ptr+strlen("DGSN:");
		str_sub_ptr=strstr(tmp_ptr,"\n");
		if(NULL != str_sub_ptr)
		{
			//system("echo find-n2 >> /tmp/test");
			length=str_sub_ptr-tmp_ptr;
			strncpy(buffer,tmp_ptr,length);
			buffer[length]='\0';
			result_ptr=strip_head_tail_space(buffer);
			strncpy(p->imei,result_ptr,strlen(result_ptr));
		}
		if(p->imei[0] == '\0')
		{
			continue;
		}
		strncpy(p->hardver,"LT10",4);
		//system("echo find-all >> /tmp/test");
		log_info("%s_%d:%s\n",__FUNCTION__,__LINE__,buffer_recv);
		m_free(buffer_recv);
		break;
	}
}

//static char* lt10_hardware_summary[]=
//{
//	"LT10",
//};

MDI lt10_moduleinfo=
{
	{TZ_VENNOR_LT10,TZ_PRODUCT_LT10},
	"1-1.4:1.2",
	lt10_get_moduleinfo,
	"0",
	"0",
	"0",
	"0",
	"usbserial.ko",
	"usb0",
};

void lt10_init(int*  Dial_proc_state)
{
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
	static int hasSetPin = 0;
	switch(*Dial_proc_state)
	{
		case Lt10_State_initialized:
				memcpy(global_dial_vars.moduleType,"lt10",4);
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_CMEE;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_ERROR))
				{
					*Dial_proc_state=Lt10_State_CMEE;
				}
				break;
		case Lt10_State_CMEE:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_DGSN_QUERY2;
				}
				break;
		case Lt10_State_DGSN_QUERY2:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_DLTEM_QUERY;
				}
				break;
		case Lt10_State_DLTEM_QUERY:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_DLTEM;
				}
				break;
		case Lt10_State_DLTEM:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_DLTEM_QUERY2;
				}
				break;
		case Lt10_State_DLTEM_QUERY2:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_ROAMING_QUERY;
				}
				break;
		case Lt10_State_ROAMING_QUERY:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_ROAMING_SET;
				}
				break;
		case Lt10_State_ROAMING_SET:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_ROAMING_QUERY2;
				}
				break;
		case Lt10_State_ROAMING_QUERY2:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_BANDSETEX_TEST;
				}
				break;
		case Lt10_State_BANDSETEX_TEST:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_BANDSETEX_QUERY;
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^BANDSETEX: ")+strlen("^BANDSETEX: ");
					if(NULL !=ptr_tmp)
					{
						char* ptr_gw, *ptr_lte, *ptr_tds;
						ptr_gw = strstr(ptr_tmp,"0,");
						if(NULL!= ptr_gw)
						{
							ptr_gw=ptr_gw+2;
							//log_info("ptr_gw:%s\n", ptr_gw);
						}
						else
							break;
						
						ptr_tds = strstr(ptr_gw,",2,");
						if(NULL!= ptr_tds)
						{
							*ptr_tds = 0;
							ptr_tds=ptr_tds+3;
							//log_info("ptr_tds:%s\n", ptr_tds);
						}
						else
							break;
						
						ptr_lte = strstr(ptr_tds,",7,");
						if(NULL!= ptr_lte)
						{
							*ptr_lte = 0;
							ptr_lte=ptr_lte+3;
							//log_info("ptr_lte:%s\n", ptr_lte);
						}
						else
							break;

						ptr_tmp = strstr(ptr_lte,CMD_EXE_OK);
						if(NULL!= ptr_tmp)
						{
							*ptr_tmp = 0;
							ptr_gw=strip_head_tail_space(ptr_gw);
							ptr_lte=strip_head_tail_space(ptr_lte);
							ptr_tds=strip_head_tail_space(ptr_tds);
							//log_info("ptr_gw:%s ptr_tds:%s ptr_lte:%s\n", ptr_gw, ptr_tds, ptr_lte);
							if( (strlen(ptr_gw)<64) && (strlen(ptr_lte)<64) && (strlen(ptr_tds)<64) )
							{
								strncpy(global_dial_vars.gw_all_band,ptr_gw,strlen(ptr_gw));
								strncpy(global_dial_vars.lte_all_band,ptr_lte,strlen(ptr_lte));
								strncpy(global_dial_vars.tds_all_band,ptr_tds,strlen(ptr_tds));
							}
						}
						else
							break;
					}
				}
				break;
		case Lt10_State_BANDSETEX_QUERY:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_DUSIMR_QUERY;
				}
				break;
		case Lt10_State_DUSIMR_QUERY:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"1");
					if(NULL!=ptr_tmp)
					{
						global_dial_vars.is_sim_exist=1;
						*Dial_proc_state = Lt10_State_DUSIMR_REPORT;
					}
				}
				break;
		case Lt10_State_DUSIMR_REPORT:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_ICCID;
				}
				break;
		case Lt10_State_ICCID:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^DCID:");
					if(NULL!=ptr_tmp)
					{
						char* ptr_tmp1=NULL;
						ptr_tmp1=ptr_tmp+strlen("^DCID:");
						char* ptr_tmp2=strstr(ptr_tmp1,"OK");
						if(NULL!= ptr_tmp2)
							ptr_tmp2[0]='\0';
						ptr_tmp=strip_head_tail_space(ptr_tmp1);
						if(strlen(ptr_tmp)<64)
							strncpy(global_dial_vars.iccid,ptr_tmp,strlen(ptr_tmp));
						*Dial_proc_state = Lt10_State_CPIN;
					}
				}
				break;
		case Lt10_State_CPIN:
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_READY))
				{
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Lt10_State_CIMI;
					hasSetPin = 0;
					break;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_NEED_PIN))
				{
					global_dial_vars.sim_real_lock_pin = 1;
					global_dial_vars.device_lock_pin=1;
					global_dial_vars.device_lock_puk=0;
					if(global_init_parms.enable_pin[0] == '1' && strlen(global_init_parms.pin) && !hasSetPin)
					{
						*Dial_proc_state=Lt10_State_CPIN_SET;
						hasSetPin = 1;
					}
					else
						*Dial_proc_state=Lt10_State_CPIN;
					break;
				}
				else if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK) && 
					NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CPIN_NEED_PUK))
				{
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=1;
					*Dial_proc_state=Lt10_State_CPIN;
					break;
				}
				else
				{
					global_dial_vars.device_lock_pin=0;
					global_dial_vars.device_lock_puk=0;
					*Dial_proc_state=Lt10_State_CPIN;
					break;
				}
				break;
		case Lt10_State_CPIN_SET:
				*Dial_proc_state=Lt10_State_CPIN;
				sleep(5);
				break;
		case Lt10_State_CIMI:
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+CIMI:");
					if(NULL!=ptr_tmp)
					{
						char* ptr_tmp1=NULL;
						ptr_tmp1=ptr_tmp+strlen("+CIMI:");
						char* ptr_tmp2=strstr(ptr_tmp1,"OK");
						if(NULL!= ptr_tmp2)
							ptr_tmp2[0]='\0';
						ptr_tmp=strip_head_tail_space(ptr_tmp1);
						if(strlen(ptr_tmp)<64)
						{
							strncpy(global_dial_vars.imsi,ptr_tmp,strlen(ptr_tmp));
							log_info("imsi:%s,%d\n",ptr_tmp,__LINE__);
							memcpy(tmpString,ptr_tmp, 5);
							tmpString[5] = '\0';
							if( !strncmp(tmpString,"46000",5)  	|| 	!strncmp(tmpString,"46002",5)  ||
							    !strncmp(tmpString,"46004",5)  	||  	!strncmp(tmpString,"46007",5) )
							{
								memcpy(global_dial_vars.operator_current, "China Mobile",strlen("China Mobile"));
							}
							else if( !strncmp(tmpString,"46003",5)  	|| 	!strncmp(tmpString,"46005",5) ||
							    !strncmp(tmpString,"46011",5))
							{
								memcpy(global_dial_vars.operator_current, "China Telecom", strlen("China Telecom"));
							}
							else if( !strncmp(tmpString,"46020",5) )
							{
								memcpy(global_dial_vars.operator_current, "China TieTong", strlen("China TieTong"));
							}
							else if( !strncmp(tmpString,"46001",5) )
							{
								memcpy(global_dial_vars.operator_current, "China Unicom", strlen("China Unicom"));
							}
							*Dial_proc_state=Lt10_State_COPS;
						}
					}
				}
			break;
		case Lt10_State_COPS:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_DSTMEX;
				}
			break;
		case Lt10_State_DSTMEX:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_CEMODE;
				}
			break;
		case Lt10_State_CEMODE:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_CFUN_ENABLE;
				}
			break;
		case Lt10_State_DDPDN:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					global_dialtool.Dial_Lvl_1=DIAL_DIAL;
					*Dial_proc_state=Lt10_State_CGDATA;
				}
			break;
		case Lt10_State_CGDCONT:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					global_dialtool.Dial_Lvl_1=DIAL_DIAL;
					*Dial_proc_state=Lt10_State_CGDATA;
				}
			break;
		case Lt10_State_CFUN_ENABLE:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_CREG;
				}
			break;
		case Lt10_State_CFUN_DISABLE:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_CFUN_ENABLE;
				}
			break;
		case Lt10_State_CREG:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_CGREG;
				}
			break;
		case Lt10_State_CGREG:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_CEREG;
				}
			break;
		case Lt10_State_CEREG:
				if (NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					*Dial_proc_state=Lt10_State_CEREG_QUERY;
					not_searching_state_counter=0;
					cereg_to_cgreg_counter=0;
				}
			break;
		case Lt10_State_CGREG_QUERY:
				//+CGREG: 2,1,"a57e","0abd000c",2,"01"
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+CGREG: 2,");
					if(NULL!=ptr_tmp)
					{
						char* ptr_tmp1=NULL;
						unsigned int lac,cid;

						ptr_tmp1=ptr_tmp+strlen("+CGREG: 2,");
						global_dial_vars.greg_status = atoi(ptr_tmp1);
						//registered,1:home network,5:roaming
						if( ( global_dial_vars.greg_status == 1 ) || ( global_dial_vars.greg_status == 5 ) )
						{
							//now we can activate the pdp service
							//td_lte network
							*Dial_proc_state=Lt10_State_CGDCONT;
							not_searching_state_counter=0;
							cereg_to_cgreg_counter=0;
							//lac
							ptr_tmp = strstr(ptr_tmp1,",\"");
							if(NULL != ptr_tmp)
							{
								ptr_tmp = ptr_tmp+2;
								//a57e","0abd000c",2,"01"
								lac = strtoul( ptr_tmp,NULL,16 );
								//log_info("%s_%d:lac:%d\n",__FUNCTION__,__LINE__,lac);
								sprintf(global_dial_vars.lac_id,"%d",lac);
								//web get tac(4g)/lac(2g,3g) only from lac_id
								strcpy(global_dial_vars.tac,global_dial_vars.lac_id);
							}
							else
								*Dial_proc_state=Lt10_State_CGREG_QUERY;

							//cellid
							ptr_tmp1 = strstr(ptr_tmp,",\"");
							if(NULL != ptr_tmp1)
							{
								ptr_tmp1 = ptr_tmp1+2;
								//0abd000c",2,"01"
								cid = strtoul( ptr_tmp1,NULL,16 );
								//log_info("%s_%d:cid:%d\n",__FUNCTION__,__LINE__,cid);
								sprintf(global_dial_vars.cell_id,"%d",cid);
							}
							else
								*Dial_proc_state=Lt10_State_CGREG_QUERY;

							//act
							ptr_tmp = strstr(ptr_tmp1,",");
							if(NULL != ptr_tmp)
							{
								ptr_tmp++;
								global_dial_vars.act = atoi(ptr_tmp);
								//log_info("%s_%d:act:%d\n",__FUNCTION__,__LINE__,global_dial_vars.act);
								if(global_dial_vars.act == 0)
								{
									global_dial_vars.reg_status = 1;
									global_dial_vars.greg_status = 0;
									global_dial_vars.ereg_status = 0;
								}
								else if(global_dial_vars.act == 2)
								{
									global_dial_vars.reg_status = 0;
									global_dial_vars.greg_status = 1;
									global_dial_vars.ereg_status = 0;
								}
								else if(global_dial_vars.act == 7)//LT10
								{
									*Dial_proc_state=Lt10_State_CEREG_QUERY;
								}
							}
							else
								*Dial_proc_state=Lt10_State_CEREG_QUERY;
						}
						//not registered,but MT is currently searching a new operator to register to
						else if( global_dial_vars.greg_status == 2 )
						{
							//continue to query
							*Dial_proc_state=Lt10_State_CGREG_QUERY;
							not_searching_state_counter=0;
						}
						//registration denied:3
						//unknown:4
						else if( ( global_dial_vars.greg_status == 3 ) || ( global_dial_vars.greg_status == 4 ) )
						{
							//not to process
							*Dial_proc_state=Lt10_State_CEREG_QUERY;
							not_searching_state_counter=0;
						}
						else if( global_dial_vars.greg_status == 0 )
						{
							not_searching_state_counter += 1;
							if(not_searching_state_counter >= 4)
							{
								//not to process
								*Dial_proc_state=Lt10_State_CEREG_QUERY;
								not_searching_state_counter=0;
							}
						}
					}
				}
			break;
		case Lt10_State_CEREG_QUERY:
				//+CEREG: 2,1,"27bd","0ac17d03",7
				if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
				{
					char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+CEREG: 2,");
					if(NULL!=ptr_tmp)
					{
						char* ptr_tmp1=NULL;
						unsigned int lac,cid;

						ptr_tmp1=ptr_tmp+strlen("+CEREG: 2,");
						global_dial_vars.ereg_status = atoi(ptr_tmp1);
						//registered,1:home network,5:roaming
						if( ( global_dial_vars.ereg_status == 1 ) || ( global_dial_vars.ereg_status == 5 ) )
						{
							//now we can activate the pdp service
							//td_lte network
							*Dial_proc_state=Lt10_State_DDPDN;
							not_searching_state_counter=0;
							cereg_to_cgreg_counter=0;
							//lac
							ptr_tmp = strstr(ptr_tmp1,",\"");
							if(NULL != ptr_tmp)
							{
								ptr_tmp = ptr_tmp+2;
								//27bd","0ac17d03",7
								lac = strtoul( ptr_tmp,NULL,16 );
								//log_info("%s_%d:lac:%d\n",__FUNCTION__,__LINE__,lac);
								sprintf(global_dial_vars.lac_id,"%d",lac);
								//web get tac(4g)/lac(2g,3g) only from lac_id
								strcpy(global_dial_vars.tac,global_dial_vars.lac_id);
							}
							else
								*Dial_proc_state=Lt10_State_CEREG_QUERY;

							//cellid
							ptr_tmp1 = strstr(ptr_tmp,",\"");
							if(NULL != ptr_tmp1)
							{
								ptr_tmp1 = ptr_tmp1+2;
								//27bd","0ac17d03",7
								cid = strtoul( ptr_tmp1,NULL,16 );
								//log_info("%s_%d:cid:%d\n",__FUNCTION__,__LINE__,cid);
								sprintf(global_dial_vars.cell_id,"%d",cid);
							}
							else
								*Dial_proc_state=Lt10_State_CEREG_QUERY;

							//act
							ptr_tmp = strstr(ptr_tmp1,",");
							if(NULL != ptr_tmp)
							{
								ptr_tmp++;
								global_dial_vars.act = atoi(ptr_tmp);
								//log_info("%s_%d:act:%d\n",__FUNCTION__,__LINE__,global_dial_vars.act);
								if(global_dial_vars.act == 7)
								{
									global_dial_vars.reg_status = 0;
									global_dial_vars.greg_status = 0;
									global_dial_vars.ereg_status = 1;
								}
								else
									*Dial_proc_state=Lt10_State_CGREG_QUERY;
							}
							else
								*Dial_proc_state=Lt10_State_CEREG_QUERY;
						}
						//not registered,but MT is currently searching a new operator to register to
						else if( global_dial_vars.ereg_status == 2 )
						{
							//continue to query
							*Dial_proc_state=Lt10_State_CEREG_QUERY;
							not_searching_state_counter=0;
						}
						//registration denied:3
						//unknown:4
						else if( ( global_dial_vars.ereg_status == 3 ) || ( global_dial_vars.ereg_status == 4 ) )
						{
							//not to process
							*Dial_proc_state=Lt10_State_CEREG_QUERY;
							not_searching_state_counter=0;
						}
						else if( ( global_dial_vars.ereg_status == 0 ) )
						{
							//not to process
							not_searching_state_counter += 1;
							if(not_searching_state_counter >= 4)
							{
								//not to process
								*Dial_proc_state=Lt10_State_CGREG_QUERY;
								not_searching_state_counter=0;
								cereg_to_cgreg_counter++;

								if( cereg_to_cgreg_counter >= 20 )
								{
									//not to process
									*Dial_proc_state=Lt10_State_CFUN_DISABLE;
								}
							}
						}
					}
				}
			break;
		default:
			break;
	}			
}

void lt10_sendat(int num)
{
	switch(global_dialtool.Dial_proc_state)
	{
		case Lt10_State_initialized:
			util_send_cmd(global_dialtool.dev_handle,"AT+TZSPEC=0\r",&global_dialtool.pthread_moniter_flag);			//test AT
			break;
		case Lt10_State_CMEE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CMEE=1\r",&global_dialtool.pthread_moniter_flag);  	//report ERROR
			break;
		case Lt10_State_DGSN_QUERY2:
			util_send_cmd(global_dialtool.dev_handle,"AT^DGSN?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_DLTEM_QUERY:
		case Lt10_State_DLTEM_QUERY2:
			util_send_cmd(global_dialtool.dev_handle,"AT^DLTEM?\r",&global_dialtool.pthread_moniter_flag);
			break;	
		/*case Dial_State_DLTEM:
			{
				char buffer[64];
				memset(buffer,0,sizeof(buffer));
				int strategy=util_get_lte_search_network_strategy( &( module_status.config_info ) );
				if( strategy != LTE_SEARCH_NETWORK_NOT_CHANGE )
				{
					snprintf(buffer,sizeof( buffer ),"AT^DLTEM=%d\r",strategy);
				}
				else
				{
					snprintf(buffer,sizeof( buffer ),"AT\r");
				}
				util_send_cmd(global_dialtool.dev_handle,buffer,&global_dialtool.pthread_moniter_flag);
			}
			break;*/
		case Lt10_State_ROAMING_QUERY:
		case Lt10_State_ROAMING_QUERY2:
			util_send_cmd(global_dialtool.dev_handle,"AT^SYSCONFIG?\r",&global_dialtool.pthread_moniter_flag);
			break;	
		case Lt10_State_ROAMING_SET:
			//if( util_should_forbid_roaming( &( module_status.config_info ) ) )
			util_send_cmd(global_dialtool.dev_handle,"AT^SYSCONFIG=16,3,0,4\r",&global_dialtool.pthread_moniter_flag);
			//util_send_cmd(global_dialtool.dev_handle,"AT^SYSCONFIG=16,3,1,4\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_BANDSETEX_TEST:
			util_send_cmd(global_dialtool.dev_handle,"AT^BANDSETEX=?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_BANDSETEX_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT^BANDSETEX?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_DUSIMR_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT^DUSIMR=2\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_DUSIMR_REPORT:
			util_send_cmd(global_dialtool.dev_handle,"AT^DUSIMR=1\r",&global_dialtool.pthread_moniter_flag);
			break;
		//L1761 ICCID:AT^DCID
		case Lt10_State_ICCID:
			util_send_cmd(global_dialtool.dev_handle,"AT^DCID\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CPIN:
			util_send_cmd(global_dialtool.dev_handle,"AT+CPIN?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CPIN_SET:
			{
				char* pin_code=global_init_parms.pin;
				char cmd_buffer[64];
				if(strlen(pin_code))
				{
					snprintf(cmd_buffer,sizeof(cmd_buffer),"AT+CPIN=\"%s\"\r",pin_code);
					util_send_cmd(global_dialtool.dev_handle,cmd_buffer,&global_dialtool.pthread_moniter_flag);
				}
			}
			break;
		case Lt10_State_CIMI:
			util_send_cmd(global_dialtool.dev_handle,"AT+CIMI\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_COPS:
			util_send_cmd(global_dialtool.dev_handle,"AT+COPS=3,2\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_DSTMEX://default setting,need modify based on the value set in the webpage
			if(global_init_parms.network_mode_web == 0)//4G priority
				util_send_cmd(global_dialtool.dev_handle,"AT^DSTMEX=0,7,2,0\r",&global_dialtool.pthread_moniter_flag);
			else if(global_init_parms.network_mode_web == 3)//4G only
				util_send_cmd(global_dialtool.dev_handle,"AT^DSTMEX=1,7\r",&global_dialtool.pthread_moniter_flag);
			else if(global_init_parms.network_mode_web == 13)//3G only
				util_send_cmd(global_dialtool.dev_handle,"AT^DSTMEX=1,2\r",&global_dialtool.pthread_moniter_flag);
			else if(global_init_parms.network_mode_web == 18)//3G priority
				util_send_cmd(global_dialtool.dev_handle,"AT^DSTMEX=0,2,0\r",&global_dialtool.pthread_moniter_flag);
			else if(global_init_parms.network_mode_web == 20)//2G only
				util_send_cmd(global_dialtool.dev_handle,"AT^DSTMEX=1,0\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CEMODE://default setting,if necessary,need modify based on the value set in the webpage
			util_send_cmd(global_dialtool.dev_handle,"AT+CEMODE=2\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_DDPDN:
			{
				char buffer[ 256 ];
				//^DDPDN=<PDN type>,<APN>
				/*
				<PDN type>
				type>:
				1:IPv4
				2:IPv6
				3:IPv4IPv6
				 *
				 * */
				if( strcmp( global_init_parms.ipstack,"IP" ) == 0 )
				{
					//1:IPv4
					if( A_CMP_B_ACCORDING_B(global_init_parms.apn,"0") )
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT^DDPDN=1,\"\"\r");
					}
					else
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT^DDPDN=1,\"%s\"\r",global_init_parms.apn);
					}
				}
				else if( strcmp( global_init_parms.ipstack,"IPV6" ) == 0 )
				{
					//2:IPv6
					if( A_CMP_B_ACCORDING_B(global_init_parms.apn,"0") )
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT^DDPDN=2,\"\"\r");
					}
					else
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT^DDPDN=2,\"%s\"\r",global_init_parms.apn);
					}
				}
				else
				{
					//3:IPv4IPv6
					if( A_CMP_B_ACCORDING_B(global_init_parms.apn,"0") )
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT^DDPDN=3,\"\"\r");
					}
					else
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT^DDPDN=3,\"%s\"\r",global_init_parms.apn);
					}
				}
				util_send_cmd(global_dialtool.dev_handle,buffer,&global_dialtool.pthread_moniter_flag);
			}
			break;
		case Lt10_State_CGDCONT:
			{
				char buffer[ 256 ];
				
				if( strcmp( global_init_parms.ipstack,"IP" ) == 0 )
				{
					//1:IPv4
					if( A_CMP_B_ACCORDING_B(global_init_parms.apn,"0") )
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT+CGDCONT=1,\"%s\",\"\"\r",global_init_parms.ipstack);
					}
					else
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT+CGDCONT=1,\"%s\",\"%s\"\r",global_init_parms.ipstack,global_init_parms.apn);
					}
				}
				else if( strcmp( global_init_parms.ipstack,"IPV6" ) == 0 )
				{
					//2:IPv6
					if( A_CMP_B_ACCORDING_B(global_init_parms.apn,"0") )
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT+CGDCONT=1,\"%s\",\"\"\r",global_init_parms.ipstack);
					}
					else
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT+CGDCONT=1,\"%s\",\"%s\"\r",global_init_parms.ipstack,global_init_parms.apn);
					}
				}
				else
				{
					//3:IPv4IPv6
					if( A_CMP_B_ACCORDING_B(global_init_parms.apn,"0") )
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT+CGDCONT=1,\"%s\",\"\"\r",global_init_parms.ipstack);
					}
					else
					{
						snprintf(buffer
								,sizeof( buffer )
								,"AT+CGDCONT=1,\"%s\",\"%s\"\r",global_init_parms.ipstack,global_init_parms.apn);
					}
				}
				util_send_cmd(global_dialtool.dev_handle,buffer,&global_dialtool.pthread_moniter_flag);
			}
			break;
		case Lt10_State_CFUN_ENABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN=1\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CFUN_DISABLE:
			util_send_cmd(global_dialtool.dev_handle,"AT+CFUN=0\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CREG:
			util_send_cmd(global_dialtool.dev_handle,"AT+CREG=2\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CEREG:
			util_send_cmd(global_dialtool.dev_handle,"AT+CEREG=2\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CGREG:
			util_send_cmd(global_dialtool.dev_handle,"AT+CGREG=2\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CREG_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+CREG?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CEREG_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+CEREG?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CGREG_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT+CGREG?\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CGDATA:
			util_send_cmd(global_dialtool.dev_handle,"AT+CGDATA=\"M-0000\",1\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_CSQ:
			util_send_cmd(global_dialtool.dev_handle,"AT+CSQ\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_SYSINFO:
			util_send_cmd(global_dialtool.dev_handle,"AT^SYSINFO\r",&global_dialtool.pthread_moniter_flag);
			break;
		case Lt10_State_DQDATA_QUERY:
			util_send_cmd(global_dialtool.dev_handle,"AT^DQDATA=3,1\r",&global_dialtool.pthread_moniter_flag);
			break;
		default:
			util_send_cmd(global_dialtool.dev_handle,"AT\r",&global_dialtool.pthread_moniter_flag);
			break;
	}	
}

void lt10_dial(int* Dial_proc_state )
{
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
	switch(*Dial_proc_state)
	{
		case Lt10_State_CGDATA:
		case Lt10_State_CGDATA_CHECK:
			//if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_RESULT_CONNECT))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^DPPPI: 1");
				if(NULL != ptr_tmp)
				{
					char buffer_cmd[256];
					int udhcpc_pid;
					static int udhcpc_sig_count=0;

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
								system("killall udhcpc");//kill all udhcpc?
							}
							log_info("udhcpc send USR1 ERROR:%s\n",strerror(errno));
							goto REGET_IP_BY_UDHCPC;
						}
						udhcpc_sig_count=0;
					}
					else
						system(buffer_cmd);
					global_sleep_interval_long = 1;
					*Dial_proc_state=Lt10_State_CSQ;
				}
			}
			break;
		case Lt10_State_CSQ:
			//if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+CSQ: ");
				if(NULL!=ptr_tmp)
				{
					char* ptr_tmp1=NULL;
					int rsrp, rssi;
					ptr_tmp1=ptr_tmp+strlen("+CSQ: ");
					rssi = atoi(ptr_tmp1);
					if( rssi < 99 )
					{
						rsrp=-( 113-( rssi<<1 ) );
						snprintf(global_dial_vars.signal_rsrp,sizeof(global_dial_vars.signal_rsrp),"%d",rsrp);
					}
					else if( rssi < 199 )
					{
						rsrp=-( 216-rssi );
						snprintf(global_dial_vars.signal_rsrp,sizeof(global_dial_vars.signal_rsrp),"%d",rsrp);
					}
					else if( rssi < 299 )
					{
						rsrp=-( 341-rssi );
						snprintf(global_dial_vars.signal_rsrp,sizeof(global_dial_vars.signal_rsrp),"%d",rsrp);
					}
					*Dial_proc_state=Lt10_State_SYSINFO;
				}
			}
			break;
		case Lt10_State_SYSINFO:
			//if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^SYSINFO: ");
				if(NULL!=ptr_tmp)
				{
					char* ptr_tmp1=NULL;
					//^SYSINFO: 2,3,0,17,1,,25
					ptr_tmp1=ptr_tmp+strlen("^SYSINFO: ");
					//srv_status
					global_dial_vars.service_status = atoi(ptr_tmp1);

					//srv_domain CS PS
					ptr_tmp = strstr(ptr_tmp1,",");
					if(NULL != ptr_tmp)
					{
						ptr_tmp++;
						global_dial_vars.ps_cs_region = atoi(ptr_tmp);
					}

					//roam_status
					ptr_tmp1 = strstr(ptr_tmp,",");
					if(NULL != ptr_tmp1)
					{
						ptr_tmp1++;
						global_dial_vars.roam_status = atoi(ptr_tmp1);
					}
				}
			}
			global_dialtool.Dial_Lvl_1=DIAL_DEAMON;
			*Dial_proc_state=Lt10_State_CSQ;
			break;
		default:
			break;		
	}
}

void lt10_deamon(int* Dial_proc_state)
{
	log_info("%s %d global_dialtool.Dial_proc_state %d\n",__FUNCTION__,__LINE__,global_dialtool.Dial_proc_state);
	switch(*Dial_proc_state)
	{
		case Lt10_State_CSQ:
			//if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"+CSQ: ");
				if(NULL!=ptr_tmp)
				{
					char* ptr_tmp1=NULL;
					int rsrp, rssi;
					ptr_tmp1=ptr_tmp+strlen("+CSQ: ");
					rssi = atoi(ptr_tmp1);
					if( rssi < 99 )
					{
						rsrp=-( 113-( rssi<<1 ) );
						snprintf(global_dial_vars.signal_rsrp,sizeof(global_dial_vars.signal_rsrp),"%d",rsrp);
					}
					else if( rssi < 199 )
					{
						rsrp=-( 216-rssi );
						snprintf(global_dial_vars.signal_rsrp,sizeof(global_dial_vars.signal_rsrp),"%d",rsrp);
					}
					else if( rssi < 299 )
					{
						rsrp=-( 341-rssi );
						snprintf(global_dial_vars.signal_rsrp,sizeof(global_dial_vars.signal_rsrp),"%d",rsrp);
					}
					log_info("global_dial_vars.signal_rsrp: %s", global_dial_vars.signal_rsrp);
					*Dial_proc_state=Lt10_State_DQDATA_QUERY;
				}
			}
			break;
		case Lt10_State_DQDATA_QUERY:
			//if(NULL != strstr(global_dialtool.buffer_at_sponse,CMD_EXE_OK))
			{
				char* ptr_tmp=strstr(global_dialtool.buffer_at_sponse,"^DQDATA: 3,1,");
				if(NULL!=ptr_tmp)
				{
					char* ptr_tmp1=NULL;
					ptr_tmp1=ptr_tmp+strlen("^DQDATA: 3,1,");
					global_dial_vars.network_link = atoi(ptr_tmp1);
					if( global_dial_vars.network_link == 0 )
					{
						global_dialtool.Dial_Lvl_1=DIAL_INIT;
						*Dial_proc_state=Lt10_State_CFUN_DISABLE;
						break;
					}
					*Dial_proc_state=Lt10_State_CSQ;
				}
			}
			sleep(5);
			break;
		default:
			break;
	}
}

void lt10_report_handle(char* buffer)
{
	//log_info("%s %d %s\n",__FUNCTION__,__LINE__,buffer);
	if(NULL!=strstr(buffer,"+CIEV: 2,"))
	{
		char* ptr_tmp=strstr(buffer,"+CIEV: 2,")+strlen("+CIEV: 2,");
		global_dial_vars.signal_rssi_level=atoi(ptr_tmp);
	}

	//+CEREG: 1,"27bd","02bda501",7
	if(NULL != strstr(buffer,"+CEREG: "))
	{
		char* ptr_tmp = NULL;
		char* ptr_tmp1 = NULL;
		unsigned int lac,cid;

		ptr_tmp = strstr(buffer,"+CEREG: ")+strlen("+CEREG: ");
		global_dial_vars.ereg_status = atoi(ptr_tmp);
		//registered,1:home network,5:roaming
		if( ( global_dial_vars.ereg_status == 1 ) || ( global_dial_vars.ereg_status == 5 ) )
		{
			//lac	1,"27bd","02bda501",7
			ptr_tmp1 = strstr(ptr_tmp,",\"");
			if(NULL != ptr_tmp1)
			{
				ptr_tmp1 = ptr_tmp1+2;
				//27bd","02bda501",7
				lac = strtoul( ptr_tmp1,NULL,16 );
				//log_info("%s_%d:lac:%d\n",__FUNCTION__,__LINE__,lac);
				sprintf(global_dial_vars.lac_id,"%d",lac);
				//web get tac(4g)/lac(2g,3g) only from lac_id
				strcpy(global_dial_vars.tac,global_dial_vars.lac_id);
			}
			else
				return;

			//cellid 27bd","02bda501",7
			ptr_tmp = strstr(ptr_tmp1,",\"");
			if(NULL != ptr_tmp)
			{
				ptr_tmp = ptr_tmp+2;
				//02bda501",7
				cid = strtoul( ptr_tmp,NULL,16 );
				//log_info("%s_%d:cid:%d\n",__FUNCTION__,__LINE__,cid);
				sprintf(global_dial_vars.cell_id,"%d",cid);
			}
			else
				return;

			//act
			ptr_tmp1 = strstr(ptr_tmp,",");
			if(NULL != ptr_tmp1)
			{
				ptr_tmp1++;
				global_dial_vars.act = atoi(ptr_tmp1);
				//log_info("%s_%d:act:%d\n",__FUNCTION__,__LINE__,global_dial_vars.act);
				if(global_dial_vars.act == 7)
				{
					global_dial_vars.reg_status = 0;
					global_dial_vars.greg_status = 0;
					global_dial_vars.ereg_status = 1;
				}
			}
			else
				return;
		}
		//log_info("%s %d \n",__FUNCTION__,__LINE__);
	}

	//+CGREG: 1,"27bd","02bda501",2
	if(NULL != strstr(buffer,"+CGREG: "))
	{
		char* ptr_tmp = NULL;
		char* ptr_tmp1 = NULL;
		unsigned int lac,cid;

		ptr_tmp = strstr(buffer,"+CGREG: ")+strlen("+CGREG: ");
		global_dial_vars.ereg_status = atoi(ptr_tmp);
		//registered,1:home network,5:roaming
		if( ( global_dial_vars.ereg_status == 1 ) || ( global_dial_vars.ereg_status == 5 ) )
		{
			//lac	1,"27bd","02bda501",2
			ptr_tmp1 = strstr(ptr_tmp,",\"");
			if(NULL != ptr_tmp1)
			{
				ptr_tmp1 = ptr_tmp1+2;
				//27bd","02bda501",2
				lac = strtoul( ptr_tmp1,NULL,16 );
				//log_info("%s_%d:lac:%d\n",__FUNCTION__,__LINE__,lac);
				sprintf(global_dial_vars.lac_id,"%d",lac);
				//web get tac(4g)/lac(2g,3g) only from lac_id
				strcpy(global_dial_vars.tac,global_dial_vars.lac_id);
			}
			else
				return;

			//cellid 27bd","02bda501",2
			ptr_tmp = strstr(ptr_tmp1,",\"");
			if(NULL != ptr_tmp)
			{
				ptr_tmp = ptr_tmp+2;
				//02bda501",2
				cid = strtoul( ptr_tmp,NULL,16 );
				//log_info("%s_%d:cid:%d\n",__FUNCTION__,__LINE__,cid);
				sprintf(global_dial_vars.cell_id,"%d",cid);
			}
			else
				return;

			//act
			ptr_tmp1 = strstr(ptr_tmp,",");
			if(NULL != ptr_tmp1)
			{
				ptr_tmp1++;
				global_dial_vars.act = atoi(ptr_tmp1);
				//log_info("%s_%d:act:%d\n",__FUNCTION__,__LINE__,global_dial_vars.act);
				if(global_dial_vars.act == 0)
				{
					global_dial_vars.reg_status = 1;
					global_dial_vars.greg_status = 0;
					global_dial_vars.ereg_status = 0;
				}
				else if(global_dial_vars.act == 2)
				{
					global_dial_vars.reg_status = 0;
					global_dial_vars.greg_status = 1;
					global_dial_vars.ereg_status = 0;
				}
			}
			else
				return;
		}
		//log_info("%s %d \n",__FUNCTION__,__LINE__);
	}
}

DialProc process_lt10=
{
	lt10_init,
	lt10_dial,
	lt10_deamon,
	lt10_sendat,
	lt10_report_handle,
};


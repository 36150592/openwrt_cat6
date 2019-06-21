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
#include "cwmp_agent.h"
#include <cwmp/session.h>
#include "modules/data_model.h"
#include "cmdlib.h"

#define CWMP_TRUE   1

#define MAX_SESSION_RETRY 3



enum
{
    CWMP_ST_START = 0, CWMP_ST_INFORM, CWMP_ST_SEND, CWMP_ST_RESEMD, CWMP_ST_RECV, CWMP_ST_ANSLYSE, CWMP_ST_RETRY, CWMP_ST_END, CWMP_ST_EXIT
};





int cwmp_agent_retry_session(cwmp_session_t * session)
{

    int sec = 0;

    srand(time(NULL));
    switch (session->retry_count)
    {
    case 0:
    {
        sec = 5 + rand()%5; //5~10
        break;
    }
    case 1:
    {
        sec = 5 + rand()%10; //5~15
        break;
    }
    case 2:
    {
        sec = 5 + rand()%20; //5~25
        break;
    }
    default:
    {
        sec = 5 + rand()%30; //5~35
        break;
    }
    }

    while (sec>0)
    {
        sleep(1);
        sec--;
    }

    if (session->retry_count > MAX_SESSION_RETRY)
    {
        session->retry_count = 0;
        return CWMP_TIMEOUT;
    }
    else
    {
        session->retry_count ++;
        return CWMP_OK;
    }

}


int cwmp_agent_create_datetimes(datatime_t *nowtime)
{
    struct tm t;
    time_t tn;
    

    //FUNCTION_TRACE();
    tn = time(NULL);
#ifdef WIN32
    cwmp_log_debug("inform datatime");
    //localtime_s(&t, &tn);
    memset(&t, 0, sizeof(struct tm));
#else
    t = *localtime(&tn);
#endif

    nowtime->year = t.tm_year + 1900;
    nowtime->month = t.tm_mon + 1;
    nowtime->day = t.tm_mday;
    nowtime->hour = t.tm_hour;
    nowtime->min = t.tm_min;
    nowtime->sec = t.tm_sec;

    return CWMP_OK;
}

int cwmp_check_if_active_event_is_exist(cwmp_t *cwmp)
{
    //event_code_t * ev;
    int i=0;
    int flag=0;
    
    //FUNCTION_TRACE();

    event_code_t ** pec = cwmp->el->events;
    int elsize = cwmp->el->count;
    
    for(i=0; i<elsize; i++)
    {      
		if(pec[i]  && pec[i]->ref > 0)
        {
         	flag = 1;
            break;	
        }
    }
    
    return flag;
}




int cwmp_agent_send_request(cwmp_session_t * session)
{
    FUNCTION_TRACE();
    return cwmp_session_send_request(session);
}

int cwmp_agent_recv_response(cwmp_session_t * session)
{
    return cwmp_session_recv_response(session);
}

int clear_and_cwmp_event_set_value(cwmp_t *cwmp,  int event,   int value, const char * cmd_key, int fault_code, time_t start, time_t end)
{
	cwmp_event_clear_active(cwmp);
	cwmp_event_set_value(cwmp,  event,   value, cmd_key, fault_code, start, end);
	return CWMP_OK;
}

int clear_and_cwmp_event_set_array(cwmp_t *cwmp,   int count, int event[],   int value[],const char * cmd_key[], int fault_code[], time_t start[], time_t end[])
{
	int i;
	cwmp_event_clear_active(cwmp);

	for(i=0;i<count;i++)
		cwmp_event_set_value(cwmp,  event[i], value[i], 
							(cmd_key==NULL)?NULL:cmd_key[i], 
							(fault_code==NULL)?0:fault_code[i], 
							(start==NULL)?0:start[i], 
							(end==NULL)?0:end[i]);
	return CWMP_OK;
}


static cwmp_t *heart_beat_cwmp = NULL; 

extern int get_single_config_attr(char *name, char *value);

void heart_beat(int sig)
{
    int inform_interval = cwmp_conf_get_int("cwmp:interval");

    cwmp_event_set_value(heart_beat_cwmp, INFORM_PERIODIC, 1, NULL, 0, 0, 0);
    // signal(SIGALRM, heart_beat);
    alarm(inform_interval);
}
/*
void heart_beat(int sig)
{
	cwmp_log_debug("\r\n\r\n\r\nsig=%d", sig);

	char sEnabled[20], sInterval[20];

    if(isTestConfig())
    {
        get_single_config_attr("TZ_TR069_NOTICE_ENABLED2", sEnabled);
        get_single_config_attr("TZ_TR069_NOTICE_INTERVAL2", sInterval);
    }
    else
    {
        get_single_config_attr(TZ_TR069_NOTICE_ENABLED, sEnabled);
        get_single_config_attr(TZ_TR069_NOTICE_INTERVAL, sInterval);
    }

	if (strlen(sEnabled) == 0 || (strncmp(sEnabled, STR_TRUE, sizeof(STR_TRUE)) == 0)
		|| (strncmp(sEnabled, "1", 1) == 0))
	{
		int inform_interval = atoi(sInterval);
		if (inform_interval == 0)
		{
			inform_interval = 43200;
		}

		if (inform_interval > 12) {
			// minus 10 seconds(sleep time) when get data
			inform_interval = inform_interval - 10;
		}
		
	    cwmp_event_set_value(heart_beat_cwmp, INFORM_PERIODIC, 1, NULL, 0, 0, 0);
		signal(SIGALRM, heart_beat);
		alarm(inform_interval);
	} 
}
*/
static int is_first_heart_beat = 1;
void start_heart_beat(cwmp_t * cwmp)
{
	heart_beat_cwmp = cwmp;
	signal(SIGALRM, heart_beat);
	int seconds = cwmp_conf_get_int("cwmp:interval");
	if (seconds <= 0) seconds = 240;

	if (is_first_heart_beat) {
		is_first_heart_beat = 0;
		if (seconds < 30) {
			seconds = 30;
		}
	}
	
	alarm(seconds);
}
/*
void start_heart_beat(cwmp_t * cwmp)
{
	heart_beat_cwmp = cwmp;
	signal(SIGALRM, heart_beat);
	char interval[16];
    if(isTestConfig())
       get_single_config_attr("TZ_TR069_NOTICE_INTERVAL2", interval);
   else
       get_single_config_attr(TZ_TR069_NOTICE_INTERVAL, interval);

	int seconds = atoi(interval);
	if (seconds <= 0) seconds = 240;

	if (is_first_heart_beat) {
		is_first_heart_beat = 0;
		if (seconds < 30) {
			seconds = 30;
		}
	}
	
	alarm(seconds);
	//alarm(5);
}
*/
extern void cwmp_log_memory();
int cwmp_agent_run_tasks(cwmp_t * cwmp);

void cwmp_agent_start_session(cwmp_t * cwmp)
{
	//static int cnt=0;
    int rv;
    cwmp_session_t * session;
    int session_close = CWMP_NO;
    xmldoc_t * newdoc;
    FUNCTION_TRACE();
	
	int is_boot_success = 0;
	int is_first_heart_beat = 1;
	int reconnect_count = 0;
    int seconds;
    char cmdbuf[128]={0};
    char value[64]={0};
    FILE* file_handle;    

    while (TRUE)
    {
		cwmp_log_memory();
    		
        if (cwmp->new_request == CWMP_NO)
        {
        	//lizd
            cwmp_log_debug("No new req");
            sleep(1);
			
			if(is_first_heart_beat || cmd_file_exist(TR069_INFORM_FLAG_FILE)){
				is_first_heart_beat = 0;
				remove(TR069_INFORM_FLAG_FILE);
	    		start_heart_beat(cwmp);
			}

            // set INTERVAL_TIME
			if(cmd_file_exist(TR069_INTERVAL_TIME_FILE)){

                file_handle=fopen( TR069_INTERVAL_TIME_FILE,"rb" );
                //打开文件失败
                if( file_handle == NULL )
                {
                    
                }
                else
                {
                    //读取文件的内容
                   
                    fread( value,sizeof( value ),1,file_handle );

                    seconds = atoi(value);
	                if (seconds <= 30) 
                    {
                        strcpy(value,"30");
                    }else if(seconds > 240) {
                         strcpy(value,"240");
                    }
                    if(cwmp_conf_get_int("cwmp:interval") != seconds)
                    {
                        cwmp_conf_set("cwmp:interval",value);
                        
                        sprintf(cmdbuf,"uci -q set tozed.cfg.tr069_PeriodicInformInterval=%d;uci -q commit tozed", seconds);

                        system(cmdbuf); 
                    }
                }
                //关掉文件句柄
                fclose( file_handle );
				remove(TR069_INTERVAL_TIME_FILE);
			}
            

            /*cnt++;
            if(cnt==5){
				clear_and_cwmp_event_set_value(cwmp, INFORM_XCMCC_BIND, 1, NULL, 0, 0, 0);
            }*/

            if(cwmp_check_if_active_event_is_exist(cwmp)==0)
            	continue ;

            // cwmp_log_debug("cwmp_check_if_active_event_is_exist Not Zero\n"); 	
        }
        
        cwmp_log_debug("New request from ACS\n");

        cwmp->new_request = CWMP_NO;
        session = cwmp_session_create(cwmp);
        session_close  = CWMP_NO;
        session->timeout = CWMP_HTTP_TIMEOUT;//cwmp_conf_get_int("cwmpd:http_timeout");
        //cwmp_session_set_timeout(cwmp_conf_get_int("cwmpd:http_timeout"));
        //cwmp_log_debug("session timeout is %d", session->timeout);
		
        cwmp_session_open(session);

        while (!session_close)
        {
            //cwmp_log_debug("session status: %d", session->status);
            static int debug_session_st = -1;
            if(debug_session_st != session->status){
				cwmp_log_debug("session status: %d", session->status);
				debug_session_st = session->status;
            }
			
			cwmp_log_memory();

            switch (session->status)
            {
            case CWMP_ST_START:
                //create a new connection to acs
                cwmp_log_debug(">>>>>>SESSION STATUS: NEW START");
				cwmp_log_memory();

                if (cwmp_session_connect(session, cwmp->acs_url) != CWMP_OK)
                {
                    cwmp_log_error("connect to acs: %s failed.\n", cwmp->acs_url);
                    session->status = CWMP_ST_RETRY;
                }
                else
                {
                    load_params();
                    cwmp_model_init_object(cwmp, cwmp->root);
                    session->status = CWMP_ST_INFORM;
                }
                break;
            case CWMP_ST_INFORM:
				cwmp_log_debug(">>>>>>SESSION STATUS: INFORM");
				cwmp_log_memory();
				
                if (cwmp->acs_auth)
                {
					cwmp_log_debug("CWMP_ST_INFORM set auth");			
                    cwmp_session_set_auth(session,   cwmp->acs_user  , cwmp->acs_pwd );
                }				

                newdoc = cwmp_session_create_inform_message(session, session->envpool);
                cwmp_write_doc_to_chunk(newdoc, session->writers,  session->envpool);
                session->last_method = CWMP_INFORM_METHOD;
                session->status = CWMP_ST_SEND;

                break;

            case CWMP_ST_SEND:
                cwmp_log_debug(">>>>>>SESSION STATUS: SEND");
				cwmp_log_memory();
				
                cwmp_log_debug("session data request length: %d", cwmp_chunk_length(session->writers));
                session->newdata = CWMP_NO;

                rv = cwmp_agent_send_request(session);
                

                if (rv == CWMP_OK)
                {
					cwmp_log_debug("session data sended OK, rv=%d", rv);
					session->status = CWMP_ST_RECV;
                }
				else
				{
					cwmp_log_debug("session data sended faild! rv=%d", rv);
					session->status = CWMP_ST_EXIT;
					
					/*
					if (rv == CWMP_COULDNOT_CONNECT)
					{
						session->status = CWMP_ST_RETRY;
					}
					else
                    {
                        session->status = CWMP_ST_EXIT;
                    }
					*/
                }
                break;
				
            case CWMP_ST_RECV:
                cwmp_log_debug(">>>>>>SESSION STATUS: RECV");
				cwmp_log_memory();
                cwmp_chunk_clear(session->readers);

                rv = cwmp_agent_recv_response(session);

                if (rv == CWMP_OK)
                {
                    session->status = CWMP_ST_ANSLYSE;
                }
                else
                {
                    session->status = CWMP_ST_END;
                }
                break;

            case CWMP_ST_ANSLYSE:
                cwmp_log_debug(">>>>>>SESSION STATUS: ANSLYSE");
				cwmp_log_memory();
                rv = cwmp_agent_analyse_session(session);
                if (rv == CWMP_OK)
                {
                	if (session->close) {
						if (!is_boot_success && reconnect_count++ < 100) {
							// boot send failed, then reconnect
							session->reconnect = CWMP_YES;
							cwmp_log_debug("Boot failed, sleep 10 seconds, then reconnect");
							sleep(10);
						}
                    	session->status = CWMP_ST_END;
					} else {
						is_boot_success = 1;
                    	session->status = CWMP_ST_SEND;
					}
        			
                    if (access(TR069_BOOTSTRAP_FLAG, F_OK) < 0)
                    {
                        cmd_touch(TR069_BOOTSTRAP_FLAG);
                    }
                }
                else
                {
                    session->status = CWMP_ST_END;
                }
                break;
            case CWMP_ST_RETRY:
				cwmp_log_debug(">>>>>>SESSION STATUS: RETRY");
				cwmp_log_memory();
                if (cwmp_agent_retry_session(session) == CWMP_TIMEOUT)
                {
					cwmp_log_debug("session retry timeover, go out");
                    session->status = CWMP_ST_EXIT;
                }
                else
                {
                    session->status = CWMP_ST_START;
                }
                break;
            case CWMP_ST_END:
                //close connection of ACS
                cwmp_log_debug(">>>>>>SESSION STATUS: END");
				//run task from queue
		
                if (session->newdata == CWMP_YES && session->close == CWMP_NO)
                {
                    session->status = CWMP_ST_SEND;
                }
                else
                {
                    session->status = CWMP_ST_EXIT;
                }
                break;

            case CWMP_ST_EXIT:
                cwmp_log_debug(">>>>>>SESSION STATUS: EXIT");
				cwmp_log_memory();
                if (session->reconnect == CWMP_YES)
                {
                    session->reconnect = CWMP_NO;
                    session->status = CWMP_ST_START;
                } else {
                	session_close = CWMP_YES;
                	cwmp_session_close(session);
                }
                break;


            default:
				cwmp_log_debug(">>>>>>SESSION STATUS: Unknown session stutus");
				cwmp_log_memory();
                break;
            }//end switch



        }//end while(!session_close)

        cwmp_log_debug("session stutus: EXIT");
        cwmp_session_free(session);
        session = NULL;

		int newtaskres = cwmp_agent_run_tasks(cwmp);
		if(newtaskres == CWMP_YES)
		{
			cwmp->new_request = CWMP_YES;
		}
	

    }//end while(TRUE)

}


xmldoc_t * cwmp_end_tranfer_session(cwmp_session_t * session)
{
	cwmp_t * cwmp = session->cwmp;
	pool_t * doctmppool  = NULL;
	xmldoc_t *   newdoc = NULL;
	
	if(access(TR069_UPDATE_REBOOT_FLAG, F_OK) >= 0) //(cwmp->event_global.event_flag & EVENT_REBOOT_TRANSFERCOMPLETE_FLAG)
	{
		remove(TR069_UPDATE_REBOOT_FLAG);
		
		cwmp->event_global.event_flag &=  ~EVENT_REBOOT_TRANSFERCOMPLETE_FLAG;
		if(!doctmppool)
		{
			doctmppool = pool_create(POOL_DEFAULT_SIZE);
		}
		event_code_t ec;
		ec.event = INFORM_TRANSFERCOMPLETE;
		TRstrncpy(ec.command_key, cwmp->event_global.event_key, COMMAND_KEY_LEN);
		ec.fault_code = cwmp->event_global.fault_code;
		ec.start = cwmp->event_global.start;
		ec.end = cwmp->event_global.end;
		newdoc = cwmp_session_create_transfercomplete_message(session, &ec, doctmppool);	

	}

	return newdoc;
}

int cwmp_agent_analyse_session(cwmp_session_t * session)
{
    pool_t * doctmppool  = NULL;
    char * xmlbuf;
    cwmp_uint32_t len;
    xmldoc_t *  doc;
    char * method;
    xmldoc_t *   newdoc = NULL;
	cwmp_t * cwmp;
    int rc;

    static char * xml_fault = "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns=\"urn:dslforum-org:cwmp-1-0\"><SOAP-ENV:Body SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"  id=\"_0\"><SOAP-ENV:Fault>Error Message</SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>";

    cwmp_uint32_t msglength = cwmp_chunk_length(session->readers);

    FUNCTION_TRACE();
    
    if (msglength<= 0)
    {
        session->newdata = CWMP_NO;
        cwmp_log_debug("analyse receive length is 0");
	goto eventcheck;
//        return CWMP_ERROR;
    }
    
    doctmppool = pool_create(POOL_DEFAULT_SIZE);

    xmlbuf = pool_palloc(doctmppool, msglength+32);

    // len = sprintf(xmlbuf,"<cwmp>");
    cwmp_chunk_copy(xmlbuf, session->readers, msglength);
    // strcpy(xmlbuf+len+msglength, "</cwmp>");

    cwmp_log_debug("agent analyse xml: \n%s", xmlbuf);

	cwmp_log_memory();
    doc = XmlParseBuffer(doctmppool, xmlbuf);
	cwmp_log_memory();
	
    if (!doc)
    {
        cwmp_log_debug("analyse create doc null\n");
        cwmp_chunk_write_string(session->writers, xml_fault, TRstrlen(xml_fault), session->envpool);
        goto finished;

    }
	cwmp_log_memory();

    method = cwmp_get_rpc_method_name(doc);
    cwmp_log_debug("analyse method is: %s\n", method);
	cwmp_log_memory();

    cwmp_chunk_clear(session->writers);
    pool_clear(session->envpool);
	cwmp_log_memory();

    cwmp = session->cwmp;
    if (TRstrcmp(method, CWMP_RPC_GETRPCMETHODS) == 0)
    {
        newdoc = cwmp_session_create_getrpcmethods_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_INFORMRESPONSE) == 0)
    {
        newdoc = NULL;
    }
    else if (TRstrcmp(method, CWMP_RPC_GETPARAMETERNAMES) == 0)
    {
        newdoc = cwmp_session_create_getparameternames_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_GETPARAMETERVALUES) == 0)
    {
        newdoc = cwmp_session_create_getparametervalues_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_SETPARAMETERVALUES) == 0)
    {
        newdoc = cwmp_session_create_setparametervalues_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_SETPARAMETERATTRIBUTES) == 0)
    {
        newdoc = cwmp_session_create_setparameterattributes_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_GETPARAMETERATTRIBUTES) == 0)
    {
    	//cwmp_log_info("walk_parameter(session->root)====================");
    	//walk_parameter(session->root);

        newdoc = cwmp_session_create_getparameterattributes_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_DOWNLOAD) == 0)
    {
        newdoc = cwmp_session_create_download_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_UPLOAD) == 0)
    {
        newdoc = cwmp_session_create_upload_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_TRANSFERCOMPLETERESPONSE) == 0)
    {
        newdoc = NULL;
    }
    else if (TRstrcmp(method, CWMP_RPC_REBOOT) == 0)
    {
        newdoc = cwmp_session_create_reboot_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_ADDOBJECT) == 0)
    {
        newdoc = cwmp_session_create_addobject_response_message(session, doc, doctmppool);
    }
    else if (TRstrcmp(method, CWMP_RPC_DELETEOBJECT) == 0)
    {
        newdoc = cwmp_session_create_deleteobject_response_message(session, doc, doctmppool);
    }
    
    else if (TRstrcmp(method, CWMP_RPC_FACTORYRESET) == 0)
    {
        newdoc = cwmp_session_create_factoryreset_response_message(session, doc, doctmppool);
    }
	
    else
    {
    	//check event queue
    	//newdoc = cwmp_session_create_event_response_message(session, doc, doctmppool);

    }


    if(newdoc == NULL)
    {
        cwmp_log_debug("agent analyse newdoc is null. ");
		
eventcheck:
	{
		cwmp = session->cwmp;
	   	
		//cwmp_log_debug("agent analyse begin check global event, %d", cwmp->event_global.event_flag);
		//check global event for transfercomplete

		// lizd
		if(access(TR069_UPDATE_REBOOT_FLAG, F_OK)>=0) //(cwmp->event_global.event_flag & EVENT_REBOOT_TRANSFERCOMPLETE_FLAG)
		{
			cwmp_log_debug("get tr069_update_then_reboot");
			remove(TR069_UPDATE_REBOOT_FLAG);
			
			cwmp->event_global.event_flag &=  ~EVENT_REBOOT_TRANSFERCOMPLETE_FLAG;
			if(!doctmppool)
			{
				doctmppool = pool_create(POOL_DEFAULT_SIZE);
			}
			event_code_t ec;
			ec.event = INFORM_TRANSFERCOMPLETE;
			TRstrncpy(ec.command_key, cwmp->event_global.event_key, COMMAND_KEY_LEN);
			ec.fault_code = cwmp->event_global.fault_code;
			ec.start = cwmp->event_global.start;
			ec.end = cwmp->event_global.end;
			newdoc = cwmp_session_create_transfercomplete_message(session, &ec, doctmppool);	


			cwmp->event_global.event_flag = EVENT_REBOOT_NONE_FLAG;
			cwmp_event_clear_active(cwmp);
		}
		
	}

    }


    cwmp_log_debug("newdoc %p, msglength: %d", newdoc, msglength );
    if((newdoc != NULL) || (newdoc == NULL && msglength != 0)) // || (newdoc == NULL && msglength == 0 && session->retry_count < 2))
    {
        session->newdata = CWMP_YES;
        cwmp_write_doc_to_chunk(newdoc, session->writers,  session->envpool);
		rc = CWMP_OK;
    }
    else
    {  	
		rc = CWMP_ERROR;
    }
finished:
	if(doctmppool  != NULL)
	{
	    pool_destroy(doctmppool);
	}
    return rc;
}


void walk_parameter_node_tree(parameter_node_t * param, int level)
{
  if(!param) return; 
 
  parameter_node_t * child;
  char fmt[128];
  //cwmp_log_debug("name: %s, type: %s, level: %d\n", param->name, cwmp_get_type_string(param->type), level);
  
  // lizd	
  sprintf(fmt, "|%%-%ds%%s,  get:%%p set:%%p refresh:%%p", level*4);
  cwmp_log_debug(fmt, "----", param->name, param->get, param->set, param->refresh);

  child = param->child;

  if(!child)
	return;
  walk_parameter_node_tree(child, level+1);

  parameter_node_t * next = child->next_sibling;

  while(next)
 {
    walk_parameter_node_tree(next, level+1);
    next = next->next_sibling;
 }

	
}

void cwmp_agent_session(cwmp_t * cwmp)
{
    char name[1024] = {0};
    char value[1024]= {0};
    char local_ip[32];

    char * envstr;
    char * encstr;

    envstr = "SOAP-ENV"; //cwmp_conf_get("cwmp:soap_env");
    encstr = "SOAP-ENC"; // cwmp_conf_get("cwmp:soap_enc");
    
    cwmp_log_error("cwmp_agent_session start...");

    cwmp_set_envelope_ns(envstr, encstr);
    
    cwmp_agent_start_session(cwmp);
}


/*
int cwmp_agent_download_file(download_arg_t * dlarg)
{
    int faultcode = 0;
    char url[255];
    if (TRstrncasecmp(dlarg->url,  "ftp://", 6) == 0)
    {
        if (dlarg->username != NULL && strlen(dlarg->username) != 0)
        {
            TRsnprintf(url, 255, "ftp://%s:%s@%s", dlarg->username, dlarg->password, dlarg->url+6);
        }
        else
        {
            TRstrncpy(url, dlarg->url, 255);
        }
    }
    else
    {
        TRstrncpy(url, dlarg->url, 255);
    }

    fetchIO * downfile =   fetchGetURL(url, "");
    if (!downfile)
    {
        cwmp_log_debug("download file fail:  %s", url);
        faultcode = 9001;
    }
    else
    {
        char readbuf[1024];
        int readlen;
        char targetfile[64];
        if (dlarg->targetname != NULL && strlen(dlarg->targetname) != 0)
        {
            TRsnprintf(targetfile, 64, "/tmp/%s", dlarg->targetname);
        }
        else
        {
            TRsnprintf(targetfile, 64, "/tmp/%d.file", time(NULL));
        }
        FILE * df = fopen(targetfile, "wb+");
        while (df != NULL)
        {
            readlen = fetchIO_read(downfile, readbuf, 1023);
            if (readlen <= 0)
            {
                cwmp_log_debug("fetch io read zero. %s", readlen);
                break;
            }
            readbuf[readlen] = 0;
            fwrite(readbuf, readlen, 1, df);
        }

        if (df)
        {
            fclose(df);
        }
        else
        {
            faultcode = 9001;
        }

        cwmp_log_debug("download file ok:  %s, %s", url, targetfile);


        fetchIO_close(downfile);
    }


    return faultcode;

}



int cwmp_agent_upload_file(upload_arg_t * ularg)
{
    int faultcode = 0;
    char url[255];

    if (TRstrncasecmp(ularg->url,  "ftp://", 6) == 0)
    {
        if (ularg->username != NULL && strlen(ularg->username) != 0)
        {
            TRsnprintf(url, 255, "ftp://%s:%s@%s", ularg->username, ularg->password, ularg->url+6);
        }
        else
        {
            TRstrncpy(url, ularg->url, 255);
        }
    }
    else
    {
        TRstrncpy(url, ularg->url, 255);
    }

    fetchIO * uploadfile =   fetchPutURL(url, "");
    if (!uploadfile)
    {
        cwmp_log_debug("upload file fail:  %s", url);
        faultcode = 9001;
    }
    else
    {
        char readbuf[1024];
        int readlen;
	char targetfile[64];
	FILE * uf;
	int rc;
	if(strcmp(ularg->filetype, "1 Vendor Configuration File") == 0)
	{
		//¸ù¾ÝÊµ¼ÊÇé¿ö, ÐÞ¸ÄÕâÀïµÄÅäÖÃÎÄ¼þÂ·¾¶
		
		uf = fopen("/tmp/mysystem.cfg", "rb");		
	}
	else if(strcmp(ularg->filetype, "2 Vendor Log File") == 0)
	{
		//¸ù¾ÝÊµ¼ÊÇé¿ö, ÐÞ¸ÄÕâÀïµÄÅäÖÃÎÄ¼þÂ·¾¶
		uf = fopen("/tmp/mysystem.log", "rb");	
	}
	else
	{
		uf = fopen("/tmp/mysystem.log", "rb");	
	}

		
        while (uf != NULL)
        {
            readlen = fread(readbuf, 1024, 1,  uf);
            if (readlen <= 0)
            {
                cwmp_log_debug("fetch io read zero. %s", readlen);
			
                break;
            }
            readbuf[readlen] = 0;

	   rc = fetchIO_write(uploadfile, readbuf, readlen);
	   if(rc <= 0)
	   {
		faultcode = 9001;
		break;
	   }
		
        }

	if(uf)
	{
		fclose(uf);
	}
	else
	{
		faultcode = 9001;
	}
	


        cwmp_log_debug("upload file finished:  %s, file:%s", url, targetfile);


        fetchIO_close(uploadfile);
    }


    return faultcode;

}
*/

extern int http_receive_file(const char *fromurl, const char * tofile);
extern int http_send_file(const char * fromfile, const char *tourl );
extern int write_sys_log(char *value);

int cwmp_agent_download_file(download_arg_t * dlarg)
{
    int faultcode = 0;
    char * fromurl = dlarg->url;
    char * tofile = "/tmp/download.img";

    FUNCTION_TRACE();
    
	struct download_arg_st
	{
		char			*cmdkey;
		char			*filetype;
		char			*url;
		char			*username;
		char			*password;
		char			*targetname;
		char			*succurl;
		char			*failurl;
		unsigned int   delaysec;
		unsigned int   filesize;
	};

	//lizd
    cwmp_log_info("cwmp_agent_download_file url[%s] usr[%s] pwd[%s] type[%s] fsize[%d]\r\n",
    			dlarg->url, 
    			dlarg->username, 
    			dlarg->password, 
    			dlarg->filetype, 
    			dlarg->filesize);
	faultcode = CWMP_OK;

	
    faultcode = http_receive_file(fromurl, tofile);
	if(faultcode == 200) // HTTP/1.1 200
		faultcode = CWMP_OK;
		
    if(faultcode != CWMP_OK)
    {
	    faultcode = 9001;
    }

    return faultcode;

}


/*
int cwmp_agent_upload_file(upload_arg_t * ularg)
{
    int faultcode = 0;
    FUNCTION_TRACE();
    char * fromfile;

	cwmp_log_info("ularg->filetype: %s", ularg->filetype);

	if(strcmp(ularg->filetype, "1 Vendor Configuration File") == 0)
	{
		//¸ù¾ÝÊµ¼ÊÇé¿ö, ÐÞ¸ÄÕâÀïµÄÅäÖÃÎÄ¼þÂ·¾¶

		system("cfg -e > /tmp/.src_tmp_config_file");
		system("configmaker -s /tmp/.src_tmp_config_file");
		system( "md5sum "GENERATED_TMP_CONFIG_FILE"|cut -f1 -d\" \" > "GENERATED_TMP_CONFIG_FILE_MD5_SUM );
		cmd_touch( SYNC_MODULE_INFO_FLAG_FILE );
		fromfile = GENERATED_TMP_CONFIG_FILE;
	}
	else if(strcmp(ularg->filetype, "2 Vendor Log File") == 0)
	{
		//¸ù¾ÝÊµ¼ÊÇé¿ö, ÐÞ¸ÄÕâÀïµÄÅäÖÃÎÄ¼þÂ·¾¶
		cmd_cp(SYS_LOG_FILE_NAME, SYS_TMP_LOG_FILE_NAME);
		fromfile = SYS_TMP_LOG_FILE_NAME;
	}
	else
	{
		cmd_cp(SYS_LOG_FILE_NAME, SYS_TMP_LOG_FILE_NAME);
		fromfile = SYS_TMP_LOG_FILE_NAME;
	}
	cwmp_log_info("Upload: %s", fromfile);
	
    faultcode = http_send_file(fromfile, ularg->url);

    if(faultcode != CWMP_OK)
    {
		faultcode = 9001;
    }

    return faultcode;
}
*/
int cwmp_agent_upload_file(upload_arg_t * ularg)
{
    int faultcode = 0;
    FUNCTION_TRACE();
    char * fromfile;

    cwmp_log_info("ularg->filetype: %s", ularg->filetype);
    
    if(strcmp(ularg->filetype, "1 Vendor Configuration File") == 0)
    {
    	system("sysupgrade -b /tmp/config_black.tar.gz");
    	fromfile = "/tmp/config_black.tar.gz";
    }
    else if(strcmp(ularg->filetype, "2 Vendor Log File") == 0)
    {
    	system("logread >/tmp/vendor_logfile.log");
    	fromfile = "/tmp/vendor_logfile.log";
    }
    else
    {
        fromfile = "";
    }
    cwmp_log_info("Upload: %s", fromfile);
	
    faultcode = http_send_file(fromfile, ularg->url);

    if(faultcode != CWMP_OK)
    {
        faultcode = 9001;
    }

    return faultcode;
}

void restore_factory()
{
	write_sys_log("TR069: factory reset ...");
	system("/etc/tozed/tz_restore_factory&&reboot &");
}

enum REBOOT_TYPE{
	NORMAL_REBOOT = 1,
	CONFIG_CHANGE = 2,
	RESTORE_SETTING = 3
};

void process_reboot(int rebootType){
	switch(rebootType){
		case NORMAL_REBOOT:
			cmd_touch( NEED_TO_REBOOT_TEMP_FILE );
			break;
		case CONFIG_CHANGE:
			cmd_touch( REBOOT_BECAUSE_OF_CONFIG_CHANGED_TEMP_FILE );
			break;
		case RESTORE_SETTING:
			cmd_touch( NEED_TO_RESTORE_SETTINGS_TEMP_FILE );
			break;
		default:
			cmd_touch( NEED_TO_REBOOT_TEMP_FILE );
			break;
	}
}
extern int is_anhui_version();
//extern int update_partial(char *url);
int update_exec(download_arg_t *dlarg)
{
    cwmp_log_debug("TR069:Updating begin");
    char shellcmd[256];
    char filePath[256];
    char* fileName = GetBasename(dlarg->url);
    sprintf(filePath, "/tmp/%s", fileName);
    cwmp_log_debug("filename:%s", filePath);

    if (!cmd_file_exist(filePath))
    {
        return CWMP_ERROR;
    }
    if(strcmp(dlarg->filetype, "1 Firmware Upgrade Image") == 0) 
    {
/*       sprintf(shellcmd,"sysupgrade -T %s",filePath);
       if(system(shellcmd) != 0)
       {
            return CWMP_ERROR;
       }
*/       
       memset(shellcmd,0,sizeof(shellcmd));
       sprintf(shellcmd,"upgrade %s",filePath);
       system(shellcmd);
    }
    else if(strcmp(dlarg->filetype, "3 Vendor Configuration File") == 0)
    {
       sprintf(shellcmd,"/etc/tozed/config_update %s 0",filePath);
       if(system(shellcmd) != 0)
       {    
            if(cmd_file_exist("/tmp/.update_config_fail"))
            return CWMP_ERROR;
       }
       cmd_touch(NEED_TO_REBOOT_TEMP_FILE);
    }
    cwmp_log_debug("TR069:Update end");
    return CWMP_OK;
}

void reboot()
{
	cmd_touch(TR069_REBOOT_FLAG);
	if (is_anhui_version()) {
		system("reboot");
	} else {
		process_reboot(NORMAL_REBOOT);
	}
}

int cwmp_agent_run_tasks(cwmp_t * cwmp)
{
	void * data;
	int tasktype = 0;;
	int ok = CWMP_NO;

	FUNCTION_TRACE();
	
	while(1)
	{
		tasktype = queue_pop(cwmp->queue, &data);
		if(tasktype == -1)
		{
			cwmp_log_debug("no more task to run");
			break;
		}
		ok = CWMP_YES;
		switch(tasktype)
		{
			case TASK_DOWNLOAD_TAG:
				{
					download_arg_t * dlarg = (download_arg_t*)data;
					time_t starttime = time(NULL);
					int faultcode = CWMP_OK;
					if (update_exec(dlarg) != 0) {
						faultcode = 9001;
					}
					
					time_t endtime = time(NULL);
					cwmp_event_set_value(cwmp, INFORM_TRANSFERCOMPLETE, 1,dlarg->cmdkey, faultcode, starttime, endtime);
					cwmp_event_clear_active(cwmp);
					
					FREE(dlarg);

					if (faultcode == CWMP_OK)
					{
				            cmd_touch(TR069_UPDATE_REBOOT_FLAG);
				            //process_reboot(CONFIG_CHANGE);
                                            if(access(NEED_TO_REBOOT_TEMP_FILE, F_OK) >= 0)
                                            {
                                                system("reboot -d 2 &");
                                            }
                                            exit(1);
					}
                                        /*
                                        download_arg_t * dlarg = (download_arg_t*)data;
					time_t starttime = time(NULL);
                                        int faultcode = 0;
				        //download process	
					time_t endtime = time(NULL);
					cwmp_event_set_value(cwmp, INFORM_TRANSFERCOMPLETE, 1,dlarg->cmdkey, faultcode, starttime, endtime);
					cwmp_event_clear_active(cwmp);
					
					FREE(dlarg);
                                        */
				}
				break;

			case TASK_UPLOAD_TAG:
				{
					write_sys_log("TR069: uploading ...");
					
					upload_arg_t * ularg = (upload_arg_t*)data;
					time_t starttime = time(NULL);
					int faultcode = 0;
		
					write_sys_log("TR069: begin upload file ...");
					faultcode = cwmp_agent_upload_file(ularg);
					write_sys_log("TR069: end upload file ...");
					
					time_t endtime = time(NULL);
					cwmp_event_set_value(cwmp, INFORM_TRANSFERCOMPLETE, 1,ularg->cmdkey, faultcode, starttime, endtime);
					
					FREE(ularg);
					
				}
				break;

			case TASK_REBOOT_TAG:
				{
					//begin reboot system
					write_sys_log("TR069: reboot ...");
					cwmp_event_set_value(cwmp, INFORM_MREBOOT, 1, (char *)data, 0, 0, 0);
					cwmp_event_clear_active(cwmp);
					//process_reboot(NORMAL_REBOOT);
                                        system("reboot -d 3 &");
					exit(1);
				}
				break;

			case TASK_FACTORYRESET_TAG:
				{
					//begin factory reset system
					
					//cwmp_event_clear_active(cwmp);
					//system("factoryreset");
					
					restore_factory();
//					system("jffs2reset -y");
					remove(TR069_BOOTSTRAP_FLAG);
					remove(TR069_REBOOT_FLAG);
					cwmp_event_set_value(cwmp, INFORM_BOOTSTRAP, 1, NULL, 0, 0, 0);
					cwmp_event_clear_active(cwmp);
                                        system("reboot -d 3 &");
					//process_reboot(RESTORE_SETTING);
					exit(1);
				}
				break;

            case TASK_CALLBACK_TAG:
            {
                callback_func_t callback = (callback_func_t)data;
                if(callback)
                {
                    cwmp_log_debug("run callback");
                    callback(NULL, NULL);
                }
            }
            break;
            default:
            break;

		}
	}

	return ok;
}





int cpe_get_RadioNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "1");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_SSIDNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param, "%d", igd_entries.ssid_entry);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_AccessPointNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "1");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_EndPointNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "1");
    *value = param;

    return FAULT_CODE_OK;
}

int  cpe_refresh_RadioOfDevice(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, 1);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int cpe_get_StatusOfRadio(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    get_single_config_attr("TZ_ENABLE_WIFI", param);
    if(!strcmp(param, "yes"))
        strcpy(param, "Up");
    else
        strcpy(param, "Down");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_SupportedFrequencyBands(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "2.4GHz");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_SupportedStandardsOfRadio(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "b ([802.11b-1999]),g ([802.11g-2003]),n ([802.11n-2009])");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_AutoChannelSupportedOfRadio(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "true");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_AutoChannelRefreshPeriod(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "0");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_OperatingChannelBandwidth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "40MHz (wide mode)");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_CurrentOperatingChannelBandwidth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "40MHz (wide mode)");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_AboveControlChannel(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "Auto");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_GuardInterval(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "Auto");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_MCSOfRadio(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "-1");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_IEEE80211hSupported(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "false");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_IEEE80211hEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "false");
    *value = param;

    return FAULT_CODE_OK;
}

int  cpe_refresh_SSIDOfDevice(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, igd_entries.ssid_entry);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int  cpe_refresh_AccessPointOfDevice(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, igd_entries.ssid_entry);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}
#ifndef DIAG_NONE
#define DIAG_NONE "None"
#endif
#ifndef DIAG_REQUESTED
#define DIAG_REQUESTED "Requested"
#endif
#ifndef DIAG_COMPLETE
#define DIAG_COMPLETE "Complete"
#endif
#define Error_TransferFailed "Error_TransferFailed"
#define MAX_UPLOAD_FILE_SIZE        (10 * 1024 * 1024)


static int download_init_flag = 0;
static int upload_init_flag = 0;
static int download_diag_commpleted = 0;
static int upload_diag_commpleted = 0;
extern diag_download_t diag_down;
extern diag_upload_t diag_upload;

void set_download_state(char *state) {
	if (strcmp(state, DIAG_NONE) == 0) {
		download_init_flag = 1;
	} else if (strcmp(state, DIAG_REQUESTED) == 0) {
	} else {
		download_init_flag = 0;
	}
	strcpy(diag_down.DiagnosticsState, state);
}

void set_upload_state(char *state) {
    if (strcmp(state, DIAG_NONE) == 0) {
        upload_init_flag = 1;
    } else if (strcmp(state, DIAG_REQUESTED) == 0) {
    } else {
        upload_init_flag = 0;
    }
    strcpy(diag_down.DiagnosticsState, state);
}

void init_download() {
	if (download_init_flag == 0) {
		download_init_flag = 1;
		memset(&diag_down, 0, sizeof(diag_down));
		
		set_download_state(DIAG_NONE);
		diag_down.DSCP = 0;
		diag_down.EthernetPriority = 0;
	}
}

void init_upload() {
    if (upload_init_flag == 0) {
        upload_init_flag = 1;
        memset(&diag_upload, 0, sizeof(diag_upload));
        set_download_state(DIAG_NONE);
        diag_upload.DSCP = 0;
        diag_upload.EthernetPriority = 0;
        diag_upload.TestFileLength = 0;
    }
}
#define DOWNLOAD_DIAG_FILE "/tmp/download_diag_test_file"
#define DOWNLOAD_WGET_LOG "/tmp/tr069_wget.log"

int check_ftp_receive_state(const char *fromurl, const char * tofile, const char * pUsername, const char * pPassword)
{
    char shellcmd[256] = {0};
	char buffer[512] = {0};
	int size_of_file = 0;
    
	if (pUsername == NULL||strlen(pUsername) == 0) {
		sprintf(shellcmd,"wget --timeout=5 --tries=1 -o %s --spider %s ", DOWNLOAD_WGET_LOG, fromurl);
		//--spider Test download the file, this time has not yet downloaded
		// --timeout=2 Set the timeout time 5s  --tries=1 Set the number of retries If specified as 0, it means an unlimited number of retries
	} else { //This branch is not used
		sprintf(shellcmd, "wget --timeout=5 --tries=1 -o %s --spider ftp://%s:%s@%s",
						DOWNLOAD_WGET_LOG,
						pUsername,
						pPassword,
						fromurl+6);
	}
    system(shellcmd);
    memset(shellcmd,0,sizeof(shellcmd));
    sprintf(shellcmd,"cat  %s | grep \"==> SIZE\" | awk \'{print $5}\'", DOWNLOAD_WGET_LOG);
    read_memory(shellcmd,buffer,sizeof(buffer));
    if(strlen(buffer) != 0) {
		size_of_file = atoi(buffer);
		memset(shellcmd,0,sizeof(shellcmd));
        memset(buffer,0,sizeof(buffer));
        sprintf(shellcmd,"cat %s | grep \"No such file\"", DOWNLOAD_WGET_LOG);//NO such file
        read_memory(shellcmd,buffer,sizeof(buffer));
		 if(strlen(buffer) != 0) {
            set_download_state("Error_TransferFailed");//no such file
			return -1;
         } else {
         	diag_down.TotalBytesReceived = size_of_file;//Always return the file size
         	if (size_of_file > MAX_UPLOAD_FILE_SIZE) {//MAX_UPLOAD_FILE_SIZE == MAX_DOWNLOADFILE_SIZE
				cwmp_log_debug("[ftp_receive_file] file_Size > 10M");
				set_download_state("Error_IncorrectSize");
				return -1;
			}
         }
    } else {
        memset(shellcmd,0,sizeof(shellcmd));
        memset(buffer,0,sizeof(buffer));
        sprintf(shellcmd,"cat %s | grep \"failed: Connection timed out\"", DOWNLOAD_WGET_LOG);//timed out
        read_memory(shellcmd,buffer,sizeof(buffer));
        if(strlen(buffer) != 0) {
            set_download_state("Error_Timeout");
			return -1;
         } else {
         	memset(shellcmd,0,sizeof(shellcmd));
            memset(buffer,0,sizeof(buffer));
            sprintf(shellcmd,"cat %s | grep \"Error in server response\"", DOWNLOAD_WGET_LOG);//no response
            read_memory(shellcmd,buffer,sizeof(buffer));
            if(strlen(buffer) != 0) {
                set_download_state("Error_NoResponse");
				return -1;
            } else {
				memset(shellcmd,0,sizeof(shellcmd));
	            memset(buffer,0,sizeof(buffer));
	            sprintf(shellcmd,"cat %s | grep \"Login incorrect\"", DOWNLOAD_WGET_LOG);//Login faild
	            read_memory(shellcmd,buffer,sizeof(buffer));
	            if(strlen(buffer) != 0) {
	                set_download_state("Error_LoginFailed");
					return -1;
	            } else {
					set_download_state("Error_InitConnectionFailed");
					return -1;
	            }
            }
         }
    }
	return CWMP_OK;
}
int ftp_receive_file(const char *fromurl, const char * tofile, const char * pUsername, const char * pPassword)
{
	char buff[512] = {0};
	int ret = 0;
	ret = check_ftp_receive_state(fromurl,tofile,pUsername,pPassword);
	if (remove(DOWNLOAD_WGET_LOG) != 0) {
		cwmp_log_error("[ftp_receive_file]:%s remove failed", DOWNLOAD_WGET_LOG);
	}
	if (ret != 0) {
		return -1;
	}
	gettimeofday(&diag_down.TCPOpenRequestTime,NULL);
	if (pUsername == NULL||strlen(pUsername) == 0) {
		sprintf(buff, "wget --timeout=5 --tries=1 -o %s %s -O %s", DOWNLOAD_WGET_LOG, fromurl, tofile);//-o Output log to file
		// --timeout=2 Set the timeout time 5s  --tries=1 Set the number of retries If specified as 0, it means an unlimited number of retries
	} else {
		sprintf(buff, "wget --timeout=5 --tries=1 -o %s ftp://%s:%s@%s -O %s",
						DOWNLOAD_WGET_LOG,
						pUsername,
						pPassword,
						fromurl+6,
						tofile);
	}
	cwmp_log_debug("[ftp_receive_file] download.sh:\n%s", buff);
	gettimeofday(&diag_down.TCPOpenResponseTime,NULL);
	gettimeofday(&diag_down.ROMTime,NULL);
	gettimeofday(&diag_down.BOMTime,NULL);
	system(buff);
	gettimeofday(&diag_down.EOMTime,NULL);
	/*read_memory("cat /tmp/tr069_wget.log | grep Length: | awk \'{print $2}\'",buff,sizeof(buff));
	if (strlen(buff) != 0) {
		diag_down.TotalBytesReceived = atoi(buff);
	}*/
	if (remove(DOWNLOAD_WGET_LOG) != 0) {
		cwmp_log_error("[ftp_receive_file]:%s remove failed", DOWNLOAD_WGET_LOG);
	}
	return CWMP_OK;

}

void *process_downloaddiagnostics(void)
{
	int faultcode = CWMP_OK;
	char tofile[64];
	strcpy(tofile, DOWNLOAD_DIAG_FILE);
	pthread_detach(pthread_self());
	sleep(5);
	if (diag_down.URL.DownloadURL && TRstrncasecmp("ftp://", diag_down.URL.DownloadURL, 6) == 0) {
		cwmp_log_debug("TR069 DIAGNOSTICS USE FTP.");
		faultcode = ftp_receive_file(diag_down.URL.DownloadURL,tofile,NULL,NULL);
		if (faultcode == CWMP_OK) {
			set_download_state(DIAG_COMPLETE);
		} else {
			upload_init_flag = 0;
		}

	} else if (diag_down.URL.DownloadURL && TRstrncasecmp("http://", diag_down.URL.DownloadURL, 7) == 0) {
		cwmp_log_debug("TR069 DIAGNOSTICS USE FTP.");
		faultcode = http_download_diag_file(diag_down.URL.DownloadURL, tofile);
	    if (faultcode == 200) {// HTTP/1.1 200
		    set_download_state(DIAG_COMPLETE);
	    } else {
	    	upload_init_flag = 0;
	    }
		
	} else {
		cwmp_log_debug("TR069 DIAGNOSTICS Unknown way");
		set_download_state("Error_NoTransferMode");
		upload_init_flag = 0;

	}
	cwmp_log_debug("TR069 DIAGNOSTICSCOMPLETE.");
	cwmp_event_set_value(get_root_cwmp(), INFORM_DIAGNOSTICSCOMPLETE, 1, NULL, 0, 0, 0);
	cwmp_log_debug("TR069 DownloadDiagnostics END");
	if (remove(tofile) != 0) {
		cwmp_log_error("DownloadDiag tofile remove failed");
	}
	download_diag_commpleted = 0;
	pthread_exit("download_diag_exit");


}

void *upload_diag_proc(void)
{
    pthread_detach(pthread_self());
	sleep(5);
	cwmp_log_debug("####TR069 UploadDiagnostics thread>> start");
    http_upload_file_test(diag_upload.URL.UploadURL, diag_upload.TestFileLength);
    clear_and_cwmp_event_set_value(get_root_cwmp(), INFORM_DIAGNOSTICSCOMPLETE, 1, NULL, 0, 0, 0);
    cwmp_log_debug("TR069 UploadDiagnostics END");
    upload_diag_commpleted = 0;
    pthread_exit("upload_exit");
}

int cpe_get_igd_Download_DiagnosticsState(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param,diag_down.DiagnosticsState);
	if (strlen(param) == 0) {
		strcpy(param,DIAG_NONE);
	}
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_set_igd_Download_DiagnosticsState(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	init_download();
	if (strcmp(value,"Requested") == 0) {
		strcpy(diag_down.DiagnosticsState, value);	
		if (download_diag_commpleted == 0) {
			download_diag_commpleted = 1;
			cwmp_log_debug("TR069 DownloadDiagnostics");
			pthread_t download;
	    	int status = pthread_create(&download, NULL, (void*)process_downloaddiagnostics, NULL);
			if(status != 0) {
	  			cwmp_log_error("download pthread_create error");
				download_diag_commpleted = 0;
			}
		}
	} else {
		return FAULT_CODE_9007;//Invalid Parameter value (associated with SetParameterValues)  
	}
    return FAULT_CODE_OK;
}

int cpe_get_igd_Download_Interface(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param,diag_down.Interface);
	if (strlen(param) == 0) {
		strcpy(param,DIAG_NONE);
	}
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_set_igd_Download_Interface(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	init_download();
	if (value != NULL) {
		strcpy(diag_down.Interface, value);
	}
    return FAULT_CODE_OK;
}

int cpe_get_igd_Download_URL(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param,diag_down.URL.DownloadURL);
	if (strlen(param) == 0) {
		strcpy(param,N_A);
	}
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_set_igd_Download_URL(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	init_download();
	if (value != NULL) {
		strcpy(diag_down.URL.DownloadURL, value);
	}
    return FAULT_CODE_OK;
}

int cpe_get_igd_Download_DSCP(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param,"%d", diag_down.DSCP);
	if (strlen(param) == 0) {
		strcpy(param,N_A);
	}
	*value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_Download_DSCP(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	init_download();
	//diag_down.DSCP = atoi(value);
    return FAULT_CODE_OK;
}


int cpe_get_igd_Download_Priority(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param,"%d", diag_down.EthernetPriority);
	if (strlen(param) == 0) {
		strcpy(param,N_A);
	}
	*value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_Download_Priority(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	init_download();
	//diag_down.EthernetPriority = atoi(value);
    return FAULT_CODE_OK;
}

int cpe_get_igd_Download_ROMTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char ms[16] = {0};
	if (diag_down.ROMTime.tv_sec != 0) {
	   	struct tm *local_time = localtime(&diag_down.ROMTime.tv_sec);
	    strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
		sprintf(ms,".%ld",diag_down.ROMTime.tv_usec);
	    strcat(param,ms);
	} else {
		strcpy(param,N_A);
	}
	*value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_Download_BOMTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
   	char ms[16] = {0};
	if (diag_down.BOMTime.tv_sec != 0) {
	   	struct tm *local_time = localtime(&diag_down.BOMTime.tv_sec);
	    strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
		sprintf(ms,".%ld",diag_down.BOMTime.tv_usec);
	    strcat(param,ms);
	} else {
		strcpy(param,N_A);
	}
	*value = param;
    return FAULT_CODE_OK;
}


int cpe_get_igd_Download_EOMTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char ms[16] = {0};
	if (diag_down.EOMTime.tv_sec != 0) {
	   	struct tm *local_time = localtime(&diag_down.EOMTime.tv_sec);
	    strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
		sprintf(ms,".%ld", diag_down.EOMTime.tv_usec);
	    strcat(param,ms);
	} else {
		strcpy(param,N_A);
	}
	*value = param;
    return FAULT_CODE_OK;
}


int cpe_get_igd_Download_TestBytesReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param,"%d", diag_down.TestBytesReceived);
	if (strlen(param) == 0) {
		strcpy(param,N_A);
	}
	*value = param;
    return FAULT_CODE_OK;
}


int cpe_get_igd_Download_TotalBytesReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param,"%d", diag_down.TotalBytesReceived);
	if (strlen(param) == 0) {
		strcpy(param,N_A);
	}
	*value = param;

    return FAULT_CODE_OK;
}


int cpe_get_igd_Download_TCPOpenRequestTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char ms[16] = {0};
	if (diag_down.TCPOpenRequestTime.tv_sec != 0) {
	   	struct tm *local_time = localtime(&diag_down.TCPOpenRequestTime.tv_sec);
	    strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
		sprintf(ms,".%ld", diag_down.TCPOpenRequestTime.tv_usec);
	    strcat(param,ms);
	} else {
		strcpy(param,N_A);
	}
	*value = param;

    return FAULT_CODE_OK;
}


int cpe_get_igd_Download_TCPOpenResponseTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char ms[16] = {0};
	if (diag_down.TCPOpenResponseTime.tv_sec != 0) {
	   	struct tm *local_time = localtime(&diag_down.TCPOpenResponseTime.tv_sec);
	    strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
		sprintf(ms,".%ld",diag_down.TCPOpenResponseTime.tv_usec);
	    strcat(param,ms);
	} else {
        strcpy(param,N_A);
	}
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_Upload_DiagnosticsState(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, diag_upload.DiagnosticsState);
    if (strlen(param) == 0) {
        strcpy(param,N_A);
    }
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_igd_Upload_DiagnosticsState(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    init_upload();
    if (strcmp(value,"Requested") == 0) {
        strcpy(diag_upload.DiagnosticsState, value);  
        if (upload_diag_commpleted == 0) {
            upload_diag_commpleted = 1;
            cwmp_log_debug("####TR069 UploadDiagnostics start");
            pthread_t upload;
            int status = pthread_create(&upload, NULL, (void *)upload_diag_proc, NULL);
            if(status != 0) {
                cwmp_log_error("Upload pthread_create error");
                upload_diag_commpleted = 0;
            }
        }
    } else {
        return FAULT_CODE_9007;//Invalid Parameter value (associated with SetParameterValues)  
    }
    return FAULT_CODE_OK;
}

int cpe_get_igd_Upload_Interface(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, diag_upload.Interface);
    if (strlen(param) == 0) {
        strcpy(param,N_A);
    }
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_igd_Upload_Interface(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    init_upload();
    strcpy(diag_upload.Interface, value);
    return FAULT_CODE_OK;
}

int cpe_get_igd_Upload_URL(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, diag_upload.URL.UploadURL);
    if (strlen(param) == 0) {
        strcpy(param,N_A);
    }
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_igd_Upload_URL(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    init_upload();
    if (value != NULL) {
        strcpy(diag_upload.URL.UploadURL, value);
    }
    return FAULT_CODE_OK;
}

int cpe_get_igd_Upload_TestFileLength(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param, "%d", diag_upload.TestFileLength);
    if (strlen(param) == 0) {
        strcpy(param,N_A);
    }
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_igd_Upload_TestFileLength(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    init_upload();
    if (value == NULL) 
        return FAULT_CODE_OK;
    if(strlen(value) == 0)
        diag_upload.TestFileLength = 0;
    else
        diag_upload.TestFileLength = atoi(value);

    if(diag_upload.TestFileLength > MAX_UPLOAD_FILE_SIZE)
        diag_upload.TestFileLength = MAX_UPLOAD_FILE_SIZE;

    return FAULT_CODE_OK;
}

int cpe_get_igd_Upload_DSCP(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param,"%d", diag_upload.DSCP);
    if (strlen(param) == 0) {
        strcpy(param,N_A);
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_Upload_DSCP(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    init_upload();
    // diag_upload.DSCP = atoi(value);
    return FAULT_CODE_OK;
}


int cpe_get_igd_Upload_EthernetPriority(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param,"%d", diag_upload.EthernetPriority);
    if (strlen(param) == 0) {
        strcpy(param,N_A);
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_Upload_EthernetPriority(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    init_upload();
    // diag_upload.EthernetPriority = atoi(value);
    return FAULT_CODE_OK;
}

int cpe_get_igd_Upload_ROMTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char ms[16] = {0};
    if (diag_upload.ROMTime.tv_sec != 0) {
        struct tm *local_time = localtime(&diag_upload.ROMTime.tv_sec);
        strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
        sprintf(ms,".%ld", diag_upload.ROMTime.tv_usec);
        strcat(param,ms);
    } else {
        strcpy(param,N_A);
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_Upload_BOMTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char ms[16] = {0};
    if (diag_upload.BOMTime.tv_sec != 0) {
        struct tm *local_time = localtime(&diag_upload.BOMTime.tv_sec);
        strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
        sprintf(ms,".%ld",diag_upload.BOMTime.tv_usec);
        strcat(param,ms);
    } else {
        strcpy(param,N_A);
    }
    *value = param;
    return FAULT_CODE_OK;
}


int cpe_get_igd_Upload_EOMTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char ms[16] = {0};
    if (diag_upload.EOMTime.tv_sec != 0) {
        struct tm *local_time = localtime(&diag_upload.EOMTime.tv_sec);
        strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
        sprintf(ms,".%ld", diag_upload.EOMTime.tv_usec);
        strcat(param,ms);
    } else {
        strcpy(param,N_A);
    }
    *value = param;
    return FAULT_CODE_OK;
}


int cpe_get_igd_Upload_TotalBytesSent(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param,"%d", diag_upload.TotalBytesSent);
    if (strlen(param) == 0) {
        strcpy(param,N_A);
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_Upload_TCPOpenRequestTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char ms[16] = {0};
    if (diag_upload.TCPOpenRequestTime.tv_sec != 0) {
        struct tm *local_time = localtime(&diag_upload.TCPOpenRequestTime.tv_sec);
        strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
        sprintf(ms,".%ld", diag_upload.TCPOpenRequestTime.tv_usec);
        strcat(param,ms);
    } else {
        strcpy(param,N_A);
    }
    *value = param;

    return FAULT_CODE_OK;
}


int cpe_get_igd_Upload_TCPOpenResponseTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char ms[16] = {0};
    if (diag_upload.TCPOpenResponseTime.tv_sec != 0) {
        struct tm *local_time = localtime(&diag_upload.TCPOpenResponseTime.tv_sec);
        strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
        sprintf(ms,".%ld", diag_upload.TCPOpenResponseTime.tv_usec);
        strcat(param,ms);
    } else {
        strcpy(param,N_A);
    }
    *value = param;

    return FAULT_CODE_OK;
}

udp_echo_st udp_echo;
#define MAXLINE 80
#define LAN_UDP_INTERFACE 		"InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.IPInterface.1"
#define WAN_UDP_INTERFACE 		"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1"
void do_echo(int sockfd, struct sockaddr_in *client_addr, socklen_t clilen)
{
    int reret = -1, senret = -1;
    socklen_t len;
	char *client_ipaddr = NULL;
    char mesg[MAXLINE];
	
	len = clilen;
	memset(client_addr, 0, clilen);
    client_addr->sin_family = AF_INET;
	cwmp_log_debug("##### UDP Echo thread ##### Enter for(;;)");
    for(;;) {

		if (udp_echo.Enable == 0) {
			cwmp_log_debug("##### UDP Echo thread ##### Server is closed");
			close(sockfd);
			pthread_exit("upload_exit");
		}
        
        reret = recvfrom(sockfd, mesg, sizeof(mesg), MSG_DONTWAIT, (struct sockaddr *)client_addr, &len);
		if (reret == -1) {
			continue;
		}
		client_ipaddr = inet_ntoa(client_addr->sin_addr);
		if (client_ipaddr == NULL) {
			cwmp_log_error("##### UDP Echo thread ##### get client ip faild!");
			continue;
		}
		if (0 == strncmp(udp_echo.SourceIPAddress, client_ipaddr, strlen(udp_echo.SourceIPAddress))) {
			cwmp_log_debug("##### UDP Echo thread ##### addr  is %s", client_ipaddr);
			if (udp_echo.PacketsReceived == 0) {
				gettimeofday(&udp_echo.TimeFirstPacketReceived, NULL);
				memcpy(&udp_echo.TimeLastPacketReceived, &udp_echo.TimeFirstPacketReceived, sizeof(udp_echo.TimeFirstPacketReceived));
			} else {
				gettimeofday(&udp_echo.TimeLastPacketReceived, NULL);
			}
			udp_echo.PacketsReceived++;
			
			udp_echo.BytesReceived += reret;
			//cwmp_log_debug("##### UDP Echo thread ##### send msg  [%s]", mesg);
			senret = sendto(sockfd, mesg, reret, 0, (struct sockaddr *)client_addr, len);
			if (senret != -1) {
				udp_echo.BytesResponded += senret;
				udp_echo.PacketsResponded++;
			} else {
				cwmp_log_debug("##### UDP Echo thread ##### send msg fiald");
				continue;
			}
		}
        
    }
}

void *UDP_Echo_Server_thread(void)
{
	cwmp_log_error("##### UDP Echo thread #####enter UDP_Echo_Server_thread");
    pthread_detach(pthread_self());
	int sock_fd;
    struct sockaddr_in servaddr, cliaddr;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        cwmp_log_error("##### UDP Echo thread #####create socket error!");
		pthread_exit("upload_exit");

    }

    memset(&servaddr, 0,sizeof(servaddr));
	if (strlen(udp_echo.Ifr_name) == 0) {
		strcpy(udp_echo.Ifr_name, "all");
	}
	
	if (strcmp(udp_echo.Ifr_name, "all") != 0) {
		cwmp_log_debug("##### UDP Echo thread ##### Ifr name %s!", udp_echo.Ifr_name);
		struct ifreq if_servaddr;
		strncpy(if_servaddr.ifr_ifrn.ifrn_name, udp_echo.Ifr_name, \
	  	strlen(udp_echo.Ifr_name) + 1);
		
		if (setsockopt(sock_fd, SOL_SOCKET, SO_BINDTODEVICE, \
	  	(char *)&if_servaddr, sizeof(if_servaddr)),0) {
	      /* Deal with error... */
		  cwmp_log_error("##### UDP Echo thread ##### SO_BINDTODEVICE failed");
	      pthread_exit("upload_exit");
		}
	}

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (udp_echo.Port == 0) {
		udp_echo.Port = 7;
	}
    servaddr.sin_port = htons(udp_echo.Port);
	
    if(bind(sock_fd, &servaddr, sizeof(servaddr)) == -1) {
		close(sock_fd);
        cwmp_log_error("##### UDP Echo thread ##### bind error");
        pthread_exit("upload_exit");
    }
	cwmp_log_error("##### UDP Echo thread #####enter do_echo!");
    do_echo(sock_fd, &cliaddr, sizeof(cliaddr));
	
	pthread_exit("upload_exit");
}

int cpe_get_igd_UdpEcho_Enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	if (udp_echo.Enable == 1) {
		strcpy(param, "true");
	} else {
		strcpy(param, "false");
	}
	
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_igd_UdpEcho_Enable(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    if (value == NULL) {
		return FAULT_CODE_OK;
    }

	if (strlen(udp_echo.SourceIPAddress) == 0) {
		
		return FAULT_CODE_9007;
	}
	int tmpenable = udp_echo.Enable;
    if (strcmp(value, "true") == 0) {
		udp_echo.Enable = 1;
		if (tmpenable == 0) {
			udp_echo.PacketsReceived = 0;
			udp_echo.PacketsResponded = 0;
			udp_echo.BytesReceived = 0;
			udp_echo.BytesResponded = 0;
			memset(&udp_echo.TimeFirstPacketReceived, 0, sizeof(udp_echo.TimeFirstPacketReceived));
			memset(&udp_echo.TimeLastPacketReceived, 0, sizeof(udp_echo.TimeLastPacketReceived)); 
            cwmp_log_debug("####TR069 UDP Echo Service start####");
            pthread_t udpecho_id;
            int status = pthread_create(&udpecho_id, NULL, (void *)UDP_Echo_Server_thread, NULL);
            if(status != 0) {
                cwmp_log_error("UDP Echo Service pthread_create error");
            }
		}
    } else {
    	udp_echo.Enable = 0;
    }

    return FAULT_CODE_OK;
}

int cpe_get_igd_UdpEcho_Interface(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    if (strlen(udp_echo.Interface) == 0) {
        *value = NULL;
    } else {
    	strcpy(param, udp_echo.Interface);
    	*value = param;
    }
    

    return FAULT_CODE_OK;
}

int cpe_set_igd_UdpEcho_Interface(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    //init_upload();
	if (value == NULL || strlen(value) == 0) {
		memset(udp_echo.Interface, 0, sizeof(udp_echo.Interface));
		strncpy(udp_echo.Ifr_name, "all", 10);
		return FAULT_CODE_OK;
	}
	
    strcpy(udp_echo.Interface, value);
	
	if (0 == strcmp(udp_echo.Interface, LAN_UDP_INTERFACE)) {
		strncpy(udp_echo.Ifr_name, "br-lan", 10);
	} else if (0 == strcmp(udp_echo.Interface, WAN_UDP_INTERFACE)){
		strncpy(udp_echo.Ifr_name, "usb0", 10);
	} else {
		strncpy(udp_echo.Ifr_name, "all", 10);
	}
	
    return FAULT_CODE_OK;
}

int cpe_get_igd_UdpEcho_SourceIPAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    if (strlen(udp_echo.SourceIPAddress) == 0) {
        *value = NULL;
    } else {
    	strcpy(param, udp_echo.SourceIPAddress);
    	*value = param;
    }
	return FAULT_CODE_OK;
}

int cpe_set_igd_UdpEcho_SourceIPAddress(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    if (value == NULL || strlen(value) == 0) {
		memset(udp_echo.SourceIPAddress, 0, sizeof(udp_echo.SourceIPAddress));
		return FAULT_CODE_OK;
	}
    strcpy(udp_echo.SourceIPAddress, value);
    return FAULT_CODE_OK;
}

int cpe_get_igd_UdpEcho_UDPPort(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	if (udp_echo.Port == 0) {
		udp_echo.Port = 7;
	}
    sprintf(param, "%d", udp_echo.Port);
    if (strlen(param) == 0) {
        return FAULT_CODE_OK;
    }
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_igd_UdpEcho_UDPPort(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	if (value == NULL || strlen(value) == 0) {
		udp_echo.Port = 0;
		return FAULT_CODE_OK;
	}
    udp_echo.Port = atoi(value);
	if (udp_echo.Port <= 0) {
		udp_echo.Port = 7;
	}

    return FAULT_CODE_OK;
}

int cpe_get_igd_UdpEcho_EchoPlusEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	if (udp_echo.EchoPlusEnabled == 1) {
		strcpy(param, "true");
	} else {
		strcpy(param, "false");
	}
	
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_igd_UdpEcho_EchoPlusEnabled(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
     if (value == NULL) {
		return FAULT_CODE_OK;
    }
    if (strcmp(value, "true") == 0) {
		udp_echo.EchoPlusEnabled = 1;
    } else {
    	udp_echo.EchoPlusEnabled = 0;
    }
    return FAULT_CODE_OK;
}


int cpe_get_igd_UdpEcho_EchoPlusSupported(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "false");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_UdpEcho_PacketsReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	sprintf(param, "%d", udp_echo.PacketsReceived);
    *value = param;
    return FAULT_CODE_OK;
}
int cpe_get_igd_UdpEcho_PacketsResponded(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param, "%d", udp_echo.PacketsResponded);
    *value = param;
    return FAULT_CODE_OK;
}


int cpe_get_igd_UdpEcho_BytesReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param, "%d", udp_echo.BytesReceived);
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_UdpEcho_BytesResponded(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param, "%d", udp_echo.BytesResponded);
    *value = param;
    return FAULT_CODE_OK;
}


int cpe_get_igd_UdpEcho_TimeFirstPacketReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char ms[16] = {0};
    if (udp_echo.TimeFirstPacketReceived.tv_sec != 0) {
        struct tm *local_time = localtime(&udp_echo.TimeFirstPacketReceived.tv_sec);
        strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
        sprintf(ms,".%ld", udp_echo.TimeFirstPacketReceived.tv_usec);
        strcat(param,ms);
    } else {
        strcpy(param, "0");
    }
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_UdpEcho_TimeLastPacketReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char ms[16] = {0};
    if (udp_echo.TimeLastPacketReceived.tv_sec != 0) {
        struct tm *local_time = localtime(&udp_echo.TimeLastPacketReceived.tv_sec);
        strftime(param, sizeof(param), "%Y-%m-%dT%H:%M:%S", local_time);
        sprintf(ms,".%ld", udp_echo.TimeLastPacketReceived.tv_usec);
        strcat(param,ms);
    } else {
        strcpy(param, "0");
    }
    *value = param;

    return FAULT_CODE_OK;
}


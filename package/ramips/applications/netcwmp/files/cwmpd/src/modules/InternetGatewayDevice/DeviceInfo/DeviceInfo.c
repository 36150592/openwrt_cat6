#define VENDOR_LOG_FILE 		"/usr/websys.log"
#define VENDOR_LOG_MAX_SIZE 	"102400"

//InternetGatewayDevice.DeviceInfo.Manufacturer
int cpe_get_igd_di_manufacturer(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    //return get_sys_parameter(X_SYS_Manufacturer, value);
    *value = cwmp->cpe_mf;
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ManufacturerOUI
int cpe_get_igd_di_manufactureroui(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    //return get_sys_parameter(X_SYS_ManufacturerOUI, value);
    *value = cwmp->cpe_oui;
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ModelName
int cpe_get_igd_di_ModelName(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{	
	char buff[128];
	sprintf(buff, "cat /version | grep device= | awk -F= \'{print $2}\'");
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ModelNumber
int cpe_get_igd_di_ModelNumber(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	sprintf(buff, "cat /version | grep hwversion= | awk -F= \'{print $2}\'");
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}

    return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.Description
int cpe_get_igd_di_Description(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	sprintf(buff, "cat /version | grep device= | awk -F= \'{print $2}\'");
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	strcat(param, " WirelessRouter");
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ProductClass
int cpe_get_igd_di_productclass(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    //return get_sys_parameter(X_SYS_ProductClass, value);
    *value = cwmp->cpe_pc;
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.SerialNumber
int cpe_get_igd_di_serialnumber(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    //return get_sys_parameter(X_SYS_SerialNumber, value);
    *value = cwmp->cpe_sn;
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.SpecVersion
int cpe_get_igd_di_specversion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return get_sys_parameter(X_SYS_SpecVersion, value);
}

//InternetGatewayDevice.DeviceInfo.HardwareVersion
int cpe_get_igd_di_hardwareversion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    //return get_sys_parameter(X_SYS_HardwareVersion, value);
    strcpy(param,"v1.1.0");
    *value = param;
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.SoftwareVersion
int cpe_get_igd_di_softwareversion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    //return get_sys_parameter(X_SYS_SoftwareVersion, value);
    char shcmd[128];
	read_memory("uci -q get tozed.system.TZ_SYSTEM_SOFTWARE_VERSION", param, 64); 
    util_strip_traling_spaces(param);
	if (strlen(param) == 0) {
	       sprintf(shcmd, "cat /version |grep software_version|awk -F= \'{print $2}\'");
			read_memory(shcmd, param, sizeof(param));
			util_strip_traling_spaces(param);
			if (strlen(param) == 0) {
				*value = NULL;
			} else {
				*value = param;
			}
	}else{
		*value = param;
	}
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ConfigVersion
int cpe_get_igd_di_configversion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	get_single_config_attr("TZ_CONFIG_FILE_VERSION", param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}

	return FAULT_CODE_OK;
}
//InternetGatewayDevice.DeviceInfo.work_mode
int cpe_get_igd_di_workmode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	get_single_config_attr("WAN_MODE", param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}

	return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.ProvisioningCode
int cpe_get_igd_di_provisioningcode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return get_sys_parameter(X_SYS_ProvisioningCode, value);
}

//InternetGatewayDevice.DeviceInfo.UpTime
int cpe_get_igd_di_uptime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return get_sys_parameter(X_SYS_UpTime, value);
}


//InternetGatewayDevice.DeviceInfo.AdditionalHardwareVersion
int cpe_get_igd_di_AdditionalHardwareVersion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    	char buff[128];
	sprintf(buff, "cat /version | grep features= | awk -F= \'{print $2}\'");
	read_memory(buff,param,sizeof(param));
	util_remove_special_char(param,' ');
	util_strip_traling_spaces(param);
	param[strlen(param)-1] = '\0';
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.DeviceInfo.AdditionalSoftwareVersion
int cpe_get_igd_di_AdditionalSoftwareVersion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char shellcmd[128];
	sprintf(shellcmd, "cat /version | grep -E \"type|branch|build\" | awk -F= \'{print $2}\' | sed \':a;N;$!ba;s/\\n/,/g\'");
	read_memory(shellcmd,param,sizeof(param));
	util_strip_traling_spaces(param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
    return FAULT_CODE_OK;
}

int cpe_set_attr_UpTime(cwmp_t * cwmp, const char * name, int notiChange, int noti, 
						parameter_list_t *accList, int accListChange, callback_register_func_t callback_reg)
{
	if(!notiChange)
		return FAULT_CODE_OK;

	parameter_node_t *pn = cwmp_get_parameter_node(cwmp->root, name);
	if(pn == NULL)
		return FAULT_CODE_9005;
	switch(noti)
	{
		case 0:
		case 1:
		case 2:
			pn->attr.nc = noti;
			return FAULT_CODE_OK;
		case 3:
		case 4:
		case 5:
		case 6:
			return FAULT_CODE_9003;
		default:
			return FAULT_CODE_9003;
	}
}


#define DIAG_NONE "None"
#define DIAG_REQUESTED "Requested"
#define DIAG_COMPLETE "Complete"
#define DIAG_ERROR_CAN_NOT_RESOLVE_HOST_NAME "Error_CannotResolveHostName"
#define DIAG_ERROR_INTERNAL "Error_Internal"
#define DIAG_ERROR_OTHER "Error_Other"


static char diag_buffer[500];

static diag_ping_t diag_ping;
static int ping_init_flag = 0;
static int pingdiag_completed = 0;

void set_ping_state(char *state) {
	if (strcmp(state, DIAG_NONE) == 0) {
		ping_init_flag = 1;
	} else if (strcmp(state, DIAG_REQUESTED) == 0) {
	} else {
		ping_init_flag = 0;
	}
	strcpy(diag_ping.DiagnosticsState, state);
}

void init_ping() {
	if (ping_init_flag == 0) {
		ping_init_flag = 1;
		memset(&diag_ping, 0, sizeof(diag_ping));
		
		set_ping_state(DIAG_NONE);
		diag_ping.NumberOfRepetitions = 4;
		diag_ping.DataBlockSize = 56;
		diag_ping.Timeout = 10000;
	}
}

extern cwmp_t *get_root_cwmp(void);
extern int clear_and_cwmp_event_set_value(cwmp_t *cwmp,  int event,   int value, const char * cmd_key, int fault_code, time_t start, time_t end);


void *process_ping(void)
{
	pthread_detach(pthread_self());
	sleep(5);
	cwmp_log_debug("###################TR069 process_ping.###############\n");
	char shellcmd[256], buffer[4096], send[16], recv[16], times[128], tmp_time[16];
	char *head;
	
	set_ping_state(DIAG_REQUESTED);
	sprintf(shellcmd, 
			"ping -c %d -s %d \"%s\" > /tmp/tr069_pingtest.log",
			diag_ping.NumberOfRepetitions,
			diag_ping.DataBlockSize,
			diag_ping.Host);
	cwmp_log_debug("PING_HOST:%s", diag_ping.Host);
			
	system("killall ping > /dev/null 2>&1");
	system(shellcmd);
	sleep(1);
	
	read_memory("tail /tmp/tr069_pingtest.log", buffer, sizeof(buffer));

	head = strstr(buffer, "ping statistics");
	if (head == NULL) {
		cwmp_log_debug("can not found ping statistics, return!!!");
		set_ping_state(DIAG_ERROR_INTERNAL);
		pingdiag_completed = 0;
		pthread_exit("process_traceroute_exit");
	}
	head = strstr(head, "\n");
	if (head == NULL) {
		set_ping_state(DIAG_ERROR_INTERNAL);
		pingdiag_completed = 0;
		pthread_exit("process_traceroute_exit");
	}
	head++;
	
	//20 packets transmitted, 20 received
	sscanf(head, "%s%*s%*s%s%*s", send, recv);

	int totalCount = atoi(send);
	int successCount = atoi(recv);
	cwmp_log_debug("send: %d, recv: %d", totalCount, successCount);
	
	diag_ping.SuccessCount = successCount;
	diag_ping.FailureCount = totalCount - successCount;
	
	head = strstr(head, "\n");
	if (head == NULL) {
		set_ping_state(DIAG_ERROR_INTERNAL);
		pingdiag_completed = 0;
		pthread_exit("process_traceroute_exit");
	}
	head++;

	//rtt min/avg/max/mdev = 0.345/0.497/1.830/0.309 ms
	sscanf(head, "%*s%*s%*s%s%*s", times);
	cwmp_log_debug("min/avg/max/mdev = %s", times);

	char *tail, *p;

	int min;
	head = times;
	tail = strstr(head, "/");
	if (tail == NULL) {
		set_ping_state(DIAG_ERROR_INTERNAL);
		pingdiag_completed = 0;
		pthread_exit("process_traceroute_exit");
	}
	p = strstr(head, ".");
	if (p == NULL) {
		strncpy(tmp_time, head, tail - head);
		min = atoi(tmp_time);
	} else {
		strncpy(tmp_time, head, p - head);
		min = atoi(tmp_time) + 1;
	}
	cwmp_log_debug("min_time: %s", tmp_time);
	head = tail + 1;
	
	int avg;
	tail = strstr(head, "/");
	if (tail == NULL) {
		set_ping_state(DIAG_ERROR_INTERNAL);
		pingdiag_completed = 0;
		pthread_exit("process_traceroute_exit");
	}
	p = strstr(head, ".");
	if (p == NULL) {
		strncpy(tmp_time, head, tail - head);
		avg = atoi(tmp_time);
	} else {
		strncpy(tmp_time, head, p - head);
		avg = atoi(tmp_time) + 1;
	}
	cwmp_log_debug("avg_time: %s", tmp_time);
	head = tail + 1;

	int max;
	tail = strstr(head, "/");
	if (tail == NULL) {
		set_ping_state(DIAG_ERROR_INTERNAL);
		pingdiag_completed = 0;
		pthread_exit("process_traceroute_exit");
	}
	p = strstr(head, ".");
	if (p == NULL) {
		strncpy(tmp_time, head, tail - head);
		max = atoi(tmp_time);
	} else {
		strncpy(tmp_time, head, p - head);
		max = atoi(tmp_time) + 1;
	}
	cwmp_log_debug("max_time: %s", tmp_time);
	cwmp_log_debug("min: %d, avg: %d, max: %d", min, avg, max);

	diag_ping.MaximumResponseTime = max;
	diag_ping.MinimumResponseTime = min;
	diag_ping.AverageResponseTime = avg;
	
	cwmp_log_debug("TR069 DIAGNOSTICSCOMPLETE.");
	set_ping_state(DIAG_COMPLETE);
		
    //clear_and_cwmp_event_set_value(get_root_cwmp(), INFORM_TRANSFERCOMPLETE, 1, NULL, 0, 0, 0);
	clear_and_cwmp_event_set_value(get_root_cwmp(), INFORM_DIAGNOSTICSCOMPLETE, 1, NULL, 0, 0, 0);
	//cwmp_log_debug("ping end");
	cwmp_log_debug("###################TR069 process_ping end###############\n");
	pingdiag_completed = 0;
	pthread_exit("process_traceroute_exit");
}

int get_ping_parameter(enum ParameterPing paraName, const char *name, char **value) {
	switch (paraName) {
		case DIAG_PING_DiagnosticsState:
			strcpy(diag_buffer, diag_ping.DiagnosticsState);
			break;

		case DIAG_PING_Interface:
			strcpy(diag_buffer, diag_ping.Interface);
			break;
			
		case DIAG_PING_Host:
			strcpy(diag_buffer, diag_ping.Host);
			break;
			
		case DIAG_PING_NumberOfRepetitions:
			sprintf(diag_buffer, "%d", diag_ping.NumberOfRepetitions);
			break;
			
		case DIAG_PING_Timeout:
			sprintf(diag_buffer, "%d", diag_ping.Timeout);
			break;
			
		case DIAG_PING_DataBlockSize:
			sprintf(diag_buffer, "%d", diag_ping.DataBlockSize);
			break;
			
		case DIAG_PING_DSCP:
			sprintf(diag_buffer, "%d", diag_ping.DSCP);
			break;
			
		case DIAG_PING_SuccessCount:
			sprintf(diag_buffer, "%d", diag_ping.SuccessCount);
			break;
			
		case DIAG_PING_FailureCount:
			sprintf(diag_buffer, "%d", diag_ping.FailureCount);
			break;
			
		case DIAG_PING_AverageResponseTime:
			sprintf(diag_buffer, "%d", diag_ping.AverageResponseTime);
			break;
			
		case DIAG_PING_MinimumResponseTime:
			sprintf(diag_buffer, "%d", diag_ping.MinimumResponseTime);
			break;
			
		case DIAG_PING_MaximumResponseTime:
			sprintf(diag_buffer, "%d", diag_ping.MaximumResponseTime);
			break;

		default:
			break;
	}
	if (strlen(diag_buffer) == 0) {
		strcpy(diag_buffer, N_A);
	}
	replace_special_char(diag_buffer);
	
	*value = diag_buffer;

	return FAULT_CODE_OK;
}


int set_ping_parameter(enum ParameterPing paraName, const char *name, const char *value) {
	init_ping();
	
	switch (paraName) {
		case DIAG_PING_DiagnosticsState:
			memset(diag_ping.DiagnosticsState, 0, sizeof(diag_ping.DiagnosticsState));
			TRstrncpy(diag_ping.DiagnosticsState, value, strlen(value));
			break;

		case DIAG_PING_Interface:
			memset(diag_ping.Interface, 0, sizeof(diag_ping.Interface));
			TRstrncpy(diag_ping.Interface, value, strlen(value));
			break;
			
		case DIAG_PING_Host:
			memset(diag_ping.Host, 0, sizeof(diag_ping.Host));
			TRstrncpy(diag_ping.Host, value, strlen(value));
			break;
			
		case DIAG_PING_NumberOfRepetitions:
			diag_ping.NumberOfRepetitions = atoi(value);
			break;
			
		case DIAG_PING_Timeout:
			diag_ping.Timeout = atoi(value);
			break;
			
		case DIAG_PING_DataBlockSize:
			diag_ping.DataBlockSize = atoi(value);
			break;
			
		case DIAG_PING_DSCP:
			diag_ping.DSCP = atoi(value);
			break;

		default:
			break;
	}
	
	return FAULT_CODE_OK;
}

int cpe_get_igd_ping_DiagnosticsState(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_DiagnosticsState, name, value);
}

int cpe_set_igd_ping_DiagnosticsState(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_ping_parameter(DIAG_PING_DiagnosticsState, name, value);
}

int cpe_get_igd_ping_Interface(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_Interface, name, value);
}

int cpe_set_igd_ping_Interface(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	if (value != NULL) {
		return set_ping_parameter(DIAG_PING_Interface, name, value);
	}
	return FAULT_CODE_OK;
}

int cpe_get_igd_ping_Host(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_Host, name, value);
}

int cpe_set_igd_ping_Host(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	if (pingdiag_completed == 0) {
		cwmp_log_debug("TR069 PING");
		pingdiag_completed = 1;
		pthread_t pingdiag;
		cwmp_log_debug("TR069 BEGIG PING");
    	int status = pthread_create(&pingdiag, NULL, (void*)process_ping, NULL);
		if(status != 0) {
  			cwmp_log_error("pingdiag pthread_create error");
			pingdiag_completed = 0;
		}
		cwmp_log_debug("TR069 BEGIG TRACE ROUTE");
		if (value != NULL) {
			return set_ping_parameter(DIAG_PING_Host, name, value);
		}
	}
	return FAULT_CODE_OK;
}

int cpe_get_igd_ping_NumberOfRepetitions(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_NumberOfRepetitions, name, value);
}

int cpe_set_igd_ping_NumberOfRepetitions(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_ping_parameter(DIAG_PING_NumberOfRepetitions, name, value);
}

int cpe_get_igd_ping_Timeout(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_Timeout, name, value);
}

int cpe_set_igd_ping_Timeout(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_ping_parameter(DIAG_PING_Timeout, name, value);
}
	
int cpe_get_igd_ping_DataBlockSize(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_DataBlockSize, name, value);
}

int cpe_set_igd_ping_DataBlockSize(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_ping_parameter(DIAG_PING_DataBlockSize, name, value);
}

int cpe_get_igd_ping_DSCP(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_DSCP, name, value);
}

int cpe_set_igd_ping_DSCP(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_ping_parameter(DIAG_PING_DSCP, name, value);
}

int cpe_get_igd_ping_SuccessCount(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_SuccessCount, name, value);
}

int cpe_get_igd_ping_FailureCount(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_FailureCount, name, value);
}

int cpe_get_igd_ping_AverageResponseTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_AverageResponseTime, name, value);
}

int cpe_get_igd_ping_MinimumResponseTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_MinimumResponseTime, name, value);
}

int cpe_get_igd_ping_MaximumResponseTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ping_parameter(DIAG_PING_MaximumResponseTime, name, value);
}



static diag_traceroute_t diag_traceroute;
static int traceroute_init_flag = 0;
static int traceroute_completed = 0;
void set_traceroute_state(char *state) {
	if (strcmp(state, DIAG_NONE) == 0) {
		traceroute_init_flag = 1;
	} else if (strcmp(state, DIAG_REQUESTED) == 0) {
	} else {
		traceroute_init_flag = 0;
	}
	strcpy(diag_traceroute.DiagnosticsState, state);
}

void init_traceroute() {
	if (traceroute_init_flag == 0) {
		traceroute_init_flag = 1;
		memset(&diag_traceroute, 0, sizeof(diag_traceroute));
		
		set_traceroute_state(DIAG_NONE);
		diag_traceroute.NumberOfTries = 4;
		diag_traceroute.DataBlockSize = 56;
		diag_traceroute.Timeout = 10000;
	}
}

void *process_traceroute(void)
{
	pthread_detach(pthread_self());
	sleep(5);
	cwmp_log_debug("TR069 process_traceroute.");
	set_traceroute_state(DIAG_REQUESTED);
	char shellcmd[256], buffer[4096], line[512], host[64], tmp_time[32];
	char *head, *tail, *p;
	
	sprintf(shellcmd, 
			"tracepath \"%s\" > /tmp/tr069_tracepath.log",
			diag_traceroute.Host);
			
	system("killall tracepath > /dev/null 2>&1");
	system(shellcmd);

	read_memory("cat /tmp/tr069_tracepath.log", buffer, sizeof(buffer));

	int index = 0, count = 0, hopTime = 0, totalTime = 0;
	head = buffer;
	while (count < MAX_ROUTE_HOPS) {
		tail = strstr(head, "\n");
		if (tail == NULL) {
			break;
		}
		
		memset(line, 0, sizeof(line));
		strncpy(line, head, tail - head);
		head = tail + 1;

		cwmp_log_debug("LINE(%2d) %s", index + 1, line);

		if (strstr(line, "no reply") != NULL) {
			break;
		}

		p = strstr(line, ":");
		if (p == NULL) {
			count++;
			continue;
		}

		p++;
		while(*p == ' ') {
			p++;
		}

		// 1:	192.168.128.2				0.929ms 
		sscanf(p, "%s%s", host, tmp_time);
		cwmp_log_debug("%2d host: %s, ip: %s, time: %s", index + 1, host, host, tmp_time);

		/*p_ip = ip;
		if (*p_ip == '(') {
			p_ip++;
			strcpy(ip, p_ip);
			while (*p_ip != '\0') {
				if (*p_ip == ')') {
					*p_ip = '\0';
					break;
				}
				p_ip++;
			}
		}*/
		
		strcpy(diag_traceroute.RouteHops[index].HopHost, host);
		strcpy(diag_traceroute.RouteHops[index].HopHostAddress, host);

		hopTime = atoi(tmp_time) + 1;
		diag_traceroute.RouteHops[index].HopRTTimes = hopTime;
		totalTime = totalTime + hopTime;
		count++;
		index++;
	}
	diag_traceroute.ResponseTime = totalTime;
	diag_traceroute.MaxHopCount = count;
	diag_traceroute.HopsNumberOfEntries = index + 1;
	
	cwmp_log_debug("TR069 DIAGNOSTICSCOMPLETE.");
	set_traceroute_state(DIAG_COMPLETE);

	cwmp_t * cwmp = get_root_cwmp();
    //clear_and_cwmp_event_set_value(get_root_cwmp(), INFORM_TRANSFERCOMPLETE, 1, NULL, 0, 0, 0);
	clear_and_cwmp_event_set_value(cwmp, INFORM_DIAGNOSTICSCOMPLETE, 1, NULL, 0, 0, 0);
	cwmp_log_debug("tracepath end");
	
	parameter_node_t *param_node = get_node_after_paramname(cwmp->root, "RouteHops");
	parameter_node_t *child_param = param_node->child;
	parameter_node_t *tmp_param, *tmp_node, *hop_param;
	if(child_param)
	{
		for(tmp_param=child_param->next_sibling; tmp_param; )
		{
			cwmp_log_info("refresh X_CMCC_MonitorConfig node, delete param %s\n", tmp_param->name);
			tmp_node = tmp_param->next_sibling;
			cwmp_model_delete_parameter(tmp_param);
			tmp_param = tmp_node;
		}
		child_param->next_sibling = NULL;
	}

	int i;
	//parameter_node_t ;
	for (i = 0; i < count; i++) {
		cwmp_model_copy_parameter(param_node, &hop_param, i + 1);
	}
	traceroute_completed = 0;
	pthread_exit("process_traceroute_exit");
}

int get_routehops_index(const char *name) {
	return get_parameter_index((char *)name, "RouteHops.", MAX_ROUTE_HOPS) - 1;
}

int get_traceroute_parameter(enum ParameterTraceRoute paraName,const char *name, char **value) {

	int index = 0;
	switch (paraName) {
		case DIAG_TR_DiagnosticsState:
			strcpy(diag_buffer, diag_traceroute.DiagnosticsState);
			break;

		case DIAG_TR_Interface:
			strcpy(diag_buffer, diag_traceroute.Interface);
			break;
			
		case DIAG_TR_Host:
			strcpy(diag_buffer, diag_traceroute.Host);
			break;
			
		case DIAG_TR_NumberOfTries:
			sprintf(diag_buffer, "%d", diag_traceroute.NumberOfTries);
			break;
			
		case DIAG_TR_Timeout:
			sprintf(diag_buffer, "%d", diag_traceroute.Timeout);
			break;
			
		case DIAG_TR_DataBlockSize:
			sprintf(diag_buffer, "%d", diag_traceroute.DataBlockSize);
			break;
			
		case DIAG_TR_DSCP:
			sprintf(diag_buffer, "%d", diag_traceroute.DSCP);
			break;
			
		case DIAG_TR_MaxHopCount:
			sprintf(diag_buffer, "%d", diag_traceroute.MaxHopCount);
			break;
			
		case DIAG_TR_ResponseTime:
			sprintf(diag_buffer, "%d", diag_traceroute.ResponseTime);
			break;
			
		case DIAG_TR_HopsNumberOfEntries:
			sprintf(diag_buffer, "%d", diag_traceroute.HopsNumberOfEntries);
			break;
			
		case DIAG_TR_HopHost:
			index = get_routehops_index(name);
			strcpy(diag_buffer, diag_traceroute.RouteHops[index].HopHost);
			break;
			
		case DIAG_TR_HopHostAddress:
			index = get_routehops_index(name);
			strcpy(diag_buffer, diag_traceroute.RouteHops[index].HopHostAddress);
			break;
			
		case DIAG_TR_HopErrorCode:
			index = get_routehops_index(name);
			strcpy(diag_buffer, diag_traceroute.RouteHops[index].HopErrorCode);
			break;
			
		case DIAG_TR_HopRTTimes:
			index = get_routehops_index(name);
			sprintf(diag_buffer, "%d", diag_traceroute.RouteHops[index].HopRTTimes);
			break;

		default:
			break;
	}
	if (strlen(diag_buffer) == 0) {
		strcpy(diag_buffer, N_A);
	}
	replace_special_char(diag_buffer);
	
	*value = diag_buffer;

	return FAULT_CODE_OK;
}


int set_traceroute_parameter(enum ParameterPing paraName, const char *name, const char *value) {
	init_traceroute();
	
	switch (paraName) {
		case DIAG_TR_DiagnosticsState:
			strcpy(diag_traceroute.DiagnosticsState, value);
			break;

		case DIAG_TR_Interface:
			strcpy(diag_traceroute.Interface, value);
			break;
			
		case DIAG_TR_Host:
			strcpy(diag_traceroute.Host, value);
			break;
			
		case DIAG_TR_NumberOfTries:
			diag_traceroute.NumberOfTries = atoi(value);
			break;
			
		case DIAG_TR_Timeout:
			diag_traceroute.Timeout = atoi(value);
			break;
			
		case DIAG_TR_DataBlockSize:
			diag_traceroute.DataBlockSize = atoi(value);
			break;
			
		case DIAG_TR_DSCP:
			diag_traceroute.DSCP = atoi(value);
			break;
			
		case DIAG_TR_MaxHopCount:
			diag_traceroute.MaxHopCount = atoi(value);
			break;

		default:
			break;
	}
	
	return FAULT_CODE_OK;
}


int cpe_get_igd_tr_DiagnosticsState(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_DiagnosticsState, name, value);
}

int cpe_set_igd_tr_DiagnosticsState(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_traceroute_parameter(DIAG_TR_DiagnosticsState, name, value);
}

int cpe_get_igd_tr_Interface(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_Interface, name, value);
}

int cpe_set_igd_tr_Interface(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	if (value != NULL) {
		return set_traceroute_parameter(DIAG_TR_Interface, name, value);
	}
	return FAULT_CODE_OK;
}
	
int cpe_get_igd_tr_Host(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_Host, name, value);
}

int cpe_set_igd_tr_Host(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	if (traceroute_completed == 0) {
		cwmp_log_debug("TR069 TRACE ROUTE");
		traceroute_completed = 1;
		pthread_t traceroute;
    	int status = pthread_create(&traceroute, NULL, (void*)process_traceroute, NULL);
		if(status != 0) {
  			cwmp_log_error("traceroute pthread_create error");
			traceroute_completed = 0;
		}
		cwmp_log_debug("TR069 BEGIG TRACE ROUTE");
		if (value != NULL) {
			return set_traceroute_parameter(DIAG_TR_Host, name, value);
		}
	}
	return FAULT_CODE_OK;
}
	
int cpe_get_igd_tr_NumberOfTries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_NumberOfTries, name, value);
}

int cpe_set_igd_tr_NumberOfTries(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_traceroute_parameter(DIAG_TR_NumberOfTries, name, value);
}
	
int cpe_get_igd_tr_Timeout(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_Timeout, name, value);
}

int cpe_set_igd_tr_Timeout(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_traceroute_parameter(DIAG_TR_Timeout, name, value);
}
	
int cpe_get_igd_tr_DataBlockSize(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_DataBlockSize, name, value);
}

int cpe_set_igd_tr_DataBlockSize(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_traceroute_parameter(DIAG_TR_DataBlockSize, name, value);
}
	
int cpe_get_igd_tr_DSCP(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_DSCP, name, value);
}

int cpe_set_igd_tr_DSCP(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_traceroute_parameter(DIAG_TR_DSCP, name, value);
}
		
int cpe_get_igd_tr_MaxHopCount(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_MaxHopCount, name, value);
}

int cpe_set_igd_tr_MaxHopCount(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_traceroute_parameter(DIAG_TR_MaxHopCount, name, value);
}
		
int cpe_get_igd_tr_ResponseTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_ResponseTime, name, value);
}
		
int cpe_get_igd_tr_HopsNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_HopsNumberOfEntries, name, value);
}
		
int cpe_get_igd_tr_HopHost(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_HopHost, name, value);
}
		
int cpe_get_igd_tr_HopHostAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_HopHostAddress, name, value);
}
		
int cpe_get_igd_tr_HopErrorCode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_HopErrorCode, name, value);
}
		
int cpe_get_igd_tr_HopRTTimes(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_traceroute_parameter(DIAG_TR_HopRTTimes, name, value);
}

int cpe_add_igd_tr_RouteHops(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
	cwmp_log_info("cpe_add_igd_tr_RouteHops");
	
	if(!param_node)
    {
        return FAULT_CODE_9002;
    }

    int num = diag_traceroute.HopsNumberOfEntries;
    int i;

	cwmp_log_info("cpe_add_igd_tr_RouteHops node=%s",param_node->name);
	*pinstance_number = 1;
    
    parameter_node_t  *child_param;
    child_param = param_node->child;
    if(child_param)
    {
        /*for(tmp_param=child_param->next_sibling; tmp_param; )
        {
            cwmp_log_info("refresh X_CMCC_MonitorConfig node, delete param %s\n", tmp_param->name);
            tmp_node = tmp_param->next_sibling;
            cwmp_model_delete_parameter(tmp_param);
            tmp_param = tmp_node;
        }
        child_param->next_sibling = NULL; */

        parameter_node_t * wan1_param;
        for (i = 0; i < num; i++) {
        	cwmp_model_copy_parameter(param_node, &wan1_param, i + 1);
        }

        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }

    *pinstance_number = num++;

	cwmp_log_info("==================== add ");	
    _walk_parameter_node_tree(param_node, 0);
		
	return FAULT_CODE_OK;
}

int cpe_del_igd_tr_RouteHops(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
	if(!param_node)
    {
        return FAULT_CODE_9002;
    }
		
	return FAULT_CODE_OK;
}

int  cpe_refresh_igd_tr_RouteHops(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, 1);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);
    
    return FAULT_CODE_OK;
}

extern int get_wan_parameter(enum ParameterName name, char **value);

int cpe_get_igd_wan_ip_WANIPConnectionEnable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_Enable, value);
}

int cpe_get_igd_wan_ip_AddressingType(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_AddressingType, value);
}

int cpe_get_igd_wan_ip_ExternalIPAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_ExternalIPAddress, value);
}

int cpe_get_igd_wan_ip_SubnetMask(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_SubnetMask, value);
}

int cpe_get_igd_wan_ip_DefaultGateway(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_DefaultGateway, value);
}

int cpe_get_igd_wan_ip_DNSServers(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_DNSServers, value);
}

int cpe_get_igd_wan_ip_MACAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_MACAddress, value);
}

int cpe_get_igd_wan_ip_ConnectionStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_ConnectionStatus, value);
}

int cpe_get_igd_wan_ip_ConnectionTrigger(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_ConnectionTrigger, value);
}

int cpe_get_igd_wan_ip_ConnectionType(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_ConnectionType, value);
}


int cpe_get_igd_wan_ip_DNSEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_DNSEnabled, value);
}


int cpe_get_igd_wan_ip_DNSOverrideAllowed(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_DNSOverrideAllowed, value);
}


int cpe_get_igd_wan_ip_LastConnectionError(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_LastConnectionError, value);
}


int cpe_get_igd_wan_ip_MACAddressOverride(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_MACAddressOverride, value);
}


int cpe_get_igd_wan_ip_MaxMTUSize(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_MaxMTUSize, value);
}


int cpe_get_igd_wan_ip_Name(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_Name, value);
}



int cpe_get_igd_wan_ip_PossibleConnectionTypes(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_PossibleConnectionTypes, value);
}


int cpe_get_igd_wan_ip_RSIPAvailable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_RSIPAvailable, value);
}


int cpe_get_igd_wan_ip_RouteProtocolRx(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_RouteProtocolRx, value);
}


int cpe_get_igd_wan_ip_Uptime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_wan_parameter(X_CMCC_WAN_Uptime, value);
}

extern int get_remote_login_status(char **value);
extern int process_remote_login(int enabled);


int cpe_get_igd_https_Status(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_remote_login_status(value);
}

int cpe_set_igd_https_Status(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return process_remote_login(is_true_value(value));
}


int https_port_get_form_url(char *acs_url,char *result)
{
	char buf[64]={0};
	int index,num;
	if (acs_url == NULL)
	{
		strcpy(param,"443");
		return 0;
	}
	int length= strlen(acs_url);
    for(index=0;index<length-1;index++){
		if(acs_url[index]==':'&&acs_url[index+1]!='/')
		{
			for( num=0;num<length-1-index;num++)
			{
				if(acs_url[index+num+1]=='/')
				{
					break;
				}
				buf[num]=acs_url[index+1+num];
			}
			break;
		}
	}
	strcpy(result,buf);
    return 1;
}

int cpe_get_igd_https_Port(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	if(strstr(cwmp->acs_url,"https")!=NULL)
	{
       https_port_get_form_url(cwmp->acs_url,param);
	}else{
	strcpy(param,"443");
	}
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_set_igd_https_Port(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{

	return FAULT_CODE_OK;
}


int cpe_get_igd_DownloadTransports(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"HTTP,FTP");
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_igd_UploadTransports(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"HTTP");
	*value = param;
	return FAULT_CODE_OK;
}


int cpe_get_igd_Time_Enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	read_memory("uci -q get system.ntp.enabled", param, 64); 
    util_strip_traling_spaces(param);
	if (strlen(param) == 0) {
	   strcpy(param,"true");
	} else if(!strcmp(param,"1")){
		strcpy(param,"true");
	}else{
		strcpy(param,"false");
	}
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_igd_Time_Status(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"Synchronized");
	*value = param;
	return FAULT_CODE_OK;
}
int cpe_get_igd_UpdateURL(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"query.hicloud.com");
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_igd_UpdatePort(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"80");
	*value = param;
	return FAULT_CODE_OK;
}

int openwrt_ntp_set_value(int index,char *value)
{
	char tempbuf[128];
	char cmdbuf[256];
	char ntpserver1[64]={0};
	char ntpserver2[64]={0};
	char ntpserver3[64]={0};
	char ntpserver4[64]={0};

	if ( value == NULL || strlen(value) == 0) {
		return 0;
	}
    read_memory("uci -q get system.ntp.server | awk '{print $1}'", ntpserver1, 64); 
    read_memory("uci -q get system.ntp.server | awk '{print $2}'", ntpserver2, 64); 
    read_memory("uci -q get system.ntp.server | awk '{print $3}'", ntpserver3, 64); 		
	read_memory("uci -q get system.ntp.server | awk '{print $4}'", ntpserver4, 64); 
    if(index==0)
	{
      strcpy(ntpserver1,value);
	}
	else  if(index==1)
	{
 		strcpy(ntpserver2,value);
	}
	else  if(index==2)
	{
		 strcpy(ntpserver3,value);
	}
	else  if(index==3)
	{
		 strcpy(ntpserver4,value);
	}
	else{
		return 0;
	}	
    sprintf(cmdbuf,"uci -q set system.ntp.server='%s %s %s %s';uci -q commit system",ntpserver1,ntpserver2,ntpserver3,ntpserver4);
    system(cmdbuf);   
	return 1;
}


int cpe_get_igd_Time_NTPServer0(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    read_memory("uci -q get system.ntp.server | awk '{print $1}'", param, 64); 
    util_strip_traling_spaces(param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}

	return FAULT_CODE_OK;

}

int cpe_set_igd_Time_NTPServer0(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    openwrt_ntp_set_value(0,value);
	return FAULT_CODE_OK;
}


int cpe_get_igd_Time_NTPServer1(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    read_memory("uci -q get system.ntp.server | awk '{print $2}'", param, 64); 
    util_strip_traling_spaces(param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}

	return FAULT_CODE_OK;
}

int cpe_set_igd_Time_NTPServer1(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    openwrt_ntp_set_value(1,value);
	return FAULT_CODE_OK;
}


int cpe_get_igd_Time_NTPServer2(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    read_memory("uci -q get system.ntp.server | awk '{print $3}'", param, 64); 
    util_strip_traling_spaces(param);
	//get_single_config_attr("TZ_NTP_SERVER2", param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}

	return FAULT_CODE_OK;
	
}

int cpe_set_igd_Time_NTPServer2(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    openwrt_ntp_set_value(2,value);
	return FAULT_CODE_OK;
}


int cpe_get_igd_Time_NTPServer3(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    read_memory("uci -q get system.ntp.server | awk '{print $4}'", param, 64); 
    util_strip_traling_spaces(param);
    	//get_single_config_attr("TZ_NTP_SERVER3", param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	return FAULT_CODE_OK;
}

int cpe_set_igd_Time_NTPServer3(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    openwrt_ntp_set_value(3,value);
	return FAULT_CODE_OK;
}


int cpe_get_igd_Time_NTPServer4(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    read_memory("uci -q get system.ntp.server | awk '{print $4}'", param, 64); 
    util_strip_traling_spaces(param);
	//get_single_config_attr("TZ_NTP_SERVER4", param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	return FAULT_CODE_OK;
}

int cpe_set_igd_Time_NTPServer4(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{


	return FAULT_CODE_OK;
}


extern int get_sys_time(char *theDatetime);
extern int load_timeinfor_cmd(char *time_info);

int cpe_get_igd_Time_CurrentLocalTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	get_sys_time(param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	return FAULT_CODE_OK;
}


int cpe_get_igd_Time_LocalTimeZone(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	load_timeinfor_cmd(param);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	return FAULT_CODE_OK;
}


int cpe_get_igd_Time_LocalTimeZoneName(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    read_memory("uci -q get system.@system[0].zonename", param, 64); 
    util_strip_traling_spaces(param);
    if (strlen(param) == 0) {
    	*value = NULL;
    } else {
    	*value = param;
    }
    return FAULT_CODE_OK;
}

int cpe_get_FirstUseDate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	if(!cmd_file_exist("/etc/.firstusedate"))
	{
		system("date > /etc/.firstusedate");
	}
	cmd_cat("/etc/.firstusedate", param, sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_ProcessorNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "1");
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_ArchitectureOfProcessor(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "mipsel");
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_CPUUsage(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	read_memory("top -n 1 | grep CPU | awk '{print $2}' | grep '%'", param, sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_ProcessNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	sprintf(param, "%d", igd_entries.process_entry);
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_TotalOfMemoryStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char mem_free[128];

	util_read_meminfo(param, mem_free);
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_FreeOfMemoryStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char mem[128];

	util_read_meminfo(mem, param);
	*value = param;

	return FAULT_CODE_OK;
}

int  cpe_refresh_Processor(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, 1);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int  cpe_refresh_VendorLogFile(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, 1);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int  cpe_refresh_ProcessStatus(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
	int num;
	char process_num[32];

	system("ps > /tmp/.process_status");
	read_memory("cat /tmp/.process_status | wc -l", process_num, sizeof(process_num));
	num = atoi(process_num);
	if(num >= 1)
		igd_entries.process_entry = num - 1;
	else
		igd_entries.process_entry = 0;
    cwmp_refresh_i_parameter(cwmp, param_node, igd_entries.process_entry);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int cpe_get_PIDOfProcess(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index;
	char cmdbuf[64];

	index = get_parameter_index((char *)name, "Process.", igd_entries.process_entry);
	sprintf(cmdbuf, "awk 'NR==%d {print $1}' /tmp/.process_status", index + 1);
	read_memory(cmdbuf, param, sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_CommandOfProcess(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char cmdbuf[64];

	int index = get_parameter_index((char *)name, "Process.", igd_entries.process_entry);
	sprintf(cmdbuf, "awk 'NR==%d {print $5}' /tmp/.process_status", index + 1);
	read_memory(cmdbuf, param, sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_SizeOfProcess(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char cmdbuf[64];

	int index = get_parameter_index((char *)name, "Process.", igd_entries.process_entry);
	sprintf(cmdbuf, "awk 'NR==%d {print $3}' /tmp/.process_status", index + 1);
	read_memory(cmdbuf, param, sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_StateOfProcess(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char cmdbuf[64], state[64];

	int index = get_parameter_index((char *)name, "Process.", igd_entries.process_entry);
	sprintf(cmdbuf, "awk 'NR==%d {print $4}' /tmp/.process_status", index + 1);
	read_memory(cmdbuf, state, sizeof(state));
	if(!strncmp(state, "R", 1))
		strcpy(param, "Running");
	else if(!strncmp(state, "S", 1))
		strcpy(param, "Sleeping");
	else if(!strncmp(state, "I", 1))
		strcpy(param, "Idle");
	else if(!strncmp(state, "D", 1))
		strcpy(param, "Uninterruptible");
	else if(!strncmp(state, "Z", 1))
		strcpy(param, "Zombie");
	else if(!strncmp(state, "T", 1))
		strcpy(param, "Stopped");
	else if(!strncmp(state, "SW<", 3))
		strcpy(param, "Sleeping");
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_NameOfVendorLogFile(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, VENDOR_LOG_FILE);
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_MaximumSizeOfVendorLogFile(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, VENDOR_LOG_MAX_SIZE);
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_PersistentOfVendorLogFile(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "true");
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_PersistentData(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "true");
	*value = param;

    return FAULT_CODE_OK;
}
char configfile[32768];
//#define SRCCONFIGFILE "/tmp/.tr069_srcconfig_file"
int cpe_get_ConfigFile(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char shellcmd[32] = {0};
	int len = 0;
	strcpy(shellcmd, "cfg -e | wc -c");
	read_memory(shellcmd,param,sizeof(param));
	len = atoi(param);
	if( len < 32766 && len != 0) {
		strcpy(shellcmd, "cfg -e");
		read_memory(shellcmd,configfile,sizeof(configfile));
		if (strlen(configfile) == 0) {
			*value = NULL;
		} else {
			util_strip_traling_spaces(configfile);
			*value = configfile;
			}
	} else {
		strcpy(param,"The configuration file is larger than the parameter size limit, please use the Upload and Download RPCs with a FileType of 1 Vendor Configuration File.");
		*value = param;
	}
    return FAULT_CODE_OK;
}

/*int cpe_set_ConfigFile(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{

	if (strlen(value) != 0 ) {
		FILE *srcf = fopen(SRCCONFIGFILE, "wb");
		if(srcf == NULL) {
			cwmp_log_debug("open tr069 srcconfigfile faild!");
			goto error;
		}
		if(fprintf(srcf, "%s", value) < 0) {
			goto error;
		}
		fclose(srcf);
		system("cat /tmp/.tr069_srcconfig_file");
		
		
	} else {
		goto error;
	}
	return FAULT_CODE_OK;
error:
	return FAULT_CODE_9007;
	
}*/


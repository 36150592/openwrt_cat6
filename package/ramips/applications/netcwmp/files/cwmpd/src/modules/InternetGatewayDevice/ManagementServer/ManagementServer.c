#include <stdlib.h>
static char buffer[256];

int cpe_get_localip(const char * eth_name, char *hostip)
{
    register int fd,intrface,retn=0;
    struct ifreq buf[32];
    struct ifconf ifc;
    char domain_host[100] = {0};
    char local_ip_addr[20] = {0};
//    char local_mac[20] = {0};
    
    //Get Domain Name --------------------------------------------------
    strcpy(local_ip_addr, "172.16.0.76");
    if (!hostip)
        return -1;
    if (getdomainname(&domain_host[0], 100) != 0)
    {
        return -1;
    }
    //------------------------------------------------------------------
    //Get IP Address & Mac Address ----------------------------------------
    if ((fd=socket(AF_INET,SOCK_DGRAM,0))>=0)
    {
        ifc.ifc_len=sizeof buf;
        ifc.ifc_buf=(caddr_t)buf;
        if (!ioctl(fd,SIOCGIFCONF,(char*)&ifc))
        {
            intrface=ifc.ifc_len/sizeof(struct ifreq);
            while (intrface-->0)
            {
                if (!(ioctl(fd,SIOCGIFFLAGS,(char*)&buf[intrface])))
                {
                    if (buf[intrface].ifr_flags&IFF_PROMISC)
                    {
                        retn++;
                    }
                }
                //Get IP Address
                if (!(ioctl(fd,SIOCGIFADDR,(char*)&buf[intrface])))
                {
		    if(strcmp(eth_name, buf[intrface].ifr_name) == 0)
		    {
                    sprintf(local_ip_addr, "%s", inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
		
		    break;
		    }
                }
                //Get Hardware Address

            }//While
        }
    }
    if ( fd > 0 )
    {
        close(fd);
    }

    strcpy(hostip, local_ip_addr);

    return CWMP_OK;
}


int cwmp_conf_pool_get_value(const char * name, char ** value, pool_t * pool) {
	char *theValue = cwmp_conf_pool_get(pool, name);
	if (theValue == NULL || strlen(theValue) == 0) {
		*value = NULL;
	} else {
		*value = theValue;
	}
    return FAULT_CODE_OK;
}
//InternetGatewayDevice.ManagementServer.Username
int cpe_get_igd_ms_username(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return cwmp_conf_pool_get_value("cwmp:acs_username", value, pool);
}

//InternetGatewayDevice.ManagementServer.Username
int cpe_set_igd_ms_username(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	char combuf[128];
	cwmp_conf_set("cwmp:acs_username", value);
	/*
	if(isTestConfig())
	{
		set_single_config_attr("TZ_TR069_USERNAME2", value);
	}
	else
	{
		set_single_config_attr(TZ_TR069_USERNAME, value);
	}
	*/
    snprintf(combuf,sizeof(combuf),"uci set tozed.cfg.tr069_ServerUsername=%s 2>/dev/null&&uci commit tozed",value);

	uci_set_single_config_attr(combuf, "y");


    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.Password
int cpe_get_igd_ms_password(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	// *value = cwmp_conf_pool_get(pool, "cwmp:acs_password");
	*value = NULL;
	
	return FAULT_CODE_OK;
}

int cpe_set_igd_ms_password(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
		char combuf[128];
	cwmp_conf_set("cwmp:acs_password", value);
	/*
	if(isTestConfig())
		set_single_config_attr("TZ_TR069_PASSWD2", value);
	else
		set_single_config_attr(TZ_TR069_PASSWD, value);
*/
    snprintf(combuf,sizeof(combuf),"uci set tozed.cfg.tr069_ServerPassword=%s 2>/dev/null&&uci commit tozed",value);

	uci_set_single_config_attr(combuf, "y");		
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.URL
int cpe_get_igd_ms_url(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return cwmp_conf_pool_get_value("cwmp:acs_url", value, pool);
}

//InternetGatewayDevice.ManagementServer.URL
int cpe_set_igd_ms_url(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
		char combuf[128];
		
	cwmp_conf_set("cwmp:acs_url", value);
	/*
	if(isTestConfig())
		set_single_config_attr("TZ_TR069_URL2", value);
	else
		set_single_config_attr(TZ_TR069_URL, value);
*/
    snprintf(combuf,sizeof(combuf),"uci set tozed.cfg.tr069_ServerURL=%s 2>/dev/null&&uci commit tozed",value);

	uci_set_single_config_attr(combuf, "y");		
	
    return FAULT_CODE_OK;
}




//InternetGatewayDevice.ManagementServer.ConnectionRequestURL
int cpe_get_igd_ms_connectionrequesturl(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char *wanIp;
	get_wan_parameter(X_CMCC_WAN_ExternalIPAddress, &wanIp);
	sprintf(buffer, "http://%s:%d", wanIp, cwmp->httpd_port);
	*value = buffer;
	
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.ConnectionRequestUsername
int cpe_get_igd_ms_connectionrequestusername(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return cwmp_conf_pool_get_value("cwmp:cpe_username", value, pool);
}

int cpe_set_igd_ms_connectionrequestusername(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	char combuf[128]={0};
	cwmp_conf_set("cwmp:cpe_username", value);
	/*
	if(isTestConfig())
	{
		set_single_config_attr("TZ_TR069_LINK_USERNAME2", value);
		if (value != NULL && strlen(value) > 0) {
			set_single_config_attr("TZ_TR069_CPE_AUTH_ENABLED2", "1");
		} else {
			set_single_config_attr("TZ_TR069_CPE_AUTH_ENABLED2", "0");
		}
	}
	else
	{
		set_single_config_attr(TZ_TR069_LINK_USERNAME, value);
		if (value != NULL && strlen(value) > 0) {
			set_single_config_attr(TZ_TR069_CPE_AUTH_ENABLED, "y");
		} else {
			set_single_config_attr(TZ_TR069_CPE_AUTH_ENABLED, "n");
		}
	}
*/
    snprintf(combuf,sizeof(combuf),"uci set tozed.cfg.tr069_ConnectionRequestUname=%s 2>/dev/null&&uci commit tozed",value);

	uci_set_single_config_attr(combuf, "y");		

    return FAULT_CODE_OK;
}


//InternetGatewayDevice.ManagementServer.ConnectionRequestPassword
int cpe_get_igd_ms_connectionrequestpassword(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_password");
    return FAULT_CODE_OK;
}

int cpe_set_igd_ms_connectionrequestpassword(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	char combuf[128]={0};
	cwmp_conf_set("cwmp:cpe_password", value);
	/*
	if(isTestConfig())
		set_single_config_attr("TZ_TR069_LINK_PASSWD2", value);
	else
		set_single_config_attr(TZ_TR069_LINK_PASSWD, value);
		*/
    snprintf(combuf,sizeof(combuf),"uci set tozed.cfg.tr069_ConnectionRequestPassword=%s 2>/dev/null&&uci commit tozed",value);

	uci_set_single_config_attr(combuf, "y");				
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.ManagementServer.UpgradesManaged
int cpe_get_igd_ms_UpgradesManaged(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	strcpy(param,"false");
    *value = param;
    return FAULT_CODE_OK;
}

int TRF_Get_DeviceSummary(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	sprintf(param, "%d", igd_entries.lan_entry + igd_entries.wlan_entry);
	*value = param;

    return FAULT_CODE_OK;
}

int TRF_Get_LANDeviceNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	sprintf(param, "%d", igd_entries.lan_entry);
	*value = param;

	return FAULT_CODE_OK;
}

extern int getNetStat(char *ethname);
extern int process_lock_cell(const char *value);
extern char *getSupportedBands();
extern int setSupportedBands(const char *value);
extern int get_ppp_parameter(enum ParameterName paramName,const char *name, char **value);
extern int set_ppp_parameter(enum ParameterName paramName,const char *name, const char *value);


int cpe_get_igd_wan_WANIPConnectionNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(WANIPConnectionNumberOfEntries, value);
}

int cpe_get_igd_wan_EthernetLinkStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param, "usb%d", 0);
    strcpy(param, getNetStat(param)? "Up" : "Down");
	*value = param;
	return FAULT_CODE_OK;
}

int TRF_Get_WANDeviceNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(TRF_WANDeviceNumberOfEntries, value);
}


int cpe_get_igd_di_X_CMCC_WANDeviceNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_WANDeviceNumberOfEntries, value);
}

int cpe_get_igd_di_X_CMCC_NetRegStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_NetRegStatus, value);
}

int cpe_get_igd_di_X_CMCC_GprsRegStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_GprsRegStatus, value);
}

int cpe_get_igd_di_X_CMCC_EpsRegStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_EpsRegStatus, value);
}

int cpe_get_igd_di_X_CMCC_CurrentNetwork(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_CurrentNetwork, value);
}

int cpe_get_igd_di_X_CMCC_CurrentNetmode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_CurrentNetmode, value);
}

int cpe_get_igd_di_X_CMCC_NetworkPriority(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_NetworkPriority, value);
}

int cpe_get_igd_di_X_CMCC_SingalLevel(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_SingalLevel, value);
}

int cpe_get_igd_di_X_CMCC_Txpower(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_Txpower, value);
}

int cpe_get_igd_di_X_CMCC_FrequencyPoint(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_FrequencyPoint, value);
}

int cpe_get_igd_di_X_CMCC_Band(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_Band, value);
}

int cpe_get_igd_di_X_CMCC_BandWidth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_BandWidth, value);
}

int cpe_get_igd_di_X_CMCC_EnodeBId(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_EnodeBId, value);
}

int cpe_get_igd_di_X_CMCC_LAC(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_LAC, value);
}

int cpe_get_igd_di_X_CMCC_CellId(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_CellId, value);
}

int cpe_get_igd_di_X_CMCC_GlobeCellId(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_GlobeCellId, value);
}

int cpe_get_igd_di_X_CMCC_PhysicsCellId(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_PhysicsCellId, value);
}

int cpe_get_igd_di_X_CMCC_ICCID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_ICCID, value);
}

int cpe_get_igd_di_X_CMCC_APN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_APN, value);
}

int cpe_get_igd_di_X_CMCC_IsFrequencyLock(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_IsFrequencyLock, value);
}

int cpe_get_igd_di_X_CMCC_FrequencyLocking(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_FrequencyLocking, value);
}

int cpe_get_igd_di_X_CMCC_FrequencyLockInfo(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_FrequencyLockInfo, value);
}

int cpe_get_igd_di_X_CMCC_IsCellLock(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_IsCellLock, value);
}

int cpe_get_igd_di_X_CMCC_CellLockType(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_CellLockType, value);
}

int cpe_get_igd_di_X_CMCC_CellLocking(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_CellLocking, value);
}

int cpe_set_igd_di_X_CMCC_CellLocking(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return process_lock_cell(value);
}

int cpe_get_igd_di_X_CMCC_SupportedBands(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	*value = getSupportedBands();

	return 0;
}

int cpe_set_igd_di_X_CMCC_SupportedBands(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return setSupportedBands(value);
}

int cpe_get_igd_di_X_CMCC_CellLockInfo(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_CellLockInfo, value);
}

int cpe_get_igd_di_X_CMCC_LockPin(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_LockPin, value);
}

int cpe_get_igd_di_X_CMCC_LockPinType(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_LockPinType, value);
}

int cpe_get_igd_di_X_CMCC_FirstPin(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_FirstPin, value);
}

int cpe_get_igd_di_X_CMCC_FixedPin(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_FixedPin, value);
}

int cpe_get_igd_di_X_CMCC_EncryptCard(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_EncryptCard, value);
}

int cpe_get_igd_di_X_CMCC_EncryptCardKey(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_EncryptCardKey, value);
}

int cpe_get_igd_di_X_CMCC_PLMN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_PLMN, value);
}

int cpe_get_igd_di_X_CMCC_TAC(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_TAC, value);
}

int cpe_get_LockLteBands(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_LOCKLTEBAND, value);
}


int cpe_get_igd_di_X_CMCC_ECGI(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_ECGI, value);
}


int cpe_get_igd_di_X_CMCC_CQI(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_CQI, value);
}

int cpe_get_igd_di_X_CMCC_MCS(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_MCS, value);
}

int cpe_get_igd_di_X_CMCC_SignalQuality(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int SignalQuality = 0;
	char buf[64];

	int tValue=0;
	get_parameter(X_CMCC_SignalQuality, value);

	if(*value==NULL||strlen(*value)==0)
	{
		strcpy(param, "0");
	    *value = param;
		return FAULT_CODE_OK;	
	}

	tValue = atoi(*value);
	
	//p[0] = -115; p[1] = -110; p[2] = -105; p[3] = -97; p[4] = -84;
	if(tValue==0)
	{
		strcpy(param, "0");
	    *value = param;
	}else{
		if(tValue<= -115)
		{
			strcpy(param, "0");
			*value = param;
		}else if(tValue<= -110){
			strcpy(param, "1");
			*value = param;
		}else if(tValue<= -105){
			strcpy(param, "2");
			*value = param;
		}else if(tValue<= -97){
			strcpy(param, "3");
			*value = param;
		}else if(tValue<= -84){
			strcpy(param, "4");
			*value = param;
		}else{
			strcpy(param, "5");
			*value = param;
		}
	}
	return FAULT_CODE_OK;	
}

int cpe_get_igd_di_X_CMCC_RSRP(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_RSRP, value);
}

int cpe_get_igd_di_X_CMCC_RSRQ(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_RSRQ, value);
}

int cpe_get_igd_di_X_CMCC_RSSI(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_RSSI, value);
}

int cpe_get_igd_di_X_CMCC_SINR(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_SINR, value);
}

int cpe_get_igd_di_X_CMCC_UpstreamTotalByte(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_UpstreamTotalByte, value);
}

int cpe_get_igd_di_X_CMCC_DownstreamTotalByte(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_DownstreamTotalByte, value);
}

int cpe_get_igd_di_X_CMCC_StartTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_StartTime, value);
}

int cpe_get_igd_di_X_CMCC_LoadAverage(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_LoadAverage, value);
}

int cpe_get_igd_di_X_CMCC_DeviceMemory(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_DeviceMemory, value);
}

int cpe_get_igd_di_X_CMCC_MTU(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_MTU, value);
}

int cpe_get_igd_di_X_CMCC_IMSI(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_IMSI, value);
}

int cpe_get_igd_di_X_CMCC_IMEI(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	    char shcmd[64];
		sprintf(shcmd, "eth_mac g imei 2>/dev/null");
		read_memory(shcmd, param, sizeof(param));
		util_strip_traling_spaces(param);
		if (strlen(param) == 0) {
			*value = NULL;
		} else {
			*value = param;
		}
		return FAULT_CODE_OK;
	//return get_parameter(X_CMCC_IMEI, value);
}

int cpe_get_igd_di_X_CMCC_ConfigVersion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_ConfigVersion, value);
}

int cpe_get_igd_di_X_CMCC_ModuleVersion(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_ModuleVersion, value);
}

int cpe_get_igd_di_X_CMCC_ModuleType(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_parameter(X_CMCC_ModuleType, value);
}

int cpe_get_igd_WANPPPConnection_Enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ppp_parameter(X_CMCC_PPP_Enable, name, value);
}

int cpe_set_igd_WANPPPConnection_Enable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_ppp_parameter(X_CMCC_PPP_Enable, name, value);
}

int cpe_get_igd_WANPPPConnection_Name(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ppp_parameter(X_CMCC_PPP_Name, name, value);
}

int cpe_set_igd_WANPPPConnection_Name(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	return set_ppp_parameter(X_CMCC_PPP_Name, name, value);
}

int cpe_get_igd_WANPPPConnection_Uptime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ppp_parameter(X_CMCC_PPP_Uptime, name, value);
}

int cpe_get_igd_WANPPPConnection_LastConnectionError(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ppp_parameter(X_CMCC_PPP_LastConnectionError, name, value);
}

int cpe_get_igd_WANPPPConnection_Username(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ppp_parameter(X_CMCC_PPP_Username, name, value);
}

int cpe_get_igd_WANPPPConnection_Password(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ppp_parameter(X_CMCC_PPP_Password, name, value);
}

int cpe_get_igd_WANPPPConnection_ExternalIPAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	return get_ppp_parameter(X_CMCC_PPP_ExternalIPAddress, name, value);
}

int cpe_get_igd_WANPPPConnection_DNSEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WANPPPConnection_DNSEnabled(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANPPPConnection_DNSServers(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WANPPPConnection_DNSServers(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANPPPConnection_MACAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANPPPConnection_X_CMCC_TunnelDial(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WANPPPConnection_X_CMCC_TunnelDial(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANPPPConnection_X_CMCC_LNS(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "cpe_get_igd_WANPPPConnection_X_CMCC_LNS");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WANPPPConnection_X_CMCC_LNS(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANPPPConnection_X_CMCC_LnsIp(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WANPPPConnection_X_CMCC_LnsIp(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANPPPConnection_X_CMCC_LnsName(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WANPPPConnection_X_CMCC_LnsName(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANPPPConnection_X_CMCC_LnsPassword(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WANPPPConnection_X_CMCC_LnsPassword(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANPPPConnection_X_CMCC_L2TPMode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WANPPPConnection_X_CMCC_L2TPMode(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANPPPConnection_X_CMCC_UpstreamTotalByte(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANPPPConnection_X_CMCC_DownstreamTotalByte(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_add_igd_WANPPPConnection(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
	if(!param_node)
		return FAULT_CODE_9002;
    
	static int num=1;

	cwmp_log_info("=================================");
	cwmp_log_info("cpe_add_igd_WANPPPConnection node=%s",param_node->name);
    
    parameter_node_t * tmp_param, *tmp_node, *child_param;
    child_param = param_node->child;
    if(child_param)
    {
        for(tmp_param=child_param->next_sibling; tmp_param; )
        {
            cwmp_log_info("refresh WANPPPConnection node, delete param %s\n", tmp_param->name);
            tmp_node = tmp_param->next_sibling;
            cwmp_model_delete_parameter(tmp_param);
            tmp_param = tmp_node;
        }
        child_param->next_sibling = NULL;

        parameter_node_t * wan1_param;
        cwmp_model_copy_parameter(param_node, &wan1_param, num);

        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }

    *pinstance_number = num++;

    _walk_parameter_node_tree(cwmp->root, 0);
		
	return FAULT_CODE_OK;
}

int cpe_del_igd_WANPPPConnection(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
	cwmp_log_info("cpe_del_igd_WANPPPConnection node=%s",param_node->name);
	*pinstance_number = 1;
		
	return FAULT_CODE_OK;
}


/*
 * InternetGatewayDevice.DeviceInfo.X_CMCC_TeleComAccount
 */
int cpe_get_igd_di_X_CMCC_TeleComAccount_Enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char username[40];
	memset(username, 0, sizeof(username));
	get_single_config_attr("TZ_SUPER_USERNAME", username);
	if (strlen(username) == 0) {
		strcpy(buffer, STR_FALSE);
	} else {
		strcpy(buffer, STR_TRUE);
	}
	
    *value = buffer;
    
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_TeleComAccount_Enable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	if (!is_true_value(value)) {
		set_single_config_attr("TZ_SUPER_USERNAME", "");
		set_single_config_attr("TZ_SUPER_PASSWD", "");
	}
	
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_TeleComAccount_Username(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	memset(buffer, 0, sizeof(buffer));
	get_single_config_attr("TZ_SUPER_USERNAME", buffer);
	if (strlen(buffer) == 0) {
		strcpy(buffer, N_A);
	}
	
    *value = buffer;
    
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_TeleComAccount_Username(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	 set_single_config_attr("TZ_SUPER_USERNAME", value);
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_TeleComAccount_Password(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	memset(buffer, 0, sizeof(buffer));
	get_single_config_attr("TZ_SUPER_PASSWD", buffer);
	if (strlen(buffer) == 0) {
		strcpy(buffer, N_A);
	}
	
    *value = buffer;
    
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_TeleComAccount_Password(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    set_single_config_attr("TZ_SUPER_PASSWD", value);
    return FAULT_CODE_OK;
}
/**************************************************************/



int cpe_get_igd_di_X_CMCC_UserInfo_Password(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	cmd_cat(TR069_REG_PASSWD, buffer, sizeof(buffer));
    *value = buffer;
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_UserInfo_Password(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);

	cmd_echo(value, TR069_REG_PASSWD);
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_UserInfo_Status(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_UserInfo_Status(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]", name, value, length);
	
	cmd_echo(value, TR069_REG_STATUS);
	if (strncmp(value, "0", 1) == 0) {
		remove(TR069_REG_PASSWD);
		cmd_touch(TR069_REG_SUCCESS);
	}
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_UserInfo_Limit(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_UserInfo_Limit(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_UserInfo_Times(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_UserInfo_Times(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_UserInfo_Result(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_UserInfo_Result(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]", name, value, length);
	
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_UserInfo_ServiceNum(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_UserInfo_ServiceNum(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_UserInfo_ServiceName(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_UserInfo_ServiceName(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

extern void _walk_parameter_node_tree(parameter_node_t * param, int level);
int cpe_add_igd_X_CMCC_MonitorConfig(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
	if(!param_node)
    {
        return FAULT_CODE_9002;
    }

    static int num=1;

	cwmp_log_info("cpe_add_igd_X_CMCC_Monitor node=%s",param_node->name);
	*pinstance_number = 1;
    
    parameter_node_t *child_param;
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
        cwmp_model_copy_parameter(param_node, &wan1_param, num);

        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }

    *pinstance_number = num++;

	cwmp_log_info("==================== add ");	
    _walk_parameter_node_tree(param_node, 0);
		
	return FAULT_CODE_OK;
}

int cpe_del_igd_X_CMCC_MonitorConfig(cwmp_t * cwmp, parameter_node_t * param_node, int instance_number, callback_register_func_t callback_reg)
{
	if(!param_node)
    {
        return FAULT_CODE_9002;
    }

	cwmp_log_info("cpe_del_igd_X_CMCC_MonitorConfig node=%s ins=%d",param_node->name, instance_number);
	
    parameter_node_t * tmp_param, *tmp_node, *child_param;
    
    child_param = param_node->child;
    if(child_param)
    {
        for(tmp_param=child_param->next_sibling; tmp_param; )
        {
        	int index=atoi(tmp_param->name);
        	if(index>0 && index==instance_number){
	        	cwmp_log_info("free node name=%s", tmp_param->name);
	            tmp_node = tmp_param->next_sibling;
	            cwmp_model_delete_parameter(tmp_param);
	            tmp_param = tmp_node;
	            break;
            }
        }

        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }

    cwmp_log_info("==================== free");	
    _walk_parameter_node_tree(param_node, 0);  // cwmp->root
		
	return FAULT_CODE_OK;
}


int cpe_get_igd_di_X_CMCC_Monitor_Enable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_Monitor_Enable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}


int cpe_get_igd_di_X_CMCC_Monitor_MonitorNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_Monitor_ParaList(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	cwmp_log_info("get cmccm paralist name=%s", name);

	strcpy(ret_info, "cpe_get_igd_di_X_CMCC_Monitor_ParaList");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_Monitor_ParaList(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_Monitor_TimeList(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char ret_info[512];

	strcpy(ret_info, "");
    *value = ret_info;
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_CMCC_Monitor_TimeList(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int  cpe_refresh_igd_ddnsconnection(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{

    int	dhcpindexmax = 1;
	cwmp_refresh_i_parameter(cwmp, param_node, dhcpindexmax);
	cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

	return FAULT_CODE_OK;
}

int cpe_get_igd_ddns_Value_DDNSCfgEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    char cmdbuf[128];
    sprintf(cmdbuf, "uci -q get ddns.myddns_ipv4.enabled");
    read_memory(cmdbuf,param,8);
    util_strip_traling_spaces(param);
    if(0 == strcmp(param,"1")){
        strcpy(param, "true");
    }
    else{
        strcpy(param, "false");
    }
    *value = param;
    return FAULT_CODE_OK;
}
int cpe_get_igd_ddns_Value_DDNSProvider(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    char cmdbuf[128];
    sprintf(cmdbuf, "uci -q get ddns.myddns_ipv4.service_name");
    read_memory(cmdbuf,param,128);
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}
int cpe_get_igd_ddns_Value_DDNSUsername(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmdbuf[128];
    sprintf(cmdbuf, "uci -q get ddns.myddns_ipv4.username");
    read_memory(cmdbuf,param,128);
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}
int cpe_get_igd_ddns_Value_DDNSPassword(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    char cmdbuf[128];
    sprintf(cmdbuf, "uci -q get ddns.myddns_ipv4.password");
    read_memory(cmdbuf,param,128);
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}
int cpe_get_igd_ddns_Value_ServicePort(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    strcpy(param, "7547");
    *value = param;
    return FAULT_CODE_OK;
}
int cpe_get_igd_ddns_Value_DDNSDomainName(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    char cmdbuf[128];
    sprintf(cmdbuf, "uci -q get ddns.myddns_ipv4.domain");
    read_memory(cmdbuf,param,128);
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}
int cpe_get_igd_ddns_Value_DDNSHostName(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    char cmdbuf[128];
    sprintf(cmdbuf, "uci -q get ddns.myddns_ipv4.lookup_host");
    read_memory(cmdbuf,param,128);
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}
//DDNS set
#ifndef REBOOT_DDNS_MODULE
#define REBOOT_DDNS_MODULE "/tmp/.reboot_ddns_module"
#endif

int restart_ddns(void *arg1, void *arg2)
{
    if (cmd_file_exist(REBOOT_DDNS_MODULE)) 
    {   
        system("/etc/init.d/ddns restart >/dev/null 2>&1");
        cmd_rm(REBOOT_DDNS_MODULE);
    }   
}

int cpe_set_igd_ddns_Value_DDNSCfgEnabled(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //cwmp_log_info("name[%s] val[%s] len[%d]",name, value, length);
    char cmdbuf[128];
    char ena = 1;
    if(0 == strcmp(value, "true")){
        ena = 1;
    }
    else if(0 == strcmp(value, "false")){
        ena = 0;
    }
    else{
        return FAULT_CODE_9007;
    }
    sprintf(cmdbuf,"uci -q set ddns.myddns_ipv4.enabled=%d;uci -q commit ddns", ena);
    system(cmdbuf); 
    cmd_touch(REBOOT_DDNS_MODULE);
    callback_reg(cwmp, restart_ddns, NULL, NULL);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ddns_Value_DDNSProvider(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //cwmp_log_info("name[%s] val[%s] len[%d]",name, value, length);
    char cmdbuf[128];
    sprintf(cmdbuf,"uci -q set ddns.myddns_ipv4.service_name=%s;uci -q commit ddns", value);
    system(cmdbuf); 
    cmd_touch(REBOOT_DDNS_MODULE);
    callback_reg(cwmp, restart_ddns, NULL, NULL);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ddns_Value_DDNSUsername(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //cwmp_log_info("name[%s] val[%s] len[%d]",name, value, length);
    char cmdbuf[128];
    sprintf(cmdbuf,"uci -q set ddns.myddns_ipv4.username=%s;uci -q commit ddns", value);
    system(cmdbuf); 
    cmd_touch(REBOOT_DDNS_MODULE);
    callback_reg(cwmp, restart_ddns, NULL, NULL);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ddns_Value_DDNSPassword(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //cwmp_log_info("name[%s] val[%s] len[%d]",name, value, length);
    char cmdbuf[128];
    sprintf(cmdbuf,"uci -q set ddns.myddns_ipv4.password=%s;uci -q commit ddns", value);
    system(cmdbuf); 
    cmd_touch(REBOOT_DDNS_MODULE);
    callback_reg(cwmp, restart_ddns, NULL, NULL);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ddns_Value_ServicePort(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //cwmp_log_info("name[%s] val[%s] len[%d]",name, value, length);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ddns_Value_DDNSDomainName(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //cwmp_log_info("name[%s] val[%s] len[%d]",name, value, length);
    char cmdbuf[128];
    sprintf(cmdbuf,"uci -q set ddns.myddns_ipv4.domain=%s;uci -q commit ddns", value);
    system(cmdbuf); 
    cmd_touch(REBOOT_DDNS_MODULE);
    callback_reg(cwmp, restart_ddns, NULL, NULL);
    return FAULT_CODE_OK;
}
int cpe_set_igd_ddns_Value_DDNSHostName(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    //cwmp_log_info("name[%s] val[%s] len[%d]",name, value, length);
    char cmdbuf[128];
    sprintf(cmdbuf,"uci -q set ddns.myddns_ipv4.lookup_host=%s;uci -q commit ddns", value);
    system(cmdbuf); 
    cmd_touch(REBOOT_DDNS_MODULE);
    callback_reg(cwmp, restart_ddns, NULL, NULL);
    return FAULT_CODE_OK;
}


int cpe_get_igd_di_xcmccAlarm_alarmNumber(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char alarm_info[512];

	strcpy(alarm_info, "no value changed!");
    *value = alarm_info;
    return FAULT_CODE_OK;
}

int cpe_get_igd_WANConnectionNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	static char entry[20];

	strcpy(entry, "1");
    *value = entry;
    
    return FAULT_CODE_OK;
}

int cpe_set_igd_WANConnectionNumberOfEntries(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	cwmp_log_info("set WANConnectionNumberOfEntries name[%s] val[%s] len[%d]",
		name, value, length);
    return FAULT_CODE_OK;
}

int cpe_get_igd_ms_period_en(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    int enable = cwmp_conf_get_int("cwmp:enable");
    if (0 == enable)
    { 
        strcpy(param,"false");
    }
    else{
        strcpy(param,"true");
    }
    *value = param;
    
    return FAULT_CODE_OK;
}

int cpe_set_igd_ms_period_en(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    if(0 == strcmp("true",value))
    {
        cwmp_conf_set("cwmp:enable","1");
    }
    else if(0 == strcmp("false",value))
    {
        cwmp_conf_set("cwmp:enable","0");
    }
    else{
        return FAULT_CODE_9007;
    }
    return FAULT_CODE_OK;
}


int cpe_get_igd_ms_period_intval(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    int seconds = cwmp_conf_get_int("cwmp:interval");
    if (seconds <= 0)
    { 
        seconds = 240;
    }
    sprintf(param,"%d",seconds);
    *value = param;
    
    return FAULT_CODE_OK;
}

int cpe_set_igd_ms_period_intval(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	char cmdbuf[128]={0};
    cwmp_conf_set("cwmp:interval",value);
    sprintf(cmdbuf,"uci -q set tozed.cfg.tr069_PeriodicInformInterval=%s;uci -q commit tozed", value);
    system(cmdbuf); 	
    return FAULT_CODE_OK;
}

int cpe_get_igd_ms_aliasbasedaddressing(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	strcpy(param, "false");
    *value = param;
    
    return FAULT_CODE_OK;
}

#ifndef DEF_PARAMETER_ST
#define DEF_PARAMETER_ST

struct parameter_st
{
    const char * name;
    const char * value;
    unsigned int value_length;

    int	type;
    int	fault_code;

};



struct parameter_list_st
{
    int count;
    int size;
    parameter_t ** parameters;
};

#endif

#ifndef TYPEDEF_PARAMETER_ST
#define TYPEDEF_PARAMETER_ST
typedef struct parameter_st parameter_t;
typedef struct parameter_list_st parameter_list_t;
#endif

int cpe_setattr_igd_ms_connectionrequesturl(cwmp_t * cwmp, 
												const char * name, 
												int noticationChange, 
												int notication, 
												parameter_list_t *accList,
												int accListChange,
												callback_register_func_t callback_reg)
{
	cwmp_log_info("cpe_setattr_igd_ms_connectionrequesturl name=%s notiChg=%d noti=%d accChg=%d ",
		name, noticationChange, notication, accListChange);

	cwmp_log_info("accList cnt=%d size=%d, first type=%d name=%s val=%s",
		accList->count, 
		accList->size, 
		accList->parameters[0]->type,	// ref from XmlNodeType
		accList->parameters[0]->name, 
		accList->parameters[0]->value);
		
    return FAULT_CODE_OK;
}

int cpe_add_igd_wandevice(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
	cwmp_log_info("cpe_add_igd_wandevice node=%s",param_node->name);
	*pinstance_number = 1;
		
	return FAULT_CODE_OK;
}

int cpe_add_igd_WANIPConnection(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
	cwmp_log_info("cpe_add_igd_WANIPConnection node=%s",param_node->name);
	*pinstance_number = 1;
		
	return FAULT_CODE_OK;
}

int cpe_add_igd_WANConnectionDevice(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
	cwmp_log_info("cpe_add_igd_WANConnectionDevice node=%s",param_node->name);
	*pinstance_number = 1;
		
	return FAULT_CODE_OK;
}

int cpe_get_EnableCWMP(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	get_single_config_attr("uci get tozed.cfg.tr069_app_enable 2>/dev/null", param);
    *value = param;
    
    return FAULT_CODE_OK;
}

int cpe_set_EnableCWMP(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	if(!strcmp(value, "true"))
		uci_set_single_config_attr("uci set tozed.cfg.tr069_app_enable=y 2>/dev/null&&uci commit tozed", "y");
	else
		uci_set_single_config_attr("uci set tozed.cfg.tr069_app_enable=n 2>/dev/null&&uci commit tozed", "n");
 	
    return FAULT_CODE_OK;
}

int cpe_get_SupportedConnReqMethods(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	strcpy(param, "HTTP");
    *value = param;
    
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_Antenna1(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	strcpy(param, cmd_file_exist("/proc/external_main") ? "External" : "Built-in");
    *value = param;
    
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_CMCC_Antenna2(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	strcpy(param, cmd_file_exist("/proc/external_assist") ? "External" : "Built-in");
    *value = param;
    
    return FAULT_CODE_OK;
}

int cpe_get_igd_di_X_SearchNetMode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	
	strcpy(param, cmd_file_exist(LTE_DISCONNECT_FROM_NETWORK_IMMEDIATELY) ? "Unconnected" : "Connected");
    *value = param;
    
    return FAULT_CODE_OK;
}

int cpe_set_igd_di_X_SearchNetMode(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	if(strcmp(value, "true") == 0) {
		cmd_rm( LTE_DISCONNECT_FROM_NETWORK_IMMEDIATELY );
		cmd_touch( LTE_DISCONNECT_FROM_NETWORK_IMMEDIATELY);
		cmd_rm(LTE_DISCONNECT_FROM_NETWORK_IMMEDIATELY);
	}
	return FAULT_CODE_OK;
}


int cpe_get_igd_di_X_RoamingStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
	get_single_config_attr("TZ_FORBID_ROAMING", param);
	if (strcmp(param, "yes") == 0) {
		strcpy(param, "Disabled");
	} else {
		strcpy(param, "Enabled");
	}
    *value = param;
    
    return FAULT_CODE_OK;
}


int cpe_set_igd_di_X_RoamingStatus(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	if(strcmp(value, "true") == 0) {
		set_single_config_attr("TZ_FORBID_ROAMING", "no");
	} else if (strcmp(value, "false") == 0) {
		set_single_config_attr("TZ_FORBID_ROAMING", "yes");
	}
	
	return FAULT_CODE_OK;
}

int cpe_get_NetworkPriority(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char *mode = NULL, *p, *bit;
	char lteModuleModes[1024], lteModuleModes_bak[1024];
	char module_mode[64];

	memset(lteModuleModes, 0, sizeof(lteModuleModes));
	if (util_get_supported_network_mode_for_this_module(lteModuleModes) == -1) {
		lteModuleModes[0] = '\0';
	}
	strcpy(lteModuleModes_bak, lteModuleModes);

	get_single_config_attr("TZ_LTE_MODULE_MODE", module_mode);
	if(strlen(module_mode) == 0)
	{
		sprintf(param, "unknown [%s]", lteModuleModes_bak);
		*value = param;

		return FAULT_CODE_OK;
	}
	p = lteModuleModes;
	while(mode = strsep(&p, "$"))
	{
		bit = strsep(&mode, "@");
		if(bit && atoi(bit) == atoi(module_mode))
		{
			sprintf(param, "%s [%s]", mode, lteModuleModes_bak);
			*value = param;

			return FAULT_CODE_OK;
		}
	}

	sprintf(param, "unknown [%s]", lteModuleModes_bak);
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_set_NetworkPriority(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	char *mode = NULL, *p, *bit;
	char lteModuleModes[1024];
	char module_mode[64];

	if(is_digit(value) < 0)
		return FAULT_CODE_9003;

	memset(lteModuleModes, 0, sizeof(lteModuleModes));
	if (util_get_supported_network_mode_for_this_module(lteModuleModes) == -1) {
		lteModuleModes[0] = '\0';
	}

	p = lteModuleModes;
	while(mode = strsep(&p, "$"))
	{
		bit = strsep(&mode, "@");
		if(bit && atoi(bit) == atoi(value))
		{
			set_single_config_attr("TZ_LTE_MODULE_MODE", value);
			return FAULT_CODE_OK;
		}
	}

	return FAULT_CODE_9003;
}

#ifdef  PERIODREADDEVICE
extern float uplink, downlink, uplink_min, uplink_max, downlink_min, downlink_max;
extern pthread_mutex_t refresh_linkrate_lock;
#endif

void dataRateUnitConvert(char *rate_readable, float rate_bytes)
{
	int i;
	float rate;

	for(i = 1; i <= 3; i++)
	{
		rate = rate_bytes / 1024;
		if(rate < 1)
			break;
		rate_bytes = rate;
	}
	if(i == 1)
		sprintf(rate_readable, "%.3f Bps", rate_bytes);
	else if(i == 2)
		sprintf(rate_readable, "%.3f KBps", rate_bytes);
	else if(i == 3)
		sprintf(rate_readable, "%.3f MBps", rate_bytes);
}

int cpe_get_UplinkRate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	float up = 0;
#ifdef  PERIODREADDEVICE
	if (pthread_mutex_trylock(&refresh_linkrate_lock) == 0) {
		up = uplink;
		pthread_mutex_unlock(&refresh_linkrate_lock);
	}
#endif
	dataRateUnitConvert(param, up);
	*value = param;

	return FAULT_CODE_OK;	
}

int cpe_get_DownlinkRate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	float down = 0;
#ifdef  PERIODREADDEVICE
	if (pthread_mutex_trylock(&refresh_linkrate_lock) == 0) {
		down = downlink;
		pthread_mutex_unlock(&refresh_linkrate_lock);
	}
#endif
	dataRateUnitConvert(param, down);
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_MaxUplinkRate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	float max = 0;
#ifdef  PERIODREADDEVICE
	if (pthread_mutex_trylock(&refresh_linkrate_lock) == 0) {
		max = uplink_max;
		pthread_mutex_unlock(&refresh_linkrate_lock);
	}
#endif
	dataRateUnitConvert(param, max);
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_MaxDownlinkRate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	float max = 0;
#ifdef  PERIODREADDEVICE
	if (pthread_mutex_trylock(&refresh_linkrate_lock) == 0) {
		max = downlink_max;
		pthread_mutex_unlock(&refresh_linkrate_lock);
	}
#endif
	dataRateUnitConvert(param, max);
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_USIMCardStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	if(cmd_sim_exist())
		strcpy(param, "Present");
	else
		strcpy(param, "Absent");
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_IPv4Status(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	get_single_config_attr("TZ_IP_STACK_MODE", param);
	if(!strcmp(param, "IP") || !strcmp(param, "IPV4V6"))
		strcpy(param, "Enabled");
	else
		strcpy(param, "Disabled");
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_set_IPv4Status(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	if(value == NULL)
		return FAULT_CODE_9003;
	if(!strcmp(value, "true"))
		set_single_config_attr("TZ_IP_STACK_MODE", "IP");

	return FAULT_CODE_OK;
}

int cpe_get_IPv6Status(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	get_single_config_attr("TZ_IP_STACK_MODE", param);
	if(!strcmp(param, "IPV6") || !strcmp(param, "IPV4V6"))
		strcpy(param, "Enabled");
	else
		strcpy(param, "Disabled");
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_set_IPv6Status(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
	if(value == NULL)
		return FAULT_CODE_9003;
	if(!strcmp(value, "true"))
		set_single_config_attr("TZ_IP_STACK_MODE", "IPV6");

	return FAULT_CODE_OK;
}

int cpe_get_CommonStatus_BitErrorRate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	float n;

	srand(time(NULL));
	n = rand() % 10 * 0.1;
	sprintf(param, "%.2f%%", n);
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_CommonStatus_Resync(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "true");
	*value = param;

	return FAULT_CODE_OK;
}






int cpe_get_Duration(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	cmd_cat("/tmp/.online_time", param, sizeof(param));

	util_strip_traling_spaces(param);
	if(strlen(param) == 0) {
		strcpy(param, "0");
	}
	*value = param;

	return FAULT_CODE_OK;
}









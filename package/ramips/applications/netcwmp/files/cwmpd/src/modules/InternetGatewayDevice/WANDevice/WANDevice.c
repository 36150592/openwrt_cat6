#define WANDEVICE_INTERFACE_NAME         "eth0.2"

extern int cwmp_model_delete_parameter(parameter_node_t * param);

void walk_parameter_node_tree_cpe_refresh_igd_wandevice(parameter_node_t * param, int level)
{
  if(!param) return; 
 
  parameter_node_t * child;
  char fmt[128];
  //cwmp_log_debug("name: %s, type: %s, level: %d\n", param->name, cwmp_get_type_string(param->type), level);
  
  // lizd	
  sprintf(fmt, "|%%-%ds%%s ", level*4);
  cwmp_log_debug(fmt, "----", param->name );

  child = param->child;

  if(!child)
	return;
  walk_parameter_node_tree_cpe_refresh_igd_wandevice(child, level+1);

  parameter_node_t * next = child->next_sibling;

  while(next)
 {
    walk_parameter_node_tree_cpe_refresh_igd_wandevice(next, level+1);
    next = next->next_sibling;
 }

	
}


int  cpe_refresh_igd_wandevice(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, 1);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);
    
    return FAULT_CODE_OK;
}
    
int cpe_set_EnableOfRemoteAccess(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    if(!strcmp(value, "true")) 
        set_single_config_attr("TZ_ALLOW_LOGIN_FROM_WAN", "yes");
    else
        set_single_config_attr("TZ_ALLOW_LOGIN_FROM_WAN", "no");

    return FAULT_CODE_OK;
}

int cpe_get_EnableOfRemoteAccess(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    get_single_config_attr("TZ_ALLOW_LOGIN_FROM_WAN", param);
    if(!strcmp(param, "yes"))
        strcpy(param, "true");
    else
        strcpy(param, "false");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_SupportedProtocolsOfRemoteAccess(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "HTTP,HTTPS");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_ProtocolOfRemoteAccess(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    if(glb_vars.https_enable)
        strcpy(param, "HTTPS");
    else
        strcpy(param, "HTTP");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_idg_EnableofWANCommonInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, getNetStat(WANDEVICE_INTERFACE_NAME)? "true" : "false");
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_set_idg_EnableofWANCommonInterfaceConfig(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    int boolean = 0;
	if (strcmp("true",value) == 0) {
		boolean = 1;
	} else if (strcmp("false",value) == 0) {
		boolean = 0;
	} else {
		return -1;
	}
    sprintf(param, "ifconfig %s %s", WANDEVICE_INTERFACE_NAME, boolean ? "up" : "down");
    system(param);
	return FAULT_CODE_OK;
}

int cpe_get_WANAccessType(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "Ethernet");
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_idg_MACAddressofWANEthernetInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	sprintf(buff,"ifconfig %s | grep \"HWaddr\" | awk \'{print $5}\'", WANDEVICE_INTERFACE_NAME);
	read_memory(buff,param,sizeof(param));
	if (strlen(param) == 0) {
		strcpy(param,"00:00:00:00:00:00");
	}
	util_strip_traling_spaces(param);
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_idg_MaxBitRateofWANEthernetInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"100");
	*value = param;
    return FAULT_CODE_OK;
}

int cpe_get_idg_DuplexModeofWANEthernetInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"Full");
	*value = param;
    return FAULT_CODE_OK;
}

int cpe_get_idg_StatusofWANEthernetInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int ret = getNetStat(WANDEVICE_INTERFACE_NAME);
	switch (ret) {
		case 0:
			strcpy(param,"Error");
			break;
		case 1:
			strcpy(param,"NoLink");
			break;
		case 2:
			strcpy(param,"Up");
			break;
		default:
			strcpy(param,"Error");
			break;
	}
	*value = param;
	return FAULT_CODE_OK;

}
int cpe_get_idg_TotalBytesSentofWANCommonInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	/*
	get_single_config_attr("TZ_DISABLE_STATISTICS", param);
	if (strncmp(param, "yes", 3) == 0) {//No traffic statistics are enabled		
		*value = NULL;
		return FAULT_CODE_OK;
	}*/
	
	*value = tx_bytes_counter;
    return FAULT_CODE_OK;
}

int cpe_get_idg_TotalBytesReceivedofWANCommonInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	/*
	get_single_config_attr("TZ_DISABLE_STATISTICS", param);
	if (strncmp(param, "yes", 3) == 0) {//No traffic statistics are enabled		
		*value = NULL;
		return FAULT_CODE_OK;
	}*/
	
	*value = rx_bytes_counter;
    return FAULT_CODE_OK;
}



int cpe_get_idg_BytesSentofWANEthernetInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	sprintf(buff, "ifconfig %s | grep \"TX bytes\" | awk \'{print $6}\'| sed \'s/bytes://g\'", WANDEVICE_INTERFACE_NAME);
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_idg_BytesReceivedofWANEthernetInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	sprintf(buff, "ifconfig %s | grep \"RX bytes\" | awk \'{print $2}\'| sed \'s/bytes://g\'", WANDEVICE_INTERFACE_NAME);
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_idg_PacketsReceivedofWANEthernetInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	/*
	get_single_config_attr("TZ_DISABLE_STATISTICS", param);
	if (strncmp(param, "yes", 3) == 0) {//No traffic statistics are enabled		
		*value = NULL;
		return FAULT_CODE_OK;
	}*/
	
	*value = rx_packages_counter;
	return FAULT_CODE_OK;
}

int cpe_get_idg_PacketsSentofWANEthernetInterfaceConfig(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	/*
	get_single_config_attr("TZ_DISABLE_STATISTICS", param);
	if (strncmp(param, "yes", 3) == 0) {//No traffic statistics are enabled		
		*value = NULL;
		return FAULT_CODE_OK;
	}*/
	
	*value = tx_packages_counter;
	return FAULT_CODE_OK;
}

int cpe_get_idg_MaximumActiveConnections(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"1");
	*value = param;
    return FAULT_CODE_OK;
}

int cpe_get_idg_NumberOfActiveConnections(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"1");
	*value = param;
    return FAULT_CODE_OK;
}

int  cpe_refresh_ConnectionOfWANCommonInterface(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, 1);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int cpe_get_ActiveConnectionDeviceContainer(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.");
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_ActiveConnectionServiceID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.");
	*value = param;
	
    return FAULT_CODE_OK;
}

int cpe_get_idg_PhysicalLinkStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int ret = getNetStat(WANDEVICE_INTERFACE_NAME);
	switch (ret) {
		case 0:
			strcpy(param,"Down");
			break;
		case 1:
			strcpy(param,"Initializing");
			break;
		case 2:
			strcpy(param,"Up");
			break;
		default:
			strcpy(param,"Unavailable");
			break;
	}
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_idg_Layer1UpstreamMaxBitRate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"100000000");
	*value = param;
    return FAULT_CODE_OK;
}

int cpe_get_idg_Layer1DownstreamMaxBitRate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"100000000");
	*value = param;
    return FAULT_CODE_OK;
}

int  cpe_refresh_X_TOZED_APN_PROFILE(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
	igd_entries.apn_entry = 1;
    cwmp_refresh_i_parameter(cwmp, param_node, igd_entries.apn_entry);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}


static int uci_apn_get_wifi_param(const char * name, const char * option, char ** value)
{
    int index;
    int num;
    char cmdbuf[64] = {0};
    num = 4;
    printf("jiangyibo %s\n",name);
    index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", 3);
    
    if (0 < index && index < num)
    {
        
        sprintf(cmdbuf,"uci -q get tozed.modem.%s",option);
        printf("jiangyibo 88 %s\n",cmdbuf);
        read_memory(cmdbuf, param, sizeof(param));
        printf("jiangyibo 88 %s\n",param);
        util_strip_traling_spaces(param);
        *value = param;
    }
    else
    {   
        return FAULT_CODE_9005;
    }
    printf("jiangyibo %d\n",index);
    return FAULT_CODE_OK;
}

static int uci_mul_apn_get_wifi_param(const char * name, const char * option, char ** value)
{
    int index;
    int num;
    char cmdbuf[64] = {0};
		char tempcmdbuf[64] = {0};
    num = 4;
    printf("jiangyibo %s\n",name);
    index = get_parameter_index((char *)name, "APN.", 3);
    
    if (0 < index && index < num)
    {
        
        sprintf(cmdbuf,"uci -q get tozed.modem.%s",option);
        printf("jiangyibo 88 %s\n",cmdbuf);
        read_memory(cmdbuf, param, sizeof(param));
        printf("jiangyibo 88 %s\n",param);
        util_strip_traling_spaces(param);
        *value = param;
    }
    else
    {   
        return FAULT_CODE_9005;
    }
    printf("jiangyibo %d\n",index);
    return FAULT_CODE_OK;
}

static int uci_apn_set_wifi_param(const char * name, const char * option, char * value)
{
    int index;
    int num;
    char cmdbuf[64] = {0};
	char tempcmdbuf[64] = {0};
    num = 4;

    printf("jiangyibo set apn %s %s\n",option,value);
    index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", 3);
	if(option==NULL||value==NULL)
	{
		return FAULT_CODE_OK;
	}
    
    if (0 < index && index < num)
    {
      
        sprintf(cmdbuf,"uci -q set tozed.modem.%s=%s&&uci -q commit tozed",option,value);
        read_memory(cmdbuf, tempcmdbuf, sizeof(tempcmdbuf));
    }
    else
    {   
        return FAULT_CODE_9005;
    }
    return FAULT_CODE_OK;
}


static int uci_mul_apn_set_wifi_param(const char * name, const char * option, char * value)
{
    int index;
    int num;
    char cmdbuf[64] = {0};
	char tempcmdbuf[64] = {0};
    num = 4;
    printf("jiangyibo %s\n",name);
    index = get_parameter_index((char *)name, "APN.", 3);
	if(option==NULL||value==NULL)
	{
		return FAULT_CODE_OK;
	}
    
    if (0 < index && index < num)
    {
      
        sprintf(cmdbuf,"uci -q set tozed.modem.%s=%s&&uci -q commit tozed",option,value);
        read_memory(cmdbuf, tempcmdbuf, sizeof(tempcmdbuf));
    }
    else
    {   
        return FAULT_CODE_9005;
    }
    return FAULT_CODE_OK;
}



int cpe_get_CurrentProfileOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index;
	index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", 3);
    
    if (0 < index && index <= 3)
    {
           if(1 == index){
				strcpy(param, "main_apn");
			}
			else if(2 == index){
				strcpy(param, "primary_apn");
			}
						else if(3 == index){
				strcpy(param, "primary_apn1");
			}
			else {
				strcpy(param, "");
			}
	}
	*value = param;
    return FAULT_CODE_OK;

}
/*
static int uci_apn_get_wifi_param(const char * name, const char * option, char ** value)
{
    int index;
    int num;
    char cmdbuf[64] = {0};
    num = 4;
    printf("jiangyibo %s\n",name);
    index = get_parameter_index((char *)name, "MultiSSID.", 65535);
    
    if (0 < index && index <= num)
    {
        
        sprintf(cmdbuf,"uci -q get mutilssid.@wifi-iface[%d].%s",index - 1,option);
        printf("jiangyibo 88 %s\n",cmdbuf);
        read_memory(cmdbuf, param, sizeof(param));
        printf("jiangyibo 88 %s\n",param);
        util_strip_traling_spaces(param);
        *value = param;
    }
    else
    {   
        return FAULT_CODE_9005;
    }
    printf("jiangyibo %d\n",index);
    return FAULT_CODE_OK;
}
*/

int cpe_set_CurrentProfileOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	unsigned int apn_index;
	char config[64];

	if(value == NULL)
		return FAULT_CODE_OK;
	return FAULT_CODE_OK;
}

int cpe_get_ProfileNameOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{  
	int res ;
    strcpy(param, "mainapn");
    *value = param;

    return FAULT_CODE_OK;   

}

int cpe_set_ProfileNameOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	return FAULT_CODE_OK;
}

int cpe_get_APNNameOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{

    int res ;
	int   index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", 3);
	if(index == 1)
	{
		res = uci_apn_get_wifi_param(name, "TZ_DIALTOOL2_APN_NAME",value);
	}else if(index == 2)
	{
		res = uci_apn_get_wifi_param(name, "TZ_DIALTOOL2_APN_NAME",value);
	}
	else{
		res = uci_apn_get_wifi_param(name, "TZ_DIALTOOL2_APN_NAME",value);
	}

    if(FAULT_CODE_OK != res){
        return res;
    }
    return FAULT_CODE_OK;
}

int cpe_set_APNNameOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	char config[64];
	int res = -1;
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    if(index == 1)
	{
		res = uci_apn_set_wifi_param(name, "TZ_DIALTOOL2_APN_NAME",value);
	}
	else{
		res = uci_apn_set_wifi_param(name, "TZ_DIALTOOL2_APN_NAME",value);
	}    
    if(FAULT_CODE_OK != res){
        return res;
    }

	return FAULT_CODE_OK;
}

int cpe_get_PPPUserNameOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{

	   int res ;
	int   index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", 3);
	if(index == 1)
	{
		res = uci_apn_get_wifi_param(name, "TZ_DIALTOOL2_PPP_USERNAME",value);
	}else if(index == 2)
	{
		res = uci_apn_get_wifi_param(name, "TZ_MUTILAPN1_APN_NAME",value);
	}
	else{
		res = uci_apn_get_wifi_param(name, "TZ_MUTILAPN2_APN_NAME",value);
	}
    if(FAULT_CODE_OK != res){
        return res;
    }
    return FAULT_CODE_OK;
}

int cpe_set_PPPUserNameOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	char config[64];
	int res = -1;
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    if(index == 1)
	{
		res = uci_apn_set_wifi_param(name, "TZ_DIALTOOL2_PPP_USERNAME",value);
	}
	else{
		res = uci_apn_set_wifi_param(name, "TZ_DIALTOOL2_PPP_USERNAME",value);
	}    
    if(FAULT_CODE_OK != res){
        return res;
    }

	return FAULT_CODE_OK;
}

int cpe_get_PPPPasswdOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{

	   int res ;
	int   index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", 3);
	if(index == 1)
	{
		res = uci_apn_get_wifi_param(name, "TZ_DIALTOOL2_PPP_PASSWORD",value);
	}else if(index == 1)
	{
		res = uci_apn_get_wifi_param(name, "TZ_MUTILAPN1_PASSWORD",value);
	}
	else{
		res = uci_apn_get_wifi_param(name, "TZ_MUTILAPN2_PASSWORD",value);
	}	
    if(FAULT_CODE_OK != res){
        return res;
    }
    return FAULT_CODE_OK;
}

int cpe_set_PPPPasswdOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	char config[64];
	int res = -1;
	printf("jiangyibo set %s\n",name);
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    if(index == 1)
	{
		res = uci_apn_set_wifi_param(name, "TZ_DIALTOOL2_PPP_PASSWORD",value);
	}
	else{
		res = uci_apn_set_wifi_param(name, "TZ_DIALTOOL2_PPP_PASSWORD",value);
	}    
    if(FAULT_CODE_OK != res){
        return res;
    }

	return FAULT_CODE_OK;
}

int cpe_get_AuthTypeOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{

		   int res ;
	int   index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", 3);
	if(index == 1)
	{
		res = uci_apn_get_wifi_param(name, "TZ_DIALTOOL2_PPP_AUTH_TYPE",value);
	}else if(index == 2)
	{
		res = uci_apn_get_wifi_param(name, "TZ_MUTILAPN1_MTU",value);
	}
	else{
		res = uci_apn_get_wifi_param(name, "TZ_MUTILAPN2_MTU",value);
	}

    if(FAULT_CODE_OK != res){
        return res;
    }
    return FAULT_CODE_OK;
}

int cpe_set_AuthTypeOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	char config[64];
	int res = -1;
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    if(index == 1)
	{
		res = uci_apn_set_wifi_param(name, "TZ_DIALTOOL2_PPP_AUTH_TYPE",value);
	}
	else{
		res = uci_apn_set_wifi_param(name, "TZ_DIALTOOL2_PPP_AUTH_TYPE",value);
	}    
    if(FAULT_CODE_OK != res){
        return res;
    }

    return FAULT_CODE_OK;

}

int cpe_get_PDPTypeOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{

		   int res ;
	int   index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", 3);
	if(index == 1)
	{
		res = uci_apn_get_wifi_param(name, "TZ_DIALTOOL2_IP_STACK_MODE",value);
	}else if(index == 2)
	{
		res = uci_apn_get_wifi_param(name, "TZ_DIALTOOL2_IP_STACK_MODE",value);
	}
	else{
		res = uci_apn_get_wifi_param(name, "TZ_DIALTOOL2_IP_STACK_MODE",value);
	}

    if(FAULT_CODE_OK != res){
        return res;
    }
    return FAULT_CODE_OK;
}

int cpe_set_PDPTypeOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	char config[64];
	int res = -1;
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    if(index == 1)
	{
		res = uci_apn_set_wifi_param(name, "TZ_DIALTOOL2_IP_STACK_MODE",value);
	}
	else{
		res = uci_apn_set_wifi_param(name, "TZ_DIALTOOL2_IP_STACK_MODE",value);
	}    
    if(FAULT_CODE_OK != res){
        return res;
    }

	return FAULT_CODE_OK;
}



#define WANDEVICE_INTERFACE_NAME         "usb0"

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
    cwmp_refresh_i_parameter(cwmp, param_node, igd_entries.apn_entry);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int cpe_get_CurrentProfileOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	get_single_config_attr("TZ_APN_PROFILE_NAME", param);
	if(strlen(param) == 0)
		*value = NULL;
	else
		*value = param;

	return FAULT_CODE_OK;
}

int cpe_set_CurrentProfileOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	unsigned int apn_index;
	char config[64];

	if(value == NULL)
		return FAULT_CODE_OK;
	apn_index = atoi(value);
	if(apn_index < 1)
		apn_index = 1;
	if(apn_index > igd_entries.apn_entry)
		apn_index = igd_entries.apn_entry;

	sprintf(param, "%d", apn_index);
	set_single_config_attr("TZ_APN_PROFILE_NAME", param);

	sprintf(config, "TZ_CONFIG_APN_NAME%d", apn_index);
	get_single_config_attr(config, param);
	set_single_config_attr("TZ_CONFIG_APN_NAME", param);

	sprintf(config, "TZ_PPP_USERNAME%d", apn_index);
	get_single_config_attr(config, param);
	set_single_config_attr("TZ_PPP_USERNAME", param);

	sprintf(config, "TZ_PPP_PASSWORD%d", apn_index);
	get_single_config_attr(config, param);
	set_single_config_attr("TZ_PPP_PASSWORD", param);

	sprintf(config, "TZ_PPP_AUTH_TYPE%d", apn_index);
	get_single_config_attr(config, param);
	set_single_config_attr("TZ_PPP_AUTH_TYPE", param);

	return FAULT_CODE_OK;
}

int cpe_get_ProfileNameOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char config[64];
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    sprintf(config, "TZ_APN_PROFILE_NAME%d", index);
	get_single_config_attr(config, param);
	if(strlen(param) == 0)
		*value = NULL;
	else
		*value = param;

	return FAULT_CODE_OK;
}

int cpe_set_ProfileNameOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	char config[64];
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    sprintf(config, "TZ_APN_PROFILE_NAME%d", index);
	set_single_config_attr(config, value);

	return FAULT_CODE_OK;
}

int cpe_get_APNNameOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char config[64];
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    sprintf(config, "TZ_CONFIG_APN_NAME%d", index);
	get_single_config_attr(config, param);
	if(strlen(param) == 0)
		*value = NULL;
	else
		*value = param;

	return FAULT_CODE_OK;
}

int cpe_set_APNNameOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	char config[64];
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    sprintf(config, "TZ_CONFIG_APN_NAME%d", index);
	set_single_config_attr(config, value);	

	return FAULT_CODE_OK;
}

int cpe_get_PPPUserNameOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char config[64];
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    sprintf(config, "TZ_PPP_USERNAME%d", index);
	get_single_config_attr(config, param);
	if(strlen(param) == 0)
		*value = NULL;
	else
		*value = param;

	return FAULT_CODE_OK;
}

int cpe_set_PPPUserNameOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	char config[64];
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    sprintf(config, "TZ_PPP_USERNAME%d", index);
	set_single_config_attr(config, value);	
	
	return FAULT_CODE_OK;
}

int cpe_get_PPPPasswdOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char config[64];
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    sprintf(config, "TZ_PPP_PASSWORD%d", index);
	get_single_config_attr(config, param);
	if(strlen(param) == 0)
		*value = NULL;
	else
		*value = param;

	return FAULT_CODE_OK;
}

int cpe_set_PPPPasswdOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	char config[64];
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    sprintf(config, "TZ_PPP_PASSWORD%d", index);
	set_single_config_attr(config, value);	
	
	return FAULT_CODE_OK;
}

int cpe_get_AuthTypeOfAPN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char config[64];
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    sprintf(config, "TZ_PPP_AUTH_TYPE%d", index);
	get_single_config_attr(config, param);
	if(strlen(param) == 0)
		*value = NULL;
	else
		*value = param;

	return FAULT_CODE_OK;
}

int cpe_set_AuthTypeOfAPN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	char config[64];
    int index = get_parameter_index((char *)name, "X_TGT_APN_PROFILE.", igd_entries.apn_entry);
    sprintf(config, "TZ_PPP_AUTH_TYPE%d", index);
	set_single_config_attr(config, value);	
	
	return FAULT_CODE_OK;
}





extern WanInfo wan_info;

#ifndef REBOOT_NETWORK_MODULE
#define REBOOT_NETWORK_MODULE "/tmp/.reboot_network_module"
#endif

int restart_network(void *arg1, void *arg2)
{
    if (cmd_file_exist(REBOOT_NETWORK_MODULE)) 
    {
        system("/etc/init.d/network restart >/dev/null 2>&1");
        cmd_rm(REBOOT_NETWORK_MODULE);
    }
}
int cpe_get_igd_wan_WANPPPConnectionNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    if(0 == strcmp(wan_info.AddressingType,"PPPoE")){ //is PPPoE
        strcpy(param,"1");
    }
    else{
        strcpy(param,"0");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_refresh_igd_WANPPPConnection(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    read_wan_info();
    int inum = 0;
    if(0 == strcmp(wan_info.AddressingType,"PPPoE")){
        inum = 1;
    }
    cwmp_refresh_i_parameter(cwmp, param_node, inum);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_WANPPPConnectionEnable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = wan_info.Enable;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_ConnectionStatus(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    if(0 == strcmp(wan_info.Enable,"true")){
        strcpy(param,"Connected");
    }
    else{
        strcpy(param,"Unconfigured");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_PossibleConnectionTypes(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param,"Unconfigured,IP_Routed");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_ConnectionType(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    if(0 == strcmp(wan_info.Enable,"true")){
        strcpy(param,"IP_Routed");
    }
    else{
        strcpy(param,"Unconfigured");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_Name(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = wan_info.Name;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_Uptime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = wan_info.Uptime;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_LastConnectionError(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param,"ERROR_NONE");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_Username(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    memset(param, 0, 64);
    if(0 == strcmp(wan_info.AddressingType,"PPPoE")){ //is PPPoE
        read_memory("uci -q get network.wan.username", param, 64);
        util_strip_traling_spaces(param);
    }
    *value = param;
    return FAULT_CODE_OK;
}
int cpe_set_igd_wan_ppp_Username(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char proto[64];
    char curname[64];
    read_memory("uci -q get network.wan.proto", proto, 64);
    util_strip_traling_spaces(proto);
    read_memory("uci -q get network.wan.username", curname, 64);
    util_strip_traling_spaces(curname);
    if(0 == strcmp(proto,"pppoe") && 0 != strcmp(curname, value))
    {
        char cmdbuf[256];
        sprintf(cmdbuf,"uci -q set network.wan.username=%s;uci -q commit network", value);
        system(cmdbuf);
        cmd_touch(REBOOT_NETWORK_MODULE);
        callback_reg(cwmp, restart_network, NULL, NULL);
    }
    return FAULT_CODE_OK;
}
int cpe_get_igd_wan_ppp_Password(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    memset(param, 0, 64);
    if(0 == strcmp(wan_info.AddressingType,"PPPoE")){ //is PPPoE
        read_memory("uci -q get network.wan.password", param, 64);
        util_strip_traling_spaces(param);
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_wan_ppp_Password(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    char proto[64];
    char curpwd[64];
    read_memory("uci -q get network.wan.proto", proto, 64);
    util_strip_traling_spaces(proto);
    read_memory("uci -q get network.wan.password", curpwd, 64);
    util_strip_traling_spaces(curpwd);
    if(0 == strcmp(proto,"pppoe") && 0 != strcmp(curpwd, value))
    {
        char cmdbuf[256];
        sprintf(cmdbuf,"uci -q set network.wan.password=%s;uci -q commit network", value);
        system(cmdbuf);
        cmd_touch(REBOOT_NETWORK_MODULE);
        callback_reg(cwmp, restart_network, NULL, NULL);
    }
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_ExternalIPAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = wan_info.ExternalIPAddress;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_DefaultGateway(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = wan_info.DefaultGateway;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_DNSEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = wan_info.DNSEnabled;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_DNSOverrideAllowed(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param,"false");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_DNSServers(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = wan_info.DNSServers;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_MaxMTUSize(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    if(0 == strcmp(wan_info.Enable,"true")){
        sprintf(param,"%d",wan_info.Mtu);
    }
    else{
        param[0] = 0;
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_MACAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = wan_info.MACAddress;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_MACAddressOverride(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param,"false");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_ConnectionTrigger(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param,"AlwaysOn");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_RouteProtocolRx(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param,"Off");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_EthernetBytesSent(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmdbuf[256];
    sprintf(cmdbuf,"ifconfig %s 2>/dev/null | grep -E 'RX bytes:' | sed 's/bytes:/ /g' | awk '{print $2}'",wan_info.Name);
    read_memory(cmdbuf,param,16);
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ppp_EthernetBytesReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmdbuf[256];
    sprintf(cmdbuf,"ifconfig %s 2>/dev/null | grep -E 'RX bytes:' | sed 's/bytes:/ /g' | awk '{print $6}'",wan_info.Name);
    read_memory(cmdbuf,param,16);
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}

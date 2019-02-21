
extern WanInfo wan_info;

extern void walk_parameter_node_tree_cpe_refresh_igd_wandevice(parameter_node_t * param, int level);

int  cpe_refresh_igd_wanipconnection(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    int inum = 0;
    if(0 != strcmp(wan_info.AddressingType,"PPPoE")){ //not PPPoE
        inum = 1;
    }
    cwmp_refresh_i_parameter(cwmp, param_node, inum);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ip_EthernetBytesSent(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{    
    return get_parameter(X_CMCC_UpstreamTotalByte, value);
}
 
int cpe_get_igd_wan_ip_EthernetBytesReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return get_parameter(X_CMCC_DownstreamTotalByte, value);
}

int cpe_get_igd_wan_ip_EthernetPacketsSent(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return get_parameter(X_CMCC_UpstreamPackages, value);
}

int cpe_get_igd_wan_ip_EthernetPacketsReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return get_parameter(X_CMCC_DownstreamPackges, value);
}
#ifndef REAL_WAN
#define REAL_WAN "eth0.2"
#endif
int cpe_get_igd_wan_ip_EthernetErrorsSent(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmd[128];
    sprintf(cmd, "ifconfig %s | grep \"TX packets\" | awk \'{print $3}\'| sed \'s/errors://g\'", REAL_WAN);
    if (read_memory(cmd, param, sizeof(param)) != 0) {
		strcpy(param, "N/A");
    }
	util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ip_EthernetErrorsReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmd[128];

    sprintf(cmd, "ifconfig %s | grep \"RX packets\" | awk \'{print $3}\'| sed \'s/errors://g\'", REAL_WAN);
    if (read_memory(cmd, param, sizeof(param)) != 0) {
		strcpy(param, "N/A");
    }
	util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ip_EthernetDiscardPacketsSent(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmd[128];

    sprintf(cmd, "cat /proc/net/dev | grep %s | awk -F ':' '{print $2}' | awk '{print $12}'", REAL_WAN);
    if (read_memory(cmd, param, sizeof(param)) != 0) {
        strcpy(param, "N/A");
    }
    util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ip_EthernetDiscardPacketsReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmd[128];

    sprintf(cmd, "cat /proc/net/dev | grep %s | awk -F ':' '{print $2}' | awk '{print $4}'", REAL_WAN);
    if (read_memory(cmd, param, sizeof(param)) != 0) {
        strcpy(param, "N/A");
    }
    util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}


//PortMapping
#ifndef REBOOT_FIREWALL_MODULE
#define REBOOT_FIREWALL_MODULE "/tmp/.reboot_firewall_module"
#endif

int restart_firewall(void *arg1, void *arg2)
{
    if (cmd_file_exist(REBOOT_FIREWALL_MODULE)) 
    {
        system("/etc/init.d/firewall restart >/dev/null 2>&1");
        cmd_rm(REBOOT_FIREWALL_MODULE);
    }
}

static int uci_get_PortMapping(const char * name, const char * option, char ** value)
{
    int index;
    int num;
    char strnum[8] = {0};
    char cmdbuf[64] = {0};
    sprintf(cmdbuf,"uci -q show firewall | grep -E \"]=redirect\" | wc -l");
    read_memory(cmdbuf, strnum, sizeof(strnum));
    util_strip_traling_spaces(strnum);
    num = atoi(strnum);
    if (0 < num)
    {
        index = get_parameter_index((char *)name, "PortMapping.", num);
        sprintf(cmdbuf,"uci -q get firewall.@redirect[%d].%s",index - 1,option);
        read_memory(cmdbuf, param, sizeof(param));
        util_strip_traling_spaces(param);
        *value = param;
    }
    else
    {
        strcpy(param, "N/A");
        *value = param;
    }
    return FAULT_CODE_OK;
}

static int uci_get_PortMapping_num(void)
{
    char strnum[8] = {0};
    char cmdbuf[64] = {0};
    sprintf(cmdbuf,"uci -q show firewall | grep -E \"]=redirect\" | wc -l");
    read_memory(cmdbuf, strnum, sizeof(strnum));
    util_strip_traling_spaces(strnum);
    return atoi(strnum);
}

int cpe_get_igd_wan_ip_PortMappingNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int num = uci_get_PortMapping_num();
    sprintf(param,"%d",num);
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_add_igd_wan_ip_PortMapping(cwmp_t *cwmp, parameter_node_t *param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
    if(!param_node)
    {
        return FAULT_CODE_9002;
    }
    char cmdres[64] = {0};
    char cmdbuf[256] = {0};
    int num = uci_get_PortMapping_num();
    sprintf(cmdbuf, "uci -q add firewall redirect");
    read_memory(cmdbuf, cmdres, sizeof(cmdres));
    util_strip_traling_spaces(cmdres);
    if(0 != strncmp(cmdres,"cfg",3))
    {
        return FAULT_CODE_9002;
    }
    sprintf(cmdbuf,"uci -q set firewall.@redirect[%d].proto=\'tcp udp\';"\
                   "uci -q set firewall.@redirect[%d].target=DNAT;"\
                   "uci -q set firewall.@redirect[%d].src=wan;"\
                   "uci -q set firewall.@redirect[%d].dest=lan;"\
                   "uci -q set firewall.@redirect[%d].name=%s;"\
                   "uci -q commit firewall;"\
                   ,num,num,num,num,num,cmdres);
    system(cmdbuf);
    num++;
    parameter_node_t *portmap_param;
    cwmp_model_copy_parameter(param_node, &portmap_param, num);
    *pinstance_number = num;
    return FAULT_CODE_OK;
}

int cpe_del_igd_wan_ip_PortMapping(cwmp_t *cwmp, parameter_node_t *param_node, int instance_number, callback_register_func_t callback_reg)
{
    if(!param_node)
    {
        return FAULT_CODE_9002;
    }
    int num = uci_get_PortMapping_num();
    char cmdbuf[256] = {0};
    if (0 < num && instance_number <= num)
    {
        sprintf(cmdbuf,"uci -q delete firewall.@redirect[%d];uci -q commit firewall",instance_number-1);
        system(cmdbuf);
    }
    cwmp_model_delete_parameter(param_node);
    return FAULT_CODE_OK;
}

int cpe_refresh_igd_wan_ip_PortMapping(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    int num = uci_get_PortMapping_num();
    cwmp_refresh_i_parameter(cwmp, param_node, num);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}


int cpe_get_igd_wan_ip_PortMapping_Enabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    uci_get_PortMapping(name,"enabled",value);
    if(NULL == *value || 0 == strlen(*value) || 0 != strcmp(*value,"0"))
    {
        strcpy(param,"true");
    }
    else
    {
        strcpy(param,"false");
    }
    *value = param;
    return FAULT_CODE_OK;
}
int cpe_set_igd_wan_ip_PortMapping_Enabled(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    int num = uci_get_PortMapping_num();
    int index = get_parameter_index((char *)name, "PortMapping.", 65535);
    if(index > num)
    {
        return FAULT_CODE_9005;
    }
    int ena = 1;
    if(0 == strcmp(value,"true"))
    {
        ena = 1;
    }
    else if(0 == strcmp(value,"false"))
    {
        ena = 0;
    }
    else
    {
        return FAULT_CODE_9007;
    }
    char cmdbuf[256];
    sprintf(cmdbuf,"uci -q set firewall.@redirect[%d].enabled=%d;uci -q commit firewall",index - 1,ena);
    system(cmdbuf);
    cmd_touch(REBOOT_FIREWALL_MODULE);
    callback_reg(cwmp, restart_firewall, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ip_PortMapping_ExternalPort(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return uci_get_PortMapping(name,"src_dport",value);
}

int cpe_set_igd_wan_ip_PortMapping_ExternalPort(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    int num = uci_get_PortMapping_num();
    int index = get_parameter_index((char *)name, "PortMapping.", 65535);
    if(index > num)
    {
        return FAULT_CODE_9005;
    }
    char cmdbuf[256];
    sprintf(cmdbuf,"uci -q set firewall.@redirect[%d].src_dport=%s;uci -q commit firewall",index - 1,value);
    system(cmdbuf);
    cmd_touch(REBOOT_FIREWALL_MODULE);
    callback_reg(cwmp, restart_firewall, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ip_PortMapping_InternalPort(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return uci_get_PortMapping(name,"dest_port",value);
}

int cpe_set_igd_wan_ip_PortMapping_InternalPort(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    int num = uci_get_PortMapping_num();
    int index = get_parameter_index((char *)name, "PortMapping.", 65535);
    if(index > num)
    {
        return FAULT_CODE_9005;
    }
    char cmdbuf[256];
    sprintf(cmdbuf,"uci -q set firewall.@redirect[%d].dest_port=%s;uci -q commit firewall",index - 1,value);
    system(cmdbuf);
    cmd_touch(REBOOT_FIREWALL_MODULE);
    callback_reg(cwmp, restart_firewall, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ip_PortMapping_InternalClient(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return uci_get_PortMapping(name,"dest_ip",value);
}

int cpe_set_igd_wan_ip_PortMapping_InternalClient(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    int num = uci_get_PortMapping_num();
    int index = get_parameter_index((char *)name, "PortMapping.", 65535);
    if(index > num)
    {
        return FAULT_CODE_9005;
    }
    char cmdbuf[256];
    sprintf(cmdbuf,"uci -q set firewall.@redirect[%d].dest_ip=%s;uci -q commit firewall",index - 1,value);
    system(cmdbuf);
    cmd_touch(REBOOT_FIREWALL_MODULE);
    callback_reg(cwmp, restart_firewall, NULL, NULL);
    return FAULT_CODE_OK;
}


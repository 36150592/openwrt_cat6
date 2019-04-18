#define WIRELESS_INTERFACE_NAME         "ra"
#define WIRELESS_INTERFACE_NAME_5G        "rai"
#define MAX_WIRELESS_CLIENT				40
#define MAX_HOSTS						80
#define MAX_MULTI_SSID_COUNT            4

struct WlanClient
{
	char mac[32];
	char ip[64];
	char rate[5];
};

typedef struct
{
	struct WlanClient client[MAX_WIRELESS_CLIENT];
	int count;
} WlanClientInfo;


struct Hosts
{
	char mac[32];
	char ip[64];
	char host_name[64];
	int leases;
	char addr_src;		//0:DHCP; 1:Static;
	char if_type;		//0:Ethernet; 1:802.11;
	char stat;			//0:inactive; 1:active;
};

typedef struct
{
	struct Hosts host[MAX_HOSTS];
	int count;
} HostsInfo;

WlanClientInfo wlan_client;
HostsInfo host_info;
typedef struct
{
	char Enable[8];
	char UUID[64];
	char DeviceName[64];
	char DeviceMac[64];
}WPSRegistrar;
#define MAXWPSRegistrar 100
ARPTable client;
WlanClientInfo wlan_client;
WPSRegistrar wps_device[MAXWPSRegistrar];

const char wifi_standard[9][2][64] = {
    {"m11ng", "802.11bgn"},
    {"m11b", "802.11b"},
    {"m11g", "802.11g"},
    {"m11bg", "802.11bg"},
    {"m11n", "802.11n"},
    {"m11nSingleFixed", "802.11n fixed rate of single flow"},
    {"m11nSingleFixedMax", "802.11n maximum fixed rate of single flow"},
    {"m11nDoubleFixed", "802.11n fixed rate of double flow"},
    {"m11nDoubleFixedMax", "802.11n maximum fixed rate of double flow"}
};

const char WPAEncryptionModes[3][3][64] = {
    {"TKIP", "TKIPEncryption", "WPA"},
    {"CCMP", "AESEncryption", "11i"},
    {"TKIP CCMP", "TKIPandAESEncryption", "WPAand11i"}
};

int restart_wifi(void *arg1, void *arg2)
{
    if (util_is_should_reboot_wifi_module()) {
        system("/sbin/wifi >/dev/null 2>&1");
        cmd_rm(REBOOT_WIFI_MODULE);
    }
    return 0;
}

#ifndef REBOOT_DNSMASQ_MODULE
#define REBOOT_DNSMASQ_MODULE "/tmp/.reboot_dnsmasq_module"
#endif

int restart_dnsmasq(void *arg1,void *arg2)
{
    if(cmd_file_exist(REBOOT_DNSMASQ_MODULE))
    {
        system("/etc/init.d/dnsmasq restart >/dev/null 2>&1");
        cmd_rm(REBOOT_FIREWALL_MODULE);
    }
}

int getWlanClientInfo(WlanClientInfo *info)
{
	int i, j;
	int count;
	char cmd[128];
	char buffer[128];

	sprintf(cmd, "iwinfo ra0 assoclist|grep SNR > /tmp/.wlan_client_list");
	if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
	{
		info->count = 0;
		return -1;
	}

	sprintf(cmd, "cat /tmp/.wlan_client_list | wc -l");
	if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
	{
		info->count = 0;
		return -1;
	}
	info->count = atoi(buffer) - 1;
	if(info->count <= 0)
	{
		info->count = 0;
		return -1;
	}
	if(info->count > MAX_WIRELESS_CLIENT)
		info->count = MAX_WIRELESS_CLIENT;

	for(i = 1; i <= info->count; i++)
	{
		sprintf(cmd, "awk 'NR==%d {print $1}' /tmp/.wlan_client_list", i + 1);
		if(read_memory(cmd, buffer, sizeof(buffer))  == 0)
		{
			util_strip_traling_spaces(buffer);
			strncpy(info->client[i - 1].mac, buffer, 32);
			memset(info->client[i - 1].ip, 0, 64);
		}
		sprintf(cmd, "awk 'NR==%d {print $2}' /tmp/.wlan_client_list", i + 1);
		if(read_memory(cmd, buffer, sizeof(buffer))  == 0)
		{
			util_strip_traling_spaces(buffer);
			strncpy(info->client[i - 1].rate, buffer, 5);
		}
	}

	sprintf(cmd, "cat /tmp/dhcp.leases  | wc -l");
	if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
		return 0;
	else
		count = atoi(buffer) - 1;
	if(count <= 0)
		return 0;
	for(i = 1; i <= count; i++)
	{
		sprintf(cmd, "awk 'NR==%d {print $2}' /tmp/dhcp.leases", i + 1);
		if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
			continue;
		util_strip_traling_spaces(buffer);
		for(j = 0; j < info->count; j++)
		{
			if(strncmp(info->client[j].mac, buffer, 32) != 0)
				continue;
			sprintf(cmd, "awk 'NR==%d {print $3}' /tmp/.dumpleases_tmp", i + 1);
			if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
				continue;
			util_strip_traling_spaces(buffer);
			strncpy(info->client[j].ip, buffer, 64);
		}
	}
	return 0;
}

int transTimeToSeconds(char *time)
{
        char *hours, *mins, *seconds;
        unsigned int h, m, s;
        if(time == NULL || strlen(time) == 0)
        	return -1;
        hours = strsep(&time, ":");
        if(hours == NULL || time == NULL)
        	return -1;
        mins = strsep(&time, ":");
        if(mins == NULL || time == NULL)
        	return -1;
        seconds = time;
        h = atoi(hours);
        m = atoi(mins);
        s = atoi(seconds);

        return (h * 60 * 60 + m * 60 + s);
}

int get_hosts_info(HostsInfo *info)
{
    int i = 0;
    char ip[32] = {0};
    char mac[32] = {0};
    char name[64] = {0};
    char lt[8] = {0};
    char linebuf[128];
    FILE *fpp = popen("getclientlist -l","r");
    while(fgets(linebuf, sizeof(linebuf), fpp) != NULL)
    {
        sscanf(linebuf, "%s%s%s%s", mac, ip, name, lt);
        strncpy(info->host[i].mac, mac, 32);
        strncpy(info->host[i].ip, ip, 32);
        strncpy(info->host[i].host_name, name, 64);
        info->host[i].leases = atoi(lt);
        info->host[i].addr_src = 0;
        info->host[i].if_type = 0;
        info->host[i].stat = 1;
        i++;
    }
    info->count = i;
    pclose(fpp);
    return i;
}

static int uci_get_wifi_num(void)
{
    char strnum[8] = {0};
    char cmdbuf[64] = {0};
    sprintf(cmdbuf,"uci show wireless | grep -E ']=wifi-iface' | wc -l");
    read_memory(cmdbuf, strnum, sizeof(strnum));
    util_strip_traling_spaces(strnum);
    return atoi(strnum);
}

static int uci_get_wifi_param(const char * name, const char * option, char ** value)
{
    int index;
    int num;
    char cmdbuf[64] = {0};
    num = uci_get_wifi_num();
    index = get_parameter_index((char *)name, "WLANConfiguration.", 65535);
    if (0 < index && index <= num)
    {
        sprintf(cmdbuf,"uci -q get wireless.@wifi-iface[%d].%s",index - 1,option);
        read_memory(cmdbuf, param, sizeof(param));
        util_strip_traling_spaces(param);
        *value = param;
    }
    else
    {   
        return FAULT_CODE_9005;
    }
    return FAULT_CODE_OK;
}

static int uci_mul_get_wifi_num(void)
{
    return 4;
}

static int uci_mul_get_wifi_param(const char * name, const char * option, char ** value)
{
    int index;
    int num;
    char cmdbuf[64] = {0};
    num = uci_mul_get_wifi_num();
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

static int uci_mul_set_wifi_param(const char * name, const char * option, char * value)
{
    int index;
    int num;
    char cmdbuf[128] = {0};
    num = uci_mul_get_wifi_num();
    index = get_parameter_index((char *)name, "MultiSSID.", 65535);
    if (0 < index && index <= num)
    {
        sprintf(cmdbuf,"uci -q set mutilssid.@wifi-iface[%d].%s=%s&&uci -q commit mutilssid",index - 1,option,value);
        
		system(cmdbuf);
        cmd_touch(REBOOT_WIFI_MODULE);
 //       callback_reg(cwmp, restart_wifi, NULL, NULL);
    }
    else
    {   
        return FAULT_CODE_9005;
    }
    return FAULT_CODE_OK;
}

static int uci_mul_get_dhcp_param(const char * name, const char * option, char ** value)
{
    int index;
    int num;
    char cmdbuf[64] = {0};
    num = uci_mul_get_wifi_num();
    index = get_parameter_index((char *)name, "MultiSSID.", 65535);
    if (0 < index && index <= num)
    {
        sprintf(cmdbuf,"uci -q get dhcp.lan%d.%s",(index - 1)%2+1,option);
        printf("jiangyibo 8811 %s\n",cmdbuf);
        read_memory(cmdbuf, param, sizeof(param));
        util_strip_traling_spaces(param);
        *value = param;
    }
    else
    {   
        return FAULT_CODE_9005;
    }
    return FAULT_CODE_OK;
}
static int uci_mul_get_ipaddr_param(const char * name, const char * option, char ** value)
{
    int index;
    int num;
    char cmdbuf[64] = {0};
    num = uci_mul_get_wifi_num();
    index = get_parameter_index((char *)name, "MultiSSID.", 65535);
    if (0 < index && index <= num)
    {
        sprintf(cmdbuf,"uci -q get network.lan%d.%s",(index - 1)%2+1,option);
        read_memory(cmdbuf, param, sizeof(param));
        printf("jiangyibo 2222 %s\n",cmdbuf);
        util_strip_traling_spaces(param);
        *value = param;
    }
    else
    {   
        return FAULT_CODE_9005;
    }
    return FAULT_CODE_OK;
}


static int uci_mul_set_dhcp_param(const char * name, const char * option, char * value)
{
    int index;
    int num;
    char cmdbuf[64] = {0};
    num = uci_mul_get_wifi_num();
    index = get_parameter_index((char *)name, "MultiSSID.", 65535);
    if (0 < index && index <= num)
    {
        sprintf(cmdbuf,"uci -q set dhcp.lan[%d].%s=%s&&uci -q commit dhcp",(index - 1)%2,option,value);
		system(cmdbuf);
        cmd_touch(REBOOT_WIFI_MODULE);
    }
    else
    {   
        return FAULT_CODE_9005;
    }
    return FAULT_CODE_OK;
}


int getHostsInfo(HostsInfo *info)
{
	int i, j, k;
	int count, sum;
	char wifi_mac[32];
	char cmd[128];
	char buffer[128];
	char tmp_file[64] = "/tmp/.hosts_tmp";

	sprintf(cmd, "dumpleases | grep -v 'Mac Address' > %s", tmp_file);
	if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
		count = 0;
	sprintf(cmd, "cat %s | wc -l", tmp_file);
	if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
		count = 0;
	else
		count = atoi(buffer);
	if(count > MAX_HOSTS)
		count = MAX_HOSTS;
	for(i = 1; i <= count; i++)
	{
		sprintf(cmd, "awk 'NR==%d {print $1}' %s", i, tmp_file);
		if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
			strcpy(info->host[i - 1].mac, "");
		else
		{
			util_strip_traling_spaces(buffer);
			strncpy(info->host[i - 1].mac, buffer, 32);
		}

		sprintf(cmd, "awk 'NR==%d {print $2}' %s", i, tmp_file);
		if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
			strcpy(info->host[i - 1].ip, "");
		else
		{
			util_strip_traling_spaces(buffer);
			strncpy(info->host[i - 1].ip, buffer, 64);
		}

		sprintf(cmd, "awk 'NR==%d {print $3}' %s", i, tmp_file);
		if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
			strcpy(info->host[i - 1].host_name, "");
		else
		{
			util_strip_traling_spaces(buffer);
			if(strstr(buffer, "expired"))
			{
				strcpy(info->host[i - 1].host_name, "");
				info->host[i - 1].leases = -1;
			}
			char tmp_buf[128];
			sprintf(cmd, "awk 'NR==%d {print $4}' %s", i, tmp_file);
			read_memory(cmd, tmp_buf, sizeof(tmp_buf));
			util_strip_traling_spaces(tmp_buf);
			if(strlen(tmp_buf) == 0)
			{
				strcpy(info->host[i - 1].host_name, "");
				info->host[i - 1].leases = transTimeToSeconds(buffer);
			}
			else
				strncpy(info->host[i - 1].host_name, buffer, 64);
		}	

		sprintf(cmd, "awk 'NR==%d {print $4}' %s", i, tmp_file);
		if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
		{
			info->host[i - 1].leases = -1;
		}
		else
		{
			// util_strip_traling_spaces(buffer);
			if(strstr(buffer, "expired"))
				info->host[i - 1].leases = -1;
			else
			{
				util_strip_traling_spaces(buffer);
				if(strlen(buffer) != 0)
					info->host[i - 1].leases = transTimeToSeconds(buffer);
			}
		}		
		info->host[i - 1].addr_src = 0;
		info->host[i - 1].if_type = 0;
		info->host[i - 1].stat = 0;
	}
	sum = count;

	sprintf(cmd, "cat /etc/rc.d/rc.dhcp.static | grep -w 'static_lease' > %s", tmp_file);
	if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
		count = 0;
	sprintf(cmd, "cat %s | wc -l", tmp_file);
	if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
		count = 0;
	else
		count = atoi(buffer);
	if(count > MAX_HOSTS - sum)
		count = MAX_HOSTS - sum;
	for(i = 1; i <= count; i++)
	{
		sprintf(cmd, "awk 'NR==%d {print $2}' %s", i, tmp_file);
		if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
			strcpy(info->host[sum + i - 1].mac, "");
		else
		{
			util_strip_traling_spaces(buffer);
			strncpy(info->host[sum + i - 1].mac, buffer, 64);
		}

		sprintf(cmd, "awk 'NR==%d {print $3}' %s", i, tmp_file);
		if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
			strcpy(info->host[sum + i - 1].ip, "");
		else
		{
			util_strip_traling_spaces(buffer);
			strncpy(info->host[sum + i - 1].ip, buffer, 64);
		}	

		info->host[sum + i - 1].addr_src = 1;
		info->host[sum + i - 1].if_type = 0;
		info->host[sum + i - 1].leases = 0;
		info->host[sum + i - 1].stat = 0;
	}
	sum += count;
	
	for(k = 0; k < 3; k++)
	{
		if(k == 0)
			sprintf(cmd, "wlanconfig ath0 list sta > %s", tmp_file);
		else if(k == 1)
			sprintf(cmd, "wlanconfig ath2 list sta > %s", tmp_file);
		else 
			sprintf(cmd, "wlanconfig ath3 list sta > %s", tmp_file);
		if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
			count = 0;
		sprintf(cmd, "cat %s | wc -l", tmp_file);
		if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
			count = 0;
		else
			count = atoi(buffer);
		for(i = 1; i <= count; i++)
		{
			sprintf(cmd, "awk 'NR==%d {print $1}' %s", i, tmp_file);
			if(read_memory(cmd, buffer, sizeof(buffer)) < 0)
				strcpy(wifi_mac, "");
			else
			{
				util_strip_traling_spaces(buffer);
				strncpy(wifi_mac, buffer, 32);
			}
			for(j = 0; j < sum; j++)
			{
				if(!strcmp(wifi_mac, info->host[j].mac))
				{
					info->host[j].if_type = 1;
					info->host[j].stat = 1;
					break;
				}
			}
		}
	}

	ARPTable arp_table;
	find_client_list_wlan(&arp_table);
	for(i = 0; i < arp_table.count; i++)
	{
		for(j = 0; j < sum; j++)
		{
			if(!strcmp(arp_table.table[i].mac, info->host[j].mac))
			{
				info->host[j].stat = 1;
				break;
			}
		}
	}

	info->count = sum;

	return sum;
}

int  cpe_refresh_igd_LANDevice(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, 1);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int cpe_get_igd_LANEthernetInterfaceNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param, "%d", igd_entries.eth_entry);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_LANWLANConfigurationNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param, "%d", igd_entries.wlan_entry);
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_HostNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    sprintf(param, "%d", igd_entries.host_entry);
    *value = param;
    
    return FAULT_CODE_OK;
}

int  cpe_refresh_igd_Host(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    //getHostsInfo(&host_info);
    get_hosts_info(&host_info);
    igd_entries.host_entry = host_info.count;
    cwmp_refresh_i_parameter(cwmp, param_node, igd_entries.host_entry);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);
    return FAULT_CODE_OK;
}

int cpe_get_igd_ActiveOfHost(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index = get_parameter_index((char *)name, "Host.", igd_entries.host_entry);
    if(host_info.host[index - 1].stat)
    	strcpy(param, "true");
    else
    	strcpy(param, "false");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_HostName(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index = get_parameter_index((char *)name, "Host.", igd_entries.host_entry);
    strcpy(param, host_info.host[index - 1].host_name);
    if(strlen(param) == 0)
    	*value = NULL;
    else
    	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_InterfaceType(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index = get_parameter_index((char *)name, "Host.", igd_entries.host_entry);
    if(host_info.host[index - 1].if_type == 0)
        strcpy(param, "Ethernet");
    else if(host_info.host[index - 1].if_type == 1)
        strcpy(param, "802.11");
    else
        strcpy(param, "Other");
    *value = param;
    
    return FAULT_CODE_OK;
}

int cpe_get_igd_Layer2Interface(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index = get_parameter_index((char *)name, "Host.", igd_entries.host_entry);
    if(host_info.host[index - 1].if_type == 0)
        strcpy(param, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.1");
    else if(host_info.host[index - 1].if_type == 1)
        strcpy(param, "InternetGatewayDevice.LANDevice.1.WLANConfiguration");
    else
        strcpy(param, "InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.1");
    *value = param;
    
    return FAULT_CODE_OK;
}

int cpe_get_IPAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index = get_parameter_index((char *)name, "Host.", igd_entries.host_entry);
    strcpy(param, host_info.host[index - 1].ip);
    *value = param;

    return FAULT_CODE_OK;;
}

int cpe_get_AddressSourceOfHost(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index = get_parameter_index((char *)name, "Host.", igd_entries.host_entry);
    if(host_info.host[index - 1].addr_src == 0)
        strcpy(param, "DHCP");
    else if(host_info.host[index - 1].addr_src == 1)
        strcpy(param, "Static");
    else
        strcpy(param, "AutoIP");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_LeaseTimeRemainingOfHost(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index = get_parameter_index((char *)name, "Host.", igd_entries.host_entry);
    sprintf(param, "%d", host_info.host[index - 1].leases);
    *value = param;

    return FAULT_CODE_OK;;
}

int cpe_get_igd_MACAddressOfHost(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index = get_parameter_index((char *)name, "Host.", igd_entries.host_entry);
    strcpy(param, host_info.host[index - 1].mac);
    *value = param;

    return FAULT_CODE_OK;;
}

int  cpe_refresh_igd_LANEthernetInterfaceConfig(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, igd_entries.eth_entry);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int cpe_get_igd_EnableOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "true");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_EnableOfEth(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}
int cpe_get_igd_NameofLANEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    //int index =0;
    //index = get_parameter_index((char *)name, "LANEthernetInterfaceConfig.", igd_entries.eth_entry);
    strcpy(param, "eth0.1");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_MACAddressOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char buff[128];
    // int index =0;
    // index = get_parameter_index((char *)name, "LANEthernetInterfaceConfig.", igd_entries.eth_entry);
    // sprintf(buff,"ifconfig eth%d | grep \"HWaddr\" | awk \'{print $5}\'",index - 1);
    strcpy(buff, "ifconfig br-lan | grep 'HWaddr' | awk '{print $5}'");
    read_memory(buff,param,sizeof(param));
    if (strlen(param) == 0) {
    	strcpy(param,"00:00:00:00:00:00");
    }
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_MaxBitRateOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index =0;
    char cmdbuf[256];
    index = get_parameter_index((char *)name, "LANEthernetInterfaceConfig.", igd_entries.eth_entry);
    sprintf(cmdbuf,"swconfig dev mt7530 port %d show | grep link |sed 's/:/ /g' | awk '{print $7}' | tr -d 'baseT'",index - 1);
    read_memory(cmdbuf,param,sizeof(param));
    util_strip_traling_spaces(param);
    if ( 0 == strlen(param)) {
	strcpy(param,"Auto");
    } 
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_DuplexModeOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param,"Full");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_StatusOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index = get_parameter_index((char *)name, "LANEthernetInterfaceConfig.", igd_entries.eth_entry);
    char cmdbuf[128];
    sprintf(cmdbuf,"swconfig dev mt7530 port %d show | grep link |sed 's/:/ /g' | awk '{print $5}'",index - 1);
    read_memory(cmdbuf,param,sizeof(param));
    util_strip_traling_spaces(param);
    if ( 0 == strcmp(param,"up")) {
	strcpy(param,"Up");
    } 
    else if ( 0 == strcmp(param,"down")) {
	strcpy(param,"Nolink");
    } 
    else if ( 0 == strcmp(param,"error")) {
	strcpy(param,"Error");
    } 
    else{
	strcpy(param,"Error");
    } 
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_BytesReceivedOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	// int index = get_parameter_index((char *)name, "LANEthernetInterfaceConfig.", igd_entries.eth_entry);
	// sprintf(buff, "ifconfig eth%d | grep \"RX bytes\" | awk \'{print $2}\'| sed \'s/bytes://g\'", index - 1);
	strcpy(buff, "ifconfig br-lan | grep \"RX bytes\" | awk \'{print $2}\'| sed \'s/bytes://g\'");
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_igd_BytesSentOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	// int index = get_parameter_index((char *)name, "LANEthernetInterfaceConfig.", igd_entries.eth_entry);
	// sprintf(buff, "ifconfig eth%d | grep \"TX bytes\" | awk \'{print $6}\'| sed \'s/bytes://g\'", index - 1);
	strcpy(buff, "ifconfig br-lan | grep \"TX bytes\" | awk \'{print $6}\'| sed \'s/bytes://g\'");
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_igd_PacketsReceivedOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	// int index = get_parameter_index((char *)name, "LANEthernetInterfaceConfig.", igd_entries.eth_entry);
	// sprintf(buff, "ifconfig eth%d | grep \"RX packets\" | awk \'{print $2}\'| sed \'s/packets://g\'", index - 1);
	strcpy(buff, "ifconfig br-lan | grep \"RX packets\" | awk \'{print $2}\'| sed \'s/packets://g\'");
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_igd_PacketsSentOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	// int index = get_parameter_index((char *)name, "LANEthernetInterfaceConfig.", igd_entries.eth_entry);
	strcpy(buff, "ifconfig br-lan | grep \"TX packets\" | awk \'{print $2}\'| sed \'s/packets://g\'");
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_igd_ErrorsSentOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	// int index = get_parameter_index((char *)name, "LANEthernetInterfaceConfig.", igd_entries.eth_entry);
	strcpy(buff, "ifconfig br-lan | grep \"TX packets\" | awk \'{print $3}\'| sed \'s/errors://g\'");
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_igd_ErrorsReceivedOfEth(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	// int index = get_parameter_index((char *)name, "LANEthernetInterfaceConfig.", igd_entries.eth_entry);
	strcpy(buff, "ifconfig br-lan | grep \"RX packets\" | awk \'{print $3}\'| sed \'s/errors://g\'");
	read_memory(buff,param,sizeof(param));
	util_strip_traling_spaces(param);
	*value = param;
	return FAULT_CODE_OK;
}

int  cpe_refresh_igd_IPInterface(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, igd_entries.ipinterface_entry);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int cpe_get_igd_EnableOfIPInterface(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index = get_parameter_index((char *)name, "IPInterface.", igd_entries.ipinterface_entry);
    sprintf(param, "br%d", index - 1);
    strcpy(param, getNetStat(param)? "true" : "false");
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_set_igd_EnableOfIPInterface(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{	
	int boolean = 0;
	if (strcmp("true",value) == 0) {
		boolean = 1;
	} else if (strcmp("false",value) == 0) {
		boolean = 0;
	} else {
		return -1;
	}
	int index =0;
	index = get_parameter_index((char *)name, "IPInterface.", igd_entries.ipinterface_entry);
    sprintf(param, "ifconfig br%d %s", index - 1, boolean ? "up" : "down");
    system(param);
	return FAULT_CODE_OK;
}

int cpe_get_igd_IPInterfaceAddressingType(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param,"DHCP");
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_igd_IPInterfaceIPAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    read_memory("uci -q get network.lan.ipaddr", param, 64);
    util_strip_traling_spaces(param);
    if (strlen(param) == 0) {
        strcpy(param,"N/A");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_IPInterfaceIPAddress(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    //set_config_attr("AP_IPADDR", value);
    return FAULT_CODE_OK;
}

int cpe_get_igd_IPInterfaceSubnetMask(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	uci_get_single_config_attr("uci get dhcp.lan.network 2>/dev/null",param);
	if (strlen(param) == 0) {
		strcpy(param,"255.255.255.0");
	} else {
		strcpy(param,"255.255.255.0");
	}
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_set_igd_IPInterfaceSubnetMask(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    //set_config_attr("AP_NETMASK", value);
    return FAULT_CODE_OK;
}

int cpe_get_igd_DHCPLeaseTime(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char lt[8]={0};
    int len = 0;
    read_memory("uci get dhcp.lan.leasetime 2>/dev/null", lt, sizeof(lt));
    len = strlen(lt);
    if (0 == len){
    	strcpy(param,"0");
    }
    else{
        int sec = 1;
        if (strchr(lt,'h')){
            sec = 3600;
        }
        else if (strchr(lt,'m')){
            sec = 60;
        }
        sprintf(param,"%d", atoi(lt) * sec);
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_DHCPLeaseTime(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    char cmdbuf[256] = {0};
    int sec = atoi(value);
    sprintf(cmdbuf,"uci -q set dhcp.lan.leasetime=%ds;uci -q commit dhcp",sec);
    system(cmdbuf);
    cmd_touch(REBOOT_DNSMASQ_MODULE);
    callback_reg(cwmp, restart_dnsmasq, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_DHCPRelay(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "false");
	*value = param;
	return FAULT_CODE_OK;
}
int cpe_get_igd_MACAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char buff[128];
	strcpy(buff, "ifconfig br-lan | grep \"HWaddr\" | awk \'{print $5}\'");
	read_memory(buff,param,sizeof(param));
	if (strlen(param) == 0) {
		strcpy(param,"00:00:00:00:00:00");
	}
	util_strip_traling_spaces(param);
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_get_igd_DHCPServerConfigurable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "false");
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_set_igd_DHCPServerConfigurable(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	strcpy(param, value);
	return FAULT_CODE_OK;

}

int cpe_get_igd_DHCPServerEnable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    read_memory("uci get dhcp.lan.ignore 2>/dev/null",param,8);
    if (strlen(param) == 0) {
    	strcpy(param,"true");
    } else {
    	if (strcmp(param,"1") == 0) {
    		strcpy(param,"false");
    	} else if(strcmp(param,"0") == 0) {
    		strcpy(param,"true");
    	} else {
    		strcpy(param,"true");
    	}
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_DHCPServerEnable(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    int ig = 0;
    char cmdbuf[64] = {0};
    if (strcmp(value,"false") == 0) {
        ig = 1;
    } else if(strcmp(value,"true") == 0) {
        ig = 0;
    } else {
        return FAULT_CODE_9007;
    }
    sprintf(cmdbuf,"uci -q set dhcp.lan.ignore=%d;uci -q commit dhcp",ig);
    system(cmdbuf);
    cmd_touch(REBOOT_DNSMASQ_MODULE);
    callback_reg(cwmp, restart_dnsmasq, NULL, NULL);
    return FAULT_CODE_OK;
}
extern int getDNS(char *outText, char *type);
extern int getGatewayByDevName(const char *devName, char *outText);
extern int getReservedAddress(char *outText,int sizelen);

int cpe_get_igd_DNSServers(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmdbuf[256]="uci -q get dhcp.lan.dhcp_option | sed 's/ /\\n/g' | grep -E '6,' | sed 's/6,//g'";
    read_memory(cmdbuf, param, 64);
    util_strip_traling_spaces(param);
    if (0 == strlen(param))
    {
        strcpy(param,"N/A");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_DNSServers(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    char cmdbuf[256] = "uci -q get dhcp.lan.dhcp_option | sed 's/ /\\n/g' | grep -E '6,'";
    char cmdres[64] = {0};
    read_memory(cmdbuf, cmdres, sizeof(cmdres));
    util_strip_traling_spaces(cmdres);
    if(0 < strlen(cmdres))
    {
        sprintf(cmdbuf,"uci -q del_list dhcp.lan.dhcp_option=%s;uci -q commit dhcp", cmdres);
        system(cmdbuf);
    }
    sprintf(cmdbuf,"uci -q add_list dhcp.lan.dhcp_option=6,%s;uci -q commit dhcp", value);
    system(cmdbuf);
    cmd_touch(REBOOT_DNSMASQ_MODULE);
    callback_reg(cwmp, restart_dnsmasq, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_DomainName(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    *value = NULL;
    return FAULT_CODE_OK;
}

int cpe_set_igd_DomainName(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}

int cpe_get_igd_IPInterfaceNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	sprintf(param, "%d", igd_entries.ipinterface_entry);
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_IPRouters(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmdbuf[256]="uci -q get dhcp.lan.dhcp_option | sed 's/ /\\n/g' | grep -E '3,' | sed 's/3,//g'";
    read_memory(cmdbuf, param, 64);
    util_strip_traling_spaces(param);
    if (0 == strlen(param))
    {
        strcpy(param,"N/A");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_IPRouters(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    char cmdbuf[256] = "uci -q get dhcp.lan.dhcp_option | sed 's/ /\\n/g' | grep -E '3,'";
    char cmdres[64] = {0};
    read_memory(cmdbuf, cmdres, sizeof(cmdres));
    util_strip_traling_spaces(cmdres);
    if(0 < strlen(cmdres))
    {
        sprintf(cmdbuf,"uci -q del_list dhcp.lan.dhcp_option=%s;uci -q commit dhcp", cmdres);
        system(cmdbuf);
    }
    sprintf(cmdbuf,"uci -q add_list dhcp.lan.dhcp_option=3,%s;uci -q commit dhcp", value);
    system(cmdbuf);
    cmd_touch(REBOOT_DNSMASQ_MODULE);
    callback_reg(cwmp, restart_dnsmasq, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_MaxAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmdbuf[256]="cat /var/etc/dnsmasq.conf | grep 'dhcp-range=lan,' |awk -F, '{print $3}'";
    read_memory(cmdbuf, param, sizeof(param));
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_MaxAddress(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    char cmdbuf[256]="uci -q get dhcp.lan.start";
    char cmdres[32] = {0};
    read_memory(cmdbuf, cmdres, sizeof(cmdres));
    util_strip_traling_spaces(cmdres);
    if (!strchr(cmdres,'.'))
    {
        strcpy(cmdbuf,"ip=$(uci -q get network.lan.ipaddr);echo ${ip%.*}.$(uci -q get dhcp.lan.start)");
        read_memory(cmdbuf, cmdres, sizeof(cmdres));
        util_strip_traling_spaces(cmdres);
    }
    int s[4] = {0}, e[4] = {0};
    int limit = 150;
    sscanf(cmdres,"%d.%d.%d.%d",&s[0],&s[1],&s[2],&s[3]);//start ip
    sscanf(value,"%d.%d.%d.%d",&e[0],&e[1],&e[2],&e[3]); //end ip
    limit = ((e[0]<<24)+(e[1]<<16)+(e[2]<<8)+e[3]) - ((s[0]<<24)+(s[1]<<16)+(s[2]<<8)+s[3]); //ip nums
    if(1 > limit || 65535 < limit)
    {
        limit = 150;
    }
    sprintf(cmdbuf,"uci -q set dhcp.lan.limit=%d;uci -q commit dhcp",limit+1);
    system(cmdbuf);
    cmd_touch(REBOOT_DNSMASQ_MODULE);
    callback_reg(cwmp, restart_dnsmasq, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_MinAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char cmdbuf[256]="cat /var/etc/dnsmasq.conf | grep 'dhcp-range=lan,' |awk -F, '{print $2}'";
    read_memory(cmdbuf, param, sizeof(param));
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_MinAddress(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    char cmdbuf[256];
    sprintf(cmdbuf,"uci -q set dhcp.lan.start=%s;uci -q commit dhcp",value);
    system(cmdbuf);
    cmd_touch(REBOOT_DNSMASQ_MODULE);
    callback_reg(cwmp, restart_dnsmasq, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_ReservedAddresses(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	/*getReservedAddress(param,sizeof(param));
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		util_strip_traling_spaces(param);
		*value = param;
	}*/
	*value = NULL;
	return FAULT_CODE_OK;

}

/*static void print_param(parameter_node_t * param, int level)
{
  if(!param) return; 
 
  parameter_node_t * child;
  char fmt[64];
  //cwmp_log_debug("name: %s, type: %s, level: %d\n", param->name, cwmp_get_type_string(param->type), level);
  
  sprintf(fmt, "|%%-%d\s%%s", level*4);
  
  cwmp_log_debug(fmt, "----", param->name);
  child = param->child;

  if(!child)
	return;
  print_param(child, level+1);

  parameter_node_t * next = child->next_sibling;

  while(next)
 {
    print_param(next, level+1);
    next = next->next_sibling;
 }

	
}*/


DHCPStatic dhcpstatic[200];
static int  dhcpindexmax = 0;
#define OLDTMPNAME "/tmp/rc.dhcp.static"
#define NEWNAME "/etc/rc.d/rc.dhcp.static"
int apply_dhcpstatic_ip_mac(void)
{
	int i;
	char strvalue[128];
	strcpy(strvalue,"# Static leases map");
	cmd_echo_append(strvalue, OLDTMPNAME);
	for(i = 0; i < dhcpindexmax; i++) {
		if (strcmp(dhcpstatic[i].Enable,"true") == 0) {
			sprintf(strvalue, "static_lease %s %s", dhcpstatic[i].MacAddress, dhcpstatic[i].IPAddress);
		} else if (strcmp(dhcpstatic[i].Enable,"false") == 0) {
			sprintf(strvalue, "#static_lease %s %s", dhcpstatic[i].MacAddress, dhcpstatic[i].IPAddress);
		} else if (strlen(dhcpstatic[i].Enable) == 0){
			continue;
		}
		cmd_echo_append(strvalue, OLDTMPNAME);
	}
	sprintf(strvalue, "mv %s %s", OLDTMPNAME, NEWNAME);
	if (cmd_file_exist(OLDTMPNAME) == 1) {
		system(strvalue);
		return 0;	
	} 
	return -1;
}

int  cpe_add_igd_DHCPStaticAddress(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();
    if(!param_node)
    {
        return FAULT_CODE_9002;
    }
	++igd_entries.dhcpstatic_entry;
    ++dhcpindexmax;
	memset(&dhcpstatic[dhcpindexmax - 1], 0, sizeof(DHCPStatic));
	parameter_node_t * wanconn_param;
    cwmp_model_copy_parameter(param_node, &wanconn_param, dhcpindexmax);
	*pinstance_number = dhcpindexmax;
    return FAULT_CODE_OK;
}


int  cpe_del_igd_DHCPStaticAddress(cwmp_t * cwmp, parameter_node_t * param_node, int instance_number,  callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    if(!param_node)
    {
        return FAULT_CODE_9002;
    }
	memset(&dhcpstatic[instance_number - 1], 0, sizeof(DHCPStatic));
	igd_entries.dhcpstatic_entry--;
	apply_dhcpstatic_ip_mac();
	cwmp_model_delete_parameter(param_node);
	
    return FAULT_CODE_OK;
}
extern int read_dhcptatic_info(DHCPStatic *dhcpstaticinfo,int sizelen);

int  cpe_refresh_igd_DHCPStaticAddress(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
	read_dhcptatic_info(dhcpstatic,sizeof(dhcpstatic));
	dhcpindexmax = igd_entries.dhcpstatic_entry;
	cwmp_refresh_i_parameter(cwmp, param_node, dhcpindexmax);
	cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

	return FAULT_CODE_OK;
}

int cpe_get_igd_DHCPStaticAddressEnable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index = get_parameter_index((char *)name, "DHCPStaticAddress.", dhcpindexmax);

	if (strlen(dhcpstatic[index - 1].Enable) == 0) {
		*value = NULL;
	} else {
		strcpy(param, dhcpstatic[index - 1].Enable);
		*value = param;
	}	
	return FAULT_CODE_OK;

}

int cpe_set_igd_DHCPStaticAddressEnable(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	int index = get_parameter_index((char *)name, "DHCPStaticAddress.", dhcpindexmax);
	if ((strlen(dhcpstatic[index - 1].IPAddress) != 0) && ( strlen(dhcpstatic[index - 1].MacAddress) != 0) ) {
		strncpy(dhcpstatic[index - 1].Enable, value, sizeof(dhcpstatic[index - 1].Enable));
		dhcpstatic[index - 1].Enable[strlen(dhcpstatic[index - 1].Enable)] = '\0';
		if (apply_dhcpstatic_ip_mac() == 0)
			return FAULT_CODE_OK;
	}
    return FAULT_CODE_9007;
}

int  cpe_get_igd_DHCPStaticAddressNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	sprintf(param, "%d", igd_entries.dhcpstatic_entry);
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_get_igd_DHCPStaticAddressIp(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index = get_parameter_index((char *)name, "DHCPStaticAddress.", dhcpindexmax);
	sprintf(param, "%s", dhcpstatic[index - 1].IPAddress);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	return FAULT_CODE_OK;

}

int cpe_set_igd_DHCPStaticAddressIp(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	int index = get_parameter_index((char *)name, "DHCPStaticAddress.", dhcpindexmax);
	strncpy(dhcpstatic[index - 1].IPAddress, value, sizeof(dhcpstatic[index - 1].IPAddress));
	dhcpstatic[index - 1].IPAddress[strlen(dhcpstatic[index - 1].IPAddress)] = '\0';
    return FAULT_CODE_OK;
}

int cpe_get_igd_DHCPStaticAddressMac(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index = get_parameter_index((char *)name, "DHCPStaticAddress.", dhcpindexmax);
	sprintf(param, "%s", dhcpstatic[index - 1].MacAddress);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	return FAULT_CODE_OK;
}

int cpe_set_igd_DHCPStaticAddressMac(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	int index = get_parameter_index((char *)name, "DHCPStaticAddress.", dhcpindexmax);
	strncpy(dhcpstatic[index - 1].MacAddress, value, sizeof(dhcpstatic[index - 1].MacAddress));
	dhcpstatic[index - 1].IPAddress[strlen(dhcpstatic[index - 1].MacAddress)] = '\0';
    return FAULT_CODE_OK;
}

int cpe_get_igd_SubnetMask(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	uci_get_single_config_attr("uci get dhcp.lan.network 2>/dev/null",param);
	if (strlen(param) == 0) {
		strcpy(param,"255.255.255.0");
	} else {
		strcpy(param,"255.255.255.0");
	}
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_set_igd_SubnetMask(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    //set_config_attr("AP_NETMASK", value);
    return FAULT_CODE_OK;
}

int  cpe_refresh_igd_AssociatedDevice(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
	getWlanClientInfo(&wlan_client);
	igd_entries.wlan_client_entry = wlan_client.count;
    cwmp_refresh_i_parameter(cwmp, param_node, igd_entries.wlan_client_entry);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 

    return FAULT_CODE_OK;
}

int cpe_get_AssociatedDeviceMACAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index;

    index = get_parameter_index((char *)name, "AssociatedDevice.", igd_entries.wlan_client_entry);
    strcpy(param, wlan_client.client[index - 1].mac);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_AssociatedDeviceIPAddress(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index;

    index = get_parameter_index((char *)name, "AssociatedDevice.", igd_entries.wlan_client_entry);
    strcpy(param, wlan_client.client[index - 1].ip);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_AssociatedDeviceAuthenticationState(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "true");
	*value = param;

    return FAULT_CODE_OK;
}

int cpe_get_LastDataTransmitRate(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int index;

    index = get_parameter_index((char *)name, "AssociatedDevice.", igd_entries.wlan_client_entry);
    strcpy(param, wlan_client.client[index - 1].rate);
    *value = param;

    return FAULT_CODE_OK;
}

int  cpe_refresh_igd_PreSharedKey(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, 1);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int  cpe_refresh_igd_WEPKey(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, 1);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int cpe_get_igd_WEPKey(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index;
	
    index = get_parameter_index((char *)name, "WLANConfiguration.", igd_entries.wlan_entry);
    if(index == 1)
    	get_single_config_attr("PWD", buffer);
    else if(index == 2)
    	get_single_config_attr("PWD_3", buffer);
    else if(index == 3)
    	get_single_config_attr("PWD_4", buffer);
    else
    {
    	*value = NULL;
    	return FAULT_CODE_9005;
    }
	if (strlen(buffer) == 0) {
		strcpy(param, "None");
	} else {
		strcpy(param, buffer);
	}
    *value = param;

    return FAULT_CODE_OK;    
}

int cpe_set_igd_WEPKey(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    int index;
    char *pwd, *psk_key;
    char *pwd_val, *psk_key_val;

    index = get_parameter_index((char *)name, "WLANConfiguration.", igd_entries.wlan_entry);
    if(index == 1)
    {
    	pwd = "PWD";
    	psk_key = "PSK_KEY";
    }
    else if(index == 2)
    {
    	pwd = "PWD_3";
    	psk_key = "PSK_KEY_3";
    }
    else if(index == 3)
    {
    	pwd = "PWD_4";
    	psk_key = "PSK_KEY_4";
    }
    else
    {
    	return FAULT_CODE_9005;
    }
	if(value == NULL || strlen(value) == 0)
	{
		pwd_val = "";
		psk_key_val = "";
	}
    else if(strlen(value) >= 8 && strlen(value) <= 63)
    {
		pwd_val = value;
		psk_key_val = value;
    }
    else
    {
    	return FAULT_CODE_9007;
    }
    set_single_config_attr(pwd, pwd_val);
    set_single_config_attr(psk_key, psk_key_val);
    callback_reg(cwmp, restart_wifi, NULL, NULL);

    return FAULT_CODE_OK;
}

int  cpe_refresh_igd_WLANConfiguration(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    igd_entries.wlan_entry = uci_get_wifi_num();
    cwmp_refresh_i_parameter(cwmp, param_node, igd_entries.wlan_entry);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int cpe_get_igd_AutoChannelEnable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    get_single_config_attr("AP_PRIMARY_CH", param);
    if(!strcmp(param, "auto"))
        strcpy(param, "true");
    else
        strcpy(param, "false");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_idg_AutoChannelEnable(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    if(!strcmp(value, "true"))
        set_single_config_attr("AP_PRIMARY_CH", "auto");

    return FAULT_CODE_OK;
}

int cpe_get_igd_AutoRateFallBackEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "true");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_BasicAuthenticationMode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name ,"encryption", &ptmp);
    if(FAULT_CODE_OK != res){
    	*value = NULL;
    	return res;
    }
    if(0 == strcmp(ptmp,"none")){
        strcpy(param, "None");
    }
    else{
        strcpy(param, "PSKAuthentication");
    } 
    return FAULT_CODE_OK;
}

int cpe_get_igd_BasicDataTransmitRates(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "1,2,5.5,6,9,11,12,18,24,36,48,54");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_BasicEncryptionModes(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name ,"encryption", &ptmp);
    if(FAULT_CODE_OK != res){
    	*value = NULL;
    	return res;
    }
    if(0 == strcmp(ptmp,"none")){
        strcpy(param, "None");
    }
    else{
        strcpy(param, "WPAEncryption");
    } 
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_BeaconType(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "WPA");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_BeaconType(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    int index;
    char *wpa, *secMode, *cypher;
    char *wpa_val, *secMode_val;

    index = get_parameter_index((char *)name, "WLANConfiguration.", igd_entries.wlan_entry);
    if(index == 1)
    {
    	wpa = "AP_WPA";
    	secMode = "AP_SECMODE";
    	cypher = "AP_CYPHER";
    }
    else if(index == 2)
    {
    	wpa = "AP_WPA_3";
    	secMode = "AP_SECMODE_3";
    	cypher = "AP_CYPHER_3";
    }
    else if(index == 3)
    {
    	wpa = "AP_WPA_3";
    	secMode = "AP_SECMODE_4";
    	cypher = "AP_CYPHER_4";
    }
    else
    {
    	return FAULT_CODE_9005;
    }

    if(value == NULL || strlen(value) == 0 || !strcmp(value, "None"))
    {
    	wpa_val = "";
    	secMode_val = "None";
    	set_single_config_attr(cypher, "");
    }
    else if(!strcmp(value, "WPA"))
    {
    	wpa_val = "1";
    	secMode_val = "WPA";
    }
    else if(!strcmp(value, "11i"))
    {
    	wpa_val = "2";
    	secMode_val = "WPA";
    }
    else if(!strcmp(value, "WPAand11i"))
    {
    	wpa_val = "3";
    	secMode_val = "WPA";
    }
    else
    {
    	return FAULT_CODE_9007;
    }
    set_single_config_attr(wpa, wpa_val);
    set_single_config_attr(secMode, secMode_val);
    callback_reg(cwmp, restart_wifi, NULL, NULL);

    return FAULT_CODE_OK;
}


int cpe_get_igd_BSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res; 
    res = uci_get_wifi_param(name, "ifname", &ptmp);
    if (FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[128];
    sprintf(cmdbuf,"iwinfo %s info | grep -E 'Access Point:' | awk '{print $3}'", ptmp);
    read_memory(cmdbuf, param, 64); 
    util_strip_traling_spaces(param); 
    *value = param;
    
    return FAULT_CODE_OK;
}

int cpe_get_igd_Channel(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name,"device",&ptmp);
    if(FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf,"uci -q get wireless.%s.channel", ptmp);
    read_memory(cmdbuf, param, 64);
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_Channel(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name,"device",&ptmp);
    if(FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf,"uci -q get wireless.%s.channel=%s;uci -q commit wireless", ptmp, value);
    system(cmdbuf);
    cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);

    return FAULT_CODE_OK;
}

int cpe_get_igd_EnableOfWLAN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    
    int res = uci_get_wifi_param(name, "disabled", value);
    if(FAULT_CODE_OK != res){
        return res;
    }
    if(0 == strcmp(*value, "1")){
    	strcpy(param, "false");
    }
    else{
    	strcpy(param, "true");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_EnableOfWLAN(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    if(value == NULL){
        return FAULT_CODE_9007;
    }
    int num = uci_get_wifi_num();
    int index = get_parameter_index((char *)name, "WLANConfiguration.", 65535);
    if(index > num){
        return FAULT_CODE_9005;
    }
    int disa = 0;
    if(0 == strcmp(value,"true")){
        disa = 0;
    }
    else if(0 == strcmp(value,"false")){
        disa = 1;
    }
    else{
        return FAULT_CODE_9007;
    }
    char cmdbuf[256];
    sprintf(cmdbuf,"uci -q set wireless.@wifi-iface[%d].disabled=%d;uci -q commit wireless",index - 1,disa);
    system(cmdbuf);
    cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_IEEE11iAuthenticationMode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    get_single_config_attr("AP_SECFILE", param);
    if(!strcmp(param, "PSK"))
    {
        strcpy(param, "PSKAuthentication");
    } else {
    	strcpy(param, "None");
    }
        *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_IEEE11iEncryptionModes(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name ,"encryption", &ptmp);
    if(FAULT_CODE_OK != res){
    	*value = NULL;
    	return res;
    }
    if(0 == strcmp(ptmp,"none")){
        strcpy(param, "None");
        *value = param;
        return FAULT_CODE_OK;
    }
    if(0 == strcmp(ptmp,"psk2+tkip+ccmp")
      || 0 == strcmp(ptmp,"psk+tkip+ccmp")
      || 0 == strcmp(ptmp,"psk+psk2")){
        strcpy(param, (char *)WPAEncryptionModes[2][1]);
    }
    else if(0 == strcmp(ptmp,"psk2+ccmp") || 0 == strcmp(ptmp,"psk+ccmp")){
        strcpy(param, (char *)WPAEncryptionModes[1][1]);
    }
    else if(0 == strcmp(ptmp,"psk2+tkip") || 0 == strcmp(ptmp,"psk+tkip")){
        strcpy(param, (char *)WPAEncryptionModes[0][1]);
    }
    else{
        //strcpy(param, (char *)WPAEncryptionModes[2][1]);
        strcpy(param, "None");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_MACAddressControlEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "true");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_MaxBitRateOfWlan(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "Auto");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_OperationalDataTransmitRates(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "1,2,5.5,6,9,11,12,18,24,36,48,54,72");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_PossibleChannels(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name,"device",&ptmp);
    if(FAULT_CODE_OK != res){
        return res;
    }
    char cmdbuf[64];
    char band[8];
    //char country[8];
    sprintf(cmdbuf, "uci -q get wireless.%s.band", ptmp);
    read_memory(cmdbuf, band, sizeof(band)); 
    util_strip_traling_spaces(band);
    /*
    sprintf(cmdbuf, "uci -q get wireless.%s.country", ptmp);
    read_memory(cmdbuf, country, sizeof(country)); 
    util_strip_traling_spaces(country);
    */
    if(0 == strcmp(band,"5G")){
        strcpy(param, "36,40,44,48,52,56,60,64,149,153,157,161,165");
    }
    else{
        strcpy(param, "1-13");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_PossibleDataTransmitRates(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "1,2,5.5,6,9,11,12,18,24,36,48,54");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_RadioEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name,"disabled",&ptmp);
    if(FAULT_CODE_OK != res){
        return res;
    }
    if(0 == strcmp(ptmp, "1")){
        strcpy(param, "false");
    }
    else{
        strcpy(param, "true");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_SSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return uci_get_wifi_param(name,"ssid", value);
}

int cpe_set_igd_SSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    if(value == NULL || strlen(value) == 0)
    {
        return FAULT_CODE_9007;	
    }
    int num = uci_get_wifi_num();
    int index = get_parameter_index((char *)name, "WLANConfiguration.", 65535);
    if(index > num)
    {   
        return FAULT_CODE_9005;
    }   
    char cmdbuf[256];
    sprintf(cmdbuf,"uci -q set wireless.@wifi-iface[%d].ssid=%s;uci -q commit wireless", index - 1,value);
    system(cmdbuf);
    cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);
    
    return FAULT_CODE_OK;
}

int cpe_get_igd_SSIDAdvertisementEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name,"hidden",&ptmp);
    if(FAULT_CODE_OK != res){
        return res;
    }
    if(0 == strcmp(ptmp, "1")){
        strcpy(param, "false");
    }
    else{
        strcpy(param, "true");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_SSIDAdvertisementEnabled(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    int inum;
    int index;
    inum = uci_get_wifi_num();
    index = get_parameter_index((char *)name, "WLANConfiguration.", 65535);
    if(index > inum){
        return FAULT_CODE_9005;
    }
    int hidden = 0;  
    if(0 == strcmp(value, "true")){
        hidden = 0;
    }
    else if(0 == strcmp(value, "false")){
        hidden = 1;
    }
    else{
        return FAULT_CODE_9007;
    }
    char cmdbuf[256];
    sprintf(cmdbuf, "uci -q set wireless.@wifi-iface[%d].hidden=%d;uci -q commit wireless",index - 1, hidden);
    system(cmdbuf); 
    cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_Standard(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int i;
    get_single_config_attr("WIFIWORKMODE", param);
    for(i = 0; i < 9; i++)
    {
        if(!strcmp((char *)wifi_standard[i][0], param))
        {
            strcpy(param, (char *)wifi_standard[i][1]);
            *value = param;

            return FAULT_CODE_OK;
        }
    }
    *value = NULL;

    return FAULT_CODE_OK;
}

int cpe_set_igd_Standard(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    int i;
    for(i = 0; i < 9; i++)
    {
        if(!strcmp((char *)wifi_standard[i][1], value))
        {
            strcpy(param, (char *)wifi_standard[i][0]);
            set_single_config_attr("WIFIWORKMODE", param);

            return FAULT_CODE_OK;
        }
    }
	return FAULT_CODE_9007;
}

int cpe_get_igd_WEPKeyIndex(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "0");
	*value = param;

	return FAULT_CODE_OK;
}

int cpe_set_igd_WEPKeyIndex(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	
	return FAULT_CODE_OK;
}

int cpe_get_igd_StatusOfWLAN(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int res = uci_get_wifi_param(name, "disabled", value);
    if(FAULT_CODE_OK != res){
        return res;
    }
    if(0 == strcmp(*value, "1")){
    	strcpy(param, "Disabled");
    }
    else{
    	strcpy(param, "Up");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_TotalAssociations(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	sprintf(param, "%d", igd_entries.wlan_client_entry);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_TotalBytesReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res; 
    res = uci_get_wifi_param(name, "ifname",&ptmp);
    if (FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf, "cat /proc/net/dev | grep %s | awk '{print $12}'", ptmp);
    read_memory(cmdbuf, param, 32);
    util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_TotalBytesSent(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name, "ifname",&ptmp);
    if (FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf, "cat /proc/net/dev | grep %s | awk '{print $10}'", ptmp);
    read_memory(cmdbuf, param, 32);
    util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_TotalPacketsReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name, "ifname",&ptmp);
    if (FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf, "cat /proc/net/dev | grep %s | awk '{print $3}'", ptmp);
    read_memory(cmdbuf, param, 32);
    util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;

}

int cpe_get_igd_TotalPacketsSent(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name, "ifname",&ptmp);
    if (FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf, "cat /proc/net/dev | grep %s | awk '{print $11}'", ptmp);
    read_memory(cmdbuf, param, 32);
    util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_ErrorsReceived(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name, "ifname",&ptmp);
    if (FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf, "cat /proc/net/dev | grep %s | awk '{print $4}'", ptmp);
    read_memory(cmdbuf, param, 32);
    util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_ErrorsSent(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name, "ifname",&ptmp);
    if (FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf, "cat /proc/net/dev | grep %s | awk '{print $12}'", ptmp);
    read_memory(cmdbuf, param, 32);
    util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_DiscardPacketsSentOfWlan(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name, "ifname",&ptmp);
    if (FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf, "cat /proc/net/dev | grep %s | awk '{print $13}'", ptmp);
    read_memory(cmdbuf, param, 32);
    util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_DiscardPacketsReceivedOfWlan(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name, "ifname",&ptmp);
    if (FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf, "cat /proc/net/dev | grep %s | awk '{print $5}'", ptmp);
    read_memory(cmdbuf, param, 32);
    util_strip_traling_spaces(param);
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_WEPEncryptionLevel(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "Disabled");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_WPAAuthenticationMode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name ,"encryption", &ptmp);
    if(FAULT_CODE_OK != res){
    	*value = NULL;
    	return res;
    }
    if(0 == strcmp(ptmp,"none")){
        strcpy(param, "None");
    }
    else{
        strcpy(param, "PSKAuthentication");
    } 
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WPAAuthenticationMode(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}

int cpe_get_igd_WPAEncryptionModes(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name ,"encryption", &ptmp);
    if(FAULT_CODE_OK != res){
    	*value = NULL;
    	return res;
    }
    if(0 == strcmp(ptmp,"none")){
        strcpy(param, "None");
        *value = param;
        return FAULT_CODE_OK;
    }
    if(0 == strcmp(ptmp,"psk2+tkip+ccmp")
      || 0 == strcmp(ptmp,"psk+tkip+ccmp")
      || 0 == strcmp(ptmp,"psk+psk2")){
        strcpy(param, (char *)WPAEncryptionModes[2][1]);
    }
    else if(0 == strcmp(ptmp,"psk2+ccmp") || 0 == strcmp(ptmp,"psk+ccmp")){
        strcpy(param, (char *)WPAEncryptionModes[1][1]);
    }
    else if(0 == strcmp(ptmp,"psk2+tkip") || 0 == strcmp(ptmp,"psk+tkip")){
        strcpy(param, (char *)WPAEncryptionModes[0][1]);
    }
    else{
        //strcpy(param, (char *)WPAEncryptionModes[2][1]);
        strcpy(param, "None");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WPAEncryptionModes(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    if(value == NULL){
        return FAULT_CODE_9007;
    }
    int num = uci_get_wifi_num();
    int index = get_parameter_index((char *)name, "WLANConfiguration.", 65535);
    if(index > num){
        return FAULT_CODE_9005;
    }
    char ena[32];
    if(0 == strcmp((char *)WPAEncryptionModes[0][1], value)){
        strcpy(ena,"psk2+tkip");
    }
    else if(0 == strcmp((char *)WPAEncryptionModes[1][1], value)){
        strcpy(ena,"psk2+ccmp");
    }
    else if(0 == strcmp((char *)WPAEncryptionModes[2][1], value)){
        strcpy(ena,"psk2+tkip+ccmp");
    }
    else if(0 == strcmp("None", value)){
        strcpy(ena,"none");
    }
    else{
        return FAULT_CODE_9007;
    }
    char cmdbuf[256];
    sprintf(cmdbuf,"uci -q set wireless.@wifi-iface[%d].encryption=%s;uci -q commit wireless",index - 1,ena);
    system(cmdbuf);
    cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);

    return FAULT_CODE_OK;
}

int cpe_get_igd_WMMEnable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name,"wmm",&ptmp);
    if(FAULT_CODE_OK != res){
        return res;
    }
    if(0 == strcmp(ptmp, "1")){
        strcpy(param, "true");
    }
    else{
        strcpy(param, "false");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_WMMEnable(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    int inum;
    int index;
    inum = uci_get_wifi_num();
    index = get_parameter_index((char *)name, "WLANConfiguration.", 65535);
    if(index > inum){
        return FAULT_CODE_9005;
    }
    int wmm = 0;
    if(0 == strcmp(value, "true")){
        wmm = 1;
    }
    else if(0 == strcmp(value, "false")){
        wmm = 0;
    }
    else{
        return FAULT_CODE_9007;
    }
    char cmdbuf[256];
    sprintf(cmdbuf, "uci -q set wireless.@wifi-iface[%d].wmm=%d;uci -q commit wireless",index - 1, wmm);
    system(cmdbuf);
    cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_refresh_igd_MultiSSID(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    cwmp_refresh_i_parameter(cwmp, param_node, 4);
    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 

    return FAULT_CODE_OK;
}

int cpe_get_igd_NameOfWLANConfiguration(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    return uci_get_wifi_param(name, "ifname", value);
}

int cpe_get_InsecureOOBAccessEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "false");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_BeaconAdvertisementEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "true");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_TransmitPowerSupported(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "0-100");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_TransmitPower(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name,"device",&ptmp);
    if(FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf,"uci -q get wireless.%s.txpower", ptmp);
    read_memory(cmdbuf, param, 64);
    util_strip_traling_spaces(param);
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_TransmitPower(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name,"device",&ptmp);
    if(FAULT_CODE_OK != res)
    {
        return res;
    }
    char cmdbuf[64];
    sprintf(cmdbuf,"uci -q get wireless.%s.txpower=%s;uci -q commit wireless", ptmp, value);
    system(cmdbuf);
    cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_ChannelsInUse(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    char *ptmp = NULL;
    int res = uci_get_wifi_param(name,"device",&ptmp);
    if(FAULT_CODE_OK != res){
        return res;
    }
    char cmdbuf[64];
    char band[8];
    //char country[8];
    sprintf(cmdbuf, "uci -q get wireless.%s.band", ptmp);
    read_memory(cmdbuf, band, sizeof(band)); 
    util_strip_traling_spaces(band);
    /*
    sprintf(cmdbuf, "uci -q get wireless.%s.country", ptmp);
    read_memory(cmdbuf, country, sizeof(country)); 
    util_strip_traling_spaces(country);
    */
    if(0 == strcmp(band,"5G")){
        strcpy(param, "36,40,44,48,52,56,60,64,149,153,157,161,165");
    }
    else{
        strcpy(param, "1-13");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_DeviceOperationMode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "WirelessStation");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_AuthenticationServiceMode(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "None");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_WMMSupported(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "true");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_UAPSDSupported(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "true");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_igd_UAPSDEnable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "true");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_EnableOfWPS(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    get_single_config_attr("WPS_ENABLE", param);
    if(!strcmp(param, "1"))
        strcpy(param, "true");
    else
        strcpy(param, "false");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_EnableOfWPS(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    if(!strcmp(value, "true"))
    {
        set_single_config_attr("WPS_ENABLE", "1");
        set_single_config_attr("TZ_WPS_IS_ENABLE", "yes");
    }
    else
    {
        set_single_config_attr("WPS_ENABLE", "0");
        set_single_config_attr("TZ_WPS_IS_ENABLE", "no");
    }
    return FAULT_CODE_OK;
}

int cpe_get_DeviceName(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index;

    index = get_parameter_index((char *)name, "WLANConfiguration.", igd_entries.wlan_entry);
    if(index == 1)
    	get_single_config_attr("AP_SSID", param);
    else if(index == 2)
    	get_single_config_attr("AP_SSID_3", param);
    else if(index == 3)
    	get_single_config_attr("AP_SSID_4", param);
    else
    {
    	*value = NULL;
    	return FAULT_CODE_9005;
    }
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_DevicePassword(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "0");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_DevicePassword(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    set_single_config_attr("TZ_CLIENT_PIN", value);

    return FAULT_CODE_OK;
}

int cpe_get_VersionOfWPS(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "1.0");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_ConfigMethodsSupported(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "Display,PushButton,Keypad");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_ConfigMethodsEnabled(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    get_single_config_attr("TZ_WPS_MODE", param);
    if(!strcmp(param, "pbc"))
        strcpy(param, "PushButton");
    else if(!strcmp(param, "client_pin"))
        strcpy(param, "Display");
    else
        strcpy(param, "Display");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_set_ConfigMethodsEnabled(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    if(!strcmp(value, "PushButton"))
        set_single_config_attr("TZ_WPS_MODE", "pbc");
    else if(!strcmp(value, "Display"))
        set_single_config_attr("TZ_WPS_MODE", "client_pin");

    return FAULT_CODE_OK;
}

int cpe_get_SetupLockedState(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "PINRetryLimitReached");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_ConfigurationState(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "Configured");
    *value = param;

    return FAULT_CODE_OK;
}

int cpe_get_LastConfigurationError(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "NoError");
    *value = param;

    return FAULT_CODE_OK;
}

static int wps_device_maxnum = 0;
static int pthread_tmp_wps_device_maxnum = 0;

#define MAXWPSENTRY 16
int count_char_number(char *string, char c)
{
	int ncount = 0;
	char *p = string;
	 while ((p = strchr(p, c)))
    {
        ncount = ncount + 1; 
        p = p + 1;    
        if (*p == '\0')
        {
            break;
        }

    }
	 return ncount;
}

int read_wps_device_name_in_str(WPSRegistrar *read_wps_device_info, char *param, int ncount)
{
    char *p;
    char *p2;
	unsigned int len=0;
	unsigned int count=0;
	p2 = param;
	while((count != ncount) && (*p2 != '\0')) {
		p = strchr(p2,',');
		if (p) {
            len = p-p2;
			if (len > 64) {
				break;
			}
            strncpy(read_wps_device_info[count].DeviceName,p2,len);
            read_wps_device_info[count].DeviceName[len] = '\0';
            count++;
            p2 = p + 1;
		} else {
			break;
		}
	}
	return 0;
}
int read_wps_device_uuid_in_str(WPSRegistrar *read_wps_device_info, char *param, int ncount)
{
    char *p;
    char *p2;
	unsigned int len=0;
	unsigned int count=0;
	p2 = param;
	while((count != ncount) && (*p2 != '\0')) {
		p = strchr(p2,',');
		if (p) {
            len = p-p2;
			if (len > 64) {
				break;
			}
            strncpy(read_wps_device_info[count].UUID,p2,len);
            read_wps_device_info[count].UUID[len] = '\0';
			strcpy(read_wps_device_info[count].Enable,"true");
            count++;
            p2 = p + 1;
		} else {
			break;
		}
	}
	return 0;
}
int read_wps_device_mac_in_str(WPSRegistrar *read_wps_device_info, char *param, int ncount)
{
    char *p;
    char *p2;
	unsigned int len=0;
	unsigned int count=0;
	p2 = param;
	while((count != ncount) && (*p2 != '\0')) {
		p = strchr(p2,',');
		if (p) {
            len = p-p2;
			if (len > 64) {
				break;
			}
            strncpy(read_wps_device_info[count].DeviceMac,p2,len);
            read_wps_device_info[count].DeviceMac[len] = '\0';
			//cwmp_log_debug(">>>%s<<<",wps_device[count].DeviceMac);
            count++;
            p2 = p + 1;
		} else {
			break;
		}
	}
	return 0;
}


int read_wps_registrar_info(WPSRegistrar *read_wps_device_info)
{
	char buffer[128] = {0};
	char shellcmd[128] = {0};
	unsigned int ncount = 0;
	strcpy(shellcmd, "hostapd_cli -i ath0 all_sta | grep  wpsDeviceName | wc -l");
	read_memory(shellcmd, buffer, sizeof(buffer));
	if (strlen(buffer) == 0) {
		igd_entries.wps_device_entry = 0;
		memset(read_wps_device_info, 0, sizeof(read_wps_device_info));
		return -1;
	}
	igd_entries.wps_device_entry = atoi(buffer);//The number of current online devices
	if (igd_entries.wps_device_entry == 0) {
		igd_entries.wps_device_entry = 0;
		memset(read_wps_device_info, 0, sizeof(read_wps_device_info));
		return -1;
	}
	if (igd_entries.wps_device_entry > MAXWPSENTRY) {
		igd_entries.wps_device_entry = MAXWPSENTRY;
	}
	memset(buffer, 0, sizeof(buffer));
	strcpy(shellcmd, "hostapd_cli -i ath0 all_sta | grep wpsDeviceName | awk -F= \'{print $2}\' | sed \':a;N;$!ba;s/\\n/,/g\'");
	read_memory(shellcmd, buffer, sizeof(buffer));
	util_strip_traling_spaces(buffer);
	buffer[strlen(buffer)] = ',';
	buffer[strlen(buffer) + 1] = '\0';
	ncount = count_char_number(buffer,',');
	if ( ncount != igd_entries.wps_device_entry) {
		memset(read_wps_device_info, 0, sizeof(read_wps_device_info));
		return -1;
	} else {
		read_wps_device_name_in_str(read_wps_device_info, buffer, ncount);
		memset(buffer, 0, sizeof(buffer));
		strcpy(shellcmd, "hostapd_cli -i ath0 all_sta | grep wpsUuid | awk -F= \'{print $2}\' | sed \':a;N;$!ba;s/\\n/,/g\'");
		read_memory(shellcmd, buffer, sizeof(buffer));
		util_strip_traling_spaces(buffer);
		buffer[strlen(buffer)] = ',';
		buffer[strlen(buffer) + 1] = '\0';
		ncount = count_char_number(buffer,',');
		if ( ncount != igd_entries.wps_device_entry) {
			memset(read_wps_device_info, 0, sizeof(read_wps_device_info));
			return -1;
		} else {
			read_wps_device_uuid_in_str(read_wps_device_info, buffer, ncount);
			memset(buffer, 0, sizeof(buffer));
			strcpy(shellcmd, "hostapd_cli -i ath0 all_sta | grep dot11RSNAStatsSTAAddress | awk -F= \'{print $2}\' | sed \':a;N;$!ba;s/\\n/,/g\'");
			read_memory(shellcmd, buffer, sizeof(buffer));
			util_strip_traling_spaces(buffer);
			buffer[strlen(buffer)] = ',';
			buffer[strlen(buffer) + 1] = '\0';
			ncount = count_char_number(buffer,',');
			if ( ncount != igd_entries.wps_device_entry) {
				memset(read_wps_device_info, 0, sizeof(read_wps_device_info));
				return -1;
			} else {
				read_wps_device_mac_in_str(read_wps_device_info, buffer, ncount);
				return 0;
			}
		}
	}
}
int merge_wps_device_info(WPSRegistrar *wps_device, int max,WPSRegistrar *read_wps_device_info, int current)
{
	int i = 0;
	int index = 0;
	int tmpmax = max;
	char macthindex[max];
	memset(macthindex,-1,max);
	for (index = 0; index < current; index++) {
		
		for (i = 0; i < tmpmax; i++) {
			if (macthindex[i] == i) {
					continue;
				}
			if (0 == strcmp(wps_device[i].UUID, read_wps_device_info[index].UUID)) {
				macthindex[i] = i;
				break;
			}
		}
		
		if (i == tmpmax) {
			strcpy(wps_device[max].DeviceMac,read_wps_device_info[index].DeviceMac);
			strcpy(wps_device[max].DeviceName,read_wps_device_info[index].DeviceName);
			strcpy(wps_device[max].UUID,read_wps_device_info[index].UUID);
			strcpy(wps_device[max].Enable,read_wps_device_info[index].Enable);
			max++;
			if (max > MAXWPSRegistrar) {
				max = MAXWPSRegistrar;
			}
		}
		
	}
	return max;
}
int make_wps_associated_table_lists()
{
	int ret = 0;
	int index = 0;
	WPSRegistrar read_wps_device_info[MAXWPSENTRY];
	ret = read_wps_registrar_info(read_wps_device_info);
	if (ret == -1) {//no device
		return -1;
	}
	if (pthread_tmp_wps_device_maxnum == 0) {//Parameter initialization
		pthread_tmp_wps_device_maxnum = igd_entries.wps_device_entry;
		for (index = 0; index < pthread_tmp_wps_device_maxnum; index++) {
			strcpy(wps_device[index].DeviceMac,read_wps_device_info[index].DeviceMac);
			strcpy(wps_device[index].DeviceName,read_wps_device_info[index].DeviceName);
			strcpy(wps_device[index].UUID,read_wps_device_info[index].UUID);
			strcpy(wps_device[index].Enable,read_wps_device_info[index].Enable);
		}
		return pthread_tmp_wps_device_maxnum;
	}
	return merge_wps_device_info(wps_device, pthread_tmp_wps_device_maxnum, read_wps_device_info, igd_entries.wps_device_entry);	
}

#ifdef  PERIODREADDEVICE
#define REFRESH_PREIOD		3
static pthread_mutex_t refresh_device_lock;
pthread_mutex_t refresh_linkrate_lock;
float uplink, downlink, uplink_max, uplink_min, downlink_max, downlink_min;
void *refresh_device_info_porcess(void)
{
	int tmpmax = 0;
	unsigned int times = 0;
	char rx_buffer[64], tx_buffer[64];
	unsigned long rx_bytes = 0, tx_bytes = 0, new_bytes;
	
	while(1) {
		if(times % (60 / REFRESH_PREIOD) == 0)
		{
			pthread_mutex_lock(&refresh_device_lock);
			tmpmax = make_wps_associated_table_lists();
			if (tmpmax >= 0) {
				pthread_tmp_wps_device_maxnum = tmpmax;
			}
			wps_device_maxnum = pthread_tmp_wps_device_maxnum;
			pthread_mutex_unlock(&refresh_device_lock);
		}

		pthread_mutex_lock(&refresh_linkrate_lock);
		cmd_netdev_flowstat_ex(util_get_wan_interface_name(), rx_buffer, tx_buffer, NULL, NULL);	
		new_bytes = atol(rx_buffer);
		downlink = (new_bytes - rx_bytes) * 1.0 / REFRESH_PREIOD;
		rx_bytes = new_bytes;
		new_bytes = atol(tx_buffer);
		uplink = (new_bytes - tx_bytes) * 1.0 / REFRESH_PREIOD;
		tx_bytes = new_bytes;
		if(downlink_min - 0 < 1e-6 || downlink < downlink_min)
			downlink_min = downlink;
		if(downlink_max - 0 < 1e-6 || downlink > downlink_max)
			downlink_max = downlink;
		if(uplink_min - 0 < 1e-6 || uplink < uplink_min)
			uplink_min = uplink;
		if(uplink_max - 0 < 1e-6 || uplink > uplink_max)	
			uplink_max = uplink;
		pthread_mutex_unlock(&refresh_linkrate_lock);

		sleep(REFRESH_PREIOD);
		times++;
	}
	
}
void refresh_device_info_porcess_thread_init(void)
{
	int ret;
	pthread_t refresh;
	pthread_mutex_init(&refresh_device_lock,NULL);
	ret = pthread_create(&refresh, NULL, (void *)refresh_device_info_porcess, NULL);
	if(ret!=0)
		cwmp_log_debug("refresh_device_info_porcess_thread failed\n");
}

#endif


int  cpe_refresh_igd_WPSRegistrar(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
	cwmp_refresh_i_parameter(cwmp, param_node, wps_device_maxnum);
	cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 

	return FAULT_CODE_OK;
}

int cpe_set_WPSRegistrarEnable(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	int index = get_parameter_index((char *)name, "Registrar.", wps_device_maxnum);
	char shellcmd[64] = {0};
	
	if (strcmp(value,"true") == 0) {
		sprintf(shellcmd, "hostapd_cli new_sta %s | sed -n \'2p\'| grep OK",wps_device[index - 1].DeviceMac);
		if (read_memory(shellcmd, param, sizeof(param)) == 0) {
			if (strlen(param) != 0) {
				strcpy(wps_device[index - 1].Enable, "true");
				return FAULT_CODE_OK;
			}
		}
		
	} else if (strcmp(value,"false") == 0) {
		sprintf(shellcmd, "hostapd_cli disassociate %s | sed -n \'2p\'| grep OK",wps_device[index - 1].DeviceMac);
		if (read_memory(shellcmd, param, sizeof(param)) == 0) {
			if (strlen(param) != 0) {
				strcpy(wps_device[index - 1].Enable, "false");
				return FAULT_CODE_OK;
			}
		}
	} else {
		return FAULT_CODE_9007;
	}

   return FAULT_CODE_9007;
}
int cpe_get_RegistrarNumberOfEntries(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int tmpmax = 0;
	int ret = 0;
#ifdef  PERIODREADDEVICE
	ret = pthread_mutex_trylock(&refresh_device_lock);
	if (ret == 0) {
#endif
		tmpmax = make_wps_associated_table_lists();
		if (tmpmax >= 0) {
			pthread_tmp_wps_device_maxnum = tmpmax;
		}
		wps_device_maxnum = pthread_tmp_wps_device_maxnum;
#ifdef  PERIODREADDEVICE
		pthread_mutex_unlock(&refresh_device_lock);
	}
#endif
	sprintf(param, "%d",wps_device_maxnum);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
		
	return FAULT_CODE_OK;
}

int cpe_get_RegistrarEstablishedOfWPS(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "false");
	*value = param;
	
	return FAULT_CODE_OK;

}

int cpe_get_WPSRegistrarEnable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index = get_parameter_index((char *)name, "Registrar.", wps_device_maxnum);
	sprintf(param, "%s", wps_device[index - 1].Enable);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	
	return FAULT_CODE_OK;

}

int cpe_get_WPSRegistrarUUID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index = get_parameter_index((char *)name, "Registrar.", wps_device_maxnum);
	sprintf(param, "%s", wps_device[index - 1].UUID);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	
	return FAULT_CODE_OK;

}

int cpe_get_WPSRegistrarDeviceName(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int index = get_parameter_index((char *)name, "Registrar.", wps_device_maxnum);
	sprintf(param, "%s", wps_device[index - 1].DeviceName);
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	
    return FAULT_CODE_OK;
}

int cpe_add_igd_tr_MultiSSID(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
    if(!param_node)
    {
        return FAULT_CODE_9002;
    }

    int num = 4;
    int i;

    cwmp_log_info("cpe_add_igd_tr_MultiSSID node=%s",param_node->name);
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

int cpe_get_igd_MultiSSIDEnable(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	strcpy(param, "Enable");
	*value = param;
	return FAULT_CODE_OK;
}

int cpe_set_igd_MultiSSIDEnable(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	return FAULT_CODE_OK;
}

int cpe_get_igd_EnableOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int res = uci_mul_get_wifi_param(name, "disabled", value);
    if(FAULT_CODE_OK != res){
        return res;
    }
    if(0 == strcmp(*value, "1")){
    	strcpy(param, "false");
    }
    else{
    	strcpy(param, "true");
    }
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_EnableOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	char nv_name[64] = {0};
	if(!strcmp(value, "1")) {
        strcpy(param, "1");
		cwmp_log_debug("[tr069][wifi]User enable WIFI swtich! %s\n",param);
    } else if(!strcmp(value, "0")) {
        strcpy(param, "0");
		cwmp_log_debug("[tr069][wifi]User disable WIFI swtich! %s\n",param);
    } else {
    	return FAULT_CODE_OK;
    }
	int res = uci_mul_set_wifi_param(name, "disabled", param);

    cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);

    return FAULT_CODE_OK;
}

int cpe_get_igd_SSIDOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int res = uci_mul_get_wifi_param(name, "ssid", value);
    if(FAULT_CODE_OK != res){
        return res;
    }
    return FAULT_CODE_OK;
}

int cpe_set_igd_SSIDOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    printf("jiangyibo uu\n");
    int res = uci_mul_set_wifi_param(name, "ssid", value);
    printf("jiangyibo uu22\n");
        cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_PasswordOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int res = uci_mul_get_wifi_param(name, "key", value);
    if(FAULT_CODE_OK != res){
        return res;
    }
    return FAULT_CODE_OK;
}

int cpe_set_igd_PasswordOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	if (value == NULL || strlen(value) == 0) {
		return FAULT_CODE_OK;
	}
    if(strlen(value) >= 8 && strlen(value) <= 63) {
        int res = uci_mul_set_wifi_param(name, "key", value);
        if(FAULT_CODE_OK != res){
            return res;
        }
    }
        cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);

    return FAULT_CODE_OK;
}
int cpe_get_igd_EncrypModeOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	int res = uci_mul_get_wifi_param(name, "encryption", value);
    if(FAULT_CODE_OK != res){
        return res;
    }
/*	
	if (!strcmp(buffer, "OPEN")) {
		
		strcpy(param, "OPEN");
	} else if (!strcmp(buffer, "WPA2PSK")){
	
		strcpy(param, "WPA2-PSK[AES]");
		
	} else if (!strcmp(buffer, "WPAPSKWPA2PSK")){
	
		strcpy(param, "WPA-PSK/WPA2-PSK");
	} else {
		*value = NULL;
    	return FAULT_CODE_OK;
	}
    *value = param;
*/
    printf("jiangyibo 111\n");
    return FAULT_CODE_OK;
}

int cpe_set_igd_EncrypModeOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	
	int ret = -1;
	int res = uci_mul_set_wifi_param(name, "encryption", value);
    if(FAULT_CODE_OK != res){
        return res;
    }
        cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);
	return FAULT_CODE_OK;
}

int cpe_get_igd_NATSwitchOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{

	strcpy(param, "OFF");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_NATSwitchOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}

int cpe_get_igd_DHCPSwitchOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    printf("jiangyibo jj \n");
    int res = uci_mul_get_dhcp_param(name, "ignore", value);
    if(*value == NULL)
    {
          strcpy(param, "ON");
          *value = param;
        return FAULT_CODE_OK;
    }
    printf("jiangyibo jj %s\n",*value);
    if(FAULT_CODE_OK != res){
        return res;
    }
    if(0 == strcmp(*value, "1")){
    	strcpy(param, "OFF");
    }
    else{
    	strcpy(param, "ON");
    }
    *value = param;
    return FAULT_CODE_OK;

}

int cpe_set_igd_DHCPSwitchOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	char nv_name[16] = {0};
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
	index++;
	sprintf(nv_name,"tz_dhcp%d_enable", index);
	if (strlen(value) == 0 || strcmp(value,"1") == 0) {
		nv_cfg_set(nv_name, "1");
	} else {
		nv_cfg_set(nv_name, "0");
	}
        cmd_touch(REBOOT_WIFI_MODULE);
    callback_reg(cwmp, restart_wifi, NULL, NULL);
    return FAULT_CODE_OK;
}

int cpe_get_igd_IPAddressOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int res = uci_mul_get_ipaddr_param(name, "dhcp_option", value);
   
    return FAULT_CODE_OK;
}

int cpe_set_igd_IPAddressOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    
    return FAULT_CODE_OK;
}
int cpe_get_igd_DHCPLeaseTimeOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int res = uci_mul_get_dhcp_param(name, "leasetime", value);
   
    return FAULT_CODE_OK;
}

int cpe_set_igd_DHCPLeaseTimeOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
        int res = uci_mul_set_dhcp_param(name, "leasetime", value);
   return FAULT_CODE_OK;
}

int cpe_set_igd_MinAddressOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    int res = uci_mul_set_dhcp_param(name, "start", value);
    return FAULT_CODE_OK;
}
int cpe_get_igd_MinAddressOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
        int res = uci_mul_get_dhcp_param(name, "start", value);

    return FAULT_CODE_OK;
}

int cpe_set_igd_MaxAddressOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    int res = uci_mul_set_dhcp_param(name, "limit", value);
    return FAULT_CODE_OK;
}

int cpe_get_igd_MaxAddressOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int res = uci_mul_get_dhcp_param(name, "limit", value);
    return FAULT_CODE_OK;
}

int cpe_set_igd_SubnetMaskOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
        int res = uci_mul_set_dhcp_param(name, "netmask", value);
    return FAULT_CODE_OK;
}

int cpe_get_igd_SubnetMaskOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int res = uci_mul_get_dhcp_param(name, "netmask", value);
    return FAULT_CODE_OK;
}

int cpe_set_igd_GatewayOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}

int cpe_get_igd_GatewayOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
        strcpy(param, "");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_FirstDNSOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}

int cpe_get_igd_FirstDNSOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    int i, j;
	int count;
	char cmd[128];
	char buffer[128];

    int res = uci_mul_get_dhcp_param(name, "dhcp_option", value);
    return FAULT_CODE_OK;
}
//P25 No second dns
int cpe_set_igd_SecondDNSOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}

int cpe_get_igd_SecondDNSOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "");
    *value = param;
    return FAULT_CODE_OK;
}
//P25 No second dns

int cpe_set_igd_APNSwitchOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	char nv_name[16] = {0};
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
	index++;// 2 3
	sprintf(nv_name,"tz_apn%d_enable", index);
	if (strlen(value) != 0) {
		nv_cfg_set(nv_name, value);
	}
    return FAULT_CODE_OK;
}

int cpe_get_igd_APNSwitchOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
    strcpy(param, "0");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_ConfigNameOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	char nv_name[16] = {0};
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
	index++;// 2, 3
	sprintf(nv_name,"apn%d_profile_name", index);
	if (strlen(value) != 0) {
		nv_cfg_set(nv_name, value);
	}
    return FAULT_CODE_OK;
}

int cpe_get_igd_ConfigNameOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char nv_name[16] = {0};
	int ret = -1;
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
    strcpy(param, "");
    *value = param;
    return FAULT_CODE_OK;

}

int cpe_set_igd_APNNameOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	char nv_name[16] = {0};
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
	index++;// 2, 3
	sprintf(nv_name,"apn%d_wan", index);
	if (strlen(value) != 0) {
		nv_cfg_set(nv_name, value);
	}
    return FAULT_CODE_OK;
}

int cpe_get_igd_APNNameOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char nv_name[16] = {0};
	int ret = -1;
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
    strcpy(param, "");
    *value = param;
    return FAULT_CODE_OK;

}

int cpe_set_igd_UsernameOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	char nv_name[16] = {0};
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
	index++;// 2, 3
	sprintf(nv_name,"apn%d_username", index);
	if (strlen(value) != 0) {
	   //	nv_cfg_set(nv_name, value);

	}
    return FAULT_CODE_OK;
}

int cpe_get_igd_UsernameOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char nv_name[16] = {0};
	int ret = -1;
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
    strcpy(param, "");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_APNPasswordOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	char nv_name[16] = {0};
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
	index++;// 2, 3
	sprintf(nv_name,"apn%d_passwd", index);
	if (strlen(value) != 0) {
		nv_cfg_set(nv_name, value);
	}
    return FAULT_CODE_OK;
}

int cpe_get_igd_APNPasswordOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char nv_name[16] = {0};
	int ret = -1;
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
    strcpy(param, "");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_set_igd_PDPTypeOfMultiSSID(cwmp_t *cwmp, const char *name, const char *value, int length, callback_register_func_t callback_reg)
{
	char nv_name[16] = {0};
	char nv_value[16] = {0};
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
	index++;// 2, 3
	sprintf(nv_name,"apn%d_type", index);
	if (strlen(value) != 0) {
		switch (atoi(value)) {
			case 1:
				strcpy(nv_value, "IP");
				break;
			case 2:
				strcpy(nv_value, "IPv6");
				break;
			case 3:
				strcpy(nv_value, "IPv4v6");
				break;
			default:
				cwmp_log_debug("Set PDP type error! value:%s\n", value);
				return FAULT_CODE_9002;
				break;
		}
		nv_cfg_set(nv_name, nv_value);
	}
    return FAULT_CODE_OK;
}

int cpe_get_igd_PDPTypeOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char nv_name[16] = {0};
	int ret = -1;
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
    strcpy(param, "");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_ApnWanIpOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char nv_name[16] = {0};
	int ret = -1;
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
    strcpy(param, "");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_ApnPrimaryDNSOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char nv_name[16] = {0};
	int ret = -1;
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
    strcpy(param, "");
    *value = param;
    return FAULT_CODE_OK;
}

int cpe_get_igd_ApnSecondDNSOfMultiSSID(cwmp_t * cwmp, const char * name, char ** value, pool_t * pool)
{
	char nv_name[16] = {0};
	int ret = -1;
	int index = get_parameter_index((char *)name, "MultiSSID.", MAX_MULTI_SSID_COUNT);
    strcpy(param, "");
    *value = param;
    return FAULT_CODE_OK;
}



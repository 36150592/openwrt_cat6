#include "include.h"

#define DUMPLEASES "dumpleases"
#define UDPCPD_LEASE_PATH "/etc_rw/udhcpd.leases"


/*
root@cpe5mod:/ # busybox dumpleases -f /var/dhcp/udhcpd.leases
Mac Address 	  IP Address	  Host Name 		  Expires in
56:67:5b:0e:d8:a3 192.168.3.100   whitewin7 		  07:15:34
84:73:03:ed:b9:8b 192.168.3.101   android-7d71f854c12 07:54:21
cc:08:8d:1a:7a:19 192.168.3.102   junjiandeiPhone	  07:57:52
*/

static int find_client_list_dum(TZ_UDHCPD_LEASE_T* c_udhcpd_lease)
{
	int i;
	char buffer[MAX_FILE_BUFFER_SIZE], shellcmd[MINI_FILE_BUFFER_SIZE];
       char tmp_values[MINI_FILE_BUFFER_SIZE];
	char *pHead, *pTail;
	char *pnextstart;
	
	pHead = NULL;
	pTail = NULL;

	print("[find_client_list_dum]");
	memset(c_udhcpd_lease, 0, sizeof(TZ_UDHCPD_LEASE_T));

	sprintf(shellcmd, "%s -f %s | grep -v \"Mac Address\"", DUMPLEASES, UDPCPD_LEASE_PATH);

	if (execute_cmd(shellcmd, buffer, sizeof(buffer)) != 0) {
		return -1;
	}
	pHead = strstr(buffer, ":");
	if (pHead == NULL) {
		return -2;
	}

	pHead = buffer;
	pnextstart = NULL;
	i = 0;
	do{
		pTail = strstr(pHead, "\n");  //一定是"\n"为结束标识
	    if(pTail != NULL) {
			memset(tmp_values, 0, sizeof(tmp_values));
			memcpy(tmp_values, pHead, pTail-pHead + 1);
			//sscanf(tmp_values, "%s%s%s%s", values[0][i], values[1][i], values[2][i], values[3][i]);
			pnextstart = get_value_by_end(tmp_values, c_udhcpd_lease->lease[i].mac, " ");
			pnextstart = get_value_by_end(pnextstart, c_udhcpd_lease->lease[i].ipaddr, " ");
			pnextstart = get_value_by_end(pnextstart, c_udhcpd_lease->lease[i].hostname, " ");
			pnextstart = get_value_by_end(pnextstart, c_udhcpd_lease->lease[i].expires, "\n");

			print("i(%d): ip(%s) mac(%s) hostname(%s) expires(%s)", i, c_udhcpd_lease->lease[i].ipaddr,
				c_udhcpd_lease->lease[i].mac, c_udhcpd_lease->lease[i].hostname, c_udhcpd_lease->lease[i].expires);
			
			pHead = pTail + 1;
			i++;

			// rm ipv6 ip
			if(strlen(c_udhcpd_lease->lease[i].ipaddr) > 24 || strstr(c_udhcpd_lease->lease[i].ipaddr,":"))
			{
				memset(&c_udhcpd_lease->lease[i], 0, sizeof(UDHCPD_LEASE));
				continue;
			}
			
			c_udhcpd_lease->cnt++;
	    }
	}while(pTail != NULL);

	ll_write_file_data(FL_TZ_UDHCPD_LEASES, c_udhcpd_lease, sizeof(TZ_UDHCPD_LEASE_T));

	return 0;
}


/*	
root@cpe5mod:/proc/wlan0 # cat sta_info | grep hwaddr:
		hwaddr: 847303edb98b
		hwaddr: cc088d1a7a19
*/

static void get_wireless_mac_list(MAC_LIST* mac_list)
{
	char cmd_buff[256] = {0};
	char tmp_buff[NORMAL_FILE_BUFFER_SIZE] = {0};
	SPLIT_STRUCT mainStrings[MINI_ARRAY_SIZE];
	int item_count = 0, m = 0, i = 0, fix_len = 0, len = 0;
	char *p = NULL;
	char mactmp[NORMAL_FIELD_LEN];
	LAN_ITEM_T litmp;

	print("[get_wireless_client]");	
	memset(mac_list, 0, sizeof(MAC_LIST));

	sprintf(cmd_buff, "cat %s | grep hwaddr:", FL_WIRELESS_SSID1_STA);

	if(execute_cmd(cmd_buff, tmp_buff, sizeof(tmp_buff)-1) == 0)
	{
		fix_len = strlen("hwaddr: ");
		item_count = StringSplit(tmp_buff, "\n", mainStrings);
		for(i = 0; i < item_count; i ++)
		{
			if (mac_list->cnt > MINI_ARRAY_SIZE)
				break;
				
			p = strstr(mainStrings[i].lp_string, "hwaddr: ");
			if (p == NULL)
				break;
			memset(mactmp, 0, sizeof(mactmp));

			len = mainStrings[i].length - (p - mainStrings[i].lp_string + fix_len);
			memcpy(mactmp, p+fix_len, (len > NORMAL_FIELD_LEN-1)?(NORMAL_FIELD_LEN-1):len);
			format_mac(mactmp, mac_list->mac_item[mac_list->cnt].mac);

			print("i(%d): wlmac(%s) - (%s)", i, mactmp, mac_list->mac_item[mac_list->cnt].mac);
			mac_list->cnt++;
		}
	}
}

static void get_eth_mac_list(MAC_LIST* wireless_mac_list, MAC_LIST* eth_mac_list, MAC_IP_LIST* ip_neigh_list)
{
	int i = 0, j = 0, k = 0;
	memset(eth_mac_list, 0, sizeof(MAC_LIST));

	for(i = 0; i < ip_neigh_list->cnt; i++)
	{
		for(j = 0; j < wireless_mac_list->cnt; j++)
		{
			if(strcmp(ip_neigh_list->mac_ip_item[i].mac, wireless_mac_list->mac_item[j].mac) == 0)
				break;
		}
		
		if(j != wireless_mac_list->cnt)
			continue;

		for(k = 0; k < eth_mac_list->cnt; k++)
		{
			if(strcmp(eth_mac_list->mac_item[i].mac, ip_neigh_list->mac_ip_item[i].mac) == 0)
			{
				print("%s has exist, drop it\n", ip_neigh_list->mac_ip_item[i].mac);
				break;
			}
		}
		
		if(k != eth_mac_list->cnt)
			continue;
		
		strcpy(eth_mac_list->mac_item[eth_mac_list->cnt].mac, ip_neigh_list->mac_ip_item[i].mac);
		eth_mac_list->cnt++;
	}

	
#if 0
	strcpy(cmd_buff, "arp -i br0 | awk '{print $4}'");
	if(execute_cmd(cmd_buff, tmp_buff, sizeof(tmp_buff)-1) == 0)
	{
		item_count = StringSplit(tmp_buff, "\n", mainStrings);
		for(i = 0; i < item_count; i ++)
		{
			if(mainStrings[i].length)
			{
				char arp_mac_item[24] = "";
				strncpy(arp_mac_item, mainStrings[i].lp_string, mainStrings[i].length);
				print(">>>>arp_mac_item = %s\n", arp_mac_item);
				if(strstr(arp_mac_item, ":") == NULL)
				{
					print("%s is no right mac, drop it\n", arp_mac_item);
					continue;
				}
				for(j = 0; j < wireless_mac_list->cnt; j++)
				{
					if(strcmp(arp_mac_item,wireless_mac_list->mac_item[j].mac) == 0)
						break;
				}

				if(j == wireless_mac_list->cnt)
				{
					for(k = 0; k < eth_mac_list->cnt; k++)
					{
						if(strcmp(arp_mac_item, eth_mac_list->mac_item[k].mac) == 0)
							break;
					}
					
					if(k == eth_mac_list->cnt)
					{
						strcpy(eth_mac_list->mac_item[eth_mac_list->cnt].mac, arp_mac_item);
						eth_mac_list->cnt++;
					}
					else
					{
						print("%s has exist, drop it\n", arp_mac_item);
					}
					
				}
			}
			
		}
	}
#endif
}



static void get_ip_neigh_list(MAC_IP_LIST* ip_neigh_list)
{
	char cmd[256] = {0};
	char tmp_buff[NORMAL_FILE_BUFFER_SIZE] = {0};
	int item_count = 0, i = 0, j = 0;
	SPLIT_STRUCT mainStrings[MINI_ARRAY_SIZE];

	memset(ip_neigh_list, 0, sizeof(MAC_IP_LIST));

	sprintf(cmd, "ip neigh list |grep -v FAILED| grep -v STALE| awk '{print $5 \" \" $1}'");
	if(execute_cmd(cmd, tmp_buff, sizeof(tmp_buff)-1) == 0)
	{
		item_count = StringSplit(tmp_buff, "\n", mainStrings);
	}

	for(i = 0; i < item_count; i ++)
	{
		if(mainStrings[i].length)
		{
			char item_mac[100] = "";
			char* item_ip = NULL;
			strncpy(item_mac, mainStrings[i].lp_string, mainStrings[i].length);
			item_ip = strstr(item_mac, " ");
			if(!item_ip)
				continue;
			*item_ip = 0; 
			item_ip++;

			// this is ipv6
			if(strlen(item_ip) > 24 || strstr(item_ip, ":"))
			{
				continue;
			}

			// check if mac is empty
			if( item_mac[0] == ' ' || item_mac[0] == 0)
				continue;

			for(j = 0; j < ip_neigh_list->cnt; j++)
			{
				if(strcmp(item_mac, ip_neigh_list->mac_ip_item[j].mac) == 0)
					break;
			}
			
			if(j == ip_neigh_list->cnt)
			{
				strcpy(ip_neigh_list->mac_ip_item[ip_neigh_list->cnt].mac, item_mac);
				strcpy(ip_neigh_list->mac_ip_item[ip_neigh_list->cnt].ip, item_ip);
				ip_neigh_list->cnt++;
			}
			
		}
	}
}


static void add_interface_to_lan_list(LAN_LIST_T* target_lan_list, TZ_UDHCPD_LEASE_T* all_udhcpd_lease, 
	MAC_LIST* mac_list, char* interface)
{
	int i = 0; 
	int j = 0;
	for(i = 0; i < mac_list->cnt; i++)
	{
		for(j = 0; j < all_udhcpd_lease->cnt; j++)
		{
			if(strcmp(mac_list->mac_item[i].mac, all_udhcpd_lease->lease[j].mac) == 0)
			{
				print(">>>>>%s: %s",interface, mac_list->mac_item[i].mac);
				strcpy(target_lan_list->list[target_lan_list->cnt].mac,    all_udhcpd_lease->lease[j].mac);
				strcpy(target_lan_list->list[target_lan_list->cnt].ipaddr,    all_udhcpd_lease->lease[j].ipaddr);
				strcpy(target_lan_list->list[target_lan_list->cnt].hostname,    all_udhcpd_lease->lease[j].hostname);
				strcpy(target_lan_list->list[target_lan_list->cnt].expires,    all_udhcpd_lease->lease[j].expires);
				strcpy(target_lan_list->list[target_lan_list->cnt].interface,    interface);
				target_lan_list->cnt++;
				break;
			}
		}
	}
}


static void fill_current_lan_list(LAN_LIST_T* target_lan_list, TZ_UDHCPD_LEASE_T* all_udhcpd_lease, 
	MAC_IP_LIST* ip_neigh_list, MAC_LIST* mac_list,  char* interface)
{
	int i = 0; 
	int j = 0;
	int k = 0;
	char tmp_ip_addr[64] = "";
	
	for(i = 0; i < mac_list->cnt; i++)
	{
		memset(tmp_ip_addr, 0, sizeof(tmp_ip_addr));
		for(j = 0; j < ip_neigh_list->cnt; j++)
		{
			if(strcmp(ip_neigh_list->mac_ip_item[j].mac, mac_list->mac_item[i].mac) == 0)
			{
				strcpy(tmp_ip_addr, ip_neigh_list->mac_ip_item[j].ip);
				break;
			}
		}

		for(k=0; k < all_udhcpd_lease->cnt; k++)
		{
			if(strcmp(all_udhcpd_lease->lease[k].mac, mac_list->mac_item[i].mac) == 0)
				break;
		}

		// has appear in dhcpd
		if(k != all_udhcpd_lease->cnt)
		{
			strcpy(target_lan_list->list[target_lan_list->cnt].mac,  all_udhcpd_lease->lease[k].mac);
			strcpy(target_lan_list->list[target_lan_list->cnt].hostname,    all_udhcpd_lease->lease[k].hostname);
			strcpy(target_lan_list->list[target_lan_list->cnt].expires,    all_udhcpd_lease->lease[k].expires);
			strcpy(target_lan_list->list[target_lan_list->cnt].interface,    interface);
			if(tmp_ip_addr[0])
				strcpy(target_lan_list->list[target_lan_list->cnt].ipaddr,    tmp_ip_addr);
			else
				strcpy(target_lan_list->list[target_lan_list->cnt].ipaddr,    all_udhcpd_lease->lease[k].ipaddr);
			target_lan_list->cnt++;
		}
		else
		{
			if(tmp_ip_addr[0])
			{
				strcpy(target_lan_list->list[target_lan_list->cnt].mac,  mac_list->mac_item[i].mac);
				strcpy(target_lan_list->list[target_lan_list->cnt].ipaddr,    tmp_ip_addr);
				strcpy(target_lan_list->list[target_lan_list->cnt].hostname,    "*");
				strcpy(target_lan_list->list[target_lan_list->cnt].expires,    "*");
				strcpy(target_lan_list->list[target_lan_list->cnt].interface,    interface);
				target_lan_list->cnt++;
			}
		}
		
	}
}



void get_current_lan_list(LAN_LIST_T* current_lan_list )
{
	TZ_UDHCPD_LEASE_T tz_dhcp_lease;
	MAC_LIST wireless_mac_list;
	MAC_LIST eth_mac_list;
	MAC_IP_LIST ip_neigh_list;
	int i = 0;
	
	//get dhcp lease
	find_client_list_dum(&tz_dhcp_lease);

	//get ip_neigh_list
	get_ip_neigh_list(&ip_neigh_list);

	//get wireless mac list
	get_wireless_mac_list(&wireless_mac_list);

	//get eth mac list
	get_eth_mac_list(&wireless_mac_list, &eth_mac_list, &ip_neigh_list);
	
	for(i = 0; i < eth_mac_list.cnt; i++)
	{
		print("eth_mac_list(%d): %s--", i, eth_mac_list.mac_item[i].mac);
	}

	for(i = 0; i < ip_neigh_list.cnt; i++)
	{
		print("ip_neigh_list(%d): %s--%s", i, ip_neigh_list.mac_ip_item[i].mac, ip_neigh_list.mac_ip_item[i].ip);
	}
	
	//form the current_lan_list
	memset(current_lan_list, 0, sizeof(LAN_LIST_T));
	
	fill_current_lan_list(current_lan_list, &tz_dhcp_lease, &ip_neigh_list, &wireless_mac_list, "wlan0-va0");
	fill_current_lan_list(current_lan_list, &tz_dhcp_lease, &ip_neigh_list, &eth_mac_list, "usblan0");
	
	//add_interface_to_lan_list(current_lan_list, &tz_dhcp_lease, &wireless_mac_list, "wlan0-va0");
	//add_interface_to_lan_list(current_lan_list, &tz_dhcp_lease, &eth_mac_list, "usblan0");
}



void form_history_lan_list(LAN_LIST_T* current_lan_list, LAN_LIST_T* history_lan_list)
{
	int i = 0, j = 0, k = 0;
	LAN_LIST_T new_lan_list;
	memset(&new_lan_list, 0, sizeof(new_lan_list));
	
	//clean the history flow
	for(i = 0; i < history_lan_list->cnt; i++)
	{
		history_lan_list->list[i].flow = 0;
	}
	
	//get the new lan list
	for(i =0; i <  current_lan_list->cnt; i++)
	{
		for(j = 0; j < history_lan_list->cnt; j++)
		{
			if(strcmp(current_lan_list->list[i].mac, history_lan_list->list[j].mac) == 0)
			{
				strcpy(history_lan_list->list[j].expires,  current_lan_list->list[i].expires );
				strcpy(history_lan_list->list[j].hostname,  current_lan_list->list[i].hostname );
				if(strcmp(history_lan_list->list[i].ipaddr, current_lan_list->list[i].ipaddr) != 0)
					strcpy(history_lan_list->list[j].ipaddr,  current_lan_list->list[i].ipaddr );
				break;
			}
		}
		
		if(j == history_lan_list->cnt)
		{
			// make sure the new_lan_mac appear int the current lan list just 1 time
			for(k = 0; k < new_lan_list.cnt; k++)
			{
				if(strcmp(current_lan_list->list[i].mac, new_lan_list.list[k].mac) == 0)
					break;
			}
			
			if(k == new_lan_list.cnt)
			{
				strcpy( new_lan_list.list[new_lan_list.cnt].mac, current_lan_list->list[i].mac);
				strcpy( new_lan_list.list[new_lan_list.cnt].ipaddr, current_lan_list->list[i].ipaddr);
				strcpy( new_lan_list.list[new_lan_list.cnt].interface, current_lan_list->list[i].interface);
				strcpy( new_lan_list.list[new_lan_list.cnt].hostname, current_lan_list->list[i].hostname);
				strcpy( new_lan_list.list[new_lan_list.cnt].expires, current_lan_list->list[i].expires);
				new_lan_list.cnt++;
			}
			
		}
	}
	
	//add to history list
	for(i = 0; i < new_lan_list.cnt; i++)
	{
		strcpy( history_lan_list->list[history_lan_list->cnt].mac,  new_lan_list.list[i].mac);
		strcpy( history_lan_list->list[history_lan_list->cnt].ipaddr,  new_lan_list.list[i].ipaddr);
		strcpy( history_lan_list->list[history_lan_list->cnt].interface,  new_lan_list.list[i].interface);
		strcpy( history_lan_list->list[history_lan_list->cnt].hostname,  new_lan_list.list[i].hostname);
		strcpy( history_lan_list->list[history_lan_list->cnt].expires,  new_lan_list.list[i].expires);
		history_lan_list->cnt++;
	}
}


void echo_lan_list_to_file(LAN_LIST_T* lan_list, char* filename)
{
	int fd;
	int i = 0;
	char shellcmd[256] = "";
	
	sprintf(shellcmd, "rm -rf %s", filename);
	system(shellcmd);
	
	fd=open(filename, O_RDWR | O_CREAT);
	if(fd == -1)
	{
		return;
	}

	for(i = 0; i < lan_list->cnt; i++)
	{
		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd, "%s |%s |%s |%s |%s |%d\n",lan_list->list[i].mac, lan_list->list[i].ipaddr, lan_list->list[i].hostname,
			lan_list->list[i].interface, lan_list->list[i].expires, lan_list->list[i].flow);
		write(fd, shellcmd, strlen(shellcmd));
	}

	close(fd);
}



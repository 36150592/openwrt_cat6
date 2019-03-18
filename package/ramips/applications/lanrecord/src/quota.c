#include "include.h"

#define QUOTA_CHAIN "flow"

MAC_IP_MAP_LIST mac_ip_list;
MAC_FLOW_LIST history_record_mac_flow_list;

void restore_quota()
{
	char shellcmd[256] = "";
	char buffer[256]="";

	memset(&mac_ip_list, 0, sizeof(MAC_IP_MAP_LIST));
	memset(&history_record_mac_flow_list, 0, sizeof(MAC_FLOW_LIST));
	
	sprintf(shellcmd, "iptables -S | grep %s | grep \"\\-N\"", QUOTA_CHAIN);

	if (execute_cmd(shellcmd, buffer, sizeof(buffer)) != 0) {
		return;
	}

	if(buffer[0])
	{
		print("iptables: %s is exist...", QUOTA_CHAIN);
	}
	else
	{
		print("iptables: %s is no exist...", QUOTA_CHAIN);
		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd, "iptables -t filter -N %s", QUOTA_CHAIN);
		system(shellcmd);
	}

	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd, "iptables -F %s", QUOTA_CHAIN);
	system(shellcmd);

	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd, "iptables -D FORWARD -j %s", QUOTA_CHAIN);
	system(shellcmd);

	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd, "iptables -A FORWARD -j %s", QUOTA_CHAIN);
	system(shellcmd);
}

static int get_the_flow_depend_ip(char* ipaddr)
{
	int download_byte = 0;
	int upload_byte = 0;
	char shellcmd[256] = "";
	char receive_buf[56] = "";

	//get download byte
	sprintf(shellcmd, "iptables -L -v | grep \"%s \" | awk '{if($7 == \"anywhere\"){print $2}}'", ipaddr);
	if (execute_cmd(shellcmd, receive_buf, sizeof(receive_buf)) != 0) {
		return 0;
	}
	
	if(receive_buf[0])
	{
		char* p = NULL;
		int multiple = 1;
		p = strstr(receive_buf, "K");
		if(p)
		{
			*p=0;
			multiple = 1024;
		}
		else
		{
			p = strstr(receive_buf, "M");
			if(p)
			{
				*p=0;
				multiple = 1024*1024;
			}
		}
		download_byte = atoi(receive_buf) * multiple;
	}

	//get upload byte
	memset(shellcmd, 0, sizeof(shellcmd));
	memset(receive_buf, 0, sizeof(receive_buf));
	sprintf(shellcmd, "iptables -L -v | grep \"%s \" | awk '{if($8 == \"anywhere\"){print $2}}'", ipaddr);
	if (execute_cmd(shellcmd, receive_buf, sizeof(receive_buf)) != 0) {
		return 0;
	}

	if(receive_buf[0])
	{
		char* p = NULL;
		int multiple = 1;
		p = strstr(receive_buf, "K");
		if(p)
		{
			*p=0;
			multiple = 1024;
		}
		else
		{
			p = strstr(receive_buf, "M");
			if(p)
			{
				*p=0;
				multiple = 1024*1024;
			}
		}
		upload_byte = atoi(receive_buf) * multiple;
	}

	return (upload_byte+download_byte);
}

static void add_to_history_record(char* mac, int flow)
{
	int i = 0;
	for(i = 0; i < history_record_mac_flow_list.cnt; i++)
	{
		if(strcmp(history_record_mac_flow_list.mac_flow_item[i].mac, mac) == 0)
			break;
	}

	// new record
	if(i == history_record_mac_flow_list.cnt)
	{
		strcpy(history_record_mac_flow_list.mac_flow_item[history_record_mac_flow_list.cnt].mac, mac);
		history_record_mac_flow_list.mac_flow_item[history_record_mac_flow_list.cnt].flow = flow;
		history_record_mac_flow_list.cnt++;
	}
	else
	{
		history_record_mac_flow_list.mac_flow_item[i].flow +=flow;
	}
}

static void rm_quota_rule_depend_ip(char* ipaddr)
{
	char shellcmd[256] = "";
	char receive_buf[56] = "";

	// rm the -A flow -d 192.168.8.100/32
	sprintf(shellcmd, "iptables -S | grep %s | grep \"%s/\" | awk '{if($3 == \"-d\"){print $0}}' | awk -F \"\\-A\" '{print $2}'", QUOTA_CHAIN, ipaddr);
	if (execute_cmd(shellcmd, receive_buf, sizeof(receive_buf)) == 0) 
	{
		if(receive_buf[0])	
		{
			memset(shellcmd, 0, sizeof(shellcmd));
			sprintf(shellcmd, "iptables -D %s", receive_buf);
			system(shellcmd);
		}
	}

	memset(shellcmd, 0, sizeof(shellcmd));
	memset(receive_buf, 0, sizeof(receive_buf));
	
	// rm the -A flow -s 192.168.8.100/32
	sprintf(shellcmd, "iptables -S | grep %s | grep \"%s/\" | awk '{if($3 == \"-s\"){print $0}}' | awk -F \"\\-A\" '{print $2}'", QUOTA_CHAIN, ipaddr);
	if (execute_cmd(shellcmd, receive_buf, sizeof(receive_buf)) == 0) 
	{
		if(receive_buf[0])	
		{
			memset(shellcmd, 0, sizeof(shellcmd));
			sprintf(shellcmd, "iptables -D %s", receive_buf);
			system(shellcmd);
		}
	}
	
}

static void add_quota_rule_depend_ip(char* ipaddr)
{
	char shellcmd[256] = "";
	sprintf(shellcmd, "iptables -A %s -d %s", QUOTA_CHAIN, ipaddr);
	system(shellcmd);

	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd, "iptables -A %s -s %s", QUOTA_CHAIN, ipaddr);
	system(shellcmd);
}

static void check_quota_rule()
{
	char shellcmd[256] = "";
	char buffer[256]="";

	sprintf(shellcmd, "iptables -S | grep %s | grep \"FORWARD \\-j\"", QUOTA_CHAIN);

	if (execute_cmd(shellcmd, buffer, sizeof(buffer)) != 0) {
		return;
	}
	
	
	if(buffer[0] == '\0')
	{
		restore_quota();
	}

}

void set_quota_rule(LAN_LIST_T* current_lan_list)
{
	MAC_IP_MAP_LIST rm_mac_ip_list;
	MAC_IP_MAP_LIST new_mac_ip_list;
	MAC_IP_MAP_LIST old_mac_ip_list;
	
	int i = 0;
	int j = 0;
	int k = 0;
	int m = 0;

	memset(&rm_mac_ip_list, 0, sizeof(MAC_IP_MAP_LIST));
	memset(&new_mac_ip_list, 0, sizeof(MAC_IP_MAP_LIST));
	memset(&old_mac_ip_list, 0, sizeof(MAC_IP_MAP_LIST));
	
	for(i = 0; i < current_lan_list->cnt; i++)
	{
		for(j = 0; j < mac_ip_list.cnt; j ++)
		{
			//old mac
			if( strcmp(current_lan_list->list[i].mac, mac_ip_list.mac_ip[j].mac) == 0)
			{
				//if mac and ip is same as old.
				if( strcmp(current_lan_list->list[i].ipaddr, mac_ip_list.mac_ip[j].ip) == 0)
				{
					print("%s--%s is old mac, old ip 1", mac_ip_list.mac_ip[j].mac, mac_ip_list.mac_ip[j].ip);
					break;
				}
				else //mac is old, but ip is no same as old, so delete the old mac-ip, and add new mac-ip
				{
					for(m = 0; m < rm_mac_ip_list.cnt; m++)
					{
						if(strcmp(rm_mac_ip_list.mac_ip[m].mac, current_lan_list->list[i].mac) == 0)
						{
							// the old mac-ip had in rm list.
							break;
						}
					}
					
					if(m == rm_mac_ip_list.cnt)
					{
						strcpy(rm_mac_ip_list.mac_ip[rm_mac_ip_list.cnt].mac, mac_ip_list.mac_ip[j].mac);
						strcpy(rm_mac_ip_list.mac_ip[rm_mac_ip_list.cnt].ip, mac_ip_list.mac_ip[j].ip);
						rm_mac_ip_list.cnt++;
					}

					int t=0;
					for(t = 0; t < new_mac_ip_list.cnt; t++)
					{
						if(strcmp(current_lan_list->list[i].mac, new_mac_ip_list.mac_ip[t].mac) == 0)
							break;
					}

					//add new mac-ip to list
					strcpy(new_mac_ip_list.mac_ip[new_mac_ip_list.cnt].mac, current_lan_list->list[i].mac);
					strcpy(new_mac_ip_list.mac_ip[new_mac_ip_list.cnt].ip, current_lan_list->list[i].ipaddr);
					new_mac_ip_list.cnt++;

					//check if the ip had used
					for(k = 0; k < mac_ip_list.cnt; k++)
					{
						if( strcmp(current_lan_list->list[i].ipaddr, mac_ip_list.mac_ip[k].ip ) == 0)
							break;
					}

					//if ip is old
					if(k != mac_ip_list.cnt)
					{
						for(m =0; m < rm_mac_ip_list.cnt; m++)
						{
							if(strcmp(rm_mac_ip_list.mac_ip[m].mac, mac_ip_list.mac_ip[k].mac) == 0)
								break;	
						}

						//old ip's mac is not in rm list.
						if(m == rm_mac_ip_list.cnt)
						{
							strcpy(rm_mac_ip_list.mac_ip[rm_mac_ip_list.cnt].mac, mac_ip_list.mac_ip[k].mac);
							strcpy(rm_mac_ip_list.mac_ip[rm_mac_ip_list.cnt].ip, mac_ip_list.mac_ip[k].ip);
							rm_mac_ip_list.cnt++;
						}
					}
					break;
				}
				
			}
			
			
		}

		// if mac is new
		if(j == mac_ip_list.cnt)
		{
			//add new mac-ip to list
			strcpy(new_mac_ip_list.mac_ip[new_mac_ip_list.cnt].mac, current_lan_list->list[i].mac);
			strcpy(new_mac_ip_list.mac_ip[new_mac_ip_list.cnt].ip, current_lan_list->list[i].ipaddr);
			new_mac_ip_list.cnt++;
			
			//check if the ip had used
			for(k = 0; k < mac_ip_list.cnt; k++)
			{
				if( strcmp(current_lan_list->list[i].ipaddr, mac_ip_list.mac_ip[k].ip ) == 0)
					break;
			}

			// the ip is old, it had mac.so delete it
			if(k != mac_ip_list.cnt)
			{
				for(m =0; m < rm_mac_ip_list.cnt; m++)
				{
					if(strcmp(rm_mac_ip_list.mac_ip[m].mac, mac_ip_list.mac_ip[k].mac) == 0)
						break;	
				}

				//old ip's mac is not in rm list.
				if(m == rm_mac_ip_list.cnt)
				{
					strcpy(rm_mac_ip_list.mac_ip[rm_mac_ip_list.cnt].mac, mac_ip_list.mac_ip[k].mac);
					strcpy(rm_mac_ip_list.mac_ip[rm_mac_ip_list.cnt].ip, mac_ip_list.mac_ip[k].ip);
					rm_mac_ip_list.cnt++;
				}
			}
		}
	}

	// rm_mac_ip_list
	for(i = 0; i < rm_mac_ip_list.cnt; i++)
	{
		//record the mac's flow
		int item_flow = get_the_flow_depend_ip(rm_mac_ip_list.mac_ip[i].ip);
		
		//add to history record
		add_to_history_record(rm_mac_ip_list.mac_ip[i].mac, item_flow);
		
		//rm the iptables rule, it depend on ip
		print("will delete: %s--%s", rm_mac_ip_list.mac_ip[i].mac, rm_mac_ip_list.mac_ip[i].ip);
		rm_quota_rule_depend_ip(rm_mac_ip_list.mac_ip[i].ip);
	}

	// new_mac_ip_list
	for(i = 0; i < new_mac_ip_list.cnt; i++)
	{
		print("will add: %s--%s", new_mac_ip_list.mac_ip[i].mac, new_mac_ip_list.mac_ip[i].ip);
		add_quota_rule_depend_ip(new_mac_ip_list.mac_ip[i].ip);
	}

	// update map_ip_list
	for(i = 0; i < mac_ip_list.cnt; i++)
	{
		for(j = 0; j <  new_mac_ip_list.cnt; j++)
		{
			if(strcmp(new_mac_ip_list.mac_ip[j].mac, mac_ip_list.mac_ip[i].mac) == 0)
				break;
		}

		for(k = 0; k < rm_mac_ip_list.cnt; k++)
		{
			if(strcmp(rm_mac_ip_list.mac_ip[k].mac, mac_ip_list.mac_ip[i].mac) == 0)
				break;
		}

		if( (j == new_mac_ip_list.cnt) && (k == rm_mac_ip_list.cnt) )
		{
			for(k = 0; k < old_mac_ip_list.cnt; k++)
			{
				if(strcmp(old_mac_ip_list.mac_ip[k].mac, mac_ip_list.mac_ip[i].mac) == 0)
					break;
			}
			
			if(k == old_mac_ip_list.cnt)
			{
				print(">>>>old: %s--%s", mac_ip_list.mac_ip[i].mac, mac_ip_list.mac_ip[i].ip);
				strcpy(old_mac_ip_list.mac_ip[old_mac_ip_list.cnt].mac, mac_ip_list.mac_ip[i].mac);
				strcpy(old_mac_ip_list.mac_ip[old_mac_ip_list.cnt].ip, mac_ip_list.mac_ip[i].ip);
				old_mac_ip_list.cnt++;
			}
			
		}
	}

	for(i = 0; i < old_mac_ip_list.cnt; i++)
	{
		strcpy(new_mac_ip_list.mac_ip[new_mac_ip_list.cnt].mac, old_mac_ip_list.mac_ip[i].mac);
		strcpy(new_mac_ip_list.mac_ip[new_mac_ip_list.cnt].ip, old_mac_ip_list.mac_ip[i].ip);
		new_mac_ip_list.cnt++;
	}

	memset(&mac_ip_list, 0, sizeof(mac_ip_list));
	memcpy(&mac_ip_list, &new_mac_ip_list, sizeof(new_mac_ip_list));

	check_quota_rule();
}


static void get_mac_ip_list_flow()
{
	int i = 0;
	int j = 0;
	
	for(i = 0; i < mac_ip_list.cnt; i++)
	{
		mac_ip_list.mac_ip[i].flow = 0;
		mac_ip_list.mac_ip[i].flow = get_the_flow_depend_ip(mac_ip_list.mac_ip[i].ip);
		
		for(j = 0; j < history_record_mac_flow_list.cnt; j++)
		{
			if(strcmp(mac_ip_list.mac_ip[i].mac, history_record_mac_flow_list.mac_flow_item[j].mac) == 0)
			{
				mac_ip_list.mac_ip[i].flow += history_record_mac_flow_list.mac_flow_item[j].flow;
				break;
			}
		}
		
	}

}

static void fill_lan_list_flow(LAN_LIST_T* lan_list)
{
	int i = 0; 
	int j = 0;
	for(i = 0; i < lan_list->cnt; i++)
	{
		for(j = 0; j < mac_ip_list.cnt; j++)
		{
			if(strcmp(lan_list->list[i].mac, mac_ip_list.mac_ip[j].mac) == 0)
			{
				lan_list->list[i].flow = mac_ip_list.mac_ip[j].flow;
			}
		}
	}
}


void get_quota_value(LAN_LIST_T* current_lan_list, LAN_LIST_T* history_lan_list)
{
	 
	//get the mac_ip_list's flow.
	get_mac_ip_list_flow();
	
	//fill current_lan_list's flow
	fill_lan_list_flow(current_lan_list);

	//fill history_lan_list's flow
	fill_lan_list_flow(history_lan_list);
}



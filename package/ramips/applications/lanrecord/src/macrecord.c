#include "include.h"
#include "iwlib.h"
#define LEASE_FILE_PATH "/tmp/dhcp.leases"
#define WIRELESS_2G_IFNAME "ra0"
#define WIRELESS_5G_IFNAME "rai0"
#define LAN_INTERFACE "eth0.1"
static int find_client_list_dum(TZ_UDHCPD_LEASE_T* c_udhcpd_lease)
{
	int i;
	char buffer[MAX_FILE_BUFFER_SIZE], shellcmd[MINI_FILE_BUFFER_SIZE];
    char tmp_values[MINI_FILE_BUFFER_SIZE];
	char *pHead, *pTail;
	char *pnextstart;
	
	pHead = NULL;
	pTail = NULL;

	log_info("[find_client_list_dum]\n");
	memset(c_udhcpd_lease, 0, sizeof(TZ_UDHCPD_LEASE_T));

	sprintf(shellcmd, "cat %s", LEASE_FILE_PATH);

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
			pnextstart = get_value_by_end(tmp_values, c_udhcpd_lease->lease[i].expires, " ");
			pnextstart = get_value_by_end(pnextstart, c_udhcpd_lease->lease[i].mac, " ");
			pnextstart = get_value_by_end(pnextstart, c_udhcpd_lease->lease[i].ipaddr, " ");
			pnextstart = get_value_by_end(pnextstart, c_udhcpd_lease->lease[i].hostname, " ");

			log_info("i(%d): ip(%s) mac(%s) hostname(%s) expires(%s)\n", i, c_udhcpd_lease->lease[i].ipaddr,
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

/*------------------------------------------------------------------*/
/*
 * Execute a private command on the interface
 */
static int
set_private_cmd(int		skfd,		/* Socket */
		char *		ifname,		/* Dev name */
		MAC_LIST* mac_list
		)	/* Number of descriptions */
{
  struct iwreq	wrq;
  u_char	buffer[4096];	/* Only that big in v25 and later */
  int		i = 0;		/* Start with first command arg */
  int		k;		/* Index in private description table */
  int		temp;
  int		subcmd = 0;	/* sub-ioctl index */
  int		offset = 0;	/* Space for sub-ioctl index */

  iwprivargs *	priv;
  int		priv_num;		/* Max of private ioctl */
  int		ret;

  /* Read the private ioctls */
  priv_num = iw_get_priv_info(skfd, ifname, &priv);

  /* Is there any ? */
  if(priv_num <= 0)
    {
      /* Should I skip this message ? */
      log_error("%-8.16s  no private ioctls.\n\n",ifname);
      if(priv)
	free(priv);
      return(-1);
    }


  /* Search the correct ioctl */
  k = -1;
  while((++k < priv_num) && strcmp(priv[k].name, "get_on_cli"));

	/* If not found... */
	if(k == priv_num)
	{
		log_error("Invalid command : get_on_cli cannot find!\n");
		return(-1);
	}
	  
  /* Watch out for sub-ioctls ! */
	if(priv[k].cmd < SIOCDEVPRIVATE)
	{
	  int	j = -1;

		/* Find the matching *real* ioctl */
		while((++j < priv_num) && ((priv[j].name[0] != '\0') ||
				 (priv[j].set_args != priv[k].set_args) ||
				 (priv[j].get_args != priv[k].get_args)));

	    /* If not found... */
		if(j == priv_num)
		{
		  log_error("Invalid private ioctl definition for : get_on_cli\n");
		  return(-1);
		}

		/* Save sub-ioctl number */
		subcmd = priv[k].cmd;
		/* Reserve one int (simplify alignment issues) */
		offset = sizeof(__u32);
		/* Use real ioctl definition from now on */
		k = j;
	}

  /* If we have to set some data */
  	if((priv[k].set_args & IW_PRIV_TYPE_MASK) &&
     	(priv[k].set_args & IW_PRIV_SIZE_MASK))
    {
	    switch(priv[k].set_args & IW_PRIV_TYPE_MASK)
		{
			case IW_PRIV_TYPE_CHAR:
			{
				/* Size of the string to fetch */
				wrq.u.data.length = strlen("get_on_cli") + 1;
				if(wrq.u.data.length > (priv[k].set_args & IW_PRIV_SIZE_MASK))
					wrq.u.data.length = priv[k].set_args & IW_PRIV_SIZE_MASK;

				/* Fetch string */
				memcpy(buffer, "get_on_cli", wrq.u.data.length);
				buffer[sizeof(buffer) - 1] = '\0';
			} 
			break;
			default:
				log_error("Not implemented...\n");
				return(-1);
		}
	  
      if((priv[k].set_args & IW_PRIV_SIZE_FIXED) &&
	 		(wrq.u.data.length != (priv[k].set_args & IW_PRIV_SIZE_MASK)))
		{
		  	log_info("The command get_on_cli needs exactly %d argument(s)...\n",
						priv[k].set_args & IW_PRIV_SIZE_MASK);
		  	return(-1);
		}
    }	/* if args to set */
  	else
    {
      wrq.u.data.length = 0L;
    }

  strncpy(wrq.ifr_name, ifname, IFNAMSIZ);

  /* Those two tests are important. They define how the driver
   * will have to handle the data */
  if((priv[k].set_args & IW_PRIV_SIZE_FIXED) &&
      ((iw_get_priv_size(priv[k].set_args) + offset) <= IFNAMSIZ))
    {
      /* First case : all SET args fit within wrq */
      if(offset)
			wrq.u.mode = subcmd;
      memcpy(wrq.u.name + offset, buffer, IFNAMSIZ - offset);
    }
  else
    {
      if((priv[k].set_args == 0) &&
	 (priv[k].get_args & IW_PRIV_SIZE_FIXED) &&
	 (iw_get_priv_size(priv[k].get_args) <= IFNAMSIZ))
	{
	  /* Second case : no SET args, GET args fit within wrq */
	  if(offset)
	    wrq.u.mode = subcmd;
	}
      else
	{
	  /* Third case : args won't fit in wrq, or variable number of args */
	  wrq.u.data.pointer = (caddr_t) buffer;
	  wrq.u.data.flags = subcmd;
	}
    }

  /* Perform the private ioctl */
  if(ioctl(skfd, priv[k].cmd, &wrq) < 0)
    {
      log_error("Interface doesn't accept private ioctl...\n");
      return(-1);
    }

  /* If we have to get some data */
  if((priv[k].get_args & IW_PRIV_TYPE_MASK) &&
     (priv[k].get_args & IW_PRIV_SIZE_MASK))
    {
	     int	j;
	     int	n = 0;		/* number of args */

	     log_info("get online client from %s:\n", ifname);

	      /* Check where is the returned data */
	    if((priv[k].get_args & IW_PRIV_SIZE_FIXED) &&
		 (iw_get_priv_size(priv[k].get_args) <= IFNAMSIZ))
		{
		  memcpy(buffer, wrq.u.name, IFNAMSIZ);
		  n = priv[k].get_args & IW_PRIV_SIZE_MASK;
		}
	      else
		n = wrq.u.data.length;

	    switch(priv[k].get_args & IW_PRIV_TYPE_MASK)
		{
			case IW_PRIV_TYPE_CHAR:
			{
				SPLIT_STRUCT mainStrings[MINI_ARRAY_SIZE];
				/* Display args */
				buffer[n] = '\0';
				int item_count = StringSplit(buffer, "\n", mainStrings);
				int i = 0;
				for(;i<item_count;i++)
				{
					if ('M' == mainStrings[i].lp_string[0] ||
						'\n' == mainStrings[i].lp_string[0] ||
						 0 == mainStrings[i].length)
						 continue;
					strncpy(mac_list->mac_item[mac_list->cnt].mac,  mainStrings[i].lp_string,17);
					log_info("wireless mac = %s\n",mac_list->mac_item[mac_list->cnt].mac);
					mac_list->cnt++;
				}
				log_info("ioctl rev %s\n", buffer);
			}
			break;
			default:
			  log_error("Not yet implemented...\n");
			  return(-1);
		}
    }	/* if args to set */

  return(0);
}



static void get_wireless_mac_list(MAC_LIST* mac_2g_list,MAC_LIST* mac_5g_list)
{
	log_info("[get_wireless_client]\n");	
	memset(mac_2g_list, 0, sizeof(MAC_LIST));
	memset(mac_5g_list, 0, sizeof(MAC_LIST));

	int skfd;	   /* generic raw socket desc. */
	int goterr = 0;

	/* Create a channel to the NET kernel. */
	if((skfd = iw_sockets_open()) < 0)
	{
	 	perror("socket");
	 	return ;
	}

	set_private_cmd(skfd,WIRELESS_2G_IFNAME,mac_2g_list);
	set_private_cmd(skfd,WIRELESS_5G_IFNAME,mac_5g_list);

	iw_sockets_close(skfd);

}

static void get_ip_neigh_list(IPNEIGH_ITEM_LIST* ip_neigh_list)
{
	char cmd[256] = {0};
	char tmp_buff[NORMAL_FILE_BUFFER_SIZE] = {0};
	int item_count = 0, i = 0, j = 0;
	SPLIT_STRUCT mainStrings[MINI_ARRAY_SIZE];

	memset(ip_neigh_list, 0, sizeof(IPNEIGH_ITEM_LIST));

	sprintf(cmd, "ip neigh list |grep -v FAILED| grep -v STALE| awk '{print $5 \" \" $1 \" \" $3}'");
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
			char* item_if = NULL;
			strncpy(item_mac, mainStrings[i].lp_string, mainStrings[i].length);
			item_ip = strstr(item_mac, " ");
			if(!item_ip)
				continue;
			*item_ip = 0; 
			item_ip++;
			item_if = strstr(item_ip, " ");

			if(!item_if)
				continue;
			*item_if = 0;
			item_if++;
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
				if(strcmp(item_mac, ip_neigh_list->list[j].mac) == 0)
					break;
			}
			
			if(j == ip_neigh_list->cnt)
			{
				strcpy(ip_neigh_list->list[ip_neigh_list->cnt].mac, item_mac);
				strcpy(ip_neigh_list->list[ip_neigh_list->cnt].ip, item_ip);
				strcpy(ip_neigh_list->list[ip_neigh_list->cnt].ifname, item_if);
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
				log_info(">>>>>%s: %s\n",interface, mac_list->mac_item[i].mac);
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

#include <uci.h>


static void get_wireless_info_by_lannet(const char *network,const char *type, char *interface, char* ssid)
{
	 struct uci_context * uci_ctx;
	 struct uci_package * uci_wireless;
	 struct uci_package * uci_mutilssid;
	 struct uci_element *e   = NULL;
	 char device[64] ={0};
	 if (!uci_ctx)
    {
        uci_ctx = uci_alloc_context();
    }
    else
    {
        uci_wireless = uci_lookup_package(uci_ctx, "wireless");
		uci_mutilssid = uci_lookup_package(uci_ctx, "mutilssid");
        if (uci_wireless)
            uci_unload(uci_ctx, uci_wireless);

		if (uci_mutilssid)
		{
			uci_unload(uci_ctx, uci_mutilssid);
			
		}
           
    }

    if (uci_load(uci_ctx, "wireless", &uci_wireless))
    {
    	log_error("uci load wireless fail\n");
        return ;
    }

	uci_load(uci_ctx, "mutilssid", &uci_mutilssid);

	
    /* scan wireless network interfaces ! */
    uci_foreach_element(&uci_wireless->sections, e)
    {
		struct uci_section *s = uci_to_section(e);
		char *value = NULL;
		if(0 == strcmp(s->type, "wifi-device"))
		{
			value = uci_lookup_option_string(uci_ctx, s, "band");
			log_info("band = %s, type = %s", value, type);
			if(NULL != value && strcmp(value,type) == 0)
			{
				value = uci_lookup_option_string(uci_ctx, s, "type");
				if(NULL != value)
					strcpy(device,value);
			}
		}
		
        if(0 == strcmp(s->type, "wifi-iface"))
        {
			value = uci_lookup_option_string(uci_ctx, s, "device");
			log_info("wireless device = %s\n", value);
			if(NULL != value && strcmp(value,device) != 0)
				continue;
			const char* disabled = uci_lookup_option_string(uci_ctx, s, "disabled");
			if (NULL != disabled && 0 == strncmp("1", disabled, strlen("1")))
			{
				continue;
			}

      
            value = NULL;
            value = uci_lookup_option_string(uci_ctx, s, "network");
            if(NULL == value || 0 != strcmp(value, network))
				continue;

			value = NULL;
			value = uci_lookup_option_string(uci_ctx, s, "ifname");
			log_info("wireless ifname = %s\n", value);
			if(value)
				strcpy(interface, value);
			
            value = NULL;
			value = uci_lookup_option_string(uci_ctx, s, "ssid");
			if(value)
				strcpy(ssid, value);
			
       
        }
    }


	/* scan mutilssid network interfaces ! */
    uci_foreach_element(&uci_mutilssid->sections, e)
    {
        struct uci_section *s = uci_to_section(e);
        if(0 == strcmp(s->type, "wifi-iface"))
        {
			char *value = NULL;
			value = uci_lookup_option_string(uci_ctx, s, "device");
			log_info("mutilssid device = %s\n", value);
			if(NULL != value && strcmp(value,device) != 0)
				continue;
			const char* disabled = uci_lookup_option_string(uci_ctx, s, "disabled");
			if (NULL != disabled && 0 == strncmp("1", disabled, strlen("1")))
			{
				continue;
			}

      
            value = NULL;
            value = uci_lookup_option_string(uci_ctx, s, "network");
            if(NULL == value || 0 != strcmp(value, network))
				continue;

			value = NULL;
			value = uci_lookup_option_string(uci_ctx, s, "ifname");
			log_info("mutilssid ifname = %s\n", value);
			if(value)
				strcpy(interface, value);
			
            value = NULL;
			value = uci_lookup_option_string(uci_ctx, s, "ssid");
			if(value)
				strcpy(ssid, value);
			
       
        }
    }


}

static int check_mac_exist(MAC_LIST* list, const char* mac)
{
	int i = 0;
	for(i=0;i< list->cnt;i++)
		if(strcmp(mac,list->mac_item[i].mac) == 0)
			return 1;


	return 0;
}

static void fill_current_lan_list(LAN_LIST_T* target_lan_list, TZ_UDHCPD_LEASE_T* all_udhcpd_lease, 
	IPNEIGH_ITEM_LIST* ip_neigh_list)
{
	int i = 0; 
	int j = 0;
	int k = 0;
	MAC_LIST mac_2g_list,mac_5g_list;
	//get wireless mac list
	get_wireless_mac_list(&mac_2g_list, &mac_5g_list);

	
	for(i = 0; i < ip_neigh_list->cnt; i++)
	{
		char *mac = ip_neigh_list->list[i].mac;
		char *ip = ip_neigh_list->list[i].ip;
		char *ifname = ip_neigh_list->list[i].ifname;
		char *network = strstr(ifname, "br-");
		char type[6] = {0};
		if(check_mac_exist(&mac_2g_list, mac) && network)
		{
			network+=3;
			strcpy(type,"2.4G");
			log_info("network = %s\n", network);
		}
		else if( check_mac_exist(&mac_5g_list, mac) && network)
		{
			network+=3;
			strcpy(type,"5G");
			log_info("network = %s\n", network);
		}
		else
			network=NULL;

		for(k=0; k < all_udhcpd_lease->cnt; k++)
		{
			if(strcmp(all_udhcpd_lease->lease[k].mac, mac) == 0)
				break;
		}

		// has appear in dhcpd
		if(k != all_udhcpd_lease->cnt)
		{
			strcpy(target_lan_list->list[target_lan_list->cnt].mac,  all_udhcpd_lease->lease[k].mac);
			strcpy(target_lan_list->list[target_lan_list->cnt].hostname,    all_udhcpd_lease->lease[k].hostname);
			strcpy(target_lan_list->list[target_lan_list->cnt].expires,    all_udhcpd_lease->lease[k].expires);
			
			if(network)
			{
				char* tmp_inter = target_lan_list->list[target_lan_list->cnt].interface;
				char* tmp_ssid = target_lan_list->list[target_lan_list->cnt].ssid;
				get_wireless_info_by_lannet(network,type,tmp_inter,tmp_ssid);
				log_info("tmp_inter = %s tmp_ssid = %s\n", tmp_inter, tmp_ssid);
			}
			else
			{
				strcpy(target_lan_list->list[target_lan_list->cnt].interface,"eth0.1");
				strcpy(target_lan_list->list[target_lan_list->cnt].ssid,"*");
			}
			
			if(ip[0])
				strcpy(target_lan_list->list[target_lan_list->cnt].ipaddr, ip);
			else
				strcpy(target_lan_list->list[target_lan_list->cnt].ipaddr,
									all_udhcpd_lease->lease[k].ipaddr);
			target_lan_list->cnt++;
		}
		else
		{
			if(ip[0])
			{
				strcpy(target_lan_list->list[target_lan_list->cnt].mac, mac);
				strcpy(target_lan_list->list[target_lan_list->cnt].ipaddr,    ip);
				strcpy(target_lan_list->list[target_lan_list->cnt].hostname,    "*");
				strcpy(target_lan_list->list[target_lan_list->cnt].expires,    "*");
				if(network)
				{
					char* tmp_inter = target_lan_list->list[target_lan_list->cnt].interface;
					char* tmp_ssid = target_lan_list->list[target_lan_list->cnt].ssid;
					get_wireless_info_by_lannet(network,type,tmp_inter,tmp_ssid);
					log_info("tmp_inter = %s tmp_ssid = %s\n", tmp_inter, tmp_ssid);
				}
				else
				{
					strcpy(target_lan_list->list[target_lan_list->cnt].interface,"eth0.1");
					strcpy(target_lan_list->list[target_lan_list->cnt].ssid,"*");
				}
				target_lan_list->cnt++;
			}
		}
		
	}
}



void get_current_lan_list(LAN_LIST_T* current_lan_list )
{
	TZ_UDHCPD_LEASE_T tz_dhcp_lease;
	MAC_LIST eth_mac_list;
	IPNEIGH_ITEM_LIST ip_neigh_list;
	int i = 0;

	//get dhcp lease
	find_client_list_dum(&tz_dhcp_lease);
	
	//get ip_neigh_list
	get_ip_neigh_list(&ip_neigh_list);
	
	for(i = 0; i < ip_neigh_list.cnt; i++)
	{
		log_info("ip_neigh_list(%d): %s--%s--%s\n", i, 
				ip_neigh_list.list[i].mac, 
				ip_neigh_list.list[i].ip,
				ip_neigh_list.list[i].ifname);
	}
	
	//form the current_lan_list
	memset(current_lan_list, 0, sizeof(LAN_LIST_T));
	fill_current_lan_list(current_lan_list, &tz_dhcp_lease, &ip_neigh_list);
	//fill_current_lan_list(current_lan_list, &tz_dhcp_lease, &ip_neigh_list, &wireless_5g_mac_list, WIRELESS_5G_IFNAME);	
	//fill_current_lan_list(current_lan_list, &tz_dhcp_lease, &ip_neigh_list, &eth_mac_list, LAN_INTERFACE);	
}



void read_history_lan_list(LAN_LIST_T* history_lan_list)
{
	log_info("read_history_lan_list\n");
	FILE* fd;
	int i = 0;
	char buffer[256] = {0};
	fd=fopen(HISTORY_LAN_LIST, "r");
	if(fd == NULL)
	{
		log_error("fd = NULL\n");
		return;
	}
	history_lan_list->cnt = 0;
	while(fgets(buffer,256,fd) != NULL)
	{
	
		char *temp = NULL;
		char *delim = " |";

		temp = strtok(buffer, delim);
		if(temp)
			strcpy(history_lan_list->list[i].mac, temp);
		
		temp = NULL;
		temp = strtok(NULL, delim);
		if(temp)
			strcpy(history_lan_list->list[i].ipaddr, temp);

		temp = NULL;
		temp = strtok(NULL, delim);
		if(temp)
			strcpy(history_lan_list->list[i].hostname, temp);

		temp = NULL;
		temp = strtok(NULL, delim);
		if(temp)
			strcpy(history_lan_list->list[i].interface, temp);

		temp = NULL;
		temp = strtok(NULL, delim);
		if(temp)
			strcpy(history_lan_list->list[i].ssid, temp);


		temp = NULL;
		temp = strtok(NULL, delim);
		if(temp)
			strcpy(history_lan_list->list[i].expires, temp);
			
		temp = NULL;
		temp = strtok(NULL, delim);
		log_info("read from file flow = %s\n", temp); 
		if(temp)
			history_lan_list->list[i].flow = atoi(temp);

		log_info("mac = %s, flow = %d\n", history_lan_list->list[i].mac,
										history_lan_list->list[i].flow);
		history_lan_list->cnt++;
		i++;
	}
	fclose(fd);
}


void echo_lan_list_to_file(LAN_LIST_T* lan_list, char* filename)
{
	int fd;
	int i = 0;
	char shellcmd[256] = "";
	system("rm -rf "CURRENT_LAN_LIST);
	fd=open(filename, O_RDWR | O_CREAT);
	if(fd == -1)
	{
		return;
	}
	log_info("echo_lan_list_to_file\n");
	
	for(i = 0; i < lan_list->cnt; i++)
	{
		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd, "%s |%s |%s |%s |%s |%s |%d\n",lan_list->list[i].mac, lan_list->list[i].ipaddr, lan_list->list[i].hostname,
			lan_list->list[i].interface, lan_list->list[i].ssid, lan_list->list[i].expires, lan_list->list[i].flow);
		write(fd, shellcmd, strlen(shellcmd));
	}

	close(fd);
}

void echo_history_list_to_file(LAN_LIST_T* lan_list, char* filename)
{
	int fd;
	int i = 0;
	char shellcmd[256] = "";
	system("rm -rf "HISTORY_LAN_LIST);
	
	fd=open(filename, O_RDWR | O_CREAT, 0644);
	if(fd == -1)
	{
		return;
	}
	log_info("echo_history_list_to_file\n");
	
	for(i = 0; i < lan_list->cnt; i++)
	{
		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd, "%s |%s |%s |%s |%s |%s |%d\n",lan_list->list[i].mac, lan_list->list[i].ipaddr, lan_list->list[i].hostname,
			lan_list->list[i].interface, lan_list->list[i].ssid, lan_list->list[i].expires, lan_list->list[i].flow);
	
		write(fd, shellcmd, strlen(shellcmd));
	}

	close(fd);
}



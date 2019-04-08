#include "include.h"
#include <errno.h>
#define QUOTA_CHAIN "forwarding_rule"

MAC_IP_MAP_LIST mac_ip_list;
MAC_FLOW_LIST history_record_mac_flow_list;

void restore_quota()
{
	log_info("restore_quota\n");
	memset(&mac_ip_list, 0, sizeof(MAC_IP_MAP_LIST));
	memset(&history_record_mac_flow_list, 0, sizeof(MAC_FLOW_LIST));
	char *table = "filter";
	struct xtc_handle *handle = NULL;
	if (NULL == handle)
			handle = iptc_init(table);

	if (NULL == handle)
	{
		log_error("libiptc:can't initialize iptables table `%s': %s\n",table, iptc_strerror(errno));
		return ;
	}
			

	if (!iptc_is_chain(QUOTA_CHAIN, handle)) 
	{
		if(!iptc_create_chain(QUOTA_CHAIN, handle))
		{
			log_error("Warning: using chain %s, not extension\n",QUOTA_CHAIN);
			return ;
		}

	}

	iptc_flush_entries(QUOTA_CHAIN, handle);
	iptc_commit(handle);

	iptc_free(handle);
	read_history_lan_list(&history_record_mac_flow_list);
	print_history_list("history lan list", &history_record_mac_flow_list);

}

static int get_the_flow_depend_ip(char* ipaddr)
{
	char *table = "filter";
	struct xtc_handle *handle = NULL;
	if (NULL == handle)
			handle = iptc_init(table);

	if (NULL == handle)
	{
		log_error("libiptc:can't initialize iptables table `%s': %s\n",table, iptc_strerror(errno));
		return ;
	}
			

	if (!iptc_is_chain(QUOTA_CHAIN, handle)) 
	{
		log_error("Warning: using chain %s, not extension\n",QUOTA_CHAIN);
		return ;
	}


	if(NULL == ipaddr)
	{
		log_error("ipaddr is NULL !");
		return -1;
	}
	int ret = 0,num=0;
	
	const struct ipt_entry *temp = NULL;
	long long upload_bytes = 0,download_bytes = 0;
	temp = iptc_first_rule(QUOTA_CHAIN,handle);

	while(temp != NULL)
	{
		
		if(temp->ip.src.s_addr == inet_addr(ipaddr) )
		{
			upload_bytes = temp->counters.bcnt;
			log_info("upload_bytes = %lld\n", upload_bytes);
		}
		else if (temp->ip.dst.s_addr == inet_addr(ipaddr))
		{
			download_bytes = temp->counters.bcnt;
			log_info("download_bytes = %lld\n", download_bytes);
		}

		temp = iptc_next_rule(temp,handle);
		num++;
	}
	iptc_free(handle);
	return upload_bytes+download_bytes;

}

static void add_to_history_record(char* mac, int flow)
{
	log_info("add_to_history_record %s %d\n", mac, flow);
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




static int check_quota_rule(const char * ipaddr)
{
	struct xtc_handle *handle = iptc_init("filter");
	if (NULL == handle)
	{
		log_error("libiptc:can't initialize iptables table `filter': %s\n", iptc_strerror(errno));
		return ;
	}
			
	if (!iptc_is_chain(QUOTA_CHAIN, handle)) 
	{
		log_error("Warning: using chain %s, not extension\n",QUOTA_CHAIN);
		return ;
	}

	if(NULL == ipaddr)
	{
		log_error("ipaddr is NULL !\n");
		return -1;
	}
	int ret = 0,num=0;
	
	const struct ipt_entry *temp = NULL;
	temp = iptc_first_rule(QUOTA_CHAIN,handle);

	while(temp != NULL)
	{
		if(temp->ip.src.s_addr == inet_addr(ipaddr) || 
			temp->ip.dst.s_addr == inet_addr(ipaddr))
		{
			
			iptc_free(handle);
			return 1;
		}

		temp = iptc_next_rule(temp,handle);
		num++;
	}
	iptc_free(handle);
	return 0;

}

int check_item_exist(LAN_LIST_T* list, const char* ipaddr)
{
	int i =0;
	for(i = 0; i < list->cnt; i++)
	{
		if(strcmp(list->list[i].ipaddr, ipaddr) == 0)
			return 1;
	}

	return 0;

}

#define u32  unsigned long
// add iptbales to forwarding_rule: iptables -s src_ip -j ACCEPT or iptables -d src_ip -j ACCEPT
struct ipt_entry *get_iptc_entry(char* src_ip, char* dest_ip)
{
    struct ipt_entry *fw = NULL;
    struct ipt_entry_target *target = NULL;
    struct nf_nat_multi_range *mr = NULL;
 
    u32 size1 = XT_ALIGN(sizeof(struct ipt_entry));
    //u32 size2 = XT_ALIGN(sizeof(struct ipt_entry_match) + sizeof(struct ipt_tcp));
    u32 size3 = XT_ALIGN(sizeof(struct ipt_entry_target) + sizeof(int));
 
    fw = calloc(1, size1  + size3);
    if ( !fw ) {
        log_info("Malloc failure\n");
        return NULL;
    }
 
    /* Offsets to the other bits */
    fw->target_offset = size1 ;
    fw->next_offset = size1 + size3;
 
    /* Set up packet matching rules */

	if(NULL != src_ip)
	{
		fw->ip.src.s_addr = inet_addr(src_ip); 
    	fw->ip.smsk.s_addr = -1;
	}

	if(NULL != dest_ip)
	{
		fw->ip.dst.s_addr = inet_addr(dest_ip); 
    	fw->ip.dmsk.s_addr = -1;
	}
    fw->ip.proto = 0;//all
    /* And the target */
    target = (struct ipt_entry_target *)(fw->elems);
    target->u.target_size = size3;
    strcpy(target->u.user.name, "ACCEPT");
    return fw;
}  


static int add_quota_rule_depend_ip(char* ipaddr)
{
	char *table = "filter";
	struct xtc_handle *handle = NULL;
	if (NULL == handle)
			handle = iptc_init(table);

	if (NULL == handle)
	{
		log_error("libiptc:can't initialize iptables table `%s': %s\n",table, iptc_strerror(errno));
		return ;
	}
			

	if (!iptc_is_chain(QUOTA_CHAIN, handle)) 
	{
		log_error("Warning: using chain %s, not extension\n",QUOTA_CHAIN);
		return ;
	}

	if(NULL == ipaddr)
	{
		log_error("ipaddr is NULL !");
		return -1;
	}
	int ret = 0;
	struct ipt_entry *s_fw = get_iptc_entry(ipaddr,NULL);
	struct ipt_entry *d_fw = get_iptc_entry(NULL, ipaddr);
	int ret1 = iptc_append_entry(QUOTA_CHAIN, s_fw, handle);
	int ret2 = iptc_append_entry(QUOTA_CHAIN, d_fw, handle);
	if(ret1 & ret2)
		ret = iptc_commit(handle);

	iptc_free(handle);
	free(s_fw);
	free(d_fw);
	return ret;
}

int delete_rule_by_num(int num)
{
	char *table = "filter";
	struct xtc_handle *handle = NULL;
	int ret = 0;
	if (NULL == handle)
			handle = iptc_init(table);

	if (NULL == handle)
	{
		log_error("libiptc:can't initialize iptables table `%s': %s\n",table, iptc_strerror(errno));
		return -1;
	}
			
	if (!iptc_is_chain(QUOTA_CHAIN, handle)) 
	{
		log_error("Warning: using chain %s, not extension\n",QUOTA_CHAIN);
		return -1;
	}

	iptc_delete_num_entry(QUOTA_CHAIN,num,handle);
	//log_info("before commit\n");
	ret = iptc_commit(handle);
	iptc_free(handle);
	return ret;

}
static int rm_quota_rule_depend_ip(char* ipaddr)
{

	char *table = "filter";
	struct xtc_handle *handle = NULL;
	if (NULL == handle)
			handle = iptc_init(table);

	if (NULL == handle)
	{
		log_error("libiptc:can't initialize iptables table `%s': %s\n",table, iptc_strerror(errno));
		return ;
	}
			

	if (!iptc_is_chain(QUOTA_CHAIN, handle)) 
	{
		log_error("Warning: using chain %s, not extension\n",QUOTA_CHAIN);
		return ;
	}


	if(NULL == ipaddr)
	{
		log_error("ipaddr is NULL !");
		return -1;
	}
	int ret = 0,num=0;
	
	const struct ipt_entry *temp = NULL;
	temp = iptc_first_rule(QUOTA_CHAIN,handle);

	while(temp != NULL && num < 100)
	{
		if(temp->ip.src.s_addr == inet_addr(ipaddr) || 
			temp->ip.dst.s_addr == inet_addr(ipaddr))
		{
			log_info("num = %d\n", num);
			iptc_free(handle);
			ret = delete_rule_by_num(num);
			
			handle = iptc_init(table);
			temp = iptc_first_rule(QUOTA_CHAIN,handle);
			num = 0;
			continue;
		}

		temp = iptc_next_rule(temp,handle);
		num++;
	}
	iptc_free(handle);
	return ret;
	
}


void refresh_quota_rule(LAN_LIST_T* current_lan_list,MAC_FLOW_LIST* history_lan_list)
{	
	int ret = 0;
	int i = 0, j = 0;

	for(i = 0; i < current_lan_list->cnt; i++)
	{
		char *p = current_lan_list->list[i].ipaddr;
		if(check_quota_rule(p))
		{
			current_lan_list->list[i].flow = get_the_flow_depend_ip(p);
		}
		else // new devices
		{
			
			ret = add_quota_rule_depend_ip(p);
				
			log_info("ret = %d\n", ret);
		}
		
	}

	for(j = 0;j < mac_ip_list.cnt;j++)
	{
		char *ip = mac_ip_list.mac_ip[j].ip;
		char *mac = mac_ip_list.mac_ip[j].mac;
		long long flow = get_the_flow_depend_ip(ip);

		log_info("mac_ip_list mac = %s, flow = %d\n", mac, flow);
		if(check_item_exist(current_lan_list, ip))
		{
			
			add_to_history_record(mac,flow);
			continue;
		}

		ret = rm_quota_rule_depend_ip(ip);
		log_info("rm ret = %d\n", ret);
		add_to_history_record(mac,flow);
	}

	mac_ip_list.cnt = 0;
	for(i = 0; i < current_lan_list->cnt; i++)
	{
		char *p = current_lan_list->list[i].ipaddr;
		char *mac =  current_lan_list->list[i].mac;
		int flow =   current_lan_list->list[i].flow;


		log_info("add to mac_ip_list ip = %s\n", p);
		log_info("add to mac_ip_list mac = %s\n", mac);
		strcpy(mac_ip_list.mac_ip[i].ip, p);  
		strcpy(mac_ip_list.mac_ip[i].mac, mac);
		mac_ip_list.mac_ip[i].flow = flow;
		mac_ip_list.cnt++;
	}

	history_lan_list->cnt = 0;
	for(j = 0; j < history_record_mac_flow_list.cnt; j++)
	{		
		strcpy(history_lan_list->mac_flow_item[j].mac, history_record_mac_flow_list.mac_flow_item[j].mac);
		history_lan_list->mac_flow_item[j].flow = history_record_mac_flow_list.mac_flow_item[j].flow;
		history_lan_list->cnt++;
	}
	
}


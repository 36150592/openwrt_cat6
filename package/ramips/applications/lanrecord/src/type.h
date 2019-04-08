#ifndef __TYPE_H_
#define __TYPE_H_

#include <stdio.h>
#include <syslog.h>
#define NORMAL_FIELD_LEN 64
#define NORMAL_ARRAY_SIZE 100
#define MINI_ARRAY_SIZE 50

typedef struct mac_addr
{
	char mac[NORMAL_FIELD_LEN];
}MAC_ADDR;

typedef struct mac_list
{
	int cnt;
	MAC_ADDR mac_item[MINI_ARRAY_SIZE];
}MAC_LIST;

typedef struct
{
	char mac[NORMAL_FIELD_LEN];
	char ip[NORMAL_FIELD_LEN];
	char ifname[NORMAL_FIELD_LEN];
}IPNEIGH_ITEM;


typedef struct
{
	int cnt;
	IPNEIGH_ITEM list[MINI_ARRAY_SIZE];
}IPNEIGH_ITEM_LIST;


typedef struct udhcpd_lease
{
	char mac[NORMAL_FIELD_LEN];
	char ipaddr[NORMAL_FIELD_LEN];
	char hostname[NORMAL_FIELD_LEN];
	char expires[NORMAL_FIELD_LEN];
}UDHCPD_LEASE;


typedef struct tz_udhcpd_lease_t
{
	int cnt;
	UDHCPD_LEASE lease[MINI_ARRAY_SIZE];
}TZ_UDHCPD_LEASE_T;

typedef struct lan_item_t
{
	char mac[NORMAL_FIELD_LEN];
	char ipaddr[NORMAL_FIELD_LEN];
	char interface[NORMAL_FIELD_LEN];
	char hostname[NORMAL_FIELD_LEN];
	char expires[NORMAL_FIELD_LEN];
	char ssid[NORMAL_FIELD_LEN];
	int flow;
}LAN_ITEM_T;

typedef struct lan_list_t
{
	int cnt;
	LAN_ITEM_T list[MINI_ARRAY_SIZE];
}LAN_LIST_T;

enum FILE_SIZE {
	MINI_FILE_BUFFER_SIZE = 512,
	NORMAL_FILE_BUFFER_SIZE = 1024 * 2,
	MAX_FILE_BUFFER_SIZE = 1024 * 16,
	FIND_AP_BUFFER_SIZE = 1024 * 100,
	IPTABLES_BUFFER_SIZE = 1024 * 8,
	MAX_POST_DATA_SIZE = 1024 * 100
};

typedef signed int            T_FFS_FD;
#define FL_TZ_UDHCPD_LEASES "/tmp/tz_udhcpd_leases"
#define FL_WIRELESS_SSID1_STA "/proc/wlan0-va0/sta_info"


#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

typedef struct split_struct
{
    char *lp_string;
    unsigned short length;
} SPLIT_STRUCT, *LP_SPLIT_STRUCT;

typedef struct mac_ip_map
{
	char mac[64];
	char ip[64];
	int  flow;
}MAC_IP_MAP;


typedef struct mac_ip_map_list
{
	int cnt;
	MAC_IP_MAP mac_ip[MINI_ARRAY_SIZE];
}MAC_IP_MAP_LIST;

typedef struct mac_flow
{
	char mac[64];
	int  flow;
}MAC_FLOW;

typedef struct mac_flow_list
{
	int cnt;
	MAC_FLOW mac_flow_item[MINI_ARRAY_SIZE];
}MAC_FLOW_LIST;

typedef struct double_mac_ip
{
	char new_mac[64]; 
	char new_ip[64];
	char old_mac[64];
	char old_ip[64];
}DOUBLE_MAC_IP;

typedef struct double_mac_ip_list
{
	int cnt;
	DOUBLE_MAC_IP double_mac_ip_item[MINI_ARRAY_SIZE];
}DOUBLE_MAC_IP_LIST;

//#define DEBUG

#ifndef DEBUG
#define log_error(...) syslog(LOG_ERR,__VA_ARGS__)
#define log_info(...) syslog(LOG_INFO,__VA_ARGS__)
#else
#define log_error(...) printf(__VA_ARGS__)
#define log_info(...) printf(__VA_ARGS__)
#endif

#endif


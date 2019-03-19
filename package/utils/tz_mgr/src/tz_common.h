#ifndef _TZ_COMMON_H_
#define _TZ_COMMON_H_

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

typedef unsigned char *  u_caddr_t;



#if 1//ndef ETH_FRAME_LEN
#define ETH_FRAME_LEN	1514
#endif

#if 1//ndef ETH_ALEN
#define ETH_ALEN 6
#endif

#define LENGTH_OF_ETHERNET_DST_MAC	6
#define LENGTH_OF_ETHERNET_SRC_MAC	6
#define LENGTH_OF_ETHERNET_TYPE		2
#define LENGTH_OF_ETHERNET_LEN		2
#define LENGTH_OF_ETHERNET_CHECKSUM	2
#define LENGTH_OF_FIELD	1
#define LENGTH_OF_FIELD_LENGTH	1

#define OFFSET_OF_ETHERNET_DST_MAC 0
#define OFFSET_OF_ETHERNET_SRC_MAC ( OFFSET_OF_ETHERNET_DST_MAC+LENGTH_OF_ETHERNET_DST_MAC )
#define OFFSET_OF_ETHERNET_TYPE ( OFFSET_OF_ETHERNET_SRC_MAC+LENGTH_OF_ETHERNET_SRC_MAC )
#define OFFSET_OF_ETHERNET_LEN ( OFFSET_OF_ETHERNET_TYPE+LENGTH_OF_ETHERNET_TYPE )
#define OFFSET_OF_ETHERNET_DATA ( OFFSET_OF_ETHERNET_LEN+LENGTH_OF_ETHERNET_LEN )


//search all the servers
#define CMD_TYPE_SEARCH_SERVER		0x01
//advertise the server
#define CMD_TYPE_SERVER_INFO		0x02
//acknowledge one cmd
#define CMD_TYPE_ACK_CMD			0x03
//send reboot sync message
#define CMD_TYPE_REBOOT_SYNC		0x04
//send reboot sync ack message
#define CMD_TYPE_REBOOT_SYNC_ACK	0x05
//send reboot ack message
#define CMD_TYPE_REBOOT_ACK			0x06
//send update sync message
#define CMD_TYPE_UPDATE_SYNC		0x07
//send update sync ack message
#define CMD_TYPE_UPDATE_SYNC_ACK	0x08
//send update ack message
#define CMD_TYPE_UPDATE_ACK			0x09
//probe the server
#define CMD_TYPE_PROBE_SERVER		0x0A
//probe the server
#define CMD_TYPE_SERVER_IND			0x0B
//send restore sync message
#define CMD_TYPE_RESTORE_SYNC		0x0C
//send restore sync ack message
#define CMD_TYPE_RESTORE_SYNC_ACK	0x0D
//send restore ack message
#define CMD_TYPE_RESTORE_ACK		0x0E

//send restore not reboot message
#define CMD_TYPE_ONLY_RESTORE_CONFIGS		0xE1
//send restore not reboot message ack
#define CMD_TYPE_ONLY_RESTORE_CONFIGS_REPLY		0xE2




#define CMD_INFO_TZ_ENABLE_WIFI					0X80
#define CMD_INFO_AP_SSID						0x81
#define CMD_INFO_AP_SECMODE						0x82
#define CMD_INFO_AP_WPA							0x83
#define CMD_INFO_AP_SECFILE						0x84
#define CMD_INFO_AP_CYPHER						0x85
#define CMD_INFO_PSK_KEY						0x86
#define CMD_INFO_AP_WEP_MODE_0					0X87
#define CMD_INFO_AP_PRIMARY_KEY_0				0X88
#define CMD_INFO_WEP_RADIO_NUM0_KEY_1			0X89
#define CMD_INFO_WEP_RADIO_NUM0_KEY_2			0X8A
#define CMD_INFO_WEP_RADIO_NUM0_KEY_3			0X8B
#define CMD_INFO_WEP_RADIO_NUM0_KEY_4			0X8C
#define CMD_INFO_PUREG							0X8D
#define CMD_INFO_PUREN							0X8E
#define CMD_INFO_AP_WPA_GROUP_REKEY				0X8F
#define CMD_INFO_AP_WPA_GMK_REKEY				0X90
#define CMD_INFO_AP_HIDESSID					0x91
#define CMD_INFO_DIAL_STATUS					0x92
#define CMD_INFO_SIGNAL_QUALITY					0x93
#define CMD_INFO_AP_IPADDR						0x94
#define CMD_INFO_AP_NETMASK						0x95
#define CMD_INFO_TZ_DHCP_IP_BEGIN				0x96
#define CMD_INFO_TZ_DHCP_IP_END					0x97
#define CMD_INFO_TZ_DHCP_EXPIRE_TIME			0x98
#define CMD_INFO_CONFIG_CHANGED_BY_SERVER		0x99
#define CMD_INFO_AP_PRIMARY_CH					0x9A
#define CMD_INFO_AP_CHMODE						0x9B
#define CMD_INFO_RTS							0x9C
#define CMD_INFO_TXPOWER						0x9D
#define CMD_INFO_TZ_ENABLE_WATCHDOG				0x9E
#define CMD_INFO_IP_NOT_THE_SAME_WITH_SERVER	0x9F
#define CMD_INFO_RATECTL						0XA0
#define CMD_INFO_MANRATE						0XA1
#define CMD_INFO_MANRETRIES						0XA2
#define CMD_INFO_WIFIWORKMODE					0XA3
#define CMD_INFO_TZ_ISOLATE_WLAN_CLIENTS		0XA4
#define CMD_INFO_AP_GATEWAY_IP				0xA5
#define CMD_INFO_TZ_SPECIAL_SSID_PREFIX		0xA6
#define CMD_INFO_TZ_WIFI_40M_ENABLE			0xA7
#define CMD_INFO_TZ_FOTA_UPGRADE_IP			0xA9
#define CMD_INFO_SIGNAL_QUALITY_COLOR		0xAA
#define CMD_INFO_IDU_VERSION					0xAB
#define CMD_INFO_RESEND_SEARCH_SERVER		0xAC



//data_len&0x01 to make sure the data length is even
#define util_add_field_to_frame(cmd,data,data_len) 	do\
													{\
														unsigned char temp_value=cmd;\
														util_add_data_to_frame(&temp_value,LENGTH_OF_FIELD);\
														temp_value=data_len;\
														util_add_data_to_frame(&temp_value,LENGTH_OF_FIELD_LENGTH);\
														if( ( data ) != NULL )\
														{\
															util_add_data_to_frame(data,data_len);\
														}\
														if( ( data_len )&0x01 )\
														{\
															temp_value=0;\
															util_add_data_to_frame(&temp_value,1);\
														}\
													}while(0)

#define util_add_data_to_frame(data_ptr,size)	do\
													{\
														if( real_frame_size + size > ETH_FRAME_LEN-20 )\
														{\
															return -1;\
														}\
														memcpy(p_ethernet_frame,data_ptr,size);\
														real_frame_size += size;	\
														p_ethernet_frame += size;	\
													}while(0)

// below is marco and functions for recv
#define util_get_common_data_from_frame(data_ptr,len)	do\
														{\
															if(current_len + len > frame_len)\
															{\
																return -1;\
															}\
															if( ( data_ptr ) && ( len ) )\
															{\
																memcpy(data_ptr,p_ethernet_frame, len);\
															}\
															p_ethernet_frame += len;\
															current_len += len;\
														}while(0)


#define STRCPY_S( dest,src ) 		{\
										snprintf(dest\
											,sizeof( dest )\
											,"%s"\
											,src\
											);\
									};

#define STRCAT_S( dest,src ) 		{\
										snprintf(dest+strlen( dest )\
											,sizeof( dest )-strlen( dest )\
											,"%s"\
											,src\
											);\
									};


typedef struct
{
	unsigned char value[6];
	unsigned char align_bytes[2];
}MacAddr;

typedef enum
{
	//initial state
	CLIENT_STATUS_INITIALIZED
	//find the server
	,CLIENT_STATUS_SEARCH_SERVER
	//get parameters from server
	,CLIENT_STATUS_REQUEST_PARAMETERS
	//download update file
	,CLIENT_STATUS_DOWNLOAD_UPDATE_FILE
	//update the system
	,CLIENT_STATUS_UPDATE_SYSTEM
	//reboot client and the server
	,CLIENT_STATUS_REBOOT_SYSTEM
	//daemon status
	,CLIENT_STATUS_DAEMON
}ClientStatus;

typedef enum
{
	AP_IPADDR_DEFINED,
	AP_NETMASK_DEFINED,
	TZ_DHCP_IP_BEGIN_DEFINED,
	TZ_DHCP_IP_END_DEFINED,
	TZ_DHCP_EXPIRE_TIME_DEFINED,
	AP_CHMODE_DEFINED,
	PUREG_DEFINED,
	PUREN_DEFINED,
	AP_WPA_GROUP_REKEY_DEFINED,
	AP_WPA_GMK_REKEY_DEFINED,
	RTS_DEFINED,
	RATECTL_DEFINED,
	MANRATE_DEFINED,
	MANRETRIES_DEFINED,
	WIFIWORKMODE_DEFINED,
	TZ_ENABLE_WATCHDOG_DEFINED,
	TZ_SPEED_PREFERRED_DEFINED,

	TZ_ENABLE_WIFI_DEFINED,
	AP_SSID_DEFINED,
	AP_HIDESSID_DEFINED,
	//æŒ‡å®šmacåœ°å€æ‰€ä½¿ç”¨çš„ä¿¡é“
	AP_PRIMARY_CH_DEFINED,
	//æŒ‡å®šmacå¯¹åº”çš„è®¾å¤‡çš„wifiå‘é€åŠŸçŽ‡
	TXPOWER_DEFINED,
	//WPA
	AP_SECMODE_DEFINED,
	AP_WPA_DEFINED,
	AP_SECFILE_DEFINED,
	PSK_KEY_DEFINED,
	AP_CYPHER_DEFINED,
	//WEP
	AP_WEP_MODE_0_DEFINED,
	AP_PRIMARY_KEY_0_DEFINED,
	WEP_RADIO_NUM0_KEY_1_DEFINED,
	WEP_RADIO_NUM0_KEY_2_DEFINED,
	WEP_RADIO_NUM0_KEY_3_DEFINED,
	WEP_RADIO_NUM0_KEY_4_DEFINED,

	//æ˜¯å¦é”å¡
	TZ_LOCK_CARD_DEFINED,
	//é”å¡ç±»åž‹
	TZ_LOCK_CARD_TYPE_DEFINED,
	//åˆå§‹PINç 
	TZ_INITIAL_PIN_CODE_DEFINED,
	//ä¿®æ”¹ä¹‹åŽçš„PINç 
	TZ_MODIFIED_PIN_CODE_DEFINED,
	//æ˜¯å¦é”å°åŒº,è¿™ä¸ªæ˜¯æŽ©ç ç±»åž‹:|æš‚æ—¶ä¸ç”¨|<é”4Gå°åŒº>|<é”3Gå°åŒº>|<é”2Gå°åŒº>|
	TZ_LOCK_CELL_ID_DEFINED,
	//é”å°åŒºçš„æ•°é‡,è¿™ä¸ªä¹Ÿæ˜¯æŽ©ç ç±»åž‹:|æš‚æ—¶ä¸ç”¨|<é”4Gå°åŒºæ•°é‡>|<é”3Gå°åŒºæ•°é‡>|<é”2Gå°åŒºæ•°é‡>|
	TZ_LOCK_CELL_ID_COUNT_DEFINED,
	//æ˜¯å¦å¼€å¯é€Ÿåº¦é™åˆ¶
	TZ_LIMIT_SPEED_ENABLED_DEFINED,
	//è¦é™åˆ¶çš„é€Ÿåº¦,10è¿›åˆ¶,è¦é™åˆ¶çš„é€Ÿåº¦ä¸º( TZ_LIMIT_SPEED_VALUE*1.5 ) KB/s
	TZ_LIMIT_SPEED_VALUE_DEFINED,
	TZ_LIMIT_UPLINK_SPEED_VALUE_DEFINED,
	TZ_IP_STACK_MODE_DEFINED,

	//æ‹¨å·æ–¹å¼:å…¶ä¸­<æ‹¨å·æ–¹å¼>çš„å€¼å¦‚ä¸‹:
	//0:æ— 
	//1:L2TP æ‹¨å·ä¸Šç½‘
	//2:PPPOE+L2TP æ‹¨å·ä¸Šç½‘
	TZ_L2TP_MODE_DEFINED,
	//LNS æœåŠ¡å™¨åœ°å€:
	TZ_LNS_SERVER_DEFINED,
	//éš§é“åç§°:
	TZ_TUNNEL_NAME_DEFINED,
	//éš§é“éªŒè¯å¯†ç 
	TZ_TUNNEL_PASS_DEFINED,
	//å®½å¸¦è´¦å·
	TZ_AUTH_NAME_DEFINED,
	//å®½å¸¦å¯†ç 
	TZ_AUTH_PASS_DEFINED,

	//é…ç½®æ–‡ä»¶ç‰ˆæœ¬ä¿¡æ¯,ä½¿ç”¨utf8ç¼–ç 
	TZ_CONFIG_FILE_VERSION_DEFINED,
	//é…ç½®æè¿°ä¿¡æ¯,è¿™ä¸ªå¯ä»¥æ˜¾ç¤ºåœ¨ç½‘é¡µä¸Šæ¥éªŒè¯é…ç½®æ–‡ä»¶çš„é…ç½®ä¿¡æ¯,ä¾¿äºŽäº†è§£æ˜¯å¦åŠ è½½äº†é…ç½®åŠé…ç½®çš„ä¿¡æ¯,ä½¿ç”¨utf8ç¼–ç 
	TZ_CONFIG_FILE_DESCRIPTION_DEFINED,
	//ä½¿ç”¨PPPè½¬å‘æ–¹æ³•æ¥å®žçŽ°PPPOE over L2TPæ–°çš„æ–¹æ³•
	TZ_ENABLE_PPP_FORWARD_DEFINED,
	LAN_LED_AS_LTE_STATUS_DEFINED,

	//PPPæ‹¨å·éœ€è¦ç”¨åˆ°çš„ç”¨æˆ·å
	TZ_PPP_USERNAME_DEFINED,
	//PPPæ‹¨å·éœ€è¦ç”¨åˆ°çš„å¯†ç 
	TZ_PPP_PASSWORD_DEFINED,

	//æ˜¯å¦è‡ªåŠ¨èŽ·å–DNSæœåŠ¡å™¨ä¿¡æ¯
	TZ_GET_DNS_AUTO_DEFINED,
	//é¦–é€‰DNS
	TZ_DNS_PRIMARY_DEFINED,
	//å¤‡ç”¨DNS
	TZ_DNS_SECONDARY_DEFINED,

	//è¦é”å®šçš„é¢‘æ®µ
	TZ_BAND_PREF_DEFINED,

	TZ_LTE_MODULE_MODE_DEFINED,
	TZ_LTE_MODULE_PRIORITY_DEFINED,
	TZ_HEARTBEAT_INTERVAL_DEFINED,
	TZ_SPECIAL_DEVICE_DEFINED,
	TZ_SERVICE_NUMBER_DEFINED,
	TZ_NTP_SERVER_DEFINED,
	TZ_SYS_UPDATE_SERVER_DEFINED,
	TZ_PRESTORE_FREQUENCY_FDD_DEFINED,
	TZ_PRESTORE_FREQUENCY_TDD_DEFINED,
	TZ_LOCK_IMSI_DEFINED,
	TZ_DEVICE_TYPE_DEFINED,
	//å…è®¸æŽ¥å…¥çš„ç”¨æˆ·æ•°çš„é™åˆ¶
	TZ_MAX_ACCESS_USER_NUM_DEFINED,
	//åŠ å¯†å¡ç›¸å…³çš„å®šä¹‰
	TZ_SUPPORT_SIM_ENCRYPT_DEFINED,
	TZ_ENCRYPT_CARD_VENDOR_ID_DEFINED,
	TZ_ENCRYPT_KEY_VERSION_DEFINED,
	TZ_ENCRYPT_CARD_TYPE_DEFINED,
	TZ_ENCRYPT_KEY_1_DEFINED,
	TZ_ENCRYPT_KEY_2_DEFINED,
	TZ_ENCRYPT_KEY_3_DEFINED,
	TZ_ENCRYPT_KEY_4_DEFINED,
	TZ_ENCRYPT_KEY_5_DEFINED,
	TZ_ENCRYPT_KEY_6_DEFINED,
	TZ_ENCRYPT_KEY_7_DEFINED,
	TZ_ENCRYPT_KEY_8_DEFINED,
	TZ_ENCRYPT_KEY_9_DEFINED,
	TZ_ENCRYPT_KEY_10_DEFINED,
	TZ_ENCRYPT_KEY_11_DEFINED,
	TZ_ENCRYPT_KEY_12_DEFINED,
	TZ_ENCRYPT_KEY_13_DEFINED,
	TZ_ENCRYPT_KEY_14_DEFINED,
	TZ_ENCRYPT_KEY_15_DEFINED,
	TZ_ENCRYPT_KEY_16_DEFINED,

	//é”æŒ‡å®šå°åŒºçš„å®šä¹‰
	//(.+)
	//\1_DEFINED,
	TZ_LOCK_ONE_CELL_DEFINED,
	TZ_LOCKED_EARFCN_DEFINED,
	TZ_LOCKED_PHYCELLID_DEFINED,
	TZ_LOCKED_Enode_ID_DEFINED,

	TZ_LNS_SERVER2_DEFINED,
	TZ_TUNNEL_NAME2_DEFINED,
	TZ_TUNNEL_PASS2_DEFINED,
	TZ_LNS_SERVER3_DEFINED,
	TZ_TUNNEL_NAME3_DEFINED,
	TZ_TUNNEL_PASS3_DEFINED,
	TZ_LNS_SERVER4_DEFINED,
	TZ_TUNNEL_NAME4_DEFINED,
	TZ_TUNNEL_PASS4_DEFINED,
	TZ_LNS_SERVER5_DEFINED,
	TZ_TUNNEL_NAME5_DEFINED,
	TZ_TUNNEL_PASS5_DEFINED,
	TZ_LNS_SERVER6_DEFINED,
	TZ_TUNNEL_NAME6_DEFINED,
	TZ_TUNNEL_PASS6_DEFINED,
	TZ_LNS_SERVER7_DEFINED,
	TZ_TUNNEL_NAME7_DEFINED,
	TZ_TUNNEL_PASS7_DEFINED,
	TZ_LNS_SERVER8_DEFINED,
	TZ_TUNNEL_NAME8_DEFINED,
	TZ_TUNNEL_PASS8_DEFINED,
	TZ_LNS_SERVER9_DEFINED,
	TZ_TUNNEL_NAME9_DEFINED,
	TZ_TUNNEL_PASS9_DEFINED,
	TZ_LNS_SERVER10_DEFINED,
	TZ_TUNNEL_NAME10_DEFINED,
	TZ_TUNNEL_PASS10_DEFINED,

	TZ_FLOW_THRESHOLD_DEFINED,
	TZ_FLOW_WARNING_DEFINED,

	TZ_ISOLATE_WLAN_CLIENTS_DEFINED,

	TZ_L2TP_MTU_DEFINED,

	INFO_STRUCT_FIELD_DEFINED_MAX,
}InfoStructFieldDefined;

typedef struct
{
	char AP_IPADDR[32];
	char AP_NETMASK[32];
	char AP_GATEWAY[32];
	char TZ_DHCP_IP_BEGIN[32];
	char TZ_DHCP_IP_END[32];
	char TZ_DHCP_EXPIRE_TIME[32];
	char AP_CHMODE[16];
	char PUREG[16];
	char PUREN[16];
	char AP_WPA_GROUP_REKEY[16];
	char AP_WPA_GMK_REKEY[16];
	char RTS[16];
	char RATECTL[16];
	char MANRATE[16];
	char MANRETRIES[16];
	char WIFIWORKMODE[32];
	char TZ_ENABLE_WATCHDOG[16];
	//é€Ÿåº¦ä¼˜å…ˆæ¨¡å¼
	char TZ_SPEED_PREFERRED[16];

	char TZ_ENABLE_WIFI[16];
	char AP_SSID[64];
	char AP_HIDESSID[16];
	//æŒ‡å®šmacåœ°å€æ‰€ä½¿ç”¨çš„ä¿¡é“
	char AP_PRIMARY_CH[16];
	//æŒ‡å®šmacå¯¹åº”çš„è®¾å¤‡çš„wifiå‘é€åŠŸçŽ‡
	char TXPOWER[8];
	//WPA
	char AP_SECMODE[16];
	char AP_WPA[16];
	char AP_SECFILE[16];
	char PSK_KEY[128];
	char AP_CYPHER[32];
	//WEP
	char AP_WEP_MODE_0[16];
	char AP_PRIMARY_KEY_0[16];
	char WEP_RADIO_NUM0_KEY_1[32];
	char WEP_RADIO_NUM0_KEY_2[32];
	char WEP_RADIO_NUM0_KEY_3[32];
	char WEP_RADIO_NUM0_KEY_4[32];

	char TZ_IDU_VERSION_INFO[32];

	char TZ_ISOLATE_WLAN_CLIENTS[8];
	char TZ_WIFI_40M_ENABLE[8];

	char TZRSRP[8];
	char TZRSRQ[8];
	char TZSINR[8];

	//please don't change this paramter's position
	int before_nvram_parameter_field;
	int dial_status;
	int signal_quality;
	int signal_quality_color;
	int config_changed_by_server;
	int is_ip_not_the_same_with_server;
	unsigned int defined_fields[ ( INFO_STRUCT_FIELD_DEFINED_MAX+31 )>>5 ];
}InfoStruct;




int util_receive_ethernet_frame(int socket_handle,unsigned char * ethernet_frame,int *recved_data_len);
unsigned short ip_checksum(const void *data, int len);
int util_send_ethernet_frame(
							int socket_handle
							,unsigned short ethernet_frame_type
							,unsigned char *ethernet_data
							,int ethernet_data_len
							,unsigned char* src_mac
							,unsigned char* dst_mac
							);
int cmd_file_exist(const char* file_name);

int config_commit();
const char* config_get_string(const char *sec, const char *opt, const char *def);
int config_set_string(const char *sec, const char *opt, const char *val);
void config_deinit();
int config_init(const char *name);

int file_exists(const char *file);


#endif


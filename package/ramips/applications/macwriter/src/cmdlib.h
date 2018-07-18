#ifndef _CMD_LIB_H_
#define _CMD_LIB_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stddef.h"

#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include "unistd.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include "dirent.h"
#include <time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>   /* The L2 protocols */
#include <signal.h>   // for signal()
#include <sys/time.h> // struct itimeral. Linux setitimer()
#include <syslog.h>
#include <errno.h>
#else
#include <time.h>
#include "stdio.h"
#include "windows.h"
#include "PCOMM.H"
#endif

#ifdef WIN32
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif

//输出所有的调试信息,包括锁PIN码这些关键的信息
//#define ENABLE_OUTPUT_ALL_DEBUG_INFO
//支持锁卡/锁小区功能
#define CONFIG_LOCK_CARD_AND_CELL

#ifdef WIN32
#define debug(...) printf(__VA_ARGS__)
#else


#ifndef LINT_CHECK_ENABLE

#if 0
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...) syslog(LOG_INFO,__VA_ARGS__)
#endif

#else

#define debug() {}

#endif



#endif

#define CMD_IS_PROGRAM_RUNNING(ret_code) (ret_code&0xff)
#define CMD_PROGRAM_PID(ret_code) (ret_code>>8)

#define MAX_VALUE(a,b) ( ( (a) >= (b) )?( a ):( b ) )
#define MIN_VALUE(a,b) ( ( (a) >= (b) )?( b ):( a ) )

//进行安全的字符串COPY操作,注意如果dest不能使用获取sizeof( dest )获取大小,则不能使用该函数
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

#define SIM_CARD_NOT_PRESENT 0
#define SIM_CARD_PRESENT 1
#define SIM_CARD_STATUS_UNKMOWN -1


#define ERR_CODE_NONE 0
#define ERR_CODE_NO_SIM 1
#define ERR_CODE_NEED_PIN 2
#define ERR_CODE_NEED_PUK 3
#define ERR_CODE_WRONG_PIN 4
#define ERR_CODE_NEED_NEW_PIN 5
#define ERR_CODE_CAN_NOT_REGISTER 6
#define ERR_CODE_SMS_FILE_NOT_EXIST 7
#define ERR_CODE_RECIPIENT_NOT_EXIST 8
#define ERR_CODE_INVALID_SMS_INDEX 9
#define ERR_CODE_INVALID_SMS_MEMORY_STORAGE 10
#define ERR_CODE_USB_SERIAL_NOT_EXIST 11
#define ERR_CODE_SYSTEM_NO_RESPONSE 12
#define ERR_CODE_EXECUTE_FAIL 13
#define ERR_CODE_WRONG_PUK 14
#define ERR_CODE_WRONG_PARAMETERS 15


#define SET_ERR_CODE( error_code,error ){\
												error_code=( error_code )|( 1<<( error ) );\
											}

#define CLEAR_ERR_CODE( error_code,error ){\
												error_code=( error_code )&( ~( 1<<( error ) ) );\
											}

#define IS_ERR_CODE_SET( error_code,error ) ( ( error_code )&( 1<<( error ) ) )


#define MAX_MAC_COUNT 20

typedef struct
{
	unsigned int high;
	unsigned int low;
}BigNum;

typedef struct
{
	unsigned char value[6];
//用于字节对齐的两个字节,保证真个结构体占用8个字节
	unsigned char align_bytes[2];
}MacAddr;

typedef struct
{
	unsigned char value[8];
}EUI64;

typedef enum
{
	IPV6_NEIGH_STATUS_FAILED,
	IPV6_NEIGH_STATUS_INCOMPLETE,
	IPV6_NEIGH_STATUS_PROBE,
	IPV6_NEIGH_STATUS_DELAY,
	IPV6_NEIGH_STATUS_STALE,
	IPV6_NEIGH_STATUS_REACHABLE
}NeighStatus;

typedef struct
{
	char ipv6_addr[64];
	char mac_addr[32];
	int status;
	int is_router;
}Neighbour_T;


typedef struct
{
	Neighbour_T neighbours[10];
	int neighbour_count;
}Neighbour_Info;

//mac<->地址映射对
typedef struct
{
	MacAddr mac;
	//指定mac地址所使用的ip地址
	char AP_IPADDR[64];
	char TZ_ENABLE_WIFI[16];
	char AP_SSID[64];
	char AP_HIDESSID[16];
	//指定mac地址所使用的信道
	char AP_PRIMARY_CH[16];
	//指定mac对应的设备的wifi发送功率
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

	char AP_CHMODE[16];
	char PUREG[16];
	char PUREN[16];
	char RTS[16];
	char RATECTL[16];
	char MANRATE[16];
	char MANRETRIES[16];
	char WIFIWORKMODE[32];
	char TZ_ISOLATE_WLAN_CLIENTS[32];
}MacIPPair;

//用于保存所有的信息
typedef struct
{
	MacIPPair ip_pair_info[ MAX_MAC_COUNT ];
	int mac_count;
}MacControlInfo;

typedef struct
{
	MacAddr macs[MAX_MAC_COUNT];
	int mac_count;
}MacBuffer;


//sublime text replace
//typedef struct
//typedef enum
//\}InfoStruct;
//}InfoStructFieldDefined;
//char\s+([^\[]*)\[\s*([^\]]*)\s*\];
//\1_DEFINED,
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
	//指定mac地址所使用的信道
	AP_PRIMARY_CH_DEFINED,
	//指定mac对应的设备的wifi发送功率
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

	//是否锁卡
	TZ_LOCK_CARD_DEFINED,
	//锁卡类型
	TZ_LOCK_CARD_TYPE_DEFINED,
	//初始PIN码
	TZ_INITIAL_PIN_CODE_DEFINED,
	//修改之后的PIN码
	TZ_MODIFIED_PIN_CODE_DEFINED,
	//是否锁小区,这个是掩码类型:|暂时不用|<锁4G小区>|<锁3G小区>|<锁2G小区>|
	TZ_LOCK_CELL_ID_DEFINED,
	//锁小区的数量,这个也是掩码类型:|暂时不用|<锁4G小区数量>|<锁3G小区数量>|<锁2G小区数量>|
	TZ_LOCK_CELL_ID_COUNT_DEFINED,
	//是否开启速度限制
	TZ_LIMIT_SPEED_ENABLED_DEFINED,
	//要限制的速度,10进制,要限制的速度为( TZ_LIMIT_SPEED_VALUE*1.5 ) KB/s
	TZ_LIMIT_SPEED_VALUE_DEFINED,
	TZ_LIMIT_UPLINK_SPEED_VALUE_DEFINED,
	TZ_IP_STACK_MODE_DEFINED,

	//拨号方式:其中<拨号方式>的值如下:
	//0:无
	//1:L2TP 拨号上网
	//2:PPPOE+L2TP 拨号上网
	TZ_L2TP_MODE_DEFINED,
	//LNS 服务器地址:
	TZ_LNS_SERVER_DEFINED,
	//隧道名称:
	TZ_TUNNEL_NAME_DEFINED,
	//隧道验证密码
	TZ_TUNNEL_PASS_DEFINED,
	//宽带账号
	TZ_AUTH_NAME_DEFINED,
	//宽带密码
	TZ_AUTH_PASS_DEFINED,

	//配置文件版本信息,使用utf8编码
	TZ_CONFIG_FILE_VERSION_DEFINED,
	//配置描述信息,这个可以显示在网页上来验证配置文件的配置信息,便于了解是否加载了配置及配置的信息,使用utf8编码
	TZ_CONFIG_FILE_DESCRIPTION_DEFINED,
	//使用PPP转发方法来实现PPPOE over L2TP新的方法
	TZ_ENABLE_PPP_FORWARD_DEFINED,
	LAN_LED_AS_LTE_STATUS_DEFINED,

	//PPP拨号需要用到的用户名
	TZ_PPP_USERNAME_DEFINED,
	//PPP拨号需要用到的密码
	TZ_PPP_PASSWORD_DEFINED,

	//是否自动获取DNS服务器信息
	TZ_GET_DNS_AUTO_DEFINED,
	//首选DNS
	TZ_DNS_PRIMARY_DEFINED,
	//备用DNS
	TZ_DNS_SECONDARY_DEFINED,

	//要锁定的频段
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
	//允许接入的用户数的限制
	TZ_MAX_ACCESS_USER_NUM_DEFINED,
	//加密卡相关的定义
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

	//锁指定小区的定义
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

	TZ_MULTI_TUNNEL_ENABLED_DEFINED,
	TZ_MULTI_TUNNEL_NAME_DEFINED,
	TZ_MULTI_TUNNEL_PASS_DEFINED,
	TZ_MULTI_TUNNEL_NAME2_DEFINED,
	TZ_MULTI_TUNNEL_PASS2_DEFINED,
	TZ_MULTI_TUNNEL_NAME3_DEFINED,
	TZ_MULTI_TUNNEL_PASS3_DEFINED,
	TZ_MULTI_TUNNEL_NAME4_DEFINED,
	TZ_MULTI_TUNNEL_PASS4_DEFINED,

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
	//速度优先模式
	char TZ_SPEED_PREFERRED[16];

	char TZ_ENABLE_WIFI[16];
	char AP_SSID[64];
	char AP_HIDESSID[16];
	//指定mac地址所使用的信道
	char AP_PRIMARY_CH[16];
	//指定mac对应的设备的wifi发送功率
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

	//是否锁卡
	char TZ_LOCK_CARD[ 8 ];
	//锁卡类型
	char TZ_LOCK_CARD_TYPE[ 8 ];
	//初始PIN码
	char TZ_INITIAL_PIN_CODE[ 16 ];
	//修改之后的PIN码
	char TZ_MODIFIED_PIN_CODE[ 16 ];
	//是否锁小区,这个是掩码类型:|暂时不用|<锁4G小区>|<锁3G小区>|<锁2G小区>|
	char TZ_LOCK_CELL_ID[ 16 ];
	//锁小区的数量,这个也是掩码类型:|暂时不用|<锁4G小区数量>|<锁3G小区数量>|<锁2G小区数量>|
	char TZ_LOCK_CELL_ID_COUNT[ 16 ];
	//是否开启速度限制
	char TZ_LIMIT_SPEED_ENABLED[ 16 ];
	//要限制的速度,10进制,要限制的速度为( TZ_LIMIT_SPEED_VALUE*1.5 ) KB/s
	char TZ_LIMIT_SPEED_VALUE[ 16 ];
	char TZ_LIMIT_UPLINK_SPEED_VALUE[ 16 ];
	char TZ_IP_STACK_MODE[ 16 ];


	//拨号方式:其中<拨号方式>的值如下:
	//0:无
	//1:L2TP 拨号上网
	//2:PPPOE+L2TP 拨号上网
	char TZ_L2TP_MODE[ 4 ];
	//LNS 服务器地址:
	char TZ_LNS_SERVER[ 64 ];
	//隧道名称:
	char TZ_TUNNEL_NAME[ 128 ];
	//隧道验证密码
	char TZ_TUNNEL_PASS[ 128 ];
	//宽带账号
	char TZ_AUTH_NAME[ 128 ];
	//宽带密码
	char TZ_AUTH_PASS[ 128 ];

	//LNS 服务器地址:
	char TZ_LNS_SERVER2[ 64 ];
	//隧道名称:
	char TZ_TUNNEL_NAME2[ 128 ];
	//隧道验证密码
	char TZ_TUNNEL_PASS2[ 128 ];
	//LNS 服务器地址:
	char TZ_LNS_SERVER3[ 64 ];
	//隧道名称:
	char TZ_TUNNEL_NAME3[ 128 ];
	//隧道验证密码
	char TZ_TUNNEL_PASS3[ 128 ];
	//LNS 服务器地址:
	char TZ_LNS_SERVER4[ 64 ];
	//隧道名称:
	char TZ_TUNNEL_NAME4[ 128 ];
	//隧道验证密码
	char TZ_TUNNEL_PASS4[ 128 ];
	//LNS 服务器地址:
	char TZ_LNS_SERVER5[ 64 ];
	//隧道名称:
	char TZ_TUNNEL_NAME5[ 128 ];
	//隧道验证密码
	char TZ_TUNNEL_PASS5[ 128 ];
	//LNS 服务器地址:
	char TZ_LNS_SERVER6[ 64 ];
	//隧道名称:
	char TZ_TUNNEL_NAME6[ 128 ];
	//隧道验证密码
	char TZ_TUNNEL_PASS6[ 128 ];
	//LNS 服务器地址:
	char TZ_LNS_SERVER7[ 64 ];
	//隧道名称:
	char TZ_TUNNEL_NAME7[ 128 ];
	//隧道验证密码
	char TZ_TUNNEL_PASS7[ 128 ];
	//LNS 服务器地址:
	char TZ_LNS_SERVER8[ 64 ];
	//隧道名称:
	char TZ_TUNNEL_NAME8[ 128 ];
	//隧道验证密码
	char TZ_TUNNEL_PASS8[ 128 ];
	//LNS 服务器地址:
	char TZ_LNS_SERVER9[ 64 ];
	//隧道名称:
	char TZ_TUNNEL_NAME9[ 128 ];
	//隧道验证密码
	char TZ_TUNNEL_PASS9[ 128 ];
	//LNS 服务器地址:
	char TZ_LNS_SERVER10[ 64 ];
	//隧道名称:
	char TZ_TUNNEL_NAME10[ 128 ];
	//隧道验证密码
	char TZ_TUNNEL_PASS10[ 128 ];

	//是否开启多隧道模式
	char TZ_MULTI_TUNNEL_ENABLED[ 16 ];
	//隧道名称:
	char TZ_MULTI_TUNNEL_NAME[ 128 ];
	//隧道验证密码
	char TZ_MULTI_TUNNEL_PASS[ 128 ];

	char TZ_MULTI_TUNNEL_NAME2[ 128 ];
	char TZ_MULTI_TUNNEL_PASS2[ 128 ];

	char TZ_MULTI_TUNNEL_NAME3[ 128 ];
	char TZ_MULTI_TUNNEL_PASS3[ 128 ];

	char TZ_MULTI_TUNNEL_NAME4[ 128 ];
	char TZ_MULTI_TUNNEL_PASS4[ 128 ];

	//配置文件版本信息,使用utf8编码
	char TZ_CONFIG_FILE_VERSION[ 256 ];
	//配置描述信息,这个可以显示在网页上来验证配置文件的配置信息,便于了解是否加载了配置及配置的信息,使用utf8编码
	char TZ_CONFIG_FILE_DESCRIPTION[ 768 ];
	//使用PPP转发方法来实现PPPOE over L2TP新的方法
	char TZ_ENABLE_PPP_FORWARD[ 4 ];
	char LAN_LED_AS_LTE_STATUS[ 4 ];

	//PPP拨号需要用到的用户名
	char TZ_PPP_USERNAME[ 64 ];
	//PPP拨号需要用到的密码
	char TZ_PPP_PASSWORD[ 64 ];

	//是否自动获取DNS服务器信息
	char TZ_GET_DNS_AUTO[ 4 ];
	//首选DNS
	char TZ_DNS_PRIMARY[ 64 ];
	//备用DNS
	char TZ_DNS_SECONDARY[ 64 ];

	//要锁定的频段
	char TZ_BAND_PREF[ 20 ];

	//网络模式设置
	char TZ_LTE_MODULE_MODE[ 8 ];
	//网络模式优先级设置
	char TZ_LTE_MODULE_PRIORITY[ 8 ];
	//网管客户端心跳时长
	char TZ_HEARTBEAT_INTERVAL[ 8 ];
	//设备识别标志
	char TZ_SPECIAL_DEVICE[ 64 ];

	//服务号码,用于处理特殊的短消息
	char TZ_SERVICE_NUMBER[ 64 ];

	//系统NTP服务器的IP地址/域名
	char TZ_NTP_SERVER[ 128 ];
	//TZ_SYS_UPDATE_SERVER
	char TZ_SYS_UPDATE_SERVER[ 128 ];
	char TZ_PRESTORE_FREQUENCY_FDD[ 256 ];
	char TZ_PRESTORE_FREQUENCY_TDD[ 256 ];
	char TZ_LOCK_IMSI[ 8 ];
	//真正的设备型号
	char TZ_DEVICE_TYPE[ 32 ];
	//允许接入的用户数量
	char TZ_MAX_ACCESS_USER_NUM[ 32 ];
	//加密卡相关的信息
	char TZ_SUPPORT_SIM_ENCRYPT[ 8 ];
	char TZ_ENCRYPT_CARD_VENDOR_ID[ 8 ];
	char TZ_ENCRYPT_KEY_VERSION[ 8 ];
	char TZ_ENCRYPT_CARD_TYPE[ 8 ];
	char TZ_ENCRYPT_KEY_1[ 48 ];
	char TZ_ENCRYPT_KEY_2[ 48 ];
	char TZ_ENCRYPT_KEY_3[ 48 ];
	char TZ_ENCRYPT_KEY_4[ 48 ];
	char TZ_ENCRYPT_KEY_5[ 48 ];
	char TZ_ENCRYPT_KEY_6[ 48 ];
	char TZ_ENCRYPT_KEY_7[ 48 ];
	char TZ_ENCRYPT_KEY_8[ 48 ];
	char TZ_ENCRYPT_KEY_9[ 48 ];
	char TZ_ENCRYPT_KEY_10[ 48 ];
	char TZ_ENCRYPT_KEY_11[ 48 ];
	char TZ_ENCRYPT_KEY_12[ 48 ];
	char TZ_ENCRYPT_KEY_13[ 48 ];
	char TZ_ENCRYPT_KEY_14[ 48 ];
	char TZ_ENCRYPT_KEY_15[ 48 ];
	char TZ_ENCRYPT_KEY_16[ 48 ];

	//是否要开启锁指定小区
	char TZ_LOCK_ONE_CELL[8];
	//要锁定的频点
	char TZ_LOCKED_EARFCN[32];
	//要锁定的物理小区
	char TZ_LOCKED_PHYCELLID[32];
	/*
	ECGI，E-UTRAN小区全局标识符。
	ECGI(E-UTRAN Cell Global Identifier)，E-UTRAN小区全局标识符。
	ECGI由PLMN+Cell Identity组成，用于在PLMN中全局标识一个小区。
	注释：PLMN(Public Land Mobile Network，公共陆地移动网络)，由MNC(MobileNetworkCode，移动网络代码)+MCC(Mobile Country Code，移动用户所属国家代码)组成。
	Cell Identity（小区标识）由EnodeB ID+Cell ID组成。其中包含28bit信息，前20bit表示EnodeB ID，后8bit表示Cell ID。
	ECGI全称（演进型-通用移动通信系统陆地无线接入网小区全局标识符）。

	eNodeB id也不是完全不能重复,只有一种情况可以重复，就是不同的国家，或者不同的运营商之间，才可以重复！

	1、ECGI(E-UTRAN Cell Global Identifier)(not more than 52 bits) = PLMN ID+ ECI(E-UTRAN Cell Identifier)
	PLMN ID (not more than 6 digits) = MCC(Mobile Country Code) + MNC(Mobile Network Code)
	ECI(E-UTRAN Cell Identifier) (28 Bits) = eNodeB ID + Cell ID
	ECGI(E-UTRAN Cell Global Identifier)肯定不能重复，所以，如果eNB要相同，注定了PLMN ID 不能相同；

	CELLID和ENODEBID是怎么换算的
	很多的网管中ECI其实是ECGI！

	ECGI (not more than 52 bits) = PLMN ID+ ECI
	PLMN ID (not more than 6 digits) = MCC + MNC
	ECI (28 Bits) = eNB ID + Cell ID

	所以分解来看，46000是PLMN ID（460 MCC+00 MNC）
	ECI，28bit长，采用7位16进制编码，X1X2X3X4X5X6X7，其中X1X2X3X4X5代表eNodeBID，X6X7为该小区在eNB内标识。

	109215491换算为16进制就是6827F03，所以eNB id就是6827F，Cell ID就是03。

	要锁的物理小区对应的Enode ID,10进制
	*/
	char TZ_LOCKED_Enode_ID[32];

	//流量提醒的最大值MB字节数
	char TZ_FLOW_THRESHOLD[32];
	//是否开启流量提醒,yes表示开启,其他表示不开启
	char TZ_FLOW_WARNING[32];

	//TZ_ISOLATE_WLAN_CLIENTS为yes表示隔离无线客户端,否则不隔离无线客户端;
	char TZ_ISOLATE_WLAN_CLIENTS[32];

	//支持对 MTU 值进行配置,默认 MTU 值为 1400 字节。 
	char TZ_L2TP_MTU[32];	


	//编译相关的信息,保存在文件/version中
	char type[ 32 ];
	char version[ 32 ];
	char hwversion[ 32 ];
	char device[ 32 ];
	char build[ 32 ];
	char branch[ 64 ];
	char os[ 32 ];
	char sha1[ 257 ];
	char customer[ 32 ];

	//模块的IMEI号
	char imei[48];
	//SIM卡的IMSI
	char imsi[48];
	//模块版本
	char module_version[64];
	//模块物理层版本
	char module_phy_version[128];
	//模块高层协议版本
	char module_hlp_version[128];
	//模块编译的时间日期
	char module_build_date[128];
	//接收信号强度
	char RSSI[8];
	//LAC,大区ID
	char LAC[32];
	//CellID,小区ID,
	char CellID[32];
	//iccid,卡的ICCID
	char iccid[32];
	//当前所注册的网络的MCC+MNC
	char PLMN[16];
	//当前所在的网络模式
	char AcT[16];
	char band_pref[64];
	char module_type[64];

	char TZEARFCN[32];
	char TZBAND[32];
	char TZBANDWIDTH[32];
	char TZTRANSMODE[32];
	char TZRSRP[32];
	char TZRSRQ[32];
	char TZSINR[32];
	char TZTA[32];
	char TZTXPOWER[32];
	char TZGLBCELLID[32];
	char TZPHYCELLID[32];

	//please don't change this paramter's position
	int before_nvram_parameter_field;
	int dial_status;
	int signal_quality;
	int config_changed_by_server;
	int is_ip_not_the_same_with_server;
	unsigned int defined_fields[ ( INFO_STRUCT_FIELD_DEFINED_MAX+31 )>>5 ];
}InfoStruct;

//检查指定的字段是否被设置
#define infostruct_is_field_defined( infostruct_pointer,field_magic_number ) ( ( infostruct_pointer )->defined_fields[ field_magic_number>>5 ]&( 1<<( field_magic_number&0x1f ) ) )
//设置指定的字段被赋值的标志
#define infostruct_set_field_defined( infostruct_pointer,field_magic_number ) { ( infostruct_pointer )->defined_fields[ field_magic_number>>5 ]=( infostruct_pointer )->defined_fields[ field_magic_number>>5 ]|( 1<<( field_magic_number&0x1f ) ); };


typedef struct
{
	//LNS 服务器地址:
	char* TZ_LNS_SERVER;
	//隧道名称:
	char* TZ_TUNNEL_NAME;
	//隧道验证密码
	char* TZ_TUNNEL_PASS;
	//标记不存在的服务器,避免总是选择同一个服务器
	int not_exist;
	//隧道的用户名和其他隧道的用户名称存在相同的情况
	int tunnel_name_not_unique;
}LNSServerItem;

//可以配置的LNS服务器的最大数量
#define MAX_LNS_COUNT_LIMIT 10

//所有的lns服务器信息
typedef struct
{
	LNSServerItem lns[ MAX_LNS_COUNT_LIMIT ];
	int lns_count;
}LNSServerInfo;

typedef struct
{
	int vendor_id;
	int product_id;
}UsbPair;

//小区ID的最大数量
#define CELLID_BUFFER_MAX_NUMBER 128

typedef struct
{
	unsigned int lac;
	unsigned int cellid;
	int is_valid;
}CellIdInfo;

typedef struct
{
	CellIdInfo all_4g_cell_ids[ CELLID_BUFFER_MAX_NUMBER ];
	CellIdInfo all_3g_cell_ids[ CELLID_BUFFER_MAX_NUMBER ];
	CellIdInfo all_2g_cell_ids[ CELLID_BUFFER_MAX_NUMBER ];
	//这部分用于以后扩展使用
	unsigned char tmp_unused_buffer[ 256 ];
	unsigned short checksum;
}BufferedCellIdInfo;

typedef struct
{
	char mac[32];
	char ip[64];
	char user[64];
	char port[8];
}ARPTableEntry;

typedef struct
{
	ARPTableEntry table[ 40 ];
	int count;
}ARPTable;

//#define CMDLIB_TEST 1

#define IPv4_EXIST_FLAG 0x01
#define IPv6_EXIST_FLAG 0x02

#define UDHCP_DNS_SYNC_FILE "/tmp/udhcp_ipv4.pid"
#define DHCLIENT_DNS_SYNC_FILE "/tmp/dhclient_ipv4.pid"
#define DNS_CONFIG_FILE "/etc/resolv.conf"
#define DNS_CONFIG_TEMP_FILE "/etc/resolv_slaac.conf"
#define INTERFACE_ID_TEMP_FILE "/tmp/tmp_modem_interface_id"
//客户端的配置信息,所有的信息都保存在该文件中
#define MAC_CONTROL_INFO_FILE "/tmp/.mac_control_info_file"
//对于客户端,用于保存当前连接的服务器,对于服务器,用于表示当前所有连上的客户端
#define ALL_MACS_TMP_FILE "/tmp/.all_macs_tmp_file"
#ifndef WIN32
#define NEED_TO_REBOOT_TEMP_FILE "/tmp/.need_to_reboot"
#define NEED_TO_RESTORE_SETTINGS_TEMP_FILE "/tmp/.need_to_restore_settings"
#define CONFIG_CHANGED_BY_SERVER_TEMP_FILE "/tmp/.config_changed_by_server"
#define REBOOT_BECAUSE_OF_CONFIG_CHANGED_TEMP_FILE "/tmp/.reboot_because_of_config_changed"
//用于进行web页面和配置服务器之间的同步
#define MAC_CONTROL_INFO_FILE_CHANGED_TMP_FILE "/tmp/.mac_control_info_file_changed"
#define UPLINK_CONNECTION_IS_OK_TMP_FILE "/tmp/.uplink_connection_is_ok_tmp_file"
//手动进行lte拨号
#define LTE_DIAL_MANUALLY "/tmp/.dial_lte_module_manually"
//从lte网络断开
#define LTE_DISCONNECT_FROM_NETWORK_IMMEDIATELY "/tmp/.disconnect_from_lte_network_immediately"
//MF210模块被使用
#define MF210_IS_USED "/tmp/.module_mf210_is_used"
//L1761模块被使用
#define L1761_IS_USED "/tmp/.module_l1761_is_used"
//ZM8620模块被使用
#define ZM8620_IS_USED "/tmp/.module_zm8620_is_used"
//使用的是SIM5360模块
#define SIM5360_IS_USED "/tmp/.module_sim5360_is_used"
//使用的是P500模块
#define P500_IS_USED "/tmp/.module_p500_is_used"
//串口ttyUSB1是否被使用
#define TTYUSB1_IS_USED "/tmp/.is_ttyUSB1_used"
#define TMP_CONFIG_FILE "/tmp/rc.module.conf"
//要求重新进行拨号
#define NEED_REBOOT_FLAG_FILE "/tmp/.need_to_reboot_flag_file"
//当前文件下载的进度
#define FILE_DOWNLOAD_PROGRESS_FILE "/tmp/.file_download_progress_file"
//系统更新结果码
#define SYS_UPDATE_RESULT_CODE "/tmp/.sys_update_result_code"
//模块升级上传进度文件
#define FILE_UPLOAD_PROGRESS_FILE "/tmp/.file_upload_progress_file"
//进行文件上传的结果码
#define FILE_UPLOAD_RESULT_CODE "/tmp/.file_upload_result_code"
//上次文件上传的最后一个索引
#define FILE_UPLOAD_INDEX_FILE "/tmp/.file_upload_index_file"
//模块重启对应的log文件
#define REBOOT_MODULE_LOG_FILE "/tmp/.reboot_module_log"
//保存的AT命令LOG文件
#define AT_LOG_FILE "/tmp/.send_at_log"
//下发从服务器获取IPv6 DNS信息
#define CONFIG_USE_ACQUIRED_IPV6_DNS_INFO "/tmp/.config_use_acquired_ipv6_dns_info"
//IPv6 DNS服务器列表文件
#define IPV6_DNS_SERVER_LIST "/tmp/.ipv6_dns_server_list"
#define IPV6_DNS_SERVER_LIST_WITH_COMMA "/tmp/.ipv6_dns_server_list_comma"
#define CONFIG_DISABLE_PNAT "/tmp/.config_disable_pnat_flag"
#define CONFIG_DISABLE_GSM "/tmp/.config_disable_gsm_flag"
#define CONFIG_DISABLE_WIFI "/tmp/.config_disable_wifi_flag"
#define CHECK_DOWNLINK_ROUTER "/tmp/.check_downlink_router_flag"
#define RUKU_SPECIAL_VERSION "/tmp/.is_ruku_special_version_flag"
//根据客户的需求写入的配置文件
#define REQUIREMENT_CONFIG_FILE "/usr/requirement_config_file.conf"
//对客户的需求配置文件进行反向获取的文件
#define TMP_REQUIREMENT_CONFIG_FILE "/tmp/.tmp_requirement_config_file.conf"
#define TMP_REQUIREMENT_CONFIG_FILE1 "/tmp/.tmp_requirement_config_file1.conf"
#define TMP_REQUIREMENT_CONFIG_FILE2 "/tmp/.tmp_requirement_config_file2.conf"
//根据客户的需求写入的配置文件
#define SYSTEM_REQUIREMENT_CONFIG_FILE "/usr/system_requirement_config_file.conf"
//对客户的需求配置文件进行反向获取的文件
#define TMP_SYSTEM_REQUIREMENT_CONFIG_FILE "/tmp/.system_tmp_requirement_config_file.conf"
#define TMP_SYSTEM_REQUIREMENT_CONFIG_FILE2 "/tmp/.system_tmp_requirement_config_file.conf2"
#define TMP_SYSTEM_REQUIREMENT_CONFIG_FILE3 "/tmp/.system_tmp_requirement_config_file.conf3"
//系统配置文件
#define RC_SYSTEM_CONFIG_FILE "/tmp/rc.system.conf"
//保存的小区信息
#define ALL_CELL_ID_INFO "/usr/saved_all_cell_id_info"
//超出指定小区,禁止进行上网
#define OUT_OF_CELL_ID_LIMIT "/tmp/.out_of_cell_id_limit"
#define SYSTEM_SPEED_LIMIT "/tmp/iptables_sys_speed_limit"
#define EXE_SYSTEM_SPEED_LIMIT "/etc/rc.d/rc.exe.sys_speed_limit"
#define EXE_SYSTEM_UPLINK_SPEED_LIMIT "/etc/rc.d/rc.exe.sys_uplink_speed_limit"
#define CURRENT_CELL_ID_FILE "/tmp/.current_cell_id_file"
//成功获取到IP地址/DNS信息/默认网关的标志
#define GET_IP_DNS_GATEWAY_FLAG_FILE "/tmp/.get_ip_dns_gateway_flag_file"
//xl2tpd 对应的控制文件
#define L2TP_CONTROL_PIPE "/var/run/xl2tpd/l2tp-control"
//WAN端口的默认网关地址
#define WAN_NETWORK_CARD_GATEWAY "/tmp/.wan_network_card_gateway"
//首选DNS
#define WAN_DNS1_IP_ADDRESS "/tmp/.wan_dns1_ip_address"
//备用DNS
#define WAN_DNS2_IP_ADDRESS "/tmp/.wan_dns2_ip_address"
//ppp对端的IP地址
#define PPP_PEER_IP_ADDRESS "/tmp/.ppp_peer_ip_address"
#define XL2TPD_DEFAULT_CONFIG_DIRECTORY "/tmp/xl2tpd"
#define XL2TPD_DEFAULT_CONFIG_FILE XL2TPD_DEFAULT_CONFIG_DIRECTORY"/xl2tpd.conf"
#define XL2TPD_DEFAULT_AUTH_FILE XL2TPD_DEFAULT_CONFIG_DIRECTORY"/l2tp-secrets"
//L2TP拨号状态文件
#define L2TP_DIAL_STATE_FLAG_FILE "/tmp/.l2tp_dial_state_flag_file"
//版本相关的信息
#define VERSION_INFO_FILE "/version"
#define SPECIAL_COMPILE_CONFIG_FILE "/etc/rc.special_compile_config_file"
#define GENERATED_TMP_CONFIG_FILE "/tmp/.src_tmp_config_file.conf"
#define GENERATED_TMP_CONFIG_FILE_MD5_SUM "/tmp/.src_tmp_config_file.conf.md5.txt"
#define SYNC_MODULE_INFO_FLAG_FILE "/tmp/.sync_module_info_flag_file"
//拨号中的错误信息
#define DIAL_ERROR_CODE_FLAG_FILE "/tmp/.dial_error_code_flag_file"
//PPP拨号需要使用到的文件
#define PPP_PEER_DIRECTORY "/etc/ppp/peers"
#define PPP_PEER_MF210 "mf210-common"
#define PPP_PEER_SIM5360 "sim5360-common"
#define PPP_PEER_MF210_NEW "mf210-common-new"
#define PPP_PEER_SIM5360_NEW "sim5360-common-new"
#define PPP_PEER_MF210_CONFIG_FILE PPP_PEER_DIRECTORY"/"PPP_PEER_MF210
#define PPP_PEER_SIM5360_CONFIG_FILE PPP_PEER_DIRECTORY"/"PPP_PEER_SIM5360
#define TMP_PPP_PEER_MF210_CONFIG_FILE "/tmp/"PPP_PEER_MF210_NEW
#define TMP_PPP_PEER_SIM5360_CONFIG_FILE "/tmp/"PPP_PEER_SIM5360_NEW
#define CURRENT_DEVICE_IS_P11PLUS "/tmp/.current_device_is_p11plus"
#define DISPLAY_WIFI_SETTINGS_PAGE "/tmp/.display_wifi_settings_page"
#define LAN_LED_AS_WLAN_STATUS "/tmp/.lan_led_as_wlan_status"
//用于保存L1761模块状态查询结果的文件
#define L1761_MODULE_REGISTER_STATUS "/tmp/.l1761_module_register_status"
//所有保存到小区信息
#define ALL_SAVED_CELL_INFO "/tmp/.all_saved_cell_info"
//如果需要网管客户端程序立即上报自己的信息,创建文件:,网管客户端检查到该文件之后,会自动进行信息的上报,并删除该文件.
#define RUN_SYSMANAGETOOL_IMMEDIATELY "/tmp/.run_sysmanagetool_immediately"
//网管客户端成功连接到服务器之后,创建文件:
#define SYSMANAGETOOL_CONNECT_TO_SERVER "/tmp/.sysmanagetool_connect_to_server"
// 关闭本地网页管理功能
#define CLOSE_LOCAL_MANAGE "/tmp/.close_local_manage"
// 关闭上网功能,文件存在时表示关闭，不存在时表示开放，关闭上网功能只能远程打开和关闭，关闭网页管理功能支持本地和远程，因此，两种状态都需要上报
#define CLOSE_INTERNET "/tmp/.close_internet"
// 用于保存远程服务器禁止终端上报的时间，单位：秒
#define REMOTE_SERVER_DISABLE_REPORT_SECONDS "/tmp/.remote_server_disable_report_seconds"
//使用的是ZTE 7510模块
#define ZTE7510_IS_USED "/tmp/.module_zte7510_is_used"
//上次成功连接到服务器的时间
#define LAST_TIME_CONNECT_TO_SERVER "/tmp/.last_time_connect_to_server"
//终端标识
#define GLOBAL_DEVICE_IDENTIFIER "/usr/global_device_identifier"
//将当前所使用的LNS服务器信息写入到文件中
#define SELECTED_LNS_SERVER_INFO "/tmp/.selected_lns_server_info"
//使用的是ZTE MC2716模块
#define MC2716_IS_USED "/tmp/.module_mc2716_is_used"
#define C5300V_IS_USED "/tmp/.module_c5300v_is_used"
#define ONLY_3_LAN_PORTS "/tmp/.only_3_lan_ports"
#define LAST_TX_BYTES_COUNTER "/tmp/.last_tx_bytes_counter"
#define LAST_RX_BYTES_COUNTER "/tmp/.last_rx_bytes_counter"
#define SAVED_IMSI_STRING_VALUE "/usr/saved_imsi_string_value"
#define SIM_CARD_NEED_PIN_FLAG_FILE "/tmp/.sim_card_need_pin_flag_file"
#define SIM_CARD_NOT_NEED_PIN_FLAG_FILE "/tmp/.sim_card_not_need_pin_flag_file"
//请使用指定的SIM卡
#define PLEASE_USE_SPECIFIED_CARD "/tmp/.please_use_specified_card"
//将发送的心跳包的内容写入到文件中
#define MANAGETOOL_HEARTBEAT_PACKET_CONTENT "/tmp/.managetool_heartbeat_packet_content"
//删除锁小区缓存文件
#define DELETE_CELL_LOCK_CACHE_FILE "/tmp/.delete_cell_lock_cache_file"
//网管下发了配置文件,需要重新进行读取
#define MANAGE_REQUIREMENT_CONFIG_FILE_CHANGED "/tmp/.manage_requirement_config_file_changed"
//最新的nvram中的配置信息
#define NEWEST_NVRAM_CONFIG_INFO_FILE "/tmp/.newest_nvram_config_info_file"
//P500模块网络连接是否完好
#define P500_NETWORK_LINK_CONNECTED_FILE "/proc/p500_link"
//用于限制用户数的配置规则文件
#define SYSTEM_USER_COUNT_LIMIT_RULE "/tmp/.system_user_count_limit_rule"
//所有连接的WLAN客户设备
#define ALL_WLAN_CLIENT_DEVICES_INFO "/tmp/.all_wlan_client_devices_info"
//所有注册到的小区信息
#define ALL_REGISTERED_CELL_ID_INFO "/tmp/.all_registered_cell_id_info"
//允许访问外网的MAC地址和IP地址
#define ALL_ALLOWED_IP_ADDRESS_INFO "/tmp/.all_allowed_ip_address_info"
//由于锁指定小区导致的模块重启
#define REBOOT_BECAUSE_OF_LOCK_ONE_CELL_FILE "/tmp/.reboot_because_of_lock_one_cell"
//锁指定小区失败
#define FAIL_TO_LOCK_ONE_CELL_FLAG_FILE "/tmp/.fail_to_lock_one_cell_flag_file"
//禁止发送锁指定小区的AT命令
#define NOT_SEND_LOCK_ONE_CELL_AT_COMMAND "/tmp/.not_send_lock_one_cell_at_command"
//当前使用的是U8300W模块
#define U8300W_IS_USED "/tmp/.module_u8300w_is_used"
#define SIM7100_IS_USED "/tmp/.module_sim7100_is_used"
//禁止发送锁指定小区的AT命令
#define GPIO_11_AS_RUN_LED "/tmp/.gpio_11_as_run_led"
//流量统计报警文件
#define FLOW_STATISTICS_WARNING_FILE "/tmp/.flow_statistics_warning_file"
#define SLM630B_IS_USED "/tmp/.module_slm630b_is_used"
//检查是否可以正常连接lns服务器
#define RECHECK_IF_LNS_SERVER_IS_OK "/tmp/.recheck_if_lns_server_is_ok"
#define ML7810_IS_USED "/tmp/.module_ml7810_is_used"
//读取到的EMCI信息总是写入该文件中
#define TMP_EMCI_INFO_FILE "/tmp/.tmp_emci_info_file"
#else
#define NEED_TO_REBOOT_TEMP_FILE "./need_to_reboot"
#define NEED_TO_RESTORE_SETTINGS_TEMP_FILE "./need_to_restore_settings"
#define CONFIG_CHANGED_BY_SERVER_TEMP_FILE "./config_changed_by_server"
#define REBOOT_BECAUSE_OF_CONFIG_CHANGED_TEMP_FILE "./reboot_because_of_config_changed"
#define MAC_CONTROL_INFO_FILE_CHANGED_TMP_FILE "./mac_control_info_file_changed"
#define UPLINK_CONNECTION_IS_OK_TMP_FILE "./uplink_connection_is_ok_tmp_file"
//手动进行lte拨号
#define LTE_DIAL_MANUALLY "./dial_lte_module_manually"
//从lte网络断开
#define LTE_DISCONNECT_FROM_NETWORK_IMMEDIATELY "./disconnect_from_lte_network_immediately"
//MF210模块被使用
#define MF210_IS_USED "./module_mf210_is_used"
//L1761模块被使用
#define L1761_IS_USED "./module_l1761_is_used"
//ZM8620模块被使用
#define ZM8620_IS_USED "./module_zm8620_is_used"
//使用的是SIM5360模块
#define SIM5360_IS_USED "./module_sim5360_is_used"
//使用的是P500模块
#define P500_IS_USED "./module_p500_is_used"
//串口ttyUSB1是否被使用
#define TTYUSB1_IS_USED "./is_ttyUSB1_used"
#define TMP_CONFIG_FILE "./rc.module.conf"
//要求重新进行拨号
#define NEED_REBOOT_FLAG_FILE "./need_to_reboot_flag_file"
//当前文件下载的进度
#define FILE_DOWNLOAD_PROGRESS_FILE "./file_download_progress_file"
//系统更新结果码
#define SYS_UPDATE_RESULT_CODE "./sys_update_result_code"
//模块升级上传进度文件
#define FILE_UPLOAD_PROGRESS_FILE "./file_upload_progress_file"
//进行文件上传的结果码
#define FILE_UPLOAD_RESULT_CODE "./file_upload_result_code"
//上次文件上传的最后一个索引
#define FILE_UPLOAD_INDEX_FILE "./file_upload_index_file"
//模块重启对应的log文件
#define REBOOT_MODULE_LOG_FILE "./reboot_module_log"
//保存的AT命令LOG文件
#define AT_LOG_FILE "./send_at_log"
//下发从服务器获取IPv6 DNS信息
#define CONFIG_USE_ACQUIRED_IPV6_DNS_INFO "./config_use_acquired_ipv6_dns_info"
//IPv6 DNS服务器列表文件
#define IPV6_DNS_SERVER_LIST "./ipv6_dns_server_list"
#define IPV6_DNS_SERVER_LIST_WITH_COMMA "./ipv6_dns_server_list_comma"
#define CONFIG_DISABLE_PNAT "./config_disable_pnat_flag"
#define CONFIG_DISABLE_GSM "./config_disable_gsm_flag"
#define CONFIG_DISABLE_WIFI "./config_disable_wifi_flag"
#define CHECK_DOWNLINK_ROUTER "./check_downlink_router_flag"
#define RUKU_SPECIAL_VERSION "./is_ruku_special_version_flag"
//根据客户的需求写入的配置文件
#define REQUIREMENT_CONFIG_FILE "./requirement_config_file.conf"
//对客户的需求配置文件进行反向获取的文件
#define TMP_REQUIREMENT_CONFIG_FILE "./tmp_requirement_config_file.conf"
#define TMP_REQUIREMENT_CONFIG_FILE1 "./tmp_requirement_config_file1.conf"
#define TMP_REQUIREMENT_CONFIG_FILE2 "./tmp_requirement_config_file2.conf"
//根据客户的需求写入的配置文件
#define SYSTEM_REQUIREMENT_CONFIG_FILE "./system_requirement_config_file.conf"
//对客户的需求配置文件进行反向获取的文件
#define TMP_SYSTEM_REQUIREMENT_CONFIG_FILE "./system_tmp_requirement_config_file.conf"
#define TMP_SYSTEM_REQUIREMENT_CONFIG_FILE2 "./system_tmp_requirement_config_file.conf2"
#define TMP_SYSTEM_REQUIREMENT_CONFIG_FILE3 "./system_tmp_requirement_config_file.conf3"
//系统配置文件
#define RC_SYSTEM_CONFIG_FILE "./rc.system.conf"
//保存的小区信息
#define ALL_CELL_ID_INFO "./saved_all_cell_id_info"
//超出指定小区,禁止进行上网
#define OUT_OF_CELL_ID_LIMIT "./out_of_cell_id_limit"
#define SYSTEM_SPEED_LIMIT "./iptables_sys_speed_limit"
#define EXE_SYSTEM_SPEED_LIMIT "./rc.exe.sys_speed_limit"
#define EXE_SYSTEM_UPLINK_SPEED_LIMIT "./rc.exe.sys_uplink_speed_limit"
#define CURRENT_CELL_ID_FILE "./current_cell_id_file"
//成功获取到IP地址/DNS信息/默认网关的标志
#define GET_IP_DNS_GATEWAY_FLAG_FILE "./get_ip_dns_gateway_flag_file"
//xl2tpd对应的控制文件
#define L2TP_CONTROL_PIPE "./l2tp-control"
//WAN端口的默认网关地址
#define WAN_NETWORK_CARD_GATEWAY "./wan_network_card_gateway"
//首选DNS
#define WAN_DNS1_IP_ADDRESS "./wan_dns1_ip_address"
//备用DNS
#define WAN_DNS2_IP_ADDRESS "./wan_dns2_ip_address"
//ppp对端的IP地址
#define PPP_PEER_IP_ADDRESS "./ppp_peer_ip_address"
#define XL2TPD_DEFAULT_CONFIG_FILE "./xl2tpd.conf"
#define XL2TPD_DEFAULT_AUTH_FILE "./l2tp-secrets"
//L2TP拨号状态文件
#define L2TP_DIAL_STATE_FLAG_FILE "./l2tp_dial_state_flag_file"
//版本相关的信息
#define VERSION_INFO_FILE "./version"
#define SPECIAL_COMPILE_CONFIG_FILE "./rc.special_compile_config_file"
#define GENERATED_TMP_CONFIG_FILE "./src_tmp_config_file.conf"
#define GENERATED_TMP_CONFIG_FILE_MD5_SUM "./src_tmp_config_file.conf.md5.txt"
#define SYNC_MODULE_INFO_FLAG_FILE "./sync_module_info_flag_file"
//拨号中的错误信息
#define DIAL_ERROR_CODE_FLAG_FILE "./dial_error_code_flag_file"
//PPP拨号需要使用到的文件
#define PPP_PEER_DIRECTORY "."
#define PPP_PEER_MF210 "mf210-common"
#define PPP_PEER_SIM5360 "sim5360-common"
#define PPP_PEER_MF210_NEW "mf210-common-new"
#define PPP_PEER_SIM5360_NEW "sim5360-common-new"
#define PPP_PEER_MF210_CONFIG_FILE PPP_PEER_DIRECTORY"/"PPP_PEER_MF210
#define PPP_PEER_SIM5360_CONFIG_FILE PPP_PEER_DIRECTORY"/"PPP_PEER_SIM5360
#define TMP_PPP_PEER_MF210_CONFIG_FILE "./"PPP_PEER_MF210_NEW
#define TMP_PPP_PEER_SIM5360_CONFIG_FILE "./"PPP_PEER_SIM5360_NEW
#define CURRENT_DEVICE_IS_P11PLUS "./current_device_is_p11plus"
#define DISPLAY_WIFI_SETTINGS_PAGE "./display_wifi_settings_page"
#define LAN_LED_AS_WLAN_STATUS "./lan_led_as_wlan_status"
//用于保存L1761模块状态查询结果的文件
#define L1761_MODULE_REGISTER_STATUS "./l1761_module_register_status"
//所有保存到小区信息
#define ALL_SAVED_CELL_INFO "./all_saved_cell_info"
//如果需要网管客户端程序立即上报自己的信息,创建文件:,网管客户端检查到该文件之后,会自动进行信息的上报,并删除该文件.
#define RUN_SYSMANAGETOOL_IMMEDIATELY "./run_sysmanagetool_immediately"
//网管客户端成功连接到服务器之后,创建文件:
#define SYSMANAGETOOL_CONNECT_TO_SERVER "./sysmanagetool_connect_to_server"
// 关闭网页管理功能
#define CLOSE_LOCAL_MANAGE "./close_local_manage"
// 关闭上网功能,文件存在时表示关闭，不存在时表示开放，关闭上网功能只能远程打开和关闭，关闭网页管理功能支持本地和远程，因此，两种状态都需要上报
#define CLOSE_INTERNET "./close_internet"
// 用于保存远程服务器禁止终端上报的时间，单位：秒
#define REMOTE_SERVER_DISABLE_REPORT_SECONDS "./remote_server_disable_report_seconds"
//使用的是ZTE 7510模块
#define ZTE7510_IS_USED "./module_zte7510_is_used"
//上次成功连接到服务器的时间
#define LAST_TIME_CONNECT_TO_SERVER "./last_time_connect_to_server"
//终端标识
#define GLOBAL_DEVICE_IDENTIFIER "./global_device_identifier"
//将当前所使用的LNS服务器信息写入到文件中
#define SELECTED_LNS_SERVER_INFO "./selected_lns_server_info"
//使用的是ZTE MC2716模块
#define MC2716_IS_USED "./module_mc2716_is_used"
#define C5300V_IS_USED "./module_c5300v_is_used"
#define ONLY_3_LAN_PORTS "./only_3_lan_ports"
#define LAST_TX_BYTES_COUNTER "./last_tx_bytes_counter"
#define LAST_RX_BYTES_COUNTER "./last_rx_bytes_counter"
#define SAVED_IMSI_STRING_VALUE "./saved_imsi_string_value"
#define SIM_CARD_NEED_PIN_FLAG_FILE "./sim_card_need_pin_flag_file"
#define SIM_CARD_NOT_NEED_PIN_FLAG_FILE "./sim_card_not_need_pin_flag_file"
#define PLEASE_USE_SPECIFIED_CARD "./please_use_specified_card"
//将发送的心跳包的内容写入到文件中
#define MANAGETOOL_HEARTBEAT_PACKET_CONTENT "./managetool_heartbeat_packet_content"
//删除锁小区缓存文件
#define DELETE_CELL_LOCK_CACHE_FILE "./delete_cell_lock_cache_file"
//删除锁小区缓存文件
#define MANAGE_REQUIREMENT_CONFIG_FILE_CHANGED "./manage_requirement_config_file_changed"
//最新的nvram中的配置信息
#define NEWEST_NVRAM_CONFIG_INFO_FILE "./newest_nvram_config_info_file"
//P500模块网络连接是否完好
#define P500_NETWORK_LINK_CONNECTED_FILE "./p500_link"
//用于限制用户数的配置规则文件
#define SYSTEM_USER_COUNT_LIMIT_RULE "./system_user_count_limit_rule"
//所有连接的WLAN客户设备
#define ALL_WLAN_CLIENT_DEVICES_INFO "./all_wlan_client_devices_info"
//所有注册到的小区信息
#define ALL_REGISTERED_CELL_ID_INFO "./all_registered_cell_id_info"
//允许访问外网的MAC地址和IP地址
#define ALL_ALLOWED_IP_ADDRESS_INFO "./all_allowed_ip_address_info"
//由于锁指定小区导致的模块重启
#define REBOOT_BECAUSE_OF_LOCK_ONE_CELL_FILE "./reboot_because_of_lock_one_cell"
//锁指定小区失败
#define FAIL_TO_LOCK_ONE_CELL_FLAG_FILE "./fail_to_lock_one_cell_flag_file"
//禁止发送锁指定小区的AT命令
#define NOT_SEND_LOCK_ONE_CELL_AT_COMMAND "./not_send_lock_one_cell_at_command"
//当前使用的是U8300W模块
#define U8300W_IS_USED "./module_u8300w_is_used"
#define SIM7100_IS_USED "./module_sim7100_is_used"
//禁止发送锁指定小区的AT命令
#define GPIO_11_AS_RUN_LED "./gpio_11_as_run_led"
//流量统计报警文件
#define FLOW_STATISTICS_WARNING_FILE "./flow_statistics_warning_file"
#define SLM630B_IS_USED "./module_slm630b_is_used"
//检查是否可以正常连接lns服务器
#define RECHECK_IF_LNS_SERVER_IS_OK "./recheck_if_lns_server_is_ok"
#define ML7810_IS_USED "./module_ml7810_is_used"
//读取到的EMCI信息总是写入该文件中
#define TMP_EMCI_INFO_FILE "./tmp_emci_info_file"

#endif



#ifndef WIN32
#define SMS_CONTENT_FOLDER "/tmp/sms"
#define OPERATORS_DEFINE_FILE "/tmp/.operators"
#define OPERATORS_DEFINE_BIN_FILE "/tmp/.bin.operators"
#define SEND_SMS_PID_FILE "/tmp/.sendsms.pid"
#define LIST_PLMN_PID_FILE "/tmp/.listplmn.pid"
#define MODULE_INFO_FILE "/tmp/.moduleinfo"
#define MODULE_INFO_FILE2 "/tmp/.moduleinfo2"
#define USB_SERIAL_EXCEPTION_FILE "/tmp/.usb_serial_have_exception"
#define RSSI_VALUE_IND_FILE "/tmp/.rssi_value_ind"
#define SMS_COUNT_SYNC_FILE "/tmp/.sms_count_sync"
#define SMS_COUNT_INFO_FILE "/tmp/.sms_count_info"
#else
#define SMS_CONTENT_FOLDER "."
#define OPERATORS_DEFINE_FILE "./operators"
#define OPERATORS_DEFINE_BIN_FILE "./bin.operators"
#define SEND_SMS_PID_FILE "./sendsms.pid"
#define LIST_PLMN_PID_FILE "./listplmn.pid"
#define MODULE_INFO_FILE "./moduleinfo"
#define MODULE_INFO_FILE2 "./moduleinfo2"
#define USB_SERIAL_EXCEPTION_FILE "./usb_serial_have_exception"
#define RSSI_VALUE_IND_FILE "./rssi_value_ind"
#define SMS_COUNT_SYNC_FILE "./sms_count_sync"
#define SMS_COUNT_INFO_FILE "./sms_count_info"
#endif


#ifndef __i386__
#define CONFIGLOADER_PATH ""
#else
#define CONFIGLOADER_PATH "../configloader/"
#endif

//成功加载配置
#define CONFIG_LOADING_ERROR_NONE 0
//没有指定配置文件
#define CONFIG_LOADING_ERROR_NO_CONFIG_FILE_SPECIFIED 1
//配置文件不存在
#define CONFIG_LOADING_ERROR_CONFIG_FILE_NOT_EXIST 2
//申请内存失败
#define CONFIG_LOADING_ERROR_NO_BUFFER_AVAILABLE 3
//不能打开配置文件
#define CONFIG_LOADING_ERROR_OPEN_CONFIG_FILE_FAILED 4
//配置文件校验和错误
#define CONFIG_LOADING_ERROR_FILE_CHECKSUM_ERROR 5
//配置文件大小错误
#define CONFIG_LOADING_ERROR_FILE_SIZE_ERROR 6
//其他错误
#define CONFIG_LOADING_ERROR_OTHER 7



//系统更新错误代码
//成功进行系统升级
#define SYSTEM_UPDATE_ERROR_NONE 0
//不需要进行更新
#define SYSTEM_UPDATE_ERROR_NO_NEED_TO_UPDATE 1
//当前已是最新版本,不需要进行升级
#define SYSTEM_UPDATE_ERROR_CURRENT_IS_LATEST_VERSION 2
//存在最新的升级文件
#define SYSTEM_UPDATE_ERROR_HAVE_UPDATE_FILE 3
//下载升级文件失败
#define SYSTEM_UPDATE_ERROR_FAIL_TO_DOWNLOAD_UPDATE_FILE 4
//下载升级文件大小存在问题
#define SYSTEM_UPDATE_ERROR_UPDATE_FILE_SIZE_WRONG 5
//升级文件损坏
#define SYSTEM_UPDATE_ERROR_UPDATE_FILE_CORRUPTED 6
//升级文件不存在
#define SYSTEM_UPDATE_ERROR_NO_UPDATE_FILE 7
//升级文件解压失败
#define SYSTEM_UPDATE_ERROR_CAN_NOT_DECOMPRESS_UPDATE_FILE 8
//升级文件对应的md5文件不存在
#define SYSTEM_UPDATE_ERROR_NO_UPDATE_MD5_FILE 9
//升级文件MD5错误
#define SYSTEM_UPDATE_ERROR_UPDATE_FILE_MD5_WRONG 10
//服务器响应超时
#define SYSTEM_UPDATE_ERROR_RESPONSE_TIMEOUT 11



//成功执行
#define UPLOAD_ERR_CODE_NONE 0
//USB串口不存在
#define UPLOAD_ERR_CODE_USB_SERIAL_NOT_EXIST 1
//USB串口没有响应
#define UPLOAD_ERR_CODE_SYSTEM_NO_RESPONSE 2
//AT命令执行失败
#define UPLOAD_ERR_CODE_EXECUTE_FAIL 3
//要上传的文件不存在
#define UPLOAD_ERR_CODE_FILE_NOT_EXIST 4
//文件非法
#define UPLOAD_ERR_CODE_FILE_NOT_VALID 5
//串口超时
#define UPLOAD_ERR_CODE_USB_SERIAL_TIMEOUT 6

//锁卡类型
typedef enum
{
	LOCK_SIM_TYPE_MIN=1,
	//锁卡类型,锁固定PIN码
	LOCK_SIM_TYPE_FIXED=LOCK_SIM_TYPE_MIN,
	//锁卡类型,锁固定ICCID
	LOCK_SIM_TYPE_ICCID=2,
	LOCK_SIM_TYPE_MAX,
}LOCK_SIM_TYPE;

//拨号方式:其中<拨号方式>的值如下:
//0:无
//1:L2TP 拨号上网
//2:PPPOE+L2TP 拨号上网
typedef enum
{
	TZ_L2TP_MODE_NONE=0,
	TZ_L2TP_MODE_L2TP=1,
	TZ_L2TP_MODE_PPPOE=2,
}TZ_L2TP_MODE;

typedef enum
{
	PRESTORE_FREQUENCY_LIST_TYPE_FDD,
	PRESTORE_FREQUENCY_LIST_TYPE_TDD,
}PRESTORE_FREQUENCY_LIST_TYPE;

//锁IMSI的策略
typedef enum
{
	LOCK_IMSI_STRATEGY_NONE,
	LOCK_IMSI_STRATEGY_LOCK_FIRST,
	LOCK_IMSI_STRATEGY_LOCK_NEW,
	LOCK_IMSI_STRATEGY_LOCK_MAX,
}LOCK_IMSI_STRATEGY;

//check if one file exist
int cmd_file_exist(const char* file_name);

//write string to one specified file
int cmd_echo(char* str,const char *file_name);
int cmd_echo_without_new_line(char* str,const char *file_name);
//append string to one specified file
int cmd_echo_append(char* str,const char *file_name);
//append string to one specified file
int cmd_echo_append_without_new_line(char* str,const char *file_name);

//write string to one specified file
int cmd_touch(const char *file_name);

//rm one file
int cmd_rm(const char *file_name);

//设置文件的权限为可执行
int cmd_chmodx(const char* file_path);
//设置文件的读写权限的可进行读写
int cmd_chmodrw(const char* file_path);

//print file content
int cmd_cat(const char* file_name,char* buffer,int buffer_size);

//copy one file's content to the other file
int cmd_cp(const char *src_file_name,const char *dest_file_name);

//move one file to the other
int cmd_mv(const char* src_file,const char* dest_file);

//network card exist or not
int cmd_netdev_exist(const char* dev_name);

//get the network card flow statistics
int cmd_netdev_flowstat(const char* dev_name,char* rx_bytes_buffer,char* tx_bytes_buffer);
int cmd_netdev_flowstat_ex(const char* dev_name,char* rx_bytes_buffer,char* tx_bytes_buffer,char* rx_packages_buffer,char* tx_packages_buffer);
//获取所有的参数信息,参数使用空格或\t进行分割,delimiter为0表示无效
int util_get_all_args( char* buffer,char delimiter1,char delimiter2,char* args[],int max_args_count );

//check if sim card exist
int cmd_sim_exist(void);
int cmd_sim_need_pin(void);
int cmd_sim_need_puk(void);

//get ip of the specified network card
int cmd_get_ip(const char* dev_name,struct in_addr* address);

//配置IP地址
int util_config_ipv4_addr(const char *net_dev, const char* ipaddr);

//check if ip addr exist
int cmd_ip_exist(const char* dev_name);

//check if ipv6 addr exist
int cmd_ipv6_exist(const char* dev_name);

//check if one program is running
int cmd_ps_is_one_program_running(char* cmd_name);
int cmd_ps_is_one_program_running_ex(char* cmd_name,char* match_string[],int match_string_count,char*pid_value );

//get one network card's mac addr
int cmd_netdev_get_mac_addr(const char* net_dev_name,MacAddr* mac_addr);

//calc eui64 by mac address
int util_calc_eui64(MacAddr* mac_addr,EUI64* eui64_addr);

int util_get_cmd_result(char *shellcmd, char *buffer, int size);

//check if get date and time success
int util_is_getting_date_and_time_ok(void);

//analyse ipv6 addr info
int analyse_ipv6_addr(char* ipv6_addr);

int subnet_ipv6_process(char* ipv6_addr,char* subnet_id,char* dest_ipv6_addr,int dest_prefix_len);

//resolve domain name to ipv4 or ipv6 addr
int util_resolv_domain_name(const char* domain_name,char* ipv4_addr,char* ipv6_addr,int* result_ind);

//parse the neigh info
int util_parse_neigh_info(const char* file_name,Neighbour_Info* neigh_info,int only_link_local_addr,int only_global_addr);

void config_enable_sharing_64prefix_to_a_lan(const char* ipv6_addr,const char* network_dev_name);
void config_ipv6_addr(const char*prefix,EUI64* eui64_addr,const char* network_dev_name,int modem_get_interface);
void config_default_ipv6_gateway(const char* ipv6_gateway,const char* network_dev_name);
void config_ipv6_dns_server_address(const char* global_dns_server1,const char* global_dns_server2,int dns_server_count,int nat64_mode);

//write interface info to file
int util_write_interface_id(struct in6_addr* local_addr6,const char* network_dev_name );
//read interface info from file
int util_read_interface_id(struct in6_addr* ipv6_addr);
unsigned short ip_checksum(const void *data, int len);
//get the dial status and signal quality
int cmd_get_dial_status_and_signal_quality( int* dial_status,int* signal_quality );
//update signal ind
int cmd_update_signal_ind( int signal_quality,int is_running_in_p11 );
int cmd_init_signal_ind_led( int is_running_in_p11 );
void util_init_lte_led( void );
//开启LTE指示灯
void util_turn_on_lte_led( void );
//关掉LTE指示灯
void util_turn_off_lte_led( void );
#ifndef WIN32
unsigned int util_clac_random_response(unsigned int random_value);
#endif
int util_server_read_wifi_info( InfoStruct* server_wifi_info );
int util_server_read_wifi_info_ex( const char* config_file,InfoStruct* server_wifi_info );
//读取版本相关的信息
int util_read_version_info( const char* config_file,InfoStruct* server_wifi_info );

//获取文件大小,文件当前的读取位置不会改变
int util_get_file_size_by_file_handle( FILE* file_handle );
//获取文件大小,如果文件不存在则返回-1
int util_get_file_size_by_file_name( const char* file_name );
//读取MAC控制信息
void util_read_mac_control_info( MacControlInfo* mac_control_info,int is_used_by_web_page );
//保存mac控制信息
void util_save_mac_control_info( MacControlInfo* mac_control_info,int is_used_by_web_page );
//获取指定mac对应的信息,成功获取信息返回0，否则返回-1
int util_get_mac_info( MacControlInfo* mac_control_info,unsigned char* mac_addr,MacIPPair* ip_pair_info );
//写入指定mac对应的信息,该函数不会直接进行写入，需要调用函数util_save_mac_control_info进行保存,成功获取信息返回0，否则返回-1
int util_set_mac_info( MacControlInfo* mac_control_info,unsigned char* mac_addr,char* ip_addr,char* ssid,char* primary_ch );
int util_set_mac_info_ex( MacControlInfo* mac_control_info,unsigned char* mac_addr,char* ip_addr,char* ssid,char* primary_ch,char* txpower );
int util_set_mac_info2( MacControlInfo* mac_control_info,MacIPPair* mac_ip_pair );
//删除指定mac地址相关的信息,成功获取信息返回0，否则返回-1
void util_delete_mac_info( MacControlInfo* mac_control_info,unsigned char* mac_addr );

void util_get_mac(char *buf, unsigned char *mac);

//装载所有的MAC地址,成功返回0,失败返回-1
int util_load_saved_macs( MacBuffer* mac_struct );
//保存客户端的mac地址
void util_save_client_macs( MacBuffer* all_client_macs );
//保存服务器的mac地址
void util_save_server_mac( unsigned char* remote_mac_addr );

//对文件内容进行加密
void util_encrypt_file_content( unsigned char* file_content,int file_size );

char* util_get_field_value_seperated_by_equal(char* tmp_buffer);

//更新邻居ARP缓存
void util_update_neighbour_arp_cache( const char* network_dev_name,const char* ip_addr );

//是否需要用户手动进行拨号
int util_should_dial_lte_manually( void );
//是否需要马上断开lte拨号连接
int util_should_disconnect_from_lte_network_immediately( void );

//检查usb设备是否存在,如果usb设备不存在,则进行等待,直到存在相应的usb设备
int check_if_usb_device_exist( const UsbPair* matched_usb_pairs,int usb_pairs_count );
//等待USB设备就绪
void util_wait_until_usb_ready( int wait_for_usb_serial );

//将char转换为数字形式
void util_decimal_number_convert(char* number_str,BigNum* result);
//执行大数字减法
void util_big_number_sub(BigNum* number1,BigNum* number2,BigNum* result);
//执行大数字加法
void util_big_number_add(BigNum* number1,BigNum* number2,BigNum* result);
//执行大数字比较
int util_big_number_compare(BigNum* number1,BigNum* number2);

//MF210模块被使用
int is_module_mf210_used(void);
//SIM5360模块被使用
int is_module_sim5360_used(void);
//L1761模块被使用
int is_module_l1761_used(void);
//ZM8620模块被使用
int is_module_zm8620_used(void);
//P500模块被使用
int is_module_p500_used(void);
//使用的是否为ZTE 7510模块
int is_module_zte7510_used(void);
//使用的是否为ZTE MC2716模块
int is_module_mc2716_used(void);
//使用的是否为龙尚C5300V模块
int is_module_c5300v_used(void);
//使用的是否为龙尚U8300W模块
int is_module_u8300w_used(void);
//是否只有3个LAN端口
int is_there_only_3_lan_ports(void);
//使用的是否为SIM7100C模块
int is_module_sim7100_used(void);
//使用的是否为SLM630B模块
int is_module_slm630b_used(void);
//使用的是否为ML7810模块
int is_module_ml7810_used(void);
//当前设备是否为P11+
int is_this_device_p11plus(void);
//是否应该设置WIFI设置界面
int util_should_display_wifi_settings_page(void);
//串口ttyUSB1是否被使用
int is_ttyusb1_used(void);
//使用获取的IPv6 DNS信息
int is_acquired_ipv6_dns_info_used(void);
//是否关掉了PNAT功能
int is_pnat_disabled(void);
//是否关掉GSM功能
int is_gsm_disabled(void);
//wifi是否被关掉了
int is_wifi_disabled(void);
//是否应该处理针对下位机的路由表
int should_check_downlink_router(void);
//是否为入库版本
int is_ruku_special_version(void);

int util_hex_dump( unsigned char *p_hex,char *p_dump,int len );
int util_hex_recovery( char *p_dump,unsigned char *p_hex,int len );

//将数字写入文件中
void write_digit_to_file( const char* file_name,int value );

void util_write_exception_log(const char*process_name,const char* event_description);


//关掉IPv4/IPv6数据转发功能
void util_disable_data_forward( void );
void util_disable_data_forward_and_output( void );
//开启IPv4/IPv6数据转发功能
void util_enable_data_forward( void );
void util_enable_data_forward_and_output( void );

void save_debug_file_content(const char* file_name,const char* file_content);

#ifdef CONFIG_LOCK_CARD_AND_CELL
//是否应该进行锁卡
int util_should_lock_card( InfoStruct* server_wifi_info );
//是否锁固定PIN码
int util_is_lock_pin_fixed( InfoStruct* server_wifi_info );
//是否锁随机PIN码
int util_is_lock_pin_random( InfoStruct* server_wifi_info );
//锁卡开启情况下,获取修改后的PIN码
char* util_get_modified_pin_code( InfoStruct* server_wifi_info,char* iccid );

//是否开启锁小区功能
int util_should_lock_cellid( InfoStruct* server_wifi_info );
//获取4G锁小区的数量
int util_get_lock_4g_cellid_count( InfoStruct* server_wifi_info );
//获取3G锁小区的数量
int util_get_lock_3g_cellid_count( InfoStruct* server_wifi_info );
//获取2G锁小区的数量
int util_get_lock_2g_cellid_count( InfoStruct* server_wifi_info );

//是否应该进行限速
int util_should_limit_speed( InfoStruct* server_wifi_info );
//获取限速的数值
int util_should_limit_speed_value( InfoStruct* server_wifi_info );
//获取上行限速的数值
int util_should_limit_uplink_speed_value( InfoStruct* server_wifi_info );
//看门狗是否被开启了
int util_is_watchdog_enabled( InfoStruct* server_wifi_info );
//速度优先模式
int util_is_speed_preferred_mode( InfoStruct* server_wifi_info );

#endif

//使用系统系统配置信息覆盖掉客户配置的信息
void util_overwrite_config_info( InfoStruct* sys_config_info,InfoStruct* nvram_config_info );
char* util_get_config_field_value( InfoStruct *nvram_config_info,char* config_field_name );
void util_export_config_info( InfoStruct *nvram_config_info );
//获取拨号方式
int util_get_TZ_L2TP_MODE( InfoStruct* nvram_config_info );
//是否开启了PPP转发来实现用户PPPOE拨号上网
int util_get_TZ_ENABLE_PPP_FORWARD( InfoStruct* nvram_config_info );
//是否需要设置LTE灯的状态
int util_get_LAN_LED_AS_LTE_STATUS( InfoStruct* nvram_config_info );
//将IP地址保存到文件中,在另外一个进程中将文件中保存下来
void util_read_ip_address( const char* ip_file,char* buffer,int buffer_size );
int util_sync_module_info( char* network_card_name );

//0表示获取ARP信息成功,否则表示获取ARP信息失败
int util_get_arp_table_info( ARPTable* arp_table,char* network_card );
//0表示获取通过PPPOE连接的客户端信息成功,否则表示获取通过PPPOE连接的客户端信息失败
int util_get_pppoe_table_info( ARPTable* arp_table,char* network_card );
//去掉字符串末尾的空白
void util_strip_blank_of_string_end( char* str );

//去除结尾的空白字符
void util_strip_traling_spaces( char* one_string );
//装载模块相关的信息
int util_load_module_info( InfoStruct* module_info );

//跳过所有的空白字符
char* util_skip_all_spaces( char* one_string );
//读取发送给模块的AT命令查询到的状态
int util_read_module_status( char* result_file,InfoStruct* module_info );

//base64字符串编码
int base64_encode( unsigned char *in, unsigned char *out, int src_len );
//base64字符串解码
int base64_decode( unsigned char *in, unsigned char *out,int src_len );
//检查是否所有的字符均为base64编码生成的字符
int util_is_base64_string( char *in,int src_len );

//上报当前系统运行的时间
void util_read_system_uptime( char* uptime_value );
//获取MAC地址,成功返回0,否则返回-1
int util_load_mac_addr( char*net_dev_name,char* mac_addr );
//获取WAN端口的名称
char* util_get_wan_interface_name( void );
//创建sokcet连接,成功返回0,失败返回其他值
int util_create_socket( char* host,int port,int* socket_handle );

//读取当前的内存使用信息
int util_read_meminfo( char* mem_total,char* mem_free );
//读取当前所使用的lns服务器信息
int util_read_lns_info( char* lns,char* name,char* pass );
//读取文件/proc/tozed/ind的内容,获取当前系统的状态信息,成功返回0,失败返回-1
int util_read_status_ind(
							int* is_sim_exist
							,int* need_pin
							,int* need_puk
							,int* signal_quality
							,int* sms_status
							,int* reg_status
							,int* greg_status
							,int* ereg_status
							,int* act
							,int* network_data_service
						);
//检查一个UDP端口是否被打开,用于检测L2TP端口是否被打开,成功返回1,否则返回0
int util_check_if_one_udp_port_is_open( char* ip_addr,int port,char* hostname );

char* util_get_all_pppoe_users( void );
//检查FDD预置频点配置是否为空
int util_is_fdd_prestore_frequency_empty( InfoStruct* info_struct );
//检查TDD预置频点配置是否为空
int util_is_tdd_prestore_frequency_empty( InfoStruct* info_struct );
char* util_get_next_fdd_prestore_frequency_not_in_list( char* at_response_result,InfoStruct* info_struct );
char* util_get_next_tdd_prestore_frequency_not_in_list( char* at_response_result,InfoStruct* info_struct  );
int util_is_all_fdd_prestore_frequency_in_list( char* at_response_result,InfoStruct* info_struct  );
int util_is_all_tdd_prestore_frequency_in_list( char* at_response_result,InfoStruct* info_struct  );

//获取锁IMSI的策略
int util_get_TZ_LOCK_IMSI( InfoStruct* nvram_config_info );

//为网页提供用户发送AT命令的端口
char* util_get_at_port( void );
//P500模块网络连接是否完好
int util_p500_link_connected(void);
//获取指定mac地址和端口之间的对应关系,使用已经通过util_get_pppoe_table_info
//获取的信息来进一步获取指定MAC对应的端口信息
int util_get_port_number_for_one_mac( ARPTable* system_arp_table );
//获取所有连接的wifi客户端设备
int util_get_all_wlan_clients( ARPTable* arp_table );

//是否支持加密卡,支持返回1,不支持返回0
int util_encrypt_card_supported( InfoStruct* info_struct );
//加密卡是否使用了16组密钥
int util_encrypt_card_has_16_keys( InfoStruct* info_struct );

//是否应该锁定一个小区
int util_should_lock_one_cell( InfoStruct* info_struct );
//获取使用的模块的名称
char* util_get_used_module_name( void );

//获取移动数据连接默认使用的cid的值
int util_get_default_cid_value( InfoStruct* info_struct );

//使用CPE标贴纸上的端口号,获取指定端口对应的实际端口
int util_get_cpe_phy_port_from_label_number( int label_number );

//判定模块 LOG 是否正在进行抓取的,调用如下函数判定是否正在抓取 LOG:
int util_is_log_tools_running(void);


#endif

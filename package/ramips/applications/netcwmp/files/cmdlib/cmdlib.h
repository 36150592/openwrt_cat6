#ifndef _CMD_LIB_H_
#define _CMD_LIB_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stddef.h"
#include <ctype.h>
#include "module_features_definition.h"

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

/*
使用NDISDUP进行拨号还是使用QCRMCALL进行拨号,使用QCRMCALL进行拨号速度快一些

AT+SER=1,1输入之后就可以使用QCRMCALL命令了
AT$QCRMCALL=1,1,2,1,1
这个用于进行IPv6拨号


默认需要使用AT^NDISDUP进行数据拨号,最后一个0表示不进行鉴权验证:
AT^NDISDUP=1,1,\"%s\",\"%s\",\"%s\",0


AT^NDISDUP=1,1,\"%s\",\"%s\",\"%s\",3
用于进行CHAP/PAP验证,一般设置为3;


#挂掉数据连接
AT^NDISDUP=1,0

RmNet(通过AT命令QCRMCALL进行拨号)，CDC-ECM(通过AT命令NDISDUP进行拨号) 区别
在高通平台上，rmnet driver 和标准的CDC-ECM是有区别的，rmnet 也是属于CDC-ECM
他们具体的区别在于对于USB命令的封装以及使用的USB接口，端点定义方式不同
如果是使用rmnet,那么发起data call是通过QMI工具发的QMI命令，QMI工具为QMICM，QMICM集成了QMI命令
而通过标准的CDC-ECM发起data call，则是发送标准的ECM命令。
如果是QMICM建立的data call，不走router的，所以它的IP地址获得的是公网IP。
而通过标准的CDC-ECM建立的data call，是走router的，获得的IP地址是私有的IP如192.168开头。


//SLM630B模块是否使用CDC_ETHER驱动
*/
//#define USE_CDC_ETHER_TO_DIAL_NETWORK

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

#define ARRAY_ELEMENTS_COUNT( array_name ) ( sizeof(array_name)/sizeof(array_name[0]) )

#define MAX_BYTES_COUNT_FOR_BIT 8

typedef struct
{
	unsigned int all_bits_info[ MAX_BYTES_COUNT_FOR_BIT ];
}BigValue;

#define CURRENT_NETWORK_INFO_SEPARATOR "$"
#define CURRENT_NETWORK_INFO_FIELD_SEPARATOR "@"
#define CURRENT_NETWORK_INFO_System_Mode "System Mode"
#define CURRENT_NETWORK_INFO_Operation_Mode "Operation Mode"
#define CURRENT_NETWORK_INFO_MCC_MNC "MCC MNC"
#define CURRENT_NETWORK_INFO_TAC_LAC "TAC/LAC"
#define CURRENT_NETWORK_INFO_Serving_CellID "Serving CellID"
#define CURRENT_NETWORK_INFO_Physical_CellID "Physical CellID"
#define CURRENT_NETWORK_INFO_Frequency_Band "Frequency Band"
#define CURRENT_NETWORK_INFO_EARFCN "EARFCN/ARFCN"
#define CURRENT_NETWORK_INFO_Downlink_Bandwidth "Downlink Bandwidth"
#define CURRENT_NETWORK_INFO_Uplink_Bandwidth "Uplink Bandwidth"
#define CURRENT_NETWORK_INFO_RSRQ "RSRQ"
#define CURRENT_NETWORK_INFO_RSRP "RSRP"
#define CURRENT_NETWORK_INFO_RSSI "RSSI"
#define CURRENT_NETWORK_INFO_SINR "SINR"
#define CURRENT_NETWORK_INFO_TZTRANSMODE "TZTRANSMODE"
#define CURRENT_NETWORK_INFO_TZTA "TZTA"
#define CURRENT_NETWORK_INFO_TZTXPOWER "TZTXPOWER"
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

//检查一个字符串是否为空
#define IS_STRING_NULL( string ) ( ( string ) == NULL )
//获取字符串的长度
#define GET_STRING_LENGTH( string ) strlen( string )
//字符串A是否为字符串B的前缀
#define IS_STRING_A_PREFIX_OF_B( A,B ) ( strlen( A ) && !strncmp( A,B,strlen( A ) ) )
//字符串B是否为字符串A的前缀
#define IS_STRING_B_PREFIX_OF_A( A,B ) ( strlen( B ) && !strncmp( A,B,strlen( B ) ) )
//字符串是否相同
#define IS_STRING_EQUAL( A,B ) ( !strcmp( (A),(B) ) )
//检查两个字符串是否不同
#define IS_STRING_NOT_EQUAL( A,B ) ( !!strcmp( (A),(B) ) )
//字符串长度大于0
#define IS_STRING_LENGTH_GT_0( string ) ( ( ( string ) != NULL ) && GET_STRING_LENGTH( string ) )

//检查一个指定的位是否被设置了
#define IS_ONE_BIT_SET( value,bit_offset ) ( ( value>>bit_offset )&0x01 )
//获取指定的2个位的值
#define GET_2_BITS_VALUE( value,bit_offset ) ( ( value>>bit_offset )&0x03 )
//获取3个位的值
#define GET_3_BITS_VALUE( value,bit_offset ) ( ( value>>bit_offset )&0x07 )
//获取4个位的值
#define GET_4_BITS_VALUE( value,bit_offset ) ( ( value>>bit_offset )&0x0f )
//获取5个位的值
#define GET_5_BITS_VALUE( value,bit_offset ) ( ( value>>bit_offset )&0x1f )


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


//当前注册所在的网络
#define NETWORK_MODE_2G 0
#define NETWORK_MODE_3G 2
#define NETWORK_MODE_4G 7


#define SET_ERR_CODE( error_code,error ){\
												error_code=( error_code )|( 1<<( error ) );\
											}

#define CLEAR_ERR_CODE( error_code,error ){\
												error_code=( error_code )&( ~( 1<<( error ) ) );\
											}

#define IS_ERR_CODE_SET( error_code,error ) ( ( error_code )&( 1<<( error ) ) )


//不支持的网络模式
//不支持GSM
#define TZ_GSM_NOT_SUPPORTED_BIT ( 1<<0 )
//不支持WCDMA
#define TZ_WCDMA_NOT_SUPPORTED_BIT ( 1<<1 )
//不支持TDS-CDMA
#define TZ_TDSCDMA_NOT_SUPPORTED_BIT ( 1<<2 )
//不支持LTE
#define TZ_LTE_NOT_SUPPORTED_BIT ( 1<<3 )
//优先选择WCDMA网络
#define TZ_WCDMA_PREFERRED_BIT ( 1<<4 )

#define TZ_GSM_NOT_SUPPORTED_BIT_FLAG_FILE "/tmp/.gsm_not_supported_bit_flag_file"
#define TZ_WCDMA_NOT_SUPPORTED_BIT_FLAG_FILE "/tmp/.wcdma_not_supported_bit_flag_file"
#define TZ_TDSCDMA_NOT_SUPPORTED_BIT_FLAG_FILE "/tmp/.tdscdma_not_supported_bit_flag_file"
#define TZ_LTE_NOT_SUPPORTED_BIT_FLAG_FILE "/tmp/.lte_not_supported_bit_flag_file"
#define TZ_WCDMA_PREFERRED_BIT_FLAG_FILE "/tmp/.wcdma_preferred_bit_flag_file"
#define NOT_CHECK_IF_LNS_ALIVE_FILE "/tmp/.not_check_if_lns_alive_file"
#define USE_SPECIFIED_LNS_SERVER_INDEX_FILE "/tmp/.use_specified_lns_server_index_file"

//不支持GSM网络模式
#define is_gsm_mode_not_supported( network_not_supported ) ( network_not_supported&TZ_GSM_NOT_SUPPORTED_BIT )
#define is_wcdma_mode_not_supported( network_not_supported ) ( network_not_supported&TZ_WCDMA_NOT_SUPPORTED_BIT )
#define is_tdscdma_mode_not_supported( network_not_supported ) ( network_not_supported&TZ_TDSCDMA_NOT_SUPPORTED_BIT )
#define is_lte_mode_not_supported( network_not_supported ) ( network_not_supported&TZ_LTE_NOT_SUPPORTED_BIT )
#define is_wcdma_preferred( network_not_supported ) ( network_not_supported&TZ_WCDMA_PREFERRED_BIT )

#define APN_AUTH_TYPE_NOT_SUPPORT_NONE (1<<0)
#define APN_AUTH_TYPE_NOT_SUPPORT_PAP (1<<1)
#define APN_AUTH_TYPE_NOT_SUPPORT_CHAP (1<<2)
#define APN_AUTH_TYPE_NOT_SUPPORT_PAP_CHAP (1<<3)

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

/*
SIM: GSM
UIM: CDMA one/1X
USIM: UMTS and GSM
RUIM: support GSM, CDMA one/1X, CDMA 2000
UICC: all supported, including SIM, USIM, CSIM(a port of R-UIM functionality to the UICC).
LTE既可以用USIM卡也可以用RUIM卡。

SIM卡
SIM卡(Subscriber Identity Module) ，即用户识别卡，是全球通数字移动电话的一张个人资料卡。它采用A 级加密方法制作，存储着用户的数据、鉴权方法及密钥，可供GSM系统对用户身份进行鉴别。同时，用户通过它完成与系统的连接和信息的交换。
移动电话只有装上SIM卡才能使用。“SIM卡”有大小之分，功能完全相同，分别适用于不同类型的GSM移动电话。SIM卡可以插入任何一部符合GSM规范的移动电话中，而通话费则自动计入持卡用户的帐单上，与移动电话无关。
SIM卡的使用，有效的防止了盗用、并机和通话被窃听，使用户的正常通信得到了可靠的保障。
为了保证您的移动电话丢失后不被盗用，每张SIM卡都可设置一组个人密码（PIN码）来对SIM卡上锁，它是由用户自己设定的。只有正确输入密码后，手机才会进入正常的使用状态。连续三次输入错误的个人密码，手机即会将SIM卡锁住。发生这种情况，请您立即关机并携机及SIM卡到无线局营业厅解锁。如果此时您还继续操作，将引起SIM卡的自动封毁，给您造成不必要的损失。

USIM卡
USIM卡就是第三代手机卡
很多人认为在3G时代，绝大部分应用只能由手机实现，卡片上的有限资源只需实现认证功能就可以了。的确，3G的应用十分复杂，大部分的应用都不能通过STK卡来单独完成。但USIM卡并不是只能做单纯的认证功能，事实证明它正在逐步向移动商务平台、乃至最后的多应用平台过渡，在手机上实现电子钱包、电子信用卡、电子票据等其它应用已不再是难事。这一特点使USIM卡成为了不同行业跨领域合作、相互渗透经营的媒介，如银行可以参与电信的经营，反之亦然。
除能够支持多应用之外，USIM卡还在安全性方面对算法进行了升级，并增加了卡对网络的认证功能，这种双向认证可以有效防止黑客对卡片的攻击。同时，USIM卡的电话簿功能更为强大，最多可存入500个电话号码，并且针对每个电话，用户还可以选择是否录入其它信息，如电子邮件、别名、其它号码等。
尽管步履蹒跚，但3G还是向我们一步步走来。高额的3G牌照费用也许是许多运营商徘徊不前的原因之一，更重要的是它们对3G应用持以观望的态度。而且实现基于USIM卡上的多应用还有很多问题亟待解决，如相关的规范不够完善，缺乏支持这种多应用的手机，更重要的是运营商和相关的企业或政府机构的多方协调会加大这种应用的难度。无论怎样，第三代移动通信卡片在这方面已经做好了技术准备，相信基于USIM卡的多应用也终会在3G时代得到广泛使用。

RUIM
	CDMA系统定义了Removable User Identity Module(RUIM)，类似GSM系统中SIM卡的观念。
	可移动用户识别模块RUIM)，也叫做用户识别模块（UIM），和用户识别卡(SIM)是相似的，但是它为网络设计而不是为GSM，例如CDMA。R-UIM卡支持在CDMA和GSM网络之间的漫游。随着GSM技术在移动通信市场上的巨大成功，用户已经对通过将SIM卡插入不同的手机而任意更换手机习以为常了。然而在北美的移动通信标准体系中，无论是模拟还是CDMA移动台却是机卡一体的。中国联通公司在1999年首先提出了在CDMA系统中引入智能IC卡的概念，即所谓的机卡分离。通过机卡分离技术将与用户相关的信息和鉴权算法等与安全相关的信息保留在智能卡上，这个卡叫做R-UIM（Removable User Identity Module）。机卡分离技术的实现不但使得用户能够更加灵活方便地更换CDMA手机，而且能够使用户在不同制式的网络中自由漫游的愿望得以实现（比如使用CDMA/GSM双模卡的用户可以通过更换不同制式的手机或使用双模手机在CDMA和GSM网络中漫游）。在这个概念的基础上，CDG、TIA、3GPP2等国际标准化组织以及我国的CWTS积极地开展研究工作并在1999年中期开始着手对用户标识模块（UIM）以及UIM-ME接口规范的制订工作。

UIM
	UIM(User Identity Module)用户识别模块。是应用在cdmaOne手机的一种智能卡，可插入对应的2G手机以使用 移动电话服务。UIM卡的标准化工作由3GPP2（第三代伙伴计划2）负责进行。
	是由中国联通公司倡导并得到国际CDMA组织(CDG)支持的移动通信终端用户识别及加密技术。它支持专用的鉴权加密算法和OTA技术(Over The Air),可以通过无线空中接口方式对卡上的数据进行更新和管理。UIM卡的功能类似于全球通(GSM)手机中使用的SIM卡，可进行用户的身份识别及通信加密，还可以存储电话号码、短信息等用户个人信息。同时UIM卡采用了SIM卡一卡一号的便利使用方式，用户只需拥有一张属于个人的UIM卡，插入任何一部配有UIM卡接口的手机即可应用。
*/
typedef enum
{
	SIM_CARD_TYPE_APP_NONE=0,
	SIM_CARD_TYPE_APP_SIM=1,
	SIM_CARD_TYPE_APP_RUIM=2,
	SIM_CARD_TYPE_APP_USIM=3,
	SIM_CARD_TYPE_APP_CSIM=4,
	SIM_CARD_TYPE_APP_UNKNOWN=5,
}SimCardType;

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

typedef enum
{
	PPP_AUTH_TYPE_NONE=0,
	PPP_AUTH_TYPE_PAP=1,
	PPP_AUTH_TYPE_CHAP=2,
	PPP_AUTH_TYPE_BOTH_PAP_CHAP=3,
}PPPAuthProtocol;

//定义一个标志
#define DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE(field) field##_DEFINED


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
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_PPP_AUTH_TYPE ),

	//是否自动获取DNS服务器信息
	TZ_GET_DNS_AUTO_DEFINED,
	//首选DNS
	TZ_DNS_PRIMARY_DEFINED,
	//备用DNS
	TZ_DNS_SECONDARY_DEFINED,

	//选择使用的花生壳DDNS服务
	TZ_ENABLE_HSK_DDNS_SERVER_DEFINED,
	//开启花生壳DDNS功能需要用到的帐号（在花生壳网站注册的帐号）
	TZ_DDNS_HSK_USERNAME_DEFINED,
	//开启花生壳DDNS功能需要用到的密码
	TZ_DDNS_HSK_PASSWORD_DEFINED,

	//开启DDNS功能选择的网口
	TZ_DDNS_INTERFACE_DEFINED,

	//选择使用的3322的DDNS服务
	TZ_ENABLE_3322_DDNS_SERVER_DEFINED,
	//开启3322的DDNS功能需要用到的帐号（在3322网站注册的帐号）
	TZ_DDNS_3322_USERNAME_DEFINED,
	//开启3322DDNS功能需要用到的密码
	TZ_DDNS_3322_PASSWORD_DEFINED,
	//开启3322的DDNS功能是所使用的域名
	TZ_3322_DOMAIN_DEFINED,

	//选择使用的no-ip的DDNS服务
	TZ_ENABLE_NOIP_DDNS_SERVER_DEFINED,
	//开启no-ip的DDNS功能需要用到的帐号（在no-ip网站注册的帐号）
	TZ_DDNS_NOIP_USERNAME_DEFINED,
	//开启no-ipDDNS功能需要用到的密码
	TZ_DDNS_NOIP_PASSWORD_DEFINED,
	//开启no-ip的DDNS功能是所使用的域名
	TZ_NOIP_DOMAIN_DEFINED,

	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_FORBID_ROAMING ),

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

	TZ_FLOW_THRESHOLD_DEFINED,
	TZ_FLOW_WARNING_DEFINED,

	TZ_ENABLE_SET_REBOOT_DEFINED,
	TZ_SET_REBOOT_HOURS_DEFINED,

	TZ_ISOLATE_WLAN_CLIENTS_DEFINED,

	TZ_L2TP_MTU_DEFINED,
	TZ_CPE_ROUTER_MODE_DEFINED,

	TZ_SAVE_LOCK_CELL_AFTER_TIMEOUT_DEFINED,
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_NETWORK_MODE_NOT_SUPPORT ),
	TZ_URGENT_HEARTBEAT_INTERVAL_DEFINED,

	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_4G_FDD_PREFERRED ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_CHECK_IF_LNS_ALIVE ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_NTP_SERVER2 ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_NTP_SERVER3 ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_NTP_SERVER4 ),
	
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_ENABLE_LOCK_NETWORK ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_LOCK_NETWORK_PLMN ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_LOCK_NETWORK_PLMN2 ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_LOCK_NETWORK_PLMN3 ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_LOCK_NETWORK_PLMN4 ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_LOCK_NETWORK_PLMN5 ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_LOCK_NETWORK_PLMN6 ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_LOCK_NETWORK_PLMN7 ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_LOCK_NETWORK_PLMN8 ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_LOCK_NETWORK_PLMN9 ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_LOCK_NETWORK_PLMN10 ),

	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_ADMIN_RANDOM_PASSWORD ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SPECIAL_SSID_PREFIX ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_CS_PS_PREFERRED_MODE ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_IP_PASS_THROUGH_ENABLE ),
	

	TZ_PPTP_ENABLED_DEFINED,
	TZ_PPTP_MODE_DEFINED,
	TZ_PNS_SERVER_DEFINED,
	TZ_PPTP_AUTH_NAME_DEFINED,
	TZ_PPTP_AUTH_PASS_DEFINED,
	TZ_PPTP_ENABLE_MPPE_DEFINED,
	TZ_PPTP_MTU_DEFINED,
	TZ_VPN_AS_DEFAULT_ROUTE_DEFINED,

	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_IP ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_PORT ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_DOMAIN ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_DOMAIN_PORT ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_DOMAIN_RANGE ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_PROXY_ENABLED ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_PROXY_IP ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_PROXY_PORT ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_DISPLAY_NAME ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_USERNAME ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_AUTH_NAME ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_SIP_AUTH_PASSWD ),

	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_ANTENNA_MODE ),

	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_WPS_IS_ENABLE ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_WPS_MODE ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_CLIENT_PIN ),

	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_ENABLE_12_13_AUTO_CHANNEL ),

	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_ENABLE_DDOS ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_ENABLE_DEFENCE_SYN_FLOOD ),
	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_ENABLE_DEFENCE_PING_OF_DEATH ),


	DEFINE_ONE_INFOSTRUCTFIELDDEFINED_VALUE( TZ_TELNET_SERVER_ENABLE ),

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

	//选择开启花生壳服务DDNS功能
	char TZ_ENABLE_HSK_DDNS_SERVER[ 16 ];
	//开启花生壳DDNS功能需要用到的帐号（在花生壳的网站注册的帐号）
	char TZ_DDNS_HSK_USERNAME[ 128 ];
	//开启花生壳DDNS功能需要用到的密码
	char TZ_DDNS_HSK_PASSWORD[ 128 ];

	//开启DDNS功能选择的网口
	char TZ_DDNS_INTERFACE[ 32 ];

	//选择开启3322的DDNS功能
	char TZ_ENABLE_3322_DDNS_SERVER[ 16 ];
	//开启3322的DDNS功能需要用到的帐号（在3322的网站注册的帐号）
	char TZ_DDNS_3322_USERNAME[ 128 ];
	//开启3322的DDNS功能需要用到的密码
	char TZ_DDNS_3322_PASSWORD[ 128 ];
	//开启3322网站的DDNS功能选择的域名
	char TZ_3322_DOMAIN[ 128 ];

	//选择开启no-ip的DDNS功能
	char TZ_ENABLE_NOIP_DDNS_SERVER[ 16 ];
	//开启no-ip的DDNS功能需要用到的帐号（在no-ip的网站注册的帐号）
	char TZ_DDNS_NOIP_USERNAME[ 128 ];
	//开启no-ip的DDNS功能需要用到的密码
	char TZ_DDNS_NOIP_PASSWORD[ 128 ];
	//开启no-ip网站的DDNS功能选择的域名
	char TZ_NOIP_DOMAIN[ 128 ];

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
	char TZ_PPP_AUTH_TYPE[ 64 ];

	//是否自动获取DNS服务器信息
	char TZ_GET_DNS_AUTO[ 4 ];
	//首选DNS
	char TZ_DNS_PRIMARY[ 64 ];
	//备用DNS
	char TZ_DNS_SECONDARY[ 64 ];

	char TZ_FORBID_ROAMING[ 64 ];

	//要锁定的频段
	char TZ_BAND_PREF[ 64 ];

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

	//流量提醒的最大值MB字节数
	char TZ_SET_REBOOT_HOURS[32];
	//是否开启流量提醒,yes表示开启,其他表示不开启
	char TZ_ENABLE_SET_REBOOT[32];

	//TZ_ISOLATE_WLAN_CLIENTS为yes表示隔离无线客户端,否则不隔离无线客户端;
	char TZ_ISOLATE_WLAN_CLIENTS[32];

	//支持对 MTU 值进行配置,默认 MTU 值为 1400 字节。
	char TZ_L2TP_MTU[32];	

	//是否开启路由模式
	char TZ_CPE_ROUTER_MODE[32];

	//在指定的时间之后开始保存锁小区信息
	char TZ_SAVE_LOCK_CELL_AFTER_TIMEOUT[32];
	//不支持的网络模式
	char TZ_NETWORK_MODE_NOT_SUPPORT[32];
	//紧急事件心跳时间间隔
	char TZ_URGENT_HEARTBEAT_INTERVAL[32];

	//是否优先搜索FDD频段
	char TZ_4G_FDD_PREFERRED[32];

	//检查LNS服务器是否存在
	char TZ_CHECK_IF_LNS_ALIVE[32];
	char TZ_NTP_SERVER2[ 128 ];
	char TZ_NTP_SERVER3[ 128 ];
	char TZ_NTP_SERVER4[ 128 ];

	//锁网相关的字段
	char TZ_ENABLE_LOCK_NETWORK[32];
	char TZ_LOCK_NETWORK_PLMN[32];
	char TZ_LOCK_NETWORK_PLMN2[32];
	char TZ_LOCK_NETWORK_PLMN3[32];
	char TZ_LOCK_NETWORK_PLMN4[32];
	char TZ_LOCK_NETWORK_PLMN5[32];
	char TZ_LOCK_NETWORK_PLMN6[32];
	char TZ_LOCK_NETWORK_PLMN7[32];
	char TZ_LOCK_NETWORK_PLMN8[32];
	char TZ_LOCK_NETWORK_PLMN9[32];
	char TZ_LOCK_NETWORK_PLMN10[32];

	char TZ_ADMIN_RANDOM_PASSWORD[32];

	char TZ_SPECIAL_SSID_PREFIX[32];
	

	char TZ_CS_PS_PREFERRED_MODE[32];

	char TZ_IP_PASS_THROUGH_ENABLE[32];

	//PPTP VPN客户端需要配置的参数
	//PPTP客户端或服务器是否被开启
	char TZ_PPTP_ENABLED[ 64 ];
	//采用客户端还是服务器模式
	char TZ_PPTP_MODE[ 64 ];
	//PNS 服务器地址,可能是IP地址或DNS域名
	char TZ_PNS_SERVER[ 64 ];
	//PPTP VPN用户名
	char TZ_PPTP_AUTH_NAME[ 128 ];
	//PPTP VPN密码
	char TZ_PPTP_AUTH_PASS[ 128 ];
	//是否开启MPPE加密
	char TZ_PPTP_ENABLE_MPPE[ 16 ];
	//设置的MTU值
	char TZ_PPTP_MTU[32];
	//将通过VPN获取的信息作为默认路由
	char TZ_VPN_AS_DEFAULT_ROUTE[32];


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
	char TZCQI[32];
	char TZMCS[32];
	char TZTAC[32];
	char TZLOCKLTEBAND[32];
	char TZECGI[32];

	//锁IMSI段号的字段
	char TZ_ENABLE_LOCK_IMSI_PREFIX[32];
	char TZ_IMSI_PREFIX01[32];
	char TZ_IMSI_PREFIX02[32];
	char TZ_IMSI_PREFIX03[32];
	char TZ_IMSI_PREFIX04[32];
	char TZ_IMSI_PREFIX05[32];
	char TZ_IMSI_PREFIX06[32];
	char TZ_IMSI_PREFIX07[32];
	char TZ_IMSI_PREFIX08[32];
	char TZ_IMSI_PREFIX09[32];
	char TZ_IMSI_PREFIX10[32];
	char TZ_IMSI_PREFIX11[32];
	char TZ_IMSI_PREFIX12[32];
	char TZ_IMSI_PREFIX13[32];
	char TZ_IMSI_PREFIX14[32];
	char TZ_IMSI_PREFIX15[32];
	char TZ_IMSI_PREFIX16[32];
	char TZ_IMSI_PREFIX17[32];
	char TZ_IMSI_PREFIX18[32];
	char TZ_IMSI_PREFIX19[32];
	char TZ_IMSI_PREFIX20[32];

	char TZ_SIP_IP[32];
	char TZ_SIP_PORT[32];
	char TZ_SIP_DOMAIN[32];
	char TZ_SIP_DOMAIN_PORT[32];
	char TZ_SIP_DOMAIN_RANGE[32];
	char TZ_SIP_PROXY_ENABLED[32];
	char TZ_SIP_PROXY_IP[32];
	char TZ_SIP_PROXY_PORT[32];
	char TZ_SIP_DISPLAY_NAME[32];
	char TZ_SIP_USERNAME[32];
	char TZ_SIP_AUTH_NAME[32];
	char TZ_SIP_AUTH_PASSWD[32];

	char TZ_ANTENNA_MODE[16];

	char TZ_WPS_IS_ENABLE[16];
	char TZ_WPS_MODE[16];
	char TZ_CLIENT_PIN[16];

	char TZ_ENABLE_12_13_AUTO_CHANNEL[16];

	char TZ_ENABLE_DDOS[16];
	char TZ_ENABLE_DEFENCE_SYN_FLOOD[16];
	char TZ_ENABLE_DEFENCE_PING_OF_DEATH[16];

	char TZ_TELNET_SERVER_ENABLE[16];

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
}LNSServerItem;

//所有的lns服务器信息
typedef struct
{
	LNSServerItem lns[ 10 ];
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


typedef struct
{
	//字段的名称
	char* field_name;
	//字段在结构体中的偏移
	size_t field_offset;
	int field_flag;
	//字段的最大长度
	size_t field_size;
}FieldConfigInfo;

//定义一个FieldConfigInfo类型的结构体
#define DEFINE_ONE_FIELDCONFIGINFO_STRUCT( field ) {\
													#field\
													,offsetof( InfoStruct,field )\
													,field##_DEFINED\
													,sizeof( ( (InfoStruct*)0 )->field )\
													}

typedef struct
{
	char port_name[32];
}UsbSerialEntry;

typedef struct
{
	UsbSerialEntry table[20];
	int count;
}UsbSerialTable;

//#define CMDLIB_TEST 1

#define IPv4_EXIST_FLAG 0x01
#define IPv6_EXIST_FLAG 0x02


//DDNS的状态信息
//“0” 表示 成功连接上服务器
#define DDNS_RESULT_CODE_SUCCESSS "0"
//“1” 表示 输入的域名无效
#define DDNS_RESULT_CODE_INVALID_DOMAIN_NAME "1"
//“2” 表示 帐号或密码错误
#define DDNS_RESULT_CODE_USERNAME_OR_PASSWORD_ERROR "2"
//“4” 表示 未知错误(网络可能未连接)
#define DDNS_RESULT_CODE_UNKNOWN_ERROR "4"
//“5” 表示 域名未在你的帐号控制下
#define DDNS_RESULT_CODE_NOT_UNDER_YOUR_CONTROL_ERROR "5"
//“6” 服务器阻塞,请重新登陆
#define DDNS_RESULT_CODE_SERVER_BLOCKED_ERROR "6"
//“0”“7”“8”,都表示 成功连接上服务器
#define DDNS_RESULT_CODE_SUCCESSS2 "7"
#define DDNS_RESULT_CODE_SUCCESSS3 "8"


//SLM630B驱动rmnet模式
#define SLM630B_DRIVER_MODE_RMNET 1
//SLM630B驱动cdc ether模式
#define SLM630B_DRIVER_MODE_CDC_ETHER 2

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
#define NEED_TO_RESTORE_SETTINGS_NO_REBOOT_TEMP_FILE "/tmp/.need_to_restore_settings_no_reboot"
#define CONFIG_CHANGED_BY_SERVER_TEMP_FILE "/tmp/.config_changed_by_server"
#define REBOOT_BECAUSE_OF_CONFIG_CHANGED_TEMP_FILE "/tmp/.reboot_because_of_config_changed"
//用于进行web页面和配置服务器之间的同步
#define MAC_CONTROL_INFO_FILE_CHANGED_TMP_FILE "/tmp/.mac_control_info_file_changed"
#define UPLINK_CONNECTION_IS_OK_TMP_FILE "/tmp/.uplink_connection_is_ok_tmp_file"
//手动进行lte拨号
#define LTE_DIAL_MANUALLY "/tmp/.dial_lte_module_manually"
//从lte网络断开
#define LTE_DISCONNECT_FROM_NETWORK_IMMEDIATELY "/tmp/.disconnect_from_lte_network_immediately"
//SIM CARD不存在
#define SIM_CARD_IS_EXIST "/tmp/.sim_card_is_exist"
//网络正常
#define INTERNET_IS_OK "/tmp/.internet_is_ok"
//信号不正常
#define SIGNAL_IS_OK "/tmp/.signal_is_ok"
//MF210模块被使用
#define MF210_IS_USED "/tmp/.module_mf210_is_used"
//L1761模块被使用
#define L1761_IS_USED "/tmp/.module_l1761_is_used"
//LT10模块被使用
#define LT10_IS_USED "/tmp/.module_lt10_is_used"
//ZM8620模块被使用
#define ZM8620_IS_USED "/tmp/.module_zm8620_is_used"
//使用的是SIM5360模块
#define SIM5360_IS_USED "/tmp/.module_sim5360_is_used"
//使用的是P500模块
#define P500_IS_USED "/tmp/.module_p500_is_used"
//使用的是EC20模块
#define EC20_IS_USED "/tmp/.module_ec20_is_used"
#define BM806_IS_USED "/tmp/.module_bm806_is_used"
//开启了L3Bridge模式
#define L3BRIDGE_IS_ENABLE "/tmp/.l3bridge_is_enable"
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
//当前使用的卡为CDMA卡
#define CDMA_UIM_RUIM_CARD_USED_FILE "/tmp/.cdma_uim_ruim_card_used"
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
#define L2TPD_CLIENT_OPTIONS_FILE "/etc/ppp/options.l2tpd.client"
#define TMP_L2TPD_CLIENT_OPTIONS_FILE "/tmp/options.l2tpd.client"
//L2TP拨号状态文件
#define L2TP_DIAL_STATE_FLAG_FILE "/tmp/.l2tp_dial_state_flag_file"
//版本相关的信息
#define VERSION_INFO_FILE "/etc/version"
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
#define CURRENT_DEVICE_IS_P13 "/tmp/.current_device_is_p13"
#define CURRENT_DEVICE_IS_P51 "/tmp/.current_device_is_p51"
#define CURRENT_DEVICE_IS_P21_VOIP "/tmp/.current_device_is_p21_voip"
#define DISPLAY_WIFI_SETTINGS_PAGE "/tmp/.display_wifi_settings_page"
#define LAN_LED_AS_WLAN_STATUS "/tmp/.lan_led_as_wlan_status"
#define REBOOT_WIFI_MODULE "/tmp/.reboot_wifi_module"
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
//重新拨号读取配置的标志文件
#define REDIAL_CONFIG_FILE_CHANGED "/tmp/.redial_config_file_changed"
//使用的是ZTE MC2716模块
#define MC2716_IS_USED "/tmp/.module_mc2716_is_used"
#define PPP_PEER_MC2716 "mc2716-common"
#define PPP_PEER_MC2716_NEW "mc2716-common-new"
#define PPP_PEER_MC2716_CONFIG_FILE PPP_PEER_DIRECTORY"/"PPP_PEER_MC2716
#define TMP_PPP_PEER_MC2716_CONFIG_FILE "/tmp/"PPP_PEER_MC2716_NEW
#define C5300V_IS_USED "/tmp/.module_c5300v_is_used"
#define PPP_PEER_C5300V "c5300v-common"
#define PPP_PEER_C5300V_NEW "c5300v-common-new"
#define PPP_PEER_C5300V_CONFIG_FILE PPP_PEER_DIRECTORY"/"PPP_PEER_C5300V
#define TMP_PPP_PEER_C5300V_CONFIG_FILE "/tmp/"PPP_PEER_C5300V_NEW
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
#define SIM7230_IS_USED "/tmp/.module_sim7230_is_used"
#define AR9500_IS_USED "/tmp/.module_ar9500_is_used"
#define KM111E_IS_USED "/tmp/.module_km111e_is_used"
//禁止发送锁指定小区的AT命令
#define GPIO_11_AS_RUN_LED "/tmp/.gpio_11_as_run_led"
#define GPIO_19_AS_RUN_LED "/tmp/.gpio_19_as_run_led"
//流量统计报警文件
#define FLOW_STATISTICS_WARNING_FILE "/tmp/.flow_statistics_warning_file"
#define SLM630B_IS_USED "/tmp/.module_slm630b_is_used"
//检查是否可以正常连接lns服务器
#define RECHECK_IF_LNS_SERVER_IS_OK "/tmp/.recheck_if_lns_server_is_ok"
#define USE_SPECIFIED_LNS_SERVER_INDEX_FILE "/tmp/.use_specified_lns_server_index_file"
#define NOT_CHECK_IF_LNS_ALIVE_FILE "/tmp/.not_check_if_lns_alive_file"
#define ML7810_IS_USED "/tmp/.module_ml7810_is_used"
#define DDNS_HSK_STATUSINFOFILE "/tmp/hsk_login_status"
#define DDNS_DOMAININFO "/tmp/domaininfo"
#define DDNS_TMPINFO "/tmp/tmpinfo"
#define DDNS_3322_STATUSINFOFILE "/tmp/3322_login_status"
#define DDNS_NOIPSTATUS "/tmp/noip_login_status"
#define CURRENT_DOWNLINK_DEVICE_MAC "/usr/current_downlink_device_mac"
#define ALL_BLOCKED_URL_STRING "/tmp/all-blocked-url-string"
#define BLS001_IS_USED "/tmp/.module_bls001_is_used"
//检查是否可以正常连接lns服务器
#define RECHECK_IF_LNS_SERVER_IS_OK "/tmp/.recheck_if_lns_server_is_ok"
#define ML7810_IS_USED "/tmp/.module_ml7810_is_used"
#define ML7820_IS_USED "/tmp/.module_ml7820_is_used"
#define PPTP_PEER_NAME "pptp_vpn_client"
#define PPTP_PEER_PEER_FILE PPP_PEER_DIRECTORY"/"PPTP_PEER_NAME
//PPTP拨号状态文件
#define PPTP_DIAL_STATE_FLAG_FILE "/tmp/.pptp_dial_state_flag_file"
#define B78_IS_USED "/tmp/.module_b78_is_used"
#define B53_IS_USED		"/tmp/.module_b53_is_used"
#define B90_IS_USED "/tmp/.module_b90_is_used"
//读取到的EMCI信息总是写入该文件中
#define TMP_EMCI_INFO_FILE "/tmp/.tmp_emci_info_file"
#define HAVE_AUTO_LOCKED_ONE_CELL "/tmp/.have_auto_locked_one_cell"
#define HAVE_SELECTED_ONE_LNS "/tmp/.have_selected_one_lns"
#else
#define NEED_TO_REBOOT_TEMP_FILE "./need_to_reboot"
#define NEED_TO_RESTORE_SETTINGS_TEMP_FILE "./need_to_restore_settings"
#define NEED_TO_RESTORE_SETTINGS_NO_REBOOT_TEMP_FILE "./need_to_restore_settings_no_reboot"
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
//LT10模块被使用
#define LT10_IS_USED "./module_lt10_is_used"
//ZM8620模块被使用
#define ZM8620_IS_USED "./module_zm8620_is_used"
//使用的是SIM5360模块
#define SIM5360_IS_USED "./module_sim5360_is_used"
//使用的是P500模块
#define P500_IS_USED "./module_p500_is_used"
#define BM806_IS_USED "./module_bm806_is_used"
//开启了L3bridge模式
#define L3BRIDGE_IS_ENABLE "./l3bridge_is_enable"
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
//当前使用的卡为CDMA卡
#define CDMA_UIM_RUIM_CARD_USED_FILE "./cdma_uim_ruim_card_used"
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
#define L2TPD_CLIENT_OPTIONS_FILE "./options.l2tpd.client"
#define TMP_L2TPD_CLIENT_OPTIONS_FILE "./options.l2tpd.client"
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
#define CURRENT_DEVICE_IS_P13 "./current_device_is_p13"
#define CURRENT_DEVICE_IS_P51  "./current_device_is_p51"
#define CURRENT_DEVICE_IS_P21_VOIP "./current_device_is_p21_voip"
#define DISPLAY_WIFI_SETTINGS_PAGE "./display_wifi_settings_page"
#define LAN_LED_AS_WLAN_STATUS "./lan_led_as_wlan_status"
#define REBOOT_WIFI_MODULE "./reboot_wifi_module"
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
//重新拨号读取配置的标志文件
#define REDIAL_CONFIG_FILE_CHANGED "./redial_config_file_changed"
//SIM CARD不存在
#define SIM_CARD_IS_EXIST "./sim_card_is_exist"
//网络正常
#define INTERNET_IS_OK "./internet_is_ok"
//信号不正常
#define SIGNAL_IS_OK "./signal_is_ok"
//使用的是ZTE MC2716模块
#define MC2716_IS_USED "./module_mc2716_is_used"
#define PPP_PEER_MC2716 "mc2716-common"
#define PPP_PEER_MC2716_NEW "mc2716-common-new"
#define PPP_PEER_MC2716_CONFIG_FILE PPP_PEER_DIRECTORY"/"PPP_PEER_MC2716
#define TMP_PPP_PEER_MC2716_CONFIG_FILE "/tmp/"PPP_PEER_MC2716_NEW
#define C5300V_IS_USED "./module_c5300v_is_used"
#define PPP_PEER_C5300V "c5300v-common"
#define PPP_PEER_C5300V_NEW "c5300v-common-new"
#define PPP_PEER_C5300V_CONFIG_FILE PPP_PEER_DIRECTORY"/"PPP_PEER_C5300V
#define TMP_PPP_PEER_C5300V_CONFIG_FILE "/tmp/"PPP_PEER_C5300V_NEW
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
#define SIM7230_IS_USED "./module_sim7230_is_used"
#define KM111E_IS_USED "./module_km111e_is_used"
//禁止发送锁指定小区的AT命令
#define GPIO_11_AS_RUN_LED "./gpio_11_as_run_led"
#define GPIO_19_AS_RUN_LED "./gpio_19_as_run_led"
//流量统计报警文件
#define FLOW_STATISTICS_WARNING_FILE "./flow_statistics_warning_file"
#define SLM630B_IS_USED "./module_slm630b_is_used"
//检查是否可以正常连接lns服务器
#define RECHECK_IF_LNS_SERVER_IS_OK "./recheck_if_lns_server_is_ok"
#define USE_SPECIFIED_LNS_SERVER_INDEX_FILE "./use_specified_lns_server_index_file"
#define NOT_CHECK_IF_LNS_ALIVE_FILE "./not_check_if_lns_alive_file"
#define ML7810_IS_USED "./module_ml7810_is_used"
#define DDNS_HSK_STATUSINFOFILE "./hsk_login_status"
#define DDNS_DOMAININFO "./domaininfo"
#define DDNS_TMPINFO "./tmpinfo"
#define DDNS_3322_STATUSINFOFILE "./3322_login_status"
#define DDNS_NOIPSTATUS "./noip_login_status"
#define CURRENT_DOWNLINK_DEVICE_MAC "./current_downlink_device_mac"
#define ALL_BLOCKED_URL_STRING "./all-blocked-url-string"
#define BLS001_IS_USED "./module_bls001_is_used"
//检查是否可以正常连接lns服务器
#define RECHECK_IF_LNS_SERVER_IS_OK "./recheck_if_lns_server_is_ok"
#define ML7810_IS_USED "./module_ml7810_is_used"
#define ML7820_IS_USED "./module_ml7820_is_used"
#define PPTP_PEER_NAME "pptp_vpn_client"
#define PPTP_PEER_PEER_FILE PPP_PEER_DIRECTORY"/"PPTP_PEER_NAME
//PPTP拨号状态文件
#define PPTP_DIAL_STATE_FLAG_FILE "./pptp_dial_state_flag_file"
#define B78_IS_USED "./module_b78_is_used"
#define B53_IS_USED "./module_b53_is_used"
#define B90_IS_USED "./module_b90_is_used"
//读取到的EMCI信息总是写入该文件中
#define TMP_EMCI_INFO_FILE "./tmp_emci_info_file"
#define HAVE_AUTO_LOCKED_ONE_CELL "./have_auto_locked_one_cell"
#define HAVE_SELECTED_ONE_LNS "./have_selected_one_lns"

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

//pptp当前所处的模式
typedef enum
{
	TZ_PPTP_MODE_CLIENT=0,
	TZ_PPTP_MODE_SERVER=1,
}TZ_PPTP_MODE;

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

//LTE搜索网络的优先级
typedef enum
{
	LTE_SEARCH_NETWORK_STRATEGY_MIN=0,
	LTE_SEARCH_NETWORK_TDD_PREFERRED=LTE_SEARCH_NETWORK_STRATEGY_MIN,
	LTE_SEARCH_NETWORK_FDD_PREFERRED=1,
	LTE_SEARCH_NETWORK_NONE_PREFERRED=2,
	LTE_SEARCH_NETWORK_NOT_CHANGE=3,
	LTE_SEARCH_NETWORK_STRATEGY_MAX,
}LTE_SEARCH_NETWORK_STRATEGY;

//当前的CS/PS工作模式
typedef enum
{
	CS_PS_WORK_MODE_EPS_ONLY_DATA_CENTRIC=0,
	CS_PS_WORK_MODE_CS_PS_VOICE_CENTRIC=1,
	CS_PS_WORK_MODE_CS_PS_DATA_CENTRIC=2,
	CS_PS_WORK_MODE_EPS_ONLY_VOICE_CENTRIC=3,
	CS_PS_WORK_MODE_MAX,
}CS_PS_WORK_MODE;

//check if one file exist
int cmd_file_exist(const char* file_name);

//write string to one specified file
int cmd_echo(char* str,const char *file_name);
int cmd_echo_without_new_line(char* str,const char *file_name);
//append string to one specified file
int cmd_echo_append(char* str,const char *file_name);
//append string to one specified file
int cmd_echo_append_without_new_line(char* str,const char *file_name);

//去掉字符串中的特殊字符
void util_remove_special_char( char* one_string,char special_char );
//是否应该禁止漫游
int util_should_forbid_roaming( InfoStruct* info_struct );

//write string to one specified file
int cmd_touch(const char *file_name);

//当前使用的是CDMA卡
int util_is_cdma_card_used(void);

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
void util_turn_on_net_led( void );
void util_turn_off_net_led( void );
void util_turn_on_wifi_led( void );
void util_turn_off_wifi_led(void);
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

//获取查询当前所在的频段的AT命令
char* util_get_at_to_query_current_used_band( void );
//获取查询所有支持的频段的AT命令
char* util_get_at_to_query_all_supported_band( void );
//获取查询模块当前支持的频段的AT命令,这个可能和所有支持的频段不同
char* util_get_at_to_query_current_supported_band( void );
//用于对当前所在的频段的返回值进行解析的函数
int util_parse_current_used_band( char* current_used_band_result,char* dest_bit_map );
//用于对查询所有支持的频段的AT命令的返回值进行解析的函数
int util_parse_all_supported_band( char* all_supported_band_result,char* dest_bit_map );
int util_all_char_be_zero_or_zero_length( char* one_string );
//用于对查询模块当前支持的频段的AT命令的返回值进行解析的函数
int util_parse_current_supported_band( char* current_supported_band_result,char* dest_bit_map );
//用于进行锁频的函数
int util_get_at_to_lock_specified_band( char* all_supported_bands,char* current_supported_band_result,char* dest_at_cmd );
//是否支持进行锁频,支持返回TRUE,否则返回FALSE,支持的情况下使用checkbox进行选择,不支持的情况下,指示列出当前所有支持的频段及当前支持的频段
int util_is_universal_lock_band_supported( void );
//获取当前模块支持的网络模式信息,成功返回0,失败返回-1,失败或者返回的模式为空的情况下使用默认模式
int util_get_supported_network_mode_for_this_module( char* supported_network_mode );
//根据配置中设置的值,获取对应的实际应该设置的网络模式,这个值保存在TZ_LTE_MODULE_MODE中
int util_get_real_network_mode_from_settings( int network_mode );

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
int check_if_usb_device_exist( const UsbPair* matched_usb_pairs,int usb_pairs_count,int* matched_device_index );
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
//LT10模块被使用
int is_module_lt10_used(void);
//ZM8620模块被使用
int is_module_zm8620_used(void);
//P500模块被使用
int is_module_p500_used(void);
//检查使用的是否为BM806模块
int is_module_bm806_used(void);
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
//使用的是否为SIM7230_IS_USED模块
int is_module_sim7230_used(void);
//使用的是否为SLM630B模块
int is_module_slm630b_used(void);
//使用的是否为BLS001模块
int is_module_bls001_used(void);
//使用的是否为ML7810模块
int is_module_ml7810_used(void);
//使用的是否为ML7820模块
int is_module_ml7820_used(void);
//使用的是否为B78模块
int is_module_b78_used(void);
//使用的是否为B53模块
int is_module_b53_used(void);
//使用的是否为B90模块
int is_module_b90_used(void);
//使用的是否为EC20模块
int is_module_ec20_used(void);
//当前设备是否为P11+
int is_this_device_p11plus(void);
//当前设备是否为P13
int is_this_device_p13(void);
//当前设备是否为P21
int is_this_device_p21_voip(void);
//当前设备是否为P51
int is_this_device_p51(void);
//是否需要单独重启WIFI模块
int util_is_should_reboot_wifi_module(void);
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
//不支持GSM网络模式
int util_is_gsm_mode_not_supported( void );
//不支持WCDMA
int util_is_wcdma_mode_not_supported( void );
//不支持TDS-CDMA
int util_is_tdscdma_mode_not_supported( void );
//不支持LTE
int util_is_lte_mode_not_supported( void );
//优先选择WCDMA网络
int util_is_wcdma_preferred( void );
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
//由于测试发现util_calculate_pin_by_iccid这个函数的计算不太安全,因此需要使用一个更加随机的方法来计算随机PIN码
void util_calculate_pin_by_imei( char* original_imei,char* pin,int only_output_digit );
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
//convert act to network mode;
char* util_convert_act_to_network_mode_string( int act );
//获取根据AT+COPS?/AT+CGREG?/AT+CEREG?/AT+CREG?返回的网络模式信息返回对应的网络信息
//返回映射后的网络模式,并将网络模式实际代表的2G/3G/4G信息返回
int util_get_remapped_network_act( int act,char* network_mode );
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
//获取模块LOG的文件名称,返回不为空,则说明抓取的模块LOG文件在CPE内部,停止抓取LOG之后,需使用超链接进行下载
//否则不需要使用超链接进行下载
//例如返回的值为/tmp/sim5360-diag/sim5360-diag-log.tar.lzma,则需要在/tzwww目录下创建超链接文件进行下载
//ln -sf /tmp/sim5360-diag/sim5360-diag-log.tar.lzma $(INSTALL_ROOT)/tzwww/sim5360-diag-log.tar.lzma
char* util_get_modem_log_file_name( void );
//获取不支持的APN验证类型,如果不支持,则不显示对应的选项
//#define APN_AUTH_TYPE_NOT_SUPPORT_NONE (1<<0)
//#define APN_AUTH_TYPE_NOT_SUPPORT_PAP (1<<1)
//#define APN_AUTH_TYPE_NOT_SUPPORT_CHAP (1<<2)
//#define APN_AUTH_TYPE_NOT_SUPPORT_PAP_CHAP (1<<3)
unsigned int util_get_unsupported_apn_auth_type( void );
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

//+BANDINFO:GSM900,GSM1800;TDSCDMA BAND34,BAND39;TDD BAND38,BAND39,BAND40
//读取模块支持的LTE频段信息
void util_parse_bands_info( char* bands_info,int write_to_file,char* file_name,char* module_info );

//将TZ_BAND_PREF保存的锁频字段band_pref转换为ml7810的锁频字段ml7810_band_pref,成功返回0,否则返回其他值
int util_convert_band_pref_to_ml8710_set( char* band_pref,char* ml7810_band_pref );
//将ml7810的锁频字段ml7810_band_pref转换为TZ_BAND_PREF保存的锁频字段band_pref,成功返回0,否则返回其他值
int util_get_band_pref_from_ml8710_set( char* band_pref,char* ml7810_band_pref );

//获取不支持的网络模式
int util_get_unsupported_network_mode( InfoStruct* info_struct );

//获取存在的ppp网卡名称,成功返回0,否则返回其他
int util_get_ppp_net_dev( char* ppp_dev );

//路由模式是否被开启
int util_is_router_mode_enabled( InfoStruct* info_struct );

//L3bridge模式是否被开启
int util_is_l3bridge_mode_enabled( void);
/*
去掉cfg-e输出的字符串中包含如下形式的情况
export TZ_WMAC_GATEWAY="192.168.1.2"
export TZ_WMAC_DNS="202.96.134.133"
export TZ_SUPER_USERNAME=rootaccount
export TZ_SUPER_PASSWD=6b63f2cf2a0933f0a4131ced5d7f476b
export aaaa="\"cndistgt"
*/
void util_strip_quote_string_from_cfg_e( char* processed_quoted_string );

//LTE网络搜索策略
int util_get_lte_search_network_strategy( InfoStruct* info_struct );

//当前模块是否支持L3 bridging模式
int util_is_this_module_support_l3_bridge_mode( void );

//是否对不存在的LNS服务器进行重新拨号检查
int util_should_check_if_lns_alive( InfoStruct* info_struct );

//检查是否允许注册到当前的网络
int util_is_register_to_curent_plmn_alowed( InfoStruct* info_struct,char* imsi );

//是否需要对admin的帐号进行随机化
int util_should_admin_password_randomized( InfoStruct* info_struct );

//统计指定目录下的文件数量
int util_get_file_count_under_one_directory( char* directory_name );

//将所有的小写字母都转换为大写字母
void util_convert_all_chars_to_upper_case( char* original_string );
//将所有的小写字母都转换为大写字母
void util_convert_all_chars_to_lower_case( char* original_string );

//指示SIM卡存在
void util_turn_on_sim_led( void );
//指示SIM卡不存在
void util_turn_off_sim_led( void );

//获取当前设置的cs/ps业务模式
int util_get_cs_ps_service_mode( InfoStruct* info_struct );

//根据USB的idVendor/idProduct/interface_number信息获取对应的串口号,0表示成功,否则表示失败
int util_get_serial_name_by_usb_interface( int idVendor,int idProduct,int interface_number,char* serial_name );
//查找所有可用的USB串口,0表示成功,否则表示失败
int util_get_available_usb_ports( UsbSerialTable* usb_serial_table );

//判定模块 LOG 是否正在进行抓取的,调用如下函数判定是否正在抓取 LOG:
int util_is_log_tools_running(void);

//为高通使用gobinet驱动的设备查找对应的字符设备/dev/qcqmi*,成功返回0,失败返回非0
//将设备名称/设备的主设备号/次设备号保存到char_device_name/major_num/minor_number中
int util_get_gobinet_char_device_name( char* char_device_name,int* major_num,int* minor_number );

//检查是否需要重新创建设备的设备号,1表示需要,否则表示不需要
//char_device_name为设备名称,major_num为主设备号,minor_number为次设备号
int util_should_recreate_dev_file( char* char_device_name,int major_num,int minor_number );


//为网页提供用户发送AT命令的端口
char* util_get_at_port( void );

//获取当前模块在网页上不能显示的页面项
unsigned int util_get_unsupported_web_page_features( void );
//获取当前模块在网页上需要显示的页面
unsigned int util_get_supported_web_page_features( void );

//检查当前pptp客户端或服务器是否被开启,为yes表示被开启,否则表示没有被开启
//开启,返回TRUE,否则返回FALSE
int util_is_pptp_function_enabled( InfoStruct* nvram_config_info );
//获取当前PPTP功能所处的模式
int util_get_pptp_mode( InfoStruct* nvram_config_info );
//pptp是否支持MPPE加密
int util_is_pptp_mppe_enabled( InfoStruct* nvram_config_info );
//使用连接上的pptp vpn作为默认路由
int util_is_pptp_vpn_used_as_default_route( InfoStruct* nvram_config_info );

//获取存在的ppp网卡名称,成功返回0,否则返回其他
int util_get_ppp_net_dev( char* ppp_dev );
//获取当前使用的lan口
void get_used_lan(void);
//返回获取LTE状态信息需要发送的AT命令
char* util_get_at_to_query_lte_status( void );
//对获取的LTE状态信息进行解析
int util_parse_lte_status( char* lte_status_response,char* lte_status_result );
//获取查询锁物理小区的AT命令
char* util_get_at_to_query_lock_one_cell_status( void );
//对获取到的锁物理小区查询的结果进行解析
//locked_status,1表示被锁定,否则表示没有被锁定
//earfcn,physical_cell_id使用10进制
int util_parse_lock_one_cell_status( char* lock_one_cell_response,int* locked_status,char* earfcn,char* physical_cell_id  );
//执行锁物理小区操作锁需要的AT命令
//lock_or_unlock,1表示被锁定,否则表示没有被锁定
//earfcn,physical_cell_id使用10进制
int util_get_at_to_lock_one_cell( int lock_or_unlock,char* earfcn,char* physical_cell_id,char* dest_at_command );
//获取查询当前所在的频点和物理小区ID的AT命令
char* util_get_at_to_query_earfcn_physical_cell_id( void );
//对当前所在的频段和物理小区ID进行解析
int util_parse_query_earfcn_physical_cell_id_response( char* lte_status_response,char* lte_status_result );
//获取当前模块支持的网络模式信息,成功返回0,失败返回-1,失败或者返回的模式为空的情况下使用默认模式
//获取所有支持的频段的名称
int util_get_all_supported_band_names( char* all_band_map,char* band_names );
#endif

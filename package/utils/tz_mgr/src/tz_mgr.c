#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>   /* The L2 protocols */
#include <pthread.h>
#include <uci.h>


#include "configtoolc.h"
#include "tz_common.h"
#include "Cshell.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define TZ_WIFI_MAX_TXPOWER 23
//send to server
#define PROTOCOL_TYPE_ECTSP 0x9886

//send to client
#define PROTOCOL_TYPE_ECTCP 0x9887

#define DEV_LED_RED         "r"
#define DEV_LED_GREEN       "g"
#define DEV_LED_WPS         "wps"
#define CMD_LED_ON          1
#define CMD_LED_OFF         0


#define NAME_OF_WIRELESS_INTERFACE  "ra0"
#define SCRIPT_DHCPD        "/etc/init.d/odhcpd"
#define SCRIPT_DNSMAQ       "/etc/init.d/dnsmasq"
#define SCRIPT_HTTPD        "/etc/init.d/mini_httpd"
#define TZ_FACTORY_FLAG_FILE "/etc/.flag_factory_config"
#define TZ_SCRIPT_RESTORE   "/etc/tozed/tz_restore_factory"
#define TZ_VERSION_FILE     "/version"
#define TZ_FIRMWARE_VERSION "software_version"
#define TZ_FIRMWARE_INLINE_VERSION "inline_soft_version"
#define TZ_CONFIG_VERSION   "config_version"
#define TZ_MGR_RESTART_COUNT_FILE "/tmp/.tz_mgr_restart_count"//记录程序的重启次数，从而计算连接不上室外机的时间
#define TZ_RESTORE_NO_RESPONE_COUNT_FILE "/tmp/.tz_mgr_restore_no_respone_count"//记录发送恢复出厂指令没收到回复的次数，若超过5次视为与室外机失联，清除出厂标志，即不通知室外机恢复出厂
#define TZ_MGR_ENABLE_DHCP_FLAG_FILE "/tmp/.tz_mgr_dhcp_enable"//如果存在此文件，则启动dhcp服务
#define TZ_DHCP_SERVER_ALIVE_FLAG_FILE "/tmp/.tz_mgr_dhcp_alive"//记录dhcp启动状态，防止重复启动
#define AFTER_KEY_RESTORE_FLAG_FILE "/etc/.flag_after_key_restore_factory"//通过按键恢复出厂后创建，如果存在此文件，通知室外机恢复出厂


#define APP_NAME    "tz_mgr"  //程序名字,必须与配置文件及脚本一致！

//空宏
#define IN 
#define OUT

#define print(format,...)  if(global_debugPrint_flag){printf("%s--%s--%d: ",__FILE__,__FUNCTION__,__LINE__), printf(format,##__VA_ARGS__), printf("\n");}





int CreateRawSocket(char const *ifname, unsigned short type,IN unsigned char *hwaddr,IN int *IfIndex);
int util_decode_frame_info(unsigned char* ethernet_frame);
int util_decode_field_info(unsigned char* p_ethernet_frame,int frame_len);
int util_config_ipv4_addr(const char *net_dev, const char* ipaddr);
int util_client_send_reboot_sync_frame(void);
int util_client_send_reboot_sync_ack_frame(void);
int util_client_send_reboot_ack_frame(void);
int util_client_send_restore_sync_frame(void);
int util_client_send_restore_sync_ack_frame(void);
int util_client_send_restore_ack_frame(void);
int util_client_send_update_sync_ack_frame(void);
int util_client_send_only_restore_reply_frame(void);
int util_send_ack_frame(int socket_handle,unsigned short ethernet_frame_type,unsigned char ack_cmd,unsigned char* self_mac_addr,unsigned char* remote_mac_addr);
int util_send_search_server_frame(void);
int util_sync_config_info(void);
int util_client_send_probe_server_frame(int have_server_mac);
void process_1s_signal(void);
int process_client_state_machine( int socket_handle );
int uci_get_config(InfoStruct* server_wifi_info);//获取数据库内容到缓冲区
void w13_config_lte_led_according2quality(int quality);
void w13_config_lte_led_according2quality_color(int color);
void w13_config_led_according2dial_status(int dial_status);

char network_dev_name[16]="";//网络接口的名字
unsigned char self_mac_addr[6]="";//本机网络接口的MAC地址
int self_interface_index=0;//网络接口在内核的index
unsigned char remote_mac_addr[6]="";//服务端MAC地址
bool global_debugPrint_flag = false; 
unsigned char ethernet_frame_data[4000];//接收数据包缓冲区
static unsigned char ethernet_frame_buffer[4000];//发送数据包缓冲区
static int client_status=CLIENT_STATUS_INITIALIZED;//客户机状态
int g_send_sock_fd = 0;//发送至服务端的套接字
int g_receive_sock_fd = 0;
pthread_t tid_recv,tid_heartbeat;//接收和发送的tid
pthread_rwlock_t send_sock_rwlock;//套接字的读写锁
char TZ_FOTA_UPGRADE_IP[16];     //temp ip
char TZ_FOTA_UPGRADE_IP_BACKUP[16];//temp ip backup
void tz_get_if_ip(char* ip_buf,int buf_len);//获取当前接口的ip
void tz_check_restart_count(void);//获取重启次数，目前每5秒启动一次，超过1分钟没连接则启动dhcp server，否则关掉它
int setWirelessCfgValue(char *name,char *value);//使用cuci设置wireless参数


//copy form miao
static volatile int is_connected_to_server=FALSE;
static volatile int frame_sent_counter=0;
static volatile int first_connected=TRUE;
static volatile int first_disconnected=TRUE;
static InfoStruct server_wifi_info;
//check if the new configure is the same as the old configure info
static InfoStruct server_wifi_info_backup;
static volatile int reboot_timer_started=FALSE;
static volatile int restore_settings_timer_started=FALSE;
static volatile int reboot_timer_counter=0;


void *recv_thread(void* tag)//接收线程入口
{   
	unsigned short listen_protocol_type = PROTOCOL_TYPE_ECTCP;//监听发送到客户端的数据包

	
	g_receive_sock_fd=CreateRawSocket(network_dev_name,listen_protocol_type, NULL, NULL);
	if(g_receive_sock_fd < 0)
	{
		print("CreateRawSocket recv sock failed!ret=%d",g_receive_sock_fd);
	}
	process_client_state_machine(g_receive_sock_fd);
	pthread_exit("recv_thread error"); // 退出当前线程
}


void *heartbeat_thread(void* tag)//心跳线程入口
{   
	while(1)
	{
		sleep(1);
		process_1s_signal();
	}
	pthread_exit("heartbeat_thread error"); // 退出当前线程
}

void signal_handle_func(int sig)//退出
{
	if(tid_recv > 0)
	{
		pthread_cancel(tid_recv);
	}
	if(tid_heartbeat > 0)
	{
		pthread_cancel(tid_heartbeat);
	}
	pthread_rwlock_destroy(&send_sock_rwlock);
	if(g_send_sock_fd > 0)
	{
		close(g_send_sock_fd);
	}
	if(g_receive_sock_fd>0)
	{
		close(g_receive_sock_fd);
	}
	config_deinit();
	print("%s","-------------exit after deinit---------------");
	exit(0);
}

//室内机
int main(int argc, char * const argv[])
{
	tz_check_restart_count();
	const char *optstr = "i:D";
	int ch;
	if( argc == 1 )
	{
		printf("%s","please use -i [net_work_interface] specify the network interface!");
		return 0;
	}

	while( ( ch=getopt(argc,argv,optstr) ) != -1 )
	{
		switch( ch )
		{
			case 'i':
				strcpy(network_dev_name,optarg);
				break;
			case 'D':
				global_debugPrint_flag = true;
				break;  
		}
	}
	pthread_rwlock_init(&send_sock_rwlock, NULL);//初始化读写锁
	memset(TZ_FOTA_UPGRADE_IP, 0, sizeof(TZ_FOTA_UPGRADE_IP));
	memset(TZ_FOTA_UPGRADE_IP_BACKUP, 0, sizeof(TZ_FOTA_UPGRADE_IP_BACKUP));
	memset( &server_wifi_info,0,sizeof( server_wifi_info ) );
	//因系统信号退出时做好善后
	signal(SIGINT, signal_handle_func);
	signal(SIGQUIT, signal_handle_func);
	signal(SIGTERM, signal_handle_func);
	signal(SIGPIPE, signal_handle_func);

	if( strlen(network_dev_name) == 0 )
	{
		print("%s","please use -i [net_work_interface] specify the network interface!");
		return -1;
	}
	print("network_dev_name=%s",network_dev_name);
	get_interface_mac(NAME_OF_WIRELESS_INTERFACE,self_mac_addr);
	print("wifi_mac_addr=%02X:%02X:%02X:%02X:%02X:%02X:"
		,self_mac_addr[0],self_mac_addr[1],self_mac_addr[2]
		,self_mac_addr[3],self_mac_addr[4],self_mac_addr[5]);

	unsigned short send_protocol_type = PROTOCOL_TYPE_ECTSP;//发送到服务端协议类型
	g_send_sock_fd=CreateRawSocket(network_dev_name,send_protocol_type, NULL, &self_interface_index);
	if(g_send_sock_fd<0)
	{
		print("CreateRawSocket failed! ret=%d",g_send_sock_fd);
		return -1;
	}
	int ret=0;
	ret = uci_get_config(&server_wifi_info);
	if(ret < 0)
	{
		print("error:%s","uci_get_config erorr!");
		return -1;
	}
	
	

	ret = pthread_create(&tid_recv, NULL, recv_thread, NULL);
	if(ret<0)
	{
		print("Create recv thread error!%s\n", strerror(ret));
		return -1;
	}

	ret = pthread_create(&tid_heartbeat, NULL, heartbeat_thread, NULL);
	if(ret<0)
	{
		print("Create key thread error!%s\n", strerror(ret));
		return -1;
	}
	

	//阻塞接合子线程
	void *dead_str;
	pthread_join(tid_recv, &dead_str);
	print("recv_thread exit! %s",(char*) dead_str);
	pthread_join(tid_heartbeat, &dead_str);
	print("recv_thread exit! %s",(char*) dead_str);
	pthread_rwlock_destroy(&send_sock_rwlock);
	if(g_send_sock_fd > 0)
	{
		close(g_send_sock_fd);
	}
	if(g_receive_sock_fd>0)
	{
		close(g_receive_sock_fd);
	}
	

	return 0;
}




//创建原始套接字
int CreateRawSocket(char const *ifname, unsigned short type,IN unsigned char *hwaddr,IN int *IfIndex)
{
	int optval=1;
	int fd;
	struct ifreq ifr;
	int domain, stype;

	struct sockaddr_ll sa;

	memset(&sa, 0, sizeof(sa));

	domain = PF_PACKET;
	stype = SOCK_RAW;
	 
	if ((fd = socket(domain, stype, htons(type))) < 0) {
		print("Create socket failed!");
		return -1;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < 0) {
		print("setsockopt SO_BROADCAST failed!");
	}
	
	if (hwaddr) {
		strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
		if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) 
		{
			print("ioctl(SIOCGIFHWADDR) failed!");
			return -2;
		}

		memcpy(hwaddr, ifr.ifr_hwaddr.sa_data,6);
	}

	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(fd, SIOCGIFMTU, &ifr) < 0) {
		print("ioctl(SIOCGIFMTU) failed!");
		return -2;
	}

	if (ifr.ifr_mtu < ETH_DATA_LEN) {
		char buffer[256];
		print(buffer, "Interface %.16s has MTU of %d -- should be %d.  You may have serious connection problems.",
			ifname, ifr.ifr_mtu, ETH_DATA_LEN);
	}

	sa.sll_family = AF_PACKET;
	sa.sll_protocol = htons(type);
	strncpy(ifr.ifr_name,ifname,sizeof(ifr.ifr_name));

	if (ioctl(fd,SIOCGIFINDEX,&ifr)<0)
	{
		print("ioctl(SIOCFIGINDEX):Could not get interface index");
		return -2;
	}
	sa.sll_ifindex = ifr.ifr_ifindex;   

	if(IfIndex){
			*IfIndex = ifr.ifr_ifindex;
	}

	if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		print("bind socket failed!");
		return -3;
	}

	return fd;
}

//客户端状态机
int process_client_state_machine( int socket_handle  )
{
	int recved_data_len;

	while(1)
	{
		switch( client_status )
		{
			//initial state
			case CLIENT_STATUS_INITIALIZED:
				//send probe frame to search server
				{
					//waiting for server's response
					client_status=CLIENT_STATUS_SEARCH_SERVER;
				}
				break;
			//find the server
			case CLIENT_STATUS_SEARCH_SERVER:
				if( !util_receive_ethernet_frame( socket_handle,ethernet_frame_data,&recved_data_len ) )
				{
					//process frame info
					util_decode_frame_info(ethernet_frame_data);
				}
				break;
			//get parameters from server
			case CLIENT_STATUS_REQUEST_PARAMETERS:
				break;
			//download update file
			case CLIENT_STATUS_DOWNLOAD_UPDATE_FILE:
				break;
			//update the system
			case CLIENT_STATUS_UPDATE_SYSTEM:
				break;
			//reboot client and the server
			case CLIENT_STATUS_REBOOT_SYSTEM:
				break;
			//daemon status
			case CLIENT_STATUS_DAEMON:
				//receive and decode message info
				if( !util_receive_ethernet_frame( socket_handle,ethernet_frame_data,&recved_data_len ) )
				{
					//process frame info
					util_decode_frame_info(ethernet_frame_data);
				}
				break;
		}

	}

	return 0;
}

int util_decode_frame_info(unsigned char* ethernet_frame)
{
	unsigned char* p_ethernet_frame=ethernet_frame;
	int current_len=0;
	int frame_len=ntohs( *( unsigned short* )(ethernet_frame+OFFSET_OF_ETHERNET_LEN) )+OFFSET_OF_ETHERNET_DATA;
	unsigned char field;
	unsigned int field_length;
	unsigned char fake_field_length;
	unsigned char* p_fake_field_length=&fake_field_length;
	unsigned int current_len_before;
	char* p_null=NULL;
	unsigned char *p_field=&field;
	unsigned char *p_remote_mac_addr=remote_mac_addr;


	//get dest mac address
	util_get_common_data_from_frame(p_null,LENGTH_OF_ETHERNET_DST_MAC);
	//get source mac address
	util_get_common_data_from_frame(p_remote_mac_addr,LENGTH_OF_ETHERNET_SRC_MAC);
	//get ethernet frame type
	util_get_common_data_from_frame(p_null,LENGTH_OF_ETHERNET_TYPE);
	//get ethernet frame len
	util_get_common_data_from_frame(p_null,LENGTH_OF_ETHERNET_LEN);

	//保存当前的MAC地址到文件ALL_MACS_TMP_FILE
	//util_save_server_mac( remote_mac_addr );

	while( current_len < frame_len-LENGTH_OF_ETHERNET_CHECKSUM )
	{
		//always begin with even byte
		if( current_len&0x01 )
		{
			p_ethernet_frame++;
			current_len++;
		}
		//get field value
		util_get_common_data_from_frame(p_field,LENGTH_OF_FIELD);
		//get field length
		util_get_common_data_from_frame(p_fake_field_length,LENGTH_OF_FIELD_LENGTH);
		current_len_before=current_len;

		field_length=frame_len-current_len-LENGTH_OF_ETHERNET_CHECKSUM;

		switch( field )
		{
			//advertise the server
			case CMD_TYPE_SERVER_INFO:
				//have received server info
				{
					//get server info success
					if( !util_decode_field_info(p_ethernet_frame,field_length) )
					{
						client_status=CLIENT_STATUS_DAEMON;
						//check if the config have changed
						if( memcmp( &server_wifi_info_backup,&server_wifi_info,offsetof(InfoStruct,before_nvram_parameter_field) ) )
						{
							print("start to sync system config!");
							//config have changed
							if( util_sync_config_info() )
							{
								//config changed,reboot
								if( !server_wifi_info.config_changed_by_server )
								{
									print("%s","------------The configuration has been changed by server----------------");
									sleep(1);
									#ifndef __i386__
									//system("reboot");
									#endif
								}
							}
							memcpy( &server_wifi_info_backup,&server_wifi_info,sizeof(server_wifi_info) );
						}

						//被分配的IP地址和服务器的IP地址不同
						/*if( server_wifi_info.is_ip_not_the_same_with_server )
						{
							//设置当前设备的IP地址
							util_config_ipv4_addr( network_dev_name,server_wifi_info.AP_IPADDR );
						}
						else
						{
							util_config_ipv4_addr( network_dev_name,"0.0.0.0" );
						}*/
					}
				}
				break;
			case CMD_TYPE_REBOOT_SYNC:
				{
					//室外机主动重启,要求室内机也重启的命令，首先需要回复，室外机收到回复会发送CMD_TYPE_REBOOT_SYNC给室内机
					print("%s","------------receive reboot command from server----------------");
					util_client_send_reboot_sync_ack_frame();
				}
				break;
			case CMD_TYPE_REBOOT_SYNC_ACK:
				{
					//室内机主动重启，发送CMD_TYPE_REBOOT_SYNC至室外机后，收到室外机的回复，此时可以进行重启，下面的回复可能有点多余
					//send reboot sync ack message
					util_client_send_reboot_ack_frame();
					//wait for the frame is sent
					print("%s","------------reboot by server----------------");
					sleep(2);
					//start to reboot
					#ifndef __i386__
					system("reboot");
					#endif
				}
				break;
			case CMD_TYPE_REBOOT_ACK:
				{
					//室外机已经收到回复，可以进行重启操作,(form  CMD_TYPE_REBOOT_SYNC)
					print("%s","receive CMD_TYPE_REBOOT_ACK-------->reboot!");
					//start to reboot
					system("reboot");

				}
				break;
			case CMD_TYPE_UPDATE_SYNC:
				{
					print("%s","receive CMD_TYPE_UPDATE_SYNC!");
					util_client_send_update_sync_ack_frame();
				}
				break;
			case CMD_TYPE_UPDATE_ACK:
				{
					print("%s","receive CMD_TYPE_UPDATE_ACK!");
					//start to update file
				}
				break;
			case CMD_TYPE_SERVER_IND:
				{
					print("%s","receive CMD_TYPE_SERVER_IND!");
					first_disconnected=TRUE;
					frame_sent_counter=0;
					is_connected_to_server=TRUE;
					util_decode_field_info(p_ethernet_frame,field_length);
					if( client_status == CLIENT_STATUS_SEARCH_SERVER )
					{
						util_send_search_server_frame();
					}
				}
				break;
			case CMD_TYPE_RESTORE_SYNC:
				{
					//室外机主动恢复出厂，通知室内机的同步消息，只需要回复
					//如果室外机收到回复，会发送CMD_TYPE_RESTORE_ACK进行确认
					print("%s","receive CMD_TYPE_RESTORE_SYNC!");
					//send reboot sync ack message
					util_client_send_restore_sync_ack_frame();
				}
				break;
			case CMD_TYPE_RESTORE_SYNC_ACK:
				{
					//室内机主动（按键）恢复出厂后，要求室外机也回复出厂，发送CMD_TYPE_RESTORE_SYNC命令至室外机后，收到室外机的回复
					//这里实际上因为室内机已经是恢复出厂后的状态，下面无需恢复出厂，重启(与室外机同步)即可。
					//send reboot sync ack message
					print("receive CMD_TYPE_RESTORE_SYNC_ACK ,send CMD_TYPE_RESTORE_ACK now....");
					util_client_send_restore_ack_frame();
					//wait for the frame is sent
					print("%s","CMD_TYPE_RESTORE_SYNC_ACK------------>restore factory and reboot!");
					shell_recv(NULL,0,"rm -f %s", AFTER_KEY_RESTORE_FLAG_FILE);
					sleep(2);//做足延时，防止没发完就重启
					shell_recv(NULL,0,"(sleep 1 && reboot)&");
					
				}
				break;
			case CMD_TYPE_RESTORE_ACK:
				{
					//start to reboot
					//室外机已经收到回复，室内机可以恢复出厂，(form  CMD_TYPE_RESTORE_SYNC)
					print("%s","CMD_TYPE_RESTORE_ACK------------>restore factory and reboot!");
					shell_recv(NULL,0,"%s && reboot",TZ_SCRIPT_RESTORE);
					#ifndef __i386__
					system("reboot");
					#endif
				}
				break;
			case CMD_TYPE_ONLY_RESTORE_CONFIGS:
				{
					print("%s","CMD_TYPE_ONLY_RESTORE_CONFIGS------------>restore factory without reboot!");
					//发送确认帧
					//室外机主动恢复出厂，但是用户要求稍后重启，需要回复室外机后恢复出厂，但不重启
					util_client_send_only_restore_reply_frame();
					shell_recv(NULL,0,"%s",TZ_SCRIPT_RESTORE);
				}
				break;


			//not recognized cmd
			default:
				print("unkown cmd 0x%x", field);
				util_get_common_data_from_frame(p_null,field_length);
				break;
		}

		//avoid error
		if( current_len-current_len_before < field_length )
		{
			current_len=current_len_before+field_length;
		}

		//always begin with even byte
		if( current_len&0x01 )
		{
			p_ethernet_frame++;
			current_len++;
		}
	}

	return 0;
}


//收到服务器下发的配置信息
int util_decode_field_info(unsigned char* p_ethernet_frame,int frame_len)
{


	int current_len=0;
	unsigned char field;
	unsigned char *p_field=&field;
	unsigned char field_length;
	unsigned char *p_field_length=&field_length;
	unsigned int current_len_before;
	unsigned char* p_ethernet_frame_before;
	char* p_null=NULL;
	while( current_len < frame_len )
	{
		//always begin with even byte
		if( current_len&0x01 )
		{
			p_ethernet_frame++;
			current_len++;
		}

		//get field value
		util_get_common_data_from_frame(p_field,LENGTH_OF_FIELD);
		//get field length
		util_get_common_data_from_frame(p_field_length,LENGTH_OF_FIELD_LENGTH);
		current_len_before=current_len;
		p_ethernet_frame_before=p_ethernet_frame;

		switch( field )
		{
			case CMD_INFO_TZ_ENABLE_WIFI:
				print("CMD_INFO_TZ_ENABLE_WIFI:%s",( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.TZ_ENABLE_WIFI,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_SSID:
				strcpy(server_wifi_info.AP_SSID,( const char* )p_ethernet_frame);
				print("CMD_INFO_AP_SSID:%s", ( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_SECMODE:
				strcpy(server_wifi_info.AP_SECMODE,( const char* )p_ethernet_frame);
				print("CMD_INFO_AP_SECMODE:%s", ( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_WPA:
				print("CMD_INFO_AP_WPA:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_WPA,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_SECFILE:
				print("CMD_INFO_AP_SECFILE:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_SECFILE,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_CYPHER:
				print("CMD_INFO_AP_CYPHER:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_CYPHER,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_PSK_KEY:
				print("CMD_INFO_PSK_KEY:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.PSK_KEY,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_WEP_MODE_0:
				print("CMD_INFO_AP_WEP_MODE_0:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_WEP_MODE_0,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_PRIMARY_KEY_0:
				print("CMD_INFO_AP_PRIMARY_KEY_0:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_PRIMARY_KEY_0,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_WEP_RADIO_NUM0_KEY_1:
				print("CMD_INFO_WEP_RADIO_NUM0_KEY_1:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.WEP_RADIO_NUM0_KEY_1,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_WEP_RADIO_NUM0_KEY_2:
				print("CMD_INFO_WEP_RADIO_NUM0_KEY_2:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.WEP_RADIO_NUM0_KEY_2,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_WEP_RADIO_NUM0_KEY_3:
				print("CMD_INFO_WEP_RADIO_NUM0_KEY_3:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.WEP_RADIO_NUM0_KEY_3,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_WEP_RADIO_NUM0_KEY_4:
				print("CMD_INFO_WEP_RADIO_NUM0_KEY_4:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.WEP_RADIO_NUM0_KEY_4,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_PUREG:
				print("CMD_INFO_PUREG:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.PUREG,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_RATECTL:
				print("CMD_INFO_RATECTL:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.RATECTL,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_MANRATE:
				print("CMD_INFO_MANRATE:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.MANRATE,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_MANRETRIES:
				print("CMD_INFO_MANRETRIES:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.MANRETRIES,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_WIFIWORKMODE:
				print("CMD_INFO_WIFIWORKMODE:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.WIFIWORKMODE,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_TZ_ISOLATE_WLAN_CLIENTS:
				print("CMD_INFO_TZ_ISOLATE_WLAN_CLIENTS:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.TZ_ISOLATE_WLAN_CLIENTS,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_PUREN:
				print("CMD_INFO_PUREN:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.PUREN,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_WPA_GROUP_REKEY:
				print("CMD_INFO_AP_WPA_GROUP_REKEY:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_WPA_GROUP_REKEY,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_WPA_GMK_REKEY:
				print("CMD_INFO_AP_WPA_GMK_REKEY:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_WPA_GMK_REKEY,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_HIDESSID:
				print("CMD_INFO_AP_HIDESSID:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_HIDESSID,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_IPADDR:
				print("CMD_INFO_AP_IPADDR:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_IPADDR,( const char* )p_ethernet_frame);
				server_wifi_info.is_ip_not_the_same_with_server=FALSE;
				break;
			case CMD_INFO_IP_NOT_THE_SAME_WITH_SERVER:
				print("CMD_INFO_IP_NOT_THE_SAME_WITH_SERVER:%s", ( const char* )p_ethernet_frame);
				server_wifi_info.is_ip_not_the_same_with_server=atoi( ( const char* )p_ethernet_frame );
				break;
			case CMD_INFO_AP_NETMASK:
				print("CMD_INFO_AP_NETMASK:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_NETMASK,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_TZ_DHCP_IP_BEGIN:
				print("CMD_INFO_TZ_DHCP_IP_BEGIN:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.TZ_DHCP_IP_BEGIN,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_TZ_DHCP_IP_END:
				print("CMD_INFO_TZ_DHCP_IP_END:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.TZ_DHCP_IP_END,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_TZ_DHCP_EXPIRE_TIME:
				print("CMD_INFO_TZ_DHCP_EXPIRE_TIME:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.TZ_DHCP_EXPIRE_TIME,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_DIAL_STATUS:
				print("CMD_INFO_DIAL_STATUS:%s", ( const char* )p_ethernet_frame);
				server_wifi_info.dial_status=atoi( ( const char* )p_ethernet_frame );
				w13_config_led_according2dial_status(server_wifi_info.dial_status);
				break;
			case CMD_INFO_SIGNAL_QUALITY:
				print("CMD_INFO_SIGNAL_QUALITY:%s", ( const char* )p_ethernet_frame);
				server_wifi_info.signal_quality=atoi( ( const char* )p_ethernet_frame );
				w13_config_lte_led_according2quality(server_wifi_info.signal_quality);
				//cmd_update_signal_ind( server_wifi_info.signal_quality,FALSE );
				break;
			case CMD_INFO_SIGNAL_QUALITY_COLOR:
				server_wifi_info.signal_quality_color=atoi( ( const char* )p_ethernet_frame );
				w13_config_lte_led_according2quality_color(server_wifi_info.signal_quality_color);
				//cmd_update_signal_ind_color(server_wifi_info.signal_quality_color);
				break;
			case CMD_INFO_RESEND_SEARCH_SERVER:
				print("CMD_INFO_RESEND_SEARCH_SERVER:%s", ( const char* )p_ethernet_frame);
				util_send_search_server_frame();
				break;
			case CMD_INFO_CONFIG_CHANGED_BY_SERVER:
				print("CMD_INFO_CONFIG_CHANGED_BY_SERVER:%s", ( const char* )p_ethernet_frame);
				server_wifi_info.config_changed_by_server=atoi( ( const char* )p_ethernet_frame );
				break;
			case CMD_INFO_AP_PRIMARY_CH:
				print("CMD_INFO_AP_PRIMARY_CH:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_PRIMARY_CH,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_CHMODE:
				print("CMD_INFO_AP_CHMODE:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_CHMODE,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_TXPOWER:
				print("CMD_INFO_TXPOWER:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.TXPOWER,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_TZ_ENABLE_WATCHDOG:
				print("CMD_INFO_TZ_ENABLE_WATCHDOG:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.TZ_ENABLE_WATCHDOG,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_AP_GATEWAY_IP:
				print("CMD_INFO_AP_GATEWAY_IP:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.AP_GATEWAY,( const char* )p_ethernet_frame);
				break;
			case CMD_INFO_TZ_WIFI_40M_ENABLE:
				print("CMD_INFO_TZ_WIFI_40M_ENABLE:%s", ( const char* )p_ethernet_frame);
				strcpy(server_wifi_info.TZ_WIFI_40M_ENABLE,( const char* )p_ethernet_frame);
				break;      
			case CMD_INFO_TZ_FOTA_UPGRADE_IP:
				strcpy(TZ_FOTA_UPGRADE_IP,( const char* )p_ethernet_frame);
				if( strlen( TZ_FOTA_UPGRADE_IP ) )
				{   
					tz_get_if_ip(TZ_FOTA_UPGRADE_IP_BACKUP,sizeof(TZ_FOTA_UPGRADE_IP_BACKUP));
					if( strcmp( TZ_FOTA_UPGRADE_IP,TZ_FOTA_UPGRADE_IP_BACKUP ) )
					{
						print("set ip to %s for upgrade ...",TZ_FOTA_UPGRADE_IP);
						util_config_ipv4_addr(network_dev_name,TZ_FOTA_UPGRADE_IP);
						shell_recv(NULL,0,"ifconfig %s down && ifconfig %s up", network_dev_name ,network_dev_name);//
						shell_recv(NULL,0,"%s stop",SCRIPT_HTTPD);
						//strcpy(TZ_FOTA_UPGRADE_IP_BACKUP,TZ_FOTA_UPGRADE_IP);
						memset( TZ_FOTA_UPGRADE_IP,0,sizeof( TZ_FOTA_UPGRADE_IP ) );
						print("restart web\r\n");
						sleep(2);
						shell_recv(NULL,0,"%s start",SCRIPT_HTTPD);
						shell_recv(NULL,0,"rm -f %s",UPLINK_CONNECTION_IS_OK_TMP_FILE);
						raise(SIGQUIT);//restart self
					}
				}
				break;
				/*if( !strcmp(server_wifi_info.TZ_ENABLE_WATCHDOG,"no") )
				{
					system("dwatchdog");
				}
				else
				{
					system("ewatchdog");
				}*/
				break;
			//not recognized cmd
			default:
				print("unkown cmd 0x%x", field);
				util_get_common_data_from_frame(p_null,field_length);
				break;
		}

		//avoid error
		if( current_len-current_len_before < field_length )
		{
			current_len=current_len_before+field_length;
			p_ethernet_frame=p_ethernet_frame_before+field_length;
		}

		//always begin with even byte
		if( current_len&0x01 )
		{
			p_ethernet_frame++;
			current_len++;
		}

	}
	return 0;
}


//synchronize the config info
int util_sync_config_info(void)
{
	int config_have_changed=FALSE;
	bool network_config_changed=false;
	bool wifi_config_changed=false;
	int ret = -1;
	ret = config_init(APP_NAME);//初始化uci上下文
	if(ret < 0)
	{
		print("error:config_init failed! ret=%d",ret);
	}

/*
	config_init(APP_NAME);//初始化uci上下文
	int _ret = -1;
	_ret = config_set_string("main", "AP_SEC_DEBUG", "true");
	print("after config_set_string() ret=%d",_ret);
	config_commit();

		config_deinit();
	*/

	if( strcmp( server_wifi_info_backup.AP_SSID,server_wifi_info.AP_SSID ) )
	{

		print("set AP_SSID to %s..",server_wifi_info.AP_SSID);
		ret = config_set_string("main", "AP_SSID", server_wifi_info.AP_SSID);
		if(ret < 0)
		{
			print("error:config_set_string AP_SSID failed! ret=%d",ret);
		}
		//默认只有一个ssid
		setWirelessCfgValue("ssid",server_wifi_info.AP_SSID);
		//shell_recv(NULL,0,"uci set wireless.@wifi-iface[0].ssid=\"%s\"",server_wifi_info.AP_SSID);
		wifi_config_changed=true;
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.AP_SECFILE,server_wifi_info.AP_SECFILE ) )
	{
		print("set AP_SECFILE to %s..",server_wifi_info.AP_SECFILE);
		ret = config_set_string("main", "AP_SECFILE", server_wifi_info.AP_SECFILE);
		if(ret < 0)
		{
			print("error:config_set_string AP_SECFILE failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}


	//----------------------------------加密方式------------------------------------>>
	bool encrypt_changed = false; 
	if( strcmp( server_wifi_info_backup.AP_CYPHER,server_wifi_info.AP_CYPHER ) )
	{
		print("set AP_CYPHER to %s..",server_wifi_info.AP_CYPHER);
		ret = config_set_string("main", "AP_CYPHER", server_wifi_info.AP_CYPHER);
		if(ret < 0)
		{
			print("error:config_set_string AP_CYPHER failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
		encrypt_changed = true;
	}

	if( strcmp( server_wifi_info_backup.AP_WPA,server_wifi_info.AP_WPA ) )
	{
		print("set AP_WPA to %s..",server_wifi_info.AP_WPA);
		ret = config_set_string("main", "AP_WPA", server_wifi_info.AP_WPA);
		if(ret < 0)
		{
			print("error:config_set_string AP_WPA failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
		encrypt_changed = true;
	}

	if( strcmp( server_wifi_info_backup.AP_SECMODE,server_wifi_info.AP_SECMODE ) )
	{
		print("set AP_SECMODE to %s..",server_wifi_info.AP_SECMODE);
		ret = config_set_string("main", "AP_SECMODE", server_wifi_info.AP_SECMODE);
		if(ret < 0)
		{
			print("error:config_set_string AP_SECMODE failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
		encrypt_changed = true;
	}
	if( strcmp( server_wifi_info_backup.PSK_KEY,server_wifi_info.PSK_KEY ) )
	{
		print("set PSK_KEY to %s..",server_wifi_info.PSK_KEY);
		ret = config_set_string("main", "PSK_KEY", server_wifi_info.PSK_KEY);
		if(ret < 0)
		{
			print("error:config_set_string PSK_KEY failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
		encrypt_changed = true;
	}
	if(encrypt_changed)
	{
		wifi_config_changed = true;
		if(!strcmp(server_wifi_info.AP_SECMODE, "None"))//none
		{
			print("%s","encrypt mode == none");
			shell_recv(NULL,0,"%s","uci set wireless.@wifi-iface[0].encryption=none");
			shell_recv(NULL,0,"%s","uci delete wireless.@wifi-iface[0].key");
		}
		else if(!strcmp(server_wifi_info.AP_CYPHER, "TKIP") 
			&& !strcmp(server_wifi_info.AP_SECMODE, "WPA") )
		{
			if(!strcmp(server_wifi_info.AP_WPA, "1"))//WPA+PSK+TKIP
			{
				print("%s","encrypt mode == WPA+PSK+TKIP");
				//默认只有一个ssid
				shell_recv(NULL,0,"%s","uci set wireless.@wifi-iface[0].encryption=psk+tkip");
				setWirelessCfgValue("key",server_wifi_info.PSK_KEY);
				//shell_recv(NULL,0,"uci set wireless.@wifi-iface[0].key=\"%s\"", server_wifi_info.PSK_KEY);
			}
			else if(!strcmp(server_wifi_info.AP_WPA, "3"))//WPA1+WPA2+PSK+TKIP
			{
				print("%s","encrypt mode == WPA1+WPA2+PSK+TKIP");
				shell_recv(NULL,0,"%s","uci set wireless.@wifi-iface[0].encryption=psk+psk2+tkip");
				setWirelessCfgValue("key",server_wifi_info.PSK_KEY);
				//shell_recv(NULL,0,"uci set wireless.@wifi-iface[0].key=\"%s\"", server_wifi_info.PSK_KEY);
			}

		}
		else if(!strcmp(server_wifi_info.AP_CYPHER, "CCMP") 
			&& !strcmp(server_wifi_info.AP_SECMODE, "WPA"))
		{
			if(!strcmp(server_wifi_info.AP_WPA, "2"))//WPA2+PSK+AES
			{
				print("%s","encrypt mode == WPA2+PSK+AES");
				shell_recv(NULL,0,"%s","uci set wireless.@wifi-iface[0].encryption=psk2+ccmp");
				setWirelessCfgValue("key",server_wifi_info.PSK_KEY);
				//shell_recv(NULL,0,"uci set wireless.@wifi-iface[0].key=\"%s\"", server_wifi_info.PSK_KEY);
			}
			else if(!strcmp(server_wifi_info.AP_WPA, "3"))//WPA1+WPA2+PSK+AES
			{
				print("%s","encrypt mode == WPA1+WPA2+PSK+AES");
				shell_recv(NULL,0,"%s","uci set wireless.@wifi-iface[0].encryption=psk+psk2+tkip+ccmp");
				setWirelessCfgValue("key",server_wifi_info.PSK_KEY);
				//shell_recv(NULL,0,"uci set wireless.@wifi-iface[0].key=\"%s\"", server_wifi_info.PSK_KEY);
			}
		}
	}
	//<<-------------------------------------------------------------------------//



	if( strcmp( server_wifi_info_backup.WEP_RADIO_NUM0_KEY_1,server_wifi_info.WEP_RADIO_NUM0_KEY_1 ) )
	{
		print("set WEP_RADIO_NUM0_KEY_1 to %s..",server_wifi_info.WEP_RADIO_NUM0_KEY_1);
		ret = config_set_string("main", "WEP_RADIO_NUM0_KEY_1", server_wifi_info.WEP_RADIO_NUM0_KEY_1);
		if(ret < 0)
		{
			print("error:config_set_string WEP_RADIO_NUM0_KEY_1 failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.WEP_RADIO_NUM0_KEY_2,server_wifi_info.WEP_RADIO_NUM0_KEY_2 ) )
	{
		print("set WEP_RADIO_NUM0_KEY_2 to %s..",server_wifi_info.WEP_RADIO_NUM0_KEY_2);
		ret = config_set_string("main", "WEP_RADIO_NUM0_KEY_2", server_wifi_info.WEP_RADIO_NUM0_KEY_2);
		if(ret < 0)
		{
			print("error:config_set_string WEP_RADIO_NUM0_KEY_2 failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.WEP_RADIO_NUM0_KEY_3,server_wifi_info.WEP_RADIO_NUM0_KEY_3 ) )
	{
		print("set WEP_RADIO_NUM0_KEY_3 to %s..",server_wifi_info.WEP_RADIO_NUM0_KEY_3);
		ret = config_set_string("main", "WEP_RADIO_NUM0_KEY_3", server_wifi_info.WEP_RADIO_NUM0_KEY_3);
		if(ret < 0)
		{
			print("error:config_set_string WEP_RADIO_NUM0_KEY_3 failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.WEP_RADIO_NUM0_KEY_4,server_wifi_info.WEP_RADIO_NUM0_KEY_4 ) )
	{
		print("set WEP_RADIO_NUM0_KEY_4 to %s..",server_wifi_info.WEP_RADIO_NUM0_KEY_4);
		ret = config_set_string("main", "WEP_RADIO_NUM0_KEY_4", server_wifi_info.WEP_RADIO_NUM0_KEY_4);
		if(ret < 0)
		{
			print("error:config_set_string WEP_RADIO_NUM0_KEY_4 failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.TZ_ENABLE_WIFI,server_wifi_info.TZ_ENABLE_WIFI ) )
	{
		print("set TZ_ENABLE_WIFI to %s..",server_wifi_info.TZ_ENABLE_WIFI);
		ret = config_set_string("main", "TZ_ENABLE_WIFI", server_wifi_info.TZ_ENABLE_WIFI);
		if(ret < 0)
		{
			print("error:config_set_string TZ_ENABLE_WIFI failed! ret=%d",ret);
		}
		int disabled = strcmp(server_wifi_info.TZ_ENABLE_WIFI,"yes")?1:0;
		print("set wireless.%s.disabled=%d..", NAME_OF_WIRELESS_INTERFACE, disabled);
		shell_recv(NULL,0,"uci set wireless.%s.disabled=\"%d\"", NAME_OF_WIRELESS_INTERFACE, disabled);
		wifi_config_changed=true;
		config_have_changed=TRUE;
	}


	if( strcmp( server_wifi_info_backup.AP_WEP_MODE_0,server_wifi_info.AP_WEP_MODE_0 ) )
	{
		print("set AP_WEP_MODE_0 to %s..",server_wifi_info.AP_WEP_MODE_0);
		ret = config_set_string("main", "AP_WEP_MODE_0", server_wifi_info.AP_WEP_MODE_0);
		if(ret < 0)
		{
			print("error:config_set_string AP_WEP_MODE_0 failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.AP_PRIMARY_KEY_0,server_wifi_info.AP_PRIMARY_KEY_0 ) )
	{
		print("set AP_PRIMARY_KEY_0 to %s..",server_wifi_info.AP_PRIMARY_KEY_0);
		ret = config_set_string("main", "AP_PRIMARY_KEY_0", server_wifi_info.AP_PRIMARY_KEY_0);
		if(ret < 0)
		{
			print("error:config_set_string AP_PRIMARY_KEY_0 failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.PUREG,server_wifi_info.PUREG ) )
	{
		print("set PUREG to %s..",server_wifi_info.PUREG);
		ret = config_set_string("main", "PUREG", server_wifi_info.PUREG);
		if(ret < 0)
		{
			print("error:config_set_string PUREG failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.RATECTL,server_wifi_info.RATECTL ) )
	{
		print("set RATECTL to %s..",server_wifi_info.RATECTL);
		ret = config_set_string("main", "RATECTL", server_wifi_info.RATECTL);
		if(ret < 0)
		{
			print("error:config_set_string RATECTL failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.MANRATE,server_wifi_info.MANRATE ) )
	{
		print("set MANRATE to %s..",server_wifi_info.MANRATE);
		ret = config_set_string("main", "MANRATE", server_wifi_info.MANRATE);
		if(ret < 0)
		{
			print("error:config_set_string MANRATE failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.MANRETRIES,server_wifi_info.MANRETRIES ) )
	{
		print("set MANRETRIES to %s..",server_wifi_info.MANRETRIES);
		ret = config_set_string("main", "MANRETRIES", server_wifi_info.MANRETRIES);
		if(ret < 0)
		{
			print("error:config_set_string MANRETRIES failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}


	if( strcmp( server_wifi_info_backup.TZ_ISOLATE_WLAN_CLIENTS,server_wifi_info.TZ_ISOLATE_WLAN_CLIENTS ) )
	{
		print("set TZ_ISOLATE_WLAN_CLIENTS to %s..",server_wifi_info.TZ_ISOLATE_WLAN_CLIENTS);
		ret = config_set_string("main", "TZ_ISOLATE_WLAN_CLIENTS", server_wifi_info.TZ_ISOLATE_WLAN_CLIENTS);
		if(ret < 0)
		{
			print("error:config_set_string TZ_ISOLATE_WLAN_CLIENTS failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}


	if( strcmp( server_wifi_info_backup.PUREN,server_wifi_info.PUREN ) )
	{
		print("set PUREN to %s..",server_wifi_info.PUREN);
		ret = config_set_string("main", "PUREN", server_wifi_info.PUREN);
		if(ret < 0)
		{
			print("error:config_set_string PUREN failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.AP_WPA_GROUP_REKEY,server_wifi_info.AP_WPA_GROUP_REKEY ) )
	{
		print("set AP_WPA_GROUP_REKEY to %s..",server_wifi_info.AP_WPA_GROUP_REKEY);
		ret = config_set_string("main", "AP_WPA_GROUP_REKEY", server_wifi_info.AP_WPA_GROUP_REKEY);
		if(ret < 0)
		{
			print("error:config_set_string AP_WPA_GROUP_REKEY failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.AP_WPA_GMK_REKEY,server_wifi_info.AP_WPA_GMK_REKEY ) )
	{
		print("set AP_WPA_GMK_REKEY to %s..",server_wifi_info.AP_WPA_GMK_REKEY);
		ret = config_set_string("main", "AP_WPA_GMK_REKEY", server_wifi_info.AP_WPA_GMK_REKEY);
		if(ret < 0)
		{
			print("error:config_set_string AP_WPA_GMK_REKEY failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.AP_HIDESSID,server_wifi_info.AP_HIDESSID ) )
	{
		print("set AP_HIDESSID to %s..",server_wifi_info.AP_HIDESSID);
		ret = config_set_string("main", "AP_HIDESSID", server_wifi_info.AP_HIDESSID);
		if(ret < 0)
		{
			print("error:config_set_string AP_HIDESSID failed! ret=%d",ret);
		}
		shell_recv(NULL,0,"uci set wireless.@wifi-iface[0].hidden=\"%s\"", server_wifi_info.AP_HIDESSID);
		wifi_config_changed = true;
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.AP_IPADDR,server_wifi_info.AP_IPADDR ) )
	{
		print("set AP_IPADDR to %s..",server_wifi_info.AP_IPADDR);
		ret = config_set_string("main", "AP_IPADDR", server_wifi_info.AP_IPADDR);
		if(ret < 0)
		{
			print("error:config_set_string AP_IPADDR failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
		//设置当前设备的IP地址
		print("set the %s ip to %s   (ipv4)", network_dev_name, server_wifi_info.AP_IPADDR);
		shell_recv(NULL,0,"uci set network.lan.ipaddr=\"%s\"",server_wifi_info.AP_IPADDR);
		network_config_changed = true;
		//被分配的IP地址和服务器的IP地址不同,  ------>没关系
		/*if( server_wifi_info.is_ip_not_the_same_with_server )
		{
			//设置当前设备的IP地址
			print("set the %s ip to %s   (ipv4)", network_dev_name, server_wifi_info.AP_IPADDR);
			shell_recv(NULL,0,"uci set network.lan.ipaddr=%s",server_wifi_info.AP_IPADDR);
			network_config_changed = true;
			//util_config_ipv4_addr( network_dev_name,server_wifi_info.AP_IPADDR );
		}
		else
		{
			//清除配置的IP地址
			print("ip %s  is same to server, set %s ip to 0.0.0.0", server_wifi_info.AP_IPADDR, network_dev_name);
			shell_recv(NULL,0,"uci set network.lan.ipaddr=%s","0.0.0.0");
			//util_config_ipv4_addr( network_dev_name,"0.0.0.0" );
		}*/
	}

	if( strcmp( server_wifi_info_backup.AP_NETMASK,server_wifi_info.AP_NETMASK ) )
	{
		print("set AP_NETMASK to %s..",server_wifi_info.AP_NETMASK);
		ret = config_set_string("main", "AP_NETMASK", server_wifi_info.AP_NETMASK);
		if(ret < 0)
		{
			print("error:config_set_string AP_NETMASK failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.TZ_DHCP_IP_BEGIN,server_wifi_info.TZ_DHCP_IP_BEGIN ) )
	{
		print("set TZ_DHCP_IP_BEGIN to %s..",server_wifi_info.TZ_DHCP_IP_BEGIN);
		ret = config_set_string("main", "TZ_DHCP_IP_BEGIN", server_wifi_info.TZ_DHCP_IP_BEGIN);
		if(ret < 0)
		{
			print("error:config_set_string TZ_DHCP_IP_BEGIN failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.TZ_DHCP_IP_END,server_wifi_info.TZ_DHCP_IP_END ) )
	{
		print("set TZ_DHCP_IP_END to %s..",server_wifi_info.TZ_DHCP_IP_END);
		ret = config_set_string("main", "TZ_DHCP_IP_END", server_wifi_info.TZ_DHCP_IP_END);
		if(ret < 0)
		{
			print("error:config_set_string TZ_DHCP_IP_END failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.TZ_DHCP_EXPIRE_TIME,server_wifi_info.TZ_DHCP_EXPIRE_TIME ) )
	{
		print("set TZ_DHCP_EXPIRE_TIME to %s..",server_wifi_info.TZ_DHCP_EXPIRE_TIME);
		ret = config_set_string("main", "TZ_DHCP_EXPIRE_TIME", server_wifi_info.TZ_DHCP_EXPIRE_TIME);
		if(ret < 0)
		{
			print("error:config_set_string TZ_DHCP_EXPIRE_TIME failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.AP_PRIMARY_CH,server_wifi_info.AP_PRIMARY_CH ) )
	{
		print("set AP_PRIMARY_CH to %s..",server_wifi_info.AP_PRIMARY_CH);
		ret = config_set_string("main", "AP_PRIMARY_CH", server_wifi_info.AP_PRIMARY_CH);
		if(ret < 0)
		{
			print("error:config_set_string AP_PRIMARY_CH failed! ret=%d",ret);
		}
		shell_recv(NULL,0,"uci set wireless.%s.channel=\"%s\"", NAME_OF_WIRELESS_INTERFACE, server_wifi_info.AP_PRIMARY_CH);
		config_have_changed=TRUE;
		wifi_config_changed = true;
	}
	bool wifi_mode_changed = false;

	if( strcmp( server_wifi_info_backup.WIFIWORKMODE,server_wifi_info.WIFIWORKMODE ) )
	{
		print("set WIFIWORKMODE to %s..",server_wifi_info.WIFIWORKMODE);
		ret = config_set_string("main", "WIFIWORKMODE", server_wifi_info.WIFIWORKMODE);
		if(ret < 0)
		{
			print("error:config_set_string WIFIWORKMODE failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
		wifi_mode_changed=true;
	}

	if( strcmp( server_wifi_info_backup.AP_CHMODE,server_wifi_info.AP_CHMODE ) )
	{
		print("set AP_CHMODE to %s..",server_wifi_info.AP_CHMODE);
		ret = config_set_string("main", "AP_CHMODE", server_wifi_info.AP_CHMODE);
		if(ret < 0)
		{
			print("error:config_set_string AP_CHMODE failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
		wifi_mode_changed = true;
	}
	if(wifi_mode_changed == true)//这里字符串的对应关系是通过网页下发指令，w13侧抓包得到，而wifi_mode的值为对应uci配置的值，没有为什么!!!
	{
		int wifi_mode = 0;
		if(!strcmp(server_wifi_info.WIFIWORKMODE,"m11ng") && !strcmp(server_wifi_info.AP_CHMODE,"11NGHT20") )
		{
			wifi_mode=9;//bgn
		}
		else if(!strcmp(server_wifi_info.WIFIWORKMODE,"m11b") && !strcmp(server_wifi_info.AP_CHMODE,"11B"))
		{
			wifi_mode=1;//b
		}
		else if(!strcmp(server_wifi_info.WIFIWORKMODE,"m11g") && !strcmp(server_wifi_info.AP_CHMODE,"11G"))
		{
			wifi_mode=4;//g
		}
		else if(!strcmp(server_wifi_info.WIFIWORKMODE,"m11bg") && !strcmp(server_wifi_info.AP_CHMODE,"11G"))
		{
			wifi_mode=0;//bg
		}
		else if(!strcmp(server_wifi_info.WIFIWORKMODE,"m11n") && !strcmp(server_wifi_info.AP_CHMODE,"11NGHT20"))
		{
			wifi_mode=6;//n
		}
		else
		{
			wifi_mode=6;//other default n
		}
		shell_recv(NULL,0,"uci set wireless.%s.mode=\"%d\"", NAME_OF_WIRELESS_INTERFACE, wifi_mode);
		wifi_config_changed = true;
	}



	if( strcmp( server_wifi_info_backup.TXPOWER,server_wifi_info.TXPOWER ) )
	{
		int txpower=atoi(server_wifi_info.TXPOWER);
		txpower= txpower*100/TZ_WIFI_MAX_TXPOWER;
		//取整数 10 - 100%
		txpower -=(txpower%10);
		if(txpower < 0)
		{
			print("error txpower=%d",txpower);
			txpower=100;
		}
		else if(txpower == 0)
		{
			txpower=1;//支持1%
		}
		else if(txpower > 100)//最大100%
		{
			txpower = 100;
		}

		print("set TXPOWER to %d%%.. dbm=%s", txpower, server_wifi_info.TXPOWER);
		ret = config_set_string("main", "TXPOWER", server_wifi_info.TXPOWER);
		if(ret < 0)
		{
			print("error:config_set_string TXPOWER failed! ret=%d",ret);
		}
		shell_recv(NULL,0,"uci set wireless.%s.txpower=\"%d\"", NAME_OF_WIRELESS_INTERFACE, txpower);
		config_have_changed=TRUE;
		wifi_config_changed = true;
	}

	if( strcmp( server_wifi_info_backup.TZ_ENABLE_WATCHDOG,server_wifi_info.TZ_ENABLE_WATCHDOG ) )
	{
		print("set TZ_ENABLE_WATCHDOG to %s..",server_wifi_info.TZ_ENABLE_WATCHDOG);
		ret = config_set_string("main", "TZ_ENABLE_WATCHDOG", server_wifi_info.TZ_ENABLE_WATCHDOG);
		if(ret < 0)
		{
			print("error:config_set_string TZ_ENABLE_WATCHDOG failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}

	if( strcmp( server_wifi_info_backup.AP_GATEWAY,server_wifi_info.AP_GATEWAY ) )
	{
		print("set AP_GATEWAY to %s..",server_wifi_info.AP_GATEWAY);
		ret = config_set_string("main", "AP_GATEWAY", server_wifi_info.AP_GATEWAY);
		if(ret < 0)
		{
			print("error:config_set_string AP_GATEWAY failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
	}
	if( strcmp( server_wifi_info_backup.TZ_WIFI_40M_ENABLE,server_wifi_info.TZ_WIFI_40M_ENABLE ) )
	{
		print("set TZ_WIFI_40M_ENABLE to %s..",server_wifi_info.TZ_WIFI_40M_ENABLE);
		ret = config_set_string("main", "TZ_WIFI_40M_ENABLE", server_wifi_info.TZ_WIFI_40M_ENABLE);
		if(ret < 0)
		{
			print("error:config_set_string TZ_WIFI_40M_ENABLE failed! ret=%d",ret);
		}
		config_have_changed=TRUE;
		if(!strcmp(server_wifi_info.TZ_WIFI_40M_ENABLE, "2"))//40MHz
		{
			print("-->set wireless.%s.ht=%s", NAME_OF_WIRELESS_INTERFACE, "40");
			shell_recv(NULL,0,"uci set wireless.%s.ht=\"%s\"", NAME_OF_WIRELESS_INTERFACE, "40");
		}
		else if(!strcmp(server_wifi_info.TZ_WIFI_40M_ENABLE, "0"))
		{
			print("-->set wireless.%s.ht=%s", NAME_OF_WIRELESS_INTERFACE, "20");
			shell_recv(NULL,0,"uci set wireless.%s.ht=\"%s\"", NAME_OF_WIRELESS_INTERFACE, "20");
		}
		wifi_config_changed = true;
	}
	//commit the changes
	if( config_have_changed )
	{
		print("%s","------------------------------commit the wifi_info config!!----------------------------------------");
		config_commit();//提交修改
		shell_recv(NULL,0,"%s","uci commit");
		sleep(1);
		if(network_config_changed == true)//include wifi config
		{
			print("%s","-----------------network config has benn changed!-------------------");
			shell_recv(NULL,0,"%s","/etc/init.d/network restart");
		}
		else if(wifi_config_changed == true)
		{
			print("%s","-----------------wifi config has benn changed!-------------------");
			shell_recv(NULL,0,"%s","wifi restart");
		}
	}
	config_deinit();//释放uci上下文
	return config_have_changed;
}



//配置IP地址
int util_config_ipv4_addr(const char *net_dev, const char* ipaddr)
{
	struct ifreq ifr;
	int fd = 0;
	struct sockaddr_in *pAddr;

	if( (NULL == net_dev) || (NULL == ipaddr) )
	{
		print("%s","illegal call function SetGeneralIP!");
		return -1;
	}

	if ( ( fd = socket(AF_INET,SOCK_DGRAM,0) ) < 0 )
	{
		print("%s","socket....setip..false!!!");
		return -1;
	}

	strcpy(ifr.ifr_name, net_dev);

	pAddr = (struct sockaddr_in *)&(ifr.ifr_addr);
	bzero(pAddr, sizeof(struct sockaddr_in));
	pAddr->sin_addr.s_addr = inet_addr(ipaddr);
	pAddr->sin_family = AF_INET;
	int err_no = 0;
	err_no = ioctl(fd, SIOCSIFADDR, &ifr);
	if (err_no < 0)
	{
		close(fd);
		print("ioctl:%s ----> set %s ip to %s  false!!", strerror(err_no) ,net_dev, ipaddr);
		return -1;
	}
	close(fd);
	return 0;
}




int util_client_send_reboot_sync_frame(void)
{
	//resend search server message
	int real_frame_size=0;
	char * p_null=NULL;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	util_add_field_to_frame(CMD_TYPE_REBOOT_SYNC,p_null,0);

	pthread_rwlock_wrlock(&send_sock_rwlock);

	//send ethernet frame
	util_send_ethernet_frame(
								g_send_sock_fd
								,PROTOCOL_TYPE_ECTSP
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);

	pthread_rwlock_unlock(&send_sock_rwlock);
	return 0;
}

int util_client_send_reboot_sync_ack_frame(void)
{
	//resend search server message
	int real_frame_size=0;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	char * p_null=NULL;
	util_add_field_to_frame(CMD_TYPE_REBOOT_SYNC_ACK,p_null,0);

	//send ethernet frame
	pthread_rwlock_wrlock(&send_sock_rwlock);
	util_send_ethernet_frame(
								g_send_sock_fd
								,PROTOCOL_TYPE_ECTSP
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);
	pthread_rwlock_unlock(&send_sock_rwlock);
	return 0;
}

int util_client_send_reboot_ack_frame(void)
{
	//resend search server message
	int real_frame_size=0;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	char * p_null=NULL;
	util_add_field_to_frame(CMD_TYPE_REBOOT_ACK,p_null,0);

	//send ethernet frame
	pthread_rwlock_wrlock(&send_sock_rwlock);
	util_send_ethernet_frame(
								g_send_sock_fd
								,PROTOCOL_TYPE_ECTSP
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);
	pthread_rwlock_unlock(&send_sock_rwlock);
	return 0;

}

int util_client_send_restore_sync_frame(void)
{
	//resend search server message
	int real_frame_size=0;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	char * p_null=NULL;
	util_add_field_to_frame(CMD_TYPE_RESTORE_SYNC,p_null,0);

	//send ethernet frame
	pthread_rwlock_wrlock(&send_sock_rwlock);
	util_send_ethernet_frame(
								g_send_sock_fd
								,PROTOCOL_TYPE_ECTSP
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);
	pthread_rwlock_unlock(&send_sock_rwlock);
	return 0;
}

int util_client_send_restore_sync_ack_frame(void)
{
	//resend search server message
	int real_frame_size=0;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	char * p_null=NULL;
	util_add_field_to_frame(CMD_TYPE_RESTORE_SYNC_ACK,p_null,0);

	//send ethernet frame
	pthread_rwlock_wrlock(&send_sock_rwlock);
	util_send_ethernet_frame(
								g_send_sock_fd
								,PROTOCOL_TYPE_ECTSP
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);
	pthread_rwlock_unlock(&send_sock_rwlock);
	return 0;
}

int util_client_send_restore_ack_frame(void)
{
	//resend search server message
	int real_frame_size=0;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	char * p_null=NULL;
	util_add_field_to_frame(CMD_TYPE_RESTORE_ACK,p_null,0);

	//send ethernet frame
	pthread_rwlock_wrlock(&send_sock_rwlock);
	util_send_ethernet_frame(
								g_send_sock_fd
								,PROTOCOL_TYPE_ECTSP
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);
	pthread_rwlock_unlock(&send_sock_rwlock);
	return 0;
}

int util_client_send_update_sync_ack_frame(void)
{
	//resend search server message
	int real_frame_size=0;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	char * p_null=NULL;
	util_add_field_to_frame(CMD_TYPE_UPDATE_SYNC_ACK,p_null,0);

	//send ethernet frame
	pthread_rwlock_wrlock(&send_sock_rwlock);
	util_send_ethernet_frame(
								g_send_sock_fd
								,PROTOCOL_TYPE_ECTSP
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);
	pthread_rwlock_unlock(&send_sock_rwlock);
	return 0;
}

//restore default configs,no rfeboot reply
int util_client_send_only_restore_reply_frame(void)
{
	//resend search server message
	int real_frame_size=0;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	char * p_null=NULL;
	util_add_field_to_frame(CMD_TYPE_ONLY_RESTORE_CONFIGS_REPLY,p_null,0);

	//send ethernet frame
	pthread_rwlock_wrlock(&send_sock_rwlock);
	util_send_ethernet_frame(
								g_send_sock_fd
								,PROTOCOL_TYPE_ECTSP
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);
	pthread_rwlock_unlock(&send_sock_rwlock);
	return 0;
}

//client acknowledge server's info
int util_send_ack_frame(int socket_handle,unsigned short ethernet_frame_type,unsigned char ack_cmd,unsigned char* self_mac_addr,unsigned char* remote_mac_addr)
{
	//resend search server message
	int real_frame_size=0;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	unsigned char* p_ack_cmd=&ack_cmd;
	//add search server cmd
	util_add_field_to_frame(CMD_TYPE_ACK_CMD,p_ack_cmd,1);

	//send ethernet frame
	pthread_rwlock_wrlock(&send_sock_rwlock);
	util_send_ethernet_frame(   g_send_sock_fd
								,ethernet_frame_type
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);
	pthread_rwlock_unlock(&send_sock_rwlock);

	return 0;
}


int util_send_search_server_frame(void)
{
	//resend search server message
	int real_frame_size=0;
	int saved_data_len;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;

	//add search server cmd
	char * p_null=NULL;
	util_add_field_to_frame(CMD_TYPE_SEARCH_SERVER,p_null,0);

	saved_data_len=real_frame_size;

	//发送字节的wifi配置信息
	//export TZ_ENABLE_WIFI=yes
	if( strlen( server_wifi_info.TZ_ENABLE_WIFI ) )
	{
		util_add_field_to_frame(CMD_INFO_TZ_ENABLE_WIFI,server_wifi_info.TZ_ENABLE_WIFI,strlen(server_wifi_info.TZ_ENABLE_WIFI)+1);
	}
	//export AP_SSID="tozed-11274F"
	if( strlen( server_wifi_info.AP_SSID ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_SSID,server_wifi_info.AP_SSID,strlen(server_wifi_info.AP_SSID)+1);
	}
	if( strlen( server_wifi_info.AP_HIDESSID ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_HIDESSID,server_wifi_info.AP_HIDESSID,strlen(server_wifi_info.AP_HIDESSID)+1);
	}
	if( strlen( server_wifi_info.AP_PRIMARY_CH ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_PRIMARY_CH,server_wifi_info.AP_PRIMARY_CH,strlen(server_wifi_info.AP_PRIMARY_CH)+1);
	}
	if( strlen( server_wifi_info.TZ_WIFI_40M_ENABLE ) )
	{
		util_add_field_to_frame(CMD_INFO_TZ_WIFI_40M_ENABLE,server_wifi_info.TZ_WIFI_40M_ENABLE,strlen(server_wifi_info.TZ_WIFI_40M_ENABLE)+1);
	}
	if( strlen( server_wifi_info.TZ_IDU_FIRMWARE_VERSION_INFO ) )
	{
		util_add_field_to_frame(CMD_INFO_TZ_IDU_FIRMWARE_VERSION,server_wifi_info.TZ_IDU_FIRMWARE_VERSION_INFO,strlen(server_wifi_info.TZ_IDU_FIRMWARE_VERSION_INFO)+1);
	}
	if( strlen( server_wifi_info.TZ_IDU_CONFIG_VERSION_INFO ) )
	{
		util_add_field_to_frame(CMD_INFO_IDU_CONFIG_FILE_VERSION,server_wifi_info.TZ_IDU_CONFIG_VERSION_INFO,strlen(server_wifi_info.TZ_IDU_CONFIG_VERSION_INFO)+1);
	}
	if( strlen( server_wifi_info.TXPOWER ) )
	{
		util_add_field_to_frame(CMD_INFO_TXPOWER,server_wifi_info.TXPOWER,strlen(server_wifi_info.TXPOWER)+1);
	}
	if( strlen( server_wifi_info.AP_SECMODE ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_SECMODE,server_wifi_info.AP_SECMODE,strlen(server_wifi_info.AP_SECMODE)+1);
	}
	if( strlen( server_wifi_info.AP_WPA ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_WPA,server_wifi_info.AP_WPA,strlen(server_wifi_info.AP_WPA)+1);
	}
	if( strlen( server_wifi_info.AP_SECFILE ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_SECFILE,server_wifi_info.AP_SECFILE,strlen(server_wifi_info.AP_SECFILE)+1);
	}
	if( strlen( server_wifi_info.PSK_KEY ) )
	{
		util_add_field_to_frame(CMD_INFO_PSK_KEY,server_wifi_info.PSK_KEY,strlen(server_wifi_info.PSK_KEY)+1);
	}
	if( strlen( server_wifi_info.AP_CYPHER ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_CYPHER,server_wifi_info.AP_CYPHER,strlen(server_wifi_info.AP_CYPHER)+1);
	}
	if( strlen( server_wifi_info.AP_WEP_MODE_0 ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_WEP_MODE_0,server_wifi_info.AP_WEP_MODE_0,strlen(server_wifi_info.AP_WEP_MODE_0)+1);
	}
	if( strlen( server_wifi_info.AP_PRIMARY_KEY_0 ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_PRIMARY_KEY_0,server_wifi_info.AP_PRIMARY_KEY_0,strlen(server_wifi_info.AP_PRIMARY_KEY_0)+1);
	}
	if( strlen( server_wifi_info.WEP_RADIO_NUM0_KEY_1 ) )
	{
		util_add_field_to_frame(CMD_INFO_WEP_RADIO_NUM0_KEY_1,server_wifi_info.WEP_RADIO_NUM0_KEY_1,strlen(server_wifi_info.WEP_RADIO_NUM0_KEY_1)+1);
	}
	if( strlen( server_wifi_info.WEP_RADIO_NUM0_KEY_2 ) )
	{
		util_add_field_to_frame(CMD_INFO_WEP_RADIO_NUM0_KEY_2,server_wifi_info.WEP_RADIO_NUM0_KEY_2,strlen(server_wifi_info.WEP_RADIO_NUM0_KEY_2)+1);
	}
	if( strlen( server_wifi_info.WEP_RADIO_NUM0_KEY_3 ) )
	{
		util_add_field_to_frame(CMD_INFO_WEP_RADIO_NUM0_KEY_3,server_wifi_info.WEP_RADIO_NUM0_KEY_3,strlen(server_wifi_info.WEP_RADIO_NUM0_KEY_3)+1);
	}
	if( strlen( server_wifi_info.WEP_RADIO_NUM0_KEY_4 ) )
	{
		util_add_field_to_frame(CMD_INFO_WEP_RADIO_NUM0_KEY_4,server_wifi_info.WEP_RADIO_NUM0_KEY_4,strlen(server_wifi_info.WEP_RADIO_NUM0_KEY_4)+1);
	}
	//上发IP地址信息
	if( strlen( server_wifi_info.AP_IPADDR ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_IPADDR,server_wifi_info.AP_IPADDR,strlen(server_wifi_info.AP_IPADDR)+1);
	}
	if( strlen( server_wifi_info.AP_CHMODE ) )
	{
		util_add_field_to_frame(CMD_INFO_AP_CHMODE,server_wifi_info.AP_CHMODE,strlen(server_wifi_info.AP_CHMODE)+1);
	}
	if( strlen( server_wifi_info.PUREG ) )
	{
		util_add_field_to_frame(CMD_INFO_PUREG,server_wifi_info.PUREG,strlen(server_wifi_info.PUREG)+1);
	}
	if( strlen( server_wifi_info.RATECTL ) )
	{
		util_add_field_to_frame(CMD_INFO_RATECTL,server_wifi_info.RATECTL,strlen(server_wifi_info.RATECTL)+1);
	}
	if( strlen( server_wifi_info.MANRATE ) )
	{
		util_add_field_to_frame(CMD_INFO_MANRATE,server_wifi_info.MANRATE,strlen(server_wifi_info.MANRATE)+1);
	}
	if( strlen( server_wifi_info.MANRETRIES ) )
	{
		util_add_field_to_frame(CMD_INFO_MANRETRIES,server_wifi_info.MANRETRIES,strlen(server_wifi_info.MANRETRIES)+1);
	}
	if( strlen( server_wifi_info.WIFIWORKMODE ) )
	{
		util_add_field_to_frame(CMD_INFO_WIFIWORKMODE,server_wifi_info.WIFIWORKMODE,strlen(server_wifi_info.WIFIWORKMODE)+1);
	}
	if( strlen( server_wifi_info.TZ_ISOLATE_WLAN_CLIENTS ) )
	{
		util_add_field_to_frame(CMD_INFO_TZ_ISOLATE_WLAN_CLIENTS,server_wifi_info.TZ_ISOLATE_WLAN_CLIENTS,strlen(server_wifi_info.TZ_ISOLATE_WLAN_CLIENTS)+1);
	}


	if( strlen( server_wifi_info.PUREN ) )
	{
		util_add_field_to_frame(CMD_INFO_PUREN,server_wifi_info.PUREN,strlen(server_wifi_info.PUREN)+1);
	}

	//set the data length
	ethernet_frame_buffer[ LENGTH_OF_FIELD ]=real_frame_size-saved_data_len;
	if(real_frame_size-saved_data_len > 255)
	{
		ethernet_frame_buffer[ LENGTH_OF_FIELD ]=254;
	}

	//send ethernet frame
	pthread_rwlock_wrlock(&send_sock_rwlock);
	util_send_ethernet_frame(
								g_send_sock_fd
								,PROTOCOL_TYPE_ECTSP
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);
	pthread_rwlock_unlock(&send_sock_rwlock);
	return 0;

}

void process_1s_signal(void)
{
	static int counter=0;

	if( reboot_timer_started || restore_settings_timer_started )
	{
		reboot_timer_counter++;
	}

	if( reboot_timer_counter >= 3 )
	{
		if( reboot_timer_started )
		{
			//reboot the machine
			print("%s","---------------reboot---------------------");
			#ifndef __i386__
			system("reboot");
			#endif
		}
		else if( restore_settings_timer_started )
		{
			print("%s","---------------reboot after restore---------------------");
			//start to reboot
			#ifndef __i386__
			//system("reboot");
			#endif
		}
		else
		{
			reboot_timer_counter=0;
		}
	}
	else if( reboot_timer_counter == 2 )
	{
		if( restore_settings_timer_started )
		{
			print("%s","---------------restore---------------------");
			//system( EXECUTED_SCRIPT_WHEN_RESTORE_DEFAULT_CONFIGS );
		}
	}

	//as a client
	//no server found
	if( !is_connected_to_server )
	{

		util_client_send_probe_server_frame( FALSE );
		frame_sent_counter++;
		print("Send search server packet! frame_sent_counter=%d",frame_sent_counter);
		if( frame_sent_counter >= 0x7fffffff )
		{
			frame_sent_counter=4;
		}
	}
	else
	{
		counter += 1;

		if( first_connected )
		{
			first_connected=FALSE;
			first_disconnected=TRUE;
			//turn off telnet,http,dns,dhcp server
			print("%s","---------------server connected --------------------");

			if(file_exists( TZ_DHCP_SERVER_ALIVE_FLAG_FILE ))//如果开启了dhcp，则要关掉
			{
				shell_recv(NULL,0,"%s stop",SCRIPT_DHCPD);
				shell_recv(NULL,0,"%s stop",SCRIPT_DNSMAQ);//关闭dhco
				shell_recv(NULL,0,"uci set dhcp.lan.ignore=1");
				sleep(1);
				shell_recv(NULL,0,"uci commit");
				shell_recv(NULL,0,"echo 1 > %s", TZ_MGR_RESTART_COUNT_FILE);//重置重连次数
				shell_recv(NULL,0,"rm -f %s",TZ_DHCP_SERVER_ALIVE_FLAG_FILE);//删除dhcp启动标志文件
			}
			//设置当前设备的IP地址
			//util_config_ipv4_addr(network_dev_name,"0.0.0.0");
			//system("/etc/rc.d/rc.uplink.connected");
			shell_recv(NULL,0,"touch  %s",UPLINK_CONNECTION_IS_OK_TMP_FILE);
			//cmd_touch( UPLINK_CONNECTION_IS_OK_TMP_FILE );
		}

		//10s
		if( counter >= 10 )
		{
			counter=0;
			util_client_send_probe_server_frame( TRUE );
			frame_sent_counter++;

			if( frame_sent_counter >= 0x7fffffff )
			{
				frame_sent_counter=4;
			}
		}
	}

	//no response from server
	if( frame_sent_counter >= 3 )
	{
		//indicate no signal
		//cmd_update_signal_ind( 0,FALSE );
		print("-----------turn off signal led!!!----------");
		w13_config_lte_led_according2quality(0);
		w13_config_led_according2dial_status(0);
		//research the server
		client_status=CLIENT_STATUS_SEARCH_SERVER;
		counter=0;
		if( first_disconnected )
		{
			first_disconnected=FALSE;
			first_connected=TRUE;
			//start telnet,http,dns,dhcp server
			print("%s","---------------no response from server--------------------");
			is_connected_to_server=FALSE;
			//设置当前设备的IP地址
			print("set the %s ip to %s   (ipv4)", network_dev_name, server_wifi_info.AP_IPADDR)
			util_config_ipv4_addr(network_dev_name,server_wifi_info.AP_IPADDR);

			//system("/etc/rc.d/rc.uplink.disconnected");
			shell_recv(NULL,0,"rm -f %s",UPLINK_CONNECTION_IS_OK_TMP_FILE);
			//cmd_rm( UPLINK_CONNECTION_IS_OK_TMP_FILE );
			sleep(1);
			//这里最好断开连接则退出程序，由procd重启，避免因为修改网络接口导致socket没更新一直收不到server响应，也可以重建socket，但需要使用条件变量先暂停线程
			raise(SIGQUIT);
		}
	}

	//need to reboot
	if( file_exists( NEED_TO_REBOOT_TEMP_FILE ) )
	{
		reboot_timer_started=TRUE;
		restore_settings_timer_started=FALSE;
		reboot_timer_counter=0;
		print("%s","---------------NEED_TO_REBOOT_TEMP_FILE-------------------");
		shell_recv(NULL,0,"rm -f %s",NEED_TO_REBOOT_TEMP_FILE);
		//cmd_rm( NEED_TO_REBOOT_TEMP_FILE );
		util_client_send_reboot_sync_frame();
	}

	//need to restore settings
	if( file_exists( AFTER_KEY_RESTORE_FLAG_FILE ) )
	{
		reboot_timer_started=FALSE;
		//restore_settings_timer_started=TRUE;
		reboot_timer_counter=0;
		print("%s","-----Send restore cmd!-----");
		util_client_send_restore_sync_frame();
		tz_check_restore_no_respone_count();
	}
}

int util_client_send_probe_server_frame(int have_server_mac)
{
	//resend search server message
	int real_frame_size=0;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	unsigned char *p_null=NULL;
	//add search server cmd
	util_add_field_to_frame(CMD_TYPE_PROBE_SERVER,p_null,0);

	//send broadcast frame
	if( !have_server_mac )
	{
		memset( remote_mac_addr,0xff,sizeof(remote_mac_addr) );
	}

	//send ethernet frame
	pthread_rwlock_wrlock(&send_sock_rwlock);
	util_send_ethernet_frame(
								g_send_sock_fd
								,PROTOCOL_TYPE_ECTSP
								,ethernet_frame_buffer
								,real_frame_size
								,self_mac_addr
								,remote_mac_addr
								);
	pthread_rwlock_unlock(&send_sock_rwlock);
	return 0;

}


int uci_get_config(IN InfoStruct* server_wifi_info)//获取数据库内容到缓冲区
{
	if(server_wifi_info==NULL)
	{
		print("error:%s","server_wifi_info is null!");
		return -1;
	}
	int ret = -1;
	ret = config_init(APP_NAME);//初始化uci上下文
	if(ret < 0)
	{
		print("error:config_init failed! ret=%d",ret);
	}
	const char *tmp = NULL;

	/*-----------------------开始获取配置内容-----------------------*/
	/*使用config_get_string接口时，若参数不存在，则返回第三个入参def*/
	/*如果要使用返回的字符串，请先使用strdup拷贝一份,当uci上下文释放时，返回的字符串内存也会被释放*/

	tmp = config_get_string("main", "AP_IPADDR", "0.0.0.0");
	strncpy(server_wifi_info->AP_IPADDR, tmp,strlen(tmp));
	print("AP_IPADDR=%s",tmp);

	tmp = config_get_string("main", "AP_NETMASK", "255.255.255.0");
	strncpy(server_wifi_info->AP_NETMASK, tmp,strlen(tmp));
	print("AP_NETMASK=%s",tmp);

	tmp = config_get_string("main", "AP_GATEWAY", "192.168.5.1");
	strncpy(server_wifi_info->AP_GATEWAY, tmp,strlen(tmp));
	print("AP_GATEWAY=%s",tmp);

	tmp = config_get_string("main", "TZ_DHCP_IP_BEGIN", "192.168.0.100");
	strncpy(server_wifi_info->TZ_DHCP_IP_BEGIN, tmp,strlen(tmp));
	print("TZ_DHCP_IP_BEGIN=%s",tmp);

	tmp = config_get_string("main", "TZ_DHCP_IP_END", "192.168.0.254");
	strncpy(server_wifi_info->TZ_DHCP_IP_END, tmp,strlen(tmp));
	print("TZ_DHCP_IP_END=%s",tmp);

	tmp = config_get_string("main", "TZ_DHCP_EXPIRE_TIME", "86400");
	strncpy(server_wifi_info->TZ_DHCP_EXPIRE_TIME, tmp,strlen(tmp));
	print("TZ_DHCP_EXPIRE_TIME=%s",tmp);

	tmp = config_get_string("main", "PUREG", "0");
	strncpy(server_wifi_info->PUREG, tmp,strlen(tmp));
	print("PUREG=%s",tmp);

	tmp = config_get_string("main", "PUREN", "0");
	strncpy(server_wifi_info->PUREN, tmp,strlen(tmp));
	print("PUREN=%s",tmp);

	tmp = config_get_string("main", "AP_WPA_GROUP_REKEY", "600");
	strncpy(server_wifi_info->AP_WPA_GROUP_REKEY, tmp,strlen(tmp));
	print("AP_WPA_GROUP_REKEY=%s",tmp);

	tmp = config_get_string("main", "AP_WPA_GMK_REKEY", "86400");
	strncpy(server_wifi_info->AP_WPA_GMK_REKEY, tmp,strlen(tmp));
	print("AP_WPA_GMK_REKEY=%s",tmp);

	tmp = config_get_string("main", "RATECTL", "manual");
	strncpy(server_wifi_info->RATECTL, tmp,strlen(tmp));
	print("RATECTL=%s",tmp);

	tmp = config_get_string("main", "AP_WPA_GMK_REKEY", "86400");
	strncpy(server_wifi_info->AP_WPA_GMK_REKEY, tmp,strlen(tmp));
	print("AP_WPA_GMK_REKEY=%s",tmp);

	tmp = config_get_string("main", "MANRATE", "0x87868584");
	strncpy(server_wifi_info->MANRATE, tmp,strlen(tmp));
	print("MANRATE=%s",tmp);

	tmp = config_get_string("main", "MANRETRIES", "0x04040404");
	strncpy(server_wifi_info->MANRETRIES, tmp,strlen(tmp));
	print("MANRETRIES=%s",tmp);

	tmp = config_get_string("main", "TZ_ENABLE_WATCHDOG", "no");
	strncpy(server_wifi_info->TZ_ENABLE_WATCHDOG, tmp,strlen(tmp));
	print("TZ_ENABLE_WATCHDOG=%s",tmp);

	tmp = config_get_string("main", "TZ_SPEED_PREFERRED", "Not found");
	strncpy(server_wifi_info->TZ_SPEED_PREFERRED, tmp,strlen(tmp));
	print("TZ_SPEED_PREFERRED=%s",tmp);

	tmp = config_get_string("main", "TZ_ENABLE_WIFI", "yes");
	strncpy(server_wifi_info->TZ_ENABLE_WIFI, tmp,strlen(tmp));
	print("TZ_ENABLE_WIFI=%s",tmp);
	/*if(!strcmp(server_wifi_info->TZ_ENABLE_WIFI,"no"))
	{
		shell_recv(NULL,0,"iwpriv %s set RadioOn=0",NAME_OF_WIRELESS_INTERFACE);
	}*/

	tmp = config_get_string("main", "WIFIWORKMODE", "m11b");
	strncpy(server_wifi_info->WIFIWORKMODE, tmp,strlen(tmp));
	print("WIFIWORKMODE=%s",tmp);

	tmp = config_get_string("main", "AP_SSID", "Default-SSID");
	strncpy(server_wifi_info->AP_SSID, tmp,strlen(tmp));
	print("AP_SSID=%s",tmp);
	//shell_recv(NULL,0,"iwpriv %s set SSID=\"%s\"",NAME_OF_WIRELESS_INTERFACE,server_wifi_info->AP_SSID);


	tmp = config_get_string("main", "AP_HIDESSID", "0");
	strncpy(server_wifi_info->AP_HIDESSID, tmp,strlen(tmp));
	print("AP_HIDESSID=%s",tmp);
	//shell_recv(NULL,0,"iwpriv %s set HideSSID=\"%s\"",NAME_OF_WIRELESS_INTERFACE,server_wifi_info->AP_HIDESSID);

	tmp = config_get_string("main", "AP_PRIMARY_CH", "auto");
	strncpy(server_wifi_info->AP_PRIMARY_CH, tmp,strlen(tmp));
	print("AP_PRIMARY_CH=%s",tmp);
	//shell_recv(NULL,0,"iwpriv %s set Channel=\"%s\"",NAME_OF_WIRELESS_INTERFACE,server_wifi_info->AP_PRIMARY_CH);


	tmp = config_get_string("main", "TXPOWER", "18");
	strncpy(server_wifi_info->TXPOWER, tmp,strlen(tmp));
	print("TXPOWER=%s",tmp);

	tmp = config_get_string("main", "AP_SECMODE", "WPA");//OPEN || SHARED || WEPAUTO || WPA1|| WPA2 || WPA1WPA2
	strncpy(server_wifi_info->AP_SECMODE, tmp,strlen(tmp));
	print("AP_SECMODE=%s",tmp);

	tmp = config_get_string("main", "AP_SECFILE", "PSK");// "PSK" || ""
	strncpy(server_wifi_info->AP_SECFILE, tmp,strlen(tmp));
	print("AP_SECFILE=%s",tmp);

	tmp = config_get_string("main", "AP_WPA", "2");
	strncpy(server_wifi_info->AP_WPA, tmp,strlen(tmp));
	print("AP_WPA=%s",tmp);


	tmp = config_get_string("main", "PSK_KEY", "123456789");
	strncpy(server_wifi_info->PSK_KEY, tmp,strlen(tmp));
	print("PSK_KEY=%s",tmp);
	//shell_recv(NULL,0,"iwpriv %s set WPAPSK=\"%s\"", NAME_OF_WIRELESS_INTERFACE, server_wifi_info->PSK_KEY);

	tmp = config_get_string("main", "AP_CYPHER", "CCMP");
	strncpy(server_wifi_info->AP_CYPHER, tmp,strlen(tmp));
	print("AP_CYPHER=%s",tmp);

	tmp = config_get_string("main", "AP_WEP_MODE_0", "2");
	strncpy(server_wifi_info->AP_WEP_MODE_0, tmp,strlen(tmp));
	print("AP_WEP_MODE_0=%s",tmp);

	tmp = config_get_string("main", "AP_PRIMARY_KEY_0", "1");
	strncpy(server_wifi_info->AP_PRIMARY_KEY_0, tmp,strlen(tmp));
	print("AP_PRIMARY_KEY_0=%s",tmp);

	tmp = config_get_string("main", "WEP_RADIO_NUM0_KEY_1", "abcdefghijklm");
	strncpy(server_wifi_info->WEP_RADIO_NUM0_KEY_1, tmp,strlen(tmp));
	print("WEP_RADIO_NUM0_KEY_1=%s",tmp);

	tmp = config_get_string("main", "WEP_RADIO_NUM0_KEY_2", "abcdefghijklm");
	strncpy(server_wifi_info->WEP_RADIO_NUM0_KEY_2, tmp,strlen(tmp));
	print("WEP_RADIO_NUM0_KEY_2=%s",tmp);

	tmp = config_get_string("main", "WEP_RADIO_NUM0_KEY_3", "abcdefghijklm");
	strncpy(server_wifi_info->WEP_RADIO_NUM0_KEY_3, tmp,strlen(tmp));
	print("WEP_RADIO_NUM0_KEY_3=%s",tmp);

	tmp = config_get_string("main", "WEP_RADIO_NUM0_KEY_4", "abcdefghijklm");
	strncpy(server_wifi_info->WEP_RADIO_NUM0_KEY_4, tmp,strlen(tmp));
	print("WEP_RADIO_NUM0_KEY_4=%s",tmp);

	tmp = config_get_string("main", "TZ_ISOLATE_WLAN_CLIENTS", "Not found");
	strncpy(server_wifi_info->TZ_ISOLATE_WLAN_CLIENTS, tmp,strlen(tmp));
	print("TZ_ISOLATE_WLAN_CLIENTS=%s",tmp);
	//if(!strcmp(server_wifi_info->TZ_ISOLATE_WLAN_CLIENTS,"yes"))
		//shell_recv(NULL,0,"iwpriv %s set NoForwarding=1",NAME_OF_WIRELESS_INTERFACE);

	//shell_recv(NULL,0,"iwpriv %s set MaxStaNum=%s",NAME_OF_WIRELESS_INTERFACE, config_get_string("main", "TZ_MAX_STANUM", "32"));

	tmp = config_get_string("main", "TZ_WIFI_40M_ENABLE", "no");
	strncpy(server_wifi_info->TZ_WIFI_40M_ENABLE, tmp,strlen(tmp));
	print("TZ_WIFI_40M_ENABLE=%s",tmp);

	//get firmware version /config version
	char tmp_buf[32];
	memset(tmp_buf,0,sizeof(tmp_buf));
	shell_recv(tmp_buf,sizeof(tmp_buf),"cat %s | grep '%s' | sed  's/%s=//g'", TZ_VERSION_FILE, TZ_FIRMWARE_INLINE_VERSION, TZ_FIRMWARE_INLINE_VERSION);
	strncpy(server_wifi_info->TZ_IDU_FIRMWARE_VERSION_INFO, tmp_buf,strlen(tmp_buf));
	print("TZ_IDU_FIRMWARE_VERSION_INFO=%s",tmp_buf);

	memset(tmp_buf,0,sizeof(tmp_buf));
	shell_recv(tmp_buf,sizeof(tmp_buf),"cat %s | grep '%s' | sed  's/%s=//g'", TZ_VERSION_FILE, TZ_CONFIG_VERSION, TZ_CONFIG_VERSION);
	strncpy(server_wifi_info->TZ_IDU_CONFIG_VERSION_INFO, tmp_buf,strlen(tmp_buf));
	print("TZ_IDU_CONFIG_VERSION_INFO=%s",tmp_buf);


	memcpy(&server_wifi_info_backup,server_wifi_info,sizeof(InfoStruct));
	config_deinit();//释放uci上下文
	return 0;

}

//w13根据信号质量值控制lte指示灯
void w13_config_lte_led_according2quality(int quality)
{
	if(quality < 0)
	{
		print("invalid quality value:%d!",quality);
	}
	else if(quality == 0)//off
	{
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_OFF, DEV_LED_RED);//red off
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_OFF, DEV_LED_GREEN);//green off
	}
	else if(quality == 1)//bad --> red
	{
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_ON, DEV_LED_RED);//red on
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_OFF, DEV_LED_GREEN);//green off
	}
	else if(quality == 2)//mid --> red+green=>orange
	{
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_ON, DEV_LED_RED);//red on
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_ON, DEV_LED_GREEN);//green on
	}
	else//good --> green
	{
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_OFF, DEV_LED_RED);//red off
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_ON, DEV_LED_GREEN);//green on
	}
	return;
}

//w13根据信号颜色值控制lte指示灯
void w13_config_lte_led_according2quality_color(int color)
{
	if(color < 0)
	{
		print("invalid color value:%d!",color);
	}
	else if(color == 1)//bad --> red
	{
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_ON, DEV_LED_RED);//red on
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_OFF, DEV_LED_GREEN);//green off
	}
	else if(color == 2)//mid --> red+green=>orange
	{
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_ON, DEV_LED_RED);//red on
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_ON, DEV_LED_GREEN);//green on
	}
	else if(color == 3)//good --> green
	{
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_OFF, DEV_LED_RED);//red off
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_ON, DEV_LED_GREEN);//green on
	}
	else//off
	{
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_OFF, DEV_LED_RED);//red off
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:lte_%s/brightness", CMD_LED_OFF, DEV_LED_GREEN);//green off
	}
	return;
}

//w13根据dial_status值控制指示灯
void w13_config_led_according2dial_status(int dial_status)
{
	if(dial_status < 0)
	{
		print("invalid dial_status value:%d!",dial_status);
	}
	else if(dial_status == 0)//put out sim
	{
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:%s/brightness", CMD_LED_OFF, DEV_LED_WPS);//off
	}
	else if(dial_status == 1)//pull in sim
	{
		shell_recv(NULL,0,"echo %d > /sys/class/leds/deco\\:%s/brightness", CMD_LED_ON, DEV_LED_WPS);//on
	}
	else
	{
		print("unkown dial_status value:%d!",dial_status);
	}
	return;
}


void tz_get_if_ip(char* ip_buf,int buf_len)
{
	int sockfd;
	struct ifreq ifr;
	struct sockaddr_in sin;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		print("socket error");
		return;
	}
	strcpy(ifr.ifr_name,network_dev_name);
	if(ioctl(sockfd, SIOCGIFADDR, &ifr) < 0)//直接获取IP地址
	{
		print("ioctl error");
		return;
	}
	memcpy(&sin, &ifr.ifr_dstaddr, sizeof(sin));
	memset(ip_buf,0,buf_len);
	snprintf(ip_buf,buf_len,"%s",inet_ntoa(sin.sin_addr));
	print("%s ip is %s", network_dev_name, inet_ntoa(sin.sin_addr));

}


void tz_check_restart_count(void)
{
	char restart_count[32];
	memset(restart_count, '\0',sizeof(restart_count));
	shell_recv(restart_count, sizeof(restart_count),"touch %s && cat %s", TZ_MGR_RESTART_COUNT_FILE,TZ_MGR_RESTART_COUNT_FILE);
	if(strlen(restart_count) != 0)
	{
		int count = -1;
		count = atoi(restart_count);
		if(count <= 0)
		{
			print("Can not transform string %s",restart_count);
			shell_recv(NULL,0,"echo 1 > %s", TZ_MGR_RESTART_COUNT_FILE);
		}
		else if(count > 0 && count < 13)
		{
			count++;
			shell_recv(NULL,0,"echo %d > %s && rm -f %s", count, TZ_MGR_RESTART_COUNT_FILE, TZ_MGR_ENABLE_DHCP_FLAG_FILE);
		}
		else if(count >=13 && count < 65534)
		{
			shell_recv(NULL,0,"touch %s", TZ_MGR_ENABLE_DHCP_FLAG_FILE);
		}
		else//重置计数
		{
			shell_recv(NULL,0,"echo 13 > %s", TZ_MGR_RESTART_COUNT_FILE);
		}
	}
	else
	{
		print("Start counting..");
		shell_recv(NULL,0,"echo 1 > %s", TZ_MGR_RESTART_COUNT_FILE);
	}
	if( file_exists( TZ_MGR_ENABLE_DHCP_FLAG_FILE ) )
	{
		if(!file_exists( TZ_DHCP_SERVER_ALIVE_FLAG_FILE ))//file no found
		{
			//run dhcp
			shell_recv(NULL,0,"uci set dhcp.lan.ignore=0");
			sleep(1);
			shell_recv(NULL,0,"uci commit");
			shell_recv(NULL,0,"%s start",SCRIPT_DHCPD);
			shell_recv(NULL,0,"%s start",SCRIPT_DNSMAQ);
			shell_recv(NULL,0,"touch %s",TZ_DHCP_SERVER_ALIVE_FLAG_FILE);
		}
	}
	else
	{
		shell_recv(NULL,0,"%s stop",SCRIPT_DHCPD);
		shell_recv(NULL,0,"%s stop",SCRIPT_DNSMAQ);
		shell_recv(NULL,0,"rm -f %s",TZ_DHCP_SERVER_ALIVE_FLAG_FILE);
	}
}


int get_interface_mac(char const* interface_name,OUT unsigned char* mac_buf)
{
	struct ifreq interface;
	int fd=socket(AF_INET,SOCK_DGRAM,0);
	strncpy(interface.ifr_name, interface_name, sizeof(interface.ifr_name));
	if (ioctl(fd, SIOCGIFHWADDR, &interface) < 0) 
	{
		print("ioctl(SIOCGIFHWADDR) failed!");
		return -1;
	}
	close(fd);
	memcpy(mac_buf, interface.ifr_hwaddr.sa_data,6);
	return 0;
}

void tz_check_restore_no_respone_count(void)
{
	char no_respone_count[32];
	memset(no_respone_count, '\0',sizeof(no_respone_count));
	shell_recv(no_respone_count, sizeof(no_respone_count),"touch %s && cat %s", TZ_RESTORE_NO_RESPONE_COUNT_FILE,TZ_RESTORE_NO_RESPONE_COUNT_FILE);
	if(strlen(no_respone_count) != 0)
	{
		int count = -1;
		count = atoi(no_respone_count);
		if(count <= 0)
		{
			print("Can not transform string %s",no_respone_count);
			shell_recv(NULL,0,"echo 1 > %s", TZ_RESTORE_NO_RESPONE_COUNT_FILE);
		}
		else if(count > 0 && count < 6)
		{
			count++;
			shell_recv(NULL,0,"echo %d > %s", count, TZ_RESTORE_NO_RESPONE_COUNT_FILE);
		}
		else if(count >=6 && count < 65534)//长时间没收到回复，清除恢复出厂标志
		{
			shell_recv(NULL,0,"rm -f %s", AFTER_KEY_RESTORE_FLAG_FILE);
			shell_recv(NULL,0,"echo 1 > %s", TZ_RESTORE_NO_RESPONE_COUNT_FILE);//重置计数
		}
		else//重置计数
		{
			shell_recv(NULL,0,"echo 1 > %s", TZ_RESTORE_NO_RESPONE_COUNT_FILE);
		}
	}
	else
	{
		print("Start counting..");
		shell_recv(NULL,0,"echo 1 > %s", TZ_RESTORE_NO_RESPONE_COUNT_FILE);
	}
}

int setWirelessCfgValue(char *name,char *value)
{
	if (NULL == name || NULL == value)
	{
		return -1;
	}
 
	struct uci_context * ctx = uci_alloc_context();
	if (NULL == ctx)
		print("%s","setWirelessCfgValue uci_alloc_context error\n");
	
	int ret = 0;
	#if 0
	struct uci_ptr ptr;
	memset(&ptr, 0, sizeof(ptr));
	ptr.package = "wireless",  
	ptr.section = "@wifi-iface[0]",  
	ptr.option = name,  
	ptr.value = value,  
	#else
	struct uci_ptr ptr;
	memset(&ptr, 0, sizeof(ptr));
	char uciOption[128];
	memset(uciOption, 0, sizeof(uciOption));
	snprintf(uciOption, sizeof(uciOption)-1, "wireless.@wifi-iface[0].%s", name);
	print("uci_lookup_ptr %s\n", uciOption);
	if(UCI_OK != uci_lookup_ptr(ctx, &ptr, uciOption, true))
		{
		   uci_perror(ctx, "no found!\n");
		   return -1;
		}
	print("uci_lookup_ptr ok: %s.%s.%s\n", 
		ptr.package, ptr.section, ptr.option);
	ptr.value = value;
	#endif
	
	print("change cfg: %s.%s.%s, value(%s)\n", 
				ptr.package, ptr.section, ptr.option, ptr.value);
	ret = uci_set(ctx,&ptr);
	print("uci_set ret(%d)\n", ret);
	if (0 == ret){
		ret = uci_commit(ctx, &ptr.p, false);
		print("uci_commit ret(%d)\n", ret);
	}
	uci_unload(ctx,ptr.p);
	uci_free_context(ctx);
	
	print("set wireless cfg end, %s: %s\n", name, value);
	return 0;
}


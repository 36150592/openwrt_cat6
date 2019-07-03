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
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <getopt.h> 

#include "Cshell.h"

//空宏
#define IN 
#define OUT

#define print(format,...)  if(global_debugPrint_flag){printf("%s--%s--%d: ",__FILE__,__FUNCTION__,__LINE__), printf(format,##__VA_ARGS__), printf("\n");}


#define TZ_UPGRADE_TOOL_PROTOCOL_C2S 0x6666
#define TZ_UPGRADE_TOOL_PROTOCOL_S2C 0x7777
#define NAME_OF_WIRELESS_INTERFACE "ra0"


//在创建原始套接字前调用，获取通讯用的mac地址，即发送搜索服务器等命令时的源mac地址,成功返回0，失败返回非0负数
extern int extern_get_self_mac(char* mac_buf, int buf_size);
//进入准备升级状态,在设置临时IP前调用，需要关闭可能影响设备IP、灯状态以及升级的进程。
extern void extern_enter_upgrade_status(void);
//控灯，进入准备升级状态后调用，将设备指示灯设置为正常情况下不可能出现的逻辑，表示设备进入准备升级状态，方便生产人员识别
extern void extern_set_led4ready(void);

char network_dev_name[16]="";//网络接口的名字
bool global_debugPrint_flag =false;
pthread_t tid_recv =-1;//接收tid
int s_sockfd =-1;
int r_sockfd =-1;
pthread_rwlock_t rwlock;
unsigned char hwaddr[6]={0};


bool g_get_receice_flag = false;


typedef unsigned char uint8 ;
typedef unsigned short uint16;
typedef unsigned int uint32;


enum tz_cmd{
	CMD_NULL=0,
	CCMD_REARCH_SERVER,
	SCMD_SET_TMP_IP
};

typedef struct ETH_HEADER
{
	uint8 dest_mac[6];
	uint8 src_mac[6];
	uint16 etype;
}ETH_HEADER;

typedef struct DATA_OF_CMD
{
	uint32 ipv4_addr;
	uint8 reserved_field[42];
}DATA_OF_CMD;

typedef struct TZ_UPGRADE_FIELD
{
	uint16 data_len;
	uint16 cmd;
	DATA_OF_CMD cmd_data;
}TZ_UPGRADE_FIELD;


static void ipv4_to_string(unsigned int ipv4addr,char* buf,int buf_size)
{
    memset(buf, '\0', buf_size);
    snprintf(buf, buf_size,"%d.%d.%d.%d",
        ((ipv4addr>>24)&0xff),((ipv4addr>>16)&0xff),
        ((ipv4addr>>8)&0xff),(ipv4addr&0xff)
        );
}

unsigned short ip_checksum(const void *data, int len)
{
	unsigned short checksum = 0;
	int left = len;
	const unsigned char *data_8 = data;

	while(left > 1)
	{
		checksum += ( ( *data_8 )<<8 )+( *(data_8+1) );
		data_8 += 2;
		left -= 2;
	}

	if(left)
	{
		checksum += ( ( *( unsigned char* )data_8 )<<8 );
	}

	while(checksum > 0xffff)
	{
		checksum = (checksum >> 16) + (checksum & 0xFFFF);
	}

	checksum = (~checksum) & 0xffff;

	return htons(checksum);
}


//设置IP  
void set_self_ip(uint32 IpAddr)
{
	print("enter %s\n", __FUNCTION__);
	char tmpIp[32]={0};
	ipv4_to_string(IpAddr, tmpIp, sizeof(tmpIp));
	util_config_ipv4_addr(network_dev_name,tmpIp);
	return;
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


	IfIndex = ifr.ifr_ifindex;

	if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		print("bind socket failed!");
		return -3;
	}

	return fd;
}


void *recv_thread(void* tag)//接收线程入口
{   

	r_sockfd = CreateRawSocket(network_dev_name,TZ_UPGRADE_TOOL_PROTOCOL_S2C,NULL,NULL);
	if(r_sockfd < 0)
	{
		print("CreateRawSocket recv sock failed!ret=%d",r_sockfd);
	}
	int data_len;
	unsigned char buffer[1024];
	memset(buffer,0,sizeof(buffer));

	while(1)
	{
		print("%s","start recv...");
		data_len = recv(r_sockfd,buffer,1024,0);
		if(data_len >= sizeof(ETH_HEADER))
		{
			ETH_HEADER* pHeader = NULL;
			TZ_UPGRADE_FIELD* pTzField = NULL;

			pHeader = (ETH_HEADER*)buffer;
			pTzField = (TZ_UPGRADE_FIELD*)(buffer + sizeof(ETH_HEADER));

			if(pHeader->etype == TZ_UPGRADE_TOOL_PROTOCOL_S2C)
			{
				print("%s","receive packet:");
				print("dest_mac:%02X:%02X:%02X:%02X:%02X:%02X", 
					pHeader->dest_mac[0], pHeader->dest_mac[1], pHeader->dest_mac[2],
					pHeader->dest_mac[3], pHeader->dest_mac[4], pHeader->dest_mac[5]);
				print("src_mac:%02X:%02X:%02X:%02X:%02X:%02X", 
					pHeader->src_mac[0], pHeader->src_mac[1], pHeader->src_mac[2],
					pHeader->src_mac[3], pHeader->src_mac[4], pHeader->src_mac[5]);
				print("etype=%X",pHeader->etype);
			}


			if(pHeader->etype == TZ_UPGRADE_TOOL_PROTOCOL_S2C && memcmp(pHeader->dest_mac,hwaddr,6) == 0 )
			{
				switch(pTzField->cmd)
				{
					case SCMD_SET_TMP_IP:
						print("%s","receive SCMD_SET_TMP_IP!");
						uint32 recv_ip = ntohl(pTzField->cmd_data.ipv4_addr);
						if(recv_ip != 0)
						{
							pthread_rwlock_wrlock(&rwlock);
							g_get_receice_flag = true;
							pthread_rwlock_unlock(&rwlock);
							print("ip hex = %08x",recv_ip);
							char Ip[32] = {0};
							ipv4_to_string(recv_ip,Ip,sizeof(Ip));
							print("set ip=%s",Ip);
							//进入准备升级状态
							extern_enter_upgrade_status();
							//设置IP  
							set_self_ip(recv_ip);
							//控灯
							extern_set_led4ready();
							close(r_sockfd);
							pthread_exit("recv_thread exit"); // 退出当前线程
						}
						break;
					case CCMD_REARCH_SERVER:
						print("%s","receive cmd CCMD_REARCH_SERVER!");
						break;

					default:
						print("receive unkown cmd:%u",pTzField->cmd);
						break;

				}
			} 
		}
	}

	pthread_exit("recv_thread exit"); // 退出当前线程
}

void signal_handle_func(int sig)//退出
{
	if(tid_recv > 0)
	{
		pthread_cancel(tid_recv);
	}
	pthread_rwlock_destroy(&rwlock);
	if(r_sockfd > 0)
	{
		close(r_sockfd);
	}
	if(s_sockfd > 0)
	{
		close(s_sockfd);
	}
	print("%s","-------------exit after deinit---------------\n");
	exit(0);
}

int main(int argc, char const *argv[])
{

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
	//因系统信号退出时做好善后
	signal(SIGINT, signal_handle_func);
	signal(SIGQUIT, signal_handle_func);
	signal(SIGTERM, signal_handle_func);
	signal(SIGPIPE, signal_handle_func);

	pthread_rwlock_init(&rwlock, NULL);//初始化读写锁
	int ret=0;
	ret = pthread_create(&tid_recv, NULL, recv_thread, NULL);
	if(ret<0)
	{
		printf("Create recv thread error!%s\n", strerror(ret));
		goto deinit;
	}


	int IfIndex=0;
	if(0 != extern_get_self_mac(hwaddr,sizeof(hwaddr)))
	{
		printf("extern_get_self_mac failed!\n");
		if(tid_recv>0)
		{
			pthread_cancel(tid_recv);
		}
		goto deinit;
	}

	s_sockfd = CreateRawSocket(network_dev_name,TZ_UPGRADE_TOOL_PROTOCOL_C2S,NULL,&IfIndex);
	uint8 sbuffer[1024]={0};
	memset(sbuffer,0,sizeof(sbuffer));

	ETH_HEADER eth_header={0};
	memset(eth_header.dest_mac,0xff,sizeof(eth_header.dest_mac));
	memcpy(eth_header.src_mac,hwaddr,sizeof(eth_header.src_mac));
	eth_header.etype = htons(TZ_UPGRADE_TOOL_PROTOCOL_C2S);
	memcpy(sbuffer,&eth_header,sizeof(eth_header));

	TZ_UPGRADE_FIELD tz_field={0};
	memset(&tz_field,0,sizeof(tz_field));
	tz_field.cmd = CCMD_REARCH_SERVER;
	tz_field.data_len = htons(sizeof(TZ_UPGRADE_FIELD));
	memcpy(sbuffer+sizeof(ETH_HEADER),&tz_field,sizeof(tz_field));


	unsigned short 	check_sum = 0;
	check_sum = htons( sizeof(ETH_HEADER)+sizeof(TZ_UPGRADE_FIELD));
	memcpy(sbuffer+sizeof(ETH_HEADER)+sizeof(TZ_UPGRADE_FIELD),&check_sum,sizeof(check_sum));

	check_sum=ip_checksum(sbuffer+sizeof(eth_header),sizeof(TZ_UPGRADE_FIELD));
	memcpy(sbuffer+sizeof(ETH_HEADER)+sizeof(TZ_UPGRADE_FIELD)+sizeof(check_sum),&check_sum ,sizeof(check_sum));


	int send_count = 0;
	while(1)
	{
		int n = send(s_sockfd,sbuffer,sizeof(ETH_HEADER)+sizeof(TZ_UPGRADE_FIELD)+4,0);

		if(n == -1)
		{
			print("send [%d]packet error!",send_count+1);
			send_count++;
			continue;
		}
		else if(n < sizeof(ETH_HEADER)+sizeof(TZ_UPGRADE_FIELD)+4)
		{
			print("send %d byte only!",n);
		}
		print("%s","send cmd success!\n");
		sleep(2);
		pthread_rwlock_rdlock(&rwlock);
		if(g_get_receice_flag)
		{
			pthread_rwlock_unlock(&rwlock);
			print("%s","receive ack cmd!");
			void *dead_str;
    		pthread_join(tid_recv, &dead_str);
    		print("join thread recv:%s",(char *)dead_str);
			break;
		}
		else
		{
			pthread_rwlock_unlock(&rwlock);
			send_count++;
			if(send_count >= 5)
			{
				if(tid_recv>0)
				{
					pthread_cancel(tid_recv);
				}
				break;
			}
		}
	}

deinit:
	pthread_rwlock_destroy(&rwlock);
	if(r_sockfd > 0)
	{
		close(r_sockfd);
	}
	if(s_sockfd > 0)
	{
		close(s_sockfd);
	}
	return 0;
	
}

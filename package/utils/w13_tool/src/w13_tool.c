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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <getopt.h>
#include "Cshell.h"

#define W13_TOOL_VERSION "V1.0"
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif
#define print(format,...)  if(global_debugPrint_flag){printf("%s--%s--%d: ",__FILE__,__FUNCTION__,__LINE__), printf(format,##__VA_ARGS__), printf("\n");}



bool global_debugPrint_flag = false; 
bool global_error_flag = false;


enum TZ_CMD_TYPE{
	UNKOWN = 0,
	WIFI_KEY_T1 = 1,//wifi密码随机算法1
	WIFI_KEY_T2 = 2,//wifi密码随机算法2
	WEB_KEY_T1 = 3,//web密码随机算法2
	LAN_MAC = 4,//w13通过wifi mac计算lan mac
	WAN_MAC = 5,//w13通过wifi mac计算wan mac
	WIFI_SSID_T1 = 6,//通过mac随机的3位数
	WIFI_SSID_T2 = 7,//取mac的后六位
	WIFI_SSID_T3 = 8,//取mac的后三位

};

void print_usage(char const* cmd_name)
{
	printf("\nUsage:%s -[option]...\n",cmd_name);
	printf("\t-D\t\tprint debug infomation\n");
	printf("\t-v\t\tprint version infomation and current sn offset\n");
	printf("\t-h\t\tprint help infomation\n");
	printf("\t-m [MAC_ADDRESS]\t\tspecify the MAC address for get parameters.\n");
	printf("\tnote: The MAC address must be in capital letters such as format: AA:BB:CC:DD:EE:FF\n");
	printf("\t-g [type]\t\tget the parameters of the specified tozed type.\n");
	//printf("\t-s [value]\t\tset parameter of tozed.\n");
	printf("\ntype is one of the following options:\n");
	printf("\tWIFI_KEY_T1 --> %d\n",WIFI_KEY_T1);
	printf("\tWIFI_KEY_T2 --> %d\n",WIFI_KEY_T2);
	printf("\tWEB_KEY_T1 --> %d\n",WEB_KEY_T1);
	printf("\tLAN_MAC --> %d\n",LAN_MAC);
	printf("\tWAN_MAC --> %d\n",WAN_MAC);
	printf("\tWIFI_SSID_T1(special 3bit about mac) --> %d\n",WIFI_SSID_T1);		
	printf("\tWIFI_SSID_T2(mac last 6 bit) --> %d\n",WIFI_SSID_T2);	
	printf("\tWIFI_SSID_T3(mac last 3 bit) --> %d\n",WIFI_SSID_T3);		
			
			
	printf("\ne.g: %s -g %d -m AA:BB:CC:DD:EE:FF\n\n", cmd_name, LAN_MAC);
}


int char2int(char c)
{
	int Data; 
	if ((c >= '0') && (c <= '9'))
	{
		Data = c - '0';
	}
	else if ((c >= 'A') && (c <= 'F')) //A....F
	{
		Data = c - 'A' +10;
	}
	else if((c >= 'a') && (c <= 'f')) //a....f
	{
		Data = c - 'a' +10;
	}
	else
	{
		Data = 0;
		global_error_flag = true;
	}

	return Data;
}

void util_wifi_and_wan_mac(unsigned char* wifi_mac,char* lan_mac,char* wan_mac)
{
	int index;
	int have_right_value;

	if( wifi_mac[3] > 0xf9 )
	{
		index=3;
		have_right_value=FALSE;
		while( index < 6 )
		{
			if( wifi_mac[ index ] < 0xff )
			{
				switch( index )
				{
					case 3:
						sprintf(lan_mac
								,"%02X:%02X:%02X:%02X:%02X:%02X"
								,wifi_mac[0]
								,wifi_mac[1]
								,wifi_mac[2]
								,(unsigned char)( wifi_mac[3]+1 )
								,wifi_mac[4]
								,wifi_mac[5]
								);
						have_right_value=TRUE;
						break;
					case 4:
						sprintf(lan_mac
								,"%02X:%02X:%02X:%02X:%02X:%02X"
								,wifi_mac[0]
								,wifi_mac[1]
								,wifi_mac[2]
								,wifi_mac[3]
								,(unsigned char)( wifi_mac[4]+1 )
								,wifi_mac[5]
								);
						have_right_value=TRUE;
						break;
					case 5:
						sprintf(lan_mac
								,"%02X:%02X:%02X:%02X:%02X:%02X"
								,wifi_mac[0]
								,wifi_mac[1]
								,wifi_mac[2]
								,wifi_mac[3]
								,wifi_mac[4]
								,(unsigned char)( wifi_mac[5]+1 )
								);
						have_right_value=TRUE;
						break;
				}
			}

			if( have_right_value )
			{
				break;
			}

			index+=1;
		}

		if( !have_right_value )
		{
			sprintf(lan_mac
					,"%02X:%02X:%02X:%02X:%02X:%02X"
					,wifi_mac[0]
					,wifi_mac[1]
					,wifi_mac[2]
					,wifi_mac[3]
					,wifi_mac[4]
					,(unsigned char)( wifi_mac[5]+1 )
					);
		}
	}
	else if( ( 0xff-wifi_mac[3] ) > wifi_mac[3] )
	{
		sprintf(lan_mac
				,"%02X:%02X:%02X:%02X:%02X:%02X"
				,wifi_mac[0]
				,wifi_mac[1]
				,wifi_mac[2]
				,(unsigned char)( 0xff-wifi_mac[3] )
				,wifi_mac[4]
				,wifi_mac[5]
				);
	}
	else
	{
		sprintf(lan_mac
				,"%02X:%02X:%02X:%02X:%02X:%02X"
				,wifi_mac[0]
				,wifi_mac[1]
				,wifi_mac[2]
				,(unsigned char)( wifi_mac[3]+0x05 )
				,wifi_mac[4]
				,wifi_mac[5]
				);
	}

	if( wifi_mac[3] > 0xf9 )
	{
		index=3;
		have_right_value=FALSE;
		while( index < 6 )
		{
			if( wifi_mac[ index ] < 0xfe )
			{
				switch( index )
				{
					case 3:
						sprintf(wan_mac
								,"%02X:%02X:%02X:%02X:%02X:%02X"
								,wifi_mac[0]
								,wifi_mac[1]
								,wifi_mac[2]
								,(unsigned char)( wifi_mac[3]+2 )
								,wifi_mac[4]
								,wifi_mac[5]
								);
						have_right_value=TRUE;
						break;
					case 4:
						sprintf(wan_mac
								,"%02X:%02X:%02X:%02X:%02X:%02X"
								,wifi_mac[0]
								,wifi_mac[1]
								,wifi_mac[2]
								,wifi_mac[3]
								,(unsigned char)( wifi_mac[4]+2 )
								,wifi_mac[5]
								);
						have_right_value=TRUE;
						break;
					case 5:
						sprintf(wan_mac
								,"%02X:%02X:%02X:%02X:%02X:%02X"
								,wifi_mac[0]
								,wifi_mac[1]
								,wifi_mac[2]
								,wifi_mac[3]
								,wifi_mac[4]
								,(unsigned char)( wifi_mac[5]+2 )
								);
						have_right_value=TRUE;
						break;
				}
			}

			if( have_right_value )
			{
				break;
			}

			index+=1;
		}

		if( !have_right_value )
		{
			sprintf(wan_mac
					,"%02X:%02X:%02X:%02X:%02X:%02X"
					,wifi_mac[0]
					,wifi_mac[1]
					,wifi_mac[2]
					,wifi_mac[3]
					,wifi_mac[4]
					,(unsigned char)( wifi_mac[5]+2 )
					);
		}
	}
	else if( ( 0xff-wifi_mac[3] ) > ( wifi_mac[3]+5 ) )
	{
		sprintf(wan_mac
					,"%02X:%02X:%02X:%02X:%02X:%02X"
					,wifi_mac[0]
					,wifi_mac[1]
					,wifi_mac[2]
					,(unsigned char)( 0xff-wifi_mac[3]-5 )
					,wifi_mac[4]
					,wifi_mac[5]
					);
	}
	else
	{
		sprintf(wan_mac
				,"%02X:%02X:%02X:%02X:%02X:%02X"
				,wifi_mac[0]
				,wifi_mac[1]
				,wifi_mac[2]
				,(unsigned char)( wifi_mac[3]+0x06 )
				,wifi_mac[4]
				,wifi_mac[5]
				);
	}

}


void util_get_key(char *keypre,unsigned char* mac,char* key,int type,int displine)
{
	char *keylen="8";
	if(type==1)
	{
		int i,j;
		char tmpkey[20];
		char tenkey[20];
		memset(tmpkey,0,sizeof(tmpkey));
		memset(tenkey,0,sizeof(tenkey));
		sprintf(tmpkey,"%02X%02X%02X%02X", ( ( mac[3]^mac[4] )+mac[4]+13 )&0xff, ( ( mac[5]+mac[3] )+31 )&0xff, ( ( mac[4]^mac[5] )+mac[5]+19 )&0xff, ( ( mac[2]^mac[5] )+mac[5]+91 )&0xff);
		if(displine==0)
		{
			//sprintf(key,"%s%s",keypre,tmpkey);
			memcpy(key,tmpkey,strlen(tmpkey));
		}
		else if (displine==1)
		{
			for(i=0;i<8;i++)
			{
				if(tmpkey[i]=='1')
				{
					tmpkey[i]='2';
				}
			}
			memcpy(key,tmpkey,strlen(tmpkey));
		}
		else if(displine==2)
		{
			for(i=0,j=1;i<7&&j<8;i++,j++)
			{
				if(((tmpkey[i]*tmpkey[j])%223)%2==0)
				{
					tmpkey[i]=tolower(tmpkey[i]);
				}
			}
			memcpy(key,tmpkey,strlen(tmpkey));
		}
		else if(displine==3)
		{
			sprintf(tenkey,"%02X%02X%02X%02X%02X", ( ( mac[3]^mac[4] )+mac[4]+13 )&0xff, ( ( mac[5]+mac[3] )+31 )&0xff, ( ( mac[4]^mac[5] )+mac[5]+19 )&0xff, ( ( mac[2]^mac[5] )+mac[5]+91 )&0xff,mac[5]);
			//keyChange(tenkey);
			memcpy(key,tenkey,strlen(tenkey));
		}
		else if(displine==4)
		{
			for(i=0,j=1;i<7&&j<8;i++,j++)
			{
				if(((tmpkey[i]*tmpkey[j])%223)%2==0)
				{
					tmpkey[i]=tolower(tmpkey[i]);
				}
			}
			sprintf(tenkey,"%s%02X", tmpkey,mac[5]);
			//keyChange(tenkey);
			memcpy(key,tenkey,strlen(tenkey));
		}
		else if(displine==5)
		{
			sprintf(tenkey,"%s%02X", tmpkey,mac[5]);
			memcpy(key,tenkey,strlen(tenkey));
		}
	}
	else
	{
		char tmpkey[50];
		char tmpmac[50];
		memset(tmpkey,0,sizeof(tmpkey));
		memset(tmpmac,0,sizeof(tmpmac));
		if(!strcmp(keylen,"8"))
		{
			sprintf(key,"%s%02X%02X%02X%02X",keypre,mac[2],mac[3],mac[4],mac[5]);
		}
		
	}
}


void hextoString(char *inhex,char *outstr)
{
	char BCDNum[3];
	int DestPos=0;
	int i;
	for(i=0;i<strlen(inhex);i+=2)
	{
		memset(BCDNum,0,3);
		memcpy(BCDNum,inhex+i,2);//获得取两位数据
		*(outstr+DestPos) = (char)strtol(BCDNum,NULL,16);//两位字符串转为16进制
		DestPos++;
	}
}


void print_wifikey_by_mac(char *mac,char* pre,int type,int displine)
{
	char param[32];
	memset(param,0,sizeof(param));
	char mac_without_colon[16];
	snprintf(mac_without_colon,sizeof(mac_without_colon),"%C%C%C%C%C%C%C%C%C%C%C%C"
												,mac[0] ,mac[1], mac[3] ,mac[4]
												,mac[6] ,mac[7], mac[9] ,mac[10]
												,mac[12] ,mac[13], mac[15] ,mac[16]);

	char mac_hex_string[16]={0};
	hextoString(mac_without_colon,mac_hex_string);
	util_get_key(pre,(unsigned char*)mac_hex_string,param,type,displine);
	
	printf("%s", param);
}



void util_calculate_pin_by_imei( char* original_imei,char* pin,int only_output_digit,int displine)
{
	unsigned int random_value;
	int index;
	int start_index;
	int string_len=(int)strlen( original_imei );

		for( index=0;index < 8;index++ )
		{
			random_value=1;
			for( start_index=0;start_index < string_len;start_index++ )
			{
				if( random_value > 0xffffff )
				{
					random_value=~random_value;
					random_value=random_value&0xffffff;
				}
				random_value=random_value+(unsigned int)( *( original_imei+( start_index+index )%string_len ) )*( ( (index+1)*(start_index+1) )&0xff );
			}
			if( random_value > 0xffffff )
			{
				random_value=~random_value;
				random_value=random_value&0xffffff;
			}
			if( only_output_digit )
				pin[index] = ( random_value%10 ) + '0';
			else
			{
				random_value=random_value%52;
				if( random_value >= ( 10+26 ) )
					pin[index] = ( random_value-( 10+26 ) ) + 'a';
				else if( random_value >= ( 10 ) )
					pin[index] = ( random_value-( 10 ) ) + 'A';
				else
					pin[index] = random_value + '0';
				if(displine==0)
				{
				}
				else
				{
					if (pin[index] == '1' || pin[index] == 'I' || pin[index] == 'i' || pin[index] == 'L' || pin[index] == 'l')
						pin[index] += 1;
				}	
			}
		}
}


void print_webkey_by_mac(char* mac,int only_output_digit,int displine )
{
	char webkey[32];
	memset(webkey,0,sizeof(webkey));
	util_calculate_pin_by_imei(mac,webkey,only_output_digit,displine);
	
	printf("%s", webkey);
}



void print_lanwan_mac_by_wifimac(char* mac,int type)
{
	char lan[32],wan[32];
	memset(lan,0,sizeof(lan));
	memset(wan,0,sizeof(wan));


	unsigned char r_mac[7] ={0};
	int i=0;
	for(i=0;i<7;i++)
	{
		r_mac[i]= (unsigned char)(((char2int(mac[3*i])<<4)&0xf0)|(char2int(mac[3*i+1])&0x0f));
	}

	util_wifi_and_wan_mac(r_mac,lan,wan);
	if(type==1)//lan
	{
		printf("%s", lan);
	}
	else if(type==2)
	{
		printf("%s", wan);
	}
	else
	{
		printf("00:00:00:00:00:00");
	}
}

void get_ssid( char* mac ,char *ssid)
{
	int ssid_value=0;
	char ssid_suffix[ 100 ];
	//char* mac="d8d866332f4f";
	//char* mac="d8d866332234";
	
	memset( ssid_suffix,0,sizeof( ssid_suffix ) );
/*
	if( ( mac[9] >= '0' )
		&& ( mac[9] <= '9' )
		&& ( mac[10] >= '0' )
		&& ( mac[10] <= '9' )
		&& ( mac[11] >= '0' )
		&& ( mac[11] <= '9' )
		)
	{
		ssid_value=atoi( mac+9 );
	}
	else
	{
		ssid_value=strtol( mac+9,NULL,16 );
		ssid_value=ssid_value%997;
	}
*/
	ssid_value=strtol( mac+9,NULL,16 );
	ssid_value=ssid_value%1000;

	sprintf( ssid_suffix,"%03d",ssid_value );
	memcpy(ssid,ssid_suffix,strlen(ssid_suffix));
	//printf( "\r\n%s,%s",mac,ssid_suffix );
}

void util_get_ssid(unsigned char* mac,char* ssid)
{
	char tmpssid[50];
	memset(tmpssid,0,sizeof(tmpssid));
	sprintf(tmpssid,"%02X%02X%02X%02X%02X%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	get_ssid(tmpssid,ssid);
}


void print_ssid_by_wifimac(char* mac)
{
	
	char mac_without_colon[16];
	//去掉冒号
	snprintf(mac_without_colon,sizeof(mac_without_colon),"%C%C%C%C%C%C%C%C%C%C%C%C"
												,mac[0] ,mac[1], mac[3] ,mac[4]
												,mac[6] ,mac[7], mac[9] ,mac[10]
												,mac[12] ,mac[13], mac[15] ,mac[16]);

	char mac_hex_string[16]={0};

	hextoString(mac_without_colon,mac_hex_string);

	char ssid[32];
	memset(ssid,0,sizeof(ssid));

	util_get_ssid((unsigned char*)mac_hex_string,ssid);
	
	printf("%s", ssid);
}


void print_mac_last6bit(char* mac)
{
	
	char mac_last6bit[7];
	//去掉冒号
	snprintf(mac_last6bit,sizeof(mac_last6bit),"%C%C%C%C%C%C"
												, mac[9] , mac[10], mac[12] 
												, mac[13], mac[15], mac[16]);

	printf("%s", mac_last6bit);
}

void print_mac_last3bit(char* mac)
{
	
	char mac_last3bit[4];
	//去掉冒号
	snprintf(mac_last3bit,sizeof(mac_last3bit),"%C%C%C" 
												, mac[13], mac[15], mac[16]);

	printf("%s", mac_last3bit);
}



int main(int argc, char * const argv[])
{
	int tz_cmd_type=UNKOWN;
	char dev_wifi_mac[18]={0};  //AA:BB:CC:DD:EE:FF
	int ret;
	while((ret= getopt(argc,argv,"g:m:vhD")) != -1)
	{
		switch(ret)
		{
			case 'g':
				tz_cmd_type = atoi(optarg);
				print("tz_cmd_type=%d\n",tz_cmd_type);
				break;
			case 'm':
				strncpy(dev_wifi_mac , optarg, sizeof(dev_wifi_mac));
				print("tz_cmd_type=%d\n",tz_cmd_type);
				break;
			case 'v':
				printf("Current version:%s\n",W13_TOOL_VERSION); 
				return 0;  
			case 'h':
				print_usage(argv[0]);
				return 0;
			case 'D':
				global_debugPrint_flag = true;
				break;
			default:
				printf("Try %s -h get more infomation\n",argv[0]);
				return 1;
		}
	}
	
	if(tz_cmd_type != UNKOWN)
	{

		switch(tz_cmd_type)
		{
			case WIFI_KEY_T1:
				print_wifikey_by_mac(dev_wifi_mac,"",1,0);
				break;

			case WIFI_KEY_T2:
				print_wifikey_by_mac(dev_wifi_mac,"",1,2);
				break;

			case WEB_KEY_T1:
				print_webkey_by_mac(dev_wifi_mac,0,0);
				break;

			case LAN_MAC:
				print_lanwan_mac_by_wifimac(dev_wifi_mac,1);
				break;

			case WAN_MAC:
				print_lanwan_mac_by_wifimac(dev_wifi_mac,2);
				break;

			case WIFI_SSID_T1:
				print_ssid_by_wifimac(dev_wifi_mac);
				break;

			case WIFI_SSID_T2:
				print_mac_last6bit(dev_wifi_mac);
				break;

			case WIFI_SSID_T3:
				print_mac_last3bit(dev_wifi_mac);
				break;

			default:
				print_usage(argv[0]);
				break;
		}


	}
	else
	{
		print_usage(argv[0]);
	}


	return 0;
}



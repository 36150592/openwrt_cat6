#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include <fcntl.h> 
#include <stdlib.h>

#include <arpa/inet.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include "update_config.h"
#include "utility.h"
#include "include.h"
#include "web_user_setting.h"


#define CONFIG_PARAM_FILE "/tmp/config_param_file"
#define EXPORT_CONFIG_FILE "/tmp/.export_config_file"

#define WIFI_24G_SSID_PWD_FILE "/tmp/wifi_24g_ssid_pwd_file"



//unzip the config zip
int  call_decrypt_the_config_file(char* conf_file, char* target_file)
{
	print("conf_file=%s\t, target_file=%s\n",conf_file, target_file);
	int ret = decry_tzupdate_config_file(conf_file, target_file);
	//if(ret == 0)
	//	return export_file_to_param_file(EXPORT_CONFIG_FILE, target_file);
}

void help_message()
{
	printf("help message\n");
	printf("-a:\tdecrypt the config file. after decrypt, you will get a param file: %s\n", CONFIG_PARAM_FILE);
}

char mac[24];
char imei[24];


int main(int argc, char** argv)
{
	int ret = 0;
	int isConfigure=0;
	CONFIG_FUNC config_func = NONE_CONFIG;
	CONFIG_WRITE_WAY config_write_way= APPEND;

	//strcpy(imei,"864485030032153");
	//strcpy(mac,"D8D866020A64");
	
	char shellcmd[128] = "";
	strcpy(shellcmd,"eth_mac g imei");
	read_memory(shellcmd, imei, sizeof(imei));
	util_strip_traling_spaces(imei);

	char lanmac[56] = "";
	memset(shellcmd, 0, sizeof(shellcmd));
	strcpy(shellcmd, "eth_mac r lan");
	read_memory(shellcmd, lanmac, sizeof(lanmac));
	util_strip_traling_spaces(lanmac);

	int i = 0, j = 0; 
	for(i = 0; i < 17; i++)
	{
		if(lanmac[i] != ':')
		{
			mac[j] = lanmac[i];
			j++;
		}
	}

	printf("mac = %s\n", mac);
	printf("imei = %s\n", imei);
	
	while((ret= getopt(argc,argv,"a:b:c:g:h:w:")) != -1)
	{
		switch(ret)
		{
			case 'a':
				{
					char wait_decrypt_file[120] = "";
					strncpy(wait_decrypt_file,optarg,sizeof(wait_decrypt_file));
					call_decrypt_the_config_file(wait_decrypt_file, CONFIG_PARAM_FILE);
					return 0;
				}
				break;
				
			case 'b':
				{
					char wifi_type[120] = "";
					strncpy(wifi_type,optarg,sizeof(wifi_type));
					if(strcmp(wifi_type,"24g") == 0)
					{
						form_wifi_24g_ssid_pwd_to_file(WIFI_24G_SSID_PWD_FILE);
					}
					else if(strcmp(wifi_type,"58g") == 0)
					{
						
					}
					else
						printf("error wifi type\n");
					return 0;
				}
				break;
				
			case 'h':
				help_message();
				return 0;
				break;
				
			case 'c':
				isConfigure = 1;
				break;

			case 'g':
				{
					//char user_param[120] = "";
					//strncpy(user_param,optarg,sizeof(user_param));
					if(strcmp(optarg,"WEB_USER") == 0)
					{
						config_func = WEB_USER;
					}
				}
				break;

			case 'w':
				{
					if(strcmp(optarg,"rewrite") == 0 || strcmp(optarg,"REWRITE") == 0)
					{
						config_write_way = REWRITE;
					}
				}
				break;
				
			default:
				break;
		}
	}

	
	
	if (isConfigure)
	{
		switch(config_func)
		{
			case WEB_USER:
				web_user_config_rewrite();
				break;
			default:
				break;
		}
	}
	
	return 0;
}

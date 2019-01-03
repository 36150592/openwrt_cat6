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


#define CONFIG_PARAM_FILE "/tmp/config_param_file"
#define EXPORT_CONFIG_FILE "/tmp/.export_config_file"



//unzip the config zip
int  call_decrypt_the_config_file(char* conf_file, char* target_file)
{
	print("conf_file=%s\t, target_file=%s\n",conf_file, target_file);
	int ret = decry_tzupdate_config_file(conf_file, EXPORT_CONFIG_FILE);
	if(ret == 0)
		return export_file_to_param_file(EXPORT_CONFIG_FILE, target_file);
}

void help_message()
{
	printf("help message\n");
	printf("-a:\tdecrypt the config file. after decrypt, you will get a param file: %s\n", CONFIG_PARAM_FILE);
}

int main(int argc, char** argv)
{
	int ret = 0;
	while((ret= getopt(argc,argv,"a:h:")) != -1)
	{
		switch(ret)
		{
			case 'a':
				{
					char wait_decrypt_file[120] = "";
					strncpy(wait_decrypt_file,optarg,sizeof(wait_decrypt_file));
					call_decrypt_the_config_file(wait_decrypt_file, CONFIG_PARAM_FILE);
				}
				
				break;
			case 'h':
				help_message();
				break;
			default:
				break;
		}
	}
	
	return 0;
}

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Cshell.h"



int main(int argc, char const *argv[])
{
	char fail_reason[32]={0};
	if (argc < 2)
	{
		snprintf(fail_reason,sizeof(fail_reason),"invalid argument");
		goto print_fail;
	}
	char wifi_mac[18]={0};
	strncpy(wifi_mac,argv[1],sizeof(wifi_mac)-1);
	//根据wlan mac获取 lan 和wan的mac
	char lan_mac[18]={0};
	char wan_mac[18]={0};
	shell_recv(lan_mac,sizeof(lan_mac),"w13_tool -g 4 -m %s",wifi_mac);
	shell_recv(wan_mac,sizeof(wan_mac),"w13_tool -g 5 -m %s",wifi_mac);

	char cmd_result[32]={0};
	shell_recv(cmd_result,sizeof(cmd_result),"eth_mac w wlan %C%C %C%C %C%C %C%C %C%C %C%C"
												,wifi_mac[0] ,wifi_mac[1] ,wifi_mac[3] ,wifi_mac[4]
												,wifi_mac[6] ,wifi_mac[7] ,wifi_mac[9] ,wifi_mac[10]
												,wifi_mac[12] ,wifi_mac[13] ,wifi_mac[15] ,wifi_mac[16]);//写wlan mac
	memset(cmd_result,'\0',sizeof(cmd_result));
	shell_recv(cmd_result,sizeof(cmd_result),"eth_mac r wlan");//写完校验一下
	if(strcmp(cmd_result,wifi_mac))
	{
		snprintf(fail_reason,sizeof(fail_reason),"check wlan mac error");
		goto print_fail;
	}


	memset(cmd_result,'\0',sizeof(cmd_result));
	shell_recv(cmd_result,sizeof(cmd_result),"eth_mac w lan %C%C %C%C %C%C %C%C %C%C %C%C"
												,lan_mac[0] ,lan_mac[1] ,lan_mac[3] ,lan_mac[4]
												,lan_mac[6] ,lan_mac[7] ,lan_mac[9] ,lan_mac[10]
												,lan_mac[12] ,lan_mac[13] ,lan_mac[15] ,lan_mac[16]);//写lan mac
	memset(cmd_result,'\0',sizeof(cmd_result));
	shell_recv(cmd_result,sizeof(cmd_result),"eth_mac r lan");//写完校验一下
	if(strcmp(cmd_result,lan_mac))
	{
		snprintf(fail_reason,sizeof(fail_reason),"check lan mac error");
		goto print_fail;
	}


	memset(cmd_result,'\0',sizeof(cmd_result));
	shell_recv(cmd_result,sizeof(cmd_result),"eth_mac w wan %C%C %C%C %C%C %C%C %C%C %C%C"
												,wan_mac[0] ,wan_mac[1] ,wan_mac[3] ,wan_mac[4]
												,wan_mac[6] ,wan_mac[7] ,wan_mac[9] ,wan_mac[10]
												,wan_mac[12] ,wan_mac[13] ,wan_mac[15] ,wan_mac[16]);//写wan mac
	memset(cmd_result,'\0',sizeof(cmd_result));
	shell_recv(cmd_result,sizeof(cmd_result),"eth_mac r wan");//写完校验一下
	if(strcmp(cmd_result,wan_mac))
	{
		snprintf(fail_reason,sizeof(fail_reason),"check wan mac error");
		goto print_fail;
	}
	printf("success\n");

	return 0;

print_fail:
	printf("failed:%s\n",fail_reason);
	return 1;
	
}

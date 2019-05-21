#include <stdio.h>
#include <stdlib.h>

#include "utility.h"

static void regular_imei_to_bm_format_imei(char* regular_imei, char* bm_format_imei)
{
	int i = 0; 
	for(i = 0; i < 15; i++)
	{
		bm_format_imei[2*i] = '3'; 
		bm_format_imei[2*i+1] = regular_imei[i];
	}
}

static void bm_format_imei_to_regular_imei(char* bm_format_imei, char* regular_imei)
{
	int i = 0; 
	for(i = 0; i < 15; i++)
	{
		regular_imei[i] = bm_format_imei[2*i+1];
	}
}

static void imei_handle(char* get_buf, char* set_buf)
{
	// make sure ttyUSBx exist
	char t_receive_buf[128] = "";

	read_memory("ls /dev/ttyUSB*",t_receive_buf, sizeof(t_receive_buf));
	if(strlen(t_receive_buf) < 4)
	{
		strcpy(set_buf, "fail: modem is not exist");
		return ;
	}

	memset(t_receive_buf, 0, sizeof(t_receive_buf));

	//get imei
	if(strlen(get_buf) < 5)
	{
		print(">>>>>>read imei");
		char imei_in_modem[20] = "";
		read_memory("sendat -e \"at+bmimei\" | grep +BMIMEI | awk '{print $2}'",t_receive_buf, sizeof(t_receive_buf));
		util_strip_traling_spaces(t_receive_buf);
		if(strlen(t_receive_buf) < 30)
		{
			strcpy(set_buf, "fail: imei is error");
			print("fail: imei is error");
			return ;
		}
		bm_format_imei_to_regular_imei(t_receive_buf, imei_in_modem);

		memset(t_receive_buf, 0, sizeof(t_receive_buf));
		read_memory("eth_mac g imei",t_receive_buf, sizeof(t_receive_buf));
		util_strip_traling_spaces(t_receive_buf);

		if (strncmp(t_receive_buf, imei_in_modem, 15) == 0)
		{
			sprintf(set_buf, "%s", t_receive_buf);
		}
		else
		{
			sprintf(set_buf, "%s %s no same", t_receive_buf, imei_in_modem);
			print("fail: factory.bin imei = %s\n", t_receive_buf);
			print("fail: imei in modem = %s\n", imei_in_modem);
		}
	}
	else
	{
		// set imei
		print(">>>>>>write imei");
		char cmdline[128] = "";
		char target_imei[20] = ""; 
		char bm_format_imei[32] = "";
		int i = 0;
		if(strlen(get_buf) < 20)
		{
			strcpy(set_buf, "fail: imei error cmd");
			return ;
		}

		for(i = 0; i < 15; i++)
		{
			target_imei[i] = get_buf[5+i];
		}

		regular_imei_to_bm_format_imei(target_imei, bm_format_imei);

		//set factory imei
		sprintf(cmdline,"eth_mac s imei %s",target_imei);
		system(cmdline);

		//set modem imei  sendat -e "at+bmimei="
		memset(cmdline, 0, sizeof(cmdline));
		sprintf(cmdline,"sendat -e \"at+bmimei=%s\" | grep OK",bm_format_imei);
		read_memory(cmdline,t_receive_buf, sizeof(t_receive_buf));
		util_strip_traling_spaces(t_receive_buf);

		if(strlen(t_receive_buf) > 1)
			strcpy(set_buf, "ok");
		else
			strcpy(set_buf, "fail: write modem imei fail");
	}
}

static void mac_rule_calc(const char* source_mac,char* target_mac, int offset)
{
	strcpy(target_mac, source_mac);
	int s_number = target_mac[9] - '0';
	int t_number = (s_number + offset) % 16;
	if(t_number < 10)
		target_mac[9] = '0' + t_number;
	else
		target_mac[9] = 'A' + t_number - 10;
}

static void set_mac(const char* mac_name, const char* mac_addr)
{
	char cmdline[64] = "";
	char t_mac[24] = "";

	strcpy(t_mac, mac_addr);
	t_mac[2] = 0;
	t_mac[5] = 0;
	t_mac[8] = 0;
	t_mac[11] = 0;
	t_mac[14] = 0;
	t_mac[17] = 0;

	sprintf(cmdline,"eth_mac w %s %s %s %s %s %s %s", mac_name, t_mac, t_mac+3, t_mac+6, t_mac+9, t_mac+12, t_mac+15);
	system(cmdline);

}

static void mac_handle(char* get_buf, char* set_buf)
{
	char t_receive_buf[128] = "";
	//get 2.4g wifi mac
	if(strlen(get_buf) < 5)
	{
		read_memory("eth_mac r wlan",t_receive_buf, sizeof(t_receive_buf));
		util_strip_traling_spaces(t_receive_buf);
		if(strlen(t_receive_buf) < 17)
		{
			strcpy(set_buf, "fail: read wifi mac fail\n");
		}
		else
		{
			sprintf(set_buf, "%s\n",t_receive_buf);
		}
	}
	else
	{
		// set 2.4g mac , 5.8g mac, lan mac, wan mac
		char wifi_24g_mac[24] = "";
		char wifi_58g_mac[24] = "";
		char lan_mac[24] = "";
		char wan_mac[24] = "";
		int i = 0;
		if(strlen(get_buf) < 38)
		{
			strcpy(set_buf, "fail: mac error format\n");
			return ;
		}

		for(i = 0; i < 17; i ++)
		{
			wifi_24g_mac[i] = get_buf[22+i];
		}

		print("will write mac %s\n",wifi_24g_mac);

		mac_rule_calc(wifi_24g_mac, wifi_58g_mac, 4);
		mac_rule_calc(wifi_24g_mac, lan_mac, 8);
		mac_rule_calc(wifi_24g_mac, wan_mac, 12);

		set_mac("wlan", wifi_24g_mac);
		set_mac("wlan_5g", wifi_58g_mac);
		set_mac("lan", lan_mac);
		set_mac("wan", wan_mac);

		strcpy(set_buf, "ok\n");

	}
}

static void sn_handle(char* get_buf, char* set_buf)
{
	char t_receive_buf[128] = "";
	//get sn
	if(strlen(get_buf) < 5)
	{
		read_memory("eth_mac g sn",t_receive_buf, sizeof(t_receive_buf));
		util_strip_traling_spaces(t_receive_buf);
		if(strlen(t_receive_buf) <= 0)
		{
			strcpy(set_buf, "does not exist sn in factory section\n");
		}
		else
		{
			sprintf(set_buf, "%s\n",t_receive_buf);
		}
	}
	else
	{
		// set sn
		char cmdline[64] = "";
		char target_sn[33] = {0};
		int i = 0;

		for(i = 0; i < 32 && get_buf[5+i] != 0; i++)
		{
			target_sn[i] = get_buf[5+i];
		}

		sprintf(cmdline,"eth_mac s sn %s", target_sn);
		system(cmdline);
		strcpy(set_buf, "ok\n");
	}
}

static void upgrade_system_handle(char* get_buf, char* set_buf)
{
	//upgrade_system ftp://192.168.3.123/home/system.bin 
	char buf[128] = {0};
	char *temp = NULL;

	temp = strtok(get_buf, " ");
	temp = strtok(get_buf, NULL);
	if(!temp)
	{
		strcpy(set_buf, "error:NULL addr\n");
		return ;
	}
	
	sprintf(buf, "wget %s -O /tmp/system.bin", temp);
	int ret = system(buf);

	if(ret != 0)
	{
		strcpy(set_buf, "error:download upgrade file fail\n");
		return ;
	}

	ret = system("flashcp -v /tmp/system.bin /dev/mtd3");

	if(ret != 0)
		strcpy(set_buf, "error:upgrade fail\n");
	else
		strcpy(set_buf, "upgrade success\n");

//	system("reboot -f")
	return ;
}

static void update_config_handle(char* get_buf, char* set_buf)
{
	//update_config ftp://192.168.3.123/home/config.zip 
	char buf[128] = {0};
	char *temp = NULL;

	temp = strtok(get_buf, " ");
	temp = strtok(get_buf, NULL);
	if(!temp)
	{
		strcpy(set_buf, "error:NULL addr\n");
		return ;
	}
	
	sprintf(buf, "wget %s -O /tmp/config.zip", temp);
	int ret = system(buf);

	if(ret != 0)
	{
		strcpy(set_buf, "error:download config file fail\n");
		return ;
	}

	ret = system("/etc/tozed/config_update /tmp/config.zip 0");

	if(ret != 0)
		strcpy(set_buf, "error:update config fail\n");
	else
		strcpy(set_buf, "update config success\n");

//	system("reboot -f")
	return ;
}



int s21_precess(char* receive_buf, char* send_message)
{
	print("in s21_precess\n");
	if(strstr(receive_buf,"imei"))
		imei_handle(receive_buf,send_message);
	else if(strstr(receive_buf,"mac"))
		mac_handle(receive_buf,send_message);
	else if(strstr(receive_buf,"sn"))
		sn_handle(receive_buf,send_message);
	else if(strstr(receive_buf,"upgrade_system"))
		upgrade_system_handle(receive_buf,send_message);
	else if(strstr(receive_buf,"update_config"))
		update_config_handle(receive_buf,send_message);
	else
		return FALSE;

	return TRUE;
}

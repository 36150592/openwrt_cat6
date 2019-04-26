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
			return ;
		}
		bm_format_imei_to_regular_imei(imei_in_modem, t_receive_buf);

		memset(t_receive_buf, 0, sizeof(t_receive_buf));
		read_memory("eth_mac g imei",t_receive_buf, sizeof(t_receive_buf));
		util_strip_traling_spaces(t_receive_buf);

		if (strncmp (t_receive_buf, imei_in_modem, 15) == 0)
		{
			sprintf(set_buf, "%s fail", t_receive_buf);
		}
		else
		{
			sprintf(set_buf, "%s %s no same", t_receive_buf, imei_in_modem);
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
	target_mac[9] = '0' + t_number;
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
			sprintf(set_buf, "%s fail\n",t_receive_buf);
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
		if(strlen(get_buf) < 21)
		{
			strcpy(set_buf, "fail: mac error format\n");
			return ;
		}

		for(i = 0; i < 17; i ++)
		{
			wifi_24g_mac[i] = get_buf[4+i];
		}

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

int s21_precess(char* receive_buf, char* send_message)
{
	if(strstr(receive_buf,"imei"))
		imei_handle(receive_buf,send_message);
	if(strstr(receive_buf,"mac"))
		mac_handle(receive_buf,send_message);
	else
		return FALSE;
}
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

int s21_precess(char* receive_buf, char* send_message)
{
	if(strstr(receive_buf,"imei"))
		imei_handle(receive_buf,send_message);
	else
		return FALSE;
}
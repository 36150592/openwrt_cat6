#include "wifi_ssid_pwd.h"
#include "include.h"
#include "utility.h"

extern char mac[24];
extern char imei[24];


static void get_wifi_ssid_24g_suffix(int way, char* suffix)
{
	if(way==1) // mac-last-6
	{
		char* last_six_of_mac=mac+6;
		strcpy(suffix, last_six_of_mac);
	}
	else if(way == 2) //mac-last-3
	{
		int ssid_suffix_value = strtol(mac+9, NULL, 16);
		ssid_suffix_value = ssid_suffix_value%1000;
		if(ssid_suffix_value < 10)
		{
			sprintf(suffix, "00%d",ssid_suffix_value);
		}
		else if(ssid_suffix_value > 9 && ssid_suffix_value < 100)
		{
			sprintf(suffix, "0%d",ssid_suffix_value);
		}
		else
			sprintf(suffix, "%d",ssid_suffix_value);
	}
}

static void get_24g_ssid(char* ssid)
{
	char tz_config_wifi_ssid[56] = "";
	char tz_config_wifi_ssid_static_switch[56] = "";
	char tz_config_wifi_ssid_prefix[56] = "";
	char tz_config_wifi_ssid_suffix[56] = "";
	char shellcmd[256] = "";

	sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZWIFI_24G_MAIN_WIFI_SSID_STATIC_SWITCH");
	read_memory(shellcmd, tz_config_wifi_ssid_static_switch, sizeof(tz_config_wifi_ssid_static_switch));
	util_strip_traling_spaces(tz_config_wifi_ssid_static_switch);

	if( tz_config_wifi_ssid_static_switch[0] != 'n' )
	{
		sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZWIFI_24G_MAIN_WIFI_SSID");
		read_memory(shellcmd, tz_config_wifi_ssid, sizeof(tz_config_wifi_ssid));
		util_strip_traling_spaces(tz_config_wifi_ssid);

		if(strlen(tz_config_wifi_ssid) == 0)
		{
			strcpy(tz_config_wifi_ssid, "s21_2.4g_main");
		}	
	}
	else
	{
		int suffix_way = 0;
		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZWIFI_24G_MAIN_WIFI_SSID_PREFIX");
		read_memory(shellcmd, tz_config_wifi_ssid_prefix, sizeof(tz_config_wifi_ssid_prefix));
		util_strip_traling_r_n_t(tz_config_wifi_ssid_prefix);

		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZWIFI_24G_MAIN_WIFI_SSID_SUFFIX");
		read_memory(shellcmd, tz_config_wifi_ssid_suffix, sizeof(tz_config_wifi_ssid_suffix));
		util_strip_traling_spaces(tz_config_wifi_ssid_suffix);

		if(tz_config_wifi_ssid_suffix[0] == '1')
			suffix_way = 1;  //mac-6
		else if(tz_config_wifi_ssid_suffix[0] == '2')
			suffix_way = 2; //mac-3
		else
			suffix_way = 1;

		memset(tz_config_wifi_ssid_suffix, 0, sizeof(tz_config_wifi_ssid_suffix));
		get_wifi_ssid_24g_suffix(suffix_way, tz_config_wifi_ssid_suffix);

		memset(tz_config_wifi_ssid, 0, sizeof(tz_config_wifi_ssid));
		sprintf(tz_config_wifi_ssid,"%s%s",tz_config_wifi_ssid_prefix,tz_config_wifi_ssid_suffix);
	}

	strcpy(ssid, tz_config_wifi_ssid);
}

static unsigned char ascii_to_hex(unsigned char c) {
	if(c >= '0' && c <= '9') {
		return c - '0';
	}
	else if(c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	else if(c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}

	return 0xff;
}


static unsigned char HexToBin(char *bPtr)
{
	unsigned char bData;
	bData=ascii_to_hex(*bPtr);
	bData=bData<<4;
	bData=bData+ascii_to_hex(*(bPtr+1));
	return bData;
}

static void get_random_wifi_24g_pwd(int way, char* pwd)
{
	if(way==1) // number, big zimu     SLT
	{
		unsigned char m_mac[6+1] = {0};

		m_mac[2] = HexToBin(mac+4);
		m_mac[3] = HexToBin(mac+6);
		m_mac[4] = HexToBin(mac+8);
		m_mac[5] = HexToBin(mac+10);
		sprintf(pwd,"%02X%02X%02X%02X", 
			(( m_mac[3]^m_mac[4] )+m_mac[4]+13 )&0xff, (( m_mac[5]+m_mac[3] )+31 )&0xff, 
					(( m_mac[4]^m_mac[5] )+m_mac[5]+19 )&0xff, (( m_mac[2]^m_mac[5] )+m_mac[5]+91 )&0xff);
	}
	else if(way == 2) // number, big zimu little zimu     Dialog
	{
		unsigned char m_mac[6+1] = {0};
		int i = 0;
		int j = 1;
		
		m_mac[2] = HexToBin(mac+4);
		m_mac[3] = HexToBin(mac+6);
		m_mac[4] = HexToBin(mac+8);
		m_mac[5] = HexToBin(mac+10);
		sprintf(pwd,"%02X%02X%02X%02X", 
			(( m_mac[3]^m_mac[4] )+m_mac[4]+13 )&0xff, (( m_mac[5]+m_mac[3] )+31 )&0xff, 
					(( m_mac[4]^m_mac[5] )+m_mac[5]+19 )&0xff, (( m_mac[2]^m_mac[5] )+m_mac[5]+91 )&0xff);
		for (; i<7, j<8; i++,j++)
		{
			if (((pwd[i]*pwd[j])%223)%2==0)
			{
				pwd[i] = tolower(pwd[i]);
			}
		}
	}
}


static void get_24g_pwd(char* pwd)
{
	char tz_config_wifi_pwd[56] = "";
	char tz_config_wifi_pwd_random[56] = "";
	char tz_config_wifi_pwd_random_way[56] = "";
	char shellcmd[256] = "";
	
	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZWIFI_24G_MAIN_WIFI_PWD_RANDOM");
	read_memory(shellcmd, tz_config_wifi_pwd_random, sizeof(tz_config_wifi_pwd_random));
	util_strip_traling_spaces(tz_config_wifi_pwd_random);
	
	if(tz_config_wifi_pwd_random[0] != 'y')
	{
		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZWIFI_24G_MAIN_WIFI_PWD");
		read_memory(shellcmd, tz_config_wifi_pwd, sizeof(tz_config_wifi_pwd));
		util_strip_traling_spaces(tz_config_wifi_pwd);

		if(strlen(tz_config_wifi_pwd) == 0)
		{
			strcpy(tz_config_wifi_pwd, "87654321");
		}
	}
	else
	{
		int random_way = 0;
		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZWIFI_24G_MAIN_WIFI_PWD_RANDOM_WAY");
		read_memory(shellcmd, tz_config_wifi_pwd_random_way, sizeof(tz_config_wifi_pwd_random_way));
		util_strip_traling_spaces(tz_config_wifi_pwd_random_way);
		
		if(tz_config_wifi_pwd_random_way[0] == '1')
			random_way = 1;
		else if(tz_config_wifi_pwd_random_way[0] == '2')
			random_way = 2;
		else
			random_way = 2;
		
		memset(tz_config_wifi_pwd, 0, sizeof(tz_config_wifi_pwd));
		
		get_random_wifi_24g_pwd(random_way, tz_config_wifi_pwd);
	}

	strcpy(pwd, tz_config_wifi_pwd);
}


void form_wifi_24g_ssid_pwd_to_file(char* fileName)
{
	char ssid[64] = "";
	char pwd[64] = "";
	char shellcmd[256] = "";
	
	get_24g_ssid(ssid);
	get_24g_pwd(pwd);

	sprintf(shellcmd, "rm %s", fileName);
	system(shellcmd);

	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd, "echo wifi_24g_ssid=\"%s\" > %s", ssid, fileName);
	system(shellcmd);

	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd, "echo wifi_24g_pwd=\"%s\" >> %s", pwd, fileName);
	system(shellcmd);
}



#include "web_user_setting.h"
#include "include.h"
#include "utility.h"

extern char* mac;
extern char* imei;

static void util_calculate_pin_by_mac( char* original_imei,char* pin,int only_output_digit,int rm_special_letters)
{
	int string_len=strlen( original_imei );
	unsigned int random_value;
	int index;
	int start_index;

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

		//Ö»Êä³öÊý×Ö
		if( only_output_digit )
		{
			pin[index] = ( random_value%10 ) + '0';
		}
		else
		{
			random_value=random_value%52;
			if( random_value >= ( 10+26 ) )
			{
				pin[index] = ( random_value-( 10+26 ) ) + 'a';
			}
			else if( random_value >= ( 10 ) )
			{
				pin[index] = ( random_value-( 10 ) ) + 'A';
			}
			else
			{
				pin[index] = random_value + '0';
			}
			
			if(rm_special_letters != 0)
			{
				if (pin[index] == '1' || pin[index] == 'I' || pin[index] == 'i' || pin[index] == 'L' || pin[index] == 'l')
				{
					pin[index] += 1;
				}
			}
		}
	}
}


static void util_calculate_pin_by_imei(char* original_imei, char* pin, int only_output_digit )
{
	int string_len=strlen( original_imei );
	unsigned int random_value;
	int index;
	int start_index;
	int tmp;

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

		//?
		if( only_output_digit )
		{
			tmp = ( random_value%10 ) + '0';
		}
		else
		{
			random_value=random_value%52;
			if( random_value >= ( 10+26 ) )
			{
				tmp = ( random_value-( 10+26 ) ) + 'a';
			}
			else if( random_value >= ( 10 ) )
			{
				tmp = ( random_value-( 10 ) ) + 'A';
			}
			else
			{
				tmp = random_value + '0';			
			}

			if (tmp == '1' || tmp == 'I' || tmp == 'i' || tmp == 'L' || tmp == 'l')
				tmp += 1;
		}
		pin[index] = tmp;
	}
}


static void set_web_general_user()
{
	char tz_config_general_user_name[56] = "";
	char tz_config_general_user_pwd[56] = "";
	char tz_config_general_user_pwd_random[56]="";
	char general_user_pwd_md5[64] = ""; 
	char shellcmd[256] = "";

	
	sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZUSER_WEB_GENERAL_USER_NAME");
	read_memory(shellcmd, tz_config_general_user_name, sizeof(tz_config_general_user_name));
	util_strip_traling_spaces(tz_config_general_user_name);
	if(tz_config_general_user_name[0] ==0)
	{
		strcpy(tz_config_general_user_name,"admin");
	}
	set_param_to_config_tozed("TZ_USERNAME", tz_config_general_user_name);

	
	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZUSER_WEB_GENERAL_USER_PWD_RANDOM");
	read_memory(shellcmd, tz_config_general_user_pwd_random, sizeof(tz_config_general_user_pwd_random));
	util_strip_traling_spaces(tz_config_general_user_pwd_random);
	if(tz_config_general_user_pwd_random[0] != 'y')
	{
		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZUSER_WEB_GENERAL_USER_PWD");
		read_memory(shellcmd, tz_config_general_user_pwd, sizeof(tz_config_general_user_pwd));
		util_strip_traling_spaces(tz_config_general_user_pwd);
		//default
		if(tz_config_general_user_pwd[0] ==0)
		{
			strcpy(tz_config_general_user_pwd,"admin");
		}
	}
	else
	{
		int i = 0;
		int j = 0;
		char mac_buff[20] = {0};
		while (i < 12)
		{
			mac_buff[j++] = mac[i++];
			mac_buff[j++] = mac[i++];
			if (i < 12)
			{
				mac_buff[j++] = ':';
			}
		}
		memset(tz_config_general_user_pwd, 0, sizeof(tz_config_general_user_pwd));
		util_calculate_pin_by_mac(mac_buff, tz_config_general_user_pwd, 0, 1);
	}

	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd, "echo -n '%s' | md5sum | awk '{print $1}'", tz_config_general_user_pwd);
	read_memory(shellcmd, general_user_pwd_md5, sizeof(general_user_pwd_md5));
	util_strip_traling_spaces(general_user_pwd_md5);

	print("user_pwd=%s\n", tz_config_general_user_pwd);
	print("after md5 sum real_user_user_pwd=%s\n",general_user_pwd_md5);
	set_param_to_config_tozed("TZ_PASSWD", general_user_pwd_md5);
	
}


static void set_web_seniro_user()
{
	char tz_config_senior_user_name[56] = "";
	char tz_config_senior_user_pwd[56] = "";
	char tz_config_senior_user_pwd_random[56]="";
	char senior_user_pwd_md5[64] = ""; 
	char shellcmd[256] = "";

	
	sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZUSER_WEB_SENIOR_USER_NAME");
	read_memory(shellcmd, tz_config_senior_user_name, sizeof(tz_config_senior_user_name));
	util_strip_traling_spaces(tz_config_senior_user_name);
	if(tz_config_senior_user_name[0] ==0)
	{
		strcpy(tz_config_senior_user_name,"superadmin");
	}
	set_param_to_config_tozed("TZ_SUPER_USERNAME", tz_config_senior_user_name);

	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZUSER_WEB_SENIOR_USER_PWD_RANDOM");
	read_memory(shellcmd, tz_config_senior_user_pwd_random, sizeof(tz_config_senior_user_pwd_random));
	util_strip_traling_spaces(tz_config_senior_user_pwd_random);
	if(tz_config_senior_user_pwd_random[0] != 'y')
	{
		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZUSER_WEB_SENIOR_USER_PWD");
		read_memory(shellcmd, tz_config_senior_user_pwd, sizeof(tz_config_senior_user_pwd));
		util_strip_traling_spaces(tz_config_senior_user_pwd);
		//default
		if(tz_config_senior_user_pwd[0] ==0)
		{
			strcpy(tz_config_senior_user_pwd,"superadmin");
		}
	}
	else
	{
		memset(tz_config_senior_user_pwd, 0, sizeof(tz_config_senior_user_pwd));
		util_calculate_pin_by_imei(imei, tz_config_senior_user_pwd, 0);
	}
	
	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd, "echo -n '%s' | md5sum | awk '{print $1}'", tz_config_senior_user_pwd);
	read_memory(shellcmd, senior_user_pwd_md5, sizeof(senior_user_pwd_md5));
	util_strip_traling_spaces(senior_user_pwd_md5);

	print("senior_pwd=%s\n", tz_config_senior_user_pwd);
	print("after md5 sum real_senior_user_pwd=%s\n",senior_user_pwd_md5);
	set_param_to_config_tozed("TZ_SUPER_PASSWD", senior_user_pwd_md5);
	
}


static void set_web_super_user()
{
	char tz_config_super_user_name[128] = "";
	char tz_config_super_user_pwd[56] = "";
	char tz_config_super_user_pwd_random[56]="";
	char super_user_pwd_md5[64] = ""; 
	char shellcmd[256] = "";
	
	sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZUSER_WEB_SUPER_USER_NAME");
	read_memory(shellcmd, tz_config_super_user_name, sizeof(tz_config_super_user_name));
	util_strip_traling_spaces(tz_config_super_user_name);

	//default
	if(tz_config_super_user_name[0] ==0)
	{
		strcpy(tz_config_super_user_name,"sztozed");
	}

	set_param_to_config_tozed("TZ_TEST_USERNAME", tz_config_super_user_name);

	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZUSER_WEB_SUPER_USER_PWD_RANDOM");
	read_memory(shellcmd, tz_config_super_user_pwd_random, sizeof(tz_config_super_user_pwd_random));
	util_strip_traling_spaces(tz_config_super_user_pwd_random);
	if(tz_config_super_user_pwd_random[0] != 'y')
	{
		memset(shellcmd, 0, sizeof(shellcmd));
		sprintf(shellcmd,"cat %s | grep %s= | awk -F '\"' '{print $2}'", FACTORY_CONFIG_FILE, "TZUSER_WEB_SUPER_USER_PWD");
		read_memory(shellcmd, tz_config_super_user_pwd, sizeof(tz_config_super_user_pwd));
		util_strip_traling_spaces(tz_config_super_user_pwd);
		//default
		if(tz_config_super_user_pwd[0] ==0)
		{
			strcpy(tz_config_super_user_pwd,"keep0It3");
		}
	}
	else
	{
		strcpy(tz_config_super_user_pwd,"keep0It3");
	}

	//change to md5 value
	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd, "echo -n '%s' | md5sum | awk '{print $1}'", tz_config_super_user_pwd);
	read_memory(shellcmd, super_user_pwd_md5, sizeof(super_user_pwd_md5));
	util_strip_traling_spaces(super_user_pwd_md5);

	print("super_pwd=%s\n", tz_config_super_user_pwd);
	print("after md5 sum real_super_user_pwd=%s\n",super_user_pwd_md5);
	set_param_to_config_tozed("TZ_TEST_PASSWD", super_user_pwd_md5);
	
}


int web_user_config_rewrite()
{
	set_web_general_user();
	set_web_seniro_user();
	set_web_super_user();
	system("cfg -c");
	return 0;
}

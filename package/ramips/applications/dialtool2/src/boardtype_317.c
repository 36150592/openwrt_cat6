
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bm806.h"
#include "ec20.h"
#include "include.h"
#include "sim7000.h"
#include "a9500.h"
#include "lt10.h"


void init_317(void)
{
	return;
	cmd_echo("113","/proc/tozed_gpio/led");
	cmd_echo("114","/proc/tozed_gpio/led");
	cmd_echo("112","/proc/tozed_gpio/led");
	cmd_echo("118","/proc/tozed_gpio/led");
}
void update_317(void)
{
	return;
	if(global_dial_vars.signal_rssi_level==1)
	{
		cmd_echo("213","/proc/tozed_gpio/led");
		cmd_echo("314","/proc/tozed_gpio/led");
		cmd_echo("312","/proc/tozed_gpio/led");		
	}
	else if(global_dial_vars.signal_rssi_level==2)
	{
		cmd_echo("213","/proc/tozed_gpio/led");
		cmd_echo("314","/proc/tozed_gpio/led");
		cmd_echo("212","/proc/tozed_gpio/led");
	}
	else if(global_dial_vars.signal_rssi_level==4 ||
		global_dial_vars.signal_rssi_level==3)
	{
		cmd_echo("213","/proc/tozed_gpio/led");
		cmd_echo("214","/proc/tozed_gpio/led");
		cmd_echo("212","/proc/tozed_gpio/led");	
	}
	else		//signael lvl is 0 
	{
		cmd_echo("313","/proc/tozed_gpio/led");
		cmd_echo("314","/proc/tozed_gpio/led");
		cmd_echo("312","/proc/tozed_gpio/led");
	}
	if(global_dial_vars.network_link==1)
		cmd_echo("218","/proc/tozed_gpio/led");
	else
		cmd_echo("318","/proc/tozed_gpio/led");
	
}


MP bm806U_T1_317=
{
	"BM806U-T1",
	"TZ7.823.317",
	"/tmp/.module_bm806_is_used",
	init_317,
	update_317,
	&process_bm806,
};

MP bm806U_317=
{
	"BM806U",
	"TZ7.823.317",
	"/tmp/.module_bm806_is_used",
	init_317,
	update_317,
	&process_bm806,
};

MP bm806U_E1_317=
{
	"BM806U-E1",
	"TZ7.823.317",
	"/tmp/.module_bm806_is_used",
	init_317,
	update_317,
	&process_bm806,
};

MP bm806U_ET1_317=
{
	"BM817C-ET1",
	"TZ7.823.317",
	"/tmp/.module_bm806_is_used",
	init_317,
	update_317,
	&process_bm806,
};

MP bm806U_C1_317=
{
	"BM806U-C1",
	"TZ7.823.317",
	"/tmp/.module_bm806_is_used",
	init_317,
	update_317,
	&process_bm806,
};


MP lt10_317=
{
	"LT10",
	"TZ7.823.317",
	"/tmp/.module_lt10_is_used",
	init_317,
	update_317,
	&process_lt10,
};



MP* project_info_summary[]=
{
	&bm806U_T1_317,
	&bm806U_317,
	&bm806U_E1_317,
	&bm806U_C1_317,
	&lt10_317,
	&bm806U_ET1_317,
};

int project_info_len(void)
{
	return sizeof(project_info_summary)/sizeof(project_info_summary[0]);

}


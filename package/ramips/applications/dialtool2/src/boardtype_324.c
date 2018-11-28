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

void init_324(void)
{
	cmd_echo("113","/proc/tozed_gpio/led");
	cmd_echo("114","/proc/tozed_gpio/led");
	cmd_echo("112","/proc/tozed_gpio/led");
	cmd_echo("118","/proc/tozed_gpio/led");
}
void update_324(void)
{

	if(global_dial_vars.signal_rssi_level==0)
	{
		cmd_echo("313","/proc/tozed_gpio/led");
		cmd_echo("314","/proc/tozed_gpio/led");
		cmd_echo("312","/proc/tozed_gpio/led");
	}
	else if(global_dial_vars.signal_rssi_level==1)
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
	else//signael lvl >=3 3,4,5 
	{
		cmd_echo("213","/proc/tozed_gpio/led");
		cmd_echo("214","/proc/tozed_gpio/led");
		cmd_echo("212","/proc/tozed_gpio/led");	
	}
	
	if(global_dial_vars.network_link==1)
		cmd_echo("218","/proc/tozed_gpio/led");
	else
		cmd_echo("318","/proc/tozed_gpio/led");
	
}

MP bm806U_T1_324=
{
	"BM806U-T1",
	"TZ7.823.324",
	"/tmp/.module_bm806_is_used",
	init_324,
	update_324,
	&process_bm806,
};


MP bm806U_C1_324=
{
	"BM806U-C1",
	"TZ7.823.324",
	"/tmp/.module_bm806_is_used",
	init_324,
	update_324,
	&process_bm806,
};
MP bm806C_324=
{
	"BM806C",
	"TZ7.823.324",
	"/tmp/.module_bm806_is_used",
	init_324,
	update_324,
	&process_bm806,
};
MP bm806U_324=
{
	"BM806U",
	"TZ7.823.324",
	"/tmp/.module_bm806_is_used",
	init_324,
	update_324,
	&process_bm806,
};

MP bm806U_E1_324=
{
	"BM806U-E1",
	"TZ7.823.324",
	"/tmp/.module_bm806_is_used",
	init_324,
	update_324,
	&process_bm806,
};

MP sim7000c_324=
{
	"sim7000c",
	"TZ7.823.324",
	"/tmp/.module_sim7000c_is_used",
	init_324,
	update_324,
	&process_sim7000c,
};

MP ar9500_324=
{
	"A9500",
	"TZ7.823.324",
	"/tmp/.module_ar9500_is_used",
	init_324,
	update_324,
	&process_ar9500,
};

MP ec20_324=
{
	"A9500",
	"TZ7.823.324",
	"/tmp/.module_ec20_is_used",
	init_324,
	update_324,
	&process_ec20,
};

MP lt10_324=
{
	"LT10",
	"TZ7.823.324",
	"/tmp/.module_lt10_is_used",
	init_324,
	update_324,
	&process_lt10,
};

MP* project_info_summary[]=
{
	&bm806U_C1_324,
	&bm806U_T1_324,
	&bm806U_E1_324,
	&bm806U_324,
	&bm806C_324,
	&sim7000c_324,
	&ar9500_324,
	&ec20_324,
	&lt10_324,
};

int project_info_len(void)
{
	return sizeof(project_info_summary)/sizeof(project_info_summary[0]);

}


#if 0
/* we use this cmd to update system info*/
const struct sc
{
	"at+csq",
		
}Sys_Check;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bm906.h"
#include "bm916.h"
#include "bm806.h"
#include "ec20.h"
#include "include.h"
#include "sim7000.h"
#include "a9500.h"
#include "lt10.h"

void init_306a(void)
{
	cmd_echo("113","/proc/tozed_gpio/led");
	cmd_echo("114","/proc/tozed_gpio/led");
	cmd_echo("112","/proc/tozed_gpio/led");
	cmd_echo("118","/proc/tozed_gpio/led");
}
void update_306a(void)
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

MP bm906_306A=
{
	"BM906",
	"TZ7.823.306A",
	"/tmp/.module_bm906_is_used",
	init_306a,
	update_306a,
	&process_bm906,
};


MP bm916_306A=
{
	"BM916",
	"TZ7.823.306A",
	"/tmp/.module_bm916_is_used",
	init_306a,
	update_306a,
	&process_bm916,
};


MP bm806U_T1_306A=
{
	"BM806U-T1",
	"TZ7.823.306A",
	"/tmp/.module_bm806_is_used",
	init_306a,
	update_306a,
	&process_bm806,
};


MP bm806U_C1_306A=
{
	"BM806U-C1",
	"TZ7.823.306A",
	"/tmp/.module_bm806_is_used",
	init_306a,
	update_306a,
	&process_bm806,
};
MP bm806C_306A=
{
	"BM806C",
	"TZ7.823.306A",
	"/tmp/.module_bm806_is_used",
	init_306a,
	update_306a,
	&process_bm806,
};
MP bm806U_306A=
{
	"BM806U",
	"TZ7.823.306A",
	"/tmp/.module_bm806_is_used",
	init_306a,
	update_306a,
	&process_bm806,
};

MP bm806U_E1_306A=
{
	"BM806U-E1",
	"TZ7.823.306A",
	"/tmp/.module_bm806_is_used",
	init_306a,
	update_306a,
	&process_bm806,
};

MP sim7000c_306A=
{
	"sim7000c",
	"TZ7.823.306A",
	"/tmp/.module_sim7000c_is_used",
	init_306a,
	update_306a,
	&process_sim7000c,
};

MP ar9500_306A=
{
	"A9500",
	"TZ7.823.306A",
	"/tmp/.module_ar9500_is_used",
	init_306a,
	update_306a,
	&process_ar9500,
};

MP ec20_306A=
{
	"A9500",
	"TZ7.823.306A",
	"/tmp/.module_ec20_is_used",
	init_306a,
	update_306a,
	&process_ec20,
};

MP lt10_306A=
{
	"LT10",
	"TZ7.823.306A",
	"/tmp/.module_lt10_is_used",
	init_306a,
	update_306a,
	&process_lt10,
};

MP* project_info_summary[]=
{
	&bm906_306A,
	&bm916_306A,
	&bm806U_C1_306A,
	&bm806U_T1_306A,
	&bm806U_E1_306A,
	&bm806U_306A,
	&bm806C_306A,
	&sim7000c_306A,
	&ar9500_306A,
	&ec20_306A,
	&lt10_306A,
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "logger.h"

void time_printf(char* message)
{
	char cmd[256] = "";
	system("echo \"-------------------------\" >> /tmp/dia_time.log");
	system("cat /proc/uptime >> /tmp/dia_time.log");
	sprintf(cmd, "echo \"%s\" >>  /tmp/dia_time.log",message);
	system(cmd);
}



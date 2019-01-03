#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utility.h"

#define TOOL_RET "/tmp/.tozed_tool_ret"

int tz_ret(int num)
{
	char cmd[128] = "";
	if(num == 0)
	{
		sprintf(cmd, "echo success > %s", TOOL_RET);
	}
	else
	{
		sprintf(cmd, "echo \"fail:%d\" >%s",num, TOOL_RET);
	}
	system(cmd);
	return num;
}



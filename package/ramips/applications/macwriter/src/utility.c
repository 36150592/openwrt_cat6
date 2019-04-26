#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utility.h"
#include <ctype.h>
#include<unistd.h>


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


int read_memory(char *shellcmd, char *out, int size) {

	FILE *stream;
	char buffer[size];
    memset(buffer, 0, sizeof(buffer));

	stream = popen(shellcmd, "r");
	if(stream != NULL){
        fread(buffer, sizeof(char), sizeof(buffer), stream);
        pclose(stream);

		memcpy(out, buffer, strlen(buffer) + 1);

		return 0;
	} else {
		out[0] = '\0';
	}

	return -1;
}


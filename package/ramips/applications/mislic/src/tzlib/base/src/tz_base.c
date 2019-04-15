
#include "tz_base.h"


int get_app_pid_by_name(char *app_name)
{
	char buff[100];
	FILE *fp;
	
	char cmd[200];
	
	sprintf(cmd, "ps | grep %s | busybox awk \'{ print $2 }\'", app_name);
	
	fp = popen(cmd, "r");
	if(fp==NULL)
	{
		tz_base_dbg("popen %s error", cmd);
		return -1;
	}
	
	memset(buff, 0, sizeof(buff));
	if(fgets(buff, sizeof(buff)-1, fp)==NULL)
	{
		pclose(fp);
		tz_base_dbg("fget %s error", cmd);
		return -1;
	}
	
	tz_base_dbg("get line=%s", buff);
	
	return atoi(buff);
}


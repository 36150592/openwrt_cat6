#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>


int get_export_value(char* buf, int buf_len, char const *file_path, char const* param);

int main(int argc, char const *argv[])
{
	char value_buf[128]={0};
	int ret =get_export_value(value_buf,sizeof(value_buf),argv[1],argv[2]);
	printf("%s",value_buf);
	return ret;
}


int get_export_value(char* buf, int buf_len, char const *file_path, char const* param)
{
	if(buf == NULL)
	{
		return 1;
	}

	FILE* export_file=fopen(file_path,"r");//read only
	if(export_file==NULL)
	{
		return 2;
	}
	bool finded = false;
	char tmp_buf[128]={0};
	char export_param[128]={0};
	char *p_key = NULL;
	if(export_file != NULL)
	{
		while(NULL != fgets(tmp_buf, sizeof(tmp_buf), export_file))
		{
			char sub_string[128]={0};
			snprintf(sub_string, sizeof(sub_string), "export %s=\"",param);
			p_key=strstr(tmp_buf,sub_string);
			if(p_key != NULL)
			{
				finded =true;
				char *save_ptr = NULL;
				strtok_r(p_key,"\"",&save_ptr);
				char *p_value = strtok_r(NULL,"\"",&save_ptr);
				if(p_value[0] == '\n')//empty value
				{
					export_param[0]='\0';
				}
				else
				{
					strncpy(export_param, p_value, sizeof(export_param));
				}
				break;
			}

		}
	}

	if(finded)
	{
		strncpy(buf, export_param, buf_len);
	}

	return 0;
}



#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "update_config.h"
#include "utility.h"


static void write_to_file(int fd, char* name, char* value)
{
	char buf[256]="";
	sprintf(buf,"%s=%s\n",name,value);
	write(fd, buf, strlen(buf));
	print("will set %s=%sK\n",name, value);
}


static const unsigned char file_encrypt_codes[]={ 0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef };
//根据字节序数的最低三位,依次和0x78,0x89,0x9a,0xab,0xbc,0xcd,0xde,0xef相异或,最后将生成的配置文件保存为一个二进制文件;
//对文件内容进行加密
void util_encrypt_file_content( unsigned char* file_content,int file_size )
{
	int index=0;
	while( index < file_size )
	{
		file_content[ index ]=file_content[ index ]^file_encrypt_codes[ index&0x07 ];
		index+=1;
	}
}


/* function: ip_checksum_add
 * adds data to a checksum
 * current_sum - the current checksum (or 0 to start a new checksum)
 * data        - the data to add to the checksum
 * len         - length of data
 */
unsigned short ip_checksum(const void *data, int len)
{
	unsigned short checksum = 0;
	int left = len;
	const unsigned char *data_8 = data;

	while(left > 1)
	{
		checksum += ( ( *data_8 )<<8 )+( *(data_8+1) );
		data_8 += 2;
		left -= 2;
	}

	if(left)
	{
		checksum += ( ( *( unsigned char* )data_8 )<<8 );
	}

	//while(checksum > 0xffff)
	//{
		//checksum = (checksum >> 16) + (checksum & 0xFFFF);
	//}

	checksum = (~checksum) & 0xffff;

	return htons(checksum);
}



int decry_tzupdate_config_file(char* conf_file, char* export_file)
{
	if(!access(conf_file,F_OK|R_OK))
	{
		FILE *wstream;
		unsigned char file_content[CONFIG_SIZE];
		memset(file_content,0,sizeof(file_content));
		print("read encry config fle\n");
		if((wstream = fopen(conf_file, "rb")) == NULL)
		{
			print("open encry config file FAIL\n");
			return tz_ret(CONFIG_OPEN_ENCRY_CONFIG_FAIL);
		}
		int len=fread(file_content, 1, CONFIG_SIZE, wstream);
		fclose( wstream );
		if(len<=0)
		{	
			print("read encry config file FAIL\n");
			return tz_ret(CONFIG_READ_ENCRY_CONFIG_FAIL);	
		}
		
		util_encrypt_file_content(file_content,len);
		unsigned short checksum=ip_checksum(file_content, len);
		
		//unsigned short checksum2=0xff00&&(file_content[len-2]<<8)+0x00ff&&file_content[len-1];
		
		//if(!checksum1!=checksum2)
		if(checksum)
		{
			print("encry config file check sum FAIL\n");
			return tz_ret(CONFIG_CHECK_SUM_FAIL);
		}
			
		int buff_size=0;
		int real_len=0;
		char real_file_content[CONFIG_SIZE];
		memset(real_file_content,0,sizeof(real_file_content));
		buff_size=(file_content[0]<<24)&&0xff000000+(file_content[1]<<16)&&0x00ff0000+(file_content[2]<<8)&&0x0000ff00+(file_content[3]&&0x000000ff);
		if(buff_size%2)
		{
			real_len=len-7;
			memcpy(real_file_content,&file_content[4],real_len);	
		}
		else
		{
			real_len=len-6;
			memcpy(real_file_content,&file_content[4],real_len);
		}
		if((wstream = fopen(export_file, "wb")) == NULL)
		{
			print("open or create config file FAIL\n");
			return tz_ret(CONFIG_CREATE_CONFIG_FILE_FAIL);
		}
		//len=fwrite(file_content, 1, len, wstream);
		len=fwrite(real_file_content, 1, real_len, wstream);
		fclose( wstream );
		if(len<=0)
		{
			print("write config file FAIL\n");
			return tz_ret(CONFIG_WRITE_CONFIG_FILE_FAIL);
		}
			
		print("write config file ok\n");
		return tz_ret(CONFIG_SUCCESS);
	}
	else
	{
		print("encry config file is not exist\n");
		return tz_ret(CONFIG_ENCRY_CONFIG_NO_EXIST);
	}
}


int export_file_to_param_file(char* export_file, char* target_file)
{
	char* p1;
	char temp_buffer[769];
	char* name = NULL;
	char* value = NULL;
	int fd;
	char nv_cmd[256]= "";
	char cmdline[128] = "";
	char tmp_buf[128] = "";
	char cmd[128] ="";
	
	FILE* file_handle=fopen(export_file,"r" );

	if( file_handle == NULL )
	{
		return tz_ret(CONFIG_OPEN_EXPORT_FILE_FAIL);
	}

	sprintf(cmd, "rm -rf %s",target_file);
	system(cmd);

	memset( temp_buffer,0,sizeof(temp_buffer) );
	fd=open(target_file, O_RDWR | O_CREAT);
	if(fd == -1)
	{
		fclose( file_handle );
		return tz_ret(CONFIG_OPEN_PARAM_FILE_FAIL);
	}
		
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		if(strstr(temp_buffer,"export ") == NULL)
		{
			continue;
		}
		name=temp_buffer+strlen("export ");
		while( (*name == ' ' || *name == '\t') && *name != '\n')
		{
			name++;
		}
			
		value=strstr(name,"=");
		if(value)
		{
			*value = 0;
			value=value+1;
			if(*value == '\"')
			{
				value=value+1;
				//get the last "
				p1 = value+strlen(value)-1;
				while(*p1 != '\"')
				{
					p1--;
				}
				*p1=0;
			}
		}

		write_to_file(fd,name,value);
	}
	
	fclose( file_handle );
	close(fd);
		
	system("sync");
	return tz_ret(CONFIG_SUCCESS);
}



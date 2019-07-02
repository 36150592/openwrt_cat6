#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <errno.h>
#include "logger.h"
#include "common.h"
#include "queue.h"

PARMS cfg[128];  //storage of configs,read from cfg

void* strip_head_tail_space(void *str)
{
	if(str == NULL)
		return NULL;
	int len=0;
	char *head,*tail;
	len=strlen(str);
	head=str;
	tail=str+len-1;
	while((len>0) && (*head =='\t' || *head == '\r' || *head =='\n' || *head ==' '))
	{
		head++;
		len--;
	}

	while((len>0) && (*tail =='\t' || *tail == '\r' || *tail =='\n' || *tail ==' '))
	{
		*tail--='\0';
		len--;
	}
	if(len==0)
		return NULL;
	else
	{
//		log_info("%s %s",__FUNCTION__,head);
		*(head+len)='\0';
//		printf("str %s:%d\n",head,len);
		return head;
	}
}

void separate_str(char* str,char const *sep,char**  ret)
{
	char buffer[1024]={0};
	char* token=NULL;
	int i=0;
	strncpy(buffer,str,sizeof(buffer));
//	log_info("%s_%d:%s\n",__FUNCTION__,__LINE__,buffer);
	for(i=0,token = strtok(buffer,sep) ; token != NULL; token=strtok(NULL,sep),i++)
	{
//		log_info("%s_%d,i:%d\n",__FUNCTION__,__LINE__,i);
		strncpy(ret[i],token,strlen(token));
	}
}

int separate_config(char *str,char pointer,PARMS *ptr)
{
	char *tmp_ptr,*buff;
	if(NULL!= (buff=strip_head_tail_space(str)))
	{	
//		log_info("%s %d %s %s\n",__FUNCTION__,__LINE__,str,buff);
		tmp_ptr=strchr(buff,pointer);
//		log_info("%s %d %s\n",__FUNCTION__,__LINE__,buff);
		if(NULL == tmp_ptr)
		{
			perror("wrong format for config name");
			write_str_file(DIAL_INDICATOR,"wrong format for config name","w+");
			exit(-1);
		}
		else
		{
			*tmp_ptr='\0';
			strncpy(ptr->Name,buff,strlen(buff));
			strncpy(ptr->Value,tmp_ptr+1,strlen(tmp_ptr+1));
		}
		return 0;
	}
	else
	{
		return -1;
	}
}
void fill_config(char *filename,PARMS* ptr)
{
	FILE *f;
	char buff[512];
	f=fopen(filename,"r");
	if ( NULL == f)
	{
		perror("read config file error\n");
		write_str_file(DIAL_INDICATOR,"read config file error","w+");
		exit(-1);
	}
	while(!feof(f))
	{
		memset(buff,0,sizeof(buff));
		fgets(buff,sizeof(buff),f);
		if(0 == separate_config(buff,'=',ptr))
			ptr++;
	}
	fclose(f);
	return;
}

void get_config_specified(PARMS*ptr,int len,char *name,char *value)
{
	int i;
	for(i=0;i<len;i++)
	{
		if(!strncmp((ptr+i)->Name,name,strlen(name)))
		{
			strncpy(value,(ptr+i)->Value,strlen((ptr+i)->Value));
			break;
		}
	}
	return;
}

/* if format is 0 ,wirte str by string;
 * else if format is 1,write str by hex;
*/
void write_str_file(char *filename,char *str,char *format)
{
	FILE *f;
	f=fopen(filename,format);
	if (NULL == f)
	{
		printf("%s open file error\n",__FUNCTION__);
		return;
	}
	fprintf(f,"%s\n",str);
	return;
}

void str_to_hex(char *str,char *hex,int length)
{
	int i,n=0;
	for(i=0;i<strlen(str);i++)
	{
		n+=snprintf( hex+n,length-n,"%02x",*(str+i)  );
	}
	*(hex+n)=*(str+i);	//keep the last charactor
}
/*just for ascii str of num ,transfer decimal str,such as 3839 to 89*/
void num_asciistr_to_decimalstr(char * input,char * output)
{
	int i=0,j=0;
	while(*(input+i) != '\0')
	{
		if(A_CMP_B_ACCORDING_B(input+i,"3"))
		{
			i++;
		}
		else
		{
			strncpy(output+j,input+i,1);
			i++;
			j++;
		}
		
	}
	output[j]='\0';
//	log_info("%s_%d:%s\n",__FUNCTION__,__LINE__,output);
}
//rm one file
int rm_file(const char *file_name)
{
	//open file handle to write
#ifndef WIN32
	if( !access( file_name,R_OK ) )
	{
		if( 0 == unlink( file_name ))
			return 0;
		else
			return -1;
	}
#endif
	return 0;
}
//check file exist or not
int check_file_exist(char *file_path)
{
	errno=0;
	if( access( file_path,F_OK ) && errno ==ENOENT)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}
//write string to one specified file
int cmd_touch(const char *file_name)
{
	//open file handle to write
	FILE* file_handle=fopen(file_name,"wb");

	if( file_handle == NULL )
	{
		return -1;
	}

	//close file handle
	fclose(file_handle);

	return 0;
}

int check_ipv4_exist(char *dev_name )
{
    char cmd_str[256]={0};
    char result_str[256]={0};
    FILE *f; 
    snprintf(cmd_str,sizeof(cmd_str),"ifconfig -a |grep %s|grep \"inet addr\"",dev_name);
    f=popen(cmd_str,"r");
    if( NULL != f ) 
    {   
        fgets(result_str,sizeof(result_str),f);
        if(NULL != strstr(result_str,"inet addr"))
        {
            pclose(f);
            return 0;
        }
        else
        {
            pclose(f);
            return 1;
        }
    }   
    else
    {   
        pclose(f);
        return 1;
    }   
}
//get system time ,write it to str or record_file,and we don't care write file right or not
int get_sys_time(char * str,int str_lenth,char *record_file)
{
	time_t t;
	if(time(&t) != -1)
	{
		if(NULL != str )
		{
			if(NULL != record_file )
				write_str_file(record_file,ctime(&t),"w+");
			if(strncpy(str,ctime(&t),str_lenth)>0)
			{
				return TRUE;
			}
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;
}


int get_runtime(void)
{
	struct sysinfo info;
	long time_tmp;
	sysinfo(&info);
	time_tmp= info.uptime;
	return time_tmp/60;
}

int util_send_cmd(int dev_handle,char *cmd,int* exception_dealwith)
{
	int bytes_count;
	static int write_failed_count=0;
	#ifndef WIN32
	bytes_count = ( int )write( dev_handle,cmd,strlen( cmd ) );
	#else
	bytes_count=sio_write( dev_handle,cmd,strlen( cmd ) );
	#endif

	log_info("\n""%s_%d,write cmd:%s",__FUNCTION__,__LINE__,cmd);

	if(bytes_count < 0)
	{
		write_failed_count++;
		log_error("\r\nwrite cmd fail:%s ",cmd);

		if( write_failed_count >= 20 )
		{
//			cmd_touch( MODULE_ABNORMAL_FILE );  //if occured ,we try reset module
			/*here we dont exit,just shut down all pthread ,go into a waiting status*/
			if(NULL!= exception_dealwith)
				*exception_dealwith=1;
	#ifdef DEBUG
			get_sys_time(NULL,0,MODULE_ERROR_FILE);
	#endif
			log_error("\r\nfail to write at cmd for more than 20 times,maybe the module is dead!");
			char tz_reboot_buff[100] = "";
			sprintf(tz_reboot_buff,"/etc/rc.d/rc.get_reboot_info %s--%d ",__FILE__,__LINE__);
			system(tz_reboot_buff);
			system("echo kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk >> /etc/record");
			system("date >> /etc/record");
			system("lsusb >> /etc/record");
			system("logread >> /etc/record");
			system("ls /dev/tty* >> /etc/record");
			system("dmesg >> /etc/record");
			system("reboot");
//			exit(0);
		}
		return FALSE;
	}
	else
	{
		write_failed_count=0;
	}
	return TRUE;
}

void * m_malloc(unsigned int  size) 
{

	void* ret;

	if (size == 0) {
		log_info("m_malloc failed");
		return NULL;
	}
	ret = calloc(1, size);
	if (ret == NULL) {
		log_error("m_malloc failed");
		return NULL;
	}
	return ret;

}


void* m_malloc_two(int m,int n)
{
	int i;
	char** result=(char**)m_malloc(m*sizeof(char *));
	for(i=0;i<m;i++)
	{
		result[i]=(char*)m_malloc(n*sizeof(char));
	}
	return result;
}

void free_two(char** ptr,int m,int n)
{
	int i;
	for(i=0;i<m;i++)
		m_free(ptr[i]);
	m_free(ptr);
}
int stop_timer()
{
	int res;
	struct itimerval tick;
	memset(&tick,0,sizeof(tick));
	tick.it_value.tv_sec = 0;
	tick.it_value.tv_usec =0;
	tick.it_interval.tv_sec = 0;
	tick.it_interval.tv_usec =0;
	
	res=setitimer(ITIMER_REAL,&tick,NULL);
	if(res!=0)
	{
		log_error("settitimer error\n");
		return FALSE;
	}
	else
		return TRUE;
}
int refresh_timer(int sec)
{
 	int res;
	struct itimerval tick;
	memset(&tick,0,sizeof(tick));
	tick.it_value.tv_sec = sec;
	tick.it_value.tv_usec =0;
	tick.it_interval.tv_sec = sec;
	tick.it_interval.tv_usec =0;
	
	res=setitimer(ITIMER_REAL,&tick,NULL);
	if(res!=0)
	{
		log_error("settitimer error\n");
		return FALSE;
	}
	else
		return TRUE;
}
int  get_value(char* file_path,int base)
{
	FILE *f=fopen(file_path,"r");
	char buffer[512]={0};
	if(NULL!=f && NULL!=fgets(buffer,sizeof(buffer),f))
	{
		fclose(f);
		return strtol(buffer,NULL,base);
	}
	return -1;
}
#if 0
int write_pid(char* path)
{
	pid_t ppid;
	FILE* f;
	ppid=getpid();
	f=fopen(path,"w+");
	if(NULL!=f)
	{
		fwrite();
	}
	else
		return -1;
}	
#endif
int getpid_by_pidfile(char* pidfile_path)
{
	FILE*f;
	char buffer[512];
	f=fopen(pidfile_path,"r");
	if(NULL== f)
	{
		log_info("open %s error\n",pidfile_path);
		return FALSE;
	}
	if(NULL!=fgets(buffer,sizeof(buffer),f))
	{
		fclose(f);
		return atoi(buffer);
	}
	else
	{
		fclose(f);
		return FALSE;
	}
	
}
//write string to one specified file
int cmd_echo(char* str,const char *file_name)
{
	//open file handle to write
	FILE* file_handle=fopen(file_name,"wb");

	if( file_handle == NULL )
	{
		return -1;
	}

	fwrite(str,strlen( str ),1,file_handle);
	fwrite("\n",strlen( "\n" ),1,file_handle);
	//close file handle
	fclose(file_handle);

	return 0;
}

int read_memory(char *shellcmd, char *out, int size) {

	FILE *stream;
	char buffer[size];
    memset(buffer, 0, sizeof(buffer));

	stream = popen(shellcmd, "r");
	if(stream != NULL){
        fread(buffer, sizeof(char), sizeof(buffer), stream);
		printf(">>>>>>>>>>>>.buffer = %s\n",buffer);
        pclose(stream);

		memcpy(out, buffer, strlen(buffer) + 1);

		return 0;
	} else {
		out[0] = '\0';
	}

	return -1;
}



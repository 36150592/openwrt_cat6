#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include "log.h"
#include "serial.h"

#define LOG_FILE "/tmp/at_log"
#define TRUE 1
#define FALSE 0

#define CMD_EXE_OK "\r\nOK\r\n"
#define CMD_EXE_ERROR "\r\nERROR\r\n"

#define CMD_RESULT_CME_ERROR "\r\n+CME ERROR:"
#define CMD_RESULT_CMS_ERROR "\r\n+CMS ERROR:"
#define CMD_RESULT_NO_CARRIER "NO CARRIER"

#define SEND_AT_LOCK_FILE "/tmp/.sendat.lock"
#define CREATE_LOCK_FILE_CMD "touch "SEND_AT_LOCK_FILE
#define REMOVE_LOCK_FILE_CMD "rm -f "SEND_AT_LOCK_FILE
//no carrier
//#define CMD_RESULT_NO_CARRIER "\r\nNO CARRIER\r\n"

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


int config_device_mode(int dev_handle,int baud_rate,int databits, int stopbits, int parity)
{
	int baud_rate_value=util_get_baud_rate_value( baud_rate );
	struct  termios opt;
	

	if(tcgetattr(dev_handle, &opt) != 0)
	{
		return FALSE;
	}

	opt.c_cflag |= (CLOCAL | CREAD);

	tcflush(dev_handle,TCIFLUSH);
	cfsetispeed(&opt,( speed_t )baud_rate_value);
	cfsetospeed(&opt,( speed_t )baud_rate_value);

	switch(databits)
	{
		case 7:
			opt.c_cflag &= ~CSIZE;
			opt.c_cflag |= CS7;
			break;
		case 8:
			opt.c_cflag &= ~CSIZE;
			opt.c_cflag |= CS8;
			break;
		default:
			return FALSE;
	}

	switch(stopbits)
	{
		case 1:
			opt.c_cflag &= ~CSTOPB;
			break;
		case 2:
			opt.c_cflag |= CSTOPB;
			break;
		default:
			return FALSE;
	}

	switch(parity)
	{
		case 'n':
		case 'N':
			opt.c_cflag &= ~PARENB;		//clear the checking bit
			opt.c_iflag &= ~INPCK;		//enable parity checking
			break;
		case 'o':
		case 'O':
			opt.c_cflag |= PARENB;		//enable parity
			opt.c_cflag |= PARODD;		//odd check
			opt.c_iflag |= INPCK;		//disable parity checking
			break;
		case 'e':
		case 'E':
			opt.c_cflag |= PARENB;		//enable parity
			opt.c_cflag &= ~PARODD;		//even check
			opt.c_iflag |= INPCK;		//disable pairty checking
			break;
		case 's':
		case 'S':
			opt.c_cflag &= ~PARENB;		//clear the checking bit
			opt.c_cflag &= ~CSTOPB;		//
			opt.c_iflag |= INPCK;		//disable pairty checking
			break;
		default:
			return FALSE;
	}

	opt.c_cflag |= (CLOCAL | CREAD);
	opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	opt.c_oflag &= ~OPOST;
	opt.c_oflag &= ~(ONLCR | OCRNL);
	opt.c_iflag &= ~(ICRNL | INLCR);
	opt.c_iflag &= ~(IXON | IXOFF | IXANY);

	tcflush(dev_handle, TCIFLUSH);
	opt.c_cc[VTIME] = 0;		//timeout 15sec
	opt.c_cc[VMIN] = 0;			//Update the Opt and do it now

	if (tcsetattr(dev_handle,TCSANOW,&opt) != 0)
	{
	   close(dev_handle);
	   return FALSE;
	}

	return TRUE;
}

int f;
char *g_result = NULL;
void thr_recv(void *arg)
{
	log_info("in thr_recv\n");
	int bytes_n=0;
	fd_set readfds;
	int maxfd;
	
	char buffer_output[1024] = {0};
	struct timeval tv;
	int retry_count = 10;
	if(!g_result)
	{
		log_error("error result");
		return ;
	}
	memset(buffer_output,0,1024);
	while(1)
	{	
		tv.tv_sec=6;
		tv.tv_usec=0;
		FD_ZERO(&readfds);
		FD_SET(f,&readfds);
		maxfd=f+1;
		if(select(maxfd,&readfds,NULL,NULL,&tv))
			bytes_n+=read(f,buffer_output+bytes_n,sizeof(buffer_output)-bytes_n);
	
			if(NULL!=strstr(buffer_output,CMD_EXE_OK) ||
				NULL!=strstr(buffer_output,CMD_EXE_ERROR) || 
				NULL!=strstr(buffer_output,CMD_RESULT_CME_ERROR) || 
				NULL!=strstr(buffer_output,CMD_RESULT_CMS_ERROR) ||
				NULL!=strstr(buffer_output,CMD_RESULT_NO_CARRIER))
			{
				log_info("%s",buffer_output);
				strcpy(g_result, buffer_output);
				fsync(f);
				break;
			}
			else
				retry_count--;

			if(retry_count <= 0)
			{
				log_info("TIMEOUT\n");
				strcpy(buffer_output,"TIMEOUT");
				break;
			}
			
		
	}
}

int sendat(char *at, char *result)
{
	pthread_t thread_recv;
	unsigned int baudrate=115200;
	//signal(SIGINT,sig_int);
	//buffer_output = result;
	g_result  =  result;

	while(check_file_exist(SEND_AT_LOCK_FILE)) 
	{
		log_info("waiting serial\n");
		sleep(1);
	}
	system(CREATE_LOCK_FILE_CMD);
	
	
	f=open("/dev/ttyUSB1",O_RDWR);
	if(-1 == f)
	{
		log_error("open serial port error!\n");
		goto ERROR;
	}
	
	config_device_mode(f,baudrate,8,1,'s');	
   if(pthread_create(&thread_recv,NULL,(void *)thr_recv,NULL) != 0)
	{
		log_error("thr_recv create error\n");
		goto ERROR;                                                          
	}
	
	if(NULL==strchr(at,'\r'))
	{
		at[strlen(at)]='\r';
	}
	write(f,at,strlen(at));
	fsync(f);
	pthread_join(thread_recv,NULL);
	close(f);
	system(REMOVE_LOCK_FILE_CMD);
	return 0;
ERROR:
	system(REMOVE_LOCK_FILE_CMD);
	return 1;
	
}

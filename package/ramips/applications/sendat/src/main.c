#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>

#include "log.h"
#include "serial.h"

#define LOG_FILE "/tmp/at_log"
#define TRUE 1
#define FALSE 0

#define CMD_EXE_OK "\r\nOK\r\n"
#define CMD_EXE_ERROR "\r\nERROR\r\n"

#define CMD_RESULT_CME_ERROR "\r\n+CME ERROR:"
#define CMD_RESULT_CMS_ERROR "\r\n+CMS ERROR:"
//no carrier
//#define CMD_RESULT_NO_CARRIER "\r\nNO CARRIER\r\n"

#if 0
int sig_int_flag=0;

void sig_int()
{
	sig_int(SIGINT,sig_int);
	sig_int_flag=1;
}
#endif
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
enum 
{
	mode_sendat,
	mode_terminal,
	mode_once,
}mode;

char at_send_file[128];
char at_recv_file[128];

void thr_recv(void *arg)
{
	int bytes_n=0;
	fd_set readfds;
	int maxfd;
	char buffer_output[1024]={0};
	struct timeval tv;
	FILE* f_recv;

	while(1)
	{
		tv.tv_sec=6;
		tv.tv_usec=0;
		FD_ZERO(&readfds);
		FD_SET(f,&readfds);
		maxfd=f+1;
		if(select(maxfd,&readfds,NULL,NULL,&tv))
			bytes_n+=read(f,buffer_output+bytes_n,sizeof(buffer_output)-bytes_n);
	
		//printf("%s",buffer_output);
		if(mode==mode_sendat)
		{
			if(NULL!=strstr(buffer_output,CMD_EXE_OK) ||
				NULL!=strstr(buffer_output,CMD_EXE_ERROR) || 
				NULL!=strstr(buffer_output,CMD_RESULT_CME_ERROR) || 
				NULL!=strstr(buffer_output,CMD_RESULT_CMS_ERROR) )
			{
				f_recv=fopen(at_recv_file,"w+");
				if(NULL!=f_recv)
				{
					fputs(buffer_output,f_recv);
					fclose(f_recv);
					break;
				}
			}
		}
		else if(mode_terminal == mode)
		{
			printf("%s",buffer_output);
			bytes_n=0;
			memset(buffer_output,0,sizeof(buffer_output));
			fsync(f);
		}
		else 
		{
			printf("%s",buffer_output);
			fsync(f);
			exit(0);
		}
		
	}
}

int main(int argc,char *argv[])
{
	char buffer_input[512]={0};
	char serial_port[32]={0};
	pthread_t thread_recv;
	FILE* f_sendat;
	unsigned int baudrate=115200;
//	int n=0;
	int ret;
	mode=mode_sendat;
	while((ret= getopt(argc,argv,"e:d:f:o:t")) != -1)
	{
		switch(ret)
		{
			case 'd':
				strncpy(serial_port,optarg,sizeof(serial_port));
				break;
			case 'f':
				strncpy(at_send_file,optarg,sizeof(at_send_file));
				break;
			case 'o':
				strncpy(at_recv_file,optarg,sizeof(at_recv_file));
				break;
			case 't':
				mode=mode_terminal;
				break;
			case 'e':
				mode=mode_once;
				strcpy(buffer_input,optarg);
				break;
			default:
				printf("error");
				exit(-1);
		}
	}
	
	if(mode==mode_sendat)
	{
		if(argc!=4)
		{
			printf("sendat -d/dev/ttyUSB1 -f/tmp/at_send -o /tmp/at_recv\n");
			exit(1);
		}
	}
	else if(mode_terminal == mode)
	{
		if(argc!=3)
		{
			printf("sendat -d/dev/ttyUSB1 -t\n");
			exit(1);
		}
	}
	else if(mode_once == mode)
	{
		if(argc != 3 && argc != 4)
		{
			printf("sendat -d/dev/ttyUSB1 -e at-cmd OR sendat -e at-cmd \n");
			exit(1);
		}
	}
	else
	{
		printf("please special the mode(t:f:e)\n");
		exit(1);
	}
	
	f=open(serial_port,O_RDWR);
	if(-1 == f)
	{
		strncpy(serial_port, "/dev/ttyUSB3",sizeof(serial_port));
		f=open(serial_port,O_RDWR);
	}
	if(-1 == f)
	{
		printf("open serial port error!");
		return 1;
	}
		
	config_device_mode(f,baudrate,8,1,'s');	
//	signal(SIGINT,sig_int);
    if(pthread_create(&thread_recv,NULL,(void *)thr_recv,NULL) != 0)
	{
		log_error("thr_recv create error\n");
		return -1;                                                          
	}

/* the mode for sendat ,just compatible for old sendat
 */
	if(mode==mode_sendat)
	{
		f_sendat=fopen(at_send_file,"r");
		if(NULL!=f_sendat)
		{
			memset(buffer_input,0,sizeof(buffer_input));
			fgets(buffer_input,sizeof(buffer_input),f_sendat);
			if(NULL==strchr(buffer_input,'\r'))
			{
				buffer_input[strlen(buffer_input)-1]='\r';
			}
			printf("sendat:%s",buffer_input);
			write(f,buffer_input,strlen(buffer_input));
			fsync(f);
			fclose(f_sendat);
		}
		else
			return 1;
	}
/* the mode like terminator,can send at cmd like minicom
 */
	else if(mode_terminal == mode)
	{
		while(1)
		{
			memset(buffer_input,0,sizeof(buffer_input));
			scanf("%s",buffer_input);
			if(NULL==strchr(buffer_input,'\r'))
			{
				buffer_input[strlen(buffer_input)]='\r';
			}
			write(f,buffer_input,strlen(buffer_input));
			fsync(f);
		}
	}
	else
	{
		
		if(NULL==strchr(buffer_input,'\r'))
		{
			buffer_input[strlen(buffer_input)]='\r';
		}
		write(f,buffer_input,strlen(buffer_input));
		fsync(f);
		pthread_join(thread_recv,NULL);
		close(f);
		return 0;
	}
	
	pthread_join(thread_recv,NULL);
	close(f);
	return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "serial.h"
#include "logger.h"
#include "common.h"

static BaudRatePair baud_rate_pairs[]=
{
	{50,B50},
	{75,B75},
	{110,B110},
	{134,B134},
	{150,B150},
	{200,B200},
	{300,B300},
	{600,B600},
	{1200,B1200},
	{1800,B1800},
	{2400,B2400},
	{4800,B4800},
	{9600,B9600},
	{19200,B19200},
	{38400,B38400},
	{57600,B57600},
	{115200,B115200},
	{230400,B230400},
	{460800,B460800},
	{500000,B500000},
	{576000,B576000},
	{921600,B921600},
	{1000000,B1000000},
	{1152000,B1152000},
	{1500000,B1500000},
	{2000000,B2000000},
	{2500000,B2500000},
	{3000000,B3000000},
	{3500000,B3500000},
	{4000000,B4000000},
};

int util_get_baud_rate_value(int baud_rate)
{
	int begin=0;
	int end=( int )ARRAY_MEMBER_COUNT(baud_rate_pairs)-1;
	int middle;

	while(begin <= end)
	{
		middle=(begin+end)>>1;

		if(baud_rate_pairs[middle].baud_rate == baud_rate)
		{
			return baud_rate_pairs[middle].value;
		}
		else if(baud_rate_pairs[middle].baud_rate < baud_rate)
		{
			begin=middle+1;
		}
		else
		{
			end=middle-1;
		}
	}

	return -1;
}


//open the serial,COMx, /dev/ttyUSBx
int util_serial_open(char* serial_port)
{
	#ifdef WIN32
	
	int ret=0;
	int serial_number=atoi( serial_port+strlen("COM") );
	ret=sio_open( serial_number );
	if ( ret != SIO_OK ) 
	{
		debug("sio_open error number:%d!",ret);
		return -1;
	}

	return serial_number;
	
	#else
	
	return open( serial_port,O_RDWR );
	
	#endif
}

int config_device_mode(int dev_handle,int baud_rate,int databits, int stopbits, int parity)
{
	int baud_rate_value=util_get_baud_rate_value( baud_rate );
	struct  termios opt;
	

	if(tcgetattr(dev_handle, &opt) != 0)
	{
		log_error("tcgetattr fail\n");
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
			log_error("Unsupported data size.\n");
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
			log_error("Unsupported stopbits.\n");
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
			log_error("Unsupported parity.\n");
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
		log_error("Setup Serial fail!\n");
		close(dev_handle);
		return FALSE;
	}

	return TRUE;
}

#if 0
DialProc* util_get_module_handle(char *module_name)
{
	int index=0;
	while( index < ( int )ARRAY_MEMBER_COUNT( all_dial_modules ) )
	{
		if( strcmp( all_dial_modules[index]->module_name,module_name ) == 0 )
		{
			return all_dial_modules[index]->process_functions;
		}

		index+=1;
	}

	return NULL;
}
#endif

#ifndef __SERIAL_H__
#define __SERIAL_H__
typedef struct
{
	int baud_rate;
	int value;
}BaudRatePair;

#define ARRAY_LENGTH(ARRAY_NAME) ( sizeof( ARRAY_NAME )/sizeof( ARRAY_NAME[0] ) )

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
	int end=( int )ARRAY_LENGTH(baud_rate_pairs)-1;
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







#endif

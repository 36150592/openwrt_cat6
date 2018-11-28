#ifndef _SERIAL_H_
#define _SERIAL_H_


typedef struct
{
	int baud_rate;
	int value;
}BaudRatePair;


extern int util_get_baud_rate_value(int baud_rate);
extern int util_serial_open(char* serial_port);
extern int config_device_mode(int dev_handle,int baud_rate,int databits, int stopbits, int parity);





#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utility.h"



static void peripheral_test_handle(int fd, char* get_buf, char* set_buf)
{
	int wifi_button_status = 0;
	int restore_button_status = 0;
	int wps_button_status = 0;
	int lte_main_antenna_status = 0;
	int lte_vice_antenna_status = 0;

	system("killall -9 led_process");
	system("touch /tmp/.in_peripheral_test");
	system("/etc/tozed/peripheral_test_led.lua &");

	if((access("/sys/class/gpio/gpio4/value",F_OK))==-1)   
	{   
	    system("echo 3 > /sys/class/gpio/export");
		system("echo 4 > /sys/class/gpio/export");
		system("echo in > /sys/class/gpio/gpio3/direction");
		system("echo in > /sys/class/gpio/gpio4/direction");
	}

	char tmp_antenna_status[8] = "";
	

	char send_buf[128] = "";

	while(1)
	{
		usleep(50000);

		//wifi_button_status
		if((access("/tmp/.wifi_button_press",F_OK))!=-1)   
	    {   
	        wifi_button_status = 1;
	    }   
	    else  
	    {   
	        wifi_button_status = 0;   
	    }

	    //restore_button_status
		if((access("/tmp/.restore_button_press",F_OK))!=-1)   
	    {   
	        restore_button_status = 1;
	    }   
	    else  
	    {   
	        restore_button_status = 0;   
	    }

	    //wps_button_status
		if((access("/tmp/.wps_button_press",F_OK))!=-1)   
	    {   
	        wps_button_status = 1;
	    }   
	    else  
	    {   
	        wps_button_status = 0;   
	    }

	    //lte_main_antenna_status
	    memset(tmp_antenna_status, 0, sizeof(tmp_antenna_status));
	    read_memory("cat /sys/class/gpio/gpio4/value",tmp_antenna_status, sizeof(tmp_antenna_status));
	    if(tmp_antenna_status[0] == '1')
	    {
			lte_main_antenna_status = 1;
	    }
	    else
	    {
	    	lte_main_antenna_status = 0;
	    }

	    //lte_vice_antenna_status
	    memset(tmp_antenna_status, 0, sizeof(tmp_antenna_status));
	    read_memory("cat /sys/class/gpio/gpio3/value",tmp_antenna_status, sizeof(tmp_antenna_status));
	    if(tmp_antenna_status[0] == '1')
	    {
			lte_vice_antenna_status = 1;
	    }
	    else
	    {
	    	lte_vice_antenna_status = 0;
	    }


	    memset(send_buf, 0, sizeof(send_buf));

	    if(wifi_button_status)
	    	strcat(send_buf,"wifi_button_press,");
	    else
	    	strcat(send_buf,"wifi_button_off,");

	    if(restore_button_status)
	    	strcat(send_buf,"restore_button_press,");
	    else
	    	strcat(send_buf,"restore_button_off,");

	    if(wps_button_status)
	    	strcat(send_buf,"wps_button_press,");
	    else
	    	strcat(send_buf,"wps_button_off,");

	    if(lte_main_antenna_status)
	    	strcat(send_buf,"lte_main_antenna_on,");
	    else
	    	strcat(send_buf,"lte_main_antenna_off,");

	    if(lte_vice_antenna_status)
	    	strcat(send_buf,"lte_vice_antenna_on");
	    else
	    	strcat(send_buf,"lte_vice_antenna_off");


	    //sprintf(send_buf,"%d,%d,%d,%d,%d",wifi_button_status, restore_button_status, wps_button_status, lte_main_antenna_status, lte_vice_antenna_status );
	    send( fd,send_buf,strlen(send_buf),0 );
	}
}



int s21_precess(int fd, char* receive_buf, char* send_message)
{
	print("in s21_precess\n");
	if(strstr(receive_buf,"peripheral_test"))
		peripheral_test_handle(fd, receive_buf, send_message);
	else
		return FALSE;

	return TRUE;
}
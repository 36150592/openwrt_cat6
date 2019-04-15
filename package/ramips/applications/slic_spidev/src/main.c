

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <stdint.h>  
#include <unistd.h>  
#include <getopt.h>  
#include <fcntl.h>  
#include <sys/ioctl.h>  
#include <linux/types.h>  
#include "spidev.h"
#include "log.h"
#include "tz_custom_cfg.h"
#include "tz_base.h"
#include "tz_socket.h"
#include "typedefs.h"
#include "message.h"
#include "tzpriv.h"

#if 0
typedef unsigned char uint8_t;


void msleep_ext(unsigned int ms)
{
	int val = ms;
	struct timeval tmv;

	if(val < 1)
		val = 1;

	if(val > 1000)
		val = 1000;
		
    tmv.tv_sec = 0;
    tmv.tv_usec = val*1000;
	select(0, NULL, NULL, NULL, &tmv);
}

void umsleep_ext(int ms, int us)
{
	struct timeval tmv;

    tmv.tv_sec = 0;
    tmv.tv_usec = ms*1000 + us;
	select(0, NULL, NULL, NULL, &tmv);
}

  
#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0]))) 
  
static void pabort(const char *s)  
{  
    perror(s);  
    abort();  
}  
  
static const char *device = "/dev/spidev32766.1";  
static uint8_t mode=SPI_MODE_3;  
static uint8_t bits = 8;  
static uint32_t speed = 512000;  
static uint16_t delay=0;  

void transfer_write(int fd, uint8_t addr, uint8_t data)  
{  
	 int ret;  
	 uint8_t tx4[] = {	
		 0x20,12,0xbb
	 };   
	 
	 struct spi_ioc_transfer tr4[3] = 
	 {
		 {.tx_buf = (unsigned long)tx4,  
		 .len = ARRAY_SIZE(tx4),  
		 .delay_usecs = delay,	
		 .speed_hz = speed,  
		 .bits_per_word = bits,  
		 },
	 };  

	 tx4[1] = addr;
	 tx4[2] = data;
	 
	 ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr4[0]); 
	 if (ret < 1)  
		 pabort("can't send spi message ");	
	 
	 printf("write reg[%d]=0x%02x\n", addr, data);
}  

void transfer_read(int fd, uint8_t addr)  
{  
	 int ret;  
 
	 uint8_t tx5[] = {	
		 0x60,12,0x00
	 };  
	 
	 uint8_t rx4[3] = {0, }; 
	 struct spi_ioc_transfer tr4[3] = 
	 {	
		 {.tx_buf = (unsigned long)tx5,  
		 .len = ARRAY_SIZE(tx5),  
		 .delay_usecs = delay,	
		 .speed_hz = speed,  
		 .bits_per_word = bits,  
		 },
		 {.rx_buf = (unsigned long)rx4,  
		 .len = ARRAY_SIZE(rx4),  
		 .delay_usecs = delay,	
		 .speed_hz = speed,  
		 .bits_per_word = bits,  
		 }
	 };  

	 tx5[1]=addr;
	 ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr4); 
	 if (ret < 1)  
		 pabort("can't send spi message ");	
	 
	 printf("read reg[%d]=0x%02x\n", addr, rx4[2]);
}  

void transfer_mislic_init(int fd)  
{  
    int i, ret;  
    const uint8_t cmd_nop[] = {
        0x4a, 0x03, 0x06, 
        0x06, 0x06, 0x06, 0x06, 0x06,0x06, 0x06, 0x06, 
        0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06
    };   

    const uint8_t cmd_rst[] = {
        0x4a, 0x03, 0x04
    }; 

    const uint8_t cmd_read_id[] = {
        0x4a, 0x03, 0x73
    }; 
    
    uint8_t rx[ARRAY_SIZE(cmd_read_id)] = {0, };
    struct spi_ioc_transfer tr1[] = 
    { 
        {.tx_buf = (unsigned long)cmd_nop,  
        .len = ARRAY_SIZE(cmd_nop),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        },
        {.tx_buf = (unsigned long)cmd_rst,  
        .len = ARRAY_SIZE(cmd_rst),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        }
    };  

    struct spi_ioc_transfer tr2[] = 
    { 
        {.tx_buf = (unsigned long)cmd_read_id,  
        .rx_buf = (unsigned long)NULL,
        .len = ARRAY_SIZE(cmd_read_id),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        },
        {.tx_buf = (unsigned long)NULL,
        .rx_buf = (unsigned long)rx,  
        .len = ARRAY_SIZE(rx),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        }
    };  
    
    ret = ioctl(fd, SPI_IOC_MESSAGE(2), tr1);   
    if (ret < 1)  
		pabort("can't send spi message");  
    else
    {
		printf("transfer_mislic_init\r\n");  
		sleep(1);
			    
		ret = ioctl(fd, SPI_IOC_MESSAGE(2), tr2);   
		printf("read1 ret=%d \r\n", ret);  	
		for (i = 0; i < ARRAY_SIZE(rx); i++) 
			printf("%02x ", rx[i]);  
    }
    
    printf("\r\n");  
}  


void transfer_mislic_cmd_nop(int fd)  
{  
    int ret;  
    uint8_t cmd_nop[] = {
        0x4a, 0x03, 0x06, 
        0x06, 0x06, 0x06, 0x06, 0x06,0x06, 0x06, 0x06, 
        0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06
    };   
    
    struct spi_ioc_transfer tr[] = 
    { 
        {.tx_buf = (unsigned long)cmd_nop,  
        .len = ARRAY_SIZE(cmd_nop),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        }
    };  
    
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);   
    if (ret < 1)  
        pabort("can't send spi message");  
  	else
  	{
	  	printf("transfer_mislic_init cmd_nop\r\n");  
    }
}  

void transfer_mislic_cmd_rst(int fd)  
{  
    int ret;  
    uint8_t cmd_rst[] = {
        0x4a, 0x03, 0x04
    };   
    
    struct spi_ioc_transfer tr[] = 
    { 
        {.tx_buf = (unsigned long)cmd_rst,  
        .len = ARRAY_SIZE(cmd_rst),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        }
    };  
    
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);   
    if (ret < 1)  
        pabort("can't send spi message");  
  	else
  	{
	  	printf("transfer_mislic_init cmd_rst\r\n");  
    }
}  



void transfer_mislic_get_id(int fd)  
{  
    int i,ret;  
    uint8_t cmd_read_id[] = {
        0x4a, 0x03, 0x73
    }; 
    
    uint8_t rx[ARRAY_SIZE(cmd_read_id)] = {0, };
    struct spi_ioc_transfer tr2[] = 
    { 
        {.tx_buf = (unsigned long)cmd_read_id,  
        .len = ARRAY_SIZE(cmd_read_id),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        },
        {.rx_buf = (unsigned long)rx,  
        .len = ARRAY_SIZE(rx),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        }
    };  
    
    ret = ioctl(fd, SPI_IOC_MESSAGE(2), tr2);   
    if (ret < 1)  
        pabort("can't send spi message");  
  	else
  	{
	    for (i = 0; i < ARRAY_SIZE(rx); i++) { 
	        printf("%02x ", rx[i]);  
	    }  
    }
    
    printf("\r\n");  
}  


void init_mislic_and_get_id(void)
{
	int fd;
	int ret;
	
	fd = open(device, O_RDWR);
    if (fd < 0)  
        pabort("can't open device");  

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)  
        pabort("can't set spi mode");  
  
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode); 
    if (ret == -1)  
        pabort("can't get spi mode");      
        
   	//printf("spi mode: %d\n", mode); 
   	
   	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)  
        pabort("can't set max speed hz");  
  
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); 
    if (ret == -1)  
        pabort("can't get max speed hz");   

    uint8_t rx3[3] = {0,0,0}; 

	/*const uint8_t mislic_cmd_nop[] = {
        0x4a, 0x03, 0x06, 
        0x06, 0x06, 0x06, 0x06, 0x06,0x06, 0x06, 0x06, 
//        0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06
    };   

    const uint8_t mislic_cmd_rst[] = {
        0x4a, 0x03, 0x04
    }; */

    const uint8_t mislic_cmd_read_id[] = {
        0x4a, 0x03, 0x73
    }; 
    
    /*struct spi_ioc_transfer tr_mislic_init[] = 
    { 
        {.tx_buf = (unsigned long)mislic_cmd_nop,  
        .len = ARRAY_SIZE(mislic_cmd_nop),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        },
        {.tx_buf = (unsigned long)mislic_cmd_rst,  
        .len = ARRAY_SIZE(mislic_cmd_rst),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        }
    };  */

    struct spi_ioc_transfer tr_mislic_read_id[] = 
    { 
        {.tx_buf = (unsigned long)mislic_cmd_read_id,  
        .rx_buf = (unsigned long)NULL,
        .len = ARRAY_SIZE(mislic_cmd_read_id),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        },
        {.tx_buf = (unsigned long)NULL,
        .rx_buf = (unsigned long)rx3,  
        .len = ARRAY_SIZE(rx3),  
        .delay_usecs = delay,  
        .speed_hz = speed,  
        .bits_per_word = bits,  
        }
    };  

    printf("open %s ok\n", device);

    memset(rx3, 0, sizeof(rx3));
    /*ret = ioctl(fd, SPI_IOC_MESSAGE(2), tr_mislic_init);   
    if (ret < 1)  
    {
    	close(fd);
		pabort("can't send spi message[tr_mislic_init]");  
 	}
 	
	msleep_ext(50);*/
		    
	ret = ioctl(fd, SPI_IOC_MESSAGE(2), tr_mislic_read_id);   
	if (ret < 1)  
	{
    	close(fd);
		pabort("can't send spi message[tr_mislic_read_id]");  
 	}

    printf("get second type data %02x %02x %02x\n", rx3[0], rx3[1], rx3[2]);
    if(rx3[0]==0x08 && rx3[1]==0x49)
    {
		printf("get slic type is mislic\n");
    }
    else
    {
		printf("get slic type is none\n");
    }

    close(fd);
}


void init_mislic_and_get_id_all(void)
{
	int fd;
	int ret;
	
	fd = open(device, O_RDWR);
    if (fd < 0)  
        pabort("can't open device");  

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)  
        pabort("can't set spi mode");  
  
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode); 
    if (ret == -1)  
        pabort("can't get spi mode");      
        
   	//printf("spi mode: %d\n", mode); 
   	
   	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)  
        pabort("can't set max speed hz");  
  
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); 
    if (ret == -1)  
        pabort("can't get max speed hz");   

    uint8_t rx_buff[16]; 
    
	const uint8_t cmd1[] = {0x4a, 0x01, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06};
	const uint8_t cmd2[] = {0x06, 0x06, 0x06};
	const uint8_t cmd3[] = {0x4a, 0x03, 0x56, 0x0f, 0x06, 0x06, 0x06, 0x04};
    const uint8_t cmd4[] = {0x4a, 0x03, 0x73}; 

    struct spi_ioc_transfer spi_cmd_init[] = 
    {
	    {.tx_buf = (unsigned long)cmd1,  
	    .rx_buf = (unsigned long)NULL,
	    .len = sizeof(cmd1),  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    }, 
	    {.tx_buf = (unsigned long)cmd2,  
	    .rx_buf = (unsigned long)NULL,
	    .len = sizeof(cmd2),  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    },
	    
	    {.tx_buf = (unsigned long)cmd1,  
	    .rx_buf = (unsigned long)NULL,
	    .len = sizeof(cmd1),  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    },  
	    {.tx_buf = (unsigned long)cmd2,  
	    .rx_buf = (unsigned long)NULL,
	    .len = sizeof(cmd2),  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    },

	    {.tx_buf = (unsigned long)cmd3,  
	    .rx_buf = (unsigned long)NULL,
	    .len = sizeof(cmd3),  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    }
    };

    struct spi_ioc_transfer spi_cmd_read_id[] = 
    {
	    {.tx_buf = (unsigned long)cmd4,  
	    .rx_buf = (unsigned long)rx_buff,
	    .len = 2,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    }
	};

    printf("%s open %s ok\n", __FUNCTION__, device);

    ret = ioctl(fd, SPI_IOC_MESSAGE(5), spi_cmd_init);   
	printf("cmd1 rt=%d\n", ret);

	sleep(1);

	memset(rx_buff, 0, sizeof(rx_buff));
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), spi_cmd_read_id);   
	printf("cmd2 rt=%d\n", ret);

    printf("get second type data %02x %02x %02x\n", rx_buff[0], rx_buff[1], rx_buff[2]);
    if(rx_buff[0]==0x08 && rx_buff[1]==0x49)
    {
		printf("get slic type is mislic\n");
    }
    else
    {
		printf("get slic type is none\n");
    }

    close(fd);
}


void init_mislic_and_get_id_single(void)
{
	int fd;
	int ret;
	
	fd = open(device, O_RDWR);
    if (fd < 0)  
        pabort("can't open device");  

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)  
        pabort("can't set spi mode");  
  
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode); 
    if (ret == -1)  
        pabort("can't get spi mode");      
        
   	//printf("spi mode: %d\n", mode); 
   	
   	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)  
        pabort("can't set max speed hz");  
  
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); 
    if (ret == -1)  
        pabort("can't get max speed hz");   

    uint8_t rx_buff[16]; 
    
	const uint8_t cmd1[] = {0x4a, 0x01, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06};
	const uint8_t cmd2[] = {0x06, 0x06, 0x06};
	const uint8_t cmd3[] = {0x4a, 0x03, 0x56, 0x0f, 0x06, 0x06, 0x06, 0x04};
    const uint8_t cmd4[] = {0x4a, 0x03, 0x73}; 

    struct spi_ioc_transfer spi_cmd = 
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    };

    printf("%s open %s ok\n", __FUNCTION__, device);

	int i;
	for(i=0;i<2;i++)
	{
		spi_cmd.tx_buf = (unsigned long)cmd1;
		spi_cmd.rx_buf = (unsigned long)NULL;
		spi_cmd.len = ARRAY_SIZE(cmd1);
	    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd);   
		printf("cmd1 rt=%d\n", ret);
		spi_cmd.tx_buf = (unsigned long)cmd2;
		spi_cmd.rx_buf = (unsigned long)NULL;
		spi_cmd.len = ARRAY_SIZE(cmd2);
	    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd);   
		printf("cmd2 rt=%d\n", ret);
	}
	
	spi_cmd.tx_buf = (unsigned long)cmd3;
	spi_cmd.rx_buf = (unsigned long)NULL;
	spi_cmd.len = ARRAY_SIZE(cmd3);
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd);  
	printf("cmd3 rt=%d\n", ret);
    sleep(3);

	memset(rx_buff, 0, sizeof(rx_buff));
    spi_cmd.tx_buf = (unsigned long)cmd4;
	spi_cmd.rx_buf = (unsigned long)rx_buff;
	spi_cmd.len = ARRAY_SIZE(cmd4);
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd);  
	printf("cmd4 rt=%d\n", ret);

    printf("get second type data %02x %02x %02x\n", rx_buff[0], rx_buff[1], rx_buff[2]);
    if(rx_buff[0]==0x08 && rx_buff[1]==0x49)
    {
		printf("get slic type is mislic\n");
    }
    else
    {
		printf("get slic type is none\n");
    }

    close(fd);
}

void init_mislic_and_get_id_normal(void)
{
	int fd;
	int ret;
	
	fd = open(device, O_RDWR);
    if (fd < 0)  
        pabort("can't open device");  

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)  
        pabort("can't set spi mode");  
  
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode); 
    if (ret == -1)  
        pabort("can't get spi mode");      
        
   	//printf("spi mode: %d\n", mode); 
   	
   	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)  
        pabort("can't set max speed hz");  
  
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); 
    if (ret == -1)  
        pabort("can't get max speed hz");   

    uint8_t rx_buff[16]; 
    //uint8_t tx_buff[16]={0}; 
    
	const uint8_t cmd1[] = {0x4a, 0x01, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, };
	const uint8_t cmd2[] = {0x06, 0x06, 0x06};
	const uint8_t cmd3[] = {0x4a, 0x03, 0x56, 0x0f, 0x06, 0x06, 0x06, 0x04};
    const uint8_t cmd4[] = {0x4a, 0x03, 0x73}; 

    struct spi_ioc_transfer spi_cmd[] = 
    {
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    },
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    }
	};
	
    printf("%s open %s ok\n", __FUNCTION__, device);

	int i;
	for(i=0;i<2;i++)
	{
		spi_cmd[0].tx_buf = (unsigned long)cmd1;
		spi_cmd[0].rx_buf = (unsigned long)NULL;
		spi_cmd[0].len = ARRAY_SIZE(cmd1);
	    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd[0]);   
		printf("cmd1 rt=%d\n", ret);

		spi_cmd[0].tx_buf = (unsigned long)cmd2;
		spi_cmd[0].rx_buf = (unsigned long)NULL;
		spi_cmd[0].len = ARRAY_SIZE(cmd2);
	    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd[0]);   
		printf("cmd2 rt=%d\n", ret);

		//msleep_ext(1);
	}
	
	spi_cmd[0].tx_buf = (unsigned long)cmd3;
	spi_cmd[0].rx_buf = (unsigned long)NULL;
	spi_cmd[0].len = ARRAY_SIZE(cmd3);
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd[0]);  
	printf("cmd3 rt=%d\n", ret);

	//msleep_ext(5);

	memset(rx_buff, 0, sizeof(rx_buff));
    spi_cmd[0].tx_buf = (unsigned long)cmd4;
    spi_cmd[0].rx_buf = (unsigned long)NULL;
	spi_cmd[0].len = ARRAY_SIZE(cmd4);
	spi_cmd[1].tx_buf = (unsigned long)NULL;
	spi_cmd[1].rx_buf = (unsigned long)rx_buff;
	spi_cmd[1].len = 2;
    ret = ioctl(fd, SPI_IOC_MESSAGE(2), spi_cmd);  
	printf("cmd4 rt=%d\n", ret);

    printf("recv data %02x %02x %02x\n", rx_buff[0], rx_buff[1], rx_buff[2]);
    if(rx_buff[0]==0x08 && rx_buff[1]==0x49)
    {
		printf("get slic type is mislic\n");
    }
    else
    {
		printf("get slic type is none\n");
    }

    close(fd);
}

void reset_mislic_and_get_id_normal(void)
{
	int fd;
	int ret;
	
	fd = open(device, O_RDWR);
    if (fd < 0)  
        pabort("can't open device");  

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)  
        pabort("can't set spi mode");  
  
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode); 
    if (ret == -1)  
        pabort("can't get spi mode");      
        
   	//printf("spi mode: %d\n", mode); 
   	
   	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)  
        pabort("can't set max speed hz");  
  
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); 
    if (ret == -1)  
        pabort("can't get max speed hz");   

    uint8_t rx_buff[16]; 
    uint8_t tx_buff[16]={0}; 
    
	const uint8_t cmd3[] = {0x4a, 0x03, 0x56, 0x0f, 0x06, 0x06, 0x06, 0x04};
    const uint8_t cmd4[] = {0x4a, 0x03, 0x73}; 

    struct spi_ioc_transfer spi_cmd[] = 
    {
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    },
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    }
	};
	
    //printf("%s open %s ok\n", __FUNCTION__, device);
	
	spi_cmd[0].tx_buf = (unsigned long)cmd3;
	spi_cmd[0].rx_buf = (unsigned long)NULL;
	spi_cmd[0].len = ARRAY_SIZE(cmd3);
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd[0]);  
	//printf("cmd2 rt=%d\n", ret);

	memset(rx_buff, 0, sizeof(rx_buff));
	memset(tx_buff, 0, sizeof(tx_buff));
    spi_cmd[0].tx_buf = (unsigned long)cmd4;
    spi_cmd[0].rx_buf = (unsigned long)NULL;
	spi_cmd[0].len = ARRAY_SIZE(cmd4);
	spi_cmd[1].tx_buf = (unsigned long)NULL;
	spi_cmd[1].rx_buf = (unsigned long)rx_buff;
	spi_cmd[1].len = 2;
    ret = ioctl(fd, SPI_IOC_MESSAGE(2), spi_cmd);  
	//printf("cmd4 rt=%d\n", ret);
    printf("recv data %02x %02x %02x\n", rx_buff[0], rx_buff[1], rx_buff[2]);

    if(rx_buff[0]==0x08 && rx_buff[1]==0x49)
    {
		printf("get slic type is mislic\n");
    }
    else
    {
		printf("get slic type is none\n");
    }

    close(fd);
}

void mislic_get_id_loop(int num, int delayms, int delayus)
{
	int fd;
	int ret;
	
	fd = open(device, O_RDWR);
    if (fd < 0)  
        pabort("can't open device");  

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)  
        pabort("can't set spi mode");  
  
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode); 
    if (ret == -1)  
        pabort("can't get spi mode");      
        
   	//printf("spi mode: %d\n", mode); 
   	
   	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)  
        pabort("can't set max speed hz");  
  
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); 
    if (ret == -1)  
        pabort("can't get max speed hz");   

    printf("%s open %s ok\n", __FUNCTION__, device);

    uint8_t rx_buff[16]; 
    uint8_t dummy_tx[16]={0}; 
    
    const uint8_t cmd4[] = {0x4a, 0x03, 0x73}; 

    struct spi_ioc_transfer spi_cmd[] = 
    {
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    },
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    }
	};
	
    int i;
	int succeed_cnt = 0;
	for(i=0;i<num;i++)
	{
		memset(rx_buff, 0, sizeof(rx_buff));
	    spi_cmd[0].tx_buf = (unsigned long)cmd4;
	    spi_cmd[0].rx_buf = (unsigned long)NULL;
		spi_cmd[0].len = ARRAY_SIZE(cmd4);
		spi_cmd[1].tx_buf = (unsigned long)dummy_tx;
		spi_cmd[1].rx_buf = (unsigned long)rx_buff;
		spi_cmd[1].len = 2;
	    ret = ioctl(fd, SPI_IOC_MESSAGE(2), spi_cmd);  

		if(i<10 || (i%100==0))
		{
			printf("send=%02x %02x %02x\n", 
	    		cmd4[0], cmd4[1], cmd4[2]);
	    	printf("data=%02x %02x\nresult:%d/%d\n", 
	    		rx_buff[0], rx_buff[1], succeed_cnt, i);
		}
		
	    if(rx_buff[0]==0x08 && rx_buff[1]==0x49)
	    	succeed_cnt++;

	    umsleep_ext(delayms, delayus);
    }

    close(fd);

	printf("send=%02x %02x %02x\n", 
	    		cmd4[0], cmd4[1], cmd4[2]);
    printf("recv=%02x %02x %02x\nresult:%d/%d\n", 
	    		rx_buff[0], rx_buff[1], rx_buff[2], 
	    		succeed_cnt, i);

    printf("ok result: %d/%d\n", succeed_cnt, num);
}

void mislic_test_data_loop(int num, int delayms, int delayus)
{
	int fd;
	int ret;
	
	fd = open(device, O_RDWR);
    if (fd < 0)  
        pabort("can't open device");  

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)  
        pabort("can't set spi mode");  
  
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode); 
    if (ret == -1)  
        pabort("can't get spi mode");      
        
   	//printf("spi mode: %d\n", mode); 
   	
   	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)  
        pabort("can't set max speed hz");  
  
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); 
    if (ret == -1)  
        pabort("can't get max speed hz");   

    printf("%s open %s ok\n", __FUNCTION__, device);

    uint8_t rx_buff[16]; 
    
    const uint8_t cmd4[] = {0x33, 0x0f, 0x5a}; 

    struct spi_ioc_transfer spi_cmd[] = 
    {
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    },
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    }
	};
	
    int i;
	for(i=0;i<num;i++)
	{
		memset(rx_buff, 0, sizeof(rx_buff));
	    spi_cmd[0].tx_buf = (unsigned long)cmd4;
	    spi_cmd[0].rx_buf = (unsigned long)NULL;
		spi_cmd[0].len = ARRAY_SIZE(cmd4);
		spi_cmd[1].tx_buf = (unsigned long)NULL;
		spi_cmd[1].rx_buf = (unsigned long)rx_buff;
		spi_cmd[1].len = 2;
	    ret = ioctl(fd, SPI_IOC_MESSAGE(2), spi_cmd);  

		if(i<5 || (i%100==0))
		{
			printf("send=%02x %02x %02x\n", 
	    		cmd4[0], cmd4[1], cmd4[2]);
	    	printf("recv=%02x %02x %02x\nloop:%d\n", 
	    		rx_buff[0], rx_buff[1], rx_buff[2], i);
		}

	    umsleep_ext(delayms, delayus);
    }

    close(fd);

    printf("result num: %d\n", num);
}

void mislic_get_id_simu(void)
{
	int fd;
	int ret;
	
	fd = open(device, O_RDWR);
    if (fd < 0)  
        pabort("can't open device");  

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)  
        pabort("can't set spi mode");  
  
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode); 
    if (ret == -1)  
        pabort("can't get spi mode");      
        
   	//printf("spi mode: %d\n", mode); 
   	
   	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)  
        pabort("can't set max speed hz");  
  
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); 
    if (ret == -1)  
        pabort("can't get max speed hz");   

    uint8_t rx_buff[16]; 
    uint8_t dummy_tx[16]={0}; 
    
	
    const uint8_t cmd1[] = {0x4a, 0x03, 0x73}; 
    //const uint8_t cmd2[] = {0x4a, 0x03, 0xb9}; 

    struct spi_ioc_transfer spi_cmd= 
    {.tx_buf = (unsigned long)NULL,  
    .rx_buf = (unsigned long)NULL,
    .len = 0,  
    .delay_usecs = delay,  
    .speed_hz = speed,  
    .bits_per_word = bits,  
    };
	
    printf("%s open %s ok\n", __FUNCTION__, device);
	
    spi_cmd.tx_buf = (unsigned long)cmd1;
    spi_cmd.rx_buf = (unsigned long)NULL;
	spi_cmd.len = ARRAY_SIZE(cmd1);
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd);  
	printf("cmd1.send rt=%d\n", ret);
	
	memset(rx_buff, 0, sizeof(rx_buff));
    spi_cmd.tx_buf = (unsigned long)dummy_tx;
    spi_cmd.rx_buf = (unsigned long)rx_buff;
	spi_cmd.len = 2;
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd);  
	printf("cmd1.recv rt=%d\n", ret);

    printf("recv data %02x %02x %02x\n", rx_buff[0], rx_buff[1], rx_buff[2]);

    if(rx_buff[0]==0x08 && rx_buff[1]==0x49)
    {
		printf("get slic type is mislic\n");
    }
    else
    {
		printf("get slic type is none\n");
    }

    close(fd);
}

// 4a,03,73 will shift right one clk, get error id
void init_mislic_and_get_id_in_single_rx(void)
{
	int fd;
	int ret;
	
	fd = open(device, O_RDWR);
    if (fd < 0)  
        pabort("can't open device");  

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)  
        pabort("can't set spi mode");  
  
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode); 
    if (ret == -1)  
        pabort("can't get spi mode");      
        
   	//printf("spi mode: %d\n", mode); 
   	
   	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)  
        pabort("can't set max speed hz");  
  
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); 
    if (ret == -1)  
        pabort("can't get max speed hz");   

    uint8_t rx_buff[16]; 
    //uint8_t tx_buff[16]={0}; 
    
	//const uint8_t cmd1[] = {0x4a, 0x01, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, };
	//const uint8_t cmd2[] = {0x06, 0x06, 0x06};
	const uint8_t cmd3[] = {0x4a, 0x03, 0x56, 0x0f, 0x06, 0x06, 0x06, 0x04};
    const uint8_t cmd4[] = {0x4a, 0x03, 0x73}; 

    struct spi_ioc_transfer spi_cmd[] = 
    {
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    },
	    {.tx_buf = (unsigned long)NULL,  
	    .rx_buf = (unsigned long)NULL,
	    .len = 0,  
	    .delay_usecs = delay,  
	    .speed_hz = speed,  
	    .bits_per_word = bits,  
	    }
	};
	
    //printf("%s open %s ok\n", __FUNCTION__, device);

	/*int i;
	for(i=0;i<2;i++)
	{
		spi_cmd[0].tx_buf = (unsigned long)cmd1;
		spi_cmd[0].rx_buf = (unsigned long)NULL;
		spi_cmd[0].len = ARRAY_SIZE(cmd1);
	    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd[0]);   
		//printf("cmd1 rt=%d\n", ret);

		spi_cmd[0].tx_buf = (unsigned long)cmd2;
		spi_cmd[0].rx_buf = (unsigned long)NULL;
		spi_cmd[0].len = ARRAY_SIZE(cmd2);
	    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd[0]);   
		//printf("cmd2 rt=%d\n", ret);

		msleep_ext(1);
	}*/
	
	spi_cmd[0].tx_buf = (unsigned long)cmd3;
	spi_cmd[0].rx_buf = (unsigned long)NULL;
	spi_cmd[0].len = ARRAY_SIZE(cmd3);
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd[0]);  
	//printf("cmd3 rt=%d\n", ret);

    spi_cmd[0].tx_buf = (unsigned long)cmd4;
    spi_cmd[0].rx_buf = (unsigned long)NULL;
	spi_cmd[0].len = ARRAY_SIZE(cmd4);
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd[0]);  
	//printf("cmd41 rt=%d\n", ret);
	
	memset(rx_buff, 0, sizeof(rx_buff));
	spi_cmd[1].tx_buf = (unsigned long)NULL;
	spi_cmd[1].rx_buf = (unsigned long)rx_buff;
	spi_cmd[1].len = 2;
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_cmd[1]);  
	//printf("cmd42 rt=%d\n", ret);

    printf("recv data %02x %02x %02x\n", rx_buff[0], rx_buff[1], rx_buff[2]);
    if(rx_buff[0]==0x08 && rx_buff[1]==0x49)
    {
		printf("get slic type is mislic\n");
    }
    else
    {
		printf("get slic type is none\n");
    }

    close(fd);
}


int spidev_main(int argc, char *argv[])  
{
	//printf("built slic at %s %s\n", __DATE__, __TIME__);

	if(argc==1)
		init_mislic_and_get_id();
	else
	{
		switch(atoi(argv[1]))
		{
		case 0:
			reset_mislic_and_get_id_normal();	// ok
			break;
		/*case 1:	
			mislic_get_id_simu();
			break;
		case 2:
			mislic_get_id_loop(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
			break;*/
		case 3:
			init_mislic_and_get_id_normal();	// ok
			break;
		case 4:
			init_mislic_and_get_id_in_single_rx();	// error, cmd4 shift right 1 clk
			break;
		default:
			break;
		}
	}
	return 0;
}
#endif

#define FILE_FLAG_INIT_GPIO32 "/tmp/.flag.init.gpio32"

int slic_to_tzphone_msg_handler(char *app_name, int msgId, char *msgInfo)
{	
	char msg_info[64];
	
	tz_base_dbg("slic_to_tzphone_msg_handler app_name=%s id=%d msg=%s", app_name, msgId, msgInfo);
	if(strcmp(app_name, APP_NAME_RPOSLIC)!=0)
		return 0;

	switch(msgId)
	{
	case KEY_HOOKOFF:
		encode_data_to_sock_msg(APP_NAME_TZPHONE, MSG_SLIC_TONE_START, "0", msg_info);
		send_data_to_app_by_sock_path(APP_SOCK_PATH_RPOSLIC, msg_info, strlen(msg_info));
		break;
	case KEY_HOOKON:
		encode_data_to_sock_msg(APP_NAME_TZPHONE, MSG_SLIC_TONE_STOP, NULL, msg_info);
		send_data_to_app_by_sock_path(APP_SOCK_PATH_RPOSLIC, msg_info, strlen(msg_info));
		break;
	default:
		break;
	}
	return 1;
}

void start_tzphone(void)
{
	tSOCK_SRV_PARAMS srv;
	char msg_buff[64];
	
	strcpy(srv.sock_path, APP_SOCK_PATH_TZPHONE);
	srv.cb = slic_to_tzphone_msg_handler;
	if(create_socket_srv_thread(&srv)==0)
	{
		tz_base_dbg("create_socket_srv_thread fail\n");
		return ;
	}

	encode_data_to_sock_msg(APP_NAME_TZPHONE, MSG_SLIC_RING_START, "0123456789", msg_buff);
	send_data_to_app_by_sock_path(APP_SOCK_PATH_RPOSLIC, msg_buff, strlen(msg_buff));
}

int main(int argc, char *argv[])  
{
	char temp[100];
	const char *INIT_GPIO32_TO_LTE_CMD = "cd /sys/class/gpio && echo 32 > export && cd gpio32 && echo out > direction && echo 1 > value";

	if(access(FILE_FLAG_INIT_GPIO32, F_OK)!=0)
	{
		system(INIT_GPIO32_TO_LTE_CMD);

		sprintf(temp, "touch %s", FILE_FLAG_INIT_GPIO32);
		system(temp);
	}

	start_tzphone();
	while(1)
	{
		sleep(1);
	}
	return 0;
}

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <generated/autoconf.h>

#define MTD_FACTORY     "/dev/mtd2"
#if defined (CONFIG_RALINK_RT6855A) || defined (CONFIG_RALINK_MT7621) || defined (CONFIG_SOC_MT7621)
#define LAN_OFFSET    0xE000
#define WAN_OFFSET    0xE006
#else
#define LAN_OFFSET    0x28
#define WAN_OFFSET    0x2E
//#define WLAN_OFFSET   0x04
#endif

#define WLAN_OFFSET   0x04
#define WLAN_5g_OFFSET 0x8004
#define MACADDR_LEN     6
#define WIFIRF_LEN  512

//tozed_param
#define FACTORY_IMEI 0xe010
#define FACTORY_SN	 0x200
#define SN_LENGTH 32
#define MEMGETINFO  _IOR('M', 1, struct mtd_info_user)
#define MEMERASE    _IOW('M', 2, struct erase_info_user)
#define MEMUNLOCK   _IOW('M', 6, struct erase_info_user)

struct erase_info_user
{
    unsigned int start;
    unsigned int length;
};

struct mtd_info_user
{
    unsigned char type;
    unsigned int flags;
    unsigned int size;
    unsigned int erasesize;
    unsigned int oobblock;
    unsigned int oobsize;
    unsigned int ecctype;
    unsigned int eccsize;
};

int mtd_read_own(int offset, int size)
{
    int fd = open(MTD_FACTORY, O_RDWR | O_SYNC);
    int i = 0;
    unsigned char *buf = NULL;

    if(fd < 0)
    {
        printf("Could not open mtd device: %s\n", MTD_FACTORY);
        return -1;
    }

	if(offset < 0 || size <= 0 )
	{
		printf("input error:offset >= 0 and size >0\n");
		return -1;
    }

	buf = (unsigned char *)malloc(size * sizeof(unsigned char));

	if(NULL == buf)
	{
		printf("malloc memory error\n");
		return -1;
	}
	
	lseek(fd, offset, SEEK_SET);
    if(read(fd, buf, size) != size)
    {
        printf("read() failed\n");
        close(fd);
        return -1;
    }
    for (i = 0; i < size; i++)
    {
        printf("%X ", buf[i]);
    }
	printf("\n");
    close(fd);
	free(buf);
    return 0;
}


int mtd_read(char *side)
{
    int fd = open(MTD_FACTORY, O_RDWR | O_SYNC);
    int i = 0;
    unsigned char mac_addr[MACADDR_LEN];

    if(fd < 0)
    {
        printf("Could not open mtd device: %s\n", MTD_FACTORY);
        return -1;
    }

    if (!strcmp(side, "wlan"))
        lseek(fd, WLAN_OFFSET, SEEK_SET);
    else if (!strcmp(side, "wan"))
        lseek(fd, WAN_OFFSET, SEEK_SET);
    else if (!strcmp(side, "lan"))
        lseek(fd, LAN_OFFSET, SEEK_SET);
	else
		lseek(fd, WLAN_5g_OFFSET, SEEK_SET);
    if(read(fd, mac_addr, MACADDR_LEN) != MACADDR_LEN)
    {
        printf("read() failed\n");
        close(fd);
        return -1;
    }
    for (i = 0; i < MACADDR_LEN; i++)
    {
        printf("%02X", mac_addr[i]);
        if (i < MACADDR_LEN-1)
            printf(":");
        else
            printf("\n");
    }
    close(fd);

    return 0;
}

void read_imei()
{
	int fd = open(MTD_FACTORY, O_RDWR | O_SYNC);
	int i = 0;
	unsigned char mac_addr[8];
	char output[128] = "";

	if(fd < 0)
	{
		printf("Could not open mtd device: %s\n", MTD_FACTORY);
		return -1;
	}

	lseek(fd, FACTORY_IMEI, SEEK_SET);

	if(read(fd, mac_addr, 8) != 8)
	{
		printf("read() failed\n");
		close(fd);
		return -1;
	}
	for (i = 0; i < 8; i++)
	{
		sprintf(output + i*2, "%02X", mac_addr[i]);
		if (i == 7)
		{
			output[15] =0;
			printf("%s\n",output);
		}
			
	}
	close(fd);
}

void read_sn()
{
	int fd = -1;
	int i = 0;
	unsigned char buf[SN_LENGTH] = {0};
	char output[SN_LENGTH+1] = "";
	
	fd = open(MTD_FACTORY, O_RDWR | O_SYNC);
	if(fd < 0)
	{
		printf("Could not open mtd device: %s\n", MTD_FACTORY);
		return ;
	}

	lseek(fd, FACTORY_SN, SEEK_SET);

	if(read(fd, buf, SN_LENGTH) != SN_LENGTH)
	{
		printf("read() failed\n");
		close(fd);
		return ;
	}

	if(buf[i] == 255)
	{
		close(fd);
		return;
	}
	
	for (i = 0; i < SN_LENGTH && buf[i] != 0; i++)
	{
		output[i] = buf[i];
	}
	output[i] = 0;
	printf("%s\n",output);
	close(fd);
}


mtd_read_tz_param(char *side)
{
	if (!strcmp(side, "imei"))
		read_imei();
	else if(!strcmp(side, "sn"))
		read_sn();


    return 0;
}


int mtd_write(char *side, char **value)
{
    int sz = 0;
    int i;
    struct mtd_info_user mtdInfo;
    struct erase_info_user mtdEraseInfo;
    int fd = open(MTD_FACTORY, O_RDWR | O_SYNC);
    unsigned char *buf, *ptr;
    if(fd < 0)
    {
        fprintf(stderr, "Could not open mtd device: %s\n", MTD_FACTORY);
        return -1;
    }
    if(ioctl(fd, MEMGETINFO, &mtdInfo))
    {
        fprintf(stderr, "Could not get MTD device info from %s\n", MTD_FACTORY);
        close(fd);
        return -1;
    }
    mtdEraseInfo.length = sz = mtdInfo.erasesize;
    buf = (unsigned char *)malloc(sz);
	if(NULL == buf){
		printf("Allocate memory for sz failed.\n");
		close(fd);
		return -1;        
	}
	if(read(fd, buf, sz) != sz){
        fprintf(stderr, "read() %s failed\n", MTD_FACTORY);
        goto write_fail;
    }
    mtdEraseInfo.start = 0x0;
    for (mtdEraseInfo.start; mtdEraseInfo.start < mtdInfo.size; mtdEraseInfo.start += mtdInfo.erasesize)
    {
        ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
        if(ioctl(fd, MEMERASE, &mtdEraseInfo))
        {
            fprintf(stderr, "Failed to erase block on %s at 0x%x\n", MTD_FACTORY, mtdEraseInfo.start);
            goto write_fail;
        }
    }
    if (!strcmp(side, "wlan"))
        ptr = buf + WLAN_OFFSET;
    else if (!strcmp(side, "wan"))
        ptr = buf + WAN_OFFSET;
    else if (!strcmp(side, "lan")) 
        ptr = buf + LAN_OFFSET;
	else
		ptr = buf + WLAN_5g_OFFSET;
    for (i = 0; i < MACADDR_LEN; i++, ptr++)
        *ptr = strtoul(value[i], NULL, 16);
    lseek(fd, 0, SEEK_SET);
    if (write(fd, buf, sz) != sz)
    {
        fprintf(stderr, "write() %s failed\n", MTD_FACTORY);
        goto write_fail;
    }

    close(fd);
        free(buf);
    return 0;
write_fail:
    close(fd);
    free(buf);
    return -1;
}

int mtd_write_own(int offset, char **value, int size)
{
    int sz = 0;
    int i;
    struct mtd_info_user mtdInfo;
    struct erase_info_user mtdEraseInfo;
    int fd = open(MTD_FACTORY, O_RDWR | O_SYNC);
    unsigned char *buf, *ptr;
    if(fd < 0)
    {
        fprintf(stderr, "Could not open mtd device: %s\n", MTD_FACTORY);
        return -1;
    }
	if(offset < 0 || size <= 0 )
	{
		printf("input error:offset >= 0 and size >0\n");
		return -1;
    }
    if(ioctl(fd, MEMGETINFO, &mtdInfo))
    {
        fprintf(stderr, "Could not get MTD device info from %s\n", MTD_FACTORY);
        close(fd);
        return -1;
    }
    mtdEraseInfo.length = sz = mtdInfo.erasesize;
    buf = (unsigned char *)malloc(sz);
	if(NULL == buf){
		printf("Allocate memory for sz failed.\n");
		close(fd);
		return -1;        
	}
	if(read(fd, buf, sz) != sz){
        fprintf(stderr, "read() %s failed\n", MTD_FACTORY);
        goto write_fail;
    }
    mtdEraseInfo.start = 0x0;
    for (mtdEraseInfo.start; mtdEraseInfo.start < mtdInfo.size; mtdEraseInfo.start += mtdInfo.erasesize)
    {
        ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
        if(ioctl(fd, MEMERASE, &mtdEraseInfo))
        {
            fprintf(stderr, "Failed to erase block on %s at 0x%x\n", MTD_FACTORY, mtdEraseInfo.start);
            goto write_fail;
        }
    }

	ptr = buf + offset;
    for (i = 0; i < size; i++, ptr++)
        *ptr = strtoul(value[i], NULL, 16);
    lseek(fd, 0, SEEK_SET);
    if (write(fd, buf, sz) != sz)
    {
        fprintf(stderr, "write() %s failed\n", MTD_FACTORY);
        goto write_fail;
    }

    close(fd);
    free(buf);
    return 0;
write_fail:
    close(fd);
    free(buf);
    return -1;
}

int write_imei(char *value)
{
		int sz = 0;
		int i;
		struct mtd_info_user mtdInfo;
		struct erase_info_user mtdEraseInfo;
		int fd = open(MTD_FACTORY, O_RDWR | O_SYNC);
		unsigned char *buf, *ptr;
		unsigned char imei_value[8];
	
		if(strlen(value) != 15)
		{
			fprintf(stderr, "The imei value's len is not 15\n");
				return -1;
		}
	
		for(i = 0; i < 15; i++)
		{
			if(value[i] < '0' || value[i] > '9')
			{
				fprintf(stderr, "The imei format is not right\n");
					return -1;
			}
		}
	
		for(i = 0; i < 8; i++)
		{
			int high = value[i*2] - '0';
			int low = 0;
			if(i == 7)
				low = 0;
			else
				low = value[i*2 +1] - '0';
			imei_value[i] = high * 16 + low;
		}
		
		if(fd < 0)
		{
			fprintf(stderr, "Could not open mtd device: %s\n", MTD_FACTORY);
			return -1;
		}
		if(ioctl(fd, MEMGETINFO, &mtdInfo))
		{
			fprintf(stderr, "Could not get MTD device info from %s\n", MTD_FACTORY);
			close(fd);
			return -1;
		}
		mtdEraseInfo.length = sz = mtdInfo.erasesize;
		buf = (unsigned char *)malloc(sz);
		if(NULL == buf){
			printf("Allocate memory for sz failed.\n");
			close(fd);
			return -1;		  
		}
		if(read(fd, buf, sz) != sz){
			fprintf(stderr, "read() %s failed\n", MTD_FACTORY);
			goto write_fail;
		}
		mtdEraseInfo.start = 0x0;
		for (mtdEraseInfo.start; mtdEraseInfo.start < mtdInfo.size; mtdEraseInfo.start += mtdInfo.erasesize)
		{
			ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
			if(ioctl(fd, MEMERASE, &mtdEraseInfo))
			{
				fprintf(stderr, "Failed to erase block on %s at 0x%x\n", MTD_FACTORY, mtdEraseInfo.start);
				goto write_fail;
			}
		}

		ptr = buf + FACTORY_IMEI;
		for (i = 0; i < 8; i++, ptr++)
			*ptr = imei_value[i];
		lseek(fd, 0, SEEK_SET);
		if (write(fd, buf, sz) != sz)
		{
			fprintf(stderr, "write() %s failed\n", MTD_FACTORY);
			goto write_fail;
		}
	
		close(fd);
			free(buf);
		return 0;
	write_fail:
		close(fd);
		free(buf);
		return -1;


}

int write_sn(char *sn_value)
{
		int sz = 0;
		int i;
		struct mtd_info_user mtdInfo;
		struct erase_info_user mtdEraseInfo;
		int fd = -1;
		unsigned char *buf, *ptr;

		fd = open(MTD_FACTORY, O_RDWR | O_SYNC);
		if(fd < 0)
		{
			fprintf(stderr, "Could not open mtd device: %s\n", MTD_FACTORY);
			return -1;
		}
		
		if(ioctl(fd, MEMGETINFO, &mtdInfo))
		{
			fprintf(stderr, "Could not get MTD device info from %s\n", MTD_FACTORY);
			close(fd);
			return -1;
		}
		
		mtdEraseInfo.length = sz = mtdInfo.erasesize;
		buf = (unsigned char *)malloc(sz);
		if(NULL == buf)
		{
			printf("Allocate memory for sz failed.\n");
			close(fd);
			return -1;		  
		}
		
		if(read(fd, buf, sz) != sz){
			fprintf(stderr, "read() %s failed\n", MTD_FACTORY);
			goto write_fail;
		}
		mtdEraseInfo.start = 0x0;
		for (mtdEraseInfo.start; mtdEraseInfo.start < mtdInfo.size; mtdEraseInfo.start += mtdInfo.erasesize)
		{
			ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
			if(ioctl(fd, MEMERASE, &mtdEraseInfo))
			{
				fprintf(stderr, "Failed to erase block on %s at 0x%x\n", MTD_FACTORY, mtdEraseInfo.start);
				goto write_fail;
			}
		}

		ptr = buf + FACTORY_SN;
		for (i = 0; i < SN_LENGTH && i < strlen(sn_value); i++, ptr++)
			*ptr = sn_value[i];
		if(i < SN_LENGTH)
			*ptr = 0;
		lseek(fd, 0, SEEK_SET);
		if (write(fd, buf, sz) != sz)
		{
			fprintf(stderr, "write() %s failed\n", MTD_FACTORY);
			goto write_fail;
		}
	
		close(fd);
			free(buf);
		return 0;
	write_fail:
		close(fd);
		free(buf);
		return -1;


}
int mtd_write_tozed_param(char *side, char *value)
{

		if (!strcmp(side, "imei"))
			return write_imei(value);
		else if (!strcmp(side, "sn"))
			return write_sn(value);
		else
		{
			return 0;
		}
}


void usage(char **str)
{
    printf("How to use:\n");
    printf("\tread:   %s r <wlan|lan|wan|wlan_5g>\n", str[0]);
    printf("\twrite:  %s w <wlan|lan|wan|wlan_5g> <MACADDR[0]> <MACADDR[1]> ...\n", str[0]);
	 printf("\tread tozed param:   %s g <imei|sn|...>\n", str[0]);
	 printf("\twrite tozed param:  %s s <imei|sn|...> <value> ...\n", str[0]);
	printf("\tread own:  %s p offset[hex] size\n", str[0]);
	printf("\twrite own: %s t offset[hex] size value[0] value[1] ...\n", str[0]);
}

int main(int argc,char **argv)
{
    char op;

    if (argc < 3)
        goto CmdFail;

    op = *(argv[1]);
    switch (op)
    {
        case 'r':
            if (mtd_read(argv[2]) < 0)
                goto Fail;
            break;
        case 'w':
            if (argc != 9)
                goto CmdFail;
            if (mtd_write(argv[2], argv+3) < 0)
                goto Fail;
            if (!strcmp(argv[2], "wlan"))
            {
                unsigned char value = 0 ;
                strcpy(argv[2], "lan");
                value = strtoul(argv[6], NULL, 16);
                sprintf(argv[6], "%02x", (unsigned char)(value+2));
                if (mtd_write(argv[2], argv+3) < 0)
                    goto Fail;
            }
            break;
		case 'p':
			 if (mtd_read_own(strtol(argv[2],NULL,16),atoi(argv[3])) < 0)
                goto Fail;
			break;
		case 't':

			if(argc < 5 || argc != atoi(argv[3]) + 4 )
				goto CmdFail;
			
			 if (mtd_write_own(strtol(argv[2],NULL,16), argv+4, atoi(argv[3])) < 0)
		case 'g':
			if (mtd_read_tz_param(argv[2]) < 0)
                		goto Fail;
			break;
		case 's':
			if(argc != 4)
				goto CmdFail;
			if (mtd_write_tozed_param(argv[2], argv[3]) < 0)
                		goto Fail;
			break;
        default:
            goto CmdFail;
    }

    return 0;
CmdFail:
    usage(argv);
Fail:
    return -1;
}


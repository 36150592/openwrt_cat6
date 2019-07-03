#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/if.h>
#include "Cshell.h"

#define NAME_OF_WIRELESS_INTERFACE "ra0"


static int get_interface_mac(char const* interface_name, unsigned char* mac_buf, int buf_size)
{
	struct ifreq interface;
	int fd=socket(AF_INET,SOCK_DGRAM,0);
	strncpy(interface.ifr_name, interface_name, sizeof(interface.ifr_name));
	if (ioctl(fd, SIOCGIFHWADDR, &interface) < 0) 
	{
		printf("[%s-%d]:ioctl(SIOCGIFHWADDR) failed!", __FUNCTION__, __LINE__);
		return -1;
	}
	close(fd);
	memcpy(mac_buf, interface.ifr_hwaddr.sa_data, buf_size);
	return 0;
}


static int w13_get_self_mac(char*  mac_buf, int buf_size)
{
	return get_interface_mac(NAME_OF_WIRELESS_INTERFACE, mac_buf, buf_size);
}

static void w13_enter_upgrade_status(void)
{
	shell_recv(NULL, 0, "/etc/init.d/tz_mgr stop");
	shell_recv(NULL, 0, "/etc/init.d/mini_httpd stop");//close web
	shell_recv(NULL, 0, "/etc/init.d/dnsmasq stop");//close dhcp
	return;
}


static void w13_set_led4ready(void)
{
	shell_recv(NULL, 0, "echo 1 > /sys/class/leds/deco\\:lte_r/brightness");//red led on
	return;
}



/*------------------以下是不同设备需要修改的接口------------------------------*/

//在创建原始套接字前调用，获取通讯用的mac地址，即发送搜索服务器等命令时的源mac地址,成功返回0，失败返回非0整数
int extern_get_self_mac(char* mac_buf, int buf_size)
{
	return w13_get_self_mac(mac_buf,buf_size);
}
//进入准备升级状态,在设置临时IP前调用，需要关闭可能影响设备IP、灯状态以及升级的进程。
void extern_enter_upgrade_status(void)
{
	w13_enter_upgrade_status();
	return;
}

//控灯，进入准备升级状态后调用，将设备指示灯设置为正常情况下不可能出现的逻辑，表示设备进入准备升级状态，方便生产人员识别
void extern_set_led4ready(void)
{
	w13_set_led4ready();
	return;
}


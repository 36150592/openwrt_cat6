

#ifndef __TZ_BASE_H__
#define __TZ_BASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h> 
#include <stdarg.h> 

#include <signal.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/wait.h>  

#include <linux/stat.h>  


#define tz_base_dbg_enable() 1

#define tz_base_dbg(fmt, ...) do{\
									if(tz_base_dbg_enable())\
										printf("[base]"fmt"\n",## __VA_ARGS__);\
									}while(0)
									
#define clear_compile_warning(x) x=x

typedef int (*msg_callback_ptr)(char *app_name, int msgId, char *msgInfo);
typedef void (*thread_callback_ptr)(void *param);

extern int get_app_pid_by_name(char *app_name);

#endif

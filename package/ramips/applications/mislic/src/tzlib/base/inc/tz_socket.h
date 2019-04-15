


#ifndef __TZ_SOCKET_H__
#define __TZ_SOCKET_H__

#include "tz_base.h"

#define tz_sock_dbg_enable() 0

#define tz_sock_dbg(fmt, ...) do{\
									if(tz_sock_dbg_enable())\
										printf("[sock]"fmt"\n",## __VA_ARGS__);\
									}while(0)

typedef struct{
	char sock_path[64];		// with path, ie: /tmp/sock.slic
	msg_callback_ptr cb;
}tSOCK_SRV_PARAMS;

extern int create_socket_srv_thread(tSOCK_SRV_PARAMS *srv);
extern int send_data_to_app_by_sock_path(char *sock_path, char *data, int data_size);

extern int encode_data_to_sock_msg(char *app_name, int msgId, char *data, char *sock_msg);
extern int decode_sock_msg_to_data(const char *raw_msg, char *app_name, int *msgId, char *data);



#endif

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stddef.h"
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>   /* The L2 protocols */
#include <signal.h>   // for signal()
#include <sys/time.h> // struct itimeral. Linux setitimer()
#include <syslog.h>
#include <errno.h>

#include <uci.h>


#include "tz_common.h"

static struct uci_context *uci = NULL;
static char *package = NULL;
static struct uci_package *uci_package;



int config_commit()
{
	struct uci_ptr ptr = { .package = package };
	if (uci_lookup_ptr(uci, &ptr, NULL, false))
		return -1;
	return uci_commit(uci, &ptr.p, false);
}

const char* config_get_string(const char *sec, const char *opt, const char *def)
{
	struct uci_ptr ptr = {
		.package = package,
		.section = sec,
		.option = opt
	};

	if (!uci_lookup_ptr(uci, &ptr, NULL, false) &&
		(ptr.flags & UCI_LOOKUP_COMPLETE)) {
		struct uci_element *e = ptr.last;
		if (e->type == UCI_TYPE_SECTION) {
			return uci_to_section(e)->type;
		} else if (e->type == UCI_TYPE_OPTION &&
			ptr.o->type == UCI_TYPE_STRING) {
			return ptr.o->v.string;
		}
	}
	
	return def;
}

int config_set_string(const char *sec, const char *opt, const char *val)
{
	struct uci_ptr ptr = {
		.package = package,
		.section = sec,
		.option = opt,
		.value = val
	};
	
	if (uci_lookup_ptr(uci, &ptr, NULL, false)) {
		return -1;
	}

	return uci_set(uci, &ptr);
}

void config_deinit()
{
	if (uci) {
		uci_free_context(uci);
		uci = NULL;
	}
	if (package) {
		free(package);
		package = NULL;
	}
}

int config_init(const char *name)
{
	if (!name && package)
		name = strdupa(package);

	config_deinit();

	if (!(package = strdup(name)) ||
		!(uci = uci_alloc_context()) ||
		uci_load(uci, package, &uci_package))
		return -1;
	
	return 0;
}

int file_exists(const char *file)
{
	struct stat s;
	int r = stat(file, &s);
	if (!r && S_ISREG(s.st_mode)) {
		return 0;
	} 
	
	return -1;
}





unsigned short ip_checksum(const void *data, int len)
{
	unsigned short checksum = 0;
	int left = len;
	const unsigned char *data_8 = data;

	while(left > 1)
	{
		checksum += ( ( *data_8 )<<8 )+( *(data_8+1) );
		data_8 += 2;
		left -= 2;
	}

	if(left)
	{
		checksum += ( ( *( unsigned char* )data_8 )<<8 );
	}

	while(checksum > 0xffff)
	{
		checksum = (checksum >> 16) + (checksum & 0xFFFF);
	}

	checksum = (~checksum) & 0xffff;

	return htons(checksum);
}





int util_receive_ethernet_frame(int socket_handle,unsigned char * ethernet_frame,int *recved_data_len)
{
	int data_len;
	unsigned char frame[ETH_FRAME_LEN];
	unsigned char frame_header_len=6+6+2+2;	// dst_mac[6]. src_mac[6]. eth[2]. len[2]

	*recved_data_len = 0;

	memset( frame,0,sizeof(frame) );

	//receive one frame
	data_len = recv(socket_handle,frame,ETH_FRAME_LEN,0);

	if( data_len == -1 )
	{
		return -1;
	}
	else if( data_len < frame_header_len )
	{
		return -1;
	}
	else
	{
		//check if the checksum is ok
		int data_len=ntohs( *( unsigned short* )(frame+OFFSET_OF_ETHERNET_LEN) );//>>6+6+2
		int checksum=ip_checksum(frame,data_len+OFFSET_OF_ETHERNET_DATA);
		//invalid checksum
		if( checksum )
		{
			printf("%s--%d:the checksum is not right!\n",__FUNCTION__, __LINE__);
			return -1;
		}

		*recved_data_len = data_len;
		memcpy(ethernet_frame,frame,data_len+OFFSET_OF_ETHERNET_DATA);
	}

	return 0;
}

//send ethernet frame
int util_send_ethernet_frame(
							int socket_handle
							,unsigned short ethernet_frame_type
							,unsigned char *ethernet_data
							,int ethernet_data_len
							,unsigned char* src_mac
							,unsigned char* dst_mac
							)
{
	unsigned short b16;
	int n;
	int real_frame_size;
	unsigned char *p_ethernet_frame;
	unsigned char ethernet_frame[ETH_FRAME_LEN];

	real_frame_size = 0;
	p_ethernet_frame = ethernet_frame;

	//destination mac
	util_add_data_to_frame(dst_mac,ETH_ALEN);
	//source mac
	util_add_data_to_frame(src_mac,ETH_ALEN);
	//data type in the ethernet frame
	b16 = htons( ethernet_frame_type );
	util_add_data_to_frame(&b16,sizeof(b16));

	p_ethernet_frame += LENGTH_OF_ETHERNET_LEN;// skip len

	//copy ethernet data
	memcpy(p_ethernet_frame,ethernet_data,ethernet_data_len);
	if( ethernet_data_len&0x01 )
	{
		p_ethernet_frame[ ethernet_data_len ]=0;
		ethernet_data_len++;
	}

	// dst_mac[6]. src_mac[6]. eth[2]
	p_ethernet_frame = ethernet_frame+OFFSET_OF_ETHERNET_LEN;
	// bytes checksum
	b16 = htons( ethernet_data_len+LENGTH_OF_ETHERNET_CHECKSUM );
	memcpy(p_ethernet_frame,&b16,sizeof(b16));

	real_frame_size=OFFSET_OF_ETHERNET_DATA+ethernet_data_len+LENGTH_OF_ETHERNET_CHECKSUM;

	//calculate the checksum
	b16=ip_checksum(ethernet_frame,real_frame_size-LENGTH_OF_ETHERNET_CHECKSUM);
	memcpy(ethernet_frame+real_frame_size-LENGTH_OF_ETHERNET_CHECKSUM,&b16,sizeof(b16));

	n = send(socket_handle,ethernet_frame,real_frame_size,0);

	if(n == -1)
	{
		return -1;
	}
	else if(n < real_frame_size)
	{
		return -1;
	}

	return 0;
}


int cmd_file_exist(const char* file_name)
{
#ifdef WIN32
	{
		WIN32_FIND_DATA wfd;
		HANDLE hFind=FindFirstFile( file_name,&wfd );
		//need create directory
		if( hFind == INVALID_HANDLE_VALUE )
		{
			return FALSE;
		}
	}
#else
	if( access( file_name,R_OK ) )
	{
		return FALSE;
	}
#endif
	return TRUE;
}




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utility.h"
#include <ctype.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define FALSE -1
#define TRUE 0

#define IPv4_EXIST_FLAG 0x01
#define IPv6_EXIST_FLAG 0x02



#define TOOL_RET "/tmp/.tozed_tool_ret"

int tz_ret(int num)
{
	char cmd[128] = "";
	if(num == 0)
	{
		sprintf(cmd, "echo success > %s", TOOL_RET);
	}
	else
	{
		sprintf(cmd, "echo \"fail:%d\" >%s",num, TOOL_RET);
	}
	system(cmd);
	return num;
}


int read_memory(char *shellcmd, char *out, int size) {

	FILE *stream;
	char buffer[size];
    memset(buffer, 0, sizeof(buffer));

	stream = popen(shellcmd, "r");
	if(stream != NULL){
        fread(buffer, sizeof(char), sizeof(buffer), stream);
        pclose(stream);

		memcpy(out, buffer, strlen(buffer) + 1);

		return 0;
	} else {
		out[0] = '\0';
	}

	return -1;
}

void util_strip_traling_spaces( char* one_string )
{
	char* tmp=one_string;
	int length=strlen(tmp);
	while(
			length
			&&(
				( tmp[ length-1 ] == '\r' )
				|| ( tmp[ length-1 ] == '\n' )
				|| ( tmp[ length-1 ] == '\t' )
				|| ( tmp[ length-1 ] == ' ' )
			)
		)
	{
		tmp[ length-1 ]=0;
		length--;
	}
}

void util_strip_traling_r_n_t( char* one_string )
{
	char* tmp=one_string;
	int length=strlen(tmp);
	while(
			length
			&&(
				( tmp[ length-1 ] == '\r' )
				|| ( tmp[ length-1 ] == '\n' )
				|| ( tmp[ length-1 ] == '\t' )
			)
		)
	{
		tmp[ length-1 ]=0;
		length--;
	}
}


void set_param_to_config_tozed(char* name, char* value)
{
	char cmd[256] = "";
	sprintf(cmd, "cfg -a %s=\"%s\"", name, value);
	print("will set %sK\n",cmd);
	system(cmd);
}


int util_resolv_domain_name(const char* domain_name,char* ipv4_addr,char* ipv6_addr,int* result_ind)
{
	struct addrinfo *answer,hints,*addr_info_p;
	int ret;
	struct sockaddr_in *sinp4;
	struct sockaddr_in6 *sinp6;
	char* addr;

	if( result_ind )
	{
		*result_ind=0;
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	/*
	 * The getaddrinfo() function allocates and initializes a linked list of addrinfo structures, one for each network address that matches node  and  service,  subject  to  any
       restrictions imposed by hints, and returns a pointer to the start of the list in res.  The items in the linked list are linked by the ai_next field.
	 * */
	ret = getaddrinfo(domain_name, NULL, &hints, &answer);
	if ( !ret )
	{
		for (addr_info_p = answer; addr_info_p != NULL; addr_info_p = addr_info_p->ai_next)
		{
			if(addr_info_p->ai_family == AF_INET)
			{
				sinp4 = (struct sockaddr_in *)addr_info_p->ai_addr;
				/*
				 * The  inet_ntoa()  function  converts  the Internet host address in, given in network byte order, to a string in IPv4 dotted-decimal notation.  The string is returned in a
       	   	   	   statically allocated buffer, which subsequent calls will overwrite.
				 * */
				addr = inet_ntoa( sinp4->sin_addr);
				#ifdef CMDLIB_TEST
				printf("ipv4 addr = %s\n", addr?addr:"unknow ");
				#endif

				if( ipv4_addr )
				{
					strcpy(ipv4_addr,addr);
				}

				if( result_ind )
				{
					*result_ind=(*result_ind)|IPv4_EXIST_FLAG;
				}
			}
			else if(addr_info_p->ai_family == AF_INET6)
			{
				sinp6 = (struct sockaddr_in6 *)addr_info_p->ai_addr;
				#ifdef CMDLIB_TEST
				int i;
				printf("ipv6 addr = ");
				for(i = 0; i < 16; i++)
				{
					if( ( !( i&0x01 ) ) && i )
					{
						printf(":");
					}
					printf("%02x",sinp6->sin6_addr.s6_addr[i]);
				}
				printf(" \n");
				#else
				if( ipv6_addr )
				{
					sprintf(ipv6_addr
							,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x"
							,sinp6->sin6_addr.s6_addr[0]
							,sinp6->sin6_addr.s6_addr[1]
							,sinp6->sin6_addr.s6_addr[2]
							,sinp6->sin6_addr.s6_addr[3]
							,sinp6->sin6_addr.s6_addr[4]
							,sinp6->sin6_addr.s6_addr[5]
							,sinp6->sin6_addr.s6_addr[6]
							,sinp6->sin6_addr.s6_addr[7]
							,sinp6->sin6_addr.s6_addr[8]
							,sinp6->sin6_addr.s6_addr[9]
							,sinp6->sin6_addr.s6_addr[10]
							,sinp6->sin6_addr.s6_addr[11]
							,sinp6->sin6_addr.s6_addr[12]
							,sinp6->sin6_addr.s6_addr[13]
							,sinp6->sin6_addr.s6_addr[14]
							,sinp6->sin6_addr.s6_addr[15]
							);
				}
				#endif

				if( result_ind )
				{
					*result_ind=(*result_ind)|IPv6_EXIST_FLAG;
				}
			}

			#ifdef CMDLIB_TEST
			printf(" \n");
			#endif

		}

		freeaddrinfo(answer);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}




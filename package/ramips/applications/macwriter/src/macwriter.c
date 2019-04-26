#include "cmdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/times.h>
#include <pthread.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <netdb.h>
#include "des3.h"
#include "des.h"
#include "utility.h"
#include "s21.h"

#ifndef __i386__
//WEXITSTATUS(status) 当WIFEXITED返回非零值时，我们可以用这个宏来提取子进程的返回值，如果子进程调用exit(5)退出，
//WEXITSTATUS(status)就会返回5；如果子进程调用exit(7)，WEXITSTATUS(status)就会返回7。请注意，如果进程不是正常退出的，
//也就是说，WIFEXITED返回0，这个值就毫无意义。
//#define	WEXITSTATUS(status)	( ( (status)&0xff00 )>>8 )
//WIFEXITED(status) 这个宏用来指出子进程是否为正常退出的，如果是，它会返回一个非零值。
//#define	WIFEXITED(status)	( ( (status)&0x7f ) == 0 )
#endif

#define SOCKET_BUFFER_SIZE 4096
#define SOCKET_LISTEN_PORT 3002

//本工具的设计目的是用于对系统进行升级自动从服务器上下载最新的软件版本,然后进行升级

//进行数据接收的buffer
static char socket_receive_buffer[ SOCKET_BUFFER_SIZE+SOCKET_BUFFER_SIZE ];
static unsigned char socket_receive_buffer_backup[ SOCKET_BUFFER_SIZE+SOCKET_BUFFER_SIZE ];
//进行数据发送的buffer
static char socket_send_buffer[ SOCKET_BUFFER_SIZE+SOCKET_BUFFER_SIZE ];


//device type
typedef enum{
	DEVICE_S21,
	DEVICE_NONE,
}DEVICE_TYPE;

int device_type;

//可用密钥的数目
#define AVAILABLE_KEY_COUNT 5


//初始密钥
static const unsigned char sys_initial_key[24]=
{
	0xbb, 0x71, 0x82, 0x73, 0xb4, 0x87, 0x14, 0x99,
	0x39, 0xe7, 0xdc, 0x13, 0x49, 0xd8, 0x8b, 0xdb,
	0xbb, 0x71, 0x82, 0x73, 0xb4, 0x87, 0x14, 0x99,
};

//读取到的系统运行时间,作为随机因子
static unsigned char random_calc_string[24];
//random_calc_string的hex形式
static char random_calc_string_hex[ ( sizeof(random_calc_string)<<1 )+1 ];

//获取随机因子的命令
#define RANDOM_SEED_COMMAND "$$get_random_seed$"

//初始化随机因子
void util_init_random_seed( void )
{
	char uptime_value[ 128 ];
	//mac地址
	char mac_addr[ 128 ];
	int dest_index=0;
	int orignial_index=0;
	int uptime_string_length;

	//void util_read_system_uptime( char* uptime_value )
	memset( uptime_value,0,sizeof( uptime_value ) );
	util_read_system_uptime( uptime_value );

	uptime_string_length=strlen( uptime_value );

	while( dest_index < sizeof( random_calc_string ) )
	{
		random_calc_string[ dest_index ]=uptime_value[ orignial_index ];
		dest_index+=1;
		orignial_index+=1;
		if( orignial_index >= uptime_string_length )
		{
			orignial_index-=uptime_string_length;
		}
	}

	//获取当前设备的MAC地址
	memset( mac_addr,0,sizeof( mac_addr ) );
#ifndef __i386__
	util_load_mac_addr( "br0",mac_addr );
#else
	util_load_mac_addr( "eth0",mac_addr );
#endif

	//使用MAC地址对密钥分散因子进行初始化
	dest_index=0;
	while( dest_index < sizeof( random_calc_string ) )
	{
		random_calc_string[ dest_index ]^=mac_addr[ dest_index ];
		dest_index+=1;
	}

	//优化随机因子的计算
	util_encrypt_file_content( random_calc_string,sizeof( random_calc_string ) );
}

//计算要使用的密钥索引
int util_get_used_key_index( void )
{
	unsigned int result=0;
	int dest_index=0;

	while( dest_index < sizeof( random_calc_string ) )
	{
		result+=random_calc_string[ dest_index ];
		dest_index+=1;
	}

	return ( result%AVAILABLE_KEY_COUNT );
}

//使用初始化随机因子作为密钥,计算新的3DES密钥
void util_get_new_random_seed( void )
{
	char buffer[ sizeof( sys_initial_key ) ];
	des3_encode((unsigned char *)sys_initial_key,
				(unsigned char *)random_calc_string,
				(unsigned char *)buffer,
				(unsigned int)sizeof( random_calc_string )
				);
	//复制新的密钥
	memcpy( random_calc_string,buffer,sizeof( random_calc_string ) );
}

//对命令进行解密
//成功返回0,失败返回其它
int util_decode_cmd_content_to_receive_buffer( unsigned char* cmd_buffer,int cmd_length )
{
	//首先使用密钥对内容进行解密
	memset( socket_send_buffer,0,sizeof( socket_send_buffer ) );

	//首先对内容进行解密
	des3_decode((unsigned char *)sys_initial_key,
					(unsigned char *)cmd_buffer,
					(unsigned char *)socket_send_buffer,
					cmd_length
					);

	//比较随机因子是否相同
	if( memcmp( socket_send_buffer,random_calc_string,sizeof( random_calc_string ) ) )
	{
		return -1;
	}

	//移出开始的校验因此
	memset( socket_receive_buffer,0,sizeof( socket_receive_buffer ) );
	strcpy( socket_receive_buffer,socket_send_buffer+sizeof( random_calc_string ) );

	//首先获取新的随机因子
	util_get_new_random_seed();

	return 0;
}

#define MAC_RESTORE_COMMAND "TZ_restore"

//执行用户发送的命令
void execute_cmd( int socket_fd,char* received_buffer )
{
	//执行用户发送的命令,并将命令的结果发送给用户
	FILE *stream;
	int error_code;

	int had_been_process = FALSE;

	memset( socket_send_buffer,0,sizeof(socket_send_buffer) );

	printf(received_buffer);

	//需要获取随机因子
	if( !strcmp( received_buffer,RANDOM_SEED_COMMAND ) )
	{
		//初始化随机因子
		util_init_random_seed();

		//将随机因子转换为hex格式
		memset( random_calc_string_hex,0,sizeof( random_calc_string_hex ) );
		util_hex_dump( random_calc_string,random_calc_string_hex,sizeof( random_calc_string ) );

		sprintf(socket_send_buffer
							,"\n"RANDOM_SEED_COMMAND":%s\r\nERR CODE:0\r\n"
							,random_calc_string_hex
							);
		send( socket_fd,socket_send_buffer,strlen(socket_send_buffer),0 );

		return;
	}

	//首先将hex格式的字符串转换为二进制形式
	memset( socket_receive_buffer_backup,0,sizeof( socket_receive_buffer_backup ) );
	util_hex_recovery( received_buffer,socket_receive_buffer_backup,strlen( received_buffer )>>1 );
	//对收到的内容进行解密
	if( util_decode_cmd_content_to_receive_buffer( socket_receive_buffer_backup,strlen( received_buffer )>>1 ) )
	{
		//解密失败
		strcpy(socket_send_buffer,"\nCan't Recognize this command!\r\nERR CODE:-1\r\n");
		send( socket_fd,socket_send_buffer,strlen(socket_send_buffer),0 );

		//出现了错误
		return;
	}
	else
	{
		received_buffer=socket_receive_buffer;
	}

	/*
	popen, pclose - pipe stream to or from a process
	#include <stdio.h>
	FILE *popen(const char *command, const char *type);
	int pclose(FILE *stream);
	The  popen()  function  opens  a process by creating a pipe, forking, and invoking the shell.  Since a pipe is by definition unidirectional, the type argument may specify
	only reading or writing, not both; the resulting stream is correspondingly read-only or write-only.

	The command argument is a pointer to a null-terminated string containing a shell command line.  This command is passed to /bin/sh using the -c  flag;  interpretation,  if
	any,  is  performed  by  the shell.  The type argument is a pointer to a null-terminated string which must contain either the letter 'r' for reading or the letter 'w' for
	writing.  Since glibc 2.9, this argument can additionally include the letter 'e', which causes the close-on-exec flag (FD_CLOEXEC)  to  be  set  on  the  underlying  file
	descriptor; see the description of the O_CLOEXEC flag in open(2) for reasons why this may be useful.

	The  return  value from popen() is a normal standard I/O stream in all respects save that it must be closed with pclose() rather than fclose(3).  Writing to such a stream
	writes to the standard input of the command; the command’s standard output is the same as that of the process that called popen(), unless this is altered by  the  command
	itself.   Conversely, reading from a "popened" stream reads the command’s standard output, and the command’s standard input is the same as that of the process that called
	popen().

	Note that output popen() streams are fully buffered by default.

	The pclose() function waits for the associated process to terminate and returns the exit status of the command as returned by wait4(2).

	RETURN VALUE
	The popen() function returns NULL if the fork(2) or pipe(2) calls fail, or if it cannot allocate memory.

	The pclose() function returns -1 if wait4(2) returns an error, or some other error is detected.
	*/
	//run reset do not in popen

	//check the device type
	/*
	if == s21
	in s21.c
	*/

	if(device_type == DEVICE_S21)
		had_been_process = s21_precess(received_buffer, socket_send_buffer);
	else
		had_been_process = FALSE;

	if(had_been_process == FALSE)
	{
		print("popen: %s\n\n",received_buffer);
		stream = popen(received_buffer, "r");
		if(stream != NULL)
		{
			memset( socket_send_buffer,0,sizeof( socket_send_buffer ) );
			fread( socket_send_buffer,sizeof(char),sizeof(socket_send_buffer),stream );
			error_code=pclose(stream);
			/*
			3、man中对于system的说明

			RETURN VALUE
				   The value returned is -1 on error (e.g.  fork() failed), and the return
				   status  of  the command otherwise.  This latter return status is in the
				   format specified in wait(2).  Thus, the exit code of the  command  will
				   be  WEXITSTATUS(status).   In  case  /bin/sh could not be executed, the
				   exit status will be that of a command that does exit(127).
			看得很晕吧？

			system函数对返回值的处理，涉及3个阶段：
			阶段1：创建子进程等准备工作。如果失败，返回-1。
			阶段2：调用/bin/sh拉起shell脚本，如果拉起失败或者shell未正常执行结束（参见备注1），原因值被写入到status的低8~15比特位中。system的man中只说明了会写了127这个值，但实测发现还会写126等值。
			阶段3：如果shell脚本正常执行结束，将shell返回值填到status的低8~15比特位中。
			备注1：
			只要能够调用到/bin/sh，并且执行shell过程中没有被其他信号异常中断，都算正常结束。
			比如：不管shell脚本中返回什么原因值，是0还是非0，都算正常执行结束。即使shell脚本不存在或没有执行权限，也都算正常执行结束。
			如果shell脚本执行过程中被强制kill掉等情况则算异常结束。

			如何判断阶段2中，shell脚本是否正常执行结束呢？系统提供了宏：WIFEXITED(status)。如果WIFEXITED(status)为真，则说明正常结束。
			如何取得阶段3中的shell返回值？你可以直接通过右移8bit来实现，但安全的做法是使用系统提供的宏：WEXITSTATUS(status)。


			由于我们一般在shell脚本中会通过返回值判断本脚本是否正常执行，如果成功返回0，失败返回正数。
			所以综上，判断一个system函数调用shell脚本是否正常结束的方法应该是如下3个条件同时成立：
			（1）-1 != status
			（2）WIFEXITED(status)为真
			（3）0 == WEXITSTATUS(status)

			注意：
			根据以上分析，当shell脚本不存在、没有执行权限等场景下时，以上前2个条件仍会成立，此时WEXITSTATUS(status)为127，126等数值。
			所以，我们在shell脚本中不能将127，126等数值定义为返回值，否则无法区分中是shell的返回值，还是调用shell脚本异常的原因值。shell脚本中的返回值最好多1开始递增。

			*/
			if( error_code == -1 )
			{
				sprintf(socket_send_buffer+strlen( socket_send_buffer )
						,"\n\r\nERR CODE:%d\r\n"
						,-1
						);
			}
			//命令非正常结束
			else if( !WIFEXITED( error_code ) )
			{
				sprintf(socket_send_buffer+strlen( socket_send_buffer )
						,"\n\r\nERR CODE:%d\r\n"
						,-1
						);
			}
			//WIFEXITED(status) 这个宏用来指出子进程是否为正常退出的，如果是，它会返回一个非零值。
			//进程正常退出
			else if( WIFEXITED( error_code ) )
			{
				//返回结果码,返回0,执行结束,返回其他,执行失败
				sprintf(socket_send_buffer+strlen( socket_send_buffer )
						,"\n\r\nERR CODE:%d\r\n"
						,WEXITSTATUS( error_code )
						);
			}
			else
			{
				sprintf(socket_send_buffer+strlen( socket_send_buffer )
						,"\n\r\nERR CODE:%d\r\n"
						,-1
						);
			}
		}
		else
		{
			socket_send_buffer[0] = '\0';
		}
	}
	

	//如果接收的数据长度大于1,直接返回命令的结果,否则返回一个空格
	if( strlen( socket_send_buffer ) )
	{
		send( socket_fd,socket_send_buffer,strlen(socket_send_buffer),0 );
	}
	else
	{
		send( socket_fd,"\r\n\r\n",strlen("\r\n\r\n"),0 );
	}
}
int main(int argc,char **argv)
{
	socklen_t len;
	//要连接本机的主机的地址
	struct sockaddr peer_host_addr;
	unsigned int max_listened_socket_number=5;
	int should_reuse_addr=TRUE;
	char tmp_buffer[16];
	//used to get addr info
	struct addrinfo hints, *res;
	int socket_fd;
	int new_accepted_socket_fd;
	int socket_result;

	char receive_cmd[32] = "";
	read_memory("cat /version | grep type | awk -F '=' '{print $2}'", receive_cmd, sizeof(receive_cmd));
	if(strstr(receive_cmd,"s21"))
	{
		device_type = DEVICE_S21;
	}
	else
	{
		device_type = DEVICE_NONE;
	}


	//清空数据读取buffer
	bzero( socket_receive_buffer,sizeof(socket_receive_buffer) );

	//获取本机的地址信息
	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	printf("macwriter_main...\n");
	//要监听的端口
	sprintf(tmp_buffer,"%d",SOCKET_LISTEN_PORT);
	if( getaddrinfo(NULL, tmp_buffer, &hints, &res) )
	{
		//perror - print a system error message
		printf("get addr fail!");
		exit(1);
	}

	//socket - create an endpoint for communication
	//int socket(int domain, int type, int protocol);
	//RETURN VALUE
	//On success, a file descriptor for the new socket is returned.  On error, -1 is returned, and errno is set appropriately.
	socket_fd=socket( res->ai_family, res->ai_socktype, res->ai_protocol );
	if( -1 == socket_fd )
	{
		freeaddrinfo(res);
		printf("creat socket fail!");
		exit(1);
	}

	//int flags=fcntl(sockfd,F_GETFL,0);
	//fcntl(sockfd,F_SETFL,flag|O_NONBLOCK);
	//设置调用closesocket()后，仍可继续重用该socket。调用closesocket()一般不会立即关闭socket，而经历TIME_WAIT的过程
	//getsockopt, setsockopt - get and set options on sockets
	//int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
	setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, &should_reuse_addr, sizeof(should_reuse_addr) );

	//bind - bind a name to a socket
	//int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
	if( -1 == bind(socket_fd,res->ai_addr, res->ai_addrlen) )
	{
		freeaddrinfo(res);
		printf("bind socket fail!");
		close(socket_fd);
		exit(1);
	}
	//free address info
	freeaddrinfo(res);

	//listen - listen for connections on a socket
	/*
	int listen(int sockfd, int backlog);
	listen() marks the socket referred to by sockfd as a passive socket, that is, as a socket that will be used to accept incoming connection requests using accept(2).
		The sockfd argument is a file descriptor that refers to a socket of type SOCK_STREAM or SOCK_SEQPACKET.

		The  backlog  argument  defines the maximum length to which the queue of pending connections for sockfd may grow.  If a connection request arrives when the queue is full,
		the client may receive an error with an indication of ECONNREFUSED or, if the underlying protocol supports retransmission, the request may be  ignored  so  that  a  later
		reattempt at connection succeeds.
	*/
	if( -1 == listen(socket_fd,max_listened_socket_number) )
	{
		printf("listen socket fail!");
		close(socket_fd);
		exit(1);
	}

	//初始化随机因子
	util_init_random_seed();

	while( TRUE )
	{
		fd_set readfds;
		struct timeval tv;

		printf("\naccept wait...\n");
		len=sizeof(struct sockaddr);
		//accept - accept a connection on a socket
		/*
		int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
		The  accept() system call is used with connection-based socket types (SOCK_STREAM, SOCK_SEQPACKET).  It extracts the first connection request on the queue of pending con-
		nections for the listening socket, sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket.  The newly created socket is not in
		the listening state.  The original socket sockfd is unaffected by this call.

		The argument sockfd is a socket that has been created with socket(2), bound to a local address with bind(2), and is listening for connections after a listen(2).

		The argument addr is a pointer to a sockaddr structure.  This structure is filled in with the address of the peer socket, as known to the communications layer.  The exact
		format of the address returned addr is determined by the socket’s address family (see socket(2) and the respective protocol man pages).  When addr  is  NULL,  nothing  is
		filled in; in this case, addrlen is not used, and should also be NULL.

		The  addrlen argument is a value-result argument: the caller must initialize it to contain the size (in bytes) of the structure pointed to by addr; on return it will con-
		tain the actual size of the peer address.

		The returned address is truncated if the buffer provided is too small; in this case, addrlen will return a value greater than was supplied to the call.
		*/
		//接收一个新的连接
		new_accepted_socket_fd=accept( socket_fd,&peer_host_addr,&len );
		if( -1== new_accepted_socket_fd )
		{
			printf("accept fail!");
			close(socket_fd);
			exit(1);
		}

		printf("\nready communicate\n");

		tv.tv_sec=0;
		tv.tv_usec=0;
		while( TRUE )
		{
			//perror("while\n");
			FD_ZERO(&readfds);
			FD_SET(new_accepted_socket_fd,&readfds);
			/*
			select, pselect, FD_CLR, FD_ISSET, FD_SET, FD_ZERO - synchronous I/O multiplexing
			int select(int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout);
			select() and pselect() allow a program to monitor multiple file descriptors, waiting until one or more of the file descriptors become "ready" for some class of I/O
			operation (e.g., input possible).  A file descriptor is considered ready if it is possible to perform the corresponding I/O operation (e.g., read(2)) without blocking.
			timeout is an upper bound on the amount of time elapsed before select() returns.  If both fields of the timeval stucture are  zero,  then  select()  returns  immediately.
			(This is useful for polling.)  If timeout is NULL (no timeout), select() can block indefinitely.
			RETURN VALUE
			On  success,  select() and pselect() return the number of file descriptors contained in the three returned descriptor sets (that is, the total number of bits that are set
			in readfds, writefds, exceptfds) which may be zero if the timeout expires before anything interesting happens.  On error, -1 is returned, and errno is set  appropriately;
			the sets and timeout become undefined, so do not rely on their contents after an error.
			*/
			//查询是否存在数据包,查询之后立即返回
			socket_result=select(new_accepted_socket_fd+1,&readfds,NULL,NULL,&tv);
			//出现错误
			if( socket_result <= -1 )
			{
				printf("socket select error!\n");
				//关掉新创建的socket
				close(new_accepted_socket_fd);
				new_accepted_socket_fd=0;
				break;
			}
			//没有数据包
			else if( socket_result == 0 )
			{
				//usleep(10);
			}
			else
			{
				//socket有读到消息
				if( FD_ISSET(new_accepted_socket_fd,&readfds) )
				{
					int recved_data_len;

					bzero( socket_receive_buffer,sizeof(socket_receive_buffer) );
					recved_data_len = recv( new_accepted_socket_fd,socket_receive_buffer,sizeof(socket_receive_buffer),MSG_DONTWAIT );
					socket_receive_buffer[ recved_data_len ]=0;
					//接收到有效的数据
					if(recved_data_len > 0)
					{
						//去掉结尾的\n
						while( ( socket_receive_buffer[ recved_data_len-1 ] == '\n' ) && recved_data_len )
						{
							socket_receive_buffer[ recved_data_len-1 ]=0;
							recved_data_len-=1;
						}
						//对数据进行分析,执行相关的命令
						execute_cmd( new_accepted_socket_fd,socket_receive_buffer );
					}
					else
					{
						struct tcp_info info;
						int info_len=sizeof(info);
						getsockopt( new_accepted_socket_fd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t*)&info_len );
						//TCP连接处于断开状态
						if( info.tcpi_state != TCP_ESTABLISHED )
						{
							printf("socket closed!\n");
							close(new_accepted_socket_fd);
							new_accepted_socket_fd=0;
							break;
						}
					}
				}
			}
		}
	}

	return 0;
}

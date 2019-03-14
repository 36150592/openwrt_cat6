// Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
//Qualcomm Technologies Proprietary and Confidential.

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

			  Test Application for Diag Interface

GENERAL DESCRIPTION
  Contains main implementation of Diagnostic Services Application for UART.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who    what, where, why
--------   ---     ----------------------------------------------------------
01/06/12   SJ     Created
===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc.h"
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <pthread.h>

#include "socket_at.h"

/*strlcpy is from OpenBSD and not supported by Meego.
 * GNU has an equivalent g_strlcpy implementation into glib.
 * Featurized with compile time USE_GLIB flag for Meego builds.
 */


 int fd_at;
lc_char *prgname;
pthread_t read_sockets_thread = 0, write_sockets_thread =0 ; //, read_device_thread = 0;
pthread_t watchdog_thread = 0;
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t zeroclients_mutex;
pthread_cond_t zeroclients_cv;
struct sigaction sig_int;

lc_int32 socket_port = -1;
lc_char   dev_name[MAX_DEV_NAME_LEN];
//lc_bool g_verbose = -1;
lc_int32 serv_sockfd;
lc_uint32 numofclients = 0;
lc_int32 clientsockfd_list[MAX_PROXY_CLIENTS];
lc_int32 highestfd;

unsigned char read_buf[4096], buf[4100], send_buf[4100];
//boolean bInit_Success = FALSE;
int i, unused_len = 0, start, end, num_read, err;
unsigned long pipe_read_len = 0;
//extern unsigned long count_written_bytes;
#define PC_DEBUG


#ifdef PC_DEBUG
    #define cprintf  printf
#else
    #define cprintf(fmt, args...) do { \
        FILE *fp = fopen("/dev/console", "w"); \
        if (fp) { \
            fprintf(fp, fmt, ## args); \
            fclose(fp); \
        } \
    } while (0)  
#endif

/********************************************************************
 *
 *         Name:  printk_hex
 *  Description:  print data buffer with hex which is lined by 16 bytes
 *        Input:  buf:the data buffer to be print of
 *                buf_size:the data buffer length.
 *       Return:  null
 *        Notes:  
 ********************************************************************/
void cprintf_hex(lc_uint8 *buf,int buf_size)
{
	if(0)
	{
    	int i,j;
   		int total_size = 0;
    	int line_cout =  buf_size>>4;
    	if((buf_size&0x0000000f)!=0)
        	line_cout++;

    	for(i=0;i<line_cout;i++)
    	{
        	cprintf("\n%d:",total_size);
        	for(j=0;j<16;j++)
        	{
            	total_size++;
            	cprintf("%02X ",buf[(i<<4)+j]);
            	if(((i<<4)+1+j)==buf_size)
            	{
                	cprintf("\n");
                	return;
            	}
        	}
    	}
	}   
}

void PrintUsage (char *progname)
{
	cprintf("\n Usage for %s:\n", progname);
	cprintf("\n-p, sockect port number.\n");
	cprintf("\n-d,  device name.\n");
	cprintf("\n-h, --help:\t usage help\n");
	cprintf("\ne.g. socket_at -d <dev name>-p <socket port id> \n");
}


/*********************************************************************
 * Name:     writeToDiagPort
 *
 * Purpose:  This function write to /dev/diag with specific structure.
 *
 * Parms:    None.
 *
 * Return:   Server socket file descriptor.
 *
 * Notes:    None
 *
 *********************************************************************/
int writeToDiagPort(unsigned char* read_buf,int num_read)
{
#if 0	
	if(fd_at > 0) {
		if(num_read > 0) {
		//	cprintf("%s....%d.\n",__func__,__LINE__);
			cprintf_hex(read_buf,num_read);
			memcpy(buf+unused_len, read_buf, num_read);
			start = 0;
			end = 0;
			for(i=0;i<unused_len+num_read;i++) {
				if(buf[i] == CONTROL_CHAR) {
					memcpy(send_buf+4, buf+start, end-start+1);
					diag_send_data(send_buf, end-start+5);
			//		cprintf("%s....%d.\n",__func__,__LINE__);
					cprintf_hex(send_buf,end-start+5);
					start = i+1;
					end = i+1;
					continue;
				}
				end = end+1;
			}
			unused_len = end - start;
			memcpy(buf, buf+start, unused_len);
		}
	}
#else
	if(fd_at > 0) {
		if(num_read > 0) {
		//	cprintf("%s....%d.\n",__func__,__LINE__);
			cprintf_hex(read_buf,num_read);
			write(fd_at,read_buf,num_read);					
		}
	}
#endif
}
/*
 * Name:     OpenDevice
 *
 * Purpose:  This function opens the ttyport and sets the port attributes.
 *
 * Parms:    ttyport - Complete path of ttyport.
 *
 * Return:   Serial port file descriptor.
 *
 * Notes:    None
 *
 */
lc_int32 OpenDevice(lc_char* ttyport)
{
    lc_int32 fd = -1;
    struct termios term;
    
    /*open the device based on PM state*/

//	if(USB_INSERT == usb_status)
	{
		fd = open(ttyport, O_RDWR | O_NOCTTY );  
    }

    if( fd < 0)
    {
        printf("Failed to open a device!\n");
        return fd;
    }
    
	memset((void *)&term,0,sizeof(struct termios));
  
    term.c_cc[VTIME] = 0;
    term.c_cc[VMIN] = 1;
    term.c_cflag &= ~PARENB;
    term.c_cflag &= ~CSTOPB;
    term.c_cflag &= ~CSIZE;
    term.c_cflag &= ~CRTSCTS;
    term.c_cflag |= (CS8 | CLOCAL | CREAD);
    term.c_iflag = 0;
    term.c_oflag = 0;
    term.c_lflag = 0;
    
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW, &term);
    
    return fd;
}
/******************************************************************
 *
 * Name: WriteToClient
 *
 * Purpose:  Send data in buffer to socket with descriptor s
 *           
 * Params:   s - pointer to socket file descriptor.
 *           buffer - buffer containing data to be sent.
 *           lenght - length of buffer
 *
 * Return:   No value
 *
 *****************************************************************/
void WriteToClient(lc_int32* s, void* buffer , size_t length )
{
    lc_int32 retry = 0, sent = 0;    
       
    while(1)
    {
        sent = send(*s, buffer, length, MSG_EOR | MSG_NOSIGNAL );
        if( sent >= 0 )
        {
            if( sent < length )
            {
                cprintf("Error: bytes sent is less than bytes read");
            }
            break;
        }
        else if (errno == EINTR )
        {
            /*just retry*/
        }
        else if ( errno == ECONNREFUSED )
        {
            if(!retry)
                retry++; /*just retry one more time*/
            else
            {
                cprintf("Error: write to client socket failed");
                return;
            }   
        }
        else if (errno == EPIPE )
        {
            /* peer has been closed*/
            cprintf("Error: write to client socket failed");
            *s = 0;
            return;
        }
    }

    return;
}
/*******************************************************************
 *
 * Name: WriteClientSocketThread
 *
 * Purpose:  Thread that reads incoming data from pipe and
 *           writes it to list of client sockets.
 *           
 * Params:   param - dummy argument not used..
 *
 * Return:   No value
 *
 *****************************************************************/
void* WriteClientSocketThread( void* param )
{
	
    fd_set clientset, queueset;
    lc_int32 i, rv, rv1, rd,  bContinue = 1;
    lc_char buf[MAX_BUF_SIZE];

    
    while(bContinue)
    {
        FD_ZERO(&queueset);
        FD_SET(fd_at, &queueset );
        
        rv1 = select(fd_at + 1, &queueset,NULL, NULL, NULL);
        
        if( rv1 < 0 )
        {
            cprintf("Error: select on pipe read fd failed(WST)");
            exit(1);
        }
        else if( FD_ISSET(fd_at, &queueset ) )
        {
            memset((void*)buf, 0, sizeof(buf) );
            /*read the actual data from device*/
            rd = read(fd_at, buf, sizeof(buf) );
            if( rd < 0 )
            {
                cprintf("Error: read failed on pipe read end");             
                continue;
            }
            else if( rd == 0 )
            {
                continue;
            }
			pipe_read_len += rd;

            /*If we are here that means read is successful
             * so we can send the data to clients*/
             
            pthread_mutex_lock( &client_list_mutex );              
            for( i = 0; i < MAX_PROXY_CLIENTS; i++ )
            {
                if( clientsockfd_list[i] != 0 )
                {
                    FD_ZERO(&clientset);
                    FD_SET(clientsockfd_list[i], &clientset );
                    rv = select(clientsockfd_list[i] + 1, NULL, &clientset, NULL, NULL);
                    if( rv < 0 )
                    {
                       cprintf("Error: select on client fd failed(WST)");
                    }
                    else if( FD_ISSET(clientsockfd_list[i], &clientset ) )
                    {
                        /*write the actual data to device*/
                        WriteToClient(&clientsockfd_list[i], buf, rd );

					//	if((count_written_bytes % 10000)>9000)
                    //    {
                     //      cprintf("lct count_written_bytes=%d pipe_read_len=%d \n",count_written_bytes,pipe_read_len);
					//	}
                    }  
                }
            }
            pthread_mutex_unlock( &client_list_mutex );
        }
    }
    return (void*)0;
}


/*********************************************************************
 *
 * Name: ReadClientSocketThread
 *
 * Purpose:  Thread that read incoming data from clients sockets and
 *           writes it to a device.
 *           
 * Params:   param - dummy argument not used..
 *
 * Return:   No value
 *
 *******************************************************************/
void* ReadClientSocketThread( void* param )
{
    fd_set clientset, deviceset;
    lc_int32 i, rv, rv1, read, written, retry = 0;
 //   lc_char buf[MAX_BUF_SIZE];
    lc_int32 bZeroClients = 1;
        
    while( 1 )
    {
        bZeroClients = 1;
        FD_ZERO(&clientset);
        pthread_mutex_lock( &client_list_mutex );
        for( i = 0; i < MAX_PROXY_CLIENTS; i++ )
        {
            if( clientsockfd_list[i] != 0 )
            {
                FD_SET(clientsockfd_list[i], &clientset );
                /*client found*/
                pthread_mutex_lock( &zeroclients_mutex );
                bZeroClients = 0;
                pthread_mutex_unlock( &zeroclients_mutex );
            }
        }
        pthread_mutex_unlock( &client_list_mutex );
        
        if( !bZeroClients )
            rv = select(highestfd, &clientset, NULL, NULL, NULL);
        else
        {
            pthread_mutex_lock(&zeroclients_mutex);
            cprintf( "Info: No client active, blocking read socket thread\n");
            pthread_cond_wait(&zeroclients_cv, &zeroclients_mutex);
            pthread_mutex_unlock(&zeroclients_mutex);
            cprintf( "Info: read socket thread unblocked\n");
            continue;
        }            

        if( rv < 0 )
        {
            cprintf( "Error: select on client socket set failed");
            continue;
        }
        else if( rv == 0 )  /*Only in case of timeout and zero client*/ 
            continue;
        
        /*else rv > 0 */
        pthread_mutex_lock( &client_list_mutex );
        for( i = 0; i < MAX_PROXY_CLIENTS; i++ )
        {
            if( FD_ISSET(clientsockfd_list[i], &clientset ) )
            {
                /*read message*/
                do
                {
                    read = recv(clientsockfd_list[i],(void*) read_buf, sizeof(read_buf), MSG_NOSIGNAL);
             //       cprintf("%s....%d.\n",__func__,__LINE__);
					cprintf_hex(read_buf,read);
                } while (read < 0 && errno == EINTR);
    
                if (read < 0)
                {
                    if(errno == ECONNRESET )
                    {
                      /*this means peer has close
                     * remove it from socket list*/
                     cprintf( "client socket read (r) failed and clientsockfd_list[%d] is %d.",i,clientsockfd_list[i]);
                     close(clientsockfd_list[i]);
                     clientsockfd_list[i] = 0; 
                    }
                    cprintf( "client socket read (r) failed");
                    continue;
                }
                else if( read == 0 )
                {
                    /*this means peer has close
                     * remove it from socket list*/
                    cprintf( "client socket read (r) failed and clientsockfd_list[%d] is %d.",i,clientsockfd_list[i]);
                    close(clientsockfd_list[i]);
                    clientsockfd_list[i] = 0;
                    cprintf( "client socket read (r) failed");
                    continue;
                }
				
				//write to dianositic port---/dev/diag.....
				writeToDiagPort(read_buf,read);

            }    
        }
        pthread_mutex_unlock( &client_list_mutex );

    }/*end of while*/
    
    return (void*)0;
}
/*********************************************************************
 * Name:     CreateServerSocket
 *
 * Purpose:  This function creates and binds the proxy server socket.
 *
 * Parms:    None.
 *
 * Return:   Server socket file descriptor.
 *
 * Notes:    None
 *
 *********************************************************************/
lc_int32 CreateServerSocket(struct sockaddr_in *serv_addr)
{
    lc_int32 sockfd = -1;
    lc_int32 opts;
    lc_int32 reuse_addr = 1;
    /*Create server socket*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( sockfd < 0 )
        return sockfd;
        
    bzero((char *) serv_addr, sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr->sin_port = htons(socket_port);
    
    if (bind(sockfd, (struct sockaddr *)serv_addr,sizeof(*serv_addr)) < 0)
    {
        cprintf( "Error: Cannot bind to server socket");
        close(sockfd);
        return -1;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,sizeof(reuse_addr)) < 0)
    {
        cprintf( "Error: Cannot set server socket addr options");
        close(sockfd);
        return -1;
    }
    /* Make the socket non blocking to make it better*/
    opts = fcntl(sockfd,F_GETFL);
    if (opts < 0)
    {
        cprintf( "Error: fcntl (get) failed on server socket");
        close(sockfd);
        return -1;
    }
    opts = (opts | O_NONBLOCK);
    if (fcntl(sockfd,F_SETFL,opts) < 0)
    {
        cprintf( "Error: fcntl (set) failed on server socket");
        close(sockfd);
        return -1;
    }
    
    return sockfd;
}

/****************************************************************
 *
 * Name: ExitProgram
 *
 * Purpose:  Signal handler for SIGINT signal.
 *           Closes all the file descriptors 
 *           before exiting. 
 *           
 * Params:   sig - signal number intercepted.
 *
 * Return:   No value
 *
 ****************************************************************/
void ExitProgram(int sig)
{    
    CloseAllClients();
    cprintf( "Info: Closing all client sockets");
        
    cprintf( "Info: Closing OFFLINE Signal pipe");
     
    close(serv_sockfd);
    cprintf( "Info: Closing server socket");

    sig_int.sa_flags = 0;
    sig_int.sa_handler = SIG_DFL;
    sigaction(SIGINT, &sig_int, 0);
    
}
/*****************************************************************
 *
 * Name: CloseAllClients
 *
 * Purpose:  Close all client connections
 *           
 * Params:   None.
 *
 * Return:   No value
 *
 ****************************************************************/
void CloseAllClients()
{
    lc_int32 i;
    
    pthread_mutex_lock( &client_list_mutex );
    for( i = 0; i < MAX_PROXY_CLIENTS; i++ )
    {
        if( clientsockfd_list[i] != 0 )
        {
            close(clientsockfd_list[i]);
            clientsockfd_list[i] = 0;
        }
    }
    numofclients = 0;
    pthread_mutex_unlock( &client_list_mutex );
}

/******************************************************************
 *
 * Name: HandleNewClient
 *
 * Purpose:  Handle new incoming connections
 *           
 * Params:   None.
 *
 * Return:   No value
 *
 *****************************************************************/
void HandleNewClient()
{
    
    lc_int32 clientfd = -1;
    lc_int32 i, opts;
    
    clientfd = accept(serv_sockfd, NULL, NULL );

    cprintf("################HandleNewClient start ###############"); 

    if( clientfd < 0 )
    {
        cprintf("Error: accept failed");
        return;
    }
    else
    {
        cprintf("clientfd is %d here.",clientfd); 
    }

	/*
    if(1 == AT_flag)
    {
        system("killall -SIGTERM log_server >/dev/null 2>&1"); 
        system("killall -SIGTERM modem_server >/dev/null 2>&1"); 
        system("killall -SIGTERM slicmgt >/dev/null 2>&1"); 
    }
	*/
    /* Make the socket non blocking to make it better*/
    opts = fcntl(clientfd,F_GETFL);
    if (opts < 0)
    {
        cprintf("Error: fcntl (get) failed on client socket");
        close(clientfd);
        return;
    }
    opts = (opts | O_NONBLOCK);
    if (fcntl(clientfd,F_SETFL,opts) < 0)
    {
        cprintf("Error: fcntl (set) failed on client socket");
        close(clientfd);
        return;
    }
    pthread_mutex_lock( &client_list_mutex );
    /* check if we have reached max client list*/
    for( i = 0; i < MAX_PROXY_CLIENTS && clientfd != -1; i++ )
    {
        if( clientsockfd_list[i] == 0 )
        {
            cprintf("clientfd is %d here.",clientfd); 
            highestfd = clientfd+1;
            clientsockfd_list[i] = clientfd;
            clientfd = -1;
            cprintf("clientfd is %d here.",clientfd); 
            
        }
    }
    pthread_mutex_unlock( &client_list_mutex );
    if( clientfd != -1 )
    {
        /* Server has reached the limit of Max client*/
        close(clientfd);
        cprintf("Info:  Server reached the limit of max clients, connection refused"); 
    }
    else
    {
        cprintf("clientfd is %d here.",clientfd); 
    }
    cprintf("################ HandleNewClient END!!!! ###############"); 
    
}

int main(int argc, char *argv[])
{
	struct sockaddr_in serv_addr;
    fd_set fdmask;
	lc_int32 c/*, retval*/;
	lc_int16 res; 
	lc_int32 sig, opts;
    size_t rv;
	
//	*(int *)send_buf = USER_SPACE_DATA_TYPE;
     
    prgname = argv[0];        
    read_sockets_thread = 0;
    write_sockets_thread = 0;

    while( (c = getopt(argc, argv, "hp:d:") ) != -1 )
    {
        switch(c)
        {
            case 'h':
            PrintUsage(prgname);
            exit(0);
            break; 

            case 'p':
            socket_port = atoi(optarg);
            break;
            
            case 'd':
            strcpy(dev_name,optarg);
            break;
                        
            case '?':
            fprintf(stderr, "Unrecognized Option - %c \n", optopt );
            PrintUsage(prgname);
            exit(1);
            break;

            default:
            PrintUsage(prgname);
            exit(1);
        }        
    }
    
    /*verifying the validity of all the cmd line args*/
    if( socket_port < 0)
    {
        cprintf( "Error: Server Socket port number cannot be negative\n");
        exit(1);
    }

 	fd_at = OpenDevice(dev_name);
	if( fd_at < 0 )
    {
        cprintf( "Error: OpenDevice %s failed\n",dev_name);
        exit(1);
    }
    cprintf( "Info: at read fd... created\n");

	
	sig_int.sa_flags = 0;    
    /*Create a mostly open mask -- only masking SIGINT*/
    sigemptyset(&sig_int.sa_mask);
    sigaddset(&sig_int.sa_mask, SIGINT);
    sigaddset(&sig_int.sa_mask, SIGUSR1);
    sig_int.sa_handler = ExitProgram;
    
    /*Install sinal handler*/
    sig = sigaction(SIGINT,&sig_int, 0);
    if(sig == -1 )
    {
        cprintf( "Error: could not install signal handler for SIGINT");
    }
    
    /*clear the client socket list*/
    memset( (char*)&clientsockfd_list[0], 0, sizeof(clientsockfd_list) );
    
    pthread_mutex_init(&zeroclients_mutex, NULL);
    pthread_cond_init (&zeroclients_cv, NULL);
    
    /*Create proxy server socket */
    serv_sockfd = CreateServerSocket(&serv_addr);
    if( serv_sockfd < 0 )
    {
        cprintf( "Error: CreateServerSocket failed\n");
        exit(1);
    }
    cprintf( "Info: Server socket created\n");
    
    if( listen(serv_sockfd, 5) == -1 )
    {
        cprintf( "Error: Listening on server socket failed"); 
        exit(1);
    }
    cprintf( "Info: Server socket listening\n");

    
    /*wait for client requests*/
    while(1)
    {
        FD_ZERO(&fdmask);
        FD_SET(serv_sockfd, &fdmask);
        
        do {
            rv = select(serv_sockfd + 1, &fdmask, NULL, NULL, NULL);
        } while (rv < 0 && errno == EINTR);
        
        if( rv < 0 )
        {
            cprintf( "Error: select on server socket failed, exiting...");
            break;
        }
        if( rv == 0 )   
            continue;

        if( FD_ISSET(serv_sockfd, &fdmask) )
        {
            HandleNewClient();
            /*if this is the first client launch 
             * socket read/write thread*/
             if( !read_sockets_thread && !write_sockets_thread)
             {
                rv =  pthread_create(&read_sockets_thread,NULL,ReadClientSocketThread, NULL );
                if( rv != 0 )
                {
                    cprintf( "Error: Create read_sockets_thread failed ");
                    CloseAllClients();
                    close(serv_sockfd);
                    exit(1);
                }                 
                                
                rv =  pthread_create(&write_sockets_thread,NULL,WriteClientSocketThread, NULL );
                if( rv != 0 )
                {
                    cprintf("Error: Create write_sockets_thread failed ");
                    CloseAllClients();
                    close(serv_sockfd);
                    exit(1);
                }        
                cprintf( "Info: Read/Write socket threads created\n");
            }
            pthread_mutex_lock(&zeroclients_mutex);
            pthread_cond_signal(&zeroclients_cv);
            pthread_mutex_unlock(&zeroclients_mutex);
            
        }
        
    }
    close(serv_sockfd);
    

//	ioctl(fd_at, TCSETS, &options_save);	
	close(fd_at);
	
	//distroy diag port.....
	Diag_LSM_DeInit();
	return 0;
}



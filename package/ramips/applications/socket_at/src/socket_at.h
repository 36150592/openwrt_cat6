/***************************************************************************************
 *
 *
 *       Filename:  dtu_process.h
 *
 *    Description:  dtu process daemon header defination.
 *
 *        Version:  1.0
 *        Created:  13/12/2012  02:00:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  qiaoyongkang 
 *      Copyright:  Copyright (c) 2012, Xi'an LongQuest Technology LTD,all rights reserved
 *
 ***************************************************************************************/
#ifndef DIAG_UART_LOG_H_
#define DIAG_UART_LOG_H_

/* Linux definitions */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <syslog.h>
#include <termios.h>
#include <signal.h>

typedef signed char     lc_int8;
typedef char   		    lc_char;
typedef unsigned char   lc_uint8;
typedef signed short    lc_int16;
typedef signed long     lc_int32;
typedef unsigned long   lc_size_t;
typedef unsigned long   lc_uint32;
typedef long            lc_ssize_t;
typedef long            lc_bool;

#define PORTNAME_LENGTH 20
#define MAX_PROXY_CLIENTS 5
#define MAX_MODEM_RETRIES 5
#define DEFAULT_TIMEOUT 10000
#define MAX_BUF_SIZE 2048
#define DIAG_DATA_PIPE  "/tmp/diag_pipe"
#define MAX_DEV_NAME_LEN  128
#define BAUDRATE B115200    /* preferred baud rate */
#endif

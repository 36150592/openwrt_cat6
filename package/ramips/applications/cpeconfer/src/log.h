#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

//#define DEBUG

#ifdef DEBUG
#define log_info(...) printf(__VA_ARGS__)
#define log_error(...) printf(__VA_ARGS__)
#else
#define log_info(...) syslog(LOG_INFO,__VA_ARGS__)
#define log_error(...) syslog(LOG_ERR,__VA_ARGS__)
#endif









#endif

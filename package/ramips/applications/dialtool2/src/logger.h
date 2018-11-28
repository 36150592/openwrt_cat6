#ifndef _LOGGER_H_
#define _LOGGER_H_
#include <syslog.h>
#include <stdio.h>

//#define DEBUG

#ifndef DEBUG
#define log_error(...) syslog(LOG_ERR,__VA_ARGS__)
#define log_info(...) syslog(LOG_INFO,__VA_ARGS__)
#else
#define log_error(...) printf(__VA_ARGS__)
#define log_info(...) printf(__VA_ARGS__)
#endif

#endif

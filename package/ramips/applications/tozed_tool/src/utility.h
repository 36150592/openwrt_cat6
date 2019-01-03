#ifndef _UTILITY_H
#define _UTILITY_H

#include <syslog.h>
#include <stdio.h>

#define DEBUG

#ifndef DEBUG
#define print(...) syslog(LOG_INFO,__VA_ARGS__)
#else
#define print(...) printf(__VA_ARGS__)
#endif

extern int tz_ret(int num);


#endif
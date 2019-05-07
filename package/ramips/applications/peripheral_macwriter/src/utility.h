#ifndef _UTILITY_H
#define _UTILITY_H

#include <syslog.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define DEBUG

#ifndef DEBUG
#define print(...) syslog(LOG_INFO,__VA_ARGS__)
#else
#define print(...) printf(__VA_ARGS__)
#endif

extern int tz_ret(int num);
extern int read_memory(char *shellcmd, char *out, int size);

#endif
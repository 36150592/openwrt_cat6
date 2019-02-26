#ifndef _UTILITY_H
#define _UTILITY_H

#include <syslog.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>

#define DEBUG

#ifndef DEBUG
#define print(...) syslog(LOG_INFO,__VA_ARGS__)
#else
#define print(...) printf(__VA_ARGS__)
#endif

extern int tz_ret(int num);
extern int read_memory(char *shellcmd, char *out, int size);
extern void util_strip_traling_spaces( char* one_string );
extern void util_strip_traling_r_n_t( char* one_string );

extern void set_param_to_config_tozed(char* name, char* value);

#endif
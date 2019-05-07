#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>

#include "log.h"

static int use_syslog = 0;

void echo_status_info(const char* status)
{
	char cmd[1024] = {0};

	sprintf(cmd, "echo -n %s > /tmp/.ddns_status", status);

	system(cmd);
}

void log_open(const struct cfg *cfg)
{
        use_syslog = (cfg->daemonize || cfg->use_syslog);

	if(use_syslog)
	{
		openlog("yaddns", LOG_CONS, LOG_DAEMON);
	}
}

void log_close(void)
{
	if(use_syslog)
	{
		closelog();
	}
}

void log_it(int priority, char const *format, ...)
{
	va_list ap;

	va_start(ap, format);

	if(use_syslog)
	{
		vsyslog(priority, format, ap);
	}
	else
	{
		vfprintf(stdout, format, ap);
	}

	va_end(ap);
}

#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "Cshell.h"


void PutsToFile(FILE *fp, const char *__form, ...)
{
	 va_list arg_ptr;

	 if (fp && __form)
	 {
	        va_start(arg_ptr, __form);
	        vfprintf(fp, __form, arg_ptr);
	        fprintf(fp, "\r\n");
	        va_end(arg_ptr);
	 }
}

char *RemoveRinghtWrap(char *fs)
{
    /*去掉字符串最后的换行制表\0*/
    int i = 0;

    if (!fs)
        return fs;

    for (i = strlen(fs) - 1; i > 0; i--)
    {
        if ((fs[i] == '\r') || (fs[i] == '\n'))
            fs[i] = '\0';
        else
            break;
    }

    return fs;
}


BOOL Shell(const char *fshell, char *frecv, int fsize, PTShellCallBack fcall)
{
	if (!fshell)
    	{
        	//print("shell: command point is null");
        	return FALSE;
    	}

	if (!frecv && !fcall)
	{
        	system(fshell);
        	return TRUE;
    	}

	FILE *mpf = NULL;
    	char mline[512] = "";

    	if ((mpf = popen(fshell, "r")))
    	{
        	if (fcall && frecv)
        	{
           		memset(frecv, 0, fsize);
            		while(fgets(mline, sizeof(mline), mpf))
            		{
                		fcall(mline);
                		snprintf(frecv + strlen(frecv), sizeof(frecv) - strlen(frecv), "%s", mline);
                		memset(mline, 0, sizeof(mline));
            		}
            		RemoveRinghtWrap(frecv);
            		//print("shell recv:%s", frecv);
        	}
        	else if (frecv)
        	{
	            	memset(frecv, 0, fsize);
	            	fread(frecv, sizeof(char), fsize-1, mpf);
	            	RemoveRinghtWrap(frecv);
	            	//print("shell recv:%s", frecv);
        	}
        	else if (fcall)
        	{
            		while(fgets(mline, sizeof(mline), mpf))
            		{
                		fcall(mline);
                		memset(mline, 0, sizeof(mline));
            		}
        	}

       		pclose(mpf);
        	return TRUE;
    	}
    	return FALSE;
}

BOOL shell(const char *_Format, ...)
{
	char mshell[512] = "";
    	va_list arg_ptr;

    	va_start(arg_ptr, _Format);
    	vsprintf(mshell, _Format, arg_ptr);
    	va_end(arg_ptr);

    	//print("shell done: %s", mshell);
    	return Shell(mshell, NULL, 0, NULL);
}

BOOL shell_recv(char *frecv, int fsize, const char *_Format, ...)
{
	char mshell[512] = "";
    	va_list arg_ptr;

    	va_start(arg_ptr, _Format);
    	vsprintf(mshell, _Format, arg_ptr);
   		va_end(arg_ptr);
    	//print("shell done: %s", mshell);
    	return Shell(mshell, frecv, fsize, NULL);
}

char* util_get_local_time(char* str)
{
	time_t now;
	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);
	memset(str, 0, 32);
	snprintf(str, 31, "%04d-%02d-%02d %02d:%02d:%02d",
		timenow->tm_year + 1900, timenow->tm_mon+1, timenow->tm_mday,
		timenow->tm_hour, timenow->tm_min, timenow->tm_sec);
	return str;
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
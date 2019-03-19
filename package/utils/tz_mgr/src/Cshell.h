#ifndef __CSHELL_H_
#define __CSHELL_H_

#include <stdio.h>
#include <stdbool.h>
typedef unsigned int BOOL;
#define TRUE 1
#define FALSE 0
extern bool global_debugPrint_flag;
typedef void TShellCallBack(const char *fline);
typedef TShellCallBack * PTShellCallBack;



extern void PutsToFile(FILE *fp, const char *__form, ...);/*函数功能：把字符串输出至文件fp中*/
extern BOOL shell(const char *_Format, ...);/*函数功能：执行shell命令，成功返回真，失败返回假*/
extern BOOL Shell(const char *fshell, char *frecv, int fsize, PTShellCallBack fcall);/*函数功能：运行指定的shell命令，把返回结果当做回调函数的参数*/
extern BOOL shell_recv(char *frecv, int fsize, const char *_Format, ...);/*函数功能：运行shell命令，把运行结果通过管道保存至frecv中*/
extern char *RemoveRinghtWrap(char *fs);/*函数功能：去掉字符串最后的换行制表\0  配合shell_recv使用*/
extern char* util_get_local_time(char* str);/*函数功能：获取当前时间*/
extern void util_strip_traling_spaces( char* one_string );//去掉字符串中末尾的回车换行空格


#endif

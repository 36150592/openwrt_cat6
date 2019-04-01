#include <stdio.h>
#include <string.h>
#include <lauxlib.h>
#include <lualib.h>
#include "luatzlib.h"
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<linux/un.h>
#include <errno.h>
#include <unistd.h>

//待注册的C函数，该函数的声明形式在上面的例子中已经给出。
//需要说明的是，该函数必须以C的形式被导出，因此extern "C"是必须的。
//函数代码和上例相同，这里不再赘述。
int remove_stderr(lua_State* L) 
{
//    double op1 = luaL_checknumber(L,1);
//    double op2 = luaL_checknumber(L,2);
//


    freopen("/dev/null", "w", stderr);
    lua_pushnumber(L,0);
    return 1;
}

int unix_connect(lua_State* L)
{
	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);  
	if (sockfd == -1)  
		return 0;   
	char *pathname = luaL_checkstring(L,1);
	struct sockaddr_un servAddr;   
	servAddr.sun_family = AF_UNIX;	
	strcpy(servAddr.sun_path, pathname);  
	if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)	
		return 0;	
	
	lua_pushnumber(L, sockfd);
	return 1;

}

int unix_read(lua_State* L)
{
	int socket_fd = luaL_checkint(L,1);
	if(socket_fd <= 0)
		return 0;

	char buf[1024] = {0};
	int n = 0;
	while((n=read(socket_fd,buf,1024))>0)
	{
		if(n<0&&errno==EINTR)
			continue;
	}
	
	lua_pushstring(L, buf);
	return 1;
}

int unix_write(lua_State* L)
{
	int socket_fd = luaL_checkint(L,1);
	char* data = luaL_checkstring(L,2);
	int data_len = luaL_checkint(L,3);
	
	if(socket_fd <= 0 || data_len <= 0)
		return 0;
	int n = write(socket_fd, data, data_len);
	
	lua_pushnumber(L, n);
	return 1;
}


int unix_close(lua_State* L)
{
	int socket_fd = luaL_checkint(L,1);
	
	if(socket_fd <= 0)
		return 0;
	close(socket_fd);
	return 0;
}


//luaL_Reg结构体的第一个字段为字符串，在注册时用于通知Lua该函数的名字。
//第一个字段为C函数指针。
//结构体数组中的最后一个元素的两个字段均为NULL，用于提示Lua注册函数已经到达数组的末尾。
static luaL_Reg mylibs[] = { 
    {"remove_stderr", remove_stderr},
    {"unix_connect",unix_connect},
	{"unix_read",unix_read},
	{"unix_write",unix_write},
    {"unix_close",unix_close},
    {NULL, NULL} 
}; 

//该C库的唯一入口函数。其函数签名等同于上面的注册函数。见如下几点说明：
//1. 我们可以将该函数简单的理解为模块的工厂函数。
//2. 其函数名必须为luaopen_xxx，其中xxx表示library名称。Lua代码require "xxx"需要与之对应。
//3. 在luaL_register的调用中，其第一个字符串参数为模块名"xxx"，第二个参数为待注册函数的数组。
//4. 需要强调的是，所有需要用到"xxx"的代码，不论C还是Lua，都必须保持一致，这是Lua的约定，
//   否则将无法调用。
int luaopen_luatzlib(lua_State* L) 
{
    const char* libName = "luatzlib";
    luaL_register(L,libName,mylibs);
    return 1;
}

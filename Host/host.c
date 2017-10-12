#include "stdlib.h"

#include "fcgi_stdio.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define SCRIPT_NAME_LEN 256

#define FCGI_OUTPUT_HTML printf( "Content-type:text/html\r\n\r\n" );

//执行 lua 脚本，执行时会创建新的 lua 状态机，执行后自动销毁状态机
int lua_execute_file(char *filename);
//通过 lua 脚本加载一个 native 的库，返回创建的状态机
lua_State *lua_lib_host(char *filename);
//销毁一个 lua 状态机
void *lua_close_state(lua_State *L);
//load config file
int load_conf();

//Acquire counter
unsigned long counter = 0;

char script_name[SCRIPT_NAME_LEN];
//char script_home[] = "/usr/share/nginx/html/";

char *init_script;
char *script_home;

int main()
{
	char *s = script_name;
	if(load_conf())return -1;
	//执行初始化脚本
	lua_State *L = lua_lib_host(init_script);
	//响应请求
	while(FCGI_Accept() >= 0)
	{
		//FCGI_OUTPUT_HTML;
		s = script_name;
		sprintf(script_name, "%s%s", script_home, getenv("SCRIPT_NAME"));
		//lua 脚本执行时不创建新的状态机
		//首先执行 __Accept__ 函数(__Accept__由用户定义)
		luaL_dostring(L, "pcall(__Accept__)");
		luaL_dofile(L, script_name);
	}
	return 0;
}

int lua_execute_file(char *filename)
{	
	lua_State *L;
	FILE *fp;
	fp = fopen(filename, "r");
	if(fp == NULL){
		printf("OPEN_SCRIPT_FAILED :%s", filename);
		return -1;
	}
	fclose(fp);
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L, filename);
	lua_close(L);
	return 0;
}

lua_State *lua_lib_host(char *filename)
{	
	lua_State *L;
	FILE *fp;
	fp = fopen(filename, "r");
	if(fp == NULL){
		printf("OPEN_SCRIPT_FAILED :%s", filename);
		return NULL;
	}
	fclose(fp);
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L, filename);
	return L;
}

void *lua_close_state(lua_State *L)
{
	lua_close(L);
}

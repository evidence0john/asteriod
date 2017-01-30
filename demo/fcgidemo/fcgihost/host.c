#include "stdlib.h"

#include "fcgi_stdio.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define FCGI_OUTPUT_HTML printf( "Content-type:text/html\r\n\r\n" );

int lua_execute_file(char *filename);
lua_State *lua_lib_host(char *filename);
void *lua_close_state(lua_State *L);

int lua_execute_file(char *filename)
{	
	lua_State *L;
	FILE *fp;
	fp = fopen(filename,"r");
	if(fp == NULL){
		printf("OPEN_SCRIPT_FAILED :%s",filename);
		return -1;
	}
	fclose(fp);
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L,filename);
	lua_close(L);
	return 0;
}

lua_State *lua_lib_host(char *filename)
{	
	lua_State *L;
	FILE *fp;
	fp = fopen(filename,"r");
	if(fp == NULL){
		printf("OPEN_SCRIPT_FAILED :%s",filename);
		return NULL;
	}
	fclose(fp);
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L,filename);
	return L;
}

void *lua_close_state(lua_State *L)
{
	lua_close(L);
}

int main()
{
	long counter = 0;
	char script_name[128];
	char *s = script_name;
	char fcgi_home[] = "/usr/share/nginx/html/";
	lua_lib_host("/usr/share/nginx/html/init.lua");
	while(FCGI_Accept() >= 0)
	{
		FCGI_OUTPUT_HTML;
		s = script_name;
		sprintf(script_name,"%s%s", fcgi_home, getenv("QUERY_STRING"));
		while(*++s != '=') if(!*s) break;
		*s = 0;
		lua_execute_file(script_name);
	}
	return 0;
}

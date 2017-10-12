#include "stdio.h"
#include "string.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern char *init_script;
extern char *script_home;

static char *get_lua_str(lua_State *L, char *name)
{
	lua_getglobal(L, name);
	return (char*)lua_tostring(L, -1);
}

int load_conf()
{
	char filename[] = "./conf.lua";
	lua_State *L;
	FILE *fp;
	fp = fopen(filename, "r");
	if(fp == NULL){
		printf("OPEN_CONF_FAILED :%s", filename);
		return 0;
	}
	fclose(fp);
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L, filename);
	script_home = get_lua_str(L, "script_home");
	if(!script_home)return -1;
	init_script = get_lua_str(L, "init_script");
	if(!init_script)return -1;
	return 0;
}

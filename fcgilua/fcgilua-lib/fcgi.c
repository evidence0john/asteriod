#include "fcgi_stdio.h"
#include "stdlib.h"
#include "string.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static long counter = 0;

int _HashKey(char *key, unsigned int seed)
{
    int nHash = seed;
    while (*key)
        nHash = (nHash << 5) + nHash + *key++;
    return nHash;
}

int postlen(lua_State *L)
{
	int len;
	if(sscanf((char *)getenv("CONTENT_LENGTH"), "%d", &len) != 1)
		len = 0;
	lua_pushnumber(L, len);
	return 1;
}

int query(lua_State *L)
{
	lua_pushstring(L, getenv("QUERY_STRING"));
	return 1;
}


int _env(lua_State *L)
{
	lua_pushstring(L, getenv(lua_tostring(L, 1)));
	return 1;
}

int _echo(lua_State *L)
{
	int i = 1;
	while(lua_tostring(L, i))
		printf("%s", (char *)lua_tostring(L, i++));
	return 1;
}

int tpost(lua_State *L)
{
	char *buffer;
	int len;
	if(sscanf(getenv("CONTENT_LENGTH"), "%d", &len) != 1)
		len = 0;
	if(!len) return 1;
	buffer = (char *)malloc(len + 1);
	if(buffer == NULL){
		lua_pushinteger(L, len);
		return 1;
	}
	fread(buffer, len, 1, stdin);
	buffer[len] = 0;
	lua_pushstring(L, buffer);
	free(buffer);
	return 1;
}

int getip(lua_State *L)
{
	lua_pushstring(L, getenv("REMOTE_ADDR"));
	return 1;
}

int inc_counter(lua_State *L)
{
	counter++;
	return 1;
}

int set_counter(lua_State *L)
{
	counter = lua_tonumber(L, 1);
	return 1;
}

int get_counter(lua_State *L)
{
	lua_pushnumber(L, counter);
	return 1;
}

int calc_access(lua_State *L)
{
	lua_pushinteger(L, _HashKey((char *)lua_tostring(L, 1),0));
	return 1;
}

static luaL_Reg libs[] = {
	{"postlen", postlen},
	{"query", query},
	{"echo", _echo},
	{"tpost", tpost},
	{"getip", getip},
	{"env", _env},
	{"set_counter", set_counter},
	{"get_counter", get_counter},
	{"inc_counter", inc_counter},
	{"calc_access", calc_access},
	{NULL, NULL}
};

int luaopen_fcgi(lua_State * L) {
	lua_newtable(L);
	luaL_setfuncs(L, libs, 0);
	return 1;
}

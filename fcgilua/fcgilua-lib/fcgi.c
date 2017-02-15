#include "fcgi_stdio.h"
#include "stdlib.h"
#include "string.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define ENCRYPT_GAP 10

#define ERR_POST "ERR_POST"
#define ERR_ALLOC "ERR_ALLOC"
#define ERR_FILE_POST -21
#define ERR_FILE_BUF -22
#define ERR_FILE_OPEN -23

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
	if(!len){
		lua_pushstring(L, ERR_POST);
		return 1;
	}
	buffer = (char *)malloc(len + 1);
	if(buffer == NULL){
		lua_pushstring(L, ERR_ALLOC);
		return 1;
	}
	fread(buffer, len, 1, stdin);
	buffer[len] = 0;
	lua_pushstring(L, buffer);
	free(buffer);
	return 1;
}

int _wfile(lua_State *L) //post single file
{
	char *buffer, *fstart, *fend;
	char *filename = (char*)lua_tostring(L, 1);
	char *mod = (char*)lua_tostring(L, 2);
	int len;
	FILE *fp;
	if(sscanf(getenv("CONTENT_LENGTH"), "%d", &len) != 1)
		len = 0;
	if(!len){
		lua_pushinteger(L, ERR_FILE_POST);
		return 1;
	}
	buffer = (char *)malloc(len);
	if(buffer == NULL){
		lua_pushinteger(L, ERR_FILE_BUF);
		return 1;
	}
	fread(buffer, len, 1, stdin);
	fstart = buffer;
	fend = buffer + len;
	fend -= ENCRYPT_GAP;
find_start:
	while(*fstart++ != 10){
		if(fstart > fend){
			lua_pushinteger(L, ERR_FILE_POST);
			free(buffer);
			return 1;
		}
	}
	fstart++;
	if (*fstart != 10 && *fstart !=13) goto find_start;
	fstart++;
	if(fstart > fend){
		lua_pushinteger(L, ERR_FILE_POST);
		free(buffer);
		return 1;
	}
	while(*fend-- != 10){
		if(fstart > fend){
			lua_pushinteger(L, ERR_FILE_POST);
			free(buffer);
			return 1;
		}
	}
	len = (fend - fstart);
	if(mod == NULL) mod = "wb+";
	fp = fopen(filename, mod);
	if(fp == NULL){
		lua_pushinteger(L, ERR_FILE_OPEN);
		return 1;
	}
	fwrite(fstart, 1, len, fp);
	fclose(fp);
	free(buffer);
	lua_pushinteger(L, len);
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
	{"file", _wfile},
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

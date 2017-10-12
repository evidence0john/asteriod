#include "stdlib.h"
#include "string.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "fcgi_stdio.h"

#include "md5.h"

#define ENCRYPT_GAP 10

#define ERR_POST "ERR_POST"
#define ERR_ALLOC "ERR_ALLOC"
#define ERR_FILE_POST -21
#define ERR_FILE_BUF -22
#define ERR_FILE_OPEN -23

#define DECODE_FLAG '%'
#define DECODE_NBSP '+'
#define BLANK_SPACE 32
#define DECODE_BUFLEN 1048576

#define INT32_WIDTH 4
#define INT64_WIDTH 8

//33554432 -> 32MB
#define MAX_BUFLEN 33554432

//memory counter
static int mc = 0;

//write
int wbuf(lua_State *L)
{
	char *p, *str = (char*)lua_tostring(L, 1);
	int len = strlen(str) + 1;
	if(lua_tointeger(L, 2))
		len = lua_tointeger(L, 2);
	if(mc + len > MAX_BUFLEN){
		lua_pushinteger(L, 0);
		return 1;
	} else {
		mc += len;
		p = (char*)malloc(len);
		if(!p){
			lua_pushinteger(L, 0);
			return 1;
		}
		memcpy(p, str, len);
		lua_pushinteger(L, (int)p);
	}
	return 1;
}

//read
int rbuf(lua_State *L)
{
	int rp = lua_tointeger(L, 1);
	char *p = (char*)rp;
	if(lua_tointeger(L, 2)){
		p += lua_tointeger(L, 2);
		lua_pushlstring(L, p, lua_tointeger(L, 3));
	} else {
		lua_pushstring(L, p);
	}
	return 1;
}

//delete
int dbuf(lua_State *L)
{
	int rp = lua_tointeger(L, 1);
	char *p = (char*)rp;
	int len = strlen(p) + 1;
	if(lua_tointeger(L, 1))
		len = lua_tointeger(L, 1);
	if(len){
		free(p);
		mc -=len;
	}
	lua_pushinteger(L, len);
	return 1;
}

//buffer info
int buf(lua_State *L)
{
	lua_pushinteger(L, mc);
	lua_pushinteger(L, MAX_BUFLEN);
	lua_pushinteger(L, MAX_BUFLEN - mc);
	return 3;
}

static long counter = 0;

static char *_urldecoder(char *str, int buflen)
{
	char *p, *dec = (char*)malloc(buflen);
	int i;
	if(!dec) return NULL;
	p = dec;
	if(!str) return NULL;
	while(*str){
		if(*str == DECODE_FLAG){
			sscanf(++str, "%2x", &i);
			str+=2;
			*p++ = i;
		}
		else if(*str == DECODE_NBSP){
			*p++ = BLANK_SPACE;
			str++;
		}
		else
			*p++ = *str++;
	}
	*p = 0;
	return dec;
}

int urldecoder(lua_State *L)
{
	int buflen = DECODE_BUFLEN;
	char *buf;
	if(lua_tointeger(L, 2))
		buflen = lua_tointeger(L, 2);
	buf = _urldecoder((char*)lua_tostring(L, 1), buflen);
	lua_pushstring(L, buf);
	//Should I free memo here ?
	free(buf);
	return 1;
}

int int32(lua_State *L)
{
	int i = lua_tointeger(L, 1);
	lua_pushlstring(L, (char*)&i, INT32_WIDTH );
	return 1;
}

int int64(lua_State *L)
{
	long long i = lua_tointeger(L, 1);
	lua_pushlstring(L, (char*)&i, INT64_WIDTH);
	return 1;
}

int toint32(lua_State *L)
{
	int *i = (int*)lua_tostring(L, 1);
	lua_pushinteger(L, *i);
	return 1;
}

int toint64(lua_State *L)
{
	long long *i = (long long*)lua_tostring(L, 1);
	lua_pushinteger(L, *i);
	return 1;
}

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
	lua_pushinteger(L, len);
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
	return 0;
}

int _print(lua_State *L)
{
	int i = 1;
	while(lua_tostring(L, i))
		printf("%s", (char *)lua_tostring(L, i++));
	printf("<br>");
	return 0;
}

int raw_post(lua_State *L)
{
	char *buffer;
	int len, userlen;
	fpos_t *pos;
	if(sscanf(getenv("CONTENT_LENGTH"), "%d", &len) != 1)
		len = 0;
	if(!len){
		lua_pushstring(L, ERR_POST);
		return 1;
	}
	userlen = lua_tointeger(L, 1);
	if(userlen && userlen < len)
		len = userlen;
	buffer = (char *)malloc(len);
	if(buffer == NULL){
		lua_pushstring(L, ERR_ALLOC);
		return 1;
	}
	fread(buffer, len, 1, stdin);
	lua_pushlstring(L, buffer, len);
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
	while(*fend != 10 && *fend !=13){
		fend--;
		if(fstart > fend){
			lua_pushinteger(L, ERR_FILE_POST);
			free(buffer);
			return 1;
		}
	}
	fend--;
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
	counter = lua_tointeger(L, 1);
	return 1;
}

int get_counter(lua_State *L)
{
	lua_pushinteger(L, counter);
	return 1;
}

int calc_access(lua_State *L)
{
	lua_pushinteger(L, _HashKey((char *)lua_tostring(L, 1),0));
	return 1;
}

int md5_string(lua_State * L)
{
	char *str = (char*)lua_tostring(L, 1);
	MD5_CTX md5;
	unsigned char decrypt[16], fmd5[33];
	char *p = fmd5;
	int i;
	MD5Init(&md5);
	MD5Update(&md5, str, strlen(str));
	MD5Final(&md5, decrypt);
	for (i = 0; i < 16; i++) {
		sprintf(p, "%02x", decrypt[i]);
		p += 2;
	}
	lua_pushstring(L, fmd5);
	return 1;
}

static luaL_Reg libs[] = {
	{"toint32", toint32},
	{"toint64", toint64},
	{"int32", int32},
	{"int64", int64},
	{"buf", buf},
	{"wbuf", wbuf},
	{"rbuf", rbuf},
	{"dbuf", dbuf},
	{"urldecoder", urldecoder},
	{"postlen", postlen},
	{"query", query},
	{"echo", _echo},
	{"print", _print},
	{"tpost", raw_post},
	{"Raw_POST", raw_post},
	{"file", _wfile},
	{"getip", getip},
	{"env", _env},
	{"set_counter", set_counter},
	{"get_counter", get_counter},
	{"inc_counter", inc_counter},
	{"calc_access", calc_access},
	{"time33", calc_access},
	{"md5", md5_string},
	{NULL, NULL}
};

int luaopen_fcgi(lua_State * L) {
	lua_newtable(L);
	luaL_setfuncs(L, libs, 0);
	return 1;
}

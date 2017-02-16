#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "../../tools/md5/md5.h"

#define buf_size 512
#define ERR_OPEN_FILE "error open file"
#define ERR_LOAD_FILE "error load file"

int file(lua_State * L)
{
	char *filename = (char*)lua_tostring(L, 1);
	FILE *fp;
	fp = fopen(filename, "rb");
	if (fp == NULL){
		lua_pushstring(L, ERR_OPEN_FILE);
		return 1;
	}
	int i, len, gap = buf_size;
	char buf[buf_size];
	MD5_CTX md5;
	unsigned char decrypt[16], fmd5[33];
	char *p = fmd5;
	MD5Init(&md5);
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	i = len / gap;
	for (i; i > 0; i--) {
		fread(buf, gap, 1, fp);
		MD5Update(&md5, buf, gap);
	}
	gap = len % gap;
	fread(buf, gap, 1, fp);
	MD5Update(&md5, buf, gap);
	MD5Final(&md5, decrypt);
	for (i = 0; i < 16; i++) {
		sprintf(p, "%02x", decrypt[i]);
		p += 2;
	}
	lua_pushstring(L, fmd5);
	return 1;
}

int string(lua_State * L)
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
	{"file", file},
	{"string", string},
	{NULL, NULL}
};

int luaopen_md5lua(lua_State * L)
{
	lua_newtable(L);
	luaL_setfuncs(L, libs, 0);
	return 1;
}

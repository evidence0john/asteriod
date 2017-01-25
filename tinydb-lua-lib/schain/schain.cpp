#define SCHAIN_POOL 16
#define READ_BUFFER_LEN 65536

#define ERR_SCHAIN_NOOBJ -71

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "../../include/sharedchain.hpp"

static char *current_schain = NULL;
static char *schain_pool[SCHAIN_POOL] = { NULL };

static char read_buffer[READ_BUFFER_LEN] = "\0";

int create(lua_State * L)
{
	lua_pushinteger(L,
	create_shared_chain(lua_tointeger(L, 1), lua_tointeger(L, 2),
			    (char *)lua_tostring(L, 3), (char *)lua_tostring(L, 4)));
	return 1;
}

int dump(lua_State * L)
{
	dump_shared_chain(lua_tointeger(L, 1), (char *)lua_tostring(L, 2));
	return 1;
}

int load(lua_State * L)
{
	lua_pushinteger(L,
	load_shared_chain(lua_tointeger(L, 1), (char *)lua_tostring(L, 2)));
	return 1;
}

int remove(lua_State * L)
{
	remove_shared_chain(lua_tointeger(L, 1), (char *)lua_tostring(L, 2));
	return 1;
}

int link(lua_State * L)
{
	schain_pool[lua_tointeger(L, 1) - 1] =
	    link_shared_chain(lua_tointeger(L, 2), (char *)lua_tostring(L, 3));
	return 1;
}

int select(lua_State * L)
{
	current_schain = schain_pool[lua_tointeger(L, 1) - 1];
	return 1;
}

int write(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	lua_pushinteger(L,
			write_shared_chain(current_schain, lua_tointeger(L, 1),
					   (char *)lua_tostring(L, 2)));
	return 1;
}

int loadtext(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	lua_pushinteger(L,
			loadtext_shared_chain(current_schain, lua_tointeger(L, 1),
					   (char *)lua_tostring(L, 2)));
	return 1;
}

int exporttext(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	lua_pushinteger(L,
			exporttext_shared_chain(current_schain, lua_tointeger(L, 1),
					   (char *)lua_tostring(L, 2)));
	return 1;
}

int read(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		read_buffer[0] = 0;
		return 1;
	}
	if (!read_shared_chain(current_schain, lua_tointeger(L, 1), read_buffer))
		read_buffer[0] = 0;
	lua_pushstring(L, read_buffer);
	return 1;
}

int del(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	lua_pushinteger(L,
			del_shared_chain(current_schain, lua_tointeger(L, 1)));
	return 1;
}

int getlen(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	lua_pushinteger(L,
			getlen_shared_chain(current_schain,
					    lua_tointeger(L, 1)));
	return 1;
}

int available(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	lua_pushinteger(L,
			available_shared_chain(current_schain));
	return 1;
}

int status(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	lua_pushinteger(L,
			status_shared_chain(current_schain,
					    lua_tointeger(L, 1)));
	return 1;
}

int iscend(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	lua_pushinteger(L,
			iscend_shared_chain(current_schain,
					    lua_tointeger(L, 1)));
	return 1;
}

int isfstart(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	lua_pushinteger(L,
			isfstart_shared_chain(current_schain,
					    lua_tointeger(L, 1)));
	return 1;
}

int writei(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	int *i =
	    (int *)(block_shared_chain(current_schain, lua_tointeger(L, 1)) +
		    lua_tointeger(L, 2));
	*i = lua_tointeger(L, 3);
	lua_pushinteger(L, *i);
	return 1;
}

int geti(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, 0);
		return 1;
	}
	int *i =
	    (int *)(block_shared_chain(current_schain, lua_tointeger(L, 1)) +
		    lua_tointeger(L, 2));
	lua_pushinteger(L, *i);
	return 1;
}

int writeb(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, ERR_SCHAIN_NOOBJ);
		return 1;
	}
	char *b =
	    block_shared_chain(current_schain, lua_tointeger(L, 1)) +
		    lua_tointeger(L, 2);
	*b = lua_tointeger(L, 3);
	lua_pushinteger(L, *b);
	return 1;
}

int getb(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, 0);
		return 1;
	}
	char *b =
	    block_shared_chain(current_schain, lua_tointeger(L, 1)) +
		    lua_tointeger(L, 2);
	lua_pushinteger(L, *b);
	return 1;
}

int calcstr(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, 0);
		return 1;
	}
	lua_pushinteger(L, calcstr_shared_chain(current_schain,
							(char *)lua_tostring(L, 1)));
	return 1;
}

int info(lua_State * L)
{
	if (current_schain == NULL) {
		lua_pushinteger(L, 0);
		return 1;
	}
	int *i = (int *)current_schain;
	i += lua_tointeger(L, 1);
	lua_pushinteger(L, *i);
	return 1;
}

extern "C" {
	static luaL_Reg libs[] = {
		{"create", create},
		{"dump", dump},
		{"load", load},
		{"remove", remove},
		{"link", link},
		{"select", select},
		{"write", write},
		{"loadtext", loadtext},
		{"exporttext", exporttext},
		{"read", read},
		{"del", del},
		{"getlen", getlen},
		{"status", status},
		{"available", available},
		{"calcstr", calcstr},
		{"iscend", iscend},
		{"isfstart", isfstart},
		{"writei", writei},
		{"writeb", writeb},
		{"geti", geti},
		{"getb", getb},
		{"info", info},
		{NULL, NULL}
	};

	int luaopen_schain(lua_State * L) {
		lua_newtable(L);
		luaL_setfuncs(L, libs, 0);
		return 1;
	}
}

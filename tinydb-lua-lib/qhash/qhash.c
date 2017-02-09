#include "string.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "../../include/tinylist.h"

int get_seed(lua_State * L)
{
	lua_pushinteger(L, LIST_qhash_get_seed());
	return 1;
}

int set_seed(lua_State * L)
{
	unsigned int seed = lua_tointeger(L, 1);
	LIST_qhash_set_seed(seed);
	return 1;
}

int hash(lua_State * L)
{
	char *str = (char*)lua_tostring(L, 1);
	unsigned int max = lua_tointeger(L, 2);
	unsigned int hash;
	hash =
	    LIST_qhash(str, LIST_qhash_get_seed(), strlen(str), max,
		       LIST_qhash_calc_offset(max));
	lua_pushinteger(L, hash);
	return 1;
}

static luaL_Reg libs[] = {
	{"get_seed", get_seed},
	{"set_seed", set_seed},
	{"hash", hash},
	{NULL, NULL}
};

int luaopen_qhash(lua_State * L)
{
	lua_newtable(L);
	luaL_setfuncs(L, libs, 0);
	return 1;
}

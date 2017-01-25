#define SLIST_POOL 16
#define READ_BUFFER_LEN 32768

#define ERR_SLIST_NOOBJ -71

#include "stdio.h"
#include "string.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "../../include/sharedlist.h"
#include "../../include/swaplist.h"

static char *current_slist = NULL;
static char *slist_pool[SLIST_POOL] = { NULL };

static char read_buffer[READ_BUFFER_LEN] = "\0";

int _create(lua_State * L)
{
	lua_pushinteger(L,
	create_shared_list(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3),
					(char *)lua_tostring(L, 4), (char *)lua_tostring(L, 5)));
	return 1;
}

int _link(lua_State * L)
{
	slist_pool[lua_tointeger(L, 1) - 1] =
	    link_shared_list_shm(lua_tointeger(L, 2), (char *)lua_tostring(L, 3));
	return 1;
}

int _select(lua_State * L)
{
	current_slist = slist_pool[lua_tointeger(L, 1) - 1];
	return 1;
}

int _dump(lua_State * L)
{
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	lua_pushinteger(L, dump_shared_list(current_slist, lua_tointeger(L, 1),
			    (char *)lua_tostring(L, 2)));
	return 1;
}

int _load(lua_State * L)
{
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	lua_pushinteger(L, load_shared_list(current_slist, (char *)lua_tostring(L, 1)));
	return 1;
}

int _remove(lua_State * L)
{
	lua_pushinteger(L, remove_shared_list(lua_tointeger(L, 1),
			    (char *)lua_tostring(L, 2)));
	return 1;
}

int _write(lua_State * L)
{
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	lua_pushinteger(L, list_write_string(current_slist + SHARED_LIST_FLAGS,
			lua_tointeger(L, 1), (char *)lua_tostring(L, 2)));
	return 1;
}

int _read(lua_State * L)
{
	if (current_slist == NULL) {
		read_buffer[0] = 0;
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	strcpy(read_buffer, list_get_record(current_slist + SHARED_LIST_FLAGS,
							lua_tointeger(L, 1)));
	lua_pushstring(L, read_buffer);
	return 1;
}

int _writeb(lua_State * L)
{
	unsigned char *rec;
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	rec = list_get_record(current_slist + SHARED_LIST_FLAGS, lua_tointeger(L, 1));
	rec += lua_tointeger(L, 2);
	*rec = lua_tointeger(L, 3);
	return 1;
}

int _getb(lua_State * L)
{
	unsigned char *rec;
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	rec = list_get_record(current_slist + SHARED_LIST_FLAGS, lua_tointeger(L, 1));
	rec += lua_tointeger(L, 2);
	lua_pushinteger(L, *rec);
	return 1;
}

int _writei(lua_State * L)
{
	int *integer;
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	integer = (int *)(list_get_record(current_slist + SHARED_LIST_FLAGS,
					lua_tointeger(L, 1)) + lua_tointeger(L, 2));
	*integer = lua_tointeger(L, 3);
	return 1;
}

int _geti(lua_State * L)
{
	int *integer;
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	integer = (int *)(list_get_record(current_slist + SHARED_LIST_FLAGS,
					lua_tointeger(L, 1)) + lua_tointeger(L, 2));
	lua_pushinteger(L, *integer);
	return 1;
}

int _writeauxi(lua_State * L)
{
	int *integer;
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	integer = (int *)(list_get_aux(current_slist + SHARED_LIST_FLAGS,
					lua_tointeger(L, 1)) + lua_tointeger(L, 2));
	*integer = lua_tointeger(L, 3);
	return 1;
}

int _getauxi(lua_State * L)
{
	int *integer;
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	integer = (int *)(list_get_aux(current_slist + SHARED_LIST_FLAGS,
					lua_tointeger(L, 1)) + lua_tointeger(L, 2));
	lua_pushinteger(L, *integer);
	return 1;
}

int _writeauxb(lua_State * L)
{
	unsigned char *rec;
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	rec = list_get_aux(current_slist + SHARED_LIST_FLAGS, lua_tointeger(L, 1));
	rec += lua_tointeger(L, 2);
	*rec = lua_tointeger(L, 3);
	return 1;
}

int _getauxb(lua_State * L)
{
	unsigned char *rec;
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	rec = list_get_aux(current_slist + SHARED_LIST_FLAGS, lua_tointeger(L, 1));
	rec += lua_tointeger(L, 2);
	lua_pushinteger(L, *rec);
	return 1;
}

int _del(lua_State * L)
{
	if (current_slist == NULL) {
		lua_pushinteger(L, ERR_SLIST_NOOBJ);
		return 1;
	}
	list_delete_record(current_slist + SHARED_LIST_FLAGS, lua_tointeger(L, 1));
	return 1;
}

static luaL_Reg libs[] = {
	{"create", _create},
	{"link", _link},
	{"select", _select},
	{"dump", _dump},
	{"load", _load},
	{"remove", _remove},
	{"write", _write},
	{"writeb", _writeb},
	{"writeauxb", _writeauxb},
	{"writei", _writei},
	{"writeauxi", _writeauxi},
	{"read", _read},
	{"getb", _getb},
	{"getauxb", _getauxb},
	{"geti", _geti},
	{"getauxi", _getauxi},
	{"del", _del},
	{NULL, NULL}
};

int luaopen_slist(lua_State * L) {
	lua_newtable(L);
	luaL_setfuncs(L, libs, 0);
	return 1;
}

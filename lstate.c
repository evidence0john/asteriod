#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "mongoose.h"
#include "lstate.h"

static lua_State *L;
const char *cfg_lState_home;
const char *cfg_lState_init_file;
struct http_message **lSmsg;
struct mg_connection **lScon;

void asteroid_lState_init()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	if (cfg_lState_init_file)
		luaL_dofile(L, cfg_lState_init_file);
	lScon = (struct mg_connection**)lua_newuserdata(L, sizeof(void*));
	lua_setglobal(L, LS_CONNECTION_NAME);
	lSmsg = (struct http_message**)lua_newuserdata(L, sizeof(void*));
	lua_setglobal(L, LS_HTTP_MSG_P_NAME);
}

void asteroid_lState_reset()
{
	lua_close(L);
	L = luaL_newstate();
	luaL_openlibs(L);
	if (cfg_lState_init_file)
		luaL_dofile(L, cfg_lState_init_file);
	lScon = (struct mg_connection**)lua_newuserdata(L, sizeof(void*));
	lua_setglobal(L, LS_CONNECTION_NAME);
	lSmsg = (struct http_message**)lua_newuserdata(L, sizeof(void*));
	lua_setglobal(L, LS_HTTP_MSG_P_NAME);
}

void asteroid_lState_close()
{
	lua_close(L);
}

void asteroid_lState_http_request_handler
	(struct mg_connection *c, struct http_message *hm)
{
	char script[2048];
	*lScon = c;
	*lSmsg = hm;
	sprintf(script, "%s%.*s", cfg_lState_home, (int)hm->uri.len, hm->uri.p);
	luaL_dostring(L, "pcall(__Accept__)");
	if (luaL_dofile(L, script)) {
		mg_send_head(c, 200, strlen(script) + 11, "Content-Type: text/plain");
		mg_printf(c, "Cannot run %s", script);
		luaL_dostring(L, "pcall(l.b_clear)");
	}
	luaL_dostring(L, "pcall(__Finished__)");
}
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "mongoose.h"
#include "handler.h"
#include "lstate.h"

static lua_State *L;
const  char *cfg_lState_home;
const  char *cfg_lState_init_file;
extern char *cfg_lState_keyword;
extern int lState_match_mod;
extern struct mg_serve_http_opts cfg_s_http_server_opts;

struct http_message **lSmsg;
struct mg_connection **lScon;

void asteroid_lState_init()
{
	char sbuf[256];
	L = luaL_newstate();
	luaL_openlibs(L);
	if (cfg_lState_init_file)
		luaL_dofile(L, cfg_lState_init_file);
	sprintf(sbuf, "Global.HtmlHome=\"%s\"",
		cfg_s_http_server_opts.document_root);
	luaL_dostring(L, sbuf);
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
	int offset = 0;
	int truncate = 0;
	switch (lState_match_mod) {
		case PREFIX:
			offset = strlen(cfg_lState_keyword);
			truncate = offset;
			break;
		case SUFFIX:
			//truncate = strlen(cfg_lState_keyword);
			break;
		default:
			break;
	}
	sprintf(script, "%s%.*s", cfg_lState_home,
		(int)hm->uri.len - truncate, hm->uri.p + offset);
	luaL_dostring(L, "pcall(__Accept__)");
	if (luaL_dofile(L, script)) {
		mg_send_head(c, 200, strlen(script) + 11, "Content-Type: text/plain");
		mg_printf(c, "Cannot run %s", script);
		luaL_dostring(L, "pcall(l.b_clear)");
	}
	luaL_dostring(L, "pcall(__Finished__)");
}
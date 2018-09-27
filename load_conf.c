#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "mongoose.h"

//ref: asteroid.c
extern int cfg_open_native;
extern int cfg_open_lState;
extern int lState_match_mod;
extern int native_match_mod;
extern const char *cfg_native_match;
extern const char *cfg_native_keyword;
extern const char *cfg_native_lib;
extern const char *s_http_port;
extern struct mg_serve_http_opts cfg_s_http_server_opts;

//ref: handler.c
extern const char *cfg_lState_match;
extern const char *cfg_lState_keyword;

//ref: lstate.c
extern const char *cfg_lState_home;
extern const char *cfg_lState_init_file;

static char *get_lua_str(lua_State *L, char *name)
{ 
	lua_getglobal(L, name);
	const char *p = lua_tostring(L, -1);
	if (!p)
		return NULL;
	char *s = (char*)malloc(strlen(p) + 1);
	strcpy(s, p);
	return s;
}

int load_conf(const char *filename)
{
	lua_State *L;
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL){
		fprintf(stderr, "OPEN_CONF_FAILED: %s", filename);
		return 0;
	}
	fclose(fp);
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L, filename);
	//Set port
	s_http_port = get_lua_str(L, "port");
	if (!s_http_port)
		return -1;
	//Set native app support
	cfg_native_lib = get_lua_str(L, "native_lib");
	if (!cfg_native_lib) {
		cfg_open_native = 0;
		cfg_native_match = NULL;
		cfg_native_keyword = NULL;
	} else {
		cfg_open_native = 1;
		cfg_native_match = get_lua_str(L, "native_match"); 
		if (!cfg_native_match)
			return -1;
		cfg_native_keyword = get_lua_str(L, "native_keyword");
		if (!cfg_native_keyword )
			return -1;
	}
	//Set local lua state support
	cfg_lState_match = get_lua_str(L, "lState_match");
	if (!cfg_native_match) {
		cfg_open_lState = 0;
		cfg_lState_keyword = NULL;
	} else {
		cfg_open_lState = 1;
		cfg_lState_keyword = get_lua_str(L, "lState_keyword");
		if (!cfg_native_keyword )
			return -1;
		cfg_lState_init_file = get_lua_str(L, "lState_init");
		cfg_lState_home = get_lua_str(L, "lState_home");
		if (!cfg_lState_home)
			return -1;
	}
	//Mongoose http server options
	cfg_s_http_server_opts.document_root =
				get_lua_str(L, "document_root");
	cfg_s_http_server_opts.index_files =
				get_lua_str(L, "index_files");
	cfg_s_http_server_opts.per_directory_auth_file = 
				get_lua_str(L, "per_directory_auth_file");
	cfg_s_http_server_opts.auth_domain =
				get_lua_str(L, "auth_domain");
	cfg_s_http_server_opts.global_auth_file =
				get_lua_str(L, "global_auth_file");
	cfg_s_http_server_opts.enable_directory_listing = 
				get_lua_str(L, "enable_directory_listing");
	cfg_s_http_server_opts.ssi_pattern = 
				get_lua_str(L, "ssi_pattern");
	cfg_s_http_server_opts.ip_acl =
				get_lua_str(L, "ip_acl");
#if MG_ENABLE_HTTP_URL_REWRITES
	cfg_s_http_server_opts.url_rewrites =
				get_lua_str(L, "url_rewrites");
#endif
	cfg_s_http_server_opts.dav_document_root =
				get_lua_str(L, "dav_document_root");
	cfg_s_http_server_opts.dav_auth_file =
				get_lua_str(L, "dav_auth_file");
	cfg_s_http_server_opts.hidden_file_pattern =
				get_lua_str(L, "hidden_file_pattern");
	cfg_s_http_server_opts.cgi_file_pattern =
				get_lua_str(L, "cgi_file_pattern");			
	cfg_s_http_server_opts.cgi_interpreter =
				get_lua_str(L, "cgi_interpreter");
	cfg_s_http_server_opts.custom_mime_types =
				get_lua_str(L, "custom_mime_types");
	cfg_s_http_server_opts.extra_headers =
				get_lua_str(L, "extra_headers");
	lua_close(L);
	return 0;
}

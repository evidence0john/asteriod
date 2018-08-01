#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "mongoose.h"
#include "lstate.h"

#define HTTP_MSG_BUF_LEN 1048576
#define HTTP_HOST_NAME   128
#define HTTP_CLIENT_UA   512
#define HTTP_LANG_LEN    128
#define HTTP_ENCODD_LEN  128
#define HTTP_ACCEPT_LEN  256
#define HTTP_CT_TYPE_LEN 256

#define DEFAULT_CONTENT_TYPE "Content-Type: text/html"

static lua_State *L;

static char http_msg_buf[HTTP_MSG_BUF_LEN];
static char *bp = http_msg_buf;
static unsigned int http_msg_len = 0;

static void segment_cpy(char *dst, const char *src,
	int ns, char c)
{
	char *p_start = (char*)src;
	while (ns--) {
		while (*p_start++ != c);
	}
	while (*dst++ = *p_start++)
		if (*p_start == c)
			break;
	*dst = 0;
}

static int segment_match(const char *src, char *key, char c)
{
	char *p = (char*)src;
	char *key_p = NULL;
	goto __1st_cycle;
__re_match:
	while (*p++ != c)
		if (!*p)
			return -2;
__1st_cycle:
	key_p = key;
	while (*key_p) {
		if (*key_p++ != *p++) {
			if (!*(p - 1))
				return -1;
			else
				goto __re_match;
		}
	}
	return (int)(p - src);
}

/*int module_init(lua_State *L)
{
	return 0;
}*/

int http_msg_buf_echo(lua_State *L)
{
	char *p = NULL;
	int i = 1;
	while (lua_tostring(L, i)) {
		bp = http_msg_buf + http_msg_len;
		p = (char*)lua_tostring(L, i++);
		strcpy(bp, p);
		http_msg_len += strlen(p);
	}
	return 0;
}

int http_msg_buf_flush(lua_State *L)
{
	lua_getglobal(L, LS_CONNECTION_NAME);
	struct mg_connection *c = *(struct mg_connection **)lua_touserdata(L, -1);
	const char *ct = lua_tostring(L, 1);
	if (!ct) {
		if (http_msg_len)
			mg_send_head(c, 200, http_msg_len, DEFAULT_CONTENT_TYPE);
		else
			mg_send_head(c, 200, 0, DEFAULT_CONTENT_TYPE);
	} else {
		mg_send_head(c, 200, http_msg_len, ct);
		mg_send(c, http_msg_buf, http_msg_len);
	}
	http_msg_len = 0;
	return 0;
}

int http_raw(lua_State *L)
{
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	lua_pushlstring(L, hm->message.p, hm->message.len);
	return 1;
}

int http_body(lua_State *L)
{
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	lua_pushlstring(L, hm->body.p, hm->body.len);
	return 1;
}

int http_uri(lua_State *L)
{
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	lua_pushlstring(L, hm->uri.p, hm->uri.len);
	return 1;
}

int http_content_len(lua_State *L)
{
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	lua_pushinteger(L, hm->body.len);
	return 1;
}

int http_method(lua_State *L)
{
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	lua_pushlstring(L, hm->method.p, hm->method.len);
	return 1;
}

int http_query_string(lua_State *L)
{
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	lua_pushlstring(L, hm->query_string.p, hm->query_string.len);
	return 1;
}

int http_proto(lua_State *L)
{
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	lua_pushlstring(L, hm->proto.p, hm->proto.len);
	return 1;
}

int http_host(lua_State *L)
{
	char host_name[HTTP_HOST_NAME];
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	segment_cpy(host_name, hm->message.p, 1, '\n');
	lua_pushstring(L, host_name + 6); // 6 equ to strlen("HOST: ")
	return 1;
}

int http_user_agent(lua_State *L)
{
	char ua[HTTP_CLIENT_UA];
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	int i = segment_match(hm->message.p, "User-Agent: ", '\n');
	if (i >=0 ) {
		segment_cpy(ua, hm->message.p + i, 0, '\n');
		lua_pushstring(L, ua);
	} else
		lua_pushnil(L);
	return 1;
}

int http_encoding(lua_State *L)
{
	char encoding[HTTP_ENCODD_LEN];
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	int i = segment_match(hm->message.p, "Accept-Encoding: ", '\n');
	if (i >=0 ) {
		segment_cpy(encoding, hm->message.p + i, 0, '\n');
		lua_pushstring(L, encoding);
	} else
		lua_pushnil(L);
	return 1;
}

int http_language(lua_State *L)
{
	char lang[HTTP_LANG_LEN];
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	int i = segment_match(hm->message.p, "Accept-Language: ", '\n');
	if (i >=0 ) {
		segment_cpy(lang, hm->message.p + i, 0, '\n');
		lua_pushstring(L, lang);
	} else
		lua_pushnil(L);
	return 1;
}

int http_accept(lua_State *L)
{
	char accept[HTTP_ACCEPT_LEN];
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	int i = segment_match(hm->message.p, "Accept: ", '\n');
	if (i >=0 ) {
		segment_cpy(accept, hm->message.p + i, 0, '\n');
		lua_pushstring(L, accept);
	} else
		lua_pushnil(L);
	return 1;
}

int http_origin(lua_State *L)
{
	char origin[HTTP_HOST_NAME];
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	int i = segment_match(hm->message.p, "Origin: ", '\n');
	if (i >=0 ) {
		segment_cpy(origin, hm->message.p + i, 0, '\n');
		lua_pushstring(L, origin);
	} else
		lua_pushnil(L);
	return 1;
}

int http_content_type(lua_State *L)
{
	char type[HTTP_HOST_NAME];
	lua_getglobal(L, LS_HTTP_MSG_P_NAME);
	struct http_message *hm = *(struct http_message **)lua_touserdata(L, -1);
	int i = segment_match(hm->message.p, "Content-Type: ", '\n');
	if (i >=0 ) {
		segment_cpy(type, hm->message.p + i, 0, '\n');
		lua_pushstring(L, type);
	} else
		lua_pushnil(L);
	return 1;
}

static luaL_Reg libs[] = {
	/*
	Returns full http request message
	*/
	{"raw", http_raw},
	/*
	Returns http message body
	*/
	{"body", http_body},
	/*
	Returns http URI
	*/
	{"uri", http_uri},
	/*
	Returns content length
	*/
	{"content_len", http_content_len},
	/*
	Returns request method
	*/
	{"method", http_method},
	/*
	Returns request query_string
	*/
	{"query", http_query_string},
	/*
	Returns request http protocol
	*/
	{"proto", http_proto},
	/*
	Returns host info
	*/
	{"host", http_host},
	/*
	Returns user agent
	*/
	{"ua", http_user_agent},
	/*
	Returns Accept-Encoding
	*/
	{"encoding", http_encoding},
	/*
	Returns Accept-Language
	*/
	{"lang", http_language},
	/*
	Returns Accept
	*/
	{"accept", http_accept},
	/*
	Returns Origin, it shows where the client post from
	*/
	{"origin", http_origin},
	/*
	Returns Content type
	*/
	{"content_type", http_content_type},
	/*
	Usage: b_echo([str1, str2, ...])
	Write string to buffer(http_msg_buf)
	*/
	{"b_echo", http_msg_buf_echo},
	/*
	Usage: b_flush([content_type])
	*/
	{"b_flush", http_msg_buf_flush},
	{NULL, NULL}
};

int luaopen_lmodule(lua_State * L) {
	lua_newtable(L);
	luaL_setfuncs(L, libs, 0);
	return 1;
}
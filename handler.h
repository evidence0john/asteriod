/***************************************************************/
//Asteroid
/***************************************************************/
/*
HOST_
     |_native (by loading shared object module)
     |_lua_State (local lua State)
     |_mongoose_http (mongoose http, ref: mongoose.c)
*/

#define ASTEROID_ERR_CONFIG  100
#define ASTEROID_ERR_LOAD_SO 101

#define ASTEROID_HTTP_ORIGINAL 00
#define ASTEROID_HTTP_FULL_ON  11
#define ASTEROID_HTTP_WITH_NAV 01
#define ASTEROID_HTTP_WITH_LUA 10

typedef void* function;

void asteroid_handler(struct mg_connection *c, int ev, void *p);
void request_scheduler(struct mg_connection *c, struct http_message *hm);
int mg_str_prefix_match(struct mg_str *s, const char *prefix);
int mg_str_suffix_match(struct mg_str *s, const char *suffix);
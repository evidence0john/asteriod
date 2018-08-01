#include "mongoose.h"
#include "handler.h"
#include "lstate.h"

extern int cfg_asteroid_http_mode;
const char *cfg_native_keyword;
const char *cfg_lState_keyword;
struct mg_serve_http_opts cfg_s_http_server_opts;

extern void (*asteroid_native_http_request_handler)
	(struct mg_connection *c, struct http_message *hm);

//extern void (*asteroid_lState_http_request_handler)
//	(struct mg_connection *c, struct http_message *hm);

extern int (*native_match)(struct mg_str *, const char*);
extern int (*lState_match)(struct mg_str *, const char*);

void request(struct mg_connection *c, struct http_message *hm)
{
	switch (cfg_asteroid_http_mode) {
		case ASTEROID_HTTP_FULL_ON:
			if (!native_match(&hm->uri, cfg_native_keyword))
					asteroid_native_http_request_handler(c, hm);
				else if (!lState_match(&hm->uri, cfg_lState_keyword))
					asteroid_lState_http_request_handler(c, hm);
				else
					mg_serve_http(c, hm, cfg_s_http_server_opts);
			break;
		case ASTEROID_HTTP_ORIGINAL:
			mg_serve_http(c, hm, cfg_s_http_server_opts);
			break;
		case ASTEROID_HTTP_WITH_NAV:
			if (!native_match(&hm->uri, cfg_native_keyword))
					asteroid_native_http_request_handler(c, hm);
			else
				mg_serve_http(c, hm, cfg_s_http_server_opts);
			break;
		case ASTEROID_HTTP_WITH_LUA:
			if (!native_match(&hm->uri, cfg_lState_keyword))
					asteroid_lState_http_request_handler(c, hm);
			else
				mg_serve_http(c, hm, cfg_s_http_server_opts);
			break;
	}
}


void asteroid_handler(struct mg_connection *c, int ev, void *p)
{
	if (ev == MG_EV_HTTP_REQUEST) {
		struct http_message *hm = (struct http_message *) p;
		request(c, hm);
	}
}

int mg_str_prefix_match(struct mg_str *s, const char *prefix)
{
	const char *p = s->p;
	while (*prefix)
		if (*prefix++ != *p++)
			return -1;
	return 0;
}

int mg_str_suffix_match(struct mg_str *s, const char *suffix)
{
	unsigned int count = strlen(suffix);
	return memcmp(s->p + s->len - count, suffix, count);
}
#include <dlfcn.h>

#include "mongoose.h"
#include "handler.h"
#include "lstate.h"

//load configuration from a specified file and initialize a asteroid host
void asteroid_host_init(const char *cfg_file);
struct mg_mgr *asteroid_http_start();
//http polling for asteroid http server
void asteroid_http_poll(struct mg_mgr *mgr);

int main(int argc, char const *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Please specify a configuration file\n");
		fprintf(stderr, "e.g. asteroid /path/config.lua\n");
		exit(0);
	}
	asteroid_host_init(argv[1]);
	struct mg_mgr *mgr = asteroid_http_start();
	asteroid_http_poll(mgr);
	return 0;
}

void asteroid_http_poll(struct mg_mgr *mgr)
{
	for (;;) {
		mg_mgr_poll(mgr, 1000);
	}
	mg_mgr_free(mgr);
	free(mgr);
}

//ref: load_conf
extern int load_conf(const char *filename);

/*Configure asteroid*/
int cfg_open_native;
int cfg_open_lState;
int cfg_asteroid_http_mode;
const char *s_http_port;
const char *cfg_native_match;
const char *cfg_lState_match;
//Moved to handler.c
//const char *cfg_native_keyword;
//const char *cfg_lState_keyword;
const char *cfg_native_lib;
int (*native_match)(struct mg_str *, const char*);
int (*lState_match)(struct mg_str *, const char*);

void *asteroid_native_lib_handle;

void (*asteroid_native_http_request_handler)
	(struct mg_connection *c, struct http_message *hm);

void asteroid_host_init(const char *cfg_file)
{
	load_conf(cfg_file);
	//Http app mode
	if (cfg_open_native && cfg_open_lState)
		cfg_asteroid_http_mode = ASTEROID_HTTP_FULL_ON;
	else if (!cfg_open_native && !cfg_open_lState)
		cfg_asteroid_http_mode = ASTEROID_HTTP_ORIGINAL;
	else if (!cfg_open_native && cfg_open_lState)
		cfg_asteroid_http_mode = ASTEROID_HTTP_WITH_LUA;
	else if (!cfg_open_lState && cfg_open_native)
		cfg_asteroid_http_mode = ASTEROID_HTTP_WITH_NAV;
	else {
		fprintf(stderr, "init fault %d\n", ASTEROID_ERR_CONFIG);
		exit(ASTEROID_ERR_CONFIG);
	}
	//If support native app 
	if (cfg_open_native) {
		if (!strcmp(cfg_native_match, "prefix"))
			native_match = mg_str_prefix_match;
		else if (!strcmp(cfg_native_match, "suffix"))
			native_match = mg_str_suffix_match;
		else {
			fprintf(stderr, "init fault %d\n", ASTEROID_ERR_CONFIG);
			exit(ASTEROID_ERR_CONFIG);
		}
		asteroid_native_lib_handle = dlopen(cfg_native_lib, RTLD_LAZY);
		if (!asteroid_native_lib_handle) {
			fprintf (stderr, "%s\n", dlerror());
			exit(ASTEROID_ERR_LOAD_SO);
		}
		asteroid_native_http_request_handler = dlsym(asteroid_native_lib_handle,
			"asteroid_native_http_request_handler");
	}
	//If support local lua state
	if (cfg_open_lState) {
		if (!strcmp(cfg_lState_match, "prefix"))
			lState_match = mg_str_prefix_match;
		else if (!strcmp(cfg_lState_match, "suffix"))
			lState_match = mg_str_suffix_match;
		else {
			fprintf(stderr, "init fault %d\n", ASTEROID_ERR_CONFIG);
			exit(ASTEROID_ERR_CONFIG);
		}
		//Initialize local lua state
		asteroid_lState_init();
	}
}

struct mg_mgr *asteroid_http_start()
{	
	//Create mongoose http server
	struct mg_mgr *mgr = (struct mg_mgr*)malloc(sizeof(struct mg_mgr));
	struct mg_connection *c;
	mg_mgr_init(mgr, NULL);
	c = mg_bind(mgr, s_http_port, asteroid_handler);
	if (c == NULL) {
		fprintf(stderr, "Failed to create listener\n");
		exit(-1);
	}
	mg_set_protocol_http_websocket(c);
	printf("Starting web server on port %s\n", s_http_port);
	return mgr;
}
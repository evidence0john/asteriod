#define LS_HTTP_MSG_P_NAME "__mg_http_msg"
#define LS_CONNECTION_NAME "__mg_connection"

void asteroid_lState_init();
void asteroid_lState_reset();
void asteroid_lState_close();
void asteroid_lState_http_request_handler
	(struct mg_connection *c, struct http_message *hm);
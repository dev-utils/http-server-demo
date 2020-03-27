#include "mongoose.h"
#include <string>
static const char *s_http_port = "8001";
static struct mg_serve_http_opts s_http_server_opts;

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
	if (ev == MG_EV_HTTP_REQUEST) {
		http_message *mg = (struct http_message *) p;
		std::string req_uri = "";
		if ((NULL != mg)
			&& (mg->uri.len > 0)
			&& (mg->message.len > 0))
		{
			int offset = (mg->uri.p[0] == '/') ? 1 : 0;
			req_uri.clear();
			req_uri.append(mg->uri.p, offset, mg->uri.len - offset);
		}

		printf("%s\n", req_uri.c_str());

		mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);
	}
}

int main(void) {
	struct mg_mgr mgr;
	struct mg_connection *nc;

	mg_mgr_init(&mgr, NULL);
	printf("Starting web server on port %s\n", s_http_port);
	nc = mg_bind(&mgr, s_http_port, ev_handler);
	if (nc == NULL) {
		printf("Failed to create listener\n");
		return 1;
	}

	// Set up HTTP server parameters
	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.document_root = ".";  // Serve current directory
	s_http_server_opts.enable_directory_listing = "yes";

	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return 0;
}
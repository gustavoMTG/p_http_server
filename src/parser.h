/*
Parse module for extracting the data from the requests buffers.
*/

struct Request {
    const char *req_line_ptr;
	int req_line_len;
    const char *headers_ptr;
	int headers_len;

	const char *method_ptr;
	int method_len;
	const char *uri_ptr;
	int uri_len;
	const char *httpv_ptr;
	int httpv_len;
};

int request_parser(struct Request *req, const char buffer[], 
				   int buffer_len);


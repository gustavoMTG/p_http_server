/*
Parse module for extracting the data from the requests buffers.
*/

struct Request {
	char *method_ptr;
	char *uri_ptr;
	char *httpv_ptr;
    char *headers_ptr;
};

char *request_parser(struct Request *req, char buffer[], 
				   int buffer_len);


/*
	Parse module for extracting the data from the requests buffers.
*/

struct Request {
	char *method_ptr;
	char *uri_ptr;
	char *httpv_ptr;
    char **headers_ptr;
	int headers_qty;
};

struct Request *request_parser(char *buff, char buffer[], int buffer_len);


/*
 * h_*: refers to a header
 */

struct Header {
	char *name;
	char *value;
};

struct Response {
	char *httpv_ptr;
	char *statuscode_ptr;
	char *reasonp_ptr;

	struct Header *headers_ptr;
	int headers_count;

	char *messagebody_ptr;
};


char *response_gen(struct Request *);
struct Response *create_response(void);

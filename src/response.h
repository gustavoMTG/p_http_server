struct Response {
	char *httpv_ptr;
	char *statuscode_ptr;
	char *reasonp_ptr;
	char *messagebody_ptr;
};


char *response_gen(struct Request *);

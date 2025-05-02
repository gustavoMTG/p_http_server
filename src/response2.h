#ifndef RESPONSE_H
#define RESPONSE_H

#include "request.h"

#define MAX_MBLENGTH 4096

typedef struct {
	char *httpv;
	char *statuscode;
	char *reasonp;

	Header *headers;
	int headers_count;

	char messagebody[MAX_MBLENGTH];
} Response;

Response *request2response(Request *req);
void free_response(Response *res);

#endif

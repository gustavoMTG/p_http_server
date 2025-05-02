#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "response2.h"
#include "logging.h"


static Response *init_response(void)
{
	Response *res = malloc(sizeof(Response));
	res->httpv = NULL;
	res->statuscode = NULL;
	res->reasonp = NULL;

	res->headers_count = 0;
	memset(res->messagebody, 0, sizeof(res->messagebody));
	return res;
}

void free_response(Response *res)
{
	free(res->headers);
	res->headers = NULL;
	free(res);
	res = NULL;
}

void add_header(Response *res, char *name, char *value)
{
	res->headers_count++;
	if (res->headers_count == 1)
		res->headers = malloc(res->headers_count * sizeof(Header));
	else
		realloc(res->headers, res->headers_count * sizeof(Header));

	res->headers[res->headers_count-1].name = strdup(name);
	res->headers[res->headers_count-1].value = strdup(value);
}

Response *request2response(Request *req)
{
	// 1. create, allocate and initialize response datatype
	Response *res = init_response();
	char *header_value;

	// 2. read request data structure and interpret
	if (strncmp("HTTP/1.1", req->httpv, 8) == 0) {
		LOG_DEBUG("Request is same HTTP version as server");

		if (strncmp("GET", req->method, 3) == 0) {
			FILE *file = fopen(req->uri + 1, "rb");
			if (!file) {
				// TODO: handle missing file
				perror("fopen");
				res->httpv = "HTTP/1.1";
				res->statuscode = "400";
				res->reasonp = "ResourceNotFound";
				return res;
			}

			fseek(file, 0, SEEK_END);
			long filesize = ftell(file);
			rewind(file);
			size_t read_size = fread(res->messagebody, 1, filesize, file);
			fclose(file);
			asprintf(&header_value, "%ld", read_size);
			add_header(res, "Content-length", header_value);
			free(header_value);

			res->httpv = "HTTP/1.1";
			res->statuscode = "200";
			res->reasonp = "Ok";
		}

	} else {
		// TODO: Handle different HTTP version
		;
	}

	// 3. fill response fileds and return structure
	
	return res;
}

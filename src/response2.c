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

	// 2. read request data structure and fill fields
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
				add_header(res, "Content-length", "0");
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

	return res;
}

char *response2buffer(Response *res)
{
	/*
	 * Return a buffer pointer with allocated memory for the string.
	 * Caller is responsible of freeing this memory.
	 */

	char *res_buff;
	int i = 0;
	int curr_position = 0;
	int res_size = 0;
	int status_line_size = 0;

	// Add lengths of HTTP version, status code, reason phrase
	// and the 2 SP in between.
	status_line_size = strlen(res->httpv) + strlen(res->statuscode)
		+ strlen(res->reasonp) + 2;
	res_size = status_line_size;

	// Add headers lengths and 2 for each CRLF sequence at the end
	// of each header and +2 for the ": " separator between name and value.
	for (i=0; i<res->headers_count; i++) {
		res_size += strlen(res->headers[i].name) 
			+ strlen(res->headers[i].value) + 4;
	}

	// Add message body size plus 4 for the final CRLFCRLF sequence
	res_size += strlen(res->messagebody) + 4;

	// Allocate memory for the full buffer.
	// +1 for EOF just for security.
	res_buff = malloc(res_size + 1);
	LOG_DEBUG("Response size: %d", res_size);

	// Place each part in the response buffer
	snprintf(res_buff, status_line_size+1, "%s %s %s", 
			res->httpv, res->statuscode, res->reasonp);
	curr_position = status_line_size;
	for (i=0; i<res->headers_count; i++) {
		snprintf(res_buff+curr_position, res_size-curr_position+1, 
				"\r\n%s: %s", res->headers[i].name, res->headers[i].value);
		curr_position += strlen(res->headers[i].name) 
			+ strlen(res->headers[i].value) + 4;
	}
	if (res->messagebody)
		snprintf(res_buff+curr_position, res_size-curr_position+1, 
				"\r\n\r\n%s", res->messagebody);

	return res_buff;
}

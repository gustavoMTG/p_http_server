/*
 * Response generation module
 *
 * After receiving and interpreting a request message, a server responds
 * with an HTTP response message.
 *
 *     Response      = Status-Line               
 *                     *(( general-header        
 *                      | response-header        
 *                      | entity-header ) CRLF)  
 *                     CRLF
 *                     [ message-body ]          
 *
 * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
 */

#include "parser.h"	// For Request structure
#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Response *create_response(void)
{
	/*
	 * Create and return a struct Response with default values.
	 */

	struct Response *res = malloc(sizeof(struct Response));
	if (!res) return NULL;
	res->httpv_ptr = "HTTP/1.1";
	res->statuscode_ptr = "200";
	res->reasonp_ptr = "Ok";
	res->headers_ptr = NULL;
	res->headers_count = 0;
	res->messagebody_ptr = NULL;
	return res;
}

static int file2mbody(struct Response *res, FILE *file, ssize_t size)
{
	/*
	 * Read a file and copy the content to the message body.
	 * This method allocates size + 1 bytes of memory, the +1 is
	 * for EOL.
	 */

	size_t read_size;

	res->messagebody_ptr = malloc(size + 1);
	if (!res->messagebody_ptr)
		perror("malloc");
	read_size = fread(res->messagebody_ptr, 1, size, file);
	if (read_size != (size_t)size)
		printf("Size and read_size are not equal\n");
	res->messagebody_ptr[size] = '\0';

	// Content-Length header needs to be included
	res->headers_ptr = malloc(sizeof(struct Header *));
	res->headers_ptr->name = "Content-length";
	res->headers_ptr->value = "";
	res->headers_count++;
	return read_size;
}

static struct Response *tokenizer(struct Request *req)
{
	/*
	 * From a Request structure this function returns a response 
	 * appropriate for the request received.
	 */

	struct Response *res = create_response();
	size_t size;
	char *uri;
	FILE *uri_file;
	int ch;

	if (strncmp(req->method_ptr, "GET", 3) == 0) {

		uri = req->uri_ptr + 1;	// Unsafe, more checks are required
		uri_file = fopen(uri, "r");
		if (uri_file == NULL) {
			perror("fopen");
			// Resource not found?
			return NULL;
		}

		// Get the size of the file
		fseek(uri_file, 0, SEEK_END);
		size = ftell(uri_file);
		rewind(uri_file);

		// Copy file data to message body
		file2mbody(res, uri_file, size);

		fclose(uri_file);
	}

	return res;
}

char *response_gen(struct Request *req)
{
	struct Response *res;
	char *char_res;
	int i;
	long space, spacing = 0;
	long httpv_size, statuscode_size, reasonp_size, messagebody_size;

	res = tokenizer(req);

	httpv_size = strlen(res->httpv_ptr);
	statuscode_size = strlen(res->statuscode_ptr);
	reasonp_size = strlen(res->reasonp_ptr);
	messagebody_size = strlen(res->messagebody_ptr);
	// 8 characters because of spaces and CRLFs
	space = 8 + httpv_size + statuscode_size + reasonp_size 
		+ messagebody_size;

	char_res = calloc(space, sizeof(char));
	
	// HTTP version
	strncpy(char_res, res->httpv_ptr, httpv_size);
	spacing = httpv_size;
	strncpy(char_res + spacing, " ", 1);
	// Status code
	spacing++;
	strncpy(char_res + spacing, res->statuscode_ptr, statuscode_size);
	spacing += statuscode_size;
	strncpy(char_res + spacing, " ", 1);
	// Reason phrase
	spacing++;
	strncpy(char_res + spacing, res->reasonp_ptr, reasonp_size);

	// Headers
	for (i=0; i<res->headers_count; i++) {
		printf("We are in header number %d\n", i);
		printf("header is: %s\n", res->headers_ptr[i].name);
	}

	// CRLF CRLF sequence
	spacing += reasonp_size;
	strncpy(char_res + spacing, "\r\n\r\n", 4);
	// Body
	spacing += 4;
	strncpy(char_res + spacing, res->messagebody_ptr, messagebody_size);
	// strncpy(char_res + httpv_size + statuscode_size + reasonp_size 
	// 		+ messagebody_size + 6, "\r\n\r\n", 4);

	return char_res;
}


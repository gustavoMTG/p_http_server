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
#include <string.h>


static struct Response *tokenizer(struct Request *req)
{
	struct Response *res;
	char *uri;
	FILE *uri_file;
	int ch;
	long size, file_counter;

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
		// And reserve memory for message body
		res = malloc(sizeof(struct Response *));
		res->messagebody_ptr = malloc(size + 1); // +1 for EOL
		if (res->messagebody_ptr == NULL) {
			perror("malloc");
			fclose(uri_file);
			return NULL;
		}

		file_counter = 0;
		while ((ch = fgetc(uri_file)) != EOF) {
			*(res->messagebody_ptr + file_counter) = ch;
			file_counter++;
		}

		fclose(uri_file);

		// Provide HTTP version
		res->httpv_ptr = "HTTP/1.1";

		// Provide status code
		res->statuscode_ptr = "200";

		// Provide reason phrase
		res->reasonp_ptr = "Ok";
	}

	return res;
}

char *response_gen(struct Request *req)
{
	struct Response *res;
	char *char_res;
	long space;
	long httpv_size;
	long statuscode_size;
	long reasonp_size;
	long messagebody_size;

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
	strncpy(char_res + httpv_size, " ", 1);
	// Status code
	strncpy(char_res + httpv_size + 1, 
			res->statuscode_ptr, statuscode_size);
	strncpy(char_res + httpv_size + statuscode_size + 1, " ", 1);
	// Reason phrase
	strncpy(char_res + httpv_size + statuscode_size + 2, 
			res->reasonp_ptr, reasonp_size);
	strncpy(char_res + httpv_size + statuscode_size + reasonp_size + 2, 
			"\r\n\r\n", 4);
	// Body
	strncpy(char_res + httpv_size + statuscode_size + reasonp_size + 6, 
			res->messagebody_ptr, messagebody_size);
	// strncpy(char_res + httpv_size + statuscode_size + reasonp_size 
	// 		+ messagebody_size + 6, "\r\n\r\n", 4);

	return char_res;
}


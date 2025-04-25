/*
  Parse module for extracting the data from the requests buffers.

  A request message from a client to a server includes, within the
  first line of that message, the method to be applied to the resource,
  the identifier of the resource, and the protocol version in use.

  Request-Line   = Method SP Request-URI SP HTTP-Version CRLF

       Request       = Request-Line            
                       *(( general-header      
                        | request-header       
                        | entity-header ) CRLF)
                       CRLF
                       [ message-body ]        
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"


static char *tokenizer(char *buff, char buffer[], int buffer_len)
{
	/*
	Returns pointer to NULL terminated string in a separate buffer 
	for each token found in buffer within buffer_len.

	Memory is allocated by callee for the tokens, and the caller is 
	responsible of freeing this region.

	Care must be taken for threading and reentrancy because of state
	stored in static variables.	
	*/

	static int ib_size = 0;		// internal buffer size
	static char *ib_ptr; 		// internal buffer pointer
	static char *init_ib_ptr;	// remember ib_ptr to free
	static char *init_b_ptr;	// also remember the original buffer
	static int crlf_counter = 0;
	int i;

	// Working with the same buffer?
	if (ib_size != buffer_len || init_b_ptr != buffer) {
		ib_size = buffer_len;
		// free(init_ib_ptr);
		ib_ptr = (char*)malloc(ib_size);
		buff = ib_ptr;				// Caller ptr, caller responsible.
		init_ib_ptr = ib_ptr;
		init_b_ptr = buffer;
		memcpy(ib_ptr, buffer, ib_size);
	}

	// Tokenizing
	for (i=0, ib_ptr; ib_ptr-init_ib_ptr<ib_size; ib_ptr++, i++) {
		if (strncmp("\r\n\r\n", ib_ptr, 4) == 0) {
			*ib_ptr = '\0';
			ib_ptr++;
			*ib_ptr = '\0';
			ib_ptr++;
			*ib_ptr = '\0';
			ib_ptr++;
			*ib_ptr = '\0';
			return ib_ptr++ - i - 3;
		} else if (strncmp("\r\n", ib_ptr, 2) == 0) {
			crlf_counter++;
			*ib_ptr = '\0';
			ib_ptr++;
			*ib_ptr = '\0';
			return ib_ptr++ - i - 1;
		} else if (*ib_ptr == ' ' && crlf_counter == 0) {
			*ib_ptr = '\0';
			return ib_ptr++ - i;
		}
	}
	
	// Clean up and get ready for fresh input
	// free(init_ib_ptr);
	ib_ptr = NULL;
	init_ib_ptr = NULL;
	init_b_ptr = NULL;
	ib_size = 0;
	crlf_counter = 0;

	return NULL;
}

struct Request *request_parser(char *buff, char buffer[], int buffer_len)
{
	/*
	 * Returns a Request structure pointer with the data in buffer
	 * parsed until buffer_len. Memory is allocated by callee, caller 
	 * is responsible of freeing the memory in buff.
	 *
	 * Returns NULL upon an invalid HTTP format.
	 */

	int token_counter = 0;
	int headers_counter = 0;
	char *token_ptr;
	struct Request *req = malloc(sizeof(struct Request));
	req->headers_ptr = malloc(sizeof(char *));

	// TODO: Return NULL upon invalid format
	token_ptr = tokenizer(buff, buffer, buffer_len);
	while (token_ptr != NULL) {
		switch (token_counter) {
			case 0:
				req->method_ptr = token_ptr;
				break;
			case 1:
				req->uri_ptr = token_ptr;
				break;
			case 2:
				req->httpv_ptr = token_ptr;
				break;
			default:
				req->headers_ptr[headers_counter] = token_ptr;
				headers_counter++;
				req->headers_ptr = realloc(req->headers_ptr, (headers_counter + 1)*sizeof(char *));
		}

		// printf("Found: %s\n", token_ptr);
		token_ptr = tokenizer(buff, buffer, buffer_len);
		token_counter++;
	}
	req->headers_qty = headers_counter;

	return req;
}

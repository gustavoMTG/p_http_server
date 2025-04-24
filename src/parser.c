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


static char *tokenizer(char buffer[], int buffer_len)
{
	/*
	Returns pointer to NULL terminated string in a separate buffer 
	for each token found in buffer within buffer_len.

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
		free(init_ib_ptr);
		ib_ptr = (char*)malloc(ib_size);
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
	
	free(init_ib_ptr);
	ib_ptr = NULL;
	init_ib_ptr = NULL;
	init_b_ptr = NULL;
	ib_size = 0;
	crlf_counter = 0;

	return NULL;
}

char *request_parser(struct Request *req, char buffer[], int buffer_len)
{
	char *token_ptr;

	token_ptr = tokenizer(buffer, buffer_len);
	while (token_ptr != NULL) {
		printf("Found: %s\n", token_ptr);
		token_ptr = tokenizer(buffer, buffer_len);
	}

}

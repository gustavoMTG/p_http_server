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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "request.h"
#include "logging.h"


Request *init_request(void)
{
	Request *req = malloc(sizeof(Request));
	req->method = NULL;
	req->uri = NULL;
	req->httpv = NULL;
	req->headers_qty = 0;
	for (int i=0; i<MAX_HEADERS; i++) {
		req->headers[i].name = NULL;
		req->headers[i].value = NULL;
	}
	return req;
}

void free_request2(Request *req)
{
	free(req);
}

void request2struct(Request *req, char *buffer, int buff_size)
{
	int sp_counter = 0;			// sps per line
	int crlf_counter = 0;
	int crlfcrlf_counter = 0;

	req->method = buffer;
	for (int i=1; i<buff_size-4; i++) {
		if (buffer[i] == ' ')
			sp_counter++;
		else if (strncmp("\r\n\r\n", buffer+i, 4) == 0)
			crlfcrlf_counter++;
		else if (strncmp("\r\n", buffer+i, 2) == 0) {
			crlf_counter++;
			sp_counter = 0;
		}

		if (crlf_counter < 1 && crlfcrlf_counter == 0) {
			switch (sp_counter) {
				case 1:
					if (req->uri == NULL) {
						buffer[i] = '\0';
						req->uri = buffer+i+1;
					}
					break;
				case 2:
					if (req->httpv == NULL) {
						buffer[i] = '\0';
						req->httpv = buffer+i+1;
					}
					break;
			}
		} else if (crlfcrlf_counter == 0) {
			switch (sp_counter) {
				case 0:
					if (req->headers[crlf_counter-1].name == NULL) {
						req->headers_qty++;
						buffer[i] = '\0';
						buffer[i+1] = '\0';
						req->headers[crlf_counter-1].name = buffer+i+2;
					}
					break;
				case 1:
					if (req->headers[crlf_counter-1].value == NULL) {
						buffer[i] = '\0';
						buffer[i-1] = '\0';
						req->headers[crlf_counter-1].value = buffer+i+1;
					}
					break;
			}
		} else {
			LOG_DEBUG("Hit CRLFCRLF sequence: %d", crlfcrlf_counter);
			buffer[i] = '\0';
			buffer[i+1] = '\0';
			buffer[i+2] = '\0';
			buffer[i+3] = '\0';
			break;
		}
	}

}

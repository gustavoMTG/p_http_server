/*
Parse module for extracting the data from the requests buffers.
*/

#include <string.h>
#include "parser.h"


int request_parser(struct Request *req, const char buffer[], 
				   int buffer_len)
{
	int sp_counter, crlf_counter;
	int i;

	req->req_line_ptr = buffer;
	req->method_ptr = buffer;
	sp_counter = 0;
	crlf_counter = 0;
	for (i=0; i<buffer_len; i++) {
		if (strncmp(buffer+i, "\r\n", 2) == 0) {
			crlf_counter++;
			// printf("CRLF (%d) at: %d\n", crlf_counter, i);
			switch (crlf_counter) {
				case 1:
					req->req_line_len = i;
					req->httpv_len = i - (req->httpv_ptr - buffer);
					break;
				case 2:
					req->headers_ptr = buffer + i;
					break;
				default:
					break;
			}
		} else if (buffer[i] == ' ') {
			sp_counter++;
			switch (sp_counter) {
				case 1:
					req->method_len = i;
					req->uri_ptr = buffer + i + 1;
					break;
				case 2:
					req->uri_len = i - (req->uri_ptr - buffer);
					req->httpv_ptr = buffer + i + 1;
					break;
				default:
					break;
			}
		}
	}
	return 0;
}

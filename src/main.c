#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "response2.h"
#include "logging.h"
#include "request.h"

#define DEFAULT_PORT 8080
#define BUFF_SIZE 2048

int main(int argc, char *argv[])
{
    int sfd, cfd, addrlen, port=DEFAULT_PORT;
    ssize_t bytes_rec, bytes_sent;
    struct sockaddr_in address;
    char buffer[BUFF_SIZE] = {0};
	Request *req;
	Response *res;
	char *res_buff;
	int break_conn = 0;

	if (argc > 1)
		port = atoi(argv[1]);
	if (argc > 2)
		set_log_level(atoi(argv[2]));

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    address.sin_port = htons(port);

    if (bind(sfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        close(sfd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sfd, 3) < 0) {
        perror("listen");
        close(sfd);
        exit(EXIT_FAILURE);
    }
	LOG_INFO("Listening on port %d", port);

	for (;;) {
		// Accept a connection
		addrlen = sizeof(address);
		cfd = accept(sfd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
		if (cfd < 0) {
			perror("accept");
			close(sfd);
			exit(EXIT_FAILURE);
		}
		LOG_DEBUG("Connection accepted.");
		break_conn = 0;

		// MAIN LOOP
		// Receive data
		bytes_rec = 0;
		while ((bytes_rec = recv(cfd, buffer, BUFF_SIZE-1, 0)) > 0) {
			LOG_DEBUG("Connection loop with %d bytes received", bytes_rec);
			buffer[bytes_rec] = '\0';
			LOG_DEBUG("Bytes received:\n%s", buffer);
			
			// Allocates structures
			req = init_request();

			// Extract info from buffer and store pointer into request struct
			request2struct(req, buffer, BUFF_SIZE);
			LOG_INFO("%s %s %s", req->method, req->uri, req->httpv);
			LOG_DEBUG("Request method: %s", req->method);
			LOG_DEBUG("Request uri: %s", req->uri);
			LOG_DEBUG("Request httpv: %s", req->httpv);
			for (int i=0; i<req->headers_qty; i++) {
				LOG_DEBUG("Request header%d name: %s", i, req->headers[i].name);
				LOG_DEBUG("Request header%d value: %s", i, req->headers[i].value);
			}

			// Process response into a structure
			res = request2response(req);
			LOG_DEBUG("Response status line: %s %s %s", 
					res->httpv, res->statuscode, res->reasonp);
			for (int i=0; i<res->headers_count; i++) {
				LOG_DEBUG("Response header%d name: %s", i, res->headers[i].name);
				LOG_DEBUG("Response header%d value: %s", i, res->headers[i].value);
			}
			LOG_DEBUG("Response message body:\n%s", 
					res->messagebody);

			// Process response structure into a buffer to send
			res_buff = response2buffer(res);
			LOG_DEBUG("Response buffer:\n%s", res_buff);

			// Send response
			bytes_sent = send(cfd, res_buff, strlen(res_buff), 0);
			if (bytes_sent < 0)
				perror("send");
			LOG_DEBUG("Sent %d bytes", bytes_sent);

			// Check headers for connection state
			for (int i=0; i<res->headers_count; i++)
				if (strncmp(res->headers[i].name, "Connection", 10) == 0
					&& strncmp(res->headers[i].value, "close", 5)) {
					LOG_DEBUG("Found break condition");
					break_conn = 1;
					break;
				}

			// Free structures and buffers
			free_request2(req);
			free_response(res);
			free(res_buff);

			if (break_conn) {
				LOG_DEBUG("Breaking");
				break;
			}
		}
		
		if (bytes_rec == 0)
			LOG_DEBUG("Client disconnected");
		else if (bytes_rec < 0)
			perror("recv");

		close(cfd);

	}

    close(sfd);
    return 0;
}

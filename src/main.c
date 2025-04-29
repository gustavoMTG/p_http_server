#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "parser.h"
#include "response.h"
#include "logging.h"

#define DEFAULT_PORT 8080
#define BUFF_SIZE 2048
#define BUFF_REQLINE 30

int main(int argc, char *argv[])
{
    int sfd, cfd, addrlen, port=DEFAULT_PORT;
    ssize_t bytes_rec, bytes_sent;
    struct sockaddr_in address;
    char buffer[BUFF_SIZE] = {0};
	char *buff;
    struct Request *req;
    char *res;

	if (argc > 1)
		port = atoi(argv[1]);

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
		printf("Connection accepted.\n");

		// Receive data
		bytes_rec = 0;
		while ((bytes_rec = recv(cfd, buffer, BUFF_SIZE-1, 0)) > 0) {
			buffer[bytes_rec] = '\0';

			printf("Bytes received:\n%s", buffer);
			// Parsing 
			// Request line and headers
			req = request_parser(buff, buffer, BUFF_SIZE);

			// TODO: Elaborate response
			if (req == NULL) {
				; // Elaborate invalid response
			} else {
				// Elaborate valid response
				res = response_gen(req);
			}

			errno = 0;
			bytes_sent = send(cfd, res, strlen(res), 0);
			if (bytes_sent == -1) {
				perror("send");
			}
			printf("Sent %ld bytes\n", bytes_sent);

		}
		
		if (bytes_rec == 0)
			printf("Client disconnected\n");
		else if (bytes_rec < 0)
			perror("recv");

		close(cfd);

	}

    close(sfd);
    return 0;
}

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "parser.h"

#define PORT 8080
#define BUFF_SIZE 2048
#define BUFF_REQLINE 30

int main(int argc, char *argv[])
{
    int sfd, cfd, addrlen;
    ssize_t bytes_rec;
    struct sockaddr_in address;
    char buffer[BUFF_SIZE] = {0};
    struct Request req;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    address.sin_port = htons(PORT);

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

    // Accept a connection
    addrlen = sizeof(address);
    cfd = accept(sfd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (cfd < 0) {
        perror("accept");
        close(sfd);
        exit(EXIT_FAILURE);
    }

    // Receive data
    bytes_rec = 0;
    while ((bytes_rec = recv(cfd, buffer, BUFF_SIZE-1, 0)) > 0) {
        buffer[bytes_rec] = '\0';

        // Parsing 
        // Request line and headers
		request_parser(&req, buffer, BUFF_SIZE);

    }
    
    if (bytes_rec == 0)
        printf("Client disconnected\n");
    else if (bytes_rec < 0)
        perror("recv");

    close(sfd);
    close(cfd);
    return 0;
}

# p_http_server

C project for implementing an HTTP server based on the RFC2616 for HTTP 1.1.

## Building

Just clone the project and run the make command on the root folder.

```bash
$ git clone https://github.com/gustavoMTG/p_http_server.git
$ cd p_http_server
$ make
```

This will output a p_http_server executable file.

## Usage

```bash
$ ./p_http_server <port> <verbosity>
```

Where **verbosity** can be 0 for **DEBUG** level output or 1 for **INFO** level output.

The server will listen all network interfaces, if port is not provided will default to port 8080 for incomming TCP connections.

## State of development

The server currently supports **GET** and **HEAD** methods although these are still under development.

Static files serving, no dynamic content yet.


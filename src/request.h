#ifndef REQUEST_H
#define REQUEST_H

#define MAX_HEADERS 48

typedef struct {
	char *name;
	char *value;
} Header;

typedef struct {
	char *method;
	char *uri;
	char *httpv;
	Header headers[MAX_HEADERS];
	int headers_qty;
} Request;

Request *init_request(void);
void free_request2(Request *req);
void request2struct(Request *req, char *buffer, int buff_size);

#endif

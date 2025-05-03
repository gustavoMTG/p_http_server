#ifndef DATATYPES_H
#define DATATYPES_H

#define MAX_HEADERS 48
#define MAX_MBLENGTH 4096

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

typedef struct {
	char *httpv;
	char *statuscode;
	char *reasonp;

	Header *headers;
	int headers_count;

	char messagebody[MAX_MBLENGTH];
} Response;

#endif

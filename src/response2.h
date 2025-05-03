#ifndef RESPONSE_H
#define RESPONSE_H

#include "datatypes.h"

Response *request2response(Request *req);
char *response2buffer(Response *res);
void free_response(Response *res);

#endif

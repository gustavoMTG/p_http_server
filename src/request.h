#ifndef REQUEST_H
#define REQUEST_H

#include "datatypes.h"

Request *init_request(void);
void free_request2(Request *req);
void request2struct(Request *req, char *buffer, int buff_size);

#endif

CC = gcc
CFLAGS = -Wall
ifdef DEBUG
	CFLAGS += -g
endif

IN_PREFIX = src
OBJ_PREFIX = objs
BIN = p_http_server


$(BIN): main.o logging.o request.o response2.o
	$(CC) $(CFLAGS) $(OBJ_PREFIX)/main.o $(OBJ_PREFIX)/response2.o $(OBJ_PREFIX)/request.o $(OBJ_PREFIX)/logging.o -o p_http_server

main.o: $(IN_PREFIX)/main.c | $(OBJ_PREFIX)
	$(CC) $(CFLAGS) -c $(IN_PREFIX)/main.c -o $(OBJ_PREFIX)/main.o

response2.o: $(IN_PREFIX)/response2.c | $(OBJ_PREFIX)
	$(CC) $(CFLAGS) -c $(IN_PREFIX)/response2.c -o $(OBJ_PREFIX)/response2.o

request.o: $(IN_PREFIX)/request.c | $(OBJ_PREFIX)
	$(CC) $(CFLAGS) -c $(IN_PREFIX)/request.c -o $(OBJ_PREFIX)/request.o

logging.o: $(IN_PREFIX)/logging.c | $(OBJ_PREFIX)
	$(CC) $(CFLAGS) -c $(IN_PREFIX)/logging.c -o $(OBJ_PREFIX)/logging.o

# Create objs directory if needed
$(OBJ_PREFIX): 
	mkdir -p $(OBJ_PREFIX)

clean:
	rm -rf $(OBJ_PREFIX) $(BIN)

.PHONY: clean

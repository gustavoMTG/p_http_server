IN_PREFIX = src
OBJ_PREFIX = objs
BIN = p_http_server


$(BIN): main.o parser.o response.o logging.o
	gcc $(OBJ_PREFIX)/main.o $(OBJ_PREFIX)/parser.o $(OBJ_PREFIX)/response.o $(OBJ_PREFIX)/logging.o -o p_http_server

main.o: $(IN_PREFIX)/main.c | $(OBJ_PREFIX)
	gcc -c $(IN_PREFIX)/main.c -o $(OBJ_PREFIX)/main.o

parser.o: $(IN_PREFIX)/parser.c | $(OBJ_PREFIX)
	gcc -c $(IN_PREFIX)/parser.c -o $(OBJ_PREFIX)/parser.o

response.o: $(IN_PREFIX)/response.c | $(OBJ_PREFIX)
	gcc -c $(IN_PREFIX)/response.c -o $(OBJ_PREFIX)/response.o

logging.o: $(IN_PREFIX)/logging.c | $(OBJ_PREFIX)
	gcc -c $(IN_PREFIX)/logging.c -o $(OBJ_PREFIX)/logging.o

# Create objs directory if needed
$(OBJ_PREFIX): 
	mkdir -p $(OBJ_PREFIX)

clean:
	rm -rf $(OBJ_PREFIX) $(BIN)

.PHONY: clean

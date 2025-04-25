IN_PREFIX = src
OBJ_PREFIX = objs


p_http_server: $(IN_PREFIX)/main.c $(IN_PREFIX)/parser.c $(IN_PREFIX)/response.c
	gcc -o p_http_server $(IN_PREFIX)/main.c $(IN_PREFIX)/parser.c $(IN_PREFIX)/response.c

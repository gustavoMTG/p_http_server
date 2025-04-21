IN_PREFIX = src
OBJ_PREFIX = objs


p_http_server: $(IN_PREFIX)/main.c
	gcc -o p_http_server $(IN_PREFIX)/main.c

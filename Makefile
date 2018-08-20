cursor: cursor.c network.c server
	gcc cursor.c network.c -o $@ -lpthread

server: server.c
	gcc $^ -o $@

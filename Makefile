cursor: cursor.c network.c server
	gcc cursor.c network.c -o $@

server: server.c
	gcc $^ -o $@

all: cursor server

cursor: cursor.c network.c
	gcc $^ -o $@ -lpthread

server: server.c
	gcc $^ -o $@ -lpthread

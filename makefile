all: server client 

clean:
	rm -f server client 

server: server.c
	gcc -o server server.c linked_list.h -lpthread

client: client.c
	gcc -o client client.c -lpthread

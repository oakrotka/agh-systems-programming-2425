CC = gcc
CFLAGS = -Wall -O
.PHONY: all clean test

all: client server

test: all 

client: client.c 
	$(CC) $(CFLAGS) client.c -o client 

server: server.c 
	$(CC) $(CFLAGS) server.c -o server 

clean:
	-rm client server

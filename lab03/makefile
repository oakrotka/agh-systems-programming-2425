CC = gcc
CFLAGS = -Wall -O
.PHONY: all clean 
	
all: flipper

flipper: flipper.c 
	$(CC) $(CFLAGS) flipper.c -o flipper

clean:
	-rm flipper
	-rm out/*

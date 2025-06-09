CC = gcc
CFLAGS = -Wall -O
.PHONY: all clean 

all: printers

printers: printers.c 
	$(CC) $(CFLAGS) printers.c -o printers

clean:
	-rm printers

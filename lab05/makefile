CC = gcc
CFLAGS = -Wall -O
.PHONY: all clean test

all: zad1 sender catcher

test: all
	-./zad1 none
	./zad1 ignore
	./zad1 handler
	./zad1 mask
	@echo
	@echo "sender and catcher are to be tested manually."

zad1: zad1.c 
	$(CC) $(CFLAGS) zad1.c -o zad1 

sender: sender.c 
	$(CC) $(CFLAGS) sender.c -o sender

catcher: catcher.c 
	$(CC) $(CFLAGS) catcher.c -o catcher

clean:
	-rm zad1
	-rm sender catcher

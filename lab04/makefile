CC = gcc
CFLAGS = -Wall -O
.PHONY: all clean test

all: zad1 zad2

test: all 
	./zad1 5 
	@echo
	./zad2 .

zad1: zad1.c 
	$(CC) $(CFLAGS) zad1.c -o zad1

zad2: zad2.c 
	$(CC) $(CFLAGS) zad2.c -o zad2

clean:
	-rm zad1 zad2

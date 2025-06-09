CC = gcc
CFLAGS = -Wall -O
.PHONY: all clean test

all: integrator

test: all 
	./integrator 0.000000001 1
	./integrator 0.000000001 2
	./integrator 0.000000001 3
	./integrator 0.000000001 4
	./integrator 0.000000001 8

integrator: integrator.c 
	$(CC) $(CFLAGS) integrator.c -o integrator -lm -lpthread

clean:
	-rm integrator

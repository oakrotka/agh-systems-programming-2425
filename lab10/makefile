CC = gcc
CFLAGS = -Wall -O
.PHONY: all clean test

all: hospital

test: all 
	./hospital

hospital: hospital.c 
	$(CC) $(CFLAGS) hospital.c -o hospital -lpthread

clean:
	-rm hospital

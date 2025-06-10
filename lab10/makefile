CC = gcc
CFLAGS = -Wall
.PHONY: all clean test

all: hospital

test: all 
	./hospital 18 3

hospital: hospital.c 
	$(CC) $(CFLAGS) hospital.c -o hospital -lpthread

clean:
	-rm hospital

CC = gcc
CFLAGS = -Wall

all: countdown debug

debug: countdown.c 
	$(CC) $(CFLAGS) -g countdown.c -o dbg-prog

countdown: countdown.c 
	$(CC) $(CFLAGS) countdown.c -o prog

clean:
	rm prog dbg-prog

.PHONY: clean

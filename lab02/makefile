CC = gcc
CFLAGS = -Wall -O

.PHONY: clean all test

all: client-static client-shared client-dynamic

test: all 
	@echo "=> Wywołanie klienta statycznego"
	./client-static 3 100
	@echo "=> Wywołanie klienta współdzielonego"
	./client-shared 3 100
	@echo "=> Wywołanie klienta dynamicznego"
	./client-dynamic 3 100

client-static: collatz.a client.c
	$(CC) $(CFLAGS) -o client-static client.c collatz.a

client-shared: libcollatz.so client.c
	$(CC) $(CFLAGS) -o client-shared client.c -L ./ -lcollatz -Wl,-rpath,./

client-dynamic: client.c libcollatz.so
	$(CC) $(CFLAGS) -o client-dynamic client.c -D DYNAMIC -Wl,-rpath,./

collatz.a: collatz.c collatz.h
	$(CC) $(CFLAGS) -c collatz.c 
	ar rcs collatz.a collatz.o
	rm collatz.o

libcollatz.so: collatz.c collatz.h
	$(CC) $(CFLAGS) -fPIC -c collatz.c
	$(CC) $(CFLAGS) -shared -o libcollatz.so collatz.o
	rm collatz.o

clean:
	-rm libcollatz.so*
	-rm collatz.a
	-rm client-dynamic client-shared client-static

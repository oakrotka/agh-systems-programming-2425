CC = gcc
CFLAGS = -Wall -O
.PHONY: all clean test

all: zad1 zad2

test: all 
	./integrator 0.000000001 8
	@echo
	@echo "To finish testing, type \`echo [l] [r] [h] | ./io\` in another terminal"
	@echo
	./calc

zad1: integrator
zad2: io calc input output

integrator: integrator.c 
	$(CC) $(CFLAGS) integrator.c -o integrator -lm

io: io.c 
	$(CC) $(CFLAGS) io.c -o io

calc: calc.c 
	$(CC) $(CFLAGS) calc.c -o calc -lm

input:
	mkfifo input

output:
	mkfifo output

clean:
	-rm integrator
	-rm io 
	-rm calc
	-rm input 
	-rm output

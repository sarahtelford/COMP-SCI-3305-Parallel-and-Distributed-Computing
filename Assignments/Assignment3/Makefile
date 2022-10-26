CC=gcc
FLAGS=-O3 -Wall -Wextra -Wpedantic

all:main

main: parallel.c
	$(CC) $(FLAGS) parallel.c -o parallel -l OpenCL

clean:
	rm -f main

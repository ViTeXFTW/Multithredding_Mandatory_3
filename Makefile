# Makefile for main.c

all:
	gcc main.c -o main.exe

test:
	gcc main.c -o main.exe
	./main.exe

clean:
	rm *.o main.exe

# Makefile for main.c

final:
	gcc main.c -o main.exe

test:
	gcc main.c -o main.exe
	main.exe

clean:
	rm *.o main.exe

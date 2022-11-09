# Makefile for main.c

all:
	gcc main.c -o main.exe

test:
	gcc main.c -o main.exe
	./main.exe

clean:
	rm *.o main.exe

profile:
	gcc main.c -o main.exe -pg
	./main.exe
	gprof -b main.exe gmon.out > analysis.txt
	rm gmon.out
	cat analysis.txt
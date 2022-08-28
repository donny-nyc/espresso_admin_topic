all: main.c
	gcc -Wall main.c -l pthread -o main

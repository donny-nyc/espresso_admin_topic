all: main.c publish_message_list.c
	gcc -Wall -g main.c publish_message_list.c -l pthread -o main

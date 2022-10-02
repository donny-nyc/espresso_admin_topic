all: main.c publish_message_list.c
	gcc -Wall -g main.c topic.c publish_message_list.c -lpthread -o main

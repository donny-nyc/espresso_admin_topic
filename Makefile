all: main.c publish_message_list.c
	gcc -Wall -g main.c topic.c publish_message_list.c channel.c channel_subscription.c add_topic_subscription.c -lpthread -o main

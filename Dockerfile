FROM gcc:latest AS compiler
WORKDIR /build
ADD main.c publish_message_list.c messages.h client.h topic.h topic.c channel.c channel.h channel_subscription.c channel_subscription.h add_topic_subscription.h add_topic_subscription.c .
RUN gcc --static main.c topic.c channel.c channel_subscription.c add_topic_subscription.c publish_message_list.c -lpthread -o main
#COPY hello.c .
#RUN gcc --static hello.c -o main

#FROM ubuntu:latest
FROM scratch
COPY --from=compiler /build/main /

EXPOSE 6060/udp
ENTRYPOINT ["./main"]

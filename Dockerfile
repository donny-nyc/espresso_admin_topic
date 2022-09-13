FROM gcc:latest as compiler
WORKDIR /build/
ADD main.c publish_message_list.c messages.h client.h .
RUN gcc -Wall --static main.c publish_message_list.c -lpthread -o main && pwd && ls

FROM scratch
WORKDIR /build/
COPY --from=compiler /build/main ./main
CMD ["./main"]

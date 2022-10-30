#include <strings.h> // bzero

#include <stdio.h> // perror

#include <stdlib.h> // exit

// read
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
static void *connection(void *ard) {
  struct addrinfo hints, *res0;

  bzero(&hints, sizeof(struct addrinfo));

  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  int err = getaddrinfo("127.0.0.1", "4545", &hints, &res0);

  int s = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol);

  if(bind(s, res0->ai_addr, res0->ai_addrlen) < 0) {
    perror("unable to bind");
    exit(1);
  }

  char buffer[1000];

  printf("listening: 127.0.0.1:4545\n");
  
  while(1) {
    while(read(s, &buffer, 1000) > 0) {
      printf("recv: %s\n", buffer);

      bzero(buffer, 1000);
    }
  }
}

#include <pthread.h>
int main(void) {
  pthread_t conn_handler;

  int x = pthread_create(&conn_handler, NULL, connection, NULL);

  pthread_join(conn_handler, NULL);

  return 0;
}

/*
 * struct addrinfo {
 *  int ai_flags; // AF_PASSIVE, AI_CANNONNAME, etc.
 *  int ai_family;// AF_INET, AF_INET6, AF_UNSPEC
 *  int ai_socktype; // SOCK_STREAM, SOCK_DGRAM
 *  int ai_protocol; // use 0 for "any"
 *  int ai_addrlen; // size of ai_addr in bytes
 *  struct sockaddr *ai_addr; // struct sockaddr_in or _in6
 *  char  ai_cannonname; // full canonical hostname
 *  
 *  struct addrinfo *ai_next; // linked list, next node
 */

#include <sys/fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "messages.h"

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t pml_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t handle_cond = PTHREAD_COND_INITIALIZER;

// first consumer should read new messages off of the topic,
// and queue them up for their configured handler to process
//
// Handler should block until notified that a message is available
static struct publish_message_list pml;

static void *handleMessage(void *arg) {
  while(1) {
    int s = pthread_mutex_lock(&pml_mtx);

    if(s) {
      perror(strerror(errno));
    }

    s = pthread_cond_wait(&handle_cond, &pml_mtx);

    if(s) {
      perror(strerror(errno));
    }

    struct publish_message *pm;

    pm = pop_message(&pml);

    if(pm) {
      printf("handling message: %llu, %d, %llu\n", pm->message_id,
          pm->topic, pm->checksum);
    } else {
      printf("no message\n");
    }

    free(pm);

    pthread_mutex_unlock(&pml_mtx);
  }
}

static void *threadFunc(void *arg) {
  printf("started consumer\n");
  uint64_t messages = 0;
  char *path = (char *)arg;
  FILE *f;

  while(1) {
    int s = pthread_mutex_lock(&mtx);

    s = pthread_cond_wait(&cond, &mtx);

    f = fopen(path, "r");
    if(messages > 0 && fseek(f, sizeof(struct publish_message) * messages, 0) != 0) {
      fclose(f);
    }

    struct publish_message pm; // = (struct publish_message *)malloc(sizeof(struct publish_message));
    while(fread(&pm, sizeof(struct publish_message), 1, f) > 0) {

      messages++;

      pthread_mutex_lock(&pml_mtx);
      struct publish_message *pm2 = (struct publish_message *)malloc(sizeof(struct publish_message));

      bzero(pm2, sizeof(struct publish_message));
      pm2->message_id = pm.message_id;
      pm2->checksum = pm.checksum;
      pm2->topic = pm.topic;

      int len = push_message(&pml, pm2);

      pthread_mutex_unlock(&pml_mtx);

      pthread_cond_signal(&handle_cond);
    }

    fclose(f);

    s = pthread_mutex_unlock(&mtx);
  }
}

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
static void *connection(void *arg) {
  struct addrinfo hints, *res0;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  int err = getaddrinfo("127.0.0.1", "6060", &hints, &res0);

  int s = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol);

  if(bind(s, res0->ai_addr, res0->ai_addrlen) < 0) {
    perror("unable to bind");
    exit(1);
  }

  char buffer[1000];
  while(read(s, &buffer, 1000) > 0) {
    printf("buffer: %s\n", buffer);
    bzero(buffer, 1000);
  } 

  freeaddrinfo(res0);

  return 0;
}

int main(void) {
  const char *file = "tmp.txt";

  pml.length = 0;

  pthread_t *available_workers[3];
  pthread_t *busy_workers[3];

  for(int i = 0; i < 3; i++) {
    available_workers[i] = (pthread_t *)malloc(sizeof(pthread_t));
    int s = pthread_create(available_workers[i], NULL, handleMessage, NULL);
    if(s) {
      perror(strerror(errno));
      exit(1);
    }

    printf("created worker %d\n", i);
  }

  pthread_t net_c;
 
 int x  = pthread_create(&net_c, NULL, connection, NULL);

  pthread_t t1;
  void *res;
  int s = pthread_create(&t1, NULL, threadFunc, (void *)file);

  sleep(1);
  // pthread_cond_signal(&cond);
  
  uint32_t topic = 0;
  uint64_t message_id = 0;
  uint64_t checksum = 0;

  while(1) {
    sleep(1);
    struct publish_message pm;

    pm.topic = topic++;
    pm.message_id = message_id++;
    pm.checksum = checksum++;


    int s = pthread_mutex_lock(&mtx);

    FILE *f = fopen(file, "a");
    int res = fwrite(&pm, sizeof(struct publish_message), 1, f);
    int success = fclose(f);

    printf("wrote message: %llu\n", pm.message_id);

    s = pthread_mutex_unlock(&mtx);

    s = pthread_cond_signal(&cond);
  }

  pthread_join(t1, &res);

  return 0;
}

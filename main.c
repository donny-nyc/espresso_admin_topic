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
#include <string.h>
#include <unistd.h>

#include "messages.h"

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void *threadFunc(void *arg) {
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

    struct publish_message pm;
    while(fread(&pm, sizeof(struct publish_message), 1, f) > 0) {

      printf("message: %llu, %d, %llu, %llu\n", messages, pm.topic, pm.message_id, pm.checksum);
      messages++;
      sleep(1);
    }

    fclose(f);

    s = pthread_mutex_unlock(&mtx);
  }
}

int main(void) {
  const char *file = "tmp.txt";
  /*
  int fd = open(file, O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); 
  if(fd == -1) {
    perror(strerror(errno));
    return 1;
  }
  */

  pthread_t t1;
  void *res;
  int s = pthread_create(&t1, NULL, threadFunc, (void *)file);
  
  while(1) {
    struct publish_message pm;

    uint32_t topic;
    uint64_t message_id;
    uint64_t checksum;

    printf("topic: ");
    scanf("%d", &topic);

    printf("message_id: ");
    scanf("%llu", &message_id);

    printf("checksum: ");
    scanf("%llu", &checksum);
    

    pm.topic = topic;
    pm.message_id = message_id;
    pm.checksum = checksum;


    int s = pthread_mutex_lock(&mtx);

    FILE *f = fopen(file, "a");
    int res = fwrite(&pm, sizeof(struct publish_message), 1, f);
    int success = fclose(f);

    s = pthread_mutex_unlock(&mtx);

    s = pthread_cond_signal(&cond);
  }

  pthread_join(t1, &res);

  /*
  pm.topic = 1;
  pm.checksum = 2;
  pm.message_id = 3;

  // write(fd, &pm, sizeof(struct publish_message));
  int res = fwrite(&pm, sizeof(struct publish_message), 1, f);

  // int success = close(fd);
  int success = fclose(f);
  if(success == -1) {
    perror(strerror(errno));    
    return 1;
  }

//  int fd2 = open(file, O_RDONLY); 
  FILE *f2 = fopen(file, "r");

  struct publish_message pm2;

//  int res = read(fd2, &pm2, sizeof(struct publish_message));
  int r = fread(&pm2, sizeof(struct publish_message), 1, f2);

  printf("topic: %d, checksum: %llu, message_id: %llu\n", pm2.topic, pm2.checksum, pm2.message_id);
  */

  return 0;
}

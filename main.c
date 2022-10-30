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

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#include "topic.h"
#include "channel.h"

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t pml_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t handle_cond = PTHREAD_COND_INITIALIZER;

static struct topic_map_t *topic_map;

// first consumer should read new messages off of the topic,
// and queue them up for their configured handler to process
//
// Handler should block until notified that a message is available
static struct publish_message_list pml;



static void *handleMessage(void *arg) {
  struct addrinfo hints, *res, *res0;

  bzero(&hints, sizeof(struct addrinfo));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  int err = getaddrinfo("172.24.0.3", "49494", &hints, &res0);
  if(err) {
    perror(gai_strerror(err));
  }

  int sx = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol);

  for(res = res0; res; res = res->ai_next) {
    if(bind(sx, res0->ai_addr, res0->ai_addrlen) < 0) {
      perror("Trying the next bind option");
      continue;
    } else {
      break;
    }
  }

  printf("handle message bind success\n");

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

    pthread_mutex_unlock(&pml_mtx);

    if(pm) {
      printf("handling message: %llu, %d, %llu\n", pm->message_id,
          pm->topic, pm->checksum);
      /*
      struct addrinfo dest;
      bzero(&dest, sizeof(struct sockaddr));

      dest.ai_family = PF_UNSPEC;
      dest.ai_socktype = SOCK_DGRAM;
      hints.ai_flags = AI_PASSIVE;

      struct sockaddr sad;

      err = getaddrinfo("0.0.0.0", "49494", &dest, &res0);
      */

      const char* msg = "{\"message\":\"hello world\"}";
      int r = sendto(sx, msg, strlen(msg), 0, res0->ai_addr, res0->ai_addrlen);
      if(r < 0) {
        perror(strerror(errno));
      } else {
        printf("sent %d bytes\n", r);
      }

    } else {
      printf("no message\n");
    }

    free(pm);

  }

  freeaddrinfo(res0);
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

#include "client.h"

char *port;
char *host;

static void *connection(void *arg) {
  struct addrinfo hints, *res0;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  int err = getaddrinfo(host, port, &hints, &res0);

  if(err) {
    printf("%s\n", gai_strerror(err));
    exit(1);
  }

  printf("Listening on port %s at %s\n", port, host);

  int s = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol);

  if(bind(s, res0->ai_addr, res0->ai_addrlen) < 0) {
    perror("unable to bind");
    exit(1);
  }

  char buffer[1000];
  // struct publish_message pm;
  struct message_t req;
  while(read(s, &buffer, 1000) > 0) {
    printf("buffer: %s\n", buffer);

    //sscanf(buffer, "%u;%llu;%llu;\n", &pm.topic, &pm.checksum, &pm.message_id);
    int scanned = sscanf(buffer, "%u;%500c\n", &req.type, req.body);
    bzero(buffer, 1000);

    int s = pthread_mutex_lock(&mtx);
    if(s) {
      perror(strerror(errno));
    }

    FILE *f = fopen("tmp.txt", "a");
    int res = fwrite(&req, sizeof(struct message_t), 1, f);
    int success = fclose(f);

    if(scanned < 2) {
      printf("invalid msg. Scanned %d\n", scanned);
    } else {
      printf("received: %s\n", req.body);
    }
    // printf("client wrote message: %llu\n", pm.message_id);

    s = pthread_mutex_unlock(&mtx);

    s = pthread_cond_signal(&cond);

    if (req.type == COMMAND) {
      struct command_message cm;

      int count = 0;
      sscanf(req.body, "%u;%u;%n", &cm.type, &cm.body_len, &count);

      printf("command %d bytes\n", count);

      cm.body = (char *)malloc(sizeof(char) * cm.body_len);

      sscanf(req.body + count, "%s", cm.body);

      // printf("New command: %u, len: %u\n", cm.type, cm.body_len);
      switch(cm.type) {
        case ADD_TOPIC_SUBSCRIPTION:
          printf("Add Destination. %s\n", cm.body);
          struct add_topic_subscription_t ats;

          break;
        case DELETE_TOPIC_SUBSCRIPTION:
          printf("Delete Destination\n");
          break;
        case REPLACE_TOPIC_SUBSCRIPTION:
          printf("RELACE Destination\n");
          break;
        case ADD_TOPIC:
          printf("Add Topic");
          struct add_topic_t atr;

          printf("Request Body: %s\n", cm.body);

          int count = 0;
          sscanf(cm.body, "%u;%n", &atr.name_len, &count);

          atr.name = (char *)malloc(atr.name_len * sizeof(char));

          sscanf(cm.body + count, "%s", atr.name);

          printf("name(len=%u): %s\n", atr.name_len, atr.name);

          struct topic_t *topic = (struct topic_t *)malloc(sizeof(struct topic_t));

          strcpy(topic->name, atr.name);

          strcpy(topic->id, generate_id());

          set(topic_map, topic);

          break;
        case DELETE_TOPIC:
          printf("Delete Topic");
          break;
      }
      free(cm.body);
    } else if (req.type == EVENT) {

    }

  } 

  freeaddrinfo(res0);

  return 0;
}

char *get_env_or_default(char *name, char *def) {
  char *value = getenv(name);

  return value ? value : def;
}


int main(void) {
  srand(time(NULL));
  port = get_env_or_default("LISTEN_PORT_NO", "6060");

  host = get_env_or_default("LISTEN_HOST", "0.0.0.0");

  setvbuf(stdout, NULL, _IONBF, 0);
  printf("starting\n");

  const char *channels_file = "channels.bin";

  load_channels(channels_file);

  const char *file = "tmp.txt";

  topic_map = init_map(16);

  pml.length = 0;

  pthread_t *available_workers[3];
  pthread_t *busy_workers[3];

  for(int i = 0; i < 3; i++) {
    available_workers[i] = (pthread_t *)malloc(sizeof(pthread_t));
    int s = pthread_create(available_workers[i], NULL, handleMessage, NULL);
    if(s) {
      printf("something broke\n");
      perror(strerror(errno));
      exit(1);
    }

    printf("created worker %d\n", i);
  }

  pthread_t net_c;
 
 int x  = pthread_create(&net_c, NULL, connection, NULL);

 if(x) {
  printf("something broke\n");
  perror(strerror(errno));
  exit(1);
 }

  pthread_t t1;
  void *res;
  int s = pthread_create(&t1, NULL, threadFunc, (void *)file);

  if(s) {
    printf("something broke\n");
    perror(strerror(errno));
    exit(1);
  }

  sleep(1);
  
  uint32_t topic = 0;
  uint64_t message_id = 0;
  uint64_t checksum = 0;

  pthread_join(t1, &res);

  return 0;
}

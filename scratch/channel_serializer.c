#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct channel_subscription_t {
  uint32_t name_len;
  char *name;
  uint32_t host_len;
  char *host;
  uint32_t port;
};

typedef struct channel_subscription_t channel_subscription_t;

struct subscription_list_entry_t {
  channel_subscription_t *subscription;
  struct subscription_list_entry_t *next, *prev;
};

typedef struct subscription_list_entry_t subscription_list_entry_t;

struct subscription_list_t {
  uint32_t count;
  subscription_list_entry_t *first, *last;
};

typedef struct subscription_list_t subscription_list_t;

void push_subscription(subscription_list_t *subscriptions, channel_subscription_t *subscription) {
  if(subscriptions->count) {
    subscriptions->last->next = calloc(1, sizeof(subscription_list_entry_t));
    subscriptions->last->next->prev = subscriptions->last->next;
    subscriptions->last = subscriptions->last->next;

    subscriptions->last->subscription = subscription;

    subscriptions->count++;
  } else {
    subscriptions->first = subscriptions->last = calloc(1, sizeof(subscription_list_entry_t));

    subscriptions->last->subscription = subscription;

    subscriptions->count++;
  }
}

struct channel_serializer_t {
  uint32_t name_len;
  char *name;
  uint32_t subscription_count;
  channel_subscription_t *subscriptions;
};

typedef struct channel_serializer_t channel_serializer_t;

struct channel_t {
  char *name;
  subscription_list_t *subscriptions; // this should probably be a list
};

typedef struct channel_t channel_t;

struct channel_serializer {
  channel_t *(*from_file)(const char *path); 
  void (*to_file)(const channel_t *channel, const char *path);
};

#define READ_ONLY "r"
#define WRITE_ONLY "w"

channel_t *build_channel(const char *name, subscription_list_t *subscriptions) {
  channel_t *channel = (channel_t *)calloc(1, sizeof(struct channel_t));

  if(name) {
    channel->name = strdup(name);

    if(!channel->name) {
      perror(strerror(errno));
    }
  }

  if(subscriptions) {
    channel->subscriptions = subscriptions;
  } else {
    channel->subscriptions = (subscription_list_t *)calloc(1, sizeof(struct subscription_list_t));
  }

  return channel;
}

channel_t *from_file(const char *path) {
  channel_serializer_t *channel_serializer = (channel_serializer_t *)malloc(sizeof(struct channel_serializer_t));
  FILE *f = fopen(path, READ_ONLY);
  int r = fread(&channel_serializer->name_len, sizeof(uint32_t), 1, f);

  if(!r) {
    printf("something broke - from file\n");
  }

  printf("name len: %u\n", channel_serializer->name_len);

  channel_t *channel = calloc(1, sizeof(channel_t));

  if(channel_serializer->name_len) {

    channel_serializer->name = (char *)calloc(channel_serializer->name_len + 1, sizeof(char));
    r = fread(&channel_serializer->name, sizeof(char), channel_serializer->name_len, f); 

    printf("name: %s\n", channel_serializer->name);

    channel->name = strdup(channel_serializer->name);
  }

  r = fread(&channel_serializer->subscription_count, sizeof(uint32_t), 1, f);

  if(channel_serializer->subscription_count) {
    channel_serializer->subscriptions = calloc(channel_serializer->subscription_count, sizeof(struct channel_subscription_t));

    if(!channel_serializer->subscriptions) {
      perror(strerror(errno));
      exit(1);
    }

    channel->subscriptions = calloc(1, sizeof(subscription_list_t));

    for(int i = 0 ; i < channel_serializer->subscription_count; i++) {
      channel_subscription_t *sub = &channel_serializer->subscriptions[i];
      r = fread(&sub->name_len, sizeof(uint32_t), 1, f);

      sub->name = calloc(sub->name_len, sizeof(char));
      r = fread(&sub->name, sizeof(char), sub->name_len, f);

      printf("sub name (%u): %s\n", sub->name_len, sub->name);

      r = fread(&sub->host_len, sizeof(uint32_t), 1, f);
      sub->host = calloc(sub->host_len, sizeof(char));

      r = fread(&sub->host, sizeof(char), sub->host_len, f);

      printf("host (%u): %s\n", sub->host_len, sub->host);

      r = fread(&sub->port, sizeof(uint32_t), 1, f);

      printf("port: %u\n", sub->port);

      push_subscription(channel->subscriptions, sub);
    }
  }

  fclose(f);

  return channel;
}

void to_file(const channel_t *channel, const char *path) {
  FILE *f = fopen(path, WRITE_ONLY);

  size_t name_len = strlen(channel->name);

  size_t res = fwrite(&name_len, sizeof(uint32_t), 1, f);

  if(!res) {
    printf("something broke - to_file\n");
  }

  res = fwrite(&channel->name, sizeof(char), name_len, f);

  if(channel->subscriptions->count) {
    res = fwrite(&channel->subscriptions->count, sizeof(uint32_t), 1, f);

    subscription_list_entry_t *entry = channel->subscriptions->first;
    for(int i = 0; i < channel->subscriptions->count; i++) {
      res = fwrite(&entry->subscription->name_len, sizeof(uint32_t), 1, f);
      res = fwrite(&entry->subscription->name, entry->subscription->name_len, 1, f);

      printf("write name(%u): %s\n", entry->subscription->name_len, entry->subscription->name);

      res = fwrite(&entry->subscription->host_len, sizeof(uint32_t), 1, f);
      res = fwrite(&entry->subscription->host, entry->subscription->host_len, 1, f);

      res = fwrite(&entry->subscription->port, sizeof(uint32_t), 1, f);
      entry = entry->next;
    }
  }

  fclose(f);
}

channel_subscription_t *build_subscription(const char *name, const char *host, const uint32_t port) {
  channel_subscription_t *sub = (channel_subscription_t *)malloc(sizeof(channel_subscription_t));

  sub->name_len = strlen(name);
  sub->name = strdup(name);

  sub->host_len = strlen(host);
  sub->host = strdup(host);

  sub->port = port;

  return sub;
}

char *as_string(const channel_t *c) {
  char *output = calloc(1000, sizeof(char));

  snprintf(output, 1000, "%s;", c->name);

  size_t offset = strlen(output);

  subscription_list_entry_t *entry = c->subscriptions->first;
  for(size_t i = 0; i < c->subscriptions->count; i++) {
    snprintf(output + offset, 1000 - offset, "%s;", entry->subscription->name);
    offset = strlen(output);

    entry = entry->next;
  }

  return output;
}

void add_subscription(channel_t *channel, const char *name, const char *host, const uint32_t port) {
  channel_subscription_t *sub = build_subscription(name, host, port);

  push_subscription(channel->subscriptions, sub);
}

int main() {
  char *tmp_path = "./channel.dat";

  char *name = "my name";

  channel_t c;
  c.subscriptions = calloc(1, sizeof(subscription_list_t));
  c.name = name;

  add_subscription(&c, "a name", "https://example.com", 8080);
  add_subscription(&c, "b name", "https://anotherexample.com", 1234);

  to_file(&c, tmp_path);

  channel_t *res;

  res = from_file(tmp_path);

  printf("%s\n", as_string(res));

  return 0;
}

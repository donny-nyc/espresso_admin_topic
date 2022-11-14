#include "channel.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

uint32_t hash(const uint32_t capacity, uint32_t id) {
  uint32_t index = 0;

  while(id) {

    index = (id % 10) + 32 * index;
    id /= 10;
  }

  return index % capacity;
}

struct channel_map_t *init_map(uint32_t capacity) {
  struct channel_map_t *map = (struct channel_map_t *)malloc(sizeof(struct channel_map_t));

  map->entries = calloc(capacity, sizeof(struct channel_map_t));

  map->capacity = capacity;
  map->length = 0;

  return map;
}

void set(struct channel_map_t *map, const struct channel_t *channel) {
  uint32_t index = hash(map->capacity, channel->id);

  struct channel_map_entry_t *entry;

  if(!map->entries[index]) {
    entry = map->entries[index] = (struct channel_map_entry_t *)malloc(sizeof(struct channel_map_entry_t));

    entry->key = channel->id;
    entry->val = channel;
  } else {
    entry = map->entries[index];
    
    while(entry->next) {
      entry = entry->next;
    }

    entry->next = (struct channel_map_entry_t *)malloc(sizeof(struct channel_map_entry_t));
    entry = entry->next;

    entry->key = channel->id;
    entry->val = channel;
  }

  map->length++;
}

const struct channel_t *get(const struct channel_map_t *map, const uint32_t id) {
  uint32_t index = hash(map->capacity, id);

  const struct channel_map_entry_t *entry = map->entries[index];
  while(entry) {
    if(entry->key == id) {
      return entry->val;
    }

    if(entry->next) {
      entry = entry->next;
    }
  }

  return NULL;
}

channel *create_channel() {
  channel *new_channel = (channel *)malloc(sizeof(channel));

  return new_channel;
}

void to_file(FILE *f, channel *c) {
  size_t name_len = strlen(c->name);

  size_t res = fwrite(&name_len, sizeof(uint32_t), 1, f);

  if(!res) {
    dprintf(STDERR_FILENO, "Failed to write name_len\n");
    return;
  }

  res = fwrite(c->name, sizeof(char), name_len, f);

  if(!res) {
    dprintf(STDERR_FILENO, "Failed to write channel name\n");
    return;
  }

  if(c->subscriptions.count) {
    res = fwrite(&c->subscriptions.count, sizeof(uint32_t), 1, f);

    subscription_list_entry *entry = c->subscriptions.first;

    while(entry) {
      res= fwrite(&entry->subscription->id, sizeof(uint32_t), 1, f);
      if(!res) {
        dprintf(STDERR_FILENO, "Failed to write sub id\n");
        return;
      }

      size_t host_len = strlen(entry->subscription->host.url);
      res = fwrite(&host_len, sizeof(size_t), 1, f);

      if(!res) {
        dprintf(STDERR_FILENO, "Failed to write host name len\n");
      }

      res = fwrite(&entry->subscription->host.url, sizeof(char), host_len, f);
      if(!res) {
        dprintf(STDERR_FILENO, "Failed to write host url\n");
      }

      res = fwrite(&entry->subscription->host.port, sizeof(uint32_t), 1, f);
      if(!res) {
        dprintf(STDERR_FILENO, "Failed to write host port\n");
      }

      entry = entry->next;
    } 
  }
}

channel *from_file(FILE *f) {
  channel *c = calloc(1, sizeof(struct channel_t)); 

  if(!c) {
    dprintf(STDERR_FILENO, "Could not alloc space for channel\n");
    return c;
  }

  size_t name_len = 0;

  size_t res = fread(&name_len, sizeof(uint32_t), 1, f);

  if(!res) {
    dprintf(STDERR_FILENO, "Failed to read name len from file\n");
    free(c);

    return NULL;
  }

  if(name_len) {
    c->name = calloc(name_len, sizeof(char));
    res = fread(c->name, sizeof(char), name_len, f);
  } 

  size_t subscriptions_count = 0;
  res = fread(&subscriptions_count, sizeof(uint32_t), 1, f);

  if(!res) {
    dprintf(STDERR_FILENO, "Failed to read subscription count from file\n");
  }

  subscription *sub; 
  for(size_t i = 0; i < subscriptions_count; i++) {
    sub = calloc(1, sizeof(struct subscription_t)); 

    res = fread(&sub->id, sizeof(uint32_t), 1, f);

    if(!res) {
      dprintf(STDERR_FILENO, "Failed to read sub ID\n");
      free(sub);
      free(c);

      return NULL;
    }

    size_t host_len = 0;
    res = fread(&host_len, sizeof(size_t), 1, f);

    if(!res) {
      dprintf(STDERR_FILENO, "Failed to read host url len\n");
      free(c);
      free(sub);
      return NULL;
    }

    sub->host.url = calloc(host_len, sizeof(char));
    res = fread(&sub->host.url, sizeof(char), host_len, f);

    if(!res) {
      dprintf(STDERR_FILENO, "Failed to read host url\n");

      free(sub->host.url);
      free(sub);
      free(c);

      return NULL;
    }


    res = fread(&sub->host.port, sizeof(uint32_t), 1, f);

    if(!res) {
      dprintf(STDERR_FILENO, "Failed to read host port\n");

      free(sub->host.url);
      free(sub);
      free(c);

      return NULL;
    }

    if(!push_subscription(&c->subscriptions, sub)) {
      dprintf(STDERR_FILENO, "Failed to add subscription\n");

      free(sub->host.url);
      free(sub);
      free(c);

      return NULL;
    }
  }

  return c;
}

int add_subscription(channel *c, char *url, uint32_t port) {
  subscription *s = calloc(1, sizeof(struct subscription_t));

  s->host.url = strdup(url);
  s->host.port = port;

  push_subscription(&c->subscriptions, s);

  return 0;
}

void delete_channel(uint32_t id) {}

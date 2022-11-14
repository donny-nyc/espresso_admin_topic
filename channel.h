#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "channel_subscription.h"

struct channel_t {
  char *name;
  uint32_t id;
  struct subscription_list_t subscriptions;
};

typedef struct channel_t channel;

struct channel_map_entry_t {
  uint32_t key;
  const struct channel_t *val;
  struct channel_map_entry_t *next;
};

typedef struct channel_map_entry_t channel_map_entry;

struct channel_map_t {
  struct channel_map_entry_t **entries;
  uint32_t capacity;
  uint32_t length;
};

typedef struct channel_map_t channel_map;

char *generate_id();

struct channel_map_t *init_map(const uint32_t capacity);

uint32_t hash(const uint32_t capacity, uint32_t id);

void set(struct channel_map_t *, const struct channel_t *);

const struct channel_t *get(const struct channel_map_t *, const uint32_t id);

// Channels

channel *create_channel();
void delete_channel(uint32_t id);

int add_subscription(channel *, char *url, uint32_t port);

void to_file(FILE *f, channel *c);
channel *from_file(FILE *f);

// void load_channels(char *);

#include <stddef.h>

#include "channel.h"

struct channel_map_key_t {
  size_t key_len;
  char *key;
};

typedef struct channel_map_key_t channel_map_key;

struct channel_map_entry_t {
  channel_map_key *key;
  struct channel_t *channel;
  struct channel_map_entry_t *prev, *next;
};

typedef struct channel_map_entry_t channel_map_entry;

struct channel_map_t {
  size_t capacity;
  size_t size;

  channel_map_entry **channels;
};

typedef struct channel_map_t channel_map;

channel_map_key *insert(channel_map *, channel *);
int remove_key(channel_map *, channel_map_key *);
channel *get(channel_map *, channel_map_key *);
int hash(channel_map *, channel_map_key *);
int compare_key(channel_map_key *first, channel_map_key *second);

struct channel_map_t *create_map(size_t capacity);

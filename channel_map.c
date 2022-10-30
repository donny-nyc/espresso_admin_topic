#include "channel_map.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int hash(channel_map *map, channel_map_key *key) {
  int h = 0;
  int g = 31;

  for(int i = 0; i < key->key_len; i++) {
    h += g * key->key[i];
  }

  h %= map->capacity;

  return h; 
}

channel_map_key *create_key(channel *c) {
  channel_map_key *ck = (channel_map_key *)malloc(sizeof(channel_map_key));

  ck->key_len = c->name_len;
  ck->key = (char *)malloc(c->name_len);

  strncpy(ck->key, c->name, c->name_len);

  return ck;
}

channel_map_key *insert(channel_map *map, channel *c) {
  channel_map_key *key = create_key(c);
  int idx = hash(map, key);

  channel_map_entry *pos = map->channels[idx];

  if (!pos) {
    pos = (channel_map_entry *)malloc(sizeof(channel_map_entry));
    map->channels[idx] = pos;
  } else {
    while(pos->next) {
      pos = pos->next; 
    }
    pos->next = (channel_map_entry *)malloc(sizeof(channel_map_entry));
    pos->next->prev = pos;
    pos = pos->next;
  }

  pos->key = key;
  pos->channel = c;
  pos->next = 0;

  map->size++;

  return key;
}

int remove_key(channel_map *map, channel_map_key *key) {
  int idx = hash(map, key);

  channel_map_entry *pos = map->channels[idx];

  if(!pos) {
    return -1;
  }

  if(compare_key(pos->key, key) != 0) {
    if(pos->next) {
      pos = pos->next;
    } else {
      return -1;
    }
  }

  if(compare_key(pos->key, key) == 0) {
    pos->prev->next = pos->next;
    pos->next->prev = pos->prev;   

    free(pos->key);
    free(pos->channel->name);
    free(pos->channel);

    free(pos);
  } else {
    return -1;
  }

  return 0;
}

channel *get(channel_map *map, channel_map_key *key) {
  channel *result;

  int idx = hash(map, key);

  channel_map_entry *pos = map->channels[idx];

  if(!pos) {
    return 0;
  }

  while(compare_key(pos->key, key) != 0) {
    if(pos->next) {
      pos = pos->next;
    } else {
     return 0;
    } 
  }

  result = pos->channel;

  return result;
}

int compare_key(channel_map_key *first, channel_map_key *second) {
  if(first->key_len < second->key_len) {
    return -1;
  }

  if(first->key_len > second->key_len) {
    return 1;
  }

  return strncmp(first->key, second->key, first->key_len);
}

channel_map *create_map(size_t capacity) {
  channel_map *map = (channel_map *)malloc(sizeof(channel_map));

  map->size = 0;
  map->capacity = capacity;

  map->channels = calloc(capacity, sizeof(channel_map_entry));
  bzero(map->channels, capacity * sizeof(channel_map_entry));

  return map;
}

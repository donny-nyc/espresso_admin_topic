#include "topic.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

uint32_t hash(const uint32_t capacity, const char *key) {
  uint32_t index;

  for(index = 0; *key != '\0'; key++) {
    index = *key + 32 * index;
  }

  return index % capacity;
}

struct topic_map_t *init_map(uint32_t cap) {
  struct topic_map_t *map = (struct topic_map_t *)malloc(sizeof(struct topic_map_t));

  map->entries = calloc(cap, sizeof(struct topic_map_entry_t));

  map->capacity = cap;
  map->length = 0;

  return map;
}

void set(struct topic_map_t *map, const struct topic_t *topic) {
  uint32_t index = hash(map->capacity, topic->id);

  struct topic_map_entry_t *entry;

  if(!map->entries[index]) {
    entry = map->entries[index] = (struct topic_map_entry_t *)malloc(sizeof(struct topic_map_entry_t));

    entry->key = topic->id;
    entry->val = topic;
  } else {
    entry = map->entries[index];
    while(entry->next) {
      entry = entry->next;
    }

    entry->next = (struct topic_map_entry_t *)malloc(sizeof(struct topic_map_entry_t));
    entry = entry->next;

    entry->key = topic->id;
    entry->val = topic;
  }

  map->length++;
}

const struct topic_t *get(const struct topic_map_t *map, const char *key) {
  uint32_t index = hash(map->capacity, key);

  struct topic_map_entry_t *entry;

  entry = map->entries[index];

  if(strcmp(key, entry->key) == 0) {
    return entry->val;
  } else {
    while(entry->next) {
      entry = entry->next;

      if(strcmp(key, entry->key) == 0) {
        return entry->val;
      }
    } 
  }

  return 0;
}


char *permitted = "abcdefghijklmnopqrstuvwxyz0123456789";

char *generate_id() {
  char *result = calloc(10, sizeof(char));

  result[9] = '\0';

  int nbAllowed = strlen(permitted) - 1;

  for(int i = 0; i < 9; ++i) {
    int c = rand() % nbAllowed;
    result[i] = permitted[c];
  }

  return result;
}

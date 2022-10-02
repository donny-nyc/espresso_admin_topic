#include <stdint.h>

struct topic_t {
  char id[10];
  char name[1000];
};

struct topic_map_entry_t {
  const char *key;
  const struct topic_t *val;
  struct topic_map_entry_t *next;
};

struct topic_map_t {
  struct topic_map_entry_t **entries;
  uint32_t capacity;
  uint32_t length;
};

char *generate_id();

struct topic_map_t *init_map(const uint32_t);

uint32_t hash(const uint32_t, const char *);

void set(struct topic_map_t *, const struct topic_t *);

const struct topic_t *get(const struct topic_map_t *, const char *key);

#include <stddef.h>
#include <stdint.h>

#include "channel_subscription.h"

struct channel_t {
  size_t name_len;
  char *name;
  uint32_t id;
  uint32_t subscription_count;
  void (*add_subscription)(subscription_host);
  void (*remove_subscription)(subscription_id);
};

typedef struct channel_t channel;

channel *create_channel();
void delete_channel(uint32_t id);

void load_channels(char *);

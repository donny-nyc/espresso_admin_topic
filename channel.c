#include "channel.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

channel *create_channel() {
  channel *new_channel = (channel *)malloc(sizeof(channel));

  return new_channel;
}

void delete_channel(uint32_t id) {}

void load_channels(char *path) {}

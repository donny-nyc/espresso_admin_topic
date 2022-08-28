#include <stdint.h>

struct publish_message {
  uint32_t topic;
  uint64_t message_id;
  uint64_t checksum;
  char message[400];
  struct publish_message *next, *prev;
};

struct publish_message_list {
  uint64_t length;
  struct publish_message *first, *last;
};

struct publish_message *pop_message(struct publish_message_list *pml);
uint64_t push_message(struct publish_message_list *pml, struct publish_message *pm);

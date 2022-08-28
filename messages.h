#import <stdint.h>

struct subscribe_request {
  char *hostname;
  uint32_t port;
  uint32_t topic;
};

struct replay_request {
  uint32_t topic;
  uint64_t last_message_id;
};

struct publish_message {
  uint32_t topic;
  uint64_t message_id;
  uint64_t checksum;
  char message[400];
};

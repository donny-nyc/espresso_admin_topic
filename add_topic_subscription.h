#include <stdint.h>

struct add_topic_subscription_params_t {
  uint32_t topic_id;
  uint32_t host_url_len;
  char *host_url;
  uint32_t host_port;
};

int add_topic_subscription(struct add_topic_subscription_params_t *p);

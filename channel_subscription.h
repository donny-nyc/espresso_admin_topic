#include <stddef.h>
#include <stdint.h>

struct subscription_host_t {
  size_t host_len;
  char *host;
  uint32_t port;
};

typedef struct subscription_host_t subscription_host;


typedef uint32_t subscription_id;
struct subscription_t {
  subscription_id id;
  subscription_host *host;
};

typedef struct subscription_t subscription;

#include <stddef.h>
#include <stdint.h>

struct subscription_host_t {
  char *url;
  uint32_t port;
};

typedef struct subscription_host_t subscription_host;


typedef uint32_t subscription_id;
struct subscription_t {
  subscription_id id;
  subscription_host host;
};

typedef struct subscription_t subscription;

struct subscription_list_entry_t {
  subscription *subscription;
  struct subscription_list_entry_t *next, *prev;
};

typedef struct subscription_list_entry_t subscription_list_entry;

struct subscription_list_t {
  uint32_t count;
  subscription_list_entry *first, *last;
};

typedef struct subscription_list_t subscription_list;

subscription_list_entry *push_subscription(subscription_list *list, subscription *s);

void remove_subscription(subscription_list *list, const uint32_t id);

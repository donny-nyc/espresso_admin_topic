#include "../channel_subscription.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  subscription_list sl; 

  subscription *s, *t, *u, *v;

  s = (subscription *)malloc(sizeof(struct subscription_t));
  t = (subscription *)malloc(sizeof(struct subscription_t));
  u = (subscription *)malloc(sizeof(struct subscription_t));
  v = (subscription *)malloc(sizeof(struct subscription_t));

  s->id = 1;
  s->host = calloc(1, sizeof(struct subscription_host_t));

  t->id = 2;
  t->host = calloc(1, sizeof(struct subscription_host_t));

  u->id = 3;
  u->host = calloc(1, sizeof(struct subscription_host_t));

  v->id = 4;
  v->host = calloc(1, sizeof(struct subscription_host_t));

  add_subscription(&sl, s);
  add_subscription(&sl, t);
  add_subscription(&sl, u);
  add_subscription(&sl, v);

  printf("list count: %u\n", sl.count);

  remove_subscription(&sl, 3);

  printf("list count (removed): %u\n", sl.count);

  return 0;
}

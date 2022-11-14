#include "../channel.h"

#include <stdio.h>
#include <string.h>

int main() {
  struct channel_map_t *map = init_map(100);

  channel a, b;
  a.id = 10;
  a.name = strdup("something provocative");

  b.id = 22;
  b.name = strdup("something mundane");

  set(map, &a);
  set(map, &b);

  const channel *res = get(map, a.id);

  printf("result: %u, %s\n", res->id, res->name);

  res = get(map, b.id);

  printf("result(b): %u, %s\n", res->id, res->name);

  return 0;
}


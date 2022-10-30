#include <stdio.h>

#include "../channel_map.h"

int main(void) {
  channel_map *map = create_map(10);

  channel c;
  c.id = 0;
  c.name = "buddy";
  c.name_len = 5;

  channel d;
  d.id = 1;
  d.name = "uddyb";
  d.name_len = 5;

  channel x;
  x.id = 2;
  x.name = "asdf123";
  x.name_len = 7;

  channel_map_key *cdx = insert(map, &x);
  channel_map_key *cdk = insert(map, &d);
  channel_map_key *ck = insert(map, &c);

  printf("size: %zu\n", map->size);

  channel *res = get(map, ck);

  printf("res: %s\n", res->name);

  res = get(map, cdk);
  printf("res(cdk): %s\n", res->name);

  res = get(map, cdx);
  printf("res(cdx): %s\n", res->name);

  int r = remove_key(map, ck);
  printf("removed: %d\n", r);

  res = get(map, ck);
  printf("res: %lu\n", res);

  return 0;
}

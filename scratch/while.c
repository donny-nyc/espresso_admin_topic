#include <stdio.h>
#include <string.h>

#include "../topic.h"

int main(void) {

  printf("init map\n");
  struct topic_map_t *map = init_map(16);

  for(int i = 0; i < 4096; ++i) {
    char *id = generate_id();

    printf("new id: %s\n", id);

    struct topic_t t;
    strcpy(t.id, id);

    set(map, &t);

    const struct topic_t *res = get(map, id);

    printf("res: %s\n", res->id);
  }

  return 0;
}

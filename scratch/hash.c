#include <stdint.h>
#include <stdio.h>

uint32_t hash(const uint32_t cap, uint32_t id) {
  uint32_t index = 0;

  while(id) {
    index = (id % 10) + 32 * index;
    id /= 10;
  }

  return index % cap;
}

int main(void) {

  printf("hash: %u\n", hash(100, 54321));
  printf("hash: %u\n", hash(100, 100));
  printf("hash: %u\n", hash(100, 436));
  printf("hash: %u\n", hash(100, 436));

  return 0;
}

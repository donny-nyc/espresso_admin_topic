#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

void fun(int x) {
  printf("x: %d\n", x);
}

struct object_t {
  void (*do_stuff)(int x);
};

int main(void) {
  char *msg = "1;two;3";

  uint32_t u;
  char m[4];
  uint32_t v;

  // int count = 0;
  // scanf(msg, "%u;", &u, &count);
  // printf("u: %u\n", u);

  // scanf(msg + count, "%s;", m, &count);
  // printf("m: %s\n", m);

  // void (*f)(int x) = &fun;
  struct object_t doer;

  doer.do_stuff = &fun;

  doer.do_stuff(5);

  char *text = "another_some_file.txt";

  int g = 31;
  int hash = 0;

  for(int k = 0; k < strlen(text); k++) {
    hash += g * text[k];
  }
  int idx = hash % 10;
  printf("idx: %d\n", idx);

  return 0;
}

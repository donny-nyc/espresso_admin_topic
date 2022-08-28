#include "messages.h"
#include <stdio.h>

struct publish_message *pop_message(struct publish_message_list *pml) {
  printf("pop_message: len: %llu\n", pml->length);
  if(!pml->length) {
    return 0;
  }

  struct publish_message *first;

  first = pml->first;


  if(pml->length > 1) {
    struct publish_message *new_first;

    printf("current: %llu, next: %llu\n", first->message_id, first->next->message_id);
    new_first = first->next;

    new_first->prev = 0;

    pml->first = new_first;
  } else {
    printf("pop: len <= 1\n");
    pml->first = pml->last = 0;
  }

  first->prev = first->next = 0;

  pml->length--;

  return first;
}

uint64_t push_message(struct publish_message_list *pml, struct publish_message *pm) {
  if(!pml->length) {
    printf("push_message: %llu\n", pm->message_id);
    pm->prev = pm->next = 0;
    pml->last = pml->first = pm;
    pml->length = 1;

    return pml->length;
  }


  struct publish_message *last;
  last = pml->last;
  printf("push another message: id: %llu \t last: %llu\n", pm->message_id, last->message_id);
  last->next = pm;
  pml->last = last->next;
  pm->prev = last;
  pm->next = 0;

  pml->length++;

  return pml->length;
}

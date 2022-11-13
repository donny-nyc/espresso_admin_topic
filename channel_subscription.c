#include "channel_subscription.h"
#include <stdlib.h>

subscription_list_entry *push_subscription(
    subscription_list *list, 
    subscription *s) {
  subscription_list_entry *e;

  if(list->count) {
    list->last->next = calloc(1, sizeof(struct subscription_list_entry_t));
    e = list->last->next;
    e->prev = list->last;
    list->last = e;
  } else {
    list->last = list->first = calloc(1, sizeof(struct subscription_list_entry_t));
    e = list->last;
  }

  list->count++;

  e->subscription = s;

  return e;
}

void remove_subscription(subscription_list *list, const uint32_t id) {
  subscription_list_entry *entry = list->first;

  while(entry) {
    if(entry->subscription->id == id) {
      if(entry->prev) {
        entry->prev->next = entry->next;
      } else {
        // need to update the head of the list
        if(entry->next) {
          list->first = entry->next;
        } else {
          list->first = NULL;
        }
      }

      if(entry->next) {
        entry->next->prev = entry->prev;
      } else {
        if(entry->prev) {
          list->last = entry->prev;
        } else {
          list->last = NULL;
        }
      }

      free(entry->subscription);
      free(entry);

      list->count--;

      return;
    }

    entry = entry->next;
  }
}

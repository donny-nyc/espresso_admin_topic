#include "../channel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
channel *from_file(FILE *f) {
  channel *c = calloc(1, sizeof(struct channel_t)); 

  if(!c) {
    dprintf(STDERR_FILENO, "Could not alloc space for channel\n");
    return c;
  }

  size_t name_len = 0;

  size_t res = fread(&name_len, sizeof(uint32_t), 1, f);

  if(!res) {
    dprintf(STDERR_FILENO, "Failed to read name len from file\n");
    free(c);

    return NULL;
  }

  if(name_len) {
    char *name = calloc(name_len, sizeof(char));
    res = fread(name, sizeof(char), name_len, f);

    c->name = strdup(name);
  } 

  size_t subscriptions_count = 0;
  res = fread(&subscriptions_count, sizeof(uint32_t), 1, f);

  if(!res) {
    dprintf(STDERR_FILENO, "Failed to read subscription count from file\n");
  }

  subscription *sub; 
  for(size_t i = 0; i < subscriptions_count; i++) {
    sub = calloc(1, sizeof(struct subscription_t)); 

    res = fread(&sub->id, sizeof(uint32_t), 1, f);

    if(!res) {
      dprintf(STDERR_FILENO, "Failed to read sub ID\n");
      free(sub);
      free(c);

      return NULL;
    }

    size_t host_len = 0;
    res = fread(&host_len, sizeof(uint32_t), 1, f);

    if(!res) {
      dprintf(STDERR_FILENO, "Failed to read host url len\n");
      free(c);
      free(sub);
      return NULL;
    }

    sub->host.url = calloc(host_len, sizeof(char));
    res = fread(&sub->host.url, sizeof(char), host_len, f);

    if(!res) {
      dprintf(STDERR_FILENO, "Failed to read host url\n");

      free(sub->host.url);
      free(sub);
      free(c);

      return NULL;
    }


    res = fread(&sub->host.port, sizeof(uint32_t), 1, f);

    if(!res) {
      dprintf(STDERR_FILENO, "Failed to read host port\n");

      free(sub->host.url);
      free(sub);
      free(c);

      return NULL;
    }

    if(!add_subscription(&c->subscriptions, sub)) {
      dprintf(STDERR_FILENO, "Failed to add subscription\n");

      free(sub->host.url);
      free(sub);
      free(c);

      return NULL;
    }
  }

  return c;
}

void to_file(FILE *f, channel *c) {
  size_t name_len = strlen(c->name);

  size_t res = fwrite(&name_len, sizeof(uint32_t), 1, f);

  if(!res) {
    dprintf(STDERR_FILENO, "Failed to write name_len\n");
    return;
  }

  res = fwrite(c->name, sizeof(char), name_len, f);

  if(!res) {
    dprintf(STDERR_FILENO, "Failed to write channel name\n");
    return;
  }

  res = fwrite(&c->subscriptions.count, sizeof(char), name_len, f);

  if(c->subscriptions.count) {
    res = fwrite(&c->subscriptions.count, sizeof(uint32_t), 1, f);

    subscription_list_entry *entry = c->subscriptions.first;

    while(entry) {
      res= fwrite(&entry->subscription->id, sizeof(uint32_t), 1, f);
      if(!res) {
        dprintf(STDERR_FILENO, "Failed to write sub id\n");
        return;
      }

      size_t host_len = strlen(entry->subscription->host.url);
      res = fwrite(&host_len, sizeof(uint32_t), 1, f);

      if(!res) {
        dprintf(STDERR_FILENO, "Failed to write host name len\n");
      }

      res = fwrite(&entry->subscription->host.url, sizeof(char), host_len, f);
      if(!res) {
        dprintf(STDERR_FILENO, "Failed to write host url\n");
      }

      res = fwrite(&entry->subscription->host.port, sizeof(uint32_t), 1, f);
      if(!res) {
        dprintf(STDERR_FILENO, "Failed to write host port\n");
      }

      entry = entry->next;
    } 
  }
}
*/

int main(void) {
  char *tmp_path = "./channel2.dat";

  char *name = "my sub name";

  channel c;
  c.name = name;

  add_subscription(&c, "https://example.com", 8080);
  add_subscription(&c, "https://anotherexample.com", 1234);


  FILE *f = fopen(tmp_path, "w");
  to_file(f, &c);
  fclose(f);

  channel *res;

  f = fopen(tmp_path, "r");
  res = from_file(f);
  fclose(f);

  printf("channel: %s\n", res->name);
  printf("channel sub name: %s\n", c.subscriptions.last->subscription->host.url);

  return 0;
}

#include <stdint.h>

enum message_type { COMMAND=1, EVENT };

struct message_t {
  enum message_type type;
  char body[500];
};

enum command_t { 
  ADD_TOPIC_SUBSCRIPTION=1, // topic_id, host, port
  DELETE_TOPIC_SUBSCRIPTION,
  REPLACE_TOPIC_SUBSCRIPTION,
  ADD_TOPIC,
  DELETE_TOPIC
};

struct command_message {
  enum command_t type;
  uint32_t body_len;
  char *body;  
};

struct add_topic_subscription_t {
  uint32_t topic_id;
};

/*
struct request_t {
  uint32_t topic_id;
  char body[500];
};
*/




// what are we broadcasting?
// nothing interesting for the 

/*
struct broadcast_message {
  uint32_t ;

}

// why define it here? This behavior is only relevant to the destination service
// why should the topic care whether the client wants a callback?
// all it should be concerned with is where to direct the message
//
// Update 1:
//
// Maybe it should care about some message types: commands vs. messages
*/


// Do we need to include a callback addr in the message?

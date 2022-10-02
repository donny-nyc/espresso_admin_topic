# Espresso Admin Topic
A log-based, pub-sub service for asynchronous processing.

## Overview

### Components

#### Message

#### Input Buffer

#### Log

#### Subscription

#### Sidecar API

#### Channels
We expose a simple CRUD API for Channels. How are these data types persisted?
Do we introduce another file-based storage solution?

```
id,name,destination_host,destination_port
```

#### Client Registry
Clients that have completed a handshake with the server are registered with a
unique identifier. This ID is returned to the client when the handshake is
completed. This feels week, and will probably be replaced with TLS / OAUTH
almost immediately.

// how can we delegate auth, so that this service can focus solely on
// receiving messages from publishers and broadcasting to subscribers?
//
// Accept any message that comes in, deploy behind a gatekeeper that checks auth
// and _only_ forwards authorized notifications

#### Admin Client

### Architecture

#### Client Greeter Thread

#### Log Writer Thread

### Protocols

#### Client Handshake

1. Client       --HELLO->      Server
2. Client <--HELLO,CHALLENGE-- Server
3. Client      --RESPONSE-->   Server
4. Client         <--OK--      Server
5. Client         ---OK-->     Server


### Message Schema

#### `struct subscribe_request`
```
struct subscribe_message {
  char *hostname; // subscriber's host we forward messages to
  uint32 port; // subscriber's port we'll forward messages to
  uint32 topic;  // which topic is being requested
};
```

#### `struct replay_request`
```
// resend messages from selected topic to the caller, starting from
// the given `last_message_id`.
struct replay_request {
  uint32 topic;
  uint64 last_message_id;
};
```

#### `struct publish_message`
```
// messages should probably have a fixed-size, so that we can
// quickly skip around the log file, given an ID, we can compute the exact offset
struct publish_message {
  uint32 topic;
  uint64 message_id;
  uint64 checksum;
  char message[400];
};
```
### Open Questions
1. Encrypted messages?
2. Configuration
3. Eventual consistency:
3a. I'm replaying historic messages, and a fresh message comes in. What can happen?
4. How do we fan messages in and out of this log?
4a. as a subscriber (process) I should receive a notification when an event happens in my topic
4b. as a publisher (process) I should be able to emit event notifications to my topic

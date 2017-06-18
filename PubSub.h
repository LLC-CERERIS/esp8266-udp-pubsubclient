#ifndef PUBSUB_H
#define PUBSUB_H

enum Type {
  ERROR = 1,
  CONNECT = 2,
  DISCONNECT = 3,
  PUBLISH = 4,
  REPLY = 5,
  SUBSCRIBE = 6,
  UNSUBSCRIBE = 7,
  KEEP_ALIVE = 8,
};

#define CALLBACK_SIGNATURE void (*callback)(char*, char*)
#define CONNECT_CALLBACK_SIGNATURE void (*connectCallback)()
#define DISCONNECT_CALLBACK_SIGNATURE void (*disconnectCallback)()

#define DEFAULT_KEEP_ALIVE_TIMEOUT 5
#define DEFAULT_RETRY_COUNT 10
#define DEFAULT_TIMEOUT 1000
#define BUFFER_SIZE 255

#include <cstdio>
#include <cstring>
#include "Arduino.h"

char *addressFromInt(uint32_t addr);

#endif //PUBSUB_H

#ifndef PUBSUB_H
#define PUBSUB_H

#include <cstdio>
#include <cstdint>

enum Type {
  ERROR = -1,
  CONNECT,
  DISCONNECT,
  PUBLISH,
  REPLY,
  SUBSCRIBE,
  UNSUBSCRIBE,
  KEEP_ALIVE
};

#ifdef ESP8266
#include <functional>
#define CALLBACK_SIGNATURE std::function<void(char*, char*)> callback
#else
#define CALLBACK_SIGNATURE void (*callback)(char*, char*)
#endif

#ifdef ESP8266
#include <functional>
#define CONNECT_CALLBACK_SIGNATURE std::function<void()> connectCallback
#else
#define CONNECT_CALLBACK_SIGNATURE void (*connectCallback)()
#endif

#ifdef ESP8266
#include <functional>
#define DISCONNECT_CALLBACK_SIGNATURE std::function<void()> disconnectCallback
#else
#define DISCONNECT_CALLBACK_SIGNATURE void (*disconnectCallback)()
#endif

#define DEFAULT_KEEP_ALIVE_TIMEOUT 5
#define DEFAULT_RETRY_COUNT 10
#define DEFAULT_TIMEOUT 1000
#define BUFFER_SIZE 255


char *addressFromInt(uint32_t addr);

#endif //PUBSUB_H

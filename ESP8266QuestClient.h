/*
 * QuestClient for ESP8266
 * LLC CERERIS
 */

#ifndef ESP8266_QUEST_CLIENT_H
#define ESP8266_QUEST_CLIENT_H

#include <Arduino.h>
#include "WiFiUdp.h"

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

#define PUBLISH_TYPE 1
#define REPLY_TYPE 2
#define SUBSCRIBE_TYPE 3
#define KEEP_ALIVE_TYPE 4

#define DEFAULT_KEEP_ALIVE_TIMEOUT 5
#define BUFFER_SIZE 255

class ESP8266QuestClient
{
public:
  ESP8266QuestClient();
  ESP8266QuestClient(WiFiUDP& wifiClient);

  ESP8266QuestClient& setCallback(CALLBACK_SIGNATURE);
  ESP8266QuestClient& setConnectCallback(CONNECT_CALLBACK_SIGNATURE);
  ESP8266QuestClient& setClient(WiFiUDP& client);
  ESP8266QuestClient& setKeepAliveTimeout(uint8_t timeout = 10);

  void setServer(const char* address, uint16_t port);
  bool publish(const char* topic, const char* message, uint16_t timeout = 500, uint8_t retryCount = 3);
  bool subscribe(const char* topic, uint16_t timeout = 500, uint8_t retryCount = 3);

  void loop();
private:
  void setConnected(bool connected);
  void keepAlive(uint16_t timeout = 500, uint8_t retryCount = 3);
  uint8_t handle(const char* data);
private:
  WiFiUDP* _client;
  CALLBACK_SIGNATURE;
  CONNECT_CALLBACK_SIGNATURE;
  char _buffer[BUFFER_SIZE];
  uint8_t _keepAliveTimeout;
  const char* _address;
  uint16_t _port;
  bool _connected;
  unsigned long _time;
};

#endif

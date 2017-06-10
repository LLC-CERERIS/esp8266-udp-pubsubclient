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
#define DEFAULT_RETRY_COUNT 10
#define DEFAULT_TIMEOUT 1000
#define BUFFER_SIZE 255

class ESP8266PubSubClient {
public:
  ESP8266PubSubClient();
  ESP8266PubSubClient(WiFiUDP &wifiClient);

  // Sets callback which is fired when publish packet received.
  // Process your subscriptions here.
  ESP8266PubSubClient &setCallback(CALLBACK_SIGNATURE);

  // Sets connect callback which is fired when client connects to server.
  // Send your subscriptions here.
  ESP8266PubSubClient &setConnectCallback(CONNECT_CALLBACK_SIGNATURE);

  ESP8266PubSubClient &setClient(WiFiUDP &client);
  ESP8266PubSubClient &setKeepAliveTimeout(uint8_t timeout = DEFAULT_KEEP_ALIVE_TIMEOUT);

  void setServer(const char *address, uint16_t port);

  // Send publish packet to server
  bool publish(const char *topic, const char *message, uint16_t timeout = DEFAULT_TIMEOUT, uint8_t retryCount = DEFAULT_RETRY_COUNT);

  // Send subscribe packet to server
  bool subscribe(const char *topic, uint16_t timeout = DEFAULT_TIMEOUT, uint8_t retryCount = DEFAULT_RETRY_COUNT);

  // Doesn't freeze Wi-Fi & can wait in ms
  void smartDelay(const uint32_t delayTime);

  // Main loop
  void loop();
private:
  // Set current connection state to "connected"
  void setConnected(bool connected);

  // Send keep-alive packet to server
  void keepAlive(uint16_t timeout = DEFAULT_TIMEOUT, uint8_t retryCount = DEFAULT_RETRY_COUNT);

  // Handle packet
  uint8_t handle(const char *data);
private:
  WiFiUDP *_client; // UDP client
  CALLBACK_SIGNATURE;
  CONNECT_CALLBACK_SIGNATURE;
  char _buffer[BUFFER_SIZE]; // Last packet buffer
  uint8_t _keepAliveTimeout; // Period between keep-alive checks
  const char *_address; // Server address
  uint16_t _port; // Server port
  bool _connected; // Connection state
  unsigned long _time; // Last keep-alive check time
};

#endif

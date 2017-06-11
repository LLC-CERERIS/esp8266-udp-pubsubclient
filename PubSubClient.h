#ifndef PUBSUBCLIENT_H
#define PUBSUBCLIENT_H

#include <Arduino.h>
#include "WiFiUdp.h"
#include "PubSub.h"

class PubSubClient {
public:
  PubSubClient();
  PubSubClient(WiFiUDP &wifiClient);

  // Sets callback which is fired when publish packet received.
  // Process your subscriptions here.
  PubSubClient &setCallback(CALLBACK_SIGNATURE);

  // Sets connect callback which is fired when client connects to server.
  // Send your subscriptions here.
  PubSubClient &setConnectCallback(CONNECT_CALLBACK_SIGNATURE);

  PubSubClient &setClient(WiFiUDP &client);
  PubSubClient &setKeepAliveTimeout(uint8_t timeout = DEFAULT_KEEP_ALIVE_TIMEOUT);

  void setServer(char *address, uint16_t port);

  // Send publish packet to server
  bool publish(char *topic, char *message, uint16_t timeout = DEFAULT_TIMEOUT, uint8_t retryCount = DEFAULT_RETRY_COUNT);

  // Send subscribe packet to server
  bool subscribe(char *topic, uint16_t timeout = DEFAULT_TIMEOUT, uint8_t retryCount = DEFAULT_RETRY_COUNT);

  // Doesn't freeze Wi-Fi & can wait in ms
  void smartDelay(const uint32_t delayTime);

  // Main loop
  void loop();

  // Handle packet
  Type handle(const char *data);

  // Set current connection state to "connected"
  void setConnected(bool connected);

  WiFiUDP *getClient();
  char *getBuffer();
private:
  // Send keep-alive packet to server
  void keepAlive(uint16_t timeout = DEFAULT_TIMEOUT, uint8_t retryCount = DEFAULT_RETRY_COUNT);

  WiFiUDP *_client; // UDP client
  CALLBACK_SIGNATURE;
  CONNECT_CALLBACK_SIGNATURE;
  char _buffer[BUFFER_SIZE]; // Last packet buffer
  uint8_t _keepAliveTimeout; // Period between keep-alive checks
  char *_address; // Server address
  uint16_t _port; // Server port
  bool _connected; // Connection state
  unsigned long _time; // Last keep-alive check time
  bool isConnected();
};

#endif // PUBSUBCLIENT_H

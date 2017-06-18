#ifndef PUBSUBSERVER_H
#define PUBSUBSERVER_H

#include <Arduino.h>
#include "WiFiUdp.h"
#include "PubSub.h"
#include "Vector.h"

// cleanup inactive clients every "CLEANUP_PERIOD"
#define CLEANUP_PERIOD 30
const int MAX_DISCONNECT_TIME = DEFAULT_KEEP_ALIVE_TIMEOUT * DEFAULT_RETRY_COUNT;

namespace PubSub {
  struct Client {
    IPAddress *address;
    int port;

    Client(IPAddress *address, int port) {
      this->address = address;
      this->port = port;
    }
  };

  struct Topic {
    char *topic;
    Client *client;
    int lastUpdate = (int) millis();

    Topic() {}

    Topic(char *topic, Client *client) {
      this->topic = topic;
      this->client = client;
    }
  };
}

class PubSubServer {
private:
  Vector<PubSub::Topic> topics;
  WiFiUDP *_client;
  int cleanupCounter = 0;

public:
  PubSubServer(WiFiUDP &client);

  void setClient(WiFiUDP &client);
  WiFiUDP *getClient();
  Type handle(PubSub::Client *client, char *data);
  void loop();
  void onClientConnected(PubSub::Client *client);
  void onClientDisconnected(PubSub::Client *client);
  void onSubscribe(PubSub::Client *client, char *data);
  void onUnsubscribe(PubSub::Client *client, char *data);
  void onPublish(char *data);
  void sendReply(PubSub::Client *client);
  void sendPublish(PubSub::Client *client, char *topic, char *message);
  void onKeepAlive(PubSub::Client *client);
};


#endif //PUBSUBSERVER_H

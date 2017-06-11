#ifndef PUBSUBSERVER_H
#define PUBSUBSERVER_H

#include <Arduino.h>
#include "WiFiUdp.h"
#include "PubSub.h"
#include "Vector.h"

struct Client {
  char *address;
  int port;

  Client(char *address, int port) {
    this->address = address;
    this->port = port;
  }
};

struct Topic {
  char *topic;
  Client *client;

  Topic() {}

  Topic(char *topic, Client *client) {
    this->topic = topic;
    this->client = client;
  }
};

class PubSubServer {
private:
  Vector<Topic> topics;
  WiFiUDP *_client;

public:
  PubSubServer();

  void setClient(WiFiUDP &client);
  WiFiUDP *getClient();
  Type handle(Client *client, char *data);
  void loop();
  void onClientConnected(Client *client);
  void onClientDisconnected(Client *client);
  void onSubscribe(Client *client, char *data);
  void onUnsubscribe(Client *client, char *data);
  void onPublish(char *data);
  void sendReply(Client *client);
  void sendPublish(Client *client, char *topic, char *message);
};


#endif //PUBSUBSERVER_H

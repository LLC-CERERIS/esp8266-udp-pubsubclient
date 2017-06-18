#ifndef UDPPACKET_H
#define UDPPACKET_H


#include "Vector.h"
#include "PubSubClient.h"
#include "PubSubServer.h"
#include "Arduino.h"

class UDPPacket {
private:
  Vector<char *> *args = new Vector<char *>();
  IPAddress *_address;
  int _port;
  Type _type;

public:
  UDPPacket(PubSub::Client *client, Type type) : UDPPacket(client->address, client->port, type) {}

  ~UDPPacket();

  UDPPacket(IPAddress *address, int port, Type type) {
    this->_address = address;
    this->_port = port;
    this->_type = type;
  }

  UDPPacket add(char *arg);
  char *send(PubSubClient *client);
  char *send(PubSubServer *server);
  void sendWithoutResponse(WiFiUDP *client);
};


#endif //UDPPACKET_H

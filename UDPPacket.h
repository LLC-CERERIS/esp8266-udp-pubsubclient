#ifndef UDPPACKET_H
#define UDPPACKET_H


#include "Vector.h"
#include "PubSubClient.h"
#include "PubSubServer.h"

class UDPPacket {
private:
  Vector<char *> args;
  char *_address;
  int _port;
  Type _type;

public:
  UDPPacket(Client *client, Type type) : UDPPacket(client->address, client->port, type) {}

  UDPPacket(char *address, int port, Type type) {
    this->_address = address;
    this->_port = port;
    this->_type = type;
  }

  UDPPacket add(char *arg);
  char *send(PubSubClient *client);
  char *send(PubSubServer *server);
};


#endif //UDPPACKET_H

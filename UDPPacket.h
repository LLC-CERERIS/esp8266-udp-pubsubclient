#ifndef UDPPACKET_H
#define UDPPACKET_H

#include "Vector.h"
#include "PubSubClient.h"
#include "PubSubServer.h"
#include "Arduino.h"
#include "SPtr.h"

class UDPPacket {
private:
  Ref<Vector<char*>> *args;
  IPAddress _address;
  int _port;
  Type _type;

public:

  UDPPacket(PubSub::Client *client, Type type);

  ~UDPPacket();

  UDPPacket(IPAddress address, int port, Type type);

  UDPPacket &add(char *arg);
  void send(PubSubClient *client);
  void send(PubSubServer *server);
  void sendWithoutResponse(WiFiUDP *client);
};


#endif //UDPPACKET_H

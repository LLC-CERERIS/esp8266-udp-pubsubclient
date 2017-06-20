#include "PubSub.h"
#include "UDPPacket.h"

UDPPacket &UDPPacket::add(char *arg) {
  (*args)->add(arg);
  return *this;
}

void UDPPacket::send(PubSubClient *client) {
  int retryCount = DEFAULT_RETRY_COUNT;
  int timeout = DEFAULT_TIMEOUT;
  while (retryCount) {
    sendWithoutResponse(client->getClient());

    unsigned long timeDelay = millis();
    while (abs((int) (millis() - timeDelay)) < timeout) {
      if (client->getClient()->parsePacket()) {
        char buffer[BUFFER_SIZE];
        int len = client->getClient()->read(buffer, BUFFER_SIZE);
        if (len > 0) {
          buffer[len] = 0;
          Type result = client->handle(buffer);
          if (result != ERROR) {
            client->setConnected(true);
            return;
          }
        }
      }
      yield();
      delay(1);
    }

    retryCount--;
  }

  client->setConnected(false);
}

void UDPPacket::send(PubSubServer *server) {
  int retryCount = DEFAULT_RETRY_COUNT;
  int timeout = DEFAULT_TIMEOUT;
  while (retryCount) {
    sendWithoutResponse(server->getClient());

    unsigned long timeDelay = millis();
    while (abs(millis() - timeDelay) < timeout) {
      if (server->getClient()->parsePacket()) {
        char buffer[BUFFER_SIZE];
        int len = server->getClient()->read(buffer, BUFFER_SIZE);
        if (len > 0) {
          buffer[len] = 0;
          PubSub::Client client(_address, _port);
          if (server->handle(&client, buffer) != ERROR) {
            return;
          }
        }
      }
      yield();
      delay(1);
    }

    retryCount--;
  }

  return;
}

void UDPPacket::sendWithoutResponse(WiFiUDP *client) {
  client->beginPacket(_address, (uint16_t) _port);
  client->write((uint8_t) _type);
  Serial.print("Sending packet: ");
  Serial.println((uint8_t) _type);
  for (int i = 0; i < (*args)->size(); i++) {
    client->print((*args)->get(i));
  }
  client->endPacket();
}

UDPPacket::~UDPPacket() {
  delete args;
}

UDPPacket::UDPPacket(IPAddress address, int port, Type type) {
  SPtr<Vector<char *>> *ptr = new SPtr<Vector<char *>>(new Vector<char *>);
  args = ptr->get();

  this->_address = address;
  this->_port = port;
  this->_type = type;
}

UDPPacket::UDPPacket(PubSub::Client *client, Type type) : UDPPacket(client->address, client->port, type) {}

#include "PubSub.h"
#include "UDPPacket.h"

UDPPacket UDPPacket::add(char *arg) {
  args.add(arg);
  return *this;
}

char *UDPPacket::send(PubSubClient *client) {
  int retryCount = DEFAULT_RETRY_COUNT;
  int timeout = DEFAULT_TIMEOUT;
  while (retryCount) {
    client->getClient()->beginPacket(_address, _port);
    client->getClient()->write((uint8_t) _type);

    for (int i = 0; i < args.size(); i++) {
      client->getClient()->write(args.get(i));
    }

    unsigned long timeDelay = millis();
    while (abs(millis() - timeDelay) < timeout) {
      if (client->getClient()->parsePacket()) {
        int len = client->getClient()->read(client->getBuffer(), BUFFER_SIZE);
        if (len > 0) {
          client->getBuffer()[len] = 0;
          if (client->handle(client->getBuffer()) != ERROR) {
            client->setConnected(true);
            return client->getBuffer();
          }
        }
      }
      yield();
      delay(1);
    }

    retryCount--;
  }

  client->setConnected(false);
  return nullptr;
}

char *UDPPacket::send(PubSubServer *server) {
  int retryCount = DEFAULT_RETRY_COUNT;
  int timeout = DEFAULT_TIMEOUT;
  while (retryCount) {
    server->getClient()->beginPacket(_address, _port);
    server->getClient()->write((uint8_t) _type);

    for (int i = 0; i < args.size(); i++) {
      server->getClient()->write(args.get(i));
    }

    unsigned long timeDelay = millis();
    while (abs(millis() - timeDelay) < timeout) {
      if (server->getClient()->parsePacket()) {
        char *buffer = new char[BUFFER_SIZE];
        int len = server->getClient()->read(buffer, BUFFER_SIZE);
        if (len > 0) {
          buffer[len] = 0;
          if (server->handle(new Client(_address, _port), buffer) != ERROR) {
            return buffer;
          }
        }
      }
      yield();
      delay(1);
    }

    retryCount--;
  }

  return nullptr;
}
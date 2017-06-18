#include "PubSub.h"
#include "UDPPacket.h"

UDPPacket UDPPacket::add(char *arg) {
  args->add(arg);
  return *this;
}

char *UDPPacket::send(PubSubClient *client) {
  int retryCount = DEFAULT_RETRY_COUNT;
  int timeout = DEFAULT_TIMEOUT;
  while (retryCount) {
    sendWithoutResponse(client->getClient());

    unsigned long timeDelay = millis();
    while (abs(millis() - timeDelay) < timeout) {
      if (client->getClient()->parsePacket()) {
        char buffer[BUFFER_SIZE];
        int len = client->getClient()->read(buffer, BUFFER_SIZE);
        if (len > 0) {
          buffer[len] = 0;
          Type result = client->handle(buffer);
          Serial.print("Result: ");
          Serial.println(result);
          if (result != ERROR) {
            client->setConnected(true);
            return buffer;
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

void UDPPacket::sendWithoutResponse(WiFiUDP *client) {
  client->beginPacket(*_address, (uint16_t) _port);
  client->write((uint8_t) _type);
  Serial.print("Sending packet: ");
  Serial.println((uint8_t) _type);
  for (int i = 0; i < args->size(); i++) {
    client->print(args->get(i));
  }
  client->endPacket();
}

UDPPacket::~UDPPacket() {
  Serial.println("Calling UDPPacket destructor");
  delete args;
}

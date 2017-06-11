#include "PubSubServer.h"
#include "UDPPacket.h"

PubSubServer::PubSubServer() {
}

Type PubSubServer::handle(Client *client, char *data) {
  int len = (int) strlen(data);
  if (len == 0)
    return ERROR;
  Type type = (Type) data[0];
  switch (type) {
    case KEEP_ALIVE: {
      sendReply(client);
      break;
    }
    case CONNECT: {
      onClientConnected(client);
      sendReply(client);
      break;
    }
    case DISCONNECT: {
      onClientDisconnected(client);
      sendReply(client);
      break;
    }
    case SUBSCRIBE: {
      onSubscribe(client, data);
      sendReply(client);
      break;
    }
    case UNSUBSCRIBE: {
      onUnsubscribe(client, data);
      sendReply(client);
      break;
    }
    case PUBLISH: {
      onPublish(data);
      sendReply(client);
      break;
    }
  }
  return ERROR;
}

void PubSubServer::loop() {
  while (_client->parsePacket()) {
    char buffer[BUFFER_SIZE];
    int len = _client->read(buffer, BUFFER_SIZE);
    if (len > 0) {
      buffer[len] = 0;
      handle(new Client(addressFromInt(_client->remoteIP()), _client->remotePort()), buffer);
    }
    yield();
    delay(1);
  }
}

void PubSubServer::setClient(WiFiUDP &client) {
  this->_client = &client;
}

WiFiUDP *PubSubServer::getClient() {
  return _client;
}

void PubSubServer::onClientConnected(Client *client) {
  // delete all topics related to this address, because it should resend them one more time
  for (Topic t : topics) {
    if (t.client->address == client->address)
      topics.remove(t);
  }
}

void PubSubServer::onClientDisconnected(Client *client) {
  // delete all topics related to this address
  for (Topic t : topics) {
    if (t.client->address == client->address)
      topics.remove(t);
  }
}

void PubSubServer::onSubscribe(Client *client, char *data) {
  int len = (int) strlen(data);

  if (len < 2)
    return; // no topic length

  uint8_t topicLength = (uint8_t) data[1];

  if (len < 2 + topicLength)
    return; // topic not full

  char topic[topicLength + 1];
  strncpy(topic, &data[2], topicLength);

  topics.add(Topic(topic, client));
}

void PubSubServer::onUnsubscribe(Client *client, char *data) {
  int len = (int) strlen(data);

  if (len < 2)
    return; // no topic length

  uint8_t topicLength = (uint8_t) data[1];

  if (len < 2 + topicLength)
    return; // topic not full

  char topic[topicLength + 1];
  strncpy(topic, &data[2], topicLength);

  topics.remove(Topic(topic, client));
}

void PubSubServer::onPublish(char *data) {
  int len = (int) strlen(data);

  if (len < 2)
    return; // no topic length

  uint8_t topicLength = (uint8_t) data[1];

  if (len < 2 + topicLength)
    return; // no topic

  char topic[topicLength + 1];
  strncpy(topic, &data[2], topicLength);

  if (len < 3 + topicLength)
    return; // no message length

  uint8_t messageLength = (uint8_t) data[topicLength + 3];

  if (len < 4 + topicLength + messageLength)
    return; // no message

  char message[messageLength + 1];
  strncpy(message, &data[4 + topicLength], topicLength);

  for (Topic t : topics) {
    if (strcmp(t.topic, topic)) {
      sendPublish(t.client, topic, message);
    }
  }
}

void PubSubServer::sendReply(Client *client) {
  UDPPacket(client->address, client->port, REPLY).send(this);
}

void PubSubServer::sendPublish(Client *client, char *topic, char *message) {
  UDPPacket(client, PUBLISH)
          .add((char *) (uint8_t) strlen(topic))
          .add(topic)
          .add((char *) (uint8_t) strlen(message))
          .add(message)
          .send(this);
}

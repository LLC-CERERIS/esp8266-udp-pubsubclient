#include "PubSubServer.h"
#include "UDPPacket.h"

PubSubServer::PubSubServer(WiFiUDP &client) {
  setClient(client);
}

Type PubSubServer::handle(PubSub::Client *client, char *data) {
  Serial.print("Received data: ");
  Serial.println(data);
  int len = (int) strlen(data);
  if (len == 0)
    return ERROR;
  Type type = (Type) data[0];
  if (type != REPLY)
    sendReply(client);
  switch (type) {
    case KEEP_ALIVE: {
      onKeepAlive(client);
      return KEEP_ALIVE;
    }
    case CONNECT: {
      onClientConnected(client);
      return CONNECT;
    }
    case DISCONNECT: {
      onClientDisconnected(client);
      return DISCONNECT;
    }
    case SUBSCRIBE: {
      onSubscribe(client, data);
      return SUBSCRIBE;
    }
    case UNSUBSCRIBE: {
      onUnsubscribe(client, data);
      return UNSUBSCRIBE;
    }
    case PUBLISH: {
      onPublish(data);
      return PUBLISH;
    }
    case REPLY: {
      return REPLY;
    }
  }
  return ERROR;
}

// run this from loop to process all updates
void PubSubServer::loop() {
  cleanupCounter--;
  if (cleanupCounter == 0) {
    cleanupCounter = CLEANUP_PERIOD;
    topics.foreach([](PubSub::Topic t) -> bool {
      return t.lastUpdate > millis();
    });
  }

  // read all available packets
  while (_client->parsePacket()) {
    char buffer[BUFFER_SIZE];
    int len = _client->read(buffer, BUFFER_SIZE);
    if (len > 0) {
      buffer[len] = 0;
      IPAddress address = _client->remoteIP();
      PubSub::Client client(&address, _client->remotePort());
      handle(&client, buffer);
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

void PubSubServer::onClientConnected(PubSub::Client *client) {
  // delete all topics related to this address, because it should resend them one more time
  for (int i = topics.size() - 1; i >= 0; i--) {
    if (topics.get(i).client->address == client->address)
      topics.remove(i);
  }
}

void PubSubServer::onClientDisconnected(PubSub::Client *client) {
  // delete all topics related to this address
  for (int i = topics.size() - 1; i >= 0; i--) {
    if (topics.get(i).client->address == client->address)
      topics.remove(i);
  }
}

void PubSubServer::onSubscribe(PubSub::Client *client, char *data) {
  int len = (int) strlen(data);

  if (len < 2)
    return; // no topic length

  uint8_t topicLength = (uint8_t) data[1];

  if (len < 2 + topicLength)
    return; // topic not full

  char topic[topicLength + 1];
  topic[topicLength] = 0;
  strncpy(topic, &data[2], topicLength);

  topics.add(PubSub::Topic(topic, client));
}

void PubSubServer::onUnsubscribe(PubSub::Client *client, char *data) {
  int len = (int) strlen(data);

  if (len < 2)
    return; // no topic length

  uint8_t topicLength = (uint8_t) data[1];

  if (len < 2 + topicLength)
    return; // topic not full

  char topic[topicLength + 1];
  topic[topicLength] = 0;
  strncpy(topic, &data[2], topicLength);

  topics.remove(PubSub::Topic(topic, client));
}

void PubSubServer::onPublish(char *data) {
  int len = (int) strlen(data);

  if (len < 2)
    return; // no topic length

  uint8_t topicLength = (uint8_t) data[1];

  if (len < 2 + topicLength)
    return; // no topic

  char topic[topicLength + 1];
  topic[topicLength] = 0;
  strncpy(topic, &data[2], topicLength);

  if (len < 3 + topicLength)
    return; // no message length

  uint8_t messageLength = (uint8_t) data[topicLength + 2];

  if (len < 3 + topicLength + messageLength)
    return; // no message

  char message[messageLength + 1];
  message[messageLength] = 0;
  strncpy(message, &data[3 + topicLength], messageLength);

  for (PubSub::Topic t : topics) {
    if (strcmp(t.topic, topic) == 0) {
      sendPublish(t.client, topic, message);
    }
  }
}

void PubSubServer::sendReply(PubSub::Client *client) {
  UDPPacket(client->address, client->port, REPLY).sendWithoutResponse(_client);
}

void PubSubServer::sendPublish(PubSub::Client *client, char *topic, char *message) {
  char topicLength[2] = {(char) strlen(topic), '\0'},
          messageLength[2] = {(char) strlen(message), '\0'};

  UDPPacket(client, PUBLISH)
          .add(topicLength)
          .add(topic)
          .add(messageLength)
          .add(message)
          .send(this);
}

void PubSubServer::onKeepAlive(PubSub::Client *client) {
  for (PubSub::Topic t : topics) {
    if (t.client->address == client->address)
      t.lastUpdate = (int) millis();
  }
}

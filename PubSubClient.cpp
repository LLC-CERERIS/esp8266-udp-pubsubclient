#include "PubSubClient.h"

PubSubClient::PubSubClient() {
  _client = NULL;
  setCallback(NULL);
  _keepAliveTimeout = DEFAULT_KEEP_ALIVE_TIMEOUT;
  _time = millis();
  setConnectCallback(NULL);
}

PubSubClient::PubSubClient(WiFiUDP &client) {
  _keepAliveTimeout = DEFAULT_KEEP_ALIVE_TIMEOUT;
  _time = millis();
  setClient(client);
  setCallback(NULL);
  setConnectCallback(NULL);
}

PubSubClient &PubSubClient::setCallback(CALLBACK_SIGNATURE) {
  this->callback = callback;
  return *this;
}

PubSubClient &PubSubClient::setConnectCallback(CONNECT_CALLBACK_SIGNATURE) {
  this->connectCallback = connectCallback;
  return *this;
}

PubSubClient &PubSubClient::setClient(WiFiUDP &client) {
  _client = &client;
  return *this;
}

void PubSubClient::setServer(IPAddress address, uint16_t port) {
  _address = address;
  _port = port;
}

PubSubClient &PubSubClient::setKeepAliveTimeout(uint8_t timeout) {
  _keepAliveTimeout = timeout;
  return *this;
}

bool PubSubClient::publish(char *topic, char *message) {
  char topicLength[2] = {(char) strlen(topic), '\0'},
          messageLength[2] = {(char) strlen(message), '\0'};

  UDPPacket(_address, _port, PUBLISH)
          .add(topicLength)
          .add(topic)
          .add(messageLength)
          .add(message)
          .send(this);

  return isConnected();
}

bool PubSubClient::subscribe(char *topic, uint16_t timeout, uint8_t retryCount) {
  char topicLength[2] = {(char) strlen(topic), '\0'};

  UDPPacket(_address, _port, SUBSCRIBE)
          .add(topicLength)
          .add(topic)
          .send(this);
}

void PubSubClient::keepAlive(uint16_t timeout, uint8_t retryCount) {
  UDPPacket(_address, _port, KEEP_ALIVE).send(this);
}

void PubSubClient::setConnected(bool connected) {
  // if client wasn't connected to server before this moment, connects now and connectCallback is set, fire it
  if (connected && !_connected) {
    if (connectCallback)
      connectCallback();

    _connected = connected;
    UDPPacket(_address, _port, CONNECT).send(this);
  } else if (_connected && !connected) {
    // if client was connected to server before this moment, disconnects now and disconnectCallback is set, fire it
    if (disconnectCallback)
      disconnectCallback();

    _connected = connected;
    // sending disconnect packet is useless, skip it
  }
}

Type PubSubClient::handle(const char *data) {
  int len = (int) strlen(data);
  if (len == 0)
    return ERROR;

  // we've got packet, delay next keep-alive check
  _time = millis();

  Type type = (Type) data[0];

  switch (type) {
    case REPLY:
      return REPLY;
    case PUBLISH:
      if (len < 2)
        return ERROR; // no topic length
      uint8_t topicLength = (uint8_t) data[1];
      if (len < topicLength + 3)
        return ERROR; // no message length
      uint8_t messageLength = (uint8_t) data[topicLength + 2];
      if (len < topicLength + messageLength + 4)
        return ERROR; // no message
      if (callback) {
        char topic[topicLength + 1];
        char message[messageLength + 1];
        strncpy(topic, &data[2], topicLength);
        topic[topicLength] = 0;
        strncpy(message, &data[3 + topicLength], messageLength);
        message[messageLength] = 0;
        callback(topic, message);
      }
      return PUBLISH;
  }
  return ERROR;
}

void PubSubClient::smartDelay(const uint32_t delayTime) {
  uint32_t time = (uint32_t) millis();
  while (abs(millis() - time) < delayTime) {
    this->loop();
  }
}

void PubSubClient::loop() {
  if (abs((int) ((millis() - _time) / 1000)) > _keepAliveTimeout) {
    keepAlive();
    _time = millis();
  }

  while (_client->parsePacket()) {
    char buffer[BUFFER_SIZE];
    int len = _client->read(buffer, BUFFER_SIZE);
    if (len > 0) {
      buffer[len] = 0;
      handle(buffer);
    }
    yield();
    delay(1);
  }
}

WiFiUDP *PubSubClient::getClient() {
  return _client;
}

bool PubSubClient::isConnected() {
  return _connected;
}

PubSubClient &PubSubClient::setDisconnectCallback(DISCONNECT_CALLBACK_SIGNATURE) {
  this->disconnectCallback = disconnectCallback;
  return *this;
}

void PubSubClient::setServer(char *address, uint16_t port) {
  _address.fromString(address);
  _port = port;
}

#include "PubSubClient.h"
#include "UDPPacket.h"

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

void PubSubClient::setServer(char *address, uint16_t port) {
  _address = address;
  _port = port;
}

PubSubClient &PubSubClient::setKeepAliveTimeout(uint8_t timeout) {
  _keepAliveTimeout = timeout;
  return *this;
}

bool PubSubClient::publish(char *topic, char *message, uint16_t timeout, uint8_t retryCount) {
  UDPPacket(_address, _port, PUBLISH)
          .add((char *) (uint8_t) strlen(topic))
          .add(topic)
          .add((char *) (uint8_t) strlen(message))
          .add(message)
          .send(this);

  return isConnected();
}

bool PubSubClient::subscribe(char *topic, uint16_t timeout, uint8_t retryCount) {
  UDPPacket(_address, _port, SUBSCRIBE)
          .add((char *) (uint8_t) strlen(topic))
          .add(topic)
          .send(this);
}

void PubSubClient::keepAlive(uint16_t timeout, uint8_t retryCount) {
  UDPPacket(_address, _port, KEEP_ALIVE).send(this);
}

void PubSubClient::setConnected(bool connected) {
  // if client wasn't connected to server before this moment and connectCallback is set, fire it
  if (connected && !_connected && connectCallback)
    connectCallback();

  _connected = connected;
}

Type PubSubClient::handle(const char *data) {
  int len = (int) strlen(data);
  if (len == 0)
    return ERROR;

  // we've got packet, delay next keep-alive check
  _time = millis();

  Type type = (Type) _buffer[0];
  switch (type) {
    case REPLY:
      return REPLY;
    case PUBLISH:
      if (len < 2)
        return ERROR; // no topic length
      uint8_t topicLength = (uint8_t) _buffer[1];
      if (len < topicLength + 3)
        return ERROR; // no message length
      uint8_t messageLength = (uint8_t) _buffer[topicLength + 2];
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
  if (abs((millis() - _time) / 1000.0) > _keepAliveTimeout) {
    keepAlive();
    _time = millis();
  }

  while (_client->parsePacket()) {
    int len = _client->read(_buffer, BUFFER_SIZE);
    if (len > 0) {
      _buffer[len] = 0;
      if (handle(_buffer)) {
        setConnected(true);
        _buffer[0] = 0;
      }
    }
    yield();
    delay(1);
  }
}

WiFiUDP *PubSubClient::getClient() {
  return _client;
}

char *PubSubClient::getBuffer() {
  return _buffer;
}

bool PubSubClient::isConnected() {
  return _connected;
}

/*
 * QuestClient for ESP8266
 * LLC CERERIS
 */

#include "ESP8266QuestClient.h"

ESP8266QuestClient::ESP8266QuestClient()
{
  _client = NULL;
  setCallback(NULL);
  _keepAliveTimeout = DEFAULT_KEEP_ALIVE_TIMEOUT;
  _time = millis();
  setConnectCallback(NULL);
}

ESP8266QuestClient::ESP8266QuestClient(WiFiUDP& client)
{
  _keepAliveTimeout = DEFAULT_KEEP_ALIVE_TIMEOUT;
  _time = millis();
  setClient(client);
  setCallback(NULL);
  setConnectCallback(NULL);
}

ESP8266QuestClient& ESP8266QuestClient::setCallback(CALLBACK_SIGNATURE)
{
  this->callback = callback;
  return *this;
}

ESP8266QuestClient& ESP8266QuestClient::setConnectCallback(CONNECT_CALLBACK_SIGNATURE)
{
  this->connectCallback = connectCallback;
  return *this;
}

ESP8266QuestClient& ESP8266QuestClient::setClient(WiFiUDP& client)
{
  _client = &client;
  return *this;
}

void ESP8266QuestClient::setServer(const char* address, uint16_t port)
{
  _address = address;
  _port = port;
}

ESP8266QuestClient& ESP8266QuestClient::setKeepAliveTimeout(uint8_t timeout)
{
  _keepAliveTimeout = timeout;
  return *this;
}

bool ESP8266QuestClient::publish(const char* topic, const char* message, uint16_t timeout, uint8_t retryCount)
{
  while(retryCount)
  {
    _client->beginPacket(_address, _port);
    _client->write((uint8_t)PUBLISH_TYPE);
    _client->write((uint8_t)strlen(topic));
    _client->print(topic);
    _client->write((uint8_t)strlen(message));
    _client->println(message);
    _client->endPacket();

    unsigned long timeDelay = millis();
    while(abs(millis() - timeDelay) < timeout)
    {
      if(_client->parsePacket())
      {
        int len = _client->read(_buffer, BUFFER_SIZE);
        if(len > 0)
        {
          _buffer[len] = 0;
          if(handle(_buffer))
          {
            setConnected(true);
            return _connected;
          }
        }
      }
      yield();
      delay(1);
    }

    retryCount--;
  }

  setConnected(false);
  return _connected;
}

bool ESP8266QuestClient::subscribe(const char* topic, uint16_t timeout, uint8_t retryCount)
{
  while(retryCount)
  {
    _client->beginPacket(_address, _port);
    _client->write((uint8_t)SUBSCRIBE_TYPE);
    _client->write((uint8_t)strlen(topic));
    _client->print(topic);
    _client->endPacket();

    unsigned long timeDelay = millis();
    while(abs(millis() - timeDelay) < timeout)
    {
      if(_client->parsePacket())
      {
        int len = _client->read(_buffer, BUFFER_SIZE);
        if(len > 0)
        {
          _buffer[len] = 0;
          if(handle(_buffer))
          {
            return _connected;
          }
        }
      }
      yield();
      delay(1);
    }

    retryCount--;
  }

  return _connected;
}

void ESP8266QuestClient::keepAlive(uint16_t timeout, uint8_t retryCount)
{
  while(retryCount)
  {
    _client->beginPacket(_address, _port);
    _client->write((uint8_t)KEEP_ALIVE_TYPE);
    _client->endPacket();

    unsigned long timeDelay = millis();
    while(abs(millis() - timeDelay) < timeout)
    {
      if(_client->parsePacket())
      {
        int len = _client->read(_buffer, BUFFER_SIZE);
        if(len > 0)
        {
          _buffer[len] = 0;
          if(handle(_buffer))
          {
            setConnected(true);
            return;
          }
        }
      }
      yield();
      delay(1);
    }

    retryCount--;
  }

  setConnected(false);
}

void ESP8266QuestClient::setConnected(bool connected)
{
  if(connected && !_connected)
  {
    if(connectCallback)
    {
      connectCallback();
    }
  }

  _connected = connected;
}

uint8_t ESP8266QuestClient::handle(const char* data)
{
  int len = strlen(data);
  if(len > 0)
  {
    switch(_buffer[0])
    {
      case REPLY_TYPE:
        return REPLY_TYPE;
      case PUBLISH_TYPE:
        if(len > 1)
        {
          uint8_t topicLength = _buffer[1];
          if(len >= topicLength + 2)
          {
            uint8_t messageLength = _buffer[topicLength + 2];
            if(len >= topicLength + messageLength + 3)
            {
              if(callback)
              {
                char topic[topicLength + 1];
                char message[messageLength + 1];
                strncpy(topic, &data[2], topicLength);
                topic[topicLength] = 0;
                strncpy(message, &data[3 + topicLength], messageLength);
                message[messageLength] = 0;
                callback(topic, message);
              }
            }
          }
        }
        return PUBLISH_TYPE;
    }
  }

  return 0;
}

void ESP8266QuestClient::smartDelay(const uint32_t delayTime)
{
  uint32_t time = millis();
  while(abs(millis() - time) < delayTime)
  {
    this->loop();
  }
}

void ESP8266QuestClient::loop()
{
  if(abs((millis() - _time) / 1000.0) > _keepAliveTimeout)
  {
    keepAlive();
    _time = millis();
  }

  while(_client->parsePacket())
  {
    int len = _client->read(_buffer, BUFFER_SIZE);
    if(len > 0)
    {
      _buffer[len] = 0;
      if(handle(_buffer))
      {
        setConnected(true);
        _buffer[0] = 0;
      }
    }
    yield();
    delay(1);
  }
}

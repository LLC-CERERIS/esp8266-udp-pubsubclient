#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "ESP8266QuestClient.h"

const char* ssid = "Cereris";
const char* password = "123581321";
const char* server = "192.168.40.29";
uint16_t port = 33000;

WiFiUDP udp;
ESP8266QuestClient client(udp);

void connectToWiFi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void onHandle(const char* topic, const char* message)
{
  Serial.println("Handle!!!");
  Serial.println(topic);
  Serial.println(message);
  Serial.println(millis());
}

void onConnect()
{
  Serial.println("Connect!!!");
  client.subscribe("very/long/topic");
}

unsigned long sendTime;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("TEST INIT!");

  delay(10);
  randomSeed(micros());
  WiFi.mode(WIFI_STA);

  udp.begin(port);
  client.setServer(server, port);
  client.setCallback(onHandle);
  client.setConnectCallback(onConnect);

  sendTime = millis();
}

void loop()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    connectToWiFi();
  }

//  if(abs((millis() - sendTime)) > 1)
//  {
//    sendTime = millis();
    Serial.println(client.publish("very/long/topic", "message/to/god"));
//  }
  client.loop();
}

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "..";
const char* password = "het3groi";
const char* brokerUser = "antgroup0905";
const char* brokerPassword = "0905168232";
const char* broker = "broker.emqx.io";
const char* outTopic = "/antgroup0905/count";
const char* inTopic1 = "/antgroup0905/ledon";
const char* inTopic2 = "/antgroup0905/ledoff";

WiFiClient espClient;
PubSubClient client(espClient);
long currentTime, lastTime;
int count = 0;
char messages[50];

void setupWifi(){
  delay(100);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  Serial.print("\nConnected to ");
  Serial.println(ssid);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("\nConnecting to ");
    Serial.println(broker);
    if (client.connect("ESP32Client", brokerUser, brokerPassword)) {
      Serial.print("\nConnected to ");
      Serial.println(broker);
      client.subscribe(inTopic1);
      client.subscribe(inTopic2);
    } else {
      Serial.print("\nTrying to connect again");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\nReceived messages: ");
  Serial.println(topic);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  setupWifi();
  client.setServer(broker, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(1000);
  currentTime = millis();
  if (currentTime - lastTime > 5000) {
    count++;
    sprintf(messages, "Count: #%d", count);
    Serial.print("\nSending message: ");
    Serial.println(messages);
    client.publish(outTopic, messages);
    lastTime = millis();
  }
}
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include "Environment.h"

#define SensorPin A0

// wifi
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

// mqtt
const char* mqtt_host = MQTT_HOST;
const int mqtt_port = MQTT_PORT;
const char* sensor_topic = MQTT_TOPIC;
const char* mqtt_username = MQTT_USERNAME;
const char* mqtt_password = MQTT_PASSWORD;

// device identifier
const char* clientID = MQTT_CLIENT_ID;

// polling rate
const int polling_interval_s = POLLING_INTERVAL_S;

WiFiClient wifiClient;
PubSubClient client(mqtt_host, mqtt_port, wifiClient);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // setup wifi
  connectToWiFi();
  connectToMqtt();

  // setup builtin led
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);

  // setup sensor pin
  Serial.setTimeout(2000);
  readSensorAndPublish();
}

void connectToWiFi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to Wifi");
  Serial.println("...");
  WiFi.begin(ssid, password);
  int retries = 0;

  while ((WiFi.status() != WL_CONNECTED) && (retries < 15)) {
    retries++;
    delay(500);
    Serial.print(".");
  }
  if (retries > 14) {
      Serial.println(F("Wifi connection FAILED!"));
  }
  if (WiFi.status() == WL_CONNECTED) {
      Serial.println(F("Wifi connected!"));
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
  }
    Serial.println(F("Setup ready"));
}

void connectToMqtt() {
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

float readSensorAndPublish() {
  float sensorValue = analogRead(SensorPin);
  Serial.println("publishing sensor value: " + String(sensorValue) + "to topic: " + sensor_topic);

  // publish
  client.connect(clientID, mqtt_username, mqtt_password);
  delay(10);
  client.publish(sensor_topic, String(sensorValue).c_str());

  return sensorValue;
}

void run() {
  Serial.println("========================");
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, LOW);

  readSensorAndPublish();

  delay(3000);
  digitalWrite(LED_BUILTIN, HIGH);

  // ESP.deepSleep(polling_interval_s*1e6);
  delay(polling_interval_s*1000);
}

void loop() {
  run();
}

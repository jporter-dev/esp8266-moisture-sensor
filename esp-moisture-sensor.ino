#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include "Environment.h"

#define SensorPin A0

// wifi
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

// mqtt
const char* mqttHost = MQTT_HOST;
const int mqttPort = MQTT_PORT;
const char* mqttUsername = MQTT_USERNAME;
const char* mqttPassword = MQTT_PASSWORD;
const char* clientId = HASS_DEVICE_ID;
// <discovery_prefix>/<component>/<node_id>/
char *sensorTopic;

// polling rate
const int pollingInterval = POLLING_INTERVAL_S;

WiFiClient wifiClient;
PubSubClient client(mqttHost, mqttPort, wifiClient);

void setup() {
  asprintf(&sensorTopic, "%s/%s/%s", HASS_DISCOVERY_PREFIX, "sensor", clientId);

  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  // setup wifi
  connectToWiFi();
  connectToMqtt();

  // setup builtin led
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);

  run();
}

void connectToWiFi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to Wifi");
  Serial.println("...");
  WiFi.begin(ssid, password);
  int retries = 0;

  while ((WiFi.status() != WL_CONNECTED) && (retries < 30)) {
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
  if (client.connect(clientId, mqttUsername, mqttPassword)) {
    Serial.println("Connected to MQTT Broker!");
    configureHassSensor();
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void configureHassSensor() {
  const String hassConfig = "{\"name\":\""+ String(HASS_DEVICE_NAME) +"\",\"state_topic\":\""+ String(sensorTopic) +"/state\",\"unit_of_measurement\":\"?\",\"value_template\":\"{{ value_json.soil_moisture}}\" }";
  Serial.println("Configuring HASS sensor...");
  Serial.println("Publishing to MQTT..." + String(sensorTopic) + "/config");
  Serial.println(hassConfig);

  client.publish((String(sensorTopic) + "/config").c_str(), hassConfig.c_str());
}

boolean publishSensorData(float value) {
  const String sensorData = "{\"soil_moisture\": " + String(value) + "}";
  return client.publish((String(sensorTopic) + "/state").c_str(), sensorData.c_str());
}

float readSensorAndPublish() {
  float sensorValue = analogRead(SensorPin);
  Serial.println("publishing sensor value [" + String(sensorValue) + "] to topic: " + String(sensorTopic) + "/state");

  // publish
  // attempt to publish
  if (publishSensorData(sensorValue)) {
    Serial.println("Moisture sent!");
  }
  else {
    // reconnect and publish on connect failure
    Serial.println("Moisture failed to send. Reconnecting to MQTT Broker and trying again...");
    client.connect(clientId, mqttUsername, mqttPassword);
    delay(10);
    publishSensorData(sensorValue);
  }

  return sensorValue;
}

void run() {
  Serial.println("========================");
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);

  readSensorAndPublish();

  Serial.println("Sleeping for "+ String(pollingInterval*1e6) +" microseconds...");

  delay(5000);
  digitalWrite(LED_BUILTIN, LOW);

  ESP.deepSleep(pollingInterval*1e6);
}

void loop() {}

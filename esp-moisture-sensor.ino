#include "ESP8266WiFi.h"

#define SensorPin A0

// wifi
const char *ssid = "";
const char *password = "";

// mqtt
const char* mqtt_server = "1.1.1.1";
const char* sensor_topic = "moisture";
const char* mqtt_username = "username";
const char* mqtt_password = "password";

// device identifier
const char* clientID = "test_esp_moisture_sensor";

WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // setup wifi
  connectToWiFi();

  // setup builtin led
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);

  // setup sensor pin
  Serial.setTimeout(2000);
  float sensorValue = analogRead(SensorPin);
  Serial.println(sensorValue);
}

void connectToWiFi() {
//Connect to WiFi Network
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

void loop() {
Serial.println("========================");
Serial.println("Loop");
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, LOW);
  delay(2000);

  float sensorValue = analogRead(SensorPin);
  Serial.println(sensorValue);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
}

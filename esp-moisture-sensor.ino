#define SensorPin A0



void setup() {
  delay(10000);
  Serial.begin(115200);
  Serial.println("Start");
  Serial.println("---------------");
  delay(1000);
  Serial.println("Blink Example");
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.setTimeout(2000);
  float sensorValue = analogRead(SensorPin);
  Serial.println(sensorValue);

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
  delay(2000);
}

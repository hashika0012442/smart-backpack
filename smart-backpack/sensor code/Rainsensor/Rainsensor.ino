#define RAIN_PIN A0

void setup() {
  Serial.begin(9600);
  Serial.println("Rain Sensor Test Started");
}

void loop() {
  int rainValue = analogRead(RAIN_PIN);

  Serial.print("Rain Sensor Value: ");
  Serial.println(rainValue);

  delay(1000);
}
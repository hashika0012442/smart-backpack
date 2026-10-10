#define VOLTAGE_PIN A1

void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(VOLTAGE_PIN);

  // UNO R4 WiFi default ADC reading
  float pinVoltage = sensorValue * (5.0 / 1023.0);

  // 0-25V sensor uses approx. 5:1 voltage divider
  float inputVoltage = pinVoltage * 5.0;

  Serial.print("ADC: ");
  Serial.print(sensorValue);

  Serial.print(" | Voltage: ");
  Serial.print(inputVoltage, 2);
  Serial.println(" V");

  delay(1000);
}
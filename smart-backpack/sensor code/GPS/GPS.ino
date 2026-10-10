#define VIBRATION_PIN 2

void setup() {
  Serial.begin(9600);
  pinMode(VIBRATION_PIN, INPUT);

  Serial.println("SW-420 Vibration Test Started");
}

void loop() {
  int vibration = digitalRead(VIBRATION_PIN);

  if (vibration == HIGH) {
    Serial.println("Vibration Detected!");
  } else {
    Serial.println("No Vibration");
  }

  delay(300);
}
#define BUZZER_PIN 6

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);

  Serial.println("Buzzer Test Started");
}

void loop() {
  digitalWrite(BUZZER_PIN, HIGH);
  Serial.println("Buzzer ON");
  delay(1000);

  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("Buzzer OFF");
  delay(1000);
}
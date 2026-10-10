#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522.h>
#include <TinyGPSPlus.h>

// ---------- PINS ----------
#define VIBRATION_PIN 2
#define REED_PIN      3
#define TRIG_PIN      4
#define ECHO_PIN      5
#define BUZZER_PIN    6
#define DHT_PIN       7
#define LED_PIN       8

#define RFID_RST      9
#define RFID_SS       10

#define RAIN_PIN      A0
#define VOLTAGE_PIN   A1

// ---------- DHT ----------
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- RFID ----------
MFRC522 rfid(RFID_SS, RFID_RST);

// ---------- GPS ----------
TinyGPSPlus gps;

// ---------- SETUP ----------
void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(VIBRATION_PIN, INPUT);
  pinMode(REED_PIN, INPUT_PULLUP);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // DHT
  dht.begin();

  // OLED
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED FAILED!");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("SMART BACKPACK");
    display.println("Starting...");
    display.display();
  }

  // RFID
  SPI.begin();
  rfid.PCD_Init();

  Serial.println("==========================");
  Serial.println("SMART BACKPACK SYSTEM");
  Serial.println("Integration Test Started");
  Serial.println("==========================");

  delay(2000);
}

// ---------- LOOP ----------
void loop() {

  // GPS DATA
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }

  // =========================
  // DHT22
  // =========================

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // =========================
  // VIBRATION
  // =========================

  int vibration = digitalRead(VIBRATION_PIN);

  // =========================
  // REED SWITCH
  // =========================

  int reed = digitalRead(REED_PIN);

  // =========================
  // RAIN SENSOR
  // =========================

  int rainValue = analogRead(RAIN_PIN);

  // =========================
  // VOLTAGE SENSOR
  // =========================

  int voltageADC = analogRead(VOLTAGE_PIN);

  float pinVoltage =
      voltageADC * (5.0 / 1023.0);

  float inputVoltage =
      pinVoltage * 5.0;

  // =========================
  // ULTRASONIC
  // =========================

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration =
      pulseIn(ECHO_PIN, HIGH, 30000);

  float distance = 0;

  if (duration > 0) {
    distance =
        duration * 0.0343 / 2;
  }

  // =========================
  // SERIAL MONITOR
  // =========================

  Serial.println();
  Serial.println("----- SENSOR DATA -----");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Rain: ");
  Serial.println(rainValue);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Voltage: ");
  Serial.print(inputVoltage, 2);
  Serial.println(" V");

  // Vibration
  Serial.print("Vibration: ");

  if (vibration == HIGH) {
    Serial.println("DETECTED");
  } else {
    Serial.println("Normal");
  }

  // Reed
  Serial.print("Bag: ");

  if (reed == LOW) {
    Serial.println("CLOSED");
  } else {
    Serial.println("OPEN");
  }

  // =========================
  // GPS
  // =========================

  Serial.print("GPS: ");

  if (gps.location.isValid()) {

    Serial.print(
      gps.location.lat(), 6);

    Serial.print(", ");

    Serial.println(
      gps.location.lng(), 6);

  } else {

    Serial.print("Waiting for fix");

    Serial.print(" | Satellites: ");

    if (gps.satellites.isValid()) {
      Serial.println(
        gps.satellites.value());
    } else {
      Serial.println(0);
    }
  }

  // =========================
  // RFID
  // =========================

  if (rfid.PICC_IsNewCardPresent() &&
      rfid.PICC_ReadCardSerial()) {

    Serial.print("RFID UID: ");

    for (byte i = 0;
         i < rfid.uid.size;
         i++) {

      if (rfid.uid.uidByte[i] < 0x10)
        Serial.print("0");

      Serial.print(
        rfid.uid.uidByte[i],
        HEX);

      if (i <
          rfid.uid.size - 1)
        Serial.print(" ");
    }

    Serial.println();

    // Short confirmation alert
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);

    delay(200);

    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  // =========================
  // OLED DISPLAY
  // =========================

  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("SMART BACKPACK");

  display.print("Temp: ");
  display.print(temperature, 1);
  display.println(" C");

  display.print("Hum : ");
  display.print(humidity, 0);
  display.println(" %");

  display.print("Rain: ");
  display.println(rainValue);

  display.print("Bag : ");

  if (reed == LOW)
    display.println("CLOSED");
  else
    display.println("OPEN");

  display.print("Dist: ");
  display.print(distance, 0);
  display.println("cm");

  display.display();

  delay(1000);
}
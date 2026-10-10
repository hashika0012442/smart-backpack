#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <Wire.h>
#include <SPI.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522.h>
#include <TinyGPSPlus.h>

// ==========================================
// WIFI
// ==========================================

char ssid[] = "HP’s iPhone";
char pass[] = "0773989347";

// ==========================================
// FIREBASE
// ==========================================

char firebaseHost[] =
  "smart-backpack-iot-default-rtdb.firebaseio.com";

WiFiSSLClient wifi;
HttpClient client(wifi, firebaseHost, 443);

// ==========================================
// PINS
// ==========================================

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

// ==========================================
// DHT22
// ==========================================

#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

// ==========================================
// OLED
// ==========================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ==========================================
// RFID
// ==========================================

MFRC522 rfid(RFID_SS, RFID_RST);

String lastRFID = "None";
String rfidStatus = "NO CARD";

// Authorized RFID cards
String authorizedUID1 = "FD 05 4C 06";
String authorizedUID2 = "B7 BD F3 06";

// ==========================================
// GPS
// ==========================================

TinyGPSPlus gps;

// ==========================================
// ALERT FUNCTION
// ==========================================

void alarmON() {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
}

void alarmOFF() {
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void confirmationBeep() {

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);

  delay(200);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  delay(100);
}

// ==========================================
// SETUP
// ==========================================

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(VIBRATION_PIN, INPUT);
  pinMode(REED_PIN, INPUT_PULLUP);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  alarmOFF();

  // DHT
  dht.begin();

  // OLED
  Wire.begin();

  if (
    display.begin(
      SSD1306_SWITCHCAPVCC,
      0x3C
    )
  ) {

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(
      SSD1306_WHITE
    );

    display.setCursor(0, 0);

    display.println(
      "SMART BACKPACK"
    );

    display.println(
      "Starting..."
    );

    display.display();
  }

  // RFID
  SPI.begin();
  rfid.PCD_Init();

  // WIFI
  Serial.println(
    "Connecting to WiFi..."
  );

  while (
    WiFi.begin(ssid, pass)
    != WL_CONNECTED
  ) {

    Serial.print(".");
    delay(3000);
  }

  Serial.println();

  Serial.println(
    "WiFi Connected!"
  );

  Serial.print(
    "IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );

  display.clearDisplay();
  display.setCursor(0, 0);

  display.println(
    "SMART BACKPACK"
  );

  display.println(
    "WiFi Connected"
  );

  display.println(
    "Firebase Ready"
  );

  display.display();

  delay(2000);
}

// ==========================================
// LOOP
// ==========================================

void loop() {

  // ========================================
  // GPS
  // ========================================

  while (Serial1.available()) {
    gps.encode(
      Serial1.read()
    );
  }

  // ========================================
  // DHT22
  // ========================================

  float temperature =
    dht.readTemperature();

  float humidity =
    dht.readHumidity();

  // ========================================
  // RAIN
  // ========================================

  int rainValue =
    analogRead(RAIN_PIN);

  String rainStatus;

  if (rainValue < 800) {
    rainStatus = "RAIN";
  } else {
    rainStatus = "DRY";
  }

  // ========================================
  // VIBRATION
  // ========================================

  int vibrationValue =
    digitalRead(VIBRATION_PIN);

  String vibrationStatus;

  if (vibrationValue == HIGH) {

    vibrationStatus =
      "DETECTED";

  } else {

    vibrationStatus =
      "NORMAL";
  }

  // ========================================
  // REED SWITCH
  // ========================================

  int reedValue =
    digitalRead(REED_PIN);

  String bagStatus;

  if (reedValue == LOW) {

    bagStatus = "CLOSED";

  } else {

    bagStatus = "OPEN";
  }

  // ========================================
  // ULTRASONIC
  // ========================================

  digitalWrite(
    TRIG_PIN,
    LOW
  );

  delayMicroseconds(2);

  digitalWrite(
    TRIG_PIN,
    HIGH
  );

  delayMicroseconds(10);

  digitalWrite(
    TRIG_PIN,
    LOW
  );

  long duration =
    pulseIn(
      ECHO_PIN,
      HIGH,
      30000
    );

  float distance = 0;

  if (duration > 0) {

    distance =
      duration *
      0.0343 /
      2;
  }

  // ========================================
  // VOLTAGE SENSOR
  // ========================================

  int voltageADC =
    analogRead(
      VOLTAGE_PIN
    );

  float pinVoltage =
    voltageADC *
    (5.0 / 1023.0);

  float inputVoltage =
    pinVoltage * 5.0;

  // ========================================
  // RFID
  // ========================================

  if (
    rfid.PICC_IsNewCardPresent() &&
    rfid.PICC_ReadCardSerial()
  ) {

    lastRFID = "";

    for (
      byte i = 0;
      i < rfid.uid.size;
      i++
    ) {

      if (
        rfid.uid.uidByte[i]
        < 0x10
      ) {

        lastRFID += "0";
      }

      lastRFID +=
        String(
          rfid.uid.uidByte[i],
          HEX
        );

      if (
        i <
        rfid.uid.size - 1
      ) {

        lastRFID += " ";
      }
    }

    lastRFID.toUpperCase();

    Serial.print(
      "RFID UID: "
    );

    Serial.println(
      lastRFID
    );

    // Check authorized card

    if (
      lastRFID ==
        authorizedUID1 ||
      lastRFID ==
        authorizedUID2
    ) {

      rfidStatus =
        "AUTHORIZED";

      Serial.println(
        "RFID AUTHORIZED"
      );

      confirmationBeep();

    } else {

      rfidStatus =
        "UNAUTHORIZED";

      Serial.println(
        "RFID UNAUTHORIZED"
      );
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  // ========================================
  // SECURITY ALARM
  // ========================================

  bool securityAlert = false;

  // Bag opened
  if (bagStatus == "OPEN") {
    securityAlert = true;
  }

  // Vibration detected
  if (
    vibrationStatus ==
    "DETECTED"
  ) {

    securityAlert = true;
  }

  // Unauthorized RFID
  if (
    rfidStatus ==
    "UNAUTHORIZED"
  ) {

    securityAlert = true;
  }

  // ========================================
  // LED + BUZZER
  // ========================================

  if (securityAlert) {

    alarmON();

  } else {

    alarmOFF();
  }

  // ========================================
  // GPS VALUES
  // ========================================

  double latitude = 0;
  double longitude = 0;

  int satellites = 0;

  if (
    gps.location.isValid()
  ) {

    latitude =
      gps.location.lat();

    longitude =
      gps.location.lng();
  }

  if (
    gps.satellites.isValid()
  ) {

    satellites =
      gps.satellites.value();
  }

  // ========================================
  // DHT CHECK
  // ========================================

  if (
    isnan(temperature) ||
    isnan(humidity)
  ) {

    Serial.println(
      "DHT22 Read Failed!"
    );

    delay(2000);

    return;
  }

  // ========================================
  // CREATE FIREBASE JSON
  // ========================================

  String jsonData = "{";

  jsonData +=
    "\"temperature\":" +
    String(
      temperature,
      1
    ) + ",";

  jsonData +=
    "\"humidity\":" +
    String(
      humidity,
      1
    ) + ",";

  jsonData +=
    "\"rainValue\":" +
    String(
      rainValue
    ) + ",";

  jsonData +=
    "\"rainStatus\":\"" +
    rainStatus +
    "\",";

  jsonData +=
    "\"distance\":" +
    String(
      distance,
      1
    ) + ",";

  jsonData +=
    "\"voltage\":" +
    String(
      inputVoltage,
      2
    ) + ",";

  jsonData +=
    "\"vibration\":\"" +
    vibrationStatus +
    "\",";

  jsonData +=
    "\"bagStatus\":\"" +
    bagStatus +
    "\",";

  jsonData +=
    "\"rfid\":\"" +
    lastRFID +
    "\",";

  jsonData +=
    "\"rfidStatus\":\"" +
    rfidStatus +
    "\",";

  jsonData +=
    "\"securityAlert\":" +
    String(
      securityAlert
        ? "true"
        : "false"
    ) + ",";

  jsonData +=
    "\"latitude\":" +
    String(
      latitude,
      6
    ) + ",";

  jsonData +=
    "\"longitude\":" +
    String(
      longitude,
      6
    ) + ",";

  jsonData +=
    "\"satellites\":" +
    String(
      satellites
    );

  jsonData += "}";

  // ========================================
  // SERIAL MONITOR
  // ========================================

  Serial.println();

  Serial.println(
    "===== SMART BACKPACK ====="
  );

  Serial.print(
    "Temperature: "
  );

  Serial.print(
    temperature
  );

  Serial.println(" C");

  Serial.print(
    "Humidity: "
  );

  Serial.print(
    humidity
  );

  Serial.println(" %");

  Serial.print(
    "Rain: "
  );

  Serial.print(
    rainValue
  );

  Serial.print(" - ");

  Serial.println(
    rainStatus
  );

  Serial.print(
    "Distance: "
  );

  Serial.print(
    distance
  );

  Serial.println(" cm");

  Serial.print(
    "Voltage: "
  );

  Serial.print(
    inputVoltage
  );

  Serial.println(" V");

  Serial.print(
    "Vibration: "
  );

  Serial.println(
    vibrationStatus
  );

  Serial.print(
    "Bag: "
  );

  Serial.println(
    bagStatus
  );

  Serial.print(
    "RFID: "
  );

  Serial.println(
    lastRFID
  );

  Serial.print(
    "RFID Status: "
  );

  Serial.println(
    rfidStatus
  );

  Serial.print(
    "Security: "
  );

  if (securityAlert) {

    Serial.println(
      "ALERT"
    );

  } else {

    Serial.println(
      "SAFE"
    );
  }

  Serial.print(
    "GPS: "
  );

  if (
    gps.location.isValid()
  ) {

    Serial.print(
      latitude,
      6
    );

    Serial.print(", ");

    Serial.println(
      longitude,
      6
    );

  } else {

    Serial.print(
      "Waiting for fix | Satellites: "
    );

    Serial.println(
      satellites
    );
  }

  // ========================================
  // FIREBASE
  // ========================================

  Serial.println(
    "Sending to Firebase..."
  );

  client.beginRequest();

  client.put(
    "/sensors.json"
  );

  client.sendHeader(
    "Content-Type",
    "application/json"
  );

  client.sendHeader(
    "Content-Length",
    jsonData.length()
  );

  client.beginBody();

  client.print(
    jsonData
  );

  client.endRequest();

  int statusCode =
    client.responseStatusCode();

  String response =
    client.responseBody();

  Serial.print(
    "Firebase Status: "
  );

  Serial.println(
    statusCode
  );

  if (
    statusCode != 200
  ) {

    Serial.print(
      "Firebase Response: "
    );

    Serial.println(
      response
    );
  }

  // ========================================
  // OLED
  // ========================================

  display.clearDisplay();

  display.setCursor(
    0,
    0
  );

  display.println(
    "SMART BACKPACK"
  );

  display.print("T:");
  display.print(
    temperature,
    1
  );

  display.print("C H:");

  display.print(
    humidity,
    0
  );

  display.println("%");

  display.print(
    "Bag:"
  );

  display.println(
    bagStatus
  );

  display.print(
    "Rain:"
  );

  display.println(
    rainStatus
  );

  display.print(
    "Security:"
  );

  if (securityAlert) {

    display.println(
      "ALERT!"
    );

  } else {

    display.println(
      "SAFE"
    );
  }

  display.print(
    "GPS Sat:"
  );

  display.println(
    satellites
  );

  display.display();

  delay(3000);
}
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <DHT.h>

// ---------- WiFi ----------
char ssid[] = "HP’s iPhone";
char pass[] = "0773989347";

// ---------- Firebase ----------
char firebaseHost[] =
  "smart-backpack-iot-default-rtdb.firebaseio.com";

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, firebaseHost, 443);

// ---------- DHT22 ----------
#define DHTPIN 7
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(9600);
  delay(2000);

  dht.begin();

  Serial.println("Connecting to WiFi...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(3000);
  }

  Serial.println();
  Serial.println("WiFi Connected!");
}

// ======================================

void loop() {

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT22 reading failed!");
    delay(3000);
    return;
  }

  // JSON data
  String jsonData = "{";
  jsonData += "\"temperature\":";
  jsonData += String(temperature, 1);
  jsonData += ",";
  jsonData += "\"humidity\":";
  jsonData += String(humidity, 1);
  jsonData += "}";

  Serial.println();
  Serial.println("Sending to Firebase...");
  Serial.println(jsonData);

  // Send to:
  // /sensors.json

  client.beginRequest();
  client.put("/sensors.json");
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", jsonData.length());
  client.beginBody();
  client.print(jsonData);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Firebase Status Code: ");
  Serial.println(statusCode);

  Serial.print("Firebase Response: ");
  Serial.println(response);

  delay(5000);
}
#include <WiFiS3.h>

char ssid[] = "HP’s iPhone";
char pass[] = "0773989347";

void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial.println("Connecting to iPhone Hotspot...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(3000);
  }

  Serial.println();
  Serial.println("WiFi Connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Signal Strength: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

void loop() {
}
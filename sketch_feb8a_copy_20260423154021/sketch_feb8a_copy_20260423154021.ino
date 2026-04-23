#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SimpleDHT.h>

// 🔐 WiFi
#define WIFI_SSID "naughty"
#define WIFI_PASSWORD "12345678"

// 🔥 Firebase host
#define FIREBASE_HOST "soilmoisturedata-44dd2-default-rtdb.asia-southeast1.firebasedatabase.app"

// 🌱 Calibrate these after testing RAW values
#define MOISTURE_WET_VALUE 800
#define MOISTURE_DRY_VALUE 3500

// 🌡️ Sensors
const int soilPin = 32;
int pinDHT11 = 4;
SimpleDHT11 dht11(pinDHT11);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("🚀 ESP32 START");

  // 🌐 WiFi connect
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  // 🌱 READ SOIL SENSOR
  int raw = analogRead(soilPin);

  Serial.print("RAW Moisture: ");
  Serial.println(raw);

  int moisture = map(raw, MOISTURE_WET_VALUE, MOISTURE_DRY_VALUE, 100, 0);
  moisture = constrain(moisture, 0, 100);

  Serial.print("Moisture %: ");
  Serial.println(moisture);

  // 🌡️ READ DHT11
  byte temp = 0, hum = 0;

  if (dht11.read(&temp, &hum, NULL) != SimpleDHTErrSuccess) {
    Serial.println("❌ DHT read failed");
    delay(2000);
    return;
  }

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("°C  Humidity: ");
  Serial.println(hum);

  // 📡 SEND TO FIREBASE
  if (WiFi.status() == WL_CONNECTED) {

    WiFiClientSecure client;
    client.setInsecure();   // bypass SSL cert

    HTTPClient http;

    String url = "https://" + String(FIREBASE_HOST) + "/sensor.json";

    String json = "{";
    json += "\"moisture\":" + String(moisture) + ",";
    json += "\"temperature\":" + String(temp) + ",";
    json += "\"humidity\":" + String(hum);
    json += "}";

    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");

    int code = http.PUT(json);

    if (code > 0) {
      Serial.print("✅ Firebase OK, code: ");
      Serial.println(code);
    } else {
      Serial.print("❌ Firebase Error: ");
      Serial.println(code);
    }

    http.end();
  }

  Serial.println("----------------------");

  delay(5000);
}
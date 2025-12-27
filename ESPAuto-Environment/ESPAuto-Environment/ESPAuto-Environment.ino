// ESPAuto-Environment/ESPAuto-Environment.ino

#include <WiFi.h>
#include <WebServer.h>
#include "SensorManager.h"
#include "IOManager.h"
#include "Logger.h"
#include "Automation.h"
#include <FS.h>
#include <SPIFFS.h>

// put your wifi name and password here
const char* ssid = "yourWifiName";
const char* password = "yourPassword";

WebServer server(80);

void handleRoot() {
  String html = Logger::getHTML();
  server.send(200, "text/html", html);
}

void handleData() {
  String json = Logger::getJSON();
  server.send(200, "application/json", json);
}

void handleCSV() {
  if(!SPIFFS.exists("/data.csv")) {
    server.send(404, "text/plain", "no csv");
    return;
  }
  File f = SPIFFS.open("/data.csv", "r");
  size_t sz = f.size();
  server.streamFile(f, "text/csv");
  f.close();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // init SPIFFS
  if(!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
  }

  // initializers
  SensorManager::init();
  IOManager::init();
  Logger::init();
  Automation::init();

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print('.');
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("WiFi connected: ");
    Serial.println(WiFi.localIP());
  } else { // fallback
    Serial.println("\nWiFi failed, starting AP mode...");
    WiFi.softAP("ESP32_Logger", "12345678");
    Serial.print("AP IP: "); 
    Serial.println(WiFi.softAPIP());
  }

  // web sevrers
  server.on("/", handleRoot);
  server.on("/dashboard.html", handleRoot);
  server.on("/data", handleData);
  server.on("/csv", handleCSV);
  server.begin();

  Serial.println("Server started");
}

unsigned long lastLoop = 0;
const unsigned long LOOP_INTERVAL = 1000;

void loop() {
  server.handleClient();

  unsigned long now = millis();
  if (now - lastLoop >= LOOP_INTERVAL) {
    lastLoop = now;

    // read sensors
    float t = SensorManager::readTemp();
    float h = SensorManager::readHumidity();
    int light = SensorManager::readLight();
    int motion = SensorManager::readMotion();

    // automation (may toggle outputs)
    Automation::evaluate(t, h, light, motion);

    // write
    Logger::append(t, h, light, motion);

    // debug
    Serial.printf("T: %.2f H: %.2f L:%d M:%d\n", t, h, light, motion);
  }
}
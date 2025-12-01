// esp32_client.ino
// Sends Json data to the flask server via HTTP POST request

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// put your wifi name, password, and server url here
const char* ssid = "yourWifiName";
const char* password = "yourPassword";
const char* serverUrl = "http://yourServerUrl/data";

//put your dht pin and model/button pins
DHT dht(4, DHT11);
int butPin = 18;

int butOld, butNew;
bool armed = false;
unsigned long lastLog = 0;
const int logInterval = 1000;
bool lastArmed = false;
HTTPClient http;

volatile bool modeChanged = false;

void IRAM_ATTR toggleMode() {
  armed = !armed;
  modeChanged = true;
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    pinMode(butPin, INPUT_PULLUP);
    digitalWrite(butPin, HIGH);
    butOld = digitalRead(butPin);

    attachInterrupt(digitalPinToInterrupt(butPin), toggleMode, FALLING);
    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    Serial.println(armed? "ARMED" : "DISARMED");
}

void loop() {
    if (modeChanged) modeChanged = false;
    unsigned long now = millis();

    if (WiFi.status() == WL_CONNECTED) {
        if (armed) {
            if (!lastArmed) Serial.println("ARMED, LOGGING");
            lastArmed = true;

            float temp = dht.readTemperature();
            float humidity = dht.readHumidity();
            int uptime = millis() / 1000;

            if (isnan(temp) || isnan(humidity)) {
                Serial.println("Invalid DHT reading, skipped.");
                return;
            }

            if (now - lastLog >= logInterval) {
                lastLog = now;

                StaticJsonDocument<200> doc;
                doc["device_id"] = "esp32_01";
                doc["temp_c"] = temp;
                doc["hum_pct"] = humidity;
                doc["uptime_s"] = uptime;
                doc["status"] = armed ? "ON" : "OFF";

                String payload;
                serializeJson(doc, payload);

                http.begin(serverUrl);
                http.addHeader("Content-Type", "application/json");
                int httpResponseCode = http.POST(payload);

                if (httpResponseCode > 0) {
                    Serial.printf("Posted: %s -> %d\n", payload.c_str(), httpResponseCode);
                } else {
                    Serial.printf("Error on sending POST: %d\n", httpResponseCode);
                }
                http.end();
            }
        } else {
            if (lastArmed) Serial.println("NOT ARMED, NOT LOGGING");
            lastArmed = false;
        }
    } else {
        Serial.println("WiFi not connected, retrying..");
        WiFi.begin(ssid, password);
    }

    delay(100);
}

// ESPMain/ESPMain.ino
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

//put your wifi name and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";
const char* serverUrl = "http://yourServer/data";

//put your sensor pins
DHT dht(4, DHT11);
int butPin = 18;
int ledPin = 2;

bool armed = false;
bool lastArmed = false;
volatile bool modeChanged = false;
HTTPClient http;

void IRAM_ATTR toggleMode() {
    armed = !armed;
    modeChanged = true;
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    pinMode(butPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    attachInterrupt(digitalPinToInterrupt(butPin), toggleMode, FALLING);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.println(armed ? "ARMED" : "DISARMED");
}

void loop() {
    if (modeChanged) modeChanged = false;
    if (WiFi.status() == WL_CONNECTED) {
        if (armed) {
            if (!lastArmed) Serial.println("ARMED, LOGGING");
            lastArmed = true;
            float temp = dht.readTemperature();
            float hum = dht.readHumidity();
            int uptime = millis() / 1000;
            String stateStr = armed ? "true" : "false";
            if (isnan(temp) || isnan(hum)) {
                Serial.println("Invalid DHT reading, skipped.");
                return;
            }
            String jsonPayload = "{\"temperature\":" + String(temp) + ",\"humidity\":" + String(hum) + ",\"uptime\":" + String(uptime) + ",\"state\":" + stateStr + "}";
            http.begin(serverUrl);
            http.addHeader("Content-Type", "application/json");
            int httpResponseCode = http.POST(jsonPayload);
            if (httpResponseCode > 0) {
                String response = http.getString();
                Serial.print("Server Response: ");
                Serial.println(response);
                if (response.indexOf("predict") != -1 || response.indexOf("Predicted") != -1) {
                    digitalWrite(ledPin, HIGH);
                    Serial.println("Predicted");
                } else {
                    digitalWrite(ledPin, LOW);
                    Serial.println("Not Predicted");
                }
            } else {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }
            http.end();
        } else {
            if (lastArmed) Serial.println("NOT ARMED, NOT LOGGING");
            lastArmed = false;
            digitalWrite(ledPin, LOW);
            String jsonPayload = "{\"state\":false}";
            http.begin(serverUrl);
            http.addHeader("Content-Type", "application/json");
            int httpResponseCode = http.POST(jsonPayload);
            if (httpResponseCode > 0) {
                String response = http.getString();
                Serial.print("Server Response (Disarm): ");
                Serial.println(response);
            } else {
                Serial.print("Error code (Disarm): ");
                Serial.println(httpResponseCode);
            }
            http.end();
        }
        delay(1000);
    } else {
        Serial.println("WiFi not connected, retrying..");
        WiFi.begin(ssid, password);
    }
}
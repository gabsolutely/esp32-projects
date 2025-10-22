  // this was pretty tiring to do lol
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//put your wifi name and password
const char* ssid = "yourWifiName";
const char* password = "youurPassword";
const char* apiKey = "yourAPIKey"; // from OpenWeatherMap api keys
const char* city = "Manila"; // put your city, this is just an example
const char* countryCode = "PH"; // put your country code ^^
//put your button pin
//OLED PINS -- SDA=21, SCL= 22
int butPin = 18;
int butOld;
int butNew;

bool mode = 0;
unsigned long lastUpdate = 0;
String mainWeather;
String weatherDesc;
float tempC = 0.0;

void connectWifi () {
   WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void getTime () {
  configTime(8*3600, 0, "pool.ntp.org", "time.nist.gov"); // please set your GMT (mine is +8 so)
  
  Serial.println("Syncing time using NTP..");
  delay(2000);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
   Serial.println("Failed to obtain time");
   return;
  }
  Serial.println("Time synced");
}

void getWeather () {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "," + String(countryCode) + "&appid=" + String(apiKey) + "&units=metric";
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == 200) {
   String payload = http.getString();
   DynamicJsonDocument doc(1024);
   deserializeJson(doc, payload);

   tempC = doc["main"]["temp"];
   mainWeather = doc ["weather"][0]["main"].as<String>();
   weatherDesc = doc ["weather"][0]["description"].as<String>();

   Serial.println ("Updated weather! " + mainWeather + "(" + String(tempC) +"C)");
  }
  else {Serial.println("Failed to connect to weather data. Code: " + String (httpCode));}
}

void showWeather () {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("Weather in " + String(city));
  display.setTextSize(2);

  display.setCursor(0, 20);
  display.print(tempC, 1);
  display.println(" C");
  display.setTextSize(1);

  display.setCursor(0, 45);
  display.println(mainWeather);
  display.setCursor(0, 55);
  display.println(weatherDesc);
  display.display();
}

void showClock () {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  char timeStr[9];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println(timeStr);
  display.display();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(butPin, INPUT_PULLUP);
  digitalWrite(butPin, HIGH);
  butOld = digitalRead(butPin);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
   Serial.println("OLED initialization failed!");
   for (;;);
 }

 display.clearDisplay();
 display.display();

 connectWifi();
 getTime();
 getWeather();
 showClock();
}

void loop() {
  // put your main code here, to run repeatedly:

  butNew = digitalRead(butPin);
  if (butOld == 0 && butNew == 1) {
   mode = !mode;
  }
  butOld = butNew;

// get weather every 10 minutes
  if (millis() - lastUpdate > 600000) {
    getWeather();
    lastUpdate = millis();
  }

  if (mode){showClock();}
  else {showWeather();}
}

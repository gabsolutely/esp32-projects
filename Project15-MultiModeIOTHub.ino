#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define OLED_RESET -1     

Adafruit_SSD1306 display (SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);

//put your wifi name and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";
const char* apiKey = "yourAPIKey"; // from OpenWeatherMap api keys
const char* city = "Manila"; // put your city, this is just an example
const char* countryCode = "PH"; // put your country code ^^

//put your button/dht pin
//OLED PINS -- SDA=21, SCL= 22
DHT dht(15, DHT11);
int butPin = 18;
int butOld;
int butNew;

int mode = 1;
unsigned long lastUpdate = 0;
String mainWeather;
String weatherDesc;
float tempC = 0.0;
String message = "Hello World!";

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

void sendHtml() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<title>ESP32 IOT Terminal</title>";
  html += "<style>";
  html += "body{font-family:Arial;text-align:center;margin-top:40px;}";
  html += "input,button{padding:10px;margin:5px;font-size:16px;}";
  html += "div{margin:10px;font-size:18px;}";
  html += "</style></head><body>";

  html += "<h2>Multi-Mode IOT Terminal</h2>";

  html += "<div>Temperature: <span id='t'>--</span> &deg;C</div>";
  html += "<div>Humidity: <span id='h'>--</span> %</div>";
  html += "<div>Mode: <b id='mode'>--</b></div>";

  html += "<p><a href='/refresh'><button>REFRESH WEATHER</button></a>";
  html += "<a href='/next'><button>NEXT MODE</button></a></p>";

  html += "<h3>Send a Message to the OLED</h3>";
  html += "<form action='/display' method='get'>";
  html += "<input type='text' name='msg' placeholder='Enter Message' style='width:60%;'>";
  html += "<input type='submit' value='Send'>";
  html += "</form>";
  html += "<p>Current Message: <b>" + String(message) + "</b></p>";

  html += "<script>";
  html += "async function update(){";
  html += "  try{";
  html += "    const r = await fetch('/data');";
  html += "    const j = await r.json();";
  html += "    document.getElementById('t').textContent = j.temp;";
  html += "    document.getElementById('h').textContent = j.hum;";
  html += "    document.getElementById('mode').textContent = j.mode;";
  html += "  }catch(e){ console.log('error',e); }";
  html += "}";
  html += "update();";
  html += "setInterval(update,500);";
  html += "</script>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

// send data without refreshing using a bit of java script 
void sendData() {
  float t = dht.readTemperature(); // write true inside parenthesis and change signs for fahrenheit
  float h = dht.readHumidity();
  //isnan = no reading (null)
  if (isnan(t) || isnan(h)) {
    Serial.println ("Failed to read from DHT sensor :(");
    t = 0;
    h = 0;
  }
  String json = "{";
  json += "\"temp\":" + String(t,1) + ",";
  json += "\"hum\":"  + String(h,1) + ",";
  json += "\"mode\":" + String(mode);
  json += "}";
  server.send(200, "application/json", json);
}

void getTime () {
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov"); //adjust for your timezone, mine is +8 so
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

void getMessage() {
  if (server.hasArg("msg")) {
    message = server.arg("msg");
    Serial.print("New Message: ");
    Serial.println(message);
  }
  server.sendHeader("Location", "/");
  server.send(303);
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
  char dateStr[11];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
  strftime(dateStr, sizeof(dateStr), "%m-%d-%Y", &timeinfo);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(25, 10);
  display.println(dateStr);

  display.setTextSize(2);
  display.setCursor(10, 30);
  display.println(timeStr);
  display.display();
}

void showMessage () {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 25);
  display.println(message);
  display.display();
}

void showDHT () {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 10);
  display.setTextSize(2);
  display.print(t, 1);
  display.println(" C");

  display.setCursor(0, 40);
  display.setTextSize(2);
  display.print(h, 1);
  display.println(" %");
  display.display();
}

void ifNext () {
  mode++;
  if (mode>4) {mode = 1;}
  server.sendHeader("Location", "/");
  server.send(303);
}

void ifRefresh () {
  getWeather();
  lastUpdate = millis();
  server.sendHeader("Location", "/");
  server.send(303);  
}

void ifButton () {
    butNew = digitalRead(butPin);
  if (butOld == 0 && butNew == 1) {
   mode++;
   if (mode>4) {mode = 1;}
  }
  butOld = butNew;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();

  pinMode (butPin, INPUT_PULLUP);
  digitalWrite (butPin, HIGH);
  butOld = digitalRead (butPin);

 if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  Serial.println("OLED initialization failed!");
  for (;;);
 }

 connectWifi();
  // webserver routes
  server.on("/", sendHtml);
  server.on("/data", sendData);
  server.on("/next", ifNext);
  server.on("/refresh", ifRefresh);
  server.on("/display", getMessage);
  server.begin();

 getTime();
 getWeather();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  ifButton();

  switch (mode) {
    case 1: showClock(); break;
    case 2: showWeather(); break;
    case 3: showMessage(); break;
    case 4: showDHT(); break;
  }
}

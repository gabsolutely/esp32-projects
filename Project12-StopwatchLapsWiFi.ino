#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define OLED_RESET -1
#define maxLaps 20

Adafruit_SSD1306 display (SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WebServer server(80);
//put wifi name and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";

//change according to pins
//OLED PINS -- SDA=21, SCL= 22
int butPin = 19;
int butNew;
int butOld;
int butPin2 = 18;
int butNew2;
int butOld2;
int butPin3 = 5;
int butNew3;
int butOld3;

bool state = false;
unsigned long resetTime = 0;
unsigned long elapsedTime = 0;
unsigned long pauseTime = 0;
unsigned long laps[maxLaps];
int lapCount = 0;

String sendTime (unsigned long ms) {
  if (ms == 0) return "Never";
  unsigned long sec = (ms/1000)%60;
  unsigned long mins = (ms / 60000) % 60;
  unsigned long hrs = ms / 3600000;
  unsigned long remMs = (ms % 1000) / 10;
  char buf[20];
  sprintf(buf, "%02lu:%02lu:%02lu:%2lu", hrs, mins, sec, remMs);
  return String(buf);
}


void updateOLED(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor (0,0);
  display.println(state ? "STARTED" : "STOPPED");

  display.setCursor (0,16);
  display.print ("TIMER: ");
  display.println(sendTime(elapsedTime));

  display.setCursor (0,32);
  if (lapCount > 0) {
  display.print ("Lap #");
  display.print (lapCount);
  display.print (": ");
  display.println (sendTime(laps[lapCount - 1]));
  }

  display.setCursor (0,48);
  if (lapCount > 1) {
  display.print ("Lap #");
  display.print (lapCount - 1);
  display.print (": ");
  display.println (sendTime(laps[lapCount - 2]));
  }

  display.display();
  }

void sendHtml() {
  String html = "<h1>STOPWATCH w/ LAPS</h1>";

  html += "<p>STATE: <span id='state'>" + String(state ? "STARTED" : "STOPPED") + "</span></p>";
  html += "<p>TIMER: <span id='timer'>" + sendTime(elapsedTime) + "</span></p>";

  html += "<button onclick=\"fetch('/toggle')\">Stop/Play</button> ";
  html += "<button onclick=\"fetch('/reset')\">Reset Timer</button> ";
  html += "<button onclick=\"fetch('/lap')\">Set Lap</button>";

  html += "<h3>Laps:</h3><div id='lap'>";
  for (int i = 0; i < lapCount; i++) {
    html += "Lap " + String(i + 1) + ": " + sendTime(laps[i]) + "<br>";
  }
  html += "</div>";

  html += R"rawliteral(
  <script>
    async function updateData() {
      const res = await fetch('/data');
      const j = await res.json();
      document.getElementById('state').textContent = j.state;
      document.getElementById('timer').textContent = j.timer;
      document.getElementById('lap').innerHTML = j.lap;
    }
    setInterval(updateData, 300);
  </script>
  )rawliteral";

  server.send(200, "text/html", html);
}

void sendData() {
  String lapHtml = "";
  for (int i = 0; i < lapCount; i++) {
    lapHtml += "Lap " + String(i + 1) + ": " + sendTime(laps[i]) + "<br>";
  }
  String json = "{";
  json += "\"state\":\"" + String(state ? "STARTED" : "STOPPED") + "\",";
  json += "\"timer\":\"" + sendTime(elapsedTime) + "\",";
  json += "\"lap\":\"" + lapHtml + "\"";
  json += "}";

  server.send(200, "application/json", json);
}


void ifToggle (){
  if (state) {
    pauseTime = elapsedTime;
  }
  else {resetTime = millis();}
  state = !state;
  updateOLED();
  server.sendHeader("Location", "/");
  server.send(303);
}

void ifReset (){
  resetTime = millis();
  elapsedTime = 0;
  pauseTime = 0;
  lapCount = 0;
  for (int i = 0; i < maxLaps; i++) {
    laps[i] = 0;
  }
  updateOLED();
  server.sendHeader("Location", "/");
  server.send(303);
}

void ifLap() {
  if (lapCount < maxLaps) {
    laps[lapCount++] = elapsedTime;
    Serial.printf("Lap %d: %s\n", lapCount, sendTime(elapsedTime).c_str());
  }
  updateOLED();
  server.send(200, "text/plain", "OK");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  pinMode(butPin, INPUT_PULLUP);
  digitalWrite(butPin, HIGH);
  pinMode(butPin2, INPUT_PULLUP);
  digitalWrite(butPin2, HIGH);
  pinMode(butPin3, INPUT_PULLUP);
  digitalWrite(butPin3, HIGH);
  butOld = digitalRead (butPin);
  butOld2 = digitalRead (butPin2);
  butOld3 = digitalRead (butPin3);

  if (!display.begin(SSD1306_SWITCHCAPVCC,0x3C)) {
    Serial.println("OLED failed");
  for (;;);
  }
  display.display();
  delay(500);

//connecting to wifi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", sendHtml);
  server.on("/data", sendData);
  server.on("/toggle", ifToggle);
  server.on("/reset", ifReset);
  server.on("/lap", ifLap);
  server.begin();

  updateOLED();
}

void loop() {
  // put your main code here, to run repeatedly:

  server.handleClient();
  unsigned long present = millis();
  
  if (state){
    elapsedTime = pauseTime +(present - resetTime);
  }
  else {elapsedTime = pauseTime;}

  butNew = digitalRead(butPin);
  if (butOld == 0 && butNew == 1) {
   ifToggle();
   updateOLED();
  }
  butOld = butNew;

  butNew2 = digitalRead(butPin2);
  if (butOld2 == 0 && butNew2 == 1) {
   ifReset();
   updateOLED();
  }
  butOld2 = butNew2;

  butNew3 = digitalRead(butPin3);
  if (butOld3 == 0 && butNew3 == 1) {
   ifLap();
   updateOLED();
  }
  butOld3 = butNew3;

  updateOLED();
}

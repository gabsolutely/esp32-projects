#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define OLED_RESET -1

Adafruit_SSD1306 display (SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WebServer server(80);
//put wifi name and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";

//change according to pins
//OLED PINS -- SDA=21, SCL= 22
int toggleLEDPin = 13;
bool toggleLEDState = true;
int alertLEDPin = 2;
int obsPin = 34;
int butPin = 19;
int butNew;
int butOld;
int butPin2 = 18;
int butNew2;
int butOld2;

int counter;
bool mode = true;
unsigned long resetTime = 0;
unsigned long lastDetected = 0;
unsigned long lastSensor = 0;
unsigned long debounce = 200;

String sendTime (unsigned long ms) {
  if (ms == 0) return "Never";
  unsigned long elapsedTime = (ms - resetTime)/1000;
  unsigned long sec = elapsedTime%60;
  unsigned long mins = (elapsedTime / 60) % 60;
  unsigned long hrs = elapsedTime/3600;
  char buf[20];
  sprintf(buf, "%02lu:%02lu:%02lu", hrs, mins, sec);
  return String(buf);
}

void updateOLED(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.print("Counter: ");
  display.println(counter);

  display.setCursor (0,16);
  display.print ("Mode: ");
  display.println(mode ? "IN" : "OUT");

  display.setCursor (0,36);
  display.print ("Last: ");
  display.println(sendTime(lastDetected));

  display.display();
  }

void sendHtml(){
  String html = "<h1>IN/OUT Counter</h1>";

  html += "<p>Counter: <span id='counter'>" + String(counter) + "</span></p>";
  html += "<p>Mode: <span id='mode'>" + String(mode?"IN":"OUT") + "</span></p>";
  html += "<p>Last Detection: <span id='last'>" + sendTime(lastDetected) + "</span></p>";

  html += "<button onclick=\"fetch('/toggle')\">Toggle Mode</button> ";
  html += "<button onclick=\"fetch('/reset')\">Reset Counter</button>";
  
  html += "<script>setInterval(async()=>{"
          "const r=await fetch('/data');"
          "const j=await r.json();"
          "document.getElementById('counter').textContent=j.counter;"
          "document.getElementById('mode').textContent=j.mode;"
          "document.getElementById('last').textContent=j.last;"

          "},200);</script>";
  
  server.send(200,"text/html",html);
}

void sendData (){
String json="{";
  json += "\"counter\":"+String(counter)+",";
  json += "\"mode\":\""+String(mode?"IN":"OUT")+"\",";
  json += "\"last\":\""+sendTime(lastDetected)+"\"";
  json += "}";
  server.send(200,"application/json",json);
}

void ifToggle (){
  mode = !mode;
  toggleLEDState = !toggleLEDState;
  digitalWrite (toggleLEDPin, toggleLEDState);
  updateOLED();
  server.sendHeader("Location", "/");
  server.send(303);
}

void ifReset (){
  counter = 0;
  lastDetected = 0;
  resetTime = millis();
  updateOLED();
  server.sendHeader("Location", "/");
  server.send(303);
  digitalWrite (alertLEDPin, HIGH);
  delay(200);
  digitalWrite (alertLEDPin, LOW);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(toggleLEDPin, OUTPUT);
  pinMode(alertLEDPin, OUTPUT);
  pinMode(butPin, INPUT_PULLUP);
  digitalWrite(butPin, HIGH);
  pinMode(butPin2, INPUT_PULLUP);
  digitalWrite(butPin2, HIGH);
  pinMode(obsPin, INPUT);
  butOld = digitalRead (butPin);
  butOld2 = digitalRead (butPin2);
  digitalWrite(toggleLEDPin, HIGH);
  
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
  server.begin();

  updateOLED();
}

void loop() {
  // put your main code here, to run repeatedly:

  server.handleClient();
  unsigned long present = millis();

  if (digitalRead (obsPin) == 0 && (present-lastSensor>debounce)){
    if  (!mode && counter > 0) {
      counter--;
      }
    else if (mode) {
      counter++;
      }
      lastDetected = present;
      lastSensor = present;
      digitalWrite (alertLEDPin, HIGH);
      updateOLED();
    }
  else if (digitalRead(obsPin) == 1) {
    digitalWrite (alertLEDPin, LOW);
   }

  butNew = digitalRead(butPin);
  if (butOld == 0 && butNew == 1) {
   ifToggle();
  }
  butOld = butNew;

  butNew2 = digitalRead(butPin2);
  if (butOld2 == 0 && butNew2 == 1) {
   ifReset();
  }
  butOld2 = butNew2;
}

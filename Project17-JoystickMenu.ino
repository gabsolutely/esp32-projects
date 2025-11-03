#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);

//put wifi name and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";

//put joystick/LED pins
// OLED PINS-- SCL = 22, SDA = 21
int JOY_X = 34;
int JOY_Y = 35;
int JOY_SW = 32;
int ledPin1 = 25;
int ledPin2 = 26;

bool led1State = false;
bool led2State = false;
int menuIndex = 0;
const int menuItems = 3;

const char htmlPage[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
  <title>ESP32 LED Control</title>
  <style>
  body { background: #111; color: white; font-family: sans-serif; text-align: center; }
  button { padding: 12px 24px; margin: 10px; border: none; border-radius: 8px; font-size: 18px; cursor: pointer; }
  .on { background: #27ae60; }
  .off { background: #c0392b; }
  </style>
  </head>
  <body>
  <h1>ESP32 Joystick + LED Control</h1>
  <button id="led1" class="off" onclick="toggle('1')">LED 1</button>
  <button id="led2" class="off" onclick="toggle('2')">LED 2</button>

  <script>
  function toggle(led) {
    fetch(`/toggle?led=${led}`)
      .then(r => r.text())
      .then(state => {
        document.getElementById(`led${led}`).className = state === '1' ? 'on' : 'off';
      });
  }
  setInterval(() => {
  fetch('/status').then(r => r.json()).then(s => {
    document.getElementById('led1').className = s.led1 ? 'on' : 'off';
    document.getElementById('led2').className = s.led2 ? 'on' : 'off';
    document.querySelectorAll('button').forEach(b => b.style.outline = 'none');
    if (s.selected === 0) document.getElementById('led1').style.outline = '2px solid yellow';
    if (s.selected === 1) document.getElementById('led2').style.outline = '2px solid yellow';
  });
}, 500);
  </script>
  </body>
  </html>
  )rawliteral";

void sendHtml() {
  server.send_P(200, "text/html", htmlPage);
}

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

void handleToggle() {
  if (!server.hasArg("led")) return;
  int led = server.arg("led").toInt();
  if (led == 1) {
    led1State = !led1State;
    digitalWrite(ledPin1, led1State);
    server.send(200, "text/plain", led1State ? "1" : "0");
  } else if (led == 2) {
    led2State = !led2State;
    digitalWrite(ledPin2, led2State);
    server.send(200, "text/plain", led2State ? "1" : "0");
  }
}

void handleStatus() {
  String json = "{\"led1\":" + String(led1State ? "true" : "false") +
                ",\"led2\":" + String(led2State ? "true" : "false") + 
                ",\"selected\":" + String(menuIndex) + "}";
  server.send(200, "application/json", json);
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0); 
  display.println("Joystick Menu");
  display.setCursor(10, 16); 
  display.println("LED 1");
  display.setCursor(10, 28); 
  display.println("LED 2");
  display.setCursor(10, 40); 
  display.println("Reset");

  int cursorY[] = {16, 28, 40};
  display.setCursor(0, cursorY[menuIndex]);
  display.print(">");
  display.display();
}

void handleJoystick() {
  static unsigned long lastMove = 0;
  int y = analogRead(JOY_Y);
  int sw = digitalRead(JOY_SW);

  if (millis() - lastMove > 200) {
    if (y < 1000 && menuIndex < menuItems - 1) menuIndex++;
    if (y > 3000 && menuIndex > 0) menuIndex--;
    lastMove = millis();
  }

  if (sw == LOW) {
    if (menuIndex == 0) led1State = !led1State;
    else if (menuIndex == 1) led2State = !led2State;
    else if (menuIndex == 2) {
      led1State = false;
      led2State = false;
    }
    digitalWrite(ledPin1, led1State);
    digitalWrite(ledPin2, led2State);
    delay(300);
  }

  updateDisplay();
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(JOY_SW, INPUT_PULLUP);

  connectWifi();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed"); while (true);
  }

  server.on("/", sendHtml);
  server.on("/toggle", handleToggle);
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();
  handleJoystick();
}

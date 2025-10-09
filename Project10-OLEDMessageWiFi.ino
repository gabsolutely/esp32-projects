#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wi-Fi credentials (Access Point mode)
const char* ssid = "yourWifiName";
const char* password = "yourPassword";

WebServer server(80);

// Current OLED message
String message = "Hello World!";

// Function to generate the HTML interface
void sendHtml() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<title>ESP32 OLED Message</title>";
  html += "<style>body{font-family:Arial;text-align:center;margin-top:50px;}input,button{padding:10px;margin:5px;font-size:16px;}</style></head><body>";
  html += "<h2>Send a Message to the OLED</h2>";
  html += "<form action='/display' method='get'>";
  html += "<input type='text' name='msg' placeholder='Enter Message' style='width:60%;'>";
  html += "<input type='submit' value='Send'>";
  html += "</form>";
  html += "<p>Current Message Displayed: <b>" + message + "</b></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Function to update OLED display
void updateOLED() {
  if (server.hasArg("msg")) {
    message = server.arg("msg");
    Serial.print("New Message: ");
    Serial.println(message);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 25);
    display.println(message);
    display.display();
  }
  sendHtml();
}

void setup() {
  Serial.begin(115200);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed!");
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Starting WiFi AP...");
  display.display();

  // Start Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();

  Serial.println("WiFi Access Point started!");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.print("IP address: ");
  Serial.println(IP);

  // Display connection info on OLED
  display.clearDisplay();
  display.setCursor(0, 5);
  display.println("Connect to:");
  display.setTextSize(1);
  display.setCursor(0, 15);
  display.println(ssid);
  display.println("IP:");
  display.println(IP);
  display.display();

  // Route handlers
  server.on("/", sendHtml);
  server.on("/display", updateOLED);
  server.begin();
}

void loop() {
  server.handleClient();
}

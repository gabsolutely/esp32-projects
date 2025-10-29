#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
  #define FORMAT_SPIFFS_IF_FAILED true
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);

//put your wifi name and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";

//put your button/dht pin
//OLED PINS -- SDA=21, SCL= 22
DHT dht(4, DHT11);
int butPin = 18;
bool butOld, butNew;
bool armed = true;

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>ESP32 DHT Logger</title>
<style>
  body { font-family: Arial; text-align: center; margin: 20px; }
  canvas { 
    width: 90%; 
    height: 300px; 
    display: block; 
    margin: 20px auto; 
    border: 1px solid #ccc; 
  }
  button { 
    font-size: 16px; 
    padding: 10px 20px; 
    margin-top: 10px; 
  }
</style>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
<h2>ESP32 DHT Logger</h2>
<canvas id="myChart" width="600" height="300"></canvas><br>
<p><a href='/download'><button>DOWNLOAD CSV</button></a></p>

<script>
window.addEventListener('load', () => {
  const ctx = document.getElementById('myChart').getContext('2d');
  const chart = new Chart(ctx, {
    type: 'line',
    data: {
      labels: [],
      datasets: [
        { label: 'Temperature (°C)', data: [], borderColor: 'red', fill: false },
        { label: 'Humidity (%)', data: [], borderColor: 'blue', fill: false }
      ]
    },
    options: {
      animation: false,
      scales: { y: { min: 0, max: 100 } }
    }
  });

  async function fetchData() {
    try {
      const res = await fetch('/data');
      const text = (await res.text()).trim();
      console.log("RAW:", text);
      if (!text) return;

      const lines = text.split(/\r?\n/).filter(l => l.trim().length > 0);
      const times = [], temps = [], hums = [];

      for (let i = 0; i < lines.length; i++) {
        const parts = lines[i].split(' / ').map(p => p.trim());
        if (parts.length === 3) {
          const t = parts[0];
          const temp = parseFloat(parts[1]);
          const hum = parseFloat(parts[2]);
          if (!isNaN(temp) && !isNaN(hum)) {
            times.push(t);
            temps.push(temp);
            hums.push(hum);
          }
        }
      }

      chart.data.labels = times.slice(-10);
      chart.data.datasets[0].data = temps.slice(-10);
      chart.data.datasets[1].data = hums.slice(-10);
      chart.update('none');

      console.log("Parsed:", times, temps, hums);

    } catch (err) {
      console.error("Fetch error:", err);
    }
  }

  fetchData();
  setInterval(fetchData, 2000);
});
</script>
</body>
</html>
)rawliteral";

void connectWifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 30000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi failed, starting AP mode...");
    WiFi.softAP("ESP32_Logger", "12345678");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
  }
}

String sendTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "??:??:??";
  }
  char t[9];
  strftime(t, sizeof(t), "%H:%M:%S", &timeinfo);
  return String(t);
}

void sendHtml() { server.send(200, "text/html", htmlPage); }

void handleData() {
  if (!SPIFFS.exists("/log.txt")) {
    server.send(200, "text/plain", "");
    return;
  }

  File file = SPIFFS.open("/log.txt", "r");
  if (!file) {
    server.send(500, "text/plain", "Failed to open log file");
    return;
  }

  String buffer;
  while (file.available()) buffer += (char)file.read();
  file.close();
  server.send(200, "text/plain", buffer);
}

void handleDownload() {
  if (!SPIFFS.exists("/log.txt")) {
    server.send(404, "text/plain", "No log file found");
    return;
  }

  File file = SPIFFS.open("/log.txt", "r");
  if (!file) {
    server.send(500, "text/plain", "Failed to open log file");
    return;
  }

  String csv = "Time,Temperature,Humidity\n";
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.replace("\r", "");
    if (line.length() > 0) {
      line.replace(" / ", ",");
      csv += line + "\n";
    }
  }
  file.close();

  server.sendHeader("Content-Disposition", "attachment; filename=\"log.csv\"");
  server.send(200, "text/csv", csv);
}

void updateOLED(String entry) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("DATA LOGGER");
  display.setCursor(0, 30);
  display.println(entry);
  display.setCursor(0, 50);
  display.print("MODE: ");
  display.println(armed ? "ON" : "OFF");
  display.display();
}

void handleButton() {
  butNew = digitalRead(butPin);
  if (butOld == 1 && butNew == 0) armed = !armed;
  butOld = butNew;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed!");
    for (;;);
  }

  pinMode(butPin, INPUT_PULLUP);
  butOld = digitalRead(butPin);

  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
  }

  // Always clear the log on boot
  if (SPIFFS.exists("/log.txt")) {
    SPIFFS.remove("/log.txt");
    Serial.println("Log cleared on boot.");
  }

  connectWifi();
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  // webserver routes
  server.on("/", sendHtml);
  server.on("/data", handleData);
  server.on("/download", handleDownload);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  handleButton();

  float temp = dht.readTemperature();  // write true inside parenthesis and change signs for fahrenheit
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum) || temp <= 0 || hum <= 0) {
    Serial.println("Invalid DHT reading, skipped.");
    delay(2000);
    return;
  }

  String entry = sendTime() + " / " + String(temp) + " / " + String(hum);

  static unsigned long lastOLED = 0;
  if (millis() - lastOLED > 1000) {
    updateOLED(entry);
    lastOLED = millis();
  }

  static unsigned long lastLog = 0;
  if (armed && millis() - lastLog > 2000) {
    File f = SPIFFS.open("/log.txt", FILE_APPEND);
    if (f) {
      f.print(entry + "\n");
      f.close();
      Serial.println(entry);
    } else {
      Serial.println("File write failed!");
    }
    lastLog = millis();
  }
}

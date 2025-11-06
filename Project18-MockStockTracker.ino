#include <WiFi.h>
#include <WebServer.h>
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

//put your button/LED pins
//OLED PINS -- SDA = 21, SCL = 22
int redPin = 27;
int greenPin = 14;
int bluePin = 12;
int butPin = 18;
int currentPage = 0;
int totalPages = 3;
volatile bool pageChanged = false;

struct Stock {
  String name;
  float price;
  float change;
  float history[8];
};

Stock stocks[3] = {
  {"NVDA", 113.45, 1.34, {113.0,113.2,113.4,113.3,113.5,113.4,113.45,113.45}},
  {"AAPL", 189.27, -0.25, {189,189.1,189.2,189.3,189.2,189.1,189.25,189.27}},
  {"MSFT", 175.62, 4.12, {174,174.5,175,175.5,175,175.3,175.5,175.62}}
};
int stockCount = 3;

void connectWifi() {
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

const char htmlPage[] PROGMEM =
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"  <meta charset=\"UTF-8\">\n"
"  <title>Stock Tracker</title>\n"
"  <script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n"
"  <style>\n"
"    body { font-family: Arial, sans-serif; text-align: center; background: #f0f0f0; }\n"
"    canvas { max-width: 600px; margin: 20px auto; background: white; padding: 10px; border-radius: 10px; display: block; }\n"
"  </style>\n"
"</head>\n"
"<body>\n"
"  <h1>Stock Tracker</h1>\n"
"  <div id=\"charts\"></div>\n"
"\n"
"  <script>\n"
"    const stocks = [\"NVDA\", \"AAPL\", \"MSFT\"];\n"
"    let charts = {};\n"
"\n"
"    function createCharts(historyData) {\n"
"      const container = document.getElementById('charts');\n"
"      container.innerHTML = '';\n"
"\n"
"      stocks.forEach(stock => {\n"
"        const canvas = document.createElement('canvas');\n"
"        canvas.id = 'chart_' + stock;\n"
"        container.appendChild(canvas);\n"
"\n"
"        const data = historyData[stock] || [];\n"
"\n"
"        charts[stock] = new Chart(canvas.getContext('2d'), {\n"
"          type: 'line',\n"
"          data: {\n"
"            labels: Array.from({ length: data.length }, (_, i) => i + 1),\n"
"            datasets: [{\n"
"              label: stock,\n"
"              data: data,\n"
"              borderColor: 'rgba(75,192,192,1)',\n"
"              backgroundColor: 'rgba(75,192,192,0.2)',\n"
"              tension: 0\n"
"            }]\n"
"          },\n"
"          options: {\n"
"            animation: false,\n"
"            responsive: true,\n"
"            plugins: { legend: { display: true } },\n"
"            scales: { y: { beginAtZero: false } }\n"
"          }\n"
"        });\n"
"      });\n"
"    }\n"
"\n"
"    function updateCharts(historyData) {\n"
"      stocks.forEach(stock => {\n"
"        if (charts[stock]) {\n"
"          charts[stock].data.labels = Array.from({ length: historyData[stock].length }, (_, i) => i + 1);\n"
"          charts[stock].data.datasets[0].data = historyData[stock];\n"
"          charts[stock].update();\n"
"        }\n"
"      });\n"
"    }\n"
"\n"
"    async function fetchData() {\n"
"      try {\n"
"        const response = await fetch('/history');\n"
"        const json = await response.json();\n"
"\n"
"        let historyData = {};\n"
"        json.History.forEach(stock => {\n"
"          historyData[stock.name] = stock.values;\n"
"        });\n"
"\n"
"        if (Object.keys(charts).length === 0) createCharts(historyData);\n"
"        else updateCharts(historyData);\n"
"      } catch (err) {\n"
"        console.error('Error fetching data:', err);\n"
"      }\n"
"    }\n"
"\n"
"    setInterval(fetchData, 1000);\n"
"    fetchData();\n"
"  </script>\n"
"</body>\n"
"</html>\n";

void sendHtml() { server.send(200, "text/html", htmlPage); }

void handleHistory() {
  String json = "{\"History\":[";
  for (int i = 0; i < stockCount; i++) {
    json += "{\"name\":\"" + stocks[i].name + "\",\"values\":[";
    for (int j = 0; j < 8; j++) {
      json += String(stocks[i].history[j]);
      if (j < 7) json += ",";
    }
    json += "]}";
    if (i < stockCount - 1) json += ",";
  }
  json += "]}";
  server.send(200, "application/json", json);
}

void updateLED() {
  int ups = 0;
  for (int i = 0; i < stockCount; i++) if (stocks[i].history[6] > stocks[i].history[5]) ups++;
  if (ups == stockCount) setColor(0, 255, 0);
  else if (ups == 0) setColor(255, 0, 0);
  else setColor(255, 255, 0);
}

void setColor(int r, int g, int b) {
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

void updateHistory(Stock &s, float newPrice) {
  for (int i = 0; i < 7; i++) s.history[i] = s.history[i + 1];
  s.history[7] = newPrice;
}

void drawPage() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  Stock s = stocks[currentPage];
  display.setCursor(0, 0);
  display.print("STOCK: ");
  display.println(s.name);

  display.setCursor(0, 10);
  display.print("Price: ");
  display.println(s.price, 2);

  display.setCursor(0, 20);
  display.print("Change: ");
  display.print(s.change, 2);
  display.println("%");

  int graphX = 0;
  int graphY = 55;
  int graphWidth = 120;
  int graphHeight = 25;

  float minP = s.history[0], maxP = s.history[0];
  for (int i = 1; i < 8; i++) {
    if (s.history[i] < minP) minP = s.history[i];
    if (s.history[i] > maxP) maxP = s.history[i];
  }

  float range = maxP - minP;
  if (range == 0) range = 1;

  display.drawRect(graphX, graphY - graphHeight, graphWidth, graphHeight, SSD1306_WHITE);

  for (int i = 0; i < 7; i++) {
    int x1 = graphX + (i * (graphWidth / 8));
    int x2 = graphX + ((i + 1) * (graphWidth / 8));
    int y1 = graphY - map(s.history[i] * 100, minP * 100, maxP * 100, 0, graphHeight);
    int y2 = graphY - map(s.history[i + 1] * 100, minP * 100, maxP * 100, 0, graphHeight);
    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }

  display.display();
}

void randomPrice() {
  for (int i = 0; i < stockCount; i++) {
    float change = ((float)random(-20, 20)) / 100.0;
    stocks[i].price += change;
    stocks[i].change = change;
    updateHistory(stocks[i], stocks[i].price);
  }
}

void IRAM_ATTR nextPage() {
  currentPage++;
  if (currentPage >= totalPages) currentPage = 0;
  pageChanged = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(butPin, INPUT_PULLUP);
  digitalWrite (butPin, HIGH);

  attachInterrupt(digitalPinToInterrupt(butPin), nextPage, FALLING);
  
  connectWifi();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed!");
    for (;;);
  }

  server.on("/", sendHtml);
  server.on("/history", handleHistory);
  server.begin();
  Serial.println("WebServers Started1");

  drawPage();
}

void loop() {
  server.handleClient();

  if (pageChanged) {
    drawPage();
    pageChanged = false;
  }
  drawPage();
  randomPrice();
  updateLED();

  delay(1000);
}

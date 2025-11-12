#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MPU6050 mpu(Wire);
WebServer server(80);

//put your wifi name and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";

// calibration button, hold to calibrate
//OLED/GYRO pins -- SDA = 21, SCL = 22, yes both of them, crazy
const int calBut = 33;

float XFilt = 0.0;
float YFilt = 0.0;
const float alpha = 0.20f;

unsigned long timer;

struct LogEntry { unsigned long t; float roll; float pitch; };
const int LOG_MAX = 200;
LogEntry logbuf[LOG_MAX];
int logPos = 0;
bool logFull = false;
unsigned long lastLogMillis = 0;
const unsigned long LogIntervalMS = 500;
bool serverStarted = false;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>ESP32 Gyro Chart</title>
<style>
  body { font-family: Arial, sans-serif; text-align:center; margin:12px; }
  .canvas-wrap { max-width: 95%; margin: 12px auto; }
  canvas { width:100%; height:240px; background:#fff; border:1px solid #ddd; }
  h2 { margin:8px 0; }
</style>
</head>
<body>
<h2>Gyro Roll / Pitch</h2>
<div class="canvas-wrap"><canvas id="rollCanvas"></canvas></div>
<div class="canvas-wrap"><canvas id="pitchCanvas"></canvas></div>

<script>
const rollCanvas = document.getElementById('rollCanvas');
const pitchCanvas = document.getElementById('pitchCanvas');

function resizeCanvas(c) {
  const ratio = window.devicePixelRatio || 1;
  const w = c.clientWidth;
  const h = 240;
  c.width = Math.floor(w * ratio);
  c.height = Math.floor(h * ratio);
  c.getContext('2d').setTransform(ratio,0,0,ratio,0,0);
}
resizeCanvas(rollCanvas);
resizeCanvas(pitchCanvas);
window.addEventListener('resize', ()=>{ resizeCanvas(rollCanvas); resizeCanvas(pitchCanvas); drawAll(); });

let labels = [], rolls = [], pitchs = [];

function parseData(txt){
  labels = []; rolls = []; pitchs = [];
  if (!txt) return;
  const lines = txt.trim().split(/\r?\n/).filter(l=>l.trim().length);
  for (let line of lines) {
    const parts = line.split(' / ').map(p=>p.trim());
    if (parts.length === 3) {
      labels.push(parts[0]);
      rolls.push(parseFloat(parts[1]) || 0);
      pitchs.push(parseFloat(parts[2]) || 0);
    }
  }
}

function drawChart(canvas, data, color, minY, maxY){
  const ctx = canvas.getContext('2d');
  const W = canvas.clientWidth;
  const H = canvas.clientHeight;
  ctx.clearRect(0,0,W,H);
  ctx.fillStyle = '#fff';
  ctx.fillRect(0,0,W,H);
  ctx.strokeStyle = '#eee';
  ctx.lineWidth = 1;
  for (let i=0;i<=4;i++){
    const y = (i/4)*H;
    ctx.beginPath(); ctx.moveTo(0,y); ctx.lineTo(W,y); ctx.stroke();
  }
  if (!data || data.length === 0) return;
  const maxPoints = Math.min(data.length, 100);
  const start = Math.max(0, data.length - maxPoints);
  const slice = data.slice(start);
  const stepX = W / Math.max(1, slice.length - 1);
  ctx.beginPath();
  ctx.strokeStyle = color;
  ctx.lineWidth = 2;
  for (let i=0;i<slice.length;i++){
    const x = i * stepX;
    const v = slice[i];
    const t = (v - minY) / (maxY - minY);
    const y = H - t * H;
    if (i===0) ctx.moveTo(x,y); else ctx.lineTo(x,y);
  }
  ctx.stroke();
  const last = slice[slice.length-1];
  ctx.fillStyle = color;
  ctx.font = '14px Arial';
  ctx.fillText('Last: ' + last.toFixed(2), 6, 18);
}

function drawAll(){
  drawChart(rollCanvas, rolls, 'crimson', -180, 180);
  drawChart(pitchCanvas, pitchs, 'royalblue', -90, 90);
}

async function fetchData(){
  try {
    const res = await fetch('/data');
    if (!res.ok) return;
    const txt = await res.text();
    parseData(txt);
    drawAll();
  } catch (e) {
    console.error('fetch error', e);
  }
}

fetchData();
setInterval(fetchData, 1000);
</script>
</body>
</html>
)rawliteral";

void sendIndex() {
  server.send_P(200, "text/html", index_html);
}

void handleData() {
  String out;
  int count = logFull ? LOG_MAX : logPos;
  if (count == 0) { server.send(200, "text/plain", ""); return; }
  int start = logFull ? logPos : 0;
  for (int i = 0; i < count; ++i) {
    int idx = (start + i) % LOG_MAX;
    unsigned long s = logbuf[idx].t / 1000;
    String t = String(s) + "s";
    out += t + " / " + String(logbuf[idx].roll, 2) + " / " + String(logbuf[idx].pitch, 2) + "\n";
  }
  server.send(200, "text/plain", out);
}

void handleFavicon() {
  server.send(204, "image/x-icon", "");
}

bool connectWifiTimeout(unsigned long timeoutMs = 10000) {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs) {
    delay(300);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("\nWiFi connect timeout");
    return false;
  }
}

void addLog(float roll, float pitch) {
  logbuf[logPos].t = millis();
  logbuf[logPos].roll = roll;
  logbuf[logPos].pitch = pitch;
  logPos = (logPos + 1) % LOG_MAX;
  if (logPos == 0) logFull = true;
}

void drawLevel () {
  display.clearDisplay();

  int centerY = SCREEN_HEIGHT / 2;
  display.drawLine(0, centerY, SCREEN_WIDTH, centerY, SSD1306_WHITE);

  float angleX = XFilt;
  float angleY = YFilt;

  int lineLength = SCREEN_WIDTH / 2;
  float slope = angleX / 45.0f;
  slope = constrain(slope, -1.0f, 1.0f);

  int x1 = SCREEN_WIDTH / 2 - lineLength;
  int y1 = centerY - slope * lineLength;
  int x2 = SCREEN_WIDTH / 2 + lineLength;
  int y2 = centerY + slope * lineLength;
  display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);

  int bubbleX = SCREEN_WIDTH / 2;
  int bubbleY = centerY - constrain(angleY * 1.5f, -centerY, centerY);
  display.fillCircle(bubbleX, bubbleY, 3, SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("ROLL: ");
  display.println(angleX, 1);
  display.print("PITCH: ");
  display.println(angleY, 1);

  display.display();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Wire.begin(21, 22);
  pinMode(calBut, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed!");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU6050 Error Code: ");
    Serial.println(status);
    while (1);
  }

  mpu.calcOffsets();
  Serial.println("Initial MPU offsets calculated.");

  // hold LOW to recalibrate
  Serial.println("Hold CAL button (GPIO33) LOW now to recalibrate offsets...");
  delay(1000);
  if (digitalRead(calBut) == LOW) {
    Serial.println("Calibration button detected: recalibrating offsets...");
    mpu.calcOffsets();
    Serial.println("Calibration complete.");
  }

  mpu.update();
  XFilt = mpu.getAngleX();
  YFilt = mpu.getAngleY();

  server.on("/", sendIndex);
  server.on("/data", handleData);
  server.on("/favicon.ico", handleFavicon);
  if (connectWifiTimeout(10000)) {
    server.begin();
    serverStarted = true;
    Serial.println("WebServer started. Open root to view charts.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WebServer not started (no WiFi).");
  }

  addLog(XFilt, YFilt);
  lastLogMillis = millis();

  drawLevel();
}

void loop() {
  // put your main code here, to run repeatedly:
  mpu.update();

  float ax = mpu.getAngleX();
  float ay = mpu.getAngleY();

  XFilt = XFilt * (1.0f - alpha) + ax * alpha;
  YFilt = YFilt * (1.0f - alpha) + ay * alpha;

  drawLevel();

  if (millis() - lastLogMillis >= LogIntervalMS) {
    addLog(XFilt, YFilt);
    lastLogMillis = millis();
  }

  if (serverStarted) server.handleClient();

  delay(50);
}

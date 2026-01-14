// core/Logger.cpp

#include "Logger.h"
#include <SPIFFS.h>
#include "SensorManager.h"

// readings variables
static float lastTemp = 0;
static float lastHum = 0;
static int lastLight = 0;
static int lastMotion = 0;
static int lastAction = 0;

void Logger::init() {
    if (!SPIFFS.exists("/data.csv")) {
        File f = SPIFFS.open("/data.csv", FILE_WRITE);
        if (f) {
            f.println("ts,temp,avgTemp,hum,avgHum,light,avgLight,motion,action");
            f.close();
        }
    }
}

void Logger::append(float temp, float hum, int light, int motion, int action) {
    lastTemp = temp;
    lastHum = hum;
    lastLight = light;
    lastMotion = motion;
    lastAction = action;

    float avgTemp = SensorManager::readAverageTemp();
    float avgHum = SensorManager::readAverageHumidity();
    float avgLight = SensorManager::readAverageLight();

    String line = String(millis()) + "," +
                  String(temp, 2) + "," + String(avgTemp, 2) + "," +
                  String(hum, 2) + "," + String(avgHum, 2) + "," +
                  String(light) + "," + String(avgLight) + "," +
                  String(motion) + "," + String(action);

    File f = SPIFFS.open("/data.csv", FILE_APPEND);
    if (f) {
        f.println(line);
        f.close();
    }
}

String Logger::getJSON() {
    float avgTemp = SensorManager::readAverageTemp();
    float avgHum = SensorManager::readAverageHumidity();
    float avgLight = SensorManager::readAverageLight();
    String s = "{";
    s += "\"temp\":" + String(lastTemp, 2) + ",";
    s += "\"avgTemp\":" + String(avgTemp, 2) + ",";
    s += "\"humidity\":" + String(lastHum, 2) + ",";
    s += "\"avgHumidity\":" + String(avgHum, 2) + ",";
    s += "\"light\":" + String(lastLight) + ",";
    s += "\"avgLight\":" + String(avgLight) + ",";
    s += "\"motion\":" + String(lastMotion) + ",";
    s += "\"action\":" + String(lastAction);
    s += "}";
    return s;
}

String Logger::getHTML() {
    String html = R"rawliteral(
<!doctype html>
<html>
<head>
<meta charset="utf-8" />
<meta name="viewport" content="width=device-width,initial-scale=1" />
<title>ESPSmart-Environment</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<style>
body { font-family:Inter,system-ui,Segoe UI,Arial; margin:18px; background:#0f172a; color:#e6eef8; }
.card { background:#0b1220; padding:14px; border-radius:8px; margin-bottom:12px; box-shadow:0 6px 18px rgba(2,6,23,.6); }
h1,h2,h3 { margin:0 0 12px 0; }
.muted { color:#9fb0c8; font-size:13px; }
.preAvg { display:inline-block; margin-right:12px; font-size:16px; font-weight:bold; }
button { background:#1e293b; color:#e6eef8; border:none; padding:8px 14px; margin-right:6px; border-radius:6px; cursor:pointer; }
button:hover { background:#334155; }
</style>
</head>
<body>
<h2>ESPSmart-Environment Dashboard</h2>

<div class="card">
  <span class="preAvg">Avg Temp: <span id="avgTemp">0</span> °C</span>
  <span class="preAvg">Avg Hum: <span id="avgHum">0</span> %</span>
  <span class="preAvg">Avg Light: <span id="avgLight">0</span></span>
</div>

<div>
  <canvas id="tempChart" width="600" height="200"></canvas>
</div>
<div>
  <canvas id="humChart" width="600" height="200"></canvas>
</div>
<div>
  <canvas id="lightChart" width="600" height="200"></canvas>
</div>
<div>
  <canvas id="motionChart" width="600" height="100"></canvas>
</div>

<p><a href="/csv">Download CSV</a></p>

<div class="card">
  <h3>System Log</h3>
  <p id="statusLog" class="muted">Waiting for updates...</p>
</div>

<div class="card">
  <h3>Label Control</h3>
  <button onclick="setLabel(1)">Label 1</button>
  <button onclick="setLabel(2)">Label 2</button>
  <button onclick="setLabel(3)">Label 3</button>
  <p id="labelStatus" class="muted"></p>
</div>

<script>
const tempCtx = document.getElementById('tempChart').getContext('2d');
const humCtx = document.getElementById('humChart').getContext('2d');
const lightCtx = document.getElementById('lightChart').getContext('2d');
const motionCtx = document.getElementById('motionChart').getContext('2d');
const tempChart = new Chart(tempCtx, {type:'line', data:{labels:[], datasets:[{label:'Temp (C)',data:[],borderColor:'red',fill:false}]}, options:{animation:false}});
const humChart = new Chart(humCtx, {type:'line', data:{labels:[], datasets:[{label:'Humidity (%)',data:[],borderColor:'blue',fill:false}]}, options:{animation:false}});
const lightChart = new Chart(lightCtx, {type:'line', data:{labels:[], datasets:[{label:'Light (ADC)',data:[],borderColor:'orange',fill:false}]}, options:{animation:false}});
const motionChart = new Chart(motionCtx, {type:'line', data:{labels:[], datasets:[{label:'Motion',data:[],borderColor:'green',fill:false}]}, options:{animation:false}});

function statusMessage(d){
  let msg = "";
  if(d.motion > 0) msg += "Motion detected. ";
  else msg += "No motion. ";
  if(d.light < 2000) msg += "Low light levels. ";
  else if(d.light > 2600) msg += "High light levels. ";
  if(d.temp > 35) msg += "High temperature. ";
  if(d.humidity < 30) msg += "Low humidity. ";
  if(msg === "") msg = "All systems nominal.";
  document.getElementById("statusLog").innerText = msg.trim();
}

function setLabel(num){
  fetch('/override?value=' + num)
    .then(() => {
      document.getElementById("labelStatus").innerText = "Manual override: " + num;
    });
}

async function update(){
  try{
    const res = await fetch('/data');
    const d = await res.json();
    const t = new Date().toLocaleTimeString();

    function pushAndTrim(chart, value){
      chart.data.labels.push(t);
      chart.data.datasets[0].data.push(value);
      if(chart.data.labels.length>30){ chart.data.labels.shift(); chart.data.datasets[0].data.shift(); }
      chart.update();
    }

    pushAndTrim(tempChart, d.temp);
    pushAndTrim(humChart, d.humidity);
    pushAndTrim(lightChart, d.light);
    pushAndTrim(motionChart, d.motion);

    // update averages in cards
    document.getElementById("avgTemp").innerText = d.avgTemp.toFixed(2);
    document.getElementById("avgHum").innerText = d.avgHumidity.toFixed(2);
    document.getElementById("avgLight").innerText = d.avgLight.toFixed(0);

    statusMessage(d);
    if(d.action > 0){
      document.getElementById("labelStatus").innerText = "Label " + d.action + " activated.";
    }
  }catch(e){ console.error(e); }
}

setInterval(update, 1000);
update();
</script>
</body>
</html>
)rawliteral";

    return html;
}
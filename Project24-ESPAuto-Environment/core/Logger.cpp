// core/Logger.cpp

#include "Logger.h"
#include <SPIFFS.h>

static float lastTemp = 0;
static float lastHum = 0;
static int lastLight = 0;
static int lastMotion = 0;

void Logger::init() {
  // create file if no data.csv exists
  if (!SPIFFS.exists("/data.csv")) {
    File f = SPIFFS.open("/data.csv", FILE_WRITE);
    if (f) {
      f.println("ts,temp,hum,light,motion");
      f.close();
    }
  }
}

void Logger::append(float temp, float hum, int light, int motion) {
  lastTemp = temp;
  lastHum = hum;
  lastLight = light;
  lastMotion = motion;

  String line = String(millis()) + "," + String(temp,2) + "," + String(hum,2) + "," + String(light) + "," + String(motion);
  File f = SPIFFS.open("/data.csv", FILE_APPEND);
  if (f) {
    f.println(line);
    f.close();
  }
}

String Logger::getJSON() {
  String s = "{";
  s += "\"temp\":" + String(lastTemp,2) + ",";
  s += "\"humidity\":" + String(lastHum,2) + ",";
  s += "\"light\":" + String(lastLight) + ",";
  s += "\"motion\":" + String(lastMotion);
  s += "}";
  return s;
}

//front end
String Logger::getHTML() {
  String html = R"rawliteral(
<!doctype html>
<html>
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width,initial-scale=1" />
  <title>ESPAuto-Environment</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
  <h2>ESPAuto-Environment Dashboard</h2>
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

<script>
const tempCtx = document.getElementById('tempChart').getContext('2d');
const humCtx = document.getElementById('humChart').getContext('2d');
const lightCtx = document.getElementById('lightChart').getContext('2d');
const motionCtx = document.getElementById('motionChart').getContext('2d');

const tempChart = new Chart(tempCtx, {type:'line', data:{labels:[], datasets:[{label:'Temp (C)',data:[],borderColor:'red',fill:false}]}, options:{animation:false}});
const humChart = new Chart(humCtx, {type:'line', data:{labels:[], datasets:[{label:'Humidity (%)',data:[],borderColor:'blue',fill:false}]}, options:{animation:false}});
const lightChart = new Chart(lightCtx, {type:'line', data:{labels:[], datasets:[{label:'Light (ADC)',data:[],borderColor:'orange',fill:false}]}, options:{animation:false}});
const motionChart = new Chart(motionCtx, {type:'line', data:{labels:[], datasets:[{label:'Motion',data:[],borderColor:'green',fill:false}]}, options:{animation:false}});

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
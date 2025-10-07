#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

//put wifi name and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";

//change according to pinuouts
int photoPin = 4;
int ledPin = 2;
int butPin = 19;
int butNew;
int butOld;
int butPin2 = 18;
int butNew2;
int butOld2;
bool photoVal;
bool photon;
bool armed = false;
bool ledState = false;

//sned html to make the webserver
void sendHtml() {
  String html = "<!doctype html><html><head><meta charset='utf-8'><title>DHT11</title></head><body>"
                "<h2>PHOTOSENSOR SYSTEM</h2>"

                "<div>Light Value: <span id='photon'>--</span></div>"
                "<div>System State: <b id='s'>--</b></div>"

                "<p><a href='/armed'><button>ARM</button></a><p><a href='/disarmed'><button>DISARM</button></a>"

                "<div>LED State: <b id='l'>--</b></div>"
                "<p><a href='/ON'><button>TURN ON</button></a><p><a href='/OFF'><button>TURN OFF</button></a></p>"

                "<script>"
                "async function update(){"
                "  try{"
                "    const r = await fetch('/data');"
                "    const j = await r.json();"
                "    document.getElementById('photon').textContent = j.photon ? 'LOW' : 'HIGH';"
                "    document.getElementById('s').textContent = j.armed ? 'ARMED' : 'DISARMED';"
                "    document.getElementById('l').textContent = j.ledState ? 'ON' : 'OFF';"
                "  }catch(e){ console.log('err',e); }"
                "}"
                "update();"
                "setInterval(update,250);"

                "</script></body></html>";
  server.send(200, "text/html", html);
}

// send data without refreshing using a bit of java script 
void sendData() {
  photon = digitalRead(photoPin);
  String json = "{";
  json += "\"photon\":" + String(photon ? "true" : "false") + ",";
  json += "\"armed\":" + String(armed ? "true" : "false") + ",";
  json += "\"ledState\":" + String(ledState ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}
// commands/systems
void ifON() {
  ledState = true;
  armed = false;
  server.sendHeader("Location", "/");
  server.send(303);
}

void ifOFF() {
  ledState = false;
  armed = false;
  server.sendHeader("Location", "/");
  server.send(303);
}

void ifArmed() {
  armed = true;
  if (photon == 1){ledState = true;}
  else {ledState = false;}
  server.sendHeader("Location", "/");
  server.send(303);
}

void ifDisarmed() {
  armed = false;
  ledState = ledState;
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode (photoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(butPin, INPUT_PULLUP);
  digitalWrite(butPin, HIGH);
  pinMode(butPin2, INPUT_PULLUP);
  digitalWrite(butPin2, HIGH);
  butOld = digitalRead (butPin);
  butOld2 = digitalRead (butPin2);

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

// create webservers
  server.on("/", sendHtml);
  server.on("/data", sendData);
  server.on("/armed", ifArmed);
  server.on("/disarmed", ifDisarmed);
  server.on("/ON", ifON);
  server.on("/OFF", ifOFF);

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  photon = digitalRead (photoPin);
  if (armed) {
    ledState = photon;
  }

  butNew = digitalRead(butPin);
  if (butOld == 0 && butNew == 1) {
   if (armed) ifDisarmed();
   else ifArmed();
  }
  butOld = butNew;

  butNew2 = digitalRead(butPin2);
  if (butOld2 == 0 && butNew2 == 1) {
    if (ledState) ifOFF();
    else ifON();
  }
  butOld2 = butNew2;

  digitalWrite (ledPin, ledState ? HIGH : LOW);
}

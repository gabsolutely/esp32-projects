#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

WebServer server(80);

//put wifi name and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";

//change according to pinuouts and model
DHT dht(4, DHT11);
int ledPin = 2;
int butPin = 18;
int butNew;
int butOld;
bool armed = false;

//sned html to make the webserver
void sendHtml() {
  String html = "<!doctype html><html><head><meta charset='utf-8'><title>DHT11</title></head><body>"
                "<h2>DHT11 + State</h2>"

                "<div>Temperature: <span id='t'>--</span> &deg;C</div>"
                "<div>Humidity: <span id='h'>--</span> %</div>"
                "<div>State: <b id='s'>--</b></div>"

                "<p><a href='/armed'><button>ARM</button></a> <a href='/disarmed'><button>DISARM</button></a></p>"

                "<script>"
                "async function update(){"
                "  try{"
                "    const r = await fetch('/data');"
                "    const j = await r.json();"
                "    document.getElementById('t').textContent = (j.temp === null)?'--':j.temp;"
                "    document.getElementById('h').textContent = (j.hum === null)?'--':j.hum;"
                "    document.getElementById('s').textContent = j.armed ? 'ARMED' : 'DISARMED';"
                "  }catch(e){ console.log('err',e); }"
                "}"
                "update();"
                "setInterval(update,250);"

                "</script></body></html>";
  server.send(200, "text/html", html);
}

// send data without refreshing using a bit of java script 
void sendData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();;
  //isnan = no reading (null)
  String json = "{";
  if (isnan(t) && armed == false) {json += "\"temp\":null,";}
  if (armed == true) {json += "\"temp\":" + String(t,1) + ",";}
  if (isnan(h) && armed == false) {json += "\"hum\":null,";}
  if (armed == true) {json += "\"hum\":"  + String(h,1) + ",";}
  json += "\"armed\":" + String(armed ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

void ifArmed() {
  armed = true;
  digitalWrite(ledPin, HIGH); 
  server.sendHeader("Location", "/");
  server.send(303);
}

void ifDisarmed() {
  armed = false;
  digitalWrite(ledPin, LOW); 
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();

  pinMode(ledPin, OUTPUT);
  pinMode(butPin, INPUT_PULLUP);
  digitalWrite(butPin, HIGH);

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
  server.on("/armed", ifArmed);
  server.on("/disarmed", ifDisarmed);

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();

  butNew = digitalRead(butPin);
  if (butOld == 0 && butNew == 1) {
    if (armed) ifDisarmed();
    else ifArmed();
  }
  butOld = butNew;

}

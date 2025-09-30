#include <WiFi.h>
#include <WebServer.h>
WebServer server (80);
int ledPin = 2;
// put your wifi and ssid
const char* ssid = "yourWifiName";
const char* password = "yourPassword";
// you can add more buttons to it by copying the "href thing" and adding more server.on thingies
const char* htmlPage = R"rawliteral( 
  <!DOCTYPE html>
  <html>
  <nody style="textalign:center;
  font-family:sansserif;">
  <h1> ESP32 LED CONTROL </h1>
  <a href="/on"><button>Turn On</button></a>
  <a href="/off"><button>Turn Off</button></a>
  </body>
  </html>
  )rawliteral";

void setup() {
  // put your setup code here, to run once:
  pinMode (ledPin, OUTPUT);
  digitalWrite (ledPin, LOW);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
  delay (500);
  Serial.print(".");
  }
  Serial.println ("");
  Serial.println ("CONNECTED TO WIFI");
  Serial.println ("IP address: ");
  Serial.println (WiFi.localIP());

  server.on ("/", []() {
    server.send(200, "text/html", htmlPage);
  } );
   // turn on
  server.on ("/on", []() {
    digitalWrite (ledPin, HIGH);
    server.send(200, "text/plain", "LED IS ON");
  } );
   // turn off
  server.on ("/off", []() {
    digitalWrite (ledPin, LOW);
    server.send(200, "text/plain", "LED IS OFF");
  } );

server.begin();
Serial.println("HTTP SERVER STARTED");
  }

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}

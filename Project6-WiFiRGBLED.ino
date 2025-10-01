#include <WiFi.h>
#include <WebServer.h>
// put your wifi and ssid
const char* ssid = "yourWifiName";
const char* password = "yourPassword";
WebServer server (80);

//chamge according to pinouts
int redPin = 2;
int greenPin = 15;
int bluePin = 4;

// you can add more buttons to it by copying the "href thing" and adding more server.on thingies
const char* htmlPage = R"rawliteral( 
  <!DOCTYPE html>
  <html>
  <nody style="textalign:center;
  font-family:sansserif;">
  <h1> ESP32 RGB LED CONTROL </h1>
  <a href="/on"><button>Turn On</button></a>
  <a href="/off"><button>Turn Off</button></a>
  <a href="/red"><button>Turn Red</button></a>
  <a href="/green"><button>Turn Green</button></a>
  <a href="/blue"><button>Turn Blue</button></a>
  </body>
  </html>
  )rawliteral";

void setup() {
  // put your setup code here, to run once:
  pinMode (redPin, OUTPUT);
  pinMode (greenPin, OUTPUT);
  pinMode (bluePin, OUTPUT);
  digitalWrite (redPin, LOW);
  digitalWrite (greenPin, LOW);
  digitalWrite (bluePin, LOW);

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

  server.on ("/on", []() {
    digitalWrite (redPin, HIGH);
    digitalWrite (greenPin, HIGH);
    digitalWrite (bluePin, HIGH);
    server.send(200, "text/plain", "LED IS ON");
  } );

  server.on ("/off", []() {
    digitalWrite (redPin, LOW);
    digitalWrite (greenPin, LOW);
    digitalWrite (bluePin, LOW);
    server.send(200, "text/plain", "LED IS OFF");
  } );

  server.on ("/green", []() {
    digitalWrite (redPin, LOW);
    digitalWrite (greenPin, HIGH);
    digitalWrite (bluePin, LOW);
    server.send(200, "text/plain", "LED IS GREEN");
  } );

  server.on ("/red", []() {
    digitalWrite (redPin, HIGH);
    digitalWrite (greenPin, LOW);
    digitalWrite (bluePin, LOW);
    server.send(200, "text/plain", "LED IS RED");
  } );

  server.on ("/blue", []() {
    digitalWrite (redPin, LOW);
    digitalWrite (greenPin, LOW);
    digitalWrite (bluePin, HIGH);
    server.send(200, "text/plain", "LED IS BLUE");
  } );

server.begin();
Serial.println("HTTP SERVER STARTED");
  }

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}

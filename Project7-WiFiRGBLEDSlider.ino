q#include <WiFi.h>
// put wifi and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";
WiFiServer server (80);

//chamge according to pinouts
int greenPin = 15;
int redPin = 2;
int bluePin = 4;

int greenVal;
int redVal;
int blueVal;



void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode (redPin, OUTPUT);
pinMode (greenPin, OUTPUT);
pinMode (bluePin, OUTPUT);

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
server.begin ();
}

void loop() {
  // put your main code here, to run repeatedly:
WiFiClient client = server.available();
if (!client) {return;}

String req = client.readStringUntil('\r');
client.flush();

// you can duplicate these segments and change a few variables if you want to add more
  if (req.indexOf("/redSlider?value=") != -1) {
  int eq = req.indexOf('=');
  int sp = req.indexOf(' ', eq);
  String v = req.substring(eq + 1, sp);
  int redVal = constrain(v.toInt(), 0, 255);
  analogWrite(redPin, redVal);
  Serial.print("red=");
  Serial.println(redVal);
}

if (req.indexOf("/greenSlider?value=") != -1) {
  int eq = req.indexOf('=');
  int sp = req.indexOf(' ', eq);
  String v = req.substring(eq + 1, sp);
  int greenVal = constrain(v.toInt(), 0, 255);
  analogWrite(greenPin, greenVal);
  Serial.print("green=");
  Serial.println(greenVal);
}

if (req.indexOf("/blueSlider?value=") != -1) {
  int eq = req.indexOf('=');
  int sp = req.indexOf(' ', eq);
  String v = req.substring(eq + 1, sp);
  int blueVal = constrain(v.toInt(), 0, 255);
  analogWrite(bluePin, blueVal);
  Serial.print("blue=");
  Serial.println(blueVal);
}

String html = 
// you can duplicate segments and change the id and function call
  "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\n\r\n"
  "<!doctype html><html><body style='text-align:center;font-family:sans-serif;'>"
  "<h3> RGB LED CUSTOMIZER</h3>"

  "<input id='s1' type='range' min = '0' max = '255' value='" + String (redVal) + "'oninput = 'u(this.value)'>"
  "<div>Red: <scan id='v1'>" + String (redVal) + "</span></div>"

  "<input id='s2' type='range' min = '0' max = '255' value='" + String (greenVal) + "'oninput = 'u(this.value)'>"
  "<div>Green: <scan id='v2'>" + String (greenVal) + "</span></div>"

  "<input id='s3' type='range' min = '0' max = '255' value='" + String (blueVal) + "'oninput = 'u(this.value)'>"
  "<div>Blue: <scan id='v3'>" + String (blueVal) + "</span></div>"
  "<script>"

  "function u1(val){"
  "document.getElementById('v1').innerText=val;"
  "var x=new XMLHttpRequest();"
  "x.open('GET', '/redSlider?value='+val,true);"
  "x.send();"
  "}"

  "function u2(val){"
  "document.getElementById('v2').innerText=val;"
  "var x=new XMLHttpRequest();"
  "x.open('GET', '/greenSlider?value='+val,true);"
  "x.send();"
  "}"

  "function u3(val){"
  "document.getElementById('v3').innerText=val;"
  "var x=new XMLHttpRequest();"
  "x.open('GET', '/blueSlider?value='+val,true);"
  "x.send();"
  "}"

  "</script>"
  "</body></html>";


  client.print(html);
  delay (1);
}

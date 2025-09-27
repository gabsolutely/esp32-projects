#include "BluetoothSerial.h"
BluetoothSerial BTserial;
//change according to pinouts
int redPin = 13;
int greenPin = 12;
int bluePin = 14;
int redVal;
int greenVal;
int blueVal;
String cmd;


void setup() {
  // put your setup code here, to run once:
  //.          name of esp32 bluetooth
BTserial.begin ("BTserial");
pinMode (redPin, OUTPUT);
pinMode (greenPin, OUTPUT);
pinMode (bluePin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
BTserial.println("\n1. TURN LED OFF");
BTserial.println("2. TURN LED ON");
BTserial.println("3. PICK COLOR");
  while (!BTserial.available());
cmd = BTserial.readString();

if (cmd == "1") {
redVal=0;
greenVal=0;
blueVal=0;
}

if (cmd == "2") {
redVal=255;
greenVal=255;
blueVal=255;
}
//RGB color mixing
if (cmd == "3") {
 BTserial.println("\nHOW BRIGHT ARE THE COLORS? (0-255)");
 BTserial.println("ENTER RED: ");
 while (!BTserial.available());
 redVal = BTserial.parseInt();

 BTserial.println("\nENTER GREEN: ");
 while (!BTserial.available());
 greenVal = BTserial.parseInt();

 BTserial.println("\nENTER BLUE: ");
 while (!BTserial.available());
 blueVal = BTserial.parseInt();
}
else {BTserial.println("\nno");}
analogWrite (redPin, redVal);
analogWrite (greenPin, greenVal);
analogWrite (bluePin, blueVal);


}

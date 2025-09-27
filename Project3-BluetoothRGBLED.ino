#include "BluetoothSerial.h"
BluetoothSerial BTserial;
// change according to your pinouts
int redPin = 15;
int greenPin = 2;
int greenState = 1;
int butPin = 18;
//the pir is the motion sensor
int pirPin = 23;
int butNew;
int butOld;
int pirVal = 0;
int pirState = LOW;
bool armed = 1;
char cmd;
void setup() {
  // put your setup code here, to run once:
BTserial.begin ("BTserial");
pinMode (redPin, OUTPUT);
pinMode (greenPin, OUTPUT);
digitalWrite (greenPin, HIGH);
pinMode (butPin, INPUT_PULLUP);
digitalWrite (butPin, HIGH);
pinMode (pirPin, INPUT);


}

void loop() {
  // put your main code here, to run repeatedly:

if (BTserial.available()) {
cmd = BTserial.read();
BTserial.println("\nMORTION SENSOR MENU");
BTserial.println("1. ARM");
BTserial.println("2. DISARM");

if (cmd == '1') {
BTserial.println("\nARMED");
digitalWrite (greenPin, LOW);
greenState = 0;
armed = 1;
}

if (cmd == '2') {
BTserial.println("\nDISARMED");
digitalWrite (greenPin, HIGH);
greenState = 1;
armed = 0;
}
}

butNew = digitalRead (butPin);
if (butOld==0 && butNew == 1){
  armed = !armed;
 if (greenState == 1){
digitalWrite (greenPin, LOW);
greenState = 0;
 }
}
if (!armed) {
digitalWrite (greenPin, HIGH);
greenState = 1;
}
butOld = butNew;


pirVal = digitalRead (pirPin);
while (pirVal == HIGH && armed == 1) {
  if (pirState == LOW && armed == 1) {
BTserial.println("MOTION DETECTED");
digitalWrite (redPin , HIGH);
pirState = HIGH;
delay (500);
  }
}
else {
 if (pirState == HIGH && armed == 1) {
BTserial.println ("MOTION ENDED");
digitalWrite (redPin, LOW);
pirState = LOW;
 }
}

}

// put your ledPins here
int ledPin1 = 10;
int ledPin2 = 10;
int ledPin3 = 21;
int dt = 200;

void setup() {
// Put your setup code here, to run once:
pinMode (ledPin1, OUTPUT);
pinMode (ledPin2, OUTPUT);
pinMode (ledPin3, OUTPUT);
}
void loop() {
// Put your main code here, to run repeatedly:
digitalWrite (ledPin1, HIGH);
digitalWrite (ledPin2, LOW);
digitalWrite (ledPin3, LOW);
delay (dt);

digitalWrite (ledPin1, LOW);
digitalWrite (ledPin2, HIGH);
digitalWrite (ledPin3, LOW);
delay (dt);

digitalWrite (ledPin1, LOW);
digitalWrite (ledPin2, LOW);
digitalWrite (ledPin3, HIGH);
delay (dt);
}

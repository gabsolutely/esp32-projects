//put pinouts here
int redPin = 15;
int greenPin = 2;
int potPin = 4;
int potVal;
float bright;

void setup() {
  // put your setup code here, to run once:
pinMode (redPin, OUTPUT);
pinMode (greenPin, OUTPUT);
pinMode (potPin, INPUT);
Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  potVal = analogRead(potPin);
  bright = (255./4095.)*potVal;

  if (potVal == 4095){
    digitalWrite (greenPin, HIGH);
  }
  
  analogWrite (redPin, bright);
  digitalWrite (greenPin, LOW);

  Serial.println(potVal);
}

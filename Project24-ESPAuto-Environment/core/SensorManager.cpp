// core/SensorManager.cpp

#include "SensorManager.h"
#include <Arduino.h>

// change depending on your pins
const int SensorManager::DHT_PIN = 4;
const int SensorManager::DHT_TYPE = DHT11;
const int SensorManager::LIGHT_PIN = 34;
const int SensorManager::PIR_PIN = 18;
DHT SensorManager::dht(SensorManager::DHT_PIN, SensorManager::DHT_TYPE);

void SensorManager::init() {
  dht.begin();
  pinMode(LIGHT_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
}

float SensorManager::readTemp() {
  float t = dht.readTemperature();
  if (isnan(t)) return -127.0f;
  return t;
}

float SensorManager::readHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) return -1.0f;
  return h;
}

int SensorManager::readLight() {
  int light = analogRead(LIGHT_PIN);
  return light; // 0-4095
}

static int lastMotionState = LOW;
int SensorManager::readMotion() {
  int current = digitalRead(PIR_PIN);
  int triggered = (current == HIGH && lastMotionState == LOW);
  lastMotionState = current;
  return current;
}
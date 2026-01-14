// core/IOManager.cpp

#include "IOManager.h"
#include <Arduino.h>

//change dependiing on your pins
const int IOManager::LED_PIN = 15;
const int IOManager::LED2_PIN = 2;
const int IOManager::BUZZ_PIN = 19;

void IOManager::init() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(BUZZ_PIN, LOW);
}

void IOManager::setLed(bool on) {
  digitalWrite(LED_PIN, on ? HIGH : LOW);
}

void IOManager::setLed2(bool on) {
  digitalWrite(LED2_PIN, on ? HIGH : LOW);
}

void IOManager::buzz(bool on) {
  digitalWrite(BUZZ_PIN, on ? HIGH : LOW);
}

// core/Automation.cpp

#include "Automation.h"
#include "IOManager.h"
#include <Arduino.h>

// thresholds (configure if needed)
const float TEMP_THRESHOLD = 31.0; // deg C
const float HUM_THRESHOLD = 78.0;  // percent
const int LIGHT_THRESHOLD = 2200;  // high = bright

void Automation::init(){
  // 
}

void Automation::evaluate(float temp, float hum, int light, int motion){
  // LED on if temp/humidity threshold
  bool alert = false;
  if (temp > TEMP_THRESHOLD) alert = true;
  if (hum > HUM_THRESHOLD) alert = true;

  // if motion detected and if dark = on
  if (light < LIGHT_THRESHOLD) {
    IOManager::setLed(true);
  } else {
    IOManager::setLed(false);
  }

  if (motion) {
    IOManager::setLed2(true);
  } else {
    IOManager::setLed2(false);
  }

  // buzzer when alert
  IOManager::buzz(alert);
}
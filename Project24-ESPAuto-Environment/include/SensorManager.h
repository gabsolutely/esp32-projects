// include/SensorManager.h

#pragma once

#include <DHT.h>

class SensorManager {
public:
  static void init();
  static float readTemp();
  static float readHumidity();
  static int readLight();
  static int readMotion();
private:
  static const int DHT_PIN;
  static const int DHT_TYPE;
  static const int LIGHT_PIN;
  static const int PIR_PIN;
  static DHT dht;
};
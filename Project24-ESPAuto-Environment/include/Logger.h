// include/Logger.h

#pragma once

#include <Arduino.h>

class Logger {
public:
  static void init();
  static void append(float temp, float hum, int light, int motion);
  static String getJSON();
  static String getHTML();
};
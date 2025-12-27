// include/Automation.h

#pragma once

class Automation {
public:
  static void init();
  static void evaluate(float temp, float hum, int light, int motion);
};
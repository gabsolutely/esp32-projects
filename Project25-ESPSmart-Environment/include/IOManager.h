// include/IOManager.h

#pragma once

class IOManager {
public:
  static void init();
  static void setLed(bool on);
  static void setLed2(bool on);
  static void buzz(bool on);
private:
  static const int LED_PIN;
  static const int LED2_PIN;
  static const int BUZZ_PIN;
};
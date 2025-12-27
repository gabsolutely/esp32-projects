// include/IOManager.h

#pragma once

class IOManager {
public:
  static void init();
  static void setLed(bool on);
  static void setLed2(bool on);
  static void buzz(bool on);
};
// include/AutomationAI.h

#pragma once
#include <Arduino.h>

class AutomationAI {
public:
    static void init();
    static void setManualOverride(int action);
    static void evaluate(float temp, float hum, int light, int motion);

private:
    static int manualOverride;
    static unsigned long manualTime;
    static const unsigned long overrideDuration;

    static unsigned int getFinalAction(int aiAction);
};
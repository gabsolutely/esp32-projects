// core/AutomationAI.cpp

#include "AutomationAI.h"
#include "IOManager.h"
#include "SensorManager.h"
#include "Logger.h"
#include <Arduino.h>
#include "TFLIteMicro.h"
#include "model_data.h"

TFLIteMicro tflite;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

constexpr float mean_temp = 27.5f;
constexpr float mean_hum = 55.0f;
constexpr float mean_light = 2048.0f;
constexpr float std_temp = 3.5f;
constexpr float std_hum = 12.0f;
constexpr float std_light = 1200.0f;

float lastTemps[5] = {0};
float lastHums[5] = {0};
int lastLights[5] = {0};
int tempIdx = 0;

int lastMotion = 0;
unsigned long motionDebounce = 0;
const unsigned long DEBOUNCE_MS = 500;

int AutomationAI::manualOverride = -1;
unsigned long AutomationAI::manualTime = 0;
const unsigned long AutomationAI::overrideDuration = 10000;

void AutomationAI::init() {
    tflite.begin(model_data);
    input  = tflite.getInput();
    output = tflite.getOutput();
}

void AutomationAI::setManualOverride(int action) {
    manualOverride = action;
    manualTime = millis();
}

unsigned int AutomationAI::getFinalAction(int aiAction) {
    if (manualOverride >= 0) {
        if (millis() - manualTime > overrideDuration) {
            manualOverride = -1;
        } else {
            return manualOverride;
        }
    }
    return aiAction;
}

void AutomationAI::evaluate(float temp, float hum, int light, int motion) {
    if (!input || !output) return;

    // smoothing
    lastTemps[tempIdx]  = temp;
    lastHums[tempIdx]   = hum;
    lastLights[tempIdx] = light;
    tempIdx = (tempIdx + 1) % 5;

    float avgTemp = 0, avgHum = 0;
    int avgLight = 0;
    for (int i = 0; i < 5; i++) {
        avgTemp  += lastTemps[i];
        avgHum   += lastHums[i];
        avgLight += lastLights[i];
    }
    avgTemp  /= 5.0f;
    avgHum   /= 5.0f;
    avgLight /= 5;

    // motion debounce
    unsigned long now = millis();
    if (motion != lastMotion) {
        if (now - motionDebounce > DEBOUNCE_MS) {
            lastMotion = motion;
            motionDebounce = now;
        } else {
            motion = lastMotion;
        }
    }

    // normalize
    for (int i = 0; i < 4; i++) input->f[i] = 0;
    input->f[0] = (avgTemp  - mean_temp)  / std_temp;
    input->f[1] = (avgHum   - mean_hum)   / std_hum;
    input->f[2] = (avgLight - mean_light) / std_light;
    input->f[3] = motion;

    // AI predict action
    int aiAction = 0;
    if (tflite.invoke()) {
        Serial.printf("Raw: %.2f %.2f %.2f %.2f\n",
        output->f[0], output->f[1], output->f[2], output->f[3]);
        float maxVal = output->f[0];
        for (int i = 1; i < 4; i++) {
            if (output->f[i] > maxVal) {
                maxVal = output->f[i];
                aiAction = i;
            }
        }
    }

    // safety tuning (optional, only if AI model is not as fine tuned)
    if (aiAction == 2 && motion == 0) aiAction = 0;
    if (aiAction == 1 && avgLight < 1500) aiAction = 0;

    // final action
    unsigned int finalAction = getFinalAction(aiAction);

    // execute actions
    IOManager::setLed(finalAction == 1);
    IOManager::setLed2(finalAction == 2);
    IOManager::buzz(finalAction == 3);

    // write
    Logger::append(avgTemp, avgHum, avgLight, motion, finalAction);

    // debugging
    Serial.printf("AI: %d, Manual: %d, Final: %d\n", aiAction, manualOverride, finalAction);
    Serial.printf("T: %.2f, H: %.2f, L: %d, M: %d\n", avgTemp, avgHum, avgLight, motion);
}
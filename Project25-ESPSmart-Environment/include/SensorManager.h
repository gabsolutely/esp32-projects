// include/SensorManager.h

#pragma once

#include <Arduino.h>
#include <DHT.h>

class SensorManager {
public:
    static void init();
    static float readTemp();
    static float readHumidity();
    static int readLight();
    static int readMotion();
    static void averageReadings(float temp, float hum, int light);
    static float readAverageTemp();
    static float readAverageHumidity();
    static float readAverageLight();

private:
    static const int DHT_PIN;
    static const int DHT_TYPE;
    static const int LIGHT_PIN;
    static const int PIR_PIN;
    static DHT dht;
    static float avgTemp;
    static float avgHum;
    static float avgLight;
    static int sampleCount;
    static float tempReadings[10];
    static float humReadings[10];
    static float lightReadings[10];
    static int index;
    static bool bufferField;

};

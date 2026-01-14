// core/SensorManager.cpp

#include "SensorManager.h"

const int SensorManager::DHT_PIN = 4;
const int SensorManager::DHT_TYPE = DHT11;
const int SensorManager::LIGHT_PIN = 34;
const int SensorManager::PIR_PIN = 18;

DHT SensorManager::dht(SensorManager::DHT_PIN, SensorManager::DHT_TYPE);

float SensorManager::tempReadings[10] = {0};
float SensorManager::humReadings[10] = {0};
float SensorManager::lightReadings[10] = {0};
int SensorManager::index = 0;
bool SensorManager::bufferField = false;

void SensorManager::init() {
    dht.begin();
    pinMode(LIGHT_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
}

float SensorManager::readTemp() {
    float t = dht.readTemperature();
    return isnan(t) ? -127.0f : t;
}

float SensorManager::readHumidity() {
    float h = dht.readHumidity();
    return isnan(h) ? -1.0f : h;
}

int SensorManager::readLight() {
    return analogRead(LIGHT_PIN); // 0-4095
}

int SensorManager::readMotion() {
    return digitalRead(PIR_PIN);
}

void SensorManager::averageReadings(float temp, float hum, int light) {
    if (!isnan(temp)) tempReadings[index] = temp;
    if (!isnan(hum)) humReadings[index] = hum;
    if (!isnan(light)) lightReadings[index] = light;

    index = (index + 1) % 10;
    if (index == 0) bufferField = true;
}

float SensorManager::readAverageTemp() {
    int count = bufferField ? 10 : index;
    if (count == 0) return 0;
    float sum = 0;
    for (int i = 0; i < count; i++) sum += tempReadings[i];
    return sum / count;
}

float SensorManager::readAverageHumidity() {
    int count = bufferField ? 10 : index;
    if (count == 0) return 0;
    float sum = 0;
    for (int i = 0; i < count; i++) sum += humReadings[i];
    return sum / count;
}

float SensorManager::readAverageLight() {
    int count = bufferField ? 10 : index;
    if (count == 0) return 0;
    float sum = 0;
    for (int i = 0; i < count; i++) sum += lightReadings[i];
    return sum / count;
}
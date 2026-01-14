// model/TFLIteMicro.h

#pragma once
#include "Arduino.h"

struct TfLiteTensor {
    float f[4];
};

class TFLIteMicro {
public:
    TfLiteTensor inputTensor;
    TfLiteTensor outputTensor;

    void begin(const unsigned char* model) {
        //
    }

    TfLiteTensor* getInput() {
        return &inputTensor;
    }

    TfLiteTensor* getOutput() {
        return &outputTensor;
    }

    bool invoke() {
        for (int i = 0; i < 4; i++) outputTensor.f[i] = 0.0f;

        float tempNorm   = inputTensor.f[0];
        float humNorm    = inputTensor.f[1];
        float lightNorm  = inputTensor.f[2];
        float motionNorm = inputTensor.f[3];

        float temp = tempNorm * 3.5f + 27.5f;
        float hum  = humNorm  * 12.0f + 55.0f;
        if (lightNorm > 0.5f) outputTensor.f[1] = 1.0f;
        if (motionNorm > 0.5f) outputTensor.f[2] = 1.0f;
        if (temp > 33.0f || hum > 82.0f) outputTensor.f[3] = 1.0f;

        return true;
    }
};
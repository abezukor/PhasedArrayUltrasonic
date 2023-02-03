#pragma once

#include <stdlib.h>
#include <stdio.h>

#define SAMPLE_PERIOD 0.025
#define SAMPLE_RATE 500000
#define NUM_SAMPLES (int) (SAMPLE_PERIOD * SAMPLE_PERIOD)

#define ADC_PIN 26

namespace ADCRead{
    struct Amplitude_Return {
        uint32_t time;
        uint8_t amplitude;
    };

    void initADCAndDMA(void);

    Amplitude_Return readADCDistance(void);


}
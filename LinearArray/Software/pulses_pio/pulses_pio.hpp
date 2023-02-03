#pragma once

#include "pulses.pio.h"
#include "../PhasedArrayPulses.hpp"

#include "../ADCRead.hpp"

#define ULTRASONIC_FREQUENCY 40000
#define ULTRASONIC_PERIOD_US 25
#define PIO_FREQ (ULTRASONIC_FREQUENCY * NUM_STEPS * 4)

namespace PulsesPIO {
    void pulses_program_init();

    void doPulses(uint num_pulses);
    void doPulsesBlocking(uint num_pulses);
    ADCRead::Amplitude_Return measureDistance(uint num_pulses);

    void interupt(void);
}
#include "PhasedArrayPulses.hpp"
#include <cmath>
#include <math.h>
#include <bit>
#include <bitset>
#include "pico/stdlib.h"


void PhasedArrayPulses::createTimings(double theta){
    bool reversed = theta<0;
    theta = abs(theta);
    double phase_diff = 2*M_PI*DISTANCE_BETWEEN_EMITTERS*sin(theta)/(WAVELENGTH);

    double current_phase = 0.0;
    for(uint8_t i=0; i<NUM_STEPS; i++){
        uint8_t elem = reversed? NUM_STEPS-1-i: i;
        double elementPhase = std::fmod(current_phase, 2*M_PI);
        uint8_t beginningZeros = (uint8_t) (NUM_STEPS*elementPhase/(2*M_PI));

        emitterTimings[elem]= std::rotl(WAVE_UNSHIFTED, beginningZeros);

        current_phase+=phase_diff;
    }
}

void PhasedArrayPulses::writeToShiftRegisters(void) {

    gpio_put(SHIFT_REGISTER_WRITE_PIN, true);
    for(uint8_t bit = 0; bit<NUM_STEPS; bit++){

        for(uint8_t transmitter = 0; transmitter < NUM_EMITTERS; transmitter++){
            gpio_put(PIN_NUMBERS[transmitter], emitterTimings[transmitter][bit]);
        }
        
        gpio_put(CLOCK_PIN, true);
        sleep_us(CLOCK_PERIOD_US);
        gpio_put(CLOCK_PIN, false);
        sleep_us(CLOCK_PERIOD_US);
    }

    gpio_put(SHIFT_REGISTER_WRITE_PIN, false);
}
#include "PhasedArrayPulses.hpp"
#include <cmath>
#include <math.h>
#include <bit>
#include <bitset>
#include "pico/stdlib.h"

std::array<std::bitset<NUM_STEPS>, NUM_EMITTERS> emitterTimings;

#define initOutput(pin) {\
    gpio_init(pin); \
    gpio_set_dir(pin, GPIO_OUT); \
}

void PhasedArrayPulses::initShiftRegisters(){
    initOutput(SHIFT_REGISTER_WRITE_PIN);
    for(auto &p: PIN_NUMBERS){
        initOutput(p);
    }
}

void PhasedArrayPulses::createTimings(double theta){
    bool reversed = theta<0;
    theta = abs(theta);
    double phase_diff = 2*M_PI*DISTANCE_BETWEEN_EMITTERS*sin(theta)/(WAVELENGTH);

    double current_phase = 0.0;
    for(uint8_t i=0; i<NUM_EMITTERS; i++){
        uint8_t elem = reversed? NUM_EMITTERS-1-i: i;
        double elementPhase = std::fmod(current_phase, 2*M_PI);
        uint8_t beginningZeros = (uint8_t) (NUM_STEPS*elementPhase/(2*M_PI));

        emitterTimings[elem]= std::rotl(WAVE_UNSHIFTED, beginningZeros);

        current_phase+=phase_diff;
    }
}
void PhasedArrayPulses::createWavesFromPhaseShift(uint8_t phaseShift){

    uint16_t current_phase = 0;
    for(uint8_t i=0; i<NUM_EMITTERS; i++){
        emitterTimings[i]= std::rotl(WAVE_UNSHIFTED, current_phase);

        current_phase+=phaseShift;
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
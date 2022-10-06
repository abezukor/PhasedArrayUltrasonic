#include "PhasedArrayPulses.hpp"
#include <cmath>
#include <math.h>
#include <bit>
#include <bitset>
#include "pico/stdlib.h"

#include "PIO_constants.hpp"
#include "pulses_pio/pulses_pio.hpp"

#include <iostream>

std::array<std::bitset<NUM_STEPS>, NUM_EMITTERS> emitterTimings;

#define initOutput(pin) {\
    gpio_init(pin); \
    gpio_set_dir(pin, GPIO_OUT); \
    gpio_put(pin, false); \
}

void PhasedArrayPulses::init(){

    initOutput(SHIFT_REGISTER_WRITE_PIN);

    for(auto &p: PIN_NUMBERS){
        initOutput(p);
    }
}

void PhasedArrayPulses::createTimings(double theta){
    double phase_diff = 2*M_PI*DISTANCE_BETWEEN_EMITTERS*sin(theta)/(WAVELENGTH);

    double currentPhase = 0.0;
    int ps;
    for(uint8_t i=0; i<NUM_EMITTERS; i++){
        ps = (int) (NUM_STEPS*currentPhase/(2*M_PI));

        std::cout << "Beggining Zeros for " << i << " is " << ps << std::endl;

        emitterTimings[i]= std::bitset<NUM_STEPS>(std::rotl(WAVE_UNSHIFTED, ps));

        currentPhase+=phase_diff;
    }
    printf("Theta PD %f BT0: %s\n", phase_diff, emitterTimings[0].to_string<char,std::string::traits_type,std::string::allocator_type>().c_str());
    printf("Theta PD %f BT1: %s\n", phase_diff, emitterTimings[1].to_string<char,std::string::traits_type,std::string::allocator_type>().c_str());
}
void PhasedArrayPulses::createWavesFromPhaseShift(uint16_t phaseShift){

    int currentPhase = 0;
    for(uint8_t i=0; i<NUM_EMITTERS; i++){
        emitterTimings[i]= std::bitset<NUM_STEPS>(std::rotl(WAVE_UNSHIFTED, currentPhase));
        currentPhase+=phaseShift;
    }

    printf("BS BT0: %s\n", emitterTimings[0].to_string<char,std::string::traits_type,std::string::allocator_type>().c_str());
    printf("BS BT1: %s\n", emitterTimings[1].to_string<char,std::string::traits_type,std::string::allocator_type>().c_str());
}

void PhasedArrayPulses::writeToShiftRegisters(void) {
    std::bitset<NUM_EMITTERS> data(0);

    gpio_put(SHIFT_REGISTER_WRITE_PIN, true);
    for(uint8_t bit = 0; bit<NUM_STEPS; bit++){

        for(uint8_t transmitter = 0; transmitter < NUM_EMITTERS; transmitter++){
            data[transmitter] = emitterTimings[transmitter][bit];
        }
        
        pulse_shift_registers(data);
    }

    gpio_put(SHIFT_REGISTER_WRITE_PIN, false);

}

void pulse_shift_registers(const std::bitset<NUM_EMITTERS>& data){
    unsigned i = 0;
    for(auto &p: PhasedArrayPulses::PIN_NUMBERS){
        gpio_put(p, data[i]);
        i++;
    }
    PulsesPIO::doPulsesBlocking(1);
}

double PhasedArrayPulses::degToRadians(double degrees){
    return degrees * 2 *M_PI / 360;
}
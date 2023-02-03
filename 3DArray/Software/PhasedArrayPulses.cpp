#include "PhasedArrayPulses.hpp"
#include <cmath>
#include <math.h>
#include <bit>
#include <bitset>
#include "pico/stdlib.h"

#include "PIO_constants.hpp"
#include "pulses_pio/pulses_pio.hpp"

#include <iostream>

#define initOutput(pin) {\
    gpio_init(pin); \
    gpio_set_dir(pin, GPIO_OUT); \
    gpio_put(pin, false); \
}


bool initialized = false;

void PhasedArrayPulses::init(){

    for(auto &p: ROW_PIN_NUMBERS){
        initOutput(p);
    }
    for(auto &p: COL_PIN_NUMBERS){
        initOutput(p);
    }
    initialized = true;
}

PhasedArrayPulses::PhasedArrayPulse::PhasedArrayPulse(double theta, double phi) : t{theta}, p{phi} {

    if(!initialized){
        init();
    }

    const std::tuple<double, double, double> normalVector = std::make_tuple(
        sin(theta) * cos(phi),
        sin(theta) * sin(phi),
        cos(theta)
    );

    point2 const * lastEmitter;
    if(theta<=M_PI_2){
        lastEmitter = &emitterPositons[0][NUM_COLS-1];
    } else if(theta <= M_PI) {
        lastEmitter = &emitterPositons[0][0];
    } else if(theta <= M_PI + M_PI_2){
        lastEmitter = &emitterPositons[NUM_ROWS-1][0];
    } else {
        lastEmitter = &emitterPositons[NUM_ROWS-1][NUM_COLS-1];
    }

    const double d = -1 * (std::get<0>(normalVector)*std::get<0>(*lastEmitter) + 
        std::get<1>(normalVector) * std::get<1>(*lastEmitter));
    
    double distance, time;
    uint16_t steps;
    for(uint8_t i =0; i<NUM_ROWS; i++){
        for(uint8_t j = 0; j < NUM_COLS; j++){
            distance = pointPlaneDistance(normalVector, d, emitterPositons[i][j]);
            time = distance/SPEED_OF_SOUND;
            steps = std::round(time/STEP_TIME);
            waves[i][j] = std::bitset<NUM_STEPS>(std::rotl(WAVE_UNSHIFTED, steps));
        }
    }
    
}

double PhasedArrayPulses::PhasedArrayPulse::pointPlaneDistance(const vector3& normalVector, double D, const point2& point) const {
     return abs(
        std::get<0>(normalVector) * std::get<0>(point) + std::get<1>(normalVector) * std::get<1>(point) + D
    );
}

void PhasedArrayPulses::PhasedArrayPulse::writeToShiftRegisters(void) {
    std::bitset<NUM_COLS> data(0);

    for(uint8_t i =0; i<NUM_ROWS; i++){
        for(uint8_t j = 0; j < NUM_COLS; j++){
            gpio_put(ROW_PIN_NUMBERS[i], true);
            for(uint8_t bit = 0; bit<NUM_STEPS; bit++){

                for(uint8_t transmitter = 0; transmitter < NUM_COLS; transmitter++){
                    data[transmitter] = waves[i][j][bit];
                }
                
                pulse_shift_registers(data);
            }

            gpio_put(ROW_PIN_NUMBERS[i], false);
        }
    }
}

void pulse_shift_registers(const std::bitset<NUM_COLS>& data){
    unsigned i = 0;
    for(auto &p: PhasedArrayPulses::COL_PIN_NUMBERS){
        gpio_put(p, data[i]);
        i++;
    }
    PulsesPIO::doPulsesBlocking(1);
}

double PhasedArrayPulses::degToRadians(double degrees){
    return degrees * 2 * M_PI / 360;
}
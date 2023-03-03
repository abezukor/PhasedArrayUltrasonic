#include "PhasedArrayPulses.hpp"
#include <cmath>
#include <math.h>
#include <bit>
#include <bitset>
#include "pico/stdlib.h"

#include "PIO_constants.hpp"
#include "pulses_pio/pulses_pio.hpp"

#include <iostream>
#include <stdio.h>

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

    const point2& closestEmitter = getClosestCorner();
    const point2& farthestEmitter = getFarthestCorner();

    const double planeConstant = -1 * (std::get<0>(normalVector)*std::get<0>(closestEmitter) + 
        std::get<1>(normalVector) * std::get<1>(closestEmitter));

    double farthestDistance = pointPlaneDistance(normalVector, planeConstant, farthestEmitter);
    
    double distanceToPlane, distanceDifferenceFromFarthest;
    for(uint8_t i =0; i<NUM_ROWS; i++){
        for(uint8_t j = 0; j < NUM_COLS; j++){
            distanceToPlane = pointPlaneDistance(normalVector, planeConstant, emitterPositions[i][j]);
            distanceDifferenceFromFarthest = farthestDistance - distanceToPlane;
            waves[i][j] = std::bitset<NUM_STEPS>(std::rotl(WAVE_UNSHIFTED, distanceToSteps(distanceDifferenceFromFarthest)));
        }
    }
    
}

PhasedArrayPulses::PhasedArrayPulse::PhasedArrayPulse(uint8_t phase_shift) : t{0.0}, p{0.0}{

    if(!initialized){
        init();
    }

    unsigned total_phase_shift = 0;
    for(uint8_t i =0; i<NUM_ROWS; i++){
        for(uint8_t j = 0; j < NUM_COLS; j++){
            waves[i][j] = std::bitset<NUM_STEPS>(std::rotl(WAVE_UNSHIFTED, total_phase_shift));
            total_phase_shift+=phase_shift;
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

    for(uint8_t row =0; row<NUM_ROWS; row++){
        //std::cout << "Setting Row: " << row << std::endl;

        gpio_put(ROW_PIN_NUMBERS[row], true);
        for(uint8_t bit = 0; bit<NUM_STEPS; bit++){

            for(uint8_t transmitter = 0; transmitter < NUM_COLS; transmitter++){
                data[transmitter] = waves[row][transmitter][bit];
            }
            
            pulse_shift_registers(data);
        }

        gpio_put(ROW_PIN_NUMBERS[row], false);

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

const PhasedArrayPulses::point2& PhasedArrayPulses::PhasedArrayPulse::getClosestCorner() const {
    const uint8_t THETA_POS = 1 << 1;
    const uint8_t PHI_POS = 1;

    switch ((t>0) << 1 | (p>0)){
    case THETA_POS | PHI_POS:
        return emitterPositions[0][NUM_COLS-1];
        break;
    case THETA_POS:
        return emitterPositions[NUM_ROWS-1][NUM_COLS-1];
        break;
    case PHI_POS:
        return emitterPositions[NUM_ROWS-1][0];
        break;
    default:
        return emitterPositions[0][0];
        break;
    }
}

const PhasedArrayPulses::point2& PhasedArrayPulses::PhasedArrayPulse::getFarthestCorner() const {
    const uint8_t THETA_POS = 1 << 1;
    const uint8_t PHI_POS = 1;

    switch ((t>0) << 1 | (p>0)){
        case THETA_POS | PHI_POS:
            return emitterPositions[NUM_ROWS-1][0];
            break;
        case THETA_POS:
            return emitterPositions[0][0];
            break;
        case PHI_POS:
            return emitterPositions[0][NUM_COLS-1];
            break;
        default:
            return emitterPositions[NUM_ROWS-1][NUM_COLS-1];
            break;
    }
}

uint16_t PhasedArrayPulses::PhasedArrayPulse::distanceToSteps(double distance) const{
    double time;

    time = distance/SPEED_OF_SOUND;
    return std::round(time/STEP_TIME);
}
#include <stdlib.h>
#include <stdio.h>
#include <array>
#include <bitset>

#define NUM_EMITTERS 6
#define NUM_STEPS 64
#define DISTANCE_BETWEEN_EMITTERS 0.010
#define SPEED_OF_SOUND 343.0
#define FREQ 40000
#define WAVELENGTH SPEED_OF_SOUND/FREQ
#define WAVE_UNSHIFTED 0x00000000FFFFFFFF

#define CLOCK_PIN 2
#define SHIFT_REGISTER_WRITE_PIN 3
#define CLOCK_PERIOD_US 1000

namespace PhasedArrayPulses{

    std::array<std::bitset<NUM_STEPS>, NUM_EMITTERS> emitterTimings;
    const std::array<uint8_t, NUM_EMITTERS> PIN_NUMBERS = {4, 5, 6, 7, 8};

    void createTimings(double theta);

    void writeToShiftRegisters();
}
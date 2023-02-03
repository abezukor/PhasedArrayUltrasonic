#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <array>
#include <bitset>

#define NUM_EMITTERS 6
#define NUM_STEPS 64
#define DISTANCE_BETWEEN_EMITTERS 0.01778
#define SPEED_OF_SOUND 343.0
#define FREQ 40000
#define WAVELENGTH SPEED_OF_SOUND/FREQ


#define SHIFT_REGISTER_WRITE_PIN 28
#define CLOCK_PERIOD_US 1000

#define CLOCK_PIN 22

void pulse_shift_registers(const std::bitset<NUM_EMITTERS>& data);

namespace PhasedArrayPulses{

    constexpr std::array<uint8_t, NUM_EMITTERS> PIN_NUMBERS = {16, 17, 18, 19, 20, 21};

    const uint64_t WAVE_UNSHIFTED = 0x00000000FFFFFFFF;

    void init(void);

    void createTimings(double theta);
    void createWavesFromPhaseShift(uint16_t phaseShift);

    void writeToShiftRegisters();

    double degToRadians(double degrees);
}
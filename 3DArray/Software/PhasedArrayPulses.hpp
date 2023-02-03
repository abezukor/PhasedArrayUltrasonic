#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <array>
#include <bitset>
#include <tuple>

#define NUM_ROWS 6
#define NUM_COLS 6
#define NUM_STEPS 64
#define DISTANCE_BETWEEN_EMITTERS 0.01778
#define SPEED_OF_SOUND 343.0
#define FREQ 40000
#define WAVELENGTH SPEED_OF_SOUND/FREQ
#define STEP_TIME (1.0/(FREQ * NUM_STEPS))

#define CLOCK_PERIOD_US 1000

#define CLOCK_PIN 22


void pulse_shift_registers(const std::bitset<NUM_COLS>& data);

namespace PhasedArrayPulses{

    typedef std::tuple<double, double, double> vector3;
    typedef std::tuple<double, double> point2;

    static constexpr std::array<uint8_t, NUM_ROWS> ROW_PIN_NUMBERS = {16, 17, 18, 19, 20, 21};
    static constexpr std::array<uint8_t, NUM_COLS> COL_PIN_NUMBERS = {10, 11, 12, 13, 14, 15};

    static constexpr std::array<std::array<const point2, NUM_COLS>, NUM_ROWS> emitterPositons =
        {{
            {{{0.0, 5.0}, {1.0, 5.0}, {2.0, 5.0}, {3.0, 5.0}, {4.0, 5.0}, {5.0, 5.0}}},
            {{{0.0, 4.0}, {1.0, 4.0}, {2.0, 4.0}, {3.0, 4.0}, {4.0, 4.0}, {5.0, 4.0}}},
            {{{0.0, 3.0}, {1.0, 3.0}, {2.0, 3.0}, {3.0, 3.0}, {4.0, 3.0}, {5.0, 3.0}}},
            {{{0.0, 2.0}, {1.0, 2.0}, {2.0, 2.0}, {3.0, 2.0}, {4.0, 2.0}, {5.0, 2.0}}},
            {{{0.0, 1.0}, {1.0, 1.0}, {2.0, 1.0}, {3.0, 1.0}, {4.0, 1.0}, {5.0, 1.0}}},
            {{{0.0, 0.0}, {1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}, {4.0, 0.0}, {5.0, 0.0}}},
        }};
    const uint64_t WAVE_UNSHIFTED = 0x00000000FFFFFFFF;

    void init(void);

    class PhasedArrayPulse {
        public:
            PhasedArrayPulse(double theta, double phi);
            void writeToShiftRegisters();

        private:
            const double t, p;
            std::bitset<NUM_STEPS> waves[NUM_ROWS][NUM_COLS];

            double pointPlaneDistance(const vector3& normalVector, double D, const point2& point) const;
    };

    double degToRadians(double degrees);
}
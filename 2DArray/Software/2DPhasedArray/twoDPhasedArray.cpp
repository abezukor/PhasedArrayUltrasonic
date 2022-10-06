
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "pulses.pio.h"

//#include "ADCRead.hpp"
#include "PhasedArrayPulses.hpp"

#include "PIO_constants.hpp"
#include "pulses_pio/pulses_pio.hpp"

#define NUM_PULSES 16

int main() {
    stdio_init_all();
    printf("Start Program\n");
    
    PulsesPIO::pulses_program_init();

    pio_sm_set_enabled(MAIN_PIO, PULSES_PIO_SM, true);
    
    printf("Writing to Shift Registers");

    PhasedArrayPulses::init();
    PhasedArrayPulses::createTimings(
        PhasedArrayPulses::degToRadians(15)
    );
    PhasedArrayPulses::writeToShiftRegisters();
    

    int i =0;
    while(true){
        PulsesPIO::doPulses(NUM_PULSES*NUM_STEPS);
        sleep_ms(20);
        /*PhasedArrayPulses::createTimings(
            PhasedArrayPulses::degToRadians(30)
        );*/
        std::cout << "Loop: " << i << std::endl;
        //PhasedArrayPulses::createWavesFromPhaseShift(16);
        i++;
    }
}
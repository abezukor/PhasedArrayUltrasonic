
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "pulses.pio.h"
#include "pico/multicore.h"

#include "PhasedArrayPulses.hpp"

#include "PIO_constants.hpp"
#include "pulses_pio/pulses_pio.hpp"

#define NUM_PULSES 24
#define NUM_SETS 100

volatile bool updated = true;
volatile double theta = 0.0;
volatile double phi = 0.0;

/**
 * Thread For creating waves.
*/
void run_waves(){

    PhasedArrayPulses::PhasedArrayPulse a1(
        PhasedArrayPulses::degToRadians(theta),
        PhasedArrayPulses::degToRadians(phi)
    );

    while (true){
        while(!updated){
            a1.writeToShiftRegisters(); //Re-write every NUM_SETS cycles as sometimes writes are not perfect or waves change due to interference.
            for(int i=0; i< NUM_SETS; i++){
                PulsesPIO::doPulses(NUM_PULSES*NUM_STEPS);
                sleep_ms(20);
            }

        }
        std::cout << "New Theta: " << theta << " New Phi: " << phi << std::endl ;
        a1 = PhasedArrayPulses::PhasedArrayPulse(
            PhasedArrayPulses::degToRadians(theta),
            PhasedArrayPulses::degToRadians(phi)
        );
        a1.writeToShiftRegisters();
        updated = false;
    }
    
}

int main() {
    stdio_init_all();
    printf("Start Program\n");
    
    PulsesPIO::pulses_program_init();

    pio_sm_set_enabled(MAIN_PIO, PULSES_PIO_SM, true);
    
    PhasedArrayPulses::init();

    
    //PhasedArrayPulses::PhasedArrayPulse a1(2);

    sleep_ms(500); // Delay to allow for bounced power on
    
    multicore_launch_core1(run_waves);
    
    double thetaTemp, phiTemp;
    while(true){
        std::cout << "Updated: " << updated << "New Theta Phi:  " ;

        std::cin >> thetaTemp >> phiTemp;

        theta = thetaTemp; phi = phiTemp;
        updated = true;

    }
}
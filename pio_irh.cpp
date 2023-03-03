#include "PIO_constants.hpp"
#include "pulses_pio/pulses_pio.hpp"

void pio_irh(void){
    pio0_hw->irq = 1;
    //printf("PIO IRQ TRIGGERED");

    PulsesPIO::interupt();
}
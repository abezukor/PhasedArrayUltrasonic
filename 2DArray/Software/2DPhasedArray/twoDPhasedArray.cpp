
#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pulses.pio.h"

#define NUM_PULSES 16

#define ULTRASONIC_FREQUENCY 40000
#define ULTRASONIC_PERIOD_US 25
#define PIO_FREQ ULTRASONIC_FREQUENCY * 64 * 4

uint32_t do_pulses(PIO pio, uint sm, uint num_pulses) {
    pio_sm_put(pio, sm, num_pulses - 1);
}


u_int32_t time_started;

void pio_irh(){
    time_started = time_us_32();
    time_started-= NUM_PULSES * ULTRASONIC_PERIOD_US;
    pio0_hw->irq = 1;
    //printf("PIO IRQ TRIGGERED");
    //int_trig = 1;
}

int main() {
    stdio_init_all();
    printf("Start Program\n");
    
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &pulses_program);

    float div = (float)clock_get_hz(clk_sys) / PIO_FREQ;
    pulses_program_init(pio, sm, offset, 2, div);
    irq_set_exclusive_handler(PIO0_IRQ_0, pio_irh);
    irq_set_enabled(PIO0_IRQ_0, true);
    pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS;

    pio_sm_set_enabled(pio, sm, true);
    
    //printf("Starting PIO Block\n");


    while(true){
        do_pulses(pio,sm, NUM_PULSES * STEPS_PER_PULSE);
        
        sleep_ms(1000);
        
        //while (int_trig==0){}
        
        printf("Loop Time started: %d \n ", time_started);
    }
}
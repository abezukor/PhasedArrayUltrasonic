#include "pico/stdlib.h"
#include "pulses_pio.hpp"
#include "pulses.pio.h"
#include "hardware/clocks.h"
#include "../PIO_constants.hpp"

void PulsesPIO::pulses_program_init() {

   float div = (float)clock_get_hz(clk_sys) / PIO_FREQ;
   
   uint offset = pio_add_program(MAIN_PIO, &pulses_program);
   pio_gpio_init(MAIN_PIO, CLOCK_PIN);
   pio_sm_set_consecutive_pindirs(MAIN_PIO, PULSES_PIO_SM, CLOCK_PIN, 1, true);
   pio_sm_config c = pulses_program_get_default_config(offset);
   sm_config_set_set_pins(&c, CLOCK_PIN, 1);
   sm_config_set_clkdiv(&c, div);
   pio_sm_init(MAIN_PIO, PULSES_PIO_SM, offset, &c);

   irq_set_exclusive_handler(PIO0_IRQ_0, pio_irh);
   irq_set_enabled(PIO0_IRQ_0, true);
   pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS;
}

void PulsesPIO::doPulses(uint num_pulses) {
    pio_sm_put_blocking(MAIN_PIO, PULSES_PIO_SM, num_pulses - 1);
}

volatile bool finished = false;

void PulsesPIO::doPulsesBlocking(uint num_pulses) {
   finished = false;
   doPulses(num_pulses);
   while(!finished){
      sleep_us(3);
   }
}

void PulsesPIO::interupt(void){
   finished = true;
}
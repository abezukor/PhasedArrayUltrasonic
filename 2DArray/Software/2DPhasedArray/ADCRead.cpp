#include "ADCRead.hpp"
#include "hardware/adc.h"
#include "hardware/dma.h"

// Lots of code taken from https://github.com/raspberrypi/pico-examples/blob/master/adc/dma_capture/dma_capture.c


uint8_t samples[NUM_SAMPLES];

uint adcDMAChannel;

void ADCRead::initADCAndDMA(void) {

    //Init ADC
    adc_gpio_init(ADC_PIN);
    adc_init();
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        false,   // We won't see the ERR bit because of 8 bit reads; disable.
        true     // Shift each sample to 8 bits when pushing to FIFO
    );
    adc_set_clkdiv(0);

    //Init DMA
    adcDMAChannel = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(adcDMAChannel);

    // Reading from constant address, writing to incrementing byte addresses
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);

    // Pace transfers based on availability of ADC samples
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(adcDMAChannel, &cfg,
        samples,    // dst
        &adc_hw->fifo,  // src
        NUM_SAMPLES,  // transfer count
        true            // start immediately
    );
}

ADCRead::Amplitude_Return ADCRead::readADCDistance(void){
    adc_run(true);
    dma_channel_wait_for_finish_blocking(adcDMAChannel);
    adc_run(false);
    adc_fifo_drain();

    Amplitude_Return output;
    output.amplitude = 0;

    uint8_t val;
    for(uint i =0; i < NUM_SAMPLES; i++){
        val = std::abs(samples[i]-UINT8_MAX/2);
        if (val>output.amplitude){
            output.amplitude = val;
            output.time = i;
        }
    }

    return output;
}
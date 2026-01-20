#include "adc.h"
#include "uart2.h"
#include "gpio.h"
#include "stm32f446xx.h"
#include <stdint.h>

void adc1_pa0_pa1_init(void) {

    gpio_init(GPIOAEN);

    //set PA0 as analog
    gpio_pin_mode(GPIOA, 0, 3U);
    //set PA1 as analog
    gpio_pin_mode(GPIOA, 1, 3U);
}

void adc1_enable(void){
    //Enable clock access to ADC1
    RCC->APB2ENR |= ADC1EN;
}

void adc1_config_sample_time(uint8_t channel, uint8_t sample_cycles){
    //Set sample time
    if (channel>9){
        ADC1->SMPR1 &= ~(7<<(channel*3));
        ADC1->SMPR1 |= sample_cycles<<(channel*3);
    }
    else{
        ADC1->SMPR2 &= ~(7<<(channel*3));
        ADC1->SMPR2 |= sample_cycles<<(channel*3);
    }
}

void adc1_config_regular_sequence(uint8_t index, uint8_t channel){
    //Set which channel will be converted at sequence number seq
    if (index>12){
        ADC1->SQR1 &= ~(7<<(index*5));
        ADC1->SQR1 |= channel<<(index*5);
    }
    else if (index > 6) {
        ADC1->SQR2 &= ~(7<<(index*5));
        ADC1->SQR2 |= channel<<(index*5);
    }
    else{
        ADC1->SQR3 &= ~(7<<(index*5));
        ADC1->SQR3 |= channel<<(index*5);
    }
}

void adc1_config_sequence_length (int length){
    ADC1->SQR1 &= ~(0xFU<<20);
    ADC1->SQR1 |= (length<<20);
}

void adc1_start_conversion(void) {
    //Start conversion
    ADC1->CR2 |= CR2_SWSTART;
}

void adc1_start_continuous_conversion(void) {
    //Enable continuous mode
    ADC1->CR2 |= CR2_CONT;
    //Start conversion
    ADC1->CR2 |= CR2_SWSTART;
}

uint32_t adc1_read(void) {
    //Wait for conversion to complete
    while (!(ADC1->SR & SR_EOC)) {}

    //Read converted result
    return ADC1->DR;
}

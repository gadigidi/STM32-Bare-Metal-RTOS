#include "tim2.h"
#include "isr.h"
#include "stm32f446xx.h"
#include <stdint.h>

void tim2_init(void){
    //Enable clock access to TIM2
    RCC->APB1ENR |= TIM2_CLK_EN;
    //Set prescaler value
    TIM2->PSC = PSC_VAL;
    //Set auto-reload value
    TIM2->ARR = ARR_VAL;
    //Clear counter
    TIM2->CNT = 0;
}

void tim2_enable_interrupt(void) {
    TIM2->DIER |= TIM2_UIEN;
    isr_enable(TIM2_IRQn);
}

void tim2_enable(void) {
    //Enable timer2
    TIM2->CR1 |= TIM2_CEN;
}


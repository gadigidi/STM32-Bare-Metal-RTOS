#include "exti.h"
#include "gpio.h"
#include <stdio.h>
#include <stdint.h>
#include "stm32f446xx.h"

void exti_clean_flag(int line) {
    //Clean old PR before anabling
    EXTI->PR = (1U << line);
}

void exti_init(void) {
    //Enable clock to SYSCFG for EXTI configurations
    RCC->APB2ENR |= (1U << 14);
}

void exti_enable_irq(int line, char gpio_letter) {

    //Select GPIOA as IRQ source for EXTI, because the RC522 IRQ connected to PA1
    int reg = line % 4;
    uint16_t gpio_index = (uint16_t) (gpio_char_to_int(gpio_letter));
    uint16_t value = ~(0xFU << (line * 4));
    value |= (gpio_index << (line * 4));
    SYSCFG->EXTICR[reg] |= value;

    //Clear mask for EXTI line
    EXTI->IMR |= (1U << line);

    //Config IRQ sensetivity = active low (falling_dege only)
    EXTI->RTSR &= ~(1U << line);
    EXTI->FTSR |= (1U << line);
    //Clean old PR before anabling
    exti_clean_flag(line);
}

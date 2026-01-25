#include "exti.h"
#include "gpio.h"
#include "isr.h"
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

void exti_enable_irq(char gpio_letter, int line, int isr_flag) {

    //Select GPIOA as IRQ source for EXTI, because the RC522 IRQ connected to PA1
    int reg = line / 4;
    int offset = line % 4;
    uint16_t gpio_index = (uint16_t) (gpio_char_to_int(gpio_letter));
    uint16_t value = SYSCFG->EXTICR[reg];
    value = (gpio_index << (offset * 4));
    SYSCFG->EXTICR[reg] |= value;

    //Clear mask for EXTI line
    EXTI->IMR |= (1U << line);

    //Config IRQ sensetivity = active low (falling_edge only)
    EXTI->RTSR &= ~(1U << line);
    EXTI->FTSR |= (1U << line);
    //Clean old PR before anabling
    exti_clean_flag(line);

    isr_enable_interrupts(isr_flag);

}

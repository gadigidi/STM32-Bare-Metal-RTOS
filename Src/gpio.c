#include "gpio.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>

//GPIO_TypeDef * GPIO
void gpio_init(uint32_t enable) {
    //Enable clock access to GPIOA
    RCC->AHB1ENR |= enable;
}

void gpio_pin_mode(GPIO_TypeDef *GPIO, int pin, uint8_t mode) {
    int lsb = pin * 2;
    GPIO->MODER &= ~(3U << lsb);
    GPIO->MODER |= (mode << lsb);
}

void gpio_pin_afr(GPIO_TypeDef *GPIO, int pin, uint8_t af) { // AF0..AF15
    int lsb = (pin % 8) * 4;
    int afr = (pin < 8) ? 0 : 1;
    GPIO->AFR[afr] &= ~(0xFU << lsb);
    GPIO->AFR[afr] |= (af << lsb);
}

void gpio_pin_pupdr(GPIO_TypeDef *GPIO, int pin) { // AF0..AF15
    GPIO->PUPDR &= ~(3U << (pin * 2));
    GPIO->PUPDR |= (1U << (pin * 2));
}

int gpio_char_to_int(char letter) {
    if ((letter < 'A') || (letter > 'H')) {
        return -1;
    }
    return (int) (letter - 'A'); //Will return 0 for 'A', 1 for 'B' etc..
}

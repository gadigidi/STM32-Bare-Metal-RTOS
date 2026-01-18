#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f446xx.h"
#include <stdint.h>

#define GPIOAEN         (1U<<0)
#define PIN5            (1U<<5)
#define LED_PIN         PIN5

#define GPIOBEN         (1U<<1)

#define GPIOCEN         (1U<<2)
#define PIN13           (1U<<13)
#define BTN_PIN         PIN13

#define GPIO_MODE_INPUT  0x0
#define GPIO_MODE_OUTPUT 0x1
#define GPIO_MODE_AF     0x2
#define GPIO_MODE_ANALOG 0x3

void gpio_init(uint32_t enable);
void gpio_pin_mode(GPIO_TypeDef *GPIO, int pin, uint8_t mode);
void gpio_pin_afr(GPIO_TypeDef *GPIO, int pin, uint8_t af); // AF0..AF15
void gpio_pin_pupdr(GPIO_TypeDef *GPIO, int pin);
void gpio_set_LED(void);
void gpio_reset_LED(void);
int gpio_char_to_int(char letter);

#endif /* GPIO_H_ */


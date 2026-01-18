#ifndef EXTI_H_
#define EXTI_H_

#include <stdint.h>
#include <stdbool.h>

void exti_clean_flag(int line);
void exti_init(void);
void exti_enable_irq(int line, char gpio_letter); //Line, Port
#endif /* EXTI_H_ */

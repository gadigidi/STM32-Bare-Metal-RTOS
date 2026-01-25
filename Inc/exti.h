#ifndef EXTI_H_
#define EXTI_H_

#include <stdint.h>
#include <stdbool.h>

void exti_clean_flag(int line);
void exti_init(void);
void exti_enable_irq(char gpio_letter, int line, int isr_flag); //Port, Line, ISR enable_simbole
#endif /* EXTI_H_ */

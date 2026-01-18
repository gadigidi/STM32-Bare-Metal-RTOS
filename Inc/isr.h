#ifndef ISR_H_
#define ISR_H_

#include <stdint.h>

void isr_enable_interrupts(int irqn);
void TIM2_IRQHandler(void);
void EXTI1_IRQHandler(void);

#endif /* ISR_H_ */

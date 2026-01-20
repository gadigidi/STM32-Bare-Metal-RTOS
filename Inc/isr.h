#ifndef ISR_H_
#define ISR_H_

#include <stdint.h>

void isr_enable_interrupts(int irqn);
void TIM2_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI3_IRQHandler(void);
__attribute__((naked)) void PendSV_Handler(void);
#endif /* ISR_H_ */

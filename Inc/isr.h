#ifndef ISR_H_
#define ISR_H_

#include <stdint.h>
#include <stdbool.h>

void isr_enable_interrupts(int irqn);
void isr_disable_interrupts (int irqn);
void isr_set_priority(int irqn, uint8_t priority);
void isr_set_pendsv_priority(int priority);
volatile bool isr_show_exti15_10_pending_status(void);
void isr_clear_exti15_10_pending_status(void);

__attribute__((naked)) void PendSV_Handler(void);

#endif /* ISR_H_ */

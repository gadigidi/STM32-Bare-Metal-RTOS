#ifndef ISR_H_
#define ISR_H_

#include <stdint.h>
#include <stdbool.h>

void isr_enable_interrupt(int irqn);
void isr_disable_interrupt(int irqn);
void isr_set_priority(int irqn, uint8_t priority);
void isr_set_pendsv_priority(int priority);

#endif /* ISR_H_ */

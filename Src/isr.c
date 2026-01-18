#include "isr.h"
#include "tim2.h"
#include "timebase.h"
#include "rfid.h"
#include "seg7.h"
#include "stm32f446xx.h"
#include <stdint.h>

#include "rc522.h" //FIXME remove me
void isr_enable_interrupts(int irqn) {
    int reg = irqn / 32;
    int offset = irqn % 32;
    NVIC->ISER[reg] |= (1U << offset);
}

void TIM2_IRQHandler(void) {
    timebase_increase_ms();
    TIM2->SR &= ~TIM2_SR_UIF;

}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1U << 1)) {
        EXTI->PR = (1U << 1);
        //rc522_debug();
        rfid_update_irq_flag();
    }
}

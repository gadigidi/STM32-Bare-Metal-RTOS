#include "isr.h"
#include "os.h"
#include "tim2.h"
#include "timebase.h"
#include "stm32f446xx.h"
#include <stdint.h>

void isr_enable_interrupts(int irqn) {
    int reg = irqn / 32;
    int offset = irqn % 32;
    NVIC->ISER[reg] |= (1U << offset);
}

void isr_disable_interrupts (int irqn) {
    int reg = irqn / 32;
    int offset = irqn % 32;
    uint32_t mask = NVIC->ISER[reg];
    mask &= ~(1 << offset);
    NVIC->ISER[reg] = mask;
}

void isr_set_priorities(int irqn){

}

void TIM2_IRQHandler(void) {
    timebase_increase_ms();
    TIM2->SR &= ~TIM2_SR_UIF;
    //Turn on PendSV IRQ (software)
    SCB->ICSR |= PENDSVSET;
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1U << 1)) {
        EXTI->PR = (1U << 1);
    }
}

void EXTI13_IRQHandler(void) {
    if (EXTI->PR & (1U << 13)) {
        EXTI->PR = (1U << 13);
        user_set_btn_flag();
    }
}

__attribute__((naked)) void PendSV_Handler(void){

    __asm volatile (
        //Save context
        "MRS R0, PSP \n" //R0 = PSP
        "STMDB R0!, {R4-R11} \n" //Store R4-R11 in current stack; update PSP; R0 = new PSP
        "LDR R1, =current_tcb \n" //R1 = address where sp of current task is stored
        "LDR R1, [R1] \n" //Dereference. R1 = sp
        "STR R0, [R1] \n" //*sp = PSP

        //Call os_switch and return to next phase
        "PUSH {LR} \n" //Save LR (EXC_RETURN) because it will be overwritten by calling BL
        "BL os_switch \n"
        "POP {LR} \n" //Load LR

        //Restore context
        "LDR R1, =current_tcb \n" //R1 = address where sp of current task is stored
        "LDR R1, [R1] \n" //Dereference. R1 = sp
        "LDR R0, [R1] \n" //R0 = *sp (saved PSP value for current task)
        "LDMIA R0!, {R4-R11} \n" //Load R4-R11 from current stack; update PSP; R0 = new PSP
        "MSR PSP, R0 \n" //PSP = current sp

         "BX LR \n"
    );
}

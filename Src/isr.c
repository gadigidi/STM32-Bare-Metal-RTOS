#include "isr.h"
#include "os.h"
#include "tim2.h"
#include "timebase.h"
//#include "user.h"
#include "stack_debug.h"
#include "stm32f446xx.h"
#include <stdint.h>

void isr_enable_interrupts(int irqn) {
    int reg = irqn / 32;
    int offset = irqn % 32;
    NVIC->ISER[reg] = (1U << offset);
}

void isr_disable_interrupts (int irqn) {
    int reg = irqn / 32;
    int offset = irqn % 32;
    NVIC->ICER[reg] = (1U << offset);
}

void isr_set_priority(int irqn, uint8_t priority){
    NVIC->IP[irqn] = (priority << 15); //NVIC uses only bits [8:4]
}


void isr_set_pendsv_priority(int priority){
    SCB->SHP[10] = (priority<<4);
}

void TIM2_IRQHandler(void) {
    timebase_increase_ms();
    TIM2->SR &= ~TIM2_SR_UIF;

    //Turn on PendSV ISR (software)
    SCB->ICSR |= PENDSVSET;
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1U << 1)) {
        EXTI->PR = (1U << 1);
    }
}

static volatile bool exti15_10_event = 0;
void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1U << 13)) {
        EXTI->PR = (1U << 13); //Clear HW flag
        exti15_10_event = 1;
        NVIC->ICER[1] = (1U << 8); //Turn off this ISR for de-baunce
    }
}

volatile bool isr_show_exti15_10_pending_status(void){
	return exti15_10_event;
}

void isr_clear_exti15_10_pending_status(void){
	exti15_10_event = 0;
}

//uint32_t debug_psp_before_stmbd;
//uint32_t debug_lr_before_switch;
//uint32_t debug_psp_before_switch;
//uint32_t debug_lr_after_switch;
//uint32_t debug_psp_after_switch;
__attribute__((naked)) void PendSV_Handler(void){

    __asm volatile (
            /*
            "MRS R0, PSP \n" //R0 = LR
            "LDR R1, =debug_psp_before_stmbd \n" //R1 = &debug
            "STR R0, [R1] \n" //
            */
            //Save context
            "MRS R0, PSP \n" //R0 = PSP
            "STMDB R0!, {R4-R11} \n" //Store R4-R11 in current stack; update PSP; R0 = new PSP
            "LDR R1, =current_tcb \n" //R1 = address where sp of current task is stored
            "LDR R1, [R1] \n" //Dereference. R1 = *sp
            "STR R0, [R1] \n" //*sp = PSP

            /*
            "MOV R0, LR \n" //R0 = LR
            "LDR R1, =debug_lr_before_switch \n" //R1 = &debug
            "STR R0, [R1] \n" //
            "MRS R0, PSP \n" //R0 = LR
            "LDR R1, =debug_psp_before_switch \n" //R1 = &debug
            "STR R0, [R1] \n" //
            */


            //Call os_switch and return to next phase
            "PUSH {R5} \n" //Save LR (EXC_RETURN) because it will be overwritten by calling BL
            "PUSH {LR} \n"
            "BL os_switch \n"
            "POP {LR} \n" //Load LR
            "POP {R5} \n"

            //Restore context
            "LDR R1, =current_tcb \n" //R1 = address where sp of current task is stored
            "LDR R1, [R1] \n" //Dereference. R1 = sp
            "LDR R0, [R1] \n" //R0 = *sp (saved PSP value for current task)
            "LDMIA R0!, {R4-R11} \n" //Load R4-R11 from current stack; update PSP; R0 = new PSP
            "MSR PSP, R0 \n" //PSP = current sp
            "ISB \n" //Make sure pipeline is loaded with correct data

            "BX LR \n"
    );
}

void HardFault_Handler (void){
    uint32_t time_now = timebase_show_ms();
    (void) time_now;
}

uint32_t debug_psp_before_first_task;
__attribute__((naked)) void SVC_Handler (void){
    __asm volatile (
            "LDR R1, =current_tcb \n" //R1 = address where sp of current task is stored
            "LDR R1, [R1] \n" //Dereference. R1 = sp
            "LDR R1, [R1] \n" //Dereference. R1 = *sp
            "MSR PSP, R1 \n"

            "MRS R0, CONTROL \n" //Load R0 with CONTROL data
            "ORR R0, #2 \n" //SPSELECT = 1
            "MSR CONTROL, R0 \n" //Store new value in CONTROL
            "ISB \n" //Make sure pipeline is loaded with correct data

            /*
            "MRS R0, PSP \n" //R0 = LR
            "LDR R1, =debug_psp_before_first_task \n" //R1 = &debug
            "STR R0, [R1] \n" //
            */

            "LDR LR, =0xFFFFFFFD \n"
            "BX LR \n"
    );
}


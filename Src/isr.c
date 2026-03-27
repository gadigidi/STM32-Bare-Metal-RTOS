#include "isr.h"
#include "rtos.h"
#include "tim2.h"
#include "timebase.h"
#include "i2c_m.h"
#include "i2c_agents.h"
#include "spi.h"
#include "spi_m.h"
#include "spi_agents.h"
#include "stack_debug.h"
#include "stm32f446xx.h"
#include <stdint.h>

void isr_enable(int irqn) {
    int reg = irqn / 32;
    int offset = irqn % 32;
    NVIC->ISER[reg] = (1U << offset);
}

void isr_disable(int irqn) {
    int reg = irqn / 32;
    int offset = irqn % 32;
    NVIC->ICER[reg] = (1U << offset);
}

void isr_set_priority(int irqn, uint8_t priority){
    NVIC->IP[irqn] = (priority << 4); //NVIC uses only bits [8:4]
}


void isr_set_pendsv_priority(int priority){
    SCB->SHP[10] = (priority<<4);
}

//////////////////////
//// ISR Handlers ////
//////////////////////
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

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1U << 13)) {
        EXTI->PR = (1U << 13); //Clear HW flag
        NVIC->ICER[1] = (1U << 8); //Turn off this ISR for de-baunce
        rtos_give_sem(&user_button_sem);
    }
}

///////////////////////////////
//////    I2C Handlers   //////
///////////////////////////////
static uint32_t i2c1_dbg_ev_counter;
void I2C1_EV_IRQHandler (void){
    i2c1_dbg_ev_counter++;
    i2c_m_driver(&i2c1_cb);
}

static uint32_t i2c1_dbg_er_counter;
void I2C1_ER_IRQHandler (void){
    i2c1_dbg_er_counter++;
    isr_disable(I2C1_EV_IRQn);
    isr_disable(I2C1_ER_IRQn);
    *i2c1_cb.master_driver_state = I2C_M_DRVR_ERROR;
    i2c_m_driver(&i2c1_cb);
}

///////////////////////////////
//////    SPI Handlers   //////
///////////////////////////////
static uint32_t spi2_counter;
static uint32_t spi2_txe_counter;
static uint32_t spi2_rxne_counter;
static uint32_t spi2_ovr_counter;
void SPI2_IRQHandler (void){
    //isr_disable(SPI2_IRQn);
    spi2_counter++;
    if ((SPI2->SR & SPI_TXE) && (SPI2->CR2 & SPI_TXEIE)){
        spi2_txe_counter++;
    }
    if ((SPI2->SR & SPI_RXNE) && (SPI2->CR2 & SPI_RXNEIE)){
        spi2_rxne_counter++;
    }
    if (SPI2->SR & SPI_OVR){
        spi2_ovr_counter++;
    }
    if (spi_check_error(spi2_cb.spi_agent)){
        *spi2_cb.spi_driver_state = SPI_M_DRVR_ERROR_HANDLER;
    }
    spi_m_txrx_driver(&spi2_cb);
}



///////////////////////////////
///////    SV Handlers   //////
///////////////////////////////

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


            //Call rtos_switch and return to next phase
            "PUSH {R5} \n" //Save LR (EXC_RETURN) because it will be overwritten by calling BL
            "PUSH {LR} \n"
            "BL rtos_switch \n"
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

//uint32_t debug_psp_before_first_task;
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


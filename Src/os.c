#include "os.h"
#include "timebase.h"
#include "stm32f446xx.h"
#include <stdint.h>


typde struct {
    uint32_t *sp;
    uint32_t data;
} stack_line_t;

static tcb_t tcb [OS_TASKS_NUM];
static uint32_t stack[OS_TASKS_NUM][OS_STACK_DEPTH];
volatile tcb_t * current_tcb;

stack_line_t * os_push_stack(stack_line_t stack_line){
    stack_line->sp -= 4;
    *stack_line->sp = stack_line->data;
    return stack_line;
}

stack_line_t * os_pop_stack(stack_line_t stack_line){
    *stack_line->sp = stack_line->data;
    stack_line->sp += 4;
    return stack_line;
}

uint32_t * stack_frame_init (uint32_t *sp, uint32_t *pc, uint32_t arg){
    stack_line_t stack_line;
    stack_line->sp = sp;

    //Equivalent to load R0 = arg
    stack_line->data = arg;
    stack_line = os_push_stack(stack_line);

    //Equivalent to load R1, R2, R3, R12with initial value 0
    for (int i = 0; i < 5; i++){
        stack_line->data = 0x0 | i | (i<<2) | (i<<4) | (i<<6);
        stack_line = os_push_stack(stack_line);
    }

    stack_line->data = &os_error_task; //LR
    stack_line = os_push_stack(stack_line);

    stack_line->data = pc; //PC
    stack_line = os_push_stack(stack_line);

    stack_line->data = 0x01000000; //xSPR
    stack_line = os_push_stack(stack_line);

    /*
    //Equivalent to load R4-R11
    stack_line->data = 0x0;
    for (int i =0; i < 8; i++){
        stack_line = os_push_stack(stack_line);
    }
    */
    return stack_line->sp;
}

void os_init(void) {

    timebase_init();

    for (int i = 0; i < OS_TASKS_NUM; i++){
        pc = (uint32_t) task_entry[i] | 1U; //Need 1 in the lsb for Thumb mode
        uint32_t * sp = &stack[i]->sp;
        uint32_t arg = (uint32_t) task_arg[i];
        sp = stack_frame_init(sp, pc);
        tcb[i]->sp = sp;
        tcb[i]->state = OS_IDLE;
        stack_frame_init();
    }
}

void os_delay(uint32_t delay) {
    tcb->delay_start = timebase_show_ms();
    tcb->delay = delay;
}

void os_scheduler (void){

}

void os_start(void) {


    current_psp = &tcb[task_index]->sp;
}

void os_error_task(void){
    while(1){
        //Dead end
    }
}


//SCB->ICSR |= PENDSVSET;

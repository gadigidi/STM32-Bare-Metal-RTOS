#include "os.h"
#include "tasks.h"
#include "timebase.h"
#include "isr.h"
#include "stm32f446xx.h"
#include <stdint.h>

#include "stack_debug.h"

typedef struct {
    uint32_t *sp;
    uint32_t data;
} stack_line_t;

//////////////////
/// prototypes ///
//////////////////
void os_error_task(void *arg);
void os_run(void);
void os_delay(uint32_t delay_ms);
void os_config_priorities(void);
//////////////////
//////////////////

static tcb_t tcb [OS_TASKS_NUM];
volatile tcb_t * current_tcb;

static uint32_t stack[OS_TASKS_NUM][OS_STACK_DEPTH];

static bool task_ready[OS_TASKS_NUM];
static int current_task;

stack_line_t * os_push_stack(stack_line_t *stack_line_ptr){
    stack_line_ptr->sp --;
    *stack_line_ptr->sp = stack_line_ptr->data;
    return stack_line_ptr;
}

stack_line_t * os_pop_stack(stack_line_t *stack_line_ptr){
    *stack_line_ptr->sp = stack_line_ptr->data;
    stack_line_ptr->sp ++;
    return stack_line_ptr;
}

uint32_t * stack_frame_init (uint32_t *sp, uint32_t *pc, uint32_t arg){
    stack_line_t *stack_line_ptr;
    stack_line_t stack_line;
    stack_line_ptr = &stack_line;

    stack_line_ptr->sp = sp;

    stack_line_ptr->data = 0x01000000; //xSPR
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = (uint32_t)pc; //PC
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = (uint32_t)& (os_error_task) | 1; //LR
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = 0x12;
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = 0x3;
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = 0x2;
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = 0x1;
    stack_line_ptr = os_push_stack(stack_line_ptr);

    //Load R0 = arg
    stack_line_ptr->data = arg;
    stack_line_ptr = os_push_stack(stack_line_ptr);

    //Load R4-R11 with data 0x4-0x11
    uint32_t data = 4;
    for (int i = 0; i < 8; i++){
        stack_line_ptr->data = data+i;
        stack_line_ptr = os_push_stack(stack_line_ptr);
    }

    return stack_line_ptr->sp;
}

void os_init(void) {

    os_config_priorities();

    for (int i = 0; i < OS_TASKS_NUM; i++){
        uint32_t pc_uint = (uint32_t)task_entry[i] | 1U;
        uint32_t *pc = (uint32_t *) pc_uint;
        uint32_t *sp = &stack[i][OS_STACK_DEPTH-1];
        uint32_t sp_uint = (uint32_t) sp & ~(7U); //Align sp to 8
        sp = (uint32_t *)sp_uint;
        uint32_t arg = (uint32_t) task_arg[i];
        sp = stack_frame_init(sp, pc, arg);
        tcb[i].sp = sp;
        tcb[i].state = OS_READY;
        task_ready[i] = 1;

        stack_debug(sp);
    }
}

void os_config_priorities(void){
    isr_set_priority(TIM2_IRQn, 2); //TIM2 Systick
    isr_set_pendsv_priority(15);
}

void os_delay(uint32_t delay_ms) {
    current_tcb->delay_start = timebase_show_ms();
    current_tcb->delay_ms = delay_ms;
    current_tcb->state = OS_SLEEP;
}

uint32_t debug_psp_before_stmbd;
uint32_t debug_lr_before_switch;
uint32_t debug_psp_before_switch;
uint32_t debug_lr_after_switch;
uint32_t debug_psp_after_switch;
void os_switch (void){
    uint32_t time_now = timebase_show_ms();

    for (int i = 0; i < OS_TASKS_NUM; i++){
        if (tcb[i].state == OS_SLEEP){
            if ((time_now - tcb[i].delay_start) >= tcb[i].delay_ms){
                tcb[i].state = OS_READY;
                task_ready[i] = 1;
            }
        } else{ //if task is currently running or ready
            tcb[i].state = OS_READY;
        }
    }
    //bool switch_task = 0;
    for (int i = 0; i < OS_TASKS_NUM; i++){
        int next_task = (current_task + i + 1) % OS_TASKS_NUM;
        if (task_ready[next_task] == 1){
            //tcb[current_task].state = OS_;
            current_tcb = &tcb[next_task];
uint32_t *current_sp = tcb[current_task].sp;
stack_debug(current_sp);
            current_task = next_task;
            tcb[current_task].state = OS_RUN;
            break;
        }
        //else{
            //current_task = next_task;
        //}
    }
}

//uint32_t debug_psp_before_first_task;
void os_run(void) {
    tcb[OS_FIRST_TASK].sp += 32; //Remove SW frame from first task
    current_tcb = &tcb[OS_FIRST_TASK];
    current_tcb->state = OS_RUN;
    timebase_init();
    __asm volatile (
            "SVC 0 \n"
    );

    //SCB->ICSR |= PENDSVSET; //Trigger PendSV to start context switch flow
}

void os_error_task(void *arg){
    while(1){
        //Dead end
    }
}


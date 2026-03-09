#include "os.h"
#include "tasks.h"
#include "timebase.h"
#include "isr.h"
#include "lfsr_simple.h"
#include "buffer.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stddef.h>
#include "stack_debug.h"

///////////////////
///  typdefs    ///
///////////////////
typedef struct {
    uint32_t *sp;
    uint32_t data;
} stack_line_t;

//////////////////
/// semaphores ///
//////////////////
semaphore_t user_button_sem;
semaphore_t i2c_master_done_sem;


//////////////////
/// variables ////
//////////////////
static tcb_t tcb[OS_TASKS_NUM];
volatile tcb_t *current_tcb;

static uint32_t stack[USER_TASKS_NUM][OS_STACK_DEPTH]; //Idle task have deifferent stack depth
static uint32_t idle_stack[OS_IDLE_STACK_DEPTH];
static ring_buf_t prioritize_task_ready[8];
static int current_task;

//////////////////
/// functions ////
//////////////////
stack_line_t* os_push_stack(stack_line_t *stack_line_ptr) {
    stack_line_ptr->sp--;
    *stack_line_ptr->sp = stack_line_ptr->data;
    return stack_line_ptr;
}

stack_line_t* os_pop_stack(stack_line_t *stack_line_ptr) {
    *stack_line_ptr->sp = stack_line_ptr->data;
    stack_line_ptr->sp++;
    return stack_line_ptr;
}

uint32_t* stack_frame_init(uint32_t *sp, uint32_t *pc, uint32_t arg) {
    stack_line_t *stack_line_ptr;
    stack_line_t stack_line;
    stack_line_ptr = &stack_line;

    stack_line_ptr->sp = sp;

    stack_line_ptr->data = 0x01000000; //xSPR
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = (uint32_t) pc; //PC
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = (uint32_t) &(os_error_task) | 1; //LR
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = 0xc; //R12
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = 0x3; //R3
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = 0x2; //R2
    stack_line_ptr = os_push_stack(stack_line_ptr);

    stack_line_ptr->data = 0x1; //R1
    stack_line_ptr = os_push_stack(stack_line_ptr);

    //Load R0 = arg
    stack_line_ptr->data = arg;
    stack_line_ptr = os_push_stack(stack_line_ptr);

    //Load R4-R11 with data 4-11 (reverse order, R11 first)
    uint32_t data = 11;
    for (int i = 0; i < 8; i++) {
        stack_line_ptr->data = data - i;
        stack_line_ptr = os_push_stack(stack_line_ptr);
    }

    return stack_line_ptr->sp;
}

void os_set_isr_priorities(void) {
    isr_set_priority(TIM2_IRQn, 2); //TIM2 Systick
    isr_set_pendsv_priority(15);
}

void os_init(void) {
    os_set_isr_priorities();

    //Initialize priority tables and waiting lists in semaphores
    for (int pri = 0; pri < OS_NUM_PRIORITIES; pri++){
        buf_init(&prioritize_task_ready[pri]); //Initialize task ready list
        buf_init(&user_button_sem.prioritize_waiting_list[pri]); //Initialize waiting list in semaphore
        buf_init(&i2c_master_done_sem.prioritize_waiting_list[pri]); //Initialize waiting list in semaphore
    }

    for (int id = 0; id < OS_TASKS_NUM; id++) {
        tcb[id].id = id;

        uint32_t pc_uint = (id < OS_IDLE_TASK) ? (uint32_t) task_entry[id] : (uint32_t) &os_idle_task;
        pc_uint |= 1; //Thumb bit
        uint32_t *pc = (uint32_t*) pc_uint;

        uint32_t *sp = (id < OS_IDLE_TASK) ? &stack[id][OS_STACK_DEPTH - 1] : &idle_stack[OS_STACK_DEPTH - 1];
        uint32_t sp_uint = (uint32_t) sp & ~(7U); //Align sp to 8
        sp = (uint32_t*) sp_uint;

        //Load R0 = arg
        uint32_t arg;
        if (id < 3){
            arg = task_arg[id];
        }
        else{
            arg = (uint32_t) sp; //To help debug stacks for stub tasks
        }
        //uint32_t arg = (uint32_t) task_arg[id];
        sp = stack_frame_init(sp, pc, arg);

        int pri = id % 8;
        tcb[id].sp = sp;
        tcb[id].state = TASK_READY;
        tcb[id].base_pri = pri;
        tcb[id].pri = pri;
        tcb[id].sem = NULL;
        tcb[id].mutex = NULL;

        if ((id != OS_FIRST_TASK) && (id != OS_IDLE_TASK)){
            //First task will start run automatically, not through the ready list
            //Idle task don't need to be queued
            bool success = push_buf(&prioritize_task_ready[pri], id);
            if (success){
                tcb[id].state = TASK_QUEUED;
            }
        }

        //stack_debug(sp);
    }
}

void os_delay(uint32_t delay_ms) {
    current_tcb->delay_start = timebase_show_ms();
    current_tcb->delay_ms = delay_ms;
    current_tcb->state = TASK_SLEEP;

    SCB->ICSR |= PENDSVSET; //Assert PendSV
}

bool os_wait_sem(semaphore_t *semaphore) {
    static bool need_resched = 0;
    bool success;
    __disable_irq();
    if (semaphore->count > 0) {
        semaphore->count--;
        return 1;
    }
    else { //No semaphore pending
        success = push_buf(&semaphore->prioritize_waiting_list[current_tcb->pri], current_tcb->id);
        if (success){
            current_tcb->sem = semaphore;
            current_tcb->state = TASK_WAIT;
            need_resched = 1;
        }
    }
    if (need_resched) {
        SCB->ICSR |= PENDSVSET; //Assert PendSV
    }
    __enable_irq();
    return success; //Will return 1 if task is successfully on the waiting list, or 0 if not
}

void os_give_sem(semaphore_t *semaphore) {
    static bool need_resched = 0;
    __disable_irq();
    for (int pri = 7; pri >= 0; pri--){
        uint8_t id = pop_buf(&semaphore->prioritize_waiting_list[pri]);
        if (id != BUF_EMPTY){
            bool success = push_buf(&prioritize_task_ready[pri], id);
            if (success){
                tcb[id].sem = NULL;
                tcb[id].state = TASK_QUEUED;
                need_resched = 1;
                break;
            }
        }
    }
    if (!need_resched){ //Empty wait list. No task consumed semaphore
        semaphore->count++;
    }
    __enable_irq();

    if (need_resched) {
        SCB->ICSR |= PENDSVSET; //Assert PendSV
    }
}

void os_switch(void) {
    uint32_t time_now = timebase_show_ms();
    if (tcb[current_task].state == TASK_RUN){
        tcb[current_task].state = TASK_READY;
    }
    for (int id = 0; id < USER_TASKS_NUM; id++) {
        bool success;
        if (tcb[id].state == TASK_SLEEP) { //If task is in delay state
            if ((time_now - tcb[id].delay_start) >= tcb[id].delay_ms) {
                tcb[id].state = TASK_READY;
            }
        }
        if (tcb[id].state == TASK_READY){
            success = push_buf(&prioritize_task_ready[tcb[id].pri], id);
            if (success){
                tcb[id].state = TASK_QUEUED;
            }
        }
    }

    //Find the highest priority ready task
    bool user_task_run = 0;
    for (int pri = 7; pri >= 0; pri--){
        uint8_t next_task = pop_buf(&prioritize_task_ready[pri]);
        if (next_task != BUF_EMPTY){
            current_tcb = &tcb[next_task];
            current_task = next_task;
            tcb[current_task].state = TASK_RUN;
            user_task_run = 1;
            break;
        }
    }
    if (!user_task_run) {
        current_tcb = &tcb[OS_IDLE_TASK]; //Run idle task if no other task ready
        current_task = OS_IDLE_TASK;
        tcb[current_task].state = TASK_RUN;
    }
}

void os_start(void) {
    tcb[OS_FIRST_TASK].sp += 8; //Remove SW frame from first task
    current_tcb = &tcb[OS_FIRST_TASK];
    current_tcb->state = TASK_RUN;
    timebase_init();
    __asm volatile (
            "SVC 0 \n"
    );
}


#include "os.h"
#include "tasks.h"
#include "timebase.h"
#include "isr.h"
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
semaphore_t i2c_slave_done_sem;


//////////////////
/// variables ////
//////////////////
static tcb_t tcb[OS_TASKS_NUM];
volatile tcb_t *current_tcb;

static uint32_t stack[USER_TASKS_NUM][OS_STACK_DEPTH]; //Idle task have deifferent stack depth
static uint32_t idle_stack[OS_IDLE_STACK_DEPTH];
static bool task_ready[USER_TASKS_NUM];
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

void os_config_priorities(void) {
    isr_set_priority(TIM2_IRQn, 2); //TIM2 Systick
    isr_set_pendsv_priority(15);
}

void os_init(void) {
    os_config_priorities();

    for (int i = 0; i < OS_TASKS_NUM; i++) {
        tcb[i].index = i;

        uint32_t pc_uint =
                (i < OS_IDLE_TASK) ?
                        (uint32_t) task_entry[i] : (uint32_t) &os_idle_task;
        pc_uint |= 1; //Thumb bit
        uint32_t *pc = (uint32_t*) pc_uint;

        uint32_t *sp =
                (i < OS_IDLE_TASK) ?
                        &stack[i][OS_STACK_DEPTH - 1] :
                        &idle_stack[OS_STACK_DEPTH - 1];
        uint32_t sp_uint = (uint32_t) sp & ~(7U); //Align sp to 8
        sp = (uint32_t*) sp_uint;

        uint32_t arg = (uint32_t) task_arg[i];
        sp = stack_frame_init(sp, pc, arg);

        tcb[i].sp = sp;
        tcb[i].state = OS_READY;
        tcb[i].sem = NULL;
        task_ready[i] = 1;
        //stack_debug(sp);
    }
}

void os_delay(uint32_t delay_ms) {
    current_tcb->delay_start = timebase_show_ms();
    current_tcb->delay_ms = delay_ms;
    current_tcb->state = OS_SLEEP;

    SCB->ICSR |= PENDSVSET; //Assert PendSV
}

void os_wait_sem(semaphore_t *semaphore) {
    static bool need_resched = 0;
    __disable_irq();
    if (semaphore->count > 0) {
        if (!(semaphore->sem_tasks_list)) { //If this task is only consumer of this semaphore
            semaphore->count--;
        }
    } else { //No semaphore pending
        semaphore->sem_tasks_list |= (1 << (current_tcb->index));
        current_tcb->sem = semaphore;
        current_tcb->state = OS_WAIT;
        need_resched = 1;
    }
    __enable_irq();

    if (need_resched) {
        SCB->ICSR |= PENDSVSET; //Assert PendSV
    }
}

void os_sem_update(semaphore_t *semaphore) {
    semaphore->count++;

    SCB->ICSR |= PENDSVSET; //Assert PendSV
}

void os_switch(void) {
    uint32_t time_now = timebase_show_ms();

    for (int i = 0; i < USER_TASKS_NUM; i++) {
        if (tcb[i].state == OS_SLEEP) { //If task is in delay state
            if ((time_now - tcb[i].delay_start) >= tcb[i].delay_ms) {
                tcb[i].state = OS_READY;
                task_ready[i] = 1;
            } else { //If time_delay didn't passed yet
                task_ready[i] = 0;
            }
        } else if (tcb[i].state == OS_WAIT) { //If task is in wait for semaphore state
            if ((tcb[i].sem->count) > 0) {
                if (!(tcb[i].sem->sem_tasks_list &= ~(1 << i))) { //If this task is only consumer
                    tcb[i].sem->count--;
                }
                tcb[i].sem->sem_tasks_list &= ~(1 << i); //Remove task from semaphore list
                tcb[i].state = OS_READY;
                task_ready[i] = 1;
            } else { //If no pending semaphore for this task
                task_ready[i] = 0;
            }
        }
    }

    bool user_task_run = 0;
    for (int i = 0; i < USER_TASKS_NUM; i++) {
        int next_task = (current_task + i + 1) % USER_TASKS_NUM;
        if (task_ready[next_task] == 1) {
            current_tcb = &tcb[next_task];
            current_task = next_task;
            tcb[current_task].state = OS_RUN;
            user_task_run = 1;
            break;
        }
    }
    if (!user_task_run) {
        current_tcb = &tcb[OS_IDLE_TASK]; //Run idle task if no other task ready
        current_task = OS_IDLE_TASK;
        tcb[current_task].state = OS_RUN;
    }
}

void os_run(void) {
    tcb[OS_FIRST_TASK].sp += 8; //Remove SW frame from first task
    current_tcb = &tcb[OS_FIRST_TASK];
    current_tcb->state = OS_RUN;
    timebase_init();
    __asm volatile (
            "SVC 0 \n"
    );
}


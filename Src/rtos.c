#include "rtos.h"
#include "tasks.h"
#include "timebase.h"
#include "isr.h"
#include "lfsr_simple.h"
#include "buffer.h"
#include "list.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stddef.h>
#include "stack_debug.h"
#include "dwt.h"

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
//NOTE: Each semaphore need to be initialize in rtos_init
//Also need to be declared as extern in rtos.h
semaphore_t user_button_sem;
semaphore_t i2c1_done_sem;
semaphore_t spi2_done_sem;

///////////////
/// mutexes ///
///////////////
//NOTE: Each mutex need to be initialize in rtos_init
//Also need to be declared as extern in rtos_
mutex_t spi2_mutex;

/////////////
/// lists ///
/////////////
static node_t nodes[RTOS_TASKS_NUM];
static list_t sleeping_list;
static list_t prioritize_ready_lists[RTOS_NUM_PRIORITIES];

//////////////////
/// variables ////
//////////////////
//uint32_t counter[RTOS_TASKS_NUM];
static tcb_t tcb[RTOS_TASKS_NUM];
volatile tcb_t *current_tcb;

static uint32_t stack[USER_TASKS_NUM][RTOS_STACK_DEPTH];
static uint32_t idle_stack[RTOS_IDLE_STACK_DEPTH]; //Idle task have different stack depth
//static ring_buf_t prioritize_ready_lists[8];

static int current_task;

//////////////////
/// functions ////
//////////////////


stack_line_t* push_stack(stack_line_t *stack_line_ptr) {
    stack_line_ptr->sp--;
    *stack_line_ptr->sp = stack_line_ptr->data;
    return stack_line_ptr;
}

stack_line_t* pop_stack(stack_line_t *stack_line_ptr) {
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
    stack_line_ptr = push_stack(stack_line_ptr);

    stack_line_ptr->data = (uint32_t) pc; //PC
    stack_line_ptr = push_stack(stack_line_ptr);

    stack_line_ptr->data = (uint32_t) &(rtos_error_task) | 1; //LR
    stack_line_ptr = push_stack(stack_line_ptr);

    stack_line_ptr->data = 0xc; //R12
    stack_line_ptr = push_stack(stack_line_ptr);

    stack_line_ptr->data = 0x3; //R3
    stack_line_ptr = push_stack(stack_line_ptr);

    stack_line_ptr->data = 0x2; //R2
    stack_line_ptr = push_stack(stack_line_ptr);

    stack_line_ptr->data = 0x1; //R1
    stack_line_ptr = push_stack(stack_line_ptr);

    //Load R0 = arg
    stack_line_ptr->data = arg;
    stack_line_ptr = push_stack(stack_line_ptr);

    //Load R4-R11 with data 4-11 (reverse order, R11 first)
    uint32_t data = 11;
    for (int i = 0; i < 8; i++) {
        stack_line_ptr->data = data - i;
        stack_line_ptr = push_stack(stack_line_ptr);
    }

    return stack_line_ptr->sp;
}

void rtos_set_isr_priorities(void) {
    isr_set_priority(TIM2_IRQn, 2); //TIM2 Systick
    isr_set_pendsv_priority(15);
}

void rtos_init(void) {
    rtos_set_isr_priorities();

    //Each linked-list need to be initialized here
    create_list(&sleeping_list);

    //Initialize priority linked-lists and waiting lists in semaphores
    for (int pri = 0; pri < RTOS_NUM_PRIORITIES; pri++){
        create_list(&prioritize_ready_lists[pri]);

        //Each semaphore need to be initialize here
        buf_init(&user_button_sem.prioritize_waiting_list[pri]); //Initialize waiting list in semaphore
        buf_init(&i2c1_done_sem.prioritize_waiting_list[pri]); //Initialize waiting list in semaphore
        buf_init(&spi2_done_sem.prioritize_waiting_list[pri]); //Initialize waiting list in semaphore
    }

    //Each semaphore init_count and max_count need to be initialize here
    rtos_sem_init(&user_button_sem, 0, 1); //Binary semaphore
    rtos_sem_init(&i2c1_done_sem, 0, 1); //Binary semaphore
    rtos_sem_init(&spi2_done_sem, 0, 1); //Binary semaphore

    //Each mutex lock state need to be initialize here
    rtos_mutex_init(&spi2_mutex);

    for (int id = 0; id < RTOS_TASKS_NUM; id++) {
        tcb[id].id = id;

        uint32_t pc_uint = (id < RTOS_IDLE_TASK) ? (uint32_t) task_entry[id] : (uint32_t) &rtos_idle_task;
        pc_uint |= 1; //Thumb bit
        uint32_t *pc = (uint32_t*) pc_uint;

        uint32_t *sp = (id < RTOS_IDLE_TASK) ? &stack[id][RTOS_STACK_DEPTH - 1] : &idle_stack[RTOS_STACK_DEPTH - 1];
        uint32_t sp_uint = (uint32_t) sp & ~(7U); //Align sp to 8
        sp = (uint32_t*) sp_uint;

        //Load R0 = arg
        uint32_t arg = (uint32_t) task_arg[id];
        sp = stack_frame_init(sp, pc, arg);

        int pri = task_pri[id];
        tcb[id].sp = sp;
        tcb[id].state = TASK_READY;
        tcb[id].base_pri = pri;
        tcb[id].pri = pri;
        tcb[id].sem = NULL;
        tcb[id].mutex = NULL;

        tcb[id].node = &nodes[id]; //Assign static node to node pointer in tcb
        list_node_init(tcb[id].node); //Initialize node for each tcb

        if ((id != RTOS_FIRST_TASK) && (id != RTOS_IDLE_TASK)){
            //First task will start run automatically, not through the ready list
            //Idle task don't need to be queued
            list_add_node(&prioritize_ready_lists[pri], tcb[id].node, id, READY_LIST);
        }

        stack_debug(sp);
    }
}

void rtos_delay(uint32_t delay_ms) {
    __disable_irq();
    current_tcb->delay_start = timebase_show_ms();
    current_tcb->delay_ms = delay_ms;
    current_tcb->state = TASK_SLEEP;
    uint8_t id = current_tcb->id;
    uint8_t pri = current_tcb->pri;
    list_remove_node(&prioritize_ready_lists[pri], tcb[id].node);
    list_add_node(&sleeping_list, tcb[id].node, id, SLEEPING_LIST); //Add task to sleeping linked list
    __enable_irq();

    SCB->ICSR |= PENDSVSET; //Assert PendSV
}

void rtos_sem_init(semaphore_t *sem, uint8_t init_count, uint8_t max_count){
    sem->max_count = max_count;
    sem->count = init_count;
}

bool rtos_take_sem(semaphore_t *sem) {
    static bool need_resched = 0;
    bool success;
    __disable_irq();
    if (sem->count > 0) {
        sem->count--;
        return 1;
    }
    else { //No semaphore pending
        success = push_buf(&sem->prioritize_waiting_list[current_tcb->pri], current_tcb->id);
        if (success){
            current_tcb->state = TASK_WAIT;
            current_tcb->sem = sem;
            need_resched = 1;
        }
    }
    if (need_resched) {
        SCB->ICSR |= PENDSVSET; //Assert PendSV
    }
    __enable_irq();
    return success; //Will return 1 if task is successfully on the waiting list, or 0 if not
}

bool rtos_give_sem(semaphore_t *sem) {
    static bool need_resched = 0;
    if (sem->count == sem->max_count){ //
        return 0;
    }
    __disable_irq();
    for (int pri = 7; pri >= 0; pri--){
        uint8_t id = pop_buf(&sem->prioritize_waiting_list[pri]);
        if (id != BUF_EMPTY){
            tcb[id].state = TASK_READY;
            list_add_node(&prioritize_ready_lists[pri], tcb[id].node, id, READY_LIST);
            tcb[id].mutex = NULL;
            need_resched = 1;
            break;
        }
    }
    if (!need_resched){ //Empty wait list. No task consumed semaphore
        sem->count++;
    }
    __enable_irq();
    uint32_t time_now = timebase_show_ms(); //Just for debug
    if (need_resched) {
        SCB->ICSR |= PENDSVSET; //Assert PendSV
        return 1;
    }
    return 0;
}

void rtos_mutex_init(mutex_t *mutex){
    mutex->locked = 0;
}

bool rtos_mutex_try_lock(mutex_t *mutex){
    bool success = 0;
    __disable_irq();
    if (!mutex->locked){
        mutex->locked = 1;
        mutex->owner_id = current_tcb->id;
        current_tcb->mutex = mutex;
        success = 1;
    }
    else{
        success = 0;
    }
    __enable_irq();
    return success;
}

void rtos_mutex_unlock (mutex_t *mutex){
    __disable_irq();
    mutex->locked = 0;
    current_tcb->mutex = NULL;
    __enable_irq();
}

uint32_t switch_debug = 0;
uint32_t t0, t1, t2;
void rtos_switch(void) {
    static uint32_t switch_start, switch_middle, switch_finish, cycle_count1, cycle_count2;
    uint32_t time_now = timebase_show_ms();
    uint8_t current_pri = tcb[current_task].pri;
    __disable_irq();
    switch_start = dwt_count();
    if (tcb[current_task].state == TASK_RUN){
        tcb[current_task].state = TASK_READY;
        if (current_task != RTOS_IDLE_TASK){
            t0 = dwt_count();
            list_add_node(&prioritize_ready_lists[current_pri], tcb[current_task].node, current_task, READY_LIST);
            t1 = dwt_count();
            t2 = t1 - t0;
        }
    }
    else{
        switch_debug++;
    }

    node_t *node = sleeping_list.head;
    int i = 0;
    int size = sleeping_list.size;
    while (i < size){ //Run on the sleeping linked-list
        uint8_t id = node->data;
        uint8_t pri = tcb[id].pri;
        node_t *next_node = node->next; //This must set before the if because node->next can be initialized
        if ((time_now - tcb[id].delay_start) >= tcb[id].delay_ms) {
            if (node == NULL) {
                int breakpoint = 0;
                breakpoint++;
            }
            uint8_t data = list_remove_node(&sleeping_list, tcb[id].node);
            (void) data;
            tcb[id].state = TASK_READY;
            list_add_node(&prioritize_ready_lists[pri], tcb[id].node, id, READY_LIST);
        }
        node = next_node;
        i++;
    }

    switch_middle = dwt_count();
    cycle_count1 = switch_middle - switch_start;
    //Find the highest priority ready task
    bool user_task_run = 0;
    for (int pri = RTOS_NUM_PRIORITIES-1; pri >= 0; pri--){
        if (prioritize_ready_lists[pri].size > 0){
            t0 = dwt_count();
            uint8_t next_task = list_remove_node(&prioritize_ready_lists[pri], prioritize_ready_lists[pri].head);
            t1 = dwt_count();
            t2 = t1 - t0;
            //if (next_task != BUF_EMPTY){
            current_task = next_task;
            current_tcb = &tcb[current_task];
            tcb[current_task].state = TASK_RUN;
            user_task_run = 1;
            break;
        }
    }
    if (!user_task_run) {
        current_tcb = &tcb[RTOS_IDLE_TASK]; //Run idle task if no other task ready
        current_task = RTOS_IDLE_TASK;
        tcb[current_task].state = TASK_RUN;
    }
    switch_finish = dwt_count();
    cycle_count2 = switch_finish - switch_middle;
    __enable_irq();
    rtos_update_counter(current_task);
}

void rtos_start(void) {
    tcb[RTOS_FIRST_TASK].sp += 8; //Remove SW frame from first task
    current_tcb = &tcb[RTOS_FIRST_TASK];
    current_tcb->state = TASK_RUN;
    timebase_init();
    __asm volatile (
            "SVC 0 \n"
    );
}

//Use for debug fairness and starvation
void rtos_update_counter (int id){
    static uint32_t time_now = 0;
    static uint32_t num_switches = 0;
    static uint32_t rtos_counter[RTOS_TASKS_NUM];
    time_now = timebase_show_ms();
    if (time_now < RTOS_SAMPLE_TIME_MS){
        num_switches++;
        rtos_counter[id]++;
    }

    int breakpoint;
    (void) breakpoint;
}



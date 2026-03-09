#ifndef OS_H_
#define OS_H_

#include "tasks.h"
#include <stdint.h>
#include <stdbool.h>
#include "buffer.h"

#define OS_TASKS_NUM            USER_TASKS_NUM + 1
#define OS_IDLE_TASK            USER_TASKS_NUM //Idle task always last
#define OS_FIRST_TASK           FIRST_TASK

#define OS_STACK_DEPTH          256
#define OS_IDLE_STACK_DEPTH     64

#define OS_NUM_PRIORITIES       8

typedef enum {
    TASK_READY, TASK_QUEUED, TASK_RUN, TASK_SLEEP, TASK_WAIT,
} os_state_t;

typedef struct {
    volatile uint8_t count;
    ring_buf_t prioritize_waiting_list[OS_NUM_PRIORITIES];
} semaphore_t;

typedef struct {
    volatile uint8_t locked;
    volatile uint8_t owner_id;
} mutex_t;

typedef struct {
    uint32_t *sp; //Must be kept at first field of struct
    uint8_t id;
    os_state_t state;
    uint32_t delay_start;
    uint32_t delay_ms;
    volatile uint8_t base_pri;
    volatile uint8_t pri;
    semaphore_t *sem;
    mutex_t *mutex;
} tcb_t;



extern volatile tcb_t *current_tcb;
//extern volatile tcb[OS_TASKS_NUM];

//////////////////
/// semaphores ///
//////////////////
extern semaphore_t user_button_sem;
extern semaphore_t i2c_master_done_sem;

//////////////////

#define PENDSVCLR               (1U << 27)
#define PENDSVSET               (1U << 28)

void os_init(void);
void os_delay(uint32_t delay_ms);
void os_start(void);
bool os_wait_sem(semaphore_t *semaphore);
void os_give_sem(semaphore_t *semaphore);

#endif /* OS_H_ */

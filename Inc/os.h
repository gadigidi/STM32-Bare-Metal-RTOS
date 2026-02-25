#ifndef OS_H_
#define OS_H_

#include "tasks.h"
#include <stdint.h>

typedef enum {
    OS_READY, OS_RUN, OS_SLEEP, OS_WAIT,
} os_state_t;

typedef struct {
    volatile uint8_t count;
    volatile uint32_t sem_tasks_list;
} semaphore_t;

typedef struct {
    uint32_t *sp; //Must be kept at first field of struct
    uint8_t index;
    os_state_t state;
    uint32_t delay_start;
    uint32_t delay_ms;
    semaphore_t *sem;
} tcb_t;

typedef struct {
    uint8_t *buf;
    uint16_t size;
    volatile uint16_t head;
    volatile uint16_t tail;
} ring_buf_t;

#define OS_TASKS_NUM            USER_TASKS_NUM + 1
#define OS_IDLE_TASK            USER_TASKS_NUM //Idle task always last
#define OS_FIRST_TASK           FIRST_TASK

#define OS_STACK_DEPTH          256
#define OS_IDLE_STACK_DEPTH     64

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
void os_run(void);
void os_wait_sem(semaphore_t *semaphore);
void os_give_sem(semaphore_t *semaphore);

#endif /* OS_H_ */

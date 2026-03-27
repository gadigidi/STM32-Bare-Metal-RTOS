#ifndef RTOS_H_
#define RTOS_H_

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

#define OS_SAMPLE_TIME_MS       30000

typedef enum {
    TASK_READY, TASK_QUEUED, TASK_RUN, TASK_SLEEP, TASK_WAIT,
} rtos_state_t;

typedef struct {
    volatile uint8_t count;
    uint8_t max_count;
    ring_buf_t prioritize_waiting_list[OS_NUM_PRIORITIES];
} semaphore_t;

typedef struct {
    volatile uint8_t locked;
    volatile uint8_t owner_id;
} mutex_t;

typedef struct {
    uint32_t *sp; //WARNING: Must be kept at first field of this struct
    uint8_t id;
    rtos_state_t state;
    uint32_t delay_start;
    uint32_t delay_ms;
    volatile uint8_t base_pri; //For future use priority inheritance
    volatile uint8_t pri; //Actual priority
    semaphore_t *sem;
    mutex_t *mutex;
} tcb_t;


///////////////////////////
/// extern declerations ///
///////////////////////////
extern volatile tcb_t *current_tcb;

extern uint32_t counter[OS_TASKS_NUM];
//semaphores
extern semaphore_t user_button_sem;
extern semaphore_t i2c1_done_sem;
extern semaphore_t spi2_done_sem;
//mutexes
extern mutex_t spi2_mutex;
///////////////////////////
///////////////////////////


#define PENDSVCLR               (1U << 27)
#define PENDSVSET               (1U << 28)

void rtos_init(void);
void rtos_delay(uint32_t delay_ms);
void rtos_start(void);

void rtos_sem_init(semaphore_t *sem, uint8_t init_count, uint8_t max_count);
bool rtos_take_sem(semaphore_t *sem);
bool rtos_give_sem(semaphore_t *sem);

void rtos_mutex_init(mutex_t *mutex);
bool rtos_mutex_try_lock(mutex_t *mutex);
void rtos_mutex_unlock (mutex_t *mutex);

void rtos_update_counter(int id);

#endif /* RTOS_H_ */

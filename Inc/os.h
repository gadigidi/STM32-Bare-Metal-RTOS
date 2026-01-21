#ifndef OS_H_
#define OS_H_

#include "tasks.h"
#include <stdint.h>

typedef enum{
    OS_READY,
    OS_RUN,
    OS_SLEEP,
} os_state_t;

typedef struct{
    uint32_t * sp;
    os_state_t state;
    uint32_t delay_start;
    uint8_t delay_ms;
} tcb_t;

#define OS_TASKS_NUM            TASKS_NUM
#define OS_FIRST_TASK           FIRST_TASK
#define OS_STACK_DEPTH          256

#define PENDSVCLR               (1U << 27)
#define PENDSVSET               (1U << 28)

void os_init(void);

void os_delay(uint32_t delay_ms);

void os_run(void);



#endif /* OS_H_ */

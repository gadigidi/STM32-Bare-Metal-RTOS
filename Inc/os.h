#ifndef OS_H_
#define OS_H_

#include "tasks.h"
#include <stdint.h>

typedef struct{
    uint32_t * sp;
    uint8_t state;
    uint32_t delay_start;
    uint8_t delay_ms;
} tcb_t;

typdef enum{
    OS_IDLE,
    OS_RUN
};

#define OS_TASKS_NUM            TASKS_NUM
#define OS_STACK_DEPTH          256



void os_init(void);

void os_delay(void);

void os_start(void);



#endif /* OS_H_ */

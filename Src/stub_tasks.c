#include "stub_tasks.h"
#include "rtos.h"
#include "timebase.h"
#include "lfsr_simple.h"
#include "stack_debug.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stddef.h>


void stub_task_0(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 0. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_1(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 1. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_2(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 2. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_3(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 3. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_4(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 4. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_5(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 5. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_6(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 6. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_7(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 7. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_8(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 0. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_9(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 1. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_10(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 2. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_11(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 3. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_12(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 4. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_13(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 5. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_14(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 6. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_15(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: 7. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

#include "rtos.h"
#include "stub_tasks.h"
#include "timebase.h"
#include "lfsr_simple.h"
#include "stack_debug.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stddef.h>


void stub_task_4(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 4;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%6)+1; //delay between 1-6ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_5(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 5;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%5)+1; //delay between 1-5ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_6(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 6;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%4)+1; //delay between 1-4ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_7(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 7;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%3)+1; //delay between 1-3ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_8(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 0;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%10)+1; //delay between 1-10ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_9(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 1;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%9)+1; //delay between 1-9ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_10(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 2;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%8)+1; //delay between 1-8ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_11(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 3;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%7)+1; //delay between 1-7ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_12(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 4;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%6)+1; //delay between 1-6ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_13(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 5;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%5)+1; //delay between 1-5ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_14(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 6;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%4)+1; //delay between 1-4ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_15(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 7;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%3)+1; //delay between 1-3ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

void stub_task_16(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = 0;
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%10)+1; //delay between 1-10ms
        //rtos_update_counter(current_tcb->id);
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

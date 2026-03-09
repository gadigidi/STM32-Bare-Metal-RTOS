#include "timebase.h"
#include "os.h"
#include "lfsr_simple.h"
#include "stack_debug.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stddef.h>

void do_nothing (uint32_t time_now, uint32_t counter){

}

void stub_task_0(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_1(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_2(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_3(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(0x200002388); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_4(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(0x20002388); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_5(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_6(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_7(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_8(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(0x200002388); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_9(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_10(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_11(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

void stub_task_12(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}












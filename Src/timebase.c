#include "timebase.h"
#include "tim2.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>

void timebase_init(void) {
    tim2_init();

    tim2_enable_interrupt();

    tim2_enable();
}

static volatile uint32_t tim2_ms_cnt = 0;
void timebase_increase_ms(void) {
    tim2_ms_cnt++;
}

uint32_t timebase_show_ms(void) {
    return tim2_ms_cnt;
}

/*
void timebase_blocking_delay_ms(uint32_t delay_ms) {
    uint32_t t_start = timebase_show_ms();
    uint32_t t_now = timebase_show_ms();
    while ((t_now - t_start) < delay_ms) {
        t_now = timebase_show_ms();
    }
}
*/

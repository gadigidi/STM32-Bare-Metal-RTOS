#ifndef TIMBASE_H_
#define TIMBASE_H_

#include <stdint.h>
#include <stdbool.h>

void timebase_init(void);
void timebase_increase_ms(void);
uint32_t timebase_show_ms(void);
void timebase_blocking_delay_ms(uint32_t delay_ms);

#endif /* TIMBASE_H_ */

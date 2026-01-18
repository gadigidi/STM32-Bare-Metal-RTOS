#ifndef TIM2_H_
#define TIM2_H_

#include <stdint.h>

#define TIM2CLK_HZ          16000000U
#define PSC_VAL             (TIM2CLK_HZ/1000000)-1
#define ARR_VAL             1000U-1

#define TIM2_CLK_EN         (1U<<0)
#define TIM2_CEN            (1U<<0)
#define TIM2_SR_UIF         (1U<<0)

#define TIM2_UIEN            (1U<<0)

void tim2_init(void);
void tim2_enable(void);
void tim2_enable_interrupt(void);
void tim2_wait_tick(void);


#endif /* TIM2_H_ */

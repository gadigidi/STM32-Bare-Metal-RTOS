#include "user.h"
#include "os.h"
#include "gpio.h"
#include "exti.h"
#include "timebase.h"
#include "isr.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>

void user_init(void) {
    gpio_init(GPIOAEN);
    gpio_init(GPIOCEN);

    // PA5 as OUTPUT
    gpio_pin_mode(GPIOA, 5, GPIO_MODE_OUTPUT);

    // PC13 as INPUT
    gpio_pin_mode(GPIOC, 13, GPIO_MODE_INPUT);

    //Set EXTI13 for GPIOC
    exti_init();
    exti_enable_irq(USER_BTN_PORT, USER_BTN_PIN, EXTI15_10_IRQn);
}


void user_set_led(void) {
    GPIOA->BSRR = (1U << USER_LED_PIN); // LED ON
}

void user_reset_led(void) {
    GPIOA->BSRR = (1U << (USER_LED_PIN + 16)); //LED OFF
}

void user_toggle_led (void) {
    static volatile bool led_is_on = 0;

    if (led_is_on){
        user_reset_led();
        led_is_on = 0;
    }
    else{
        user_set_led();
        led_is_on = 1;
    }
}

static volatile uint32_t led_delay = 300;
void user_auto_toggle_led_task (void *arg) {
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    (void) time_now;
    while (1){
        user_toggle_led();
        counter++;
        time_now = timebase_show_ms();
        os_delay(led_delay);
    }
}

/*
static volatile bool user_btn_pressed = 0;
void user_set_button_flag(void) {
    user_btn_pressed = 1;
}
*/

void user_button_toggle_led_task (void *arg) {
    static bool curr_btn_state = 1;
    static bool prev_btn_state = 1;
    static volatile bool user_btn_pressed = 0;
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    (void) time_now;
    while (1)
    {
    	user_btn_pressed = isr_show_exti15_10_pending_status();
        if (user_btn_pressed) {
            user_btn_pressed = 0;//Clear SW flag
            isr_clear_exti15_10_pending_status();//Clear SW flag in ISR
            curr_btn_state = 0;
        }

        if ((prev_btn_state == 1 ) & (curr_btn_state == 0)) {
           led_delay = ((led_delay + 100) > 600) ? 300 : (led_delay + 100);
           counter++;
           time_now = timebase_show_ms();//Just for debug
           os_delay(35);//De-baunce
           time_now = timebase_show_ms();//Just for debug
           exti_clean_flag(USER_BTN_PIN);//Clear HW flag
           //EXTI->PR = (1U << 13);//Clear HW flag
           isr_enable_interrupts(EXTI15_10_IRQn);//Re-enable ISR
        }
        time_now = timebase_show_ms();//Just for debug
        prev_btn_state = curr_btn_state;
        curr_btn_state = 1;
        os_delay(50); //Relaxation
    }
}


#include "rtos.h"
#include "user.h"
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
    gpio_pin_mode(GPIOA, USER_LED_PIN, GPIO_MODE_OUTPUT);

    // PC13 as INPUT
    gpio_pin_mode(GPIOC, USER_BTN_PIN, GPIO_MODE_INPUT);

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

void user_toggle_led(void) {
    static volatile bool led_is_on = 0;

    if (led_is_on) {
        user_reset_led();
        led_is_on = 0;
    } else {
        user_set_led();
        led_is_on = 1;
    }
}

static volatile uint32_t led_delay = 300;
void user_auto_toggle_led_task(void *arg) {
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    (void) time_now;
    while (1) {
        user_toggle_led();
        counter++;
        time_now = timebase_show_ms();
        rtos_delay(led_delay);
    }
}

void user_button_change_frequency_task(void *arg) {
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    (void) time_now;
    while (1) {
        time_now = timebase_show_ms(); //Just for debug
        bool success = rtos_take_sem(&user_button_sem);
        time_now = timebase_show_ms(); //Just for debug

        //if task successfully assigned to sem waiting list then
        //the task will actually get to this line only after button will be pressed
        //in that case success == 1
        if (success){
            time_now = timebase_show_ms(); //Just for debug
            rtos_delay(30); //De-baunce
            time_now = timebase_show_ms(); //Just for debug

            led_delay = ((led_delay + 100) > 600) ? 300 : (led_delay + 100);
            counter++;

            exti_clean_flag(USER_BTN_PIN); //Clear HW flag
            isr_enable(EXTI15_10_IRQn); //Re-enable ISR
        }
        //if sem waiting list wasn't available the task will get to this line immediately
        //in that case success == 0. then task will sleep for 10 ms
        else{
            rtos_delay(10);
        }
    }
}


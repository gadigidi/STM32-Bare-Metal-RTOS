#include "user.h"
#include "os.h"
#include "gpio.h"
#include "exti.h"
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
    exti_enable_irq(USER_BTN_PIN, USER_BTN_PORT);
    //isr_enable_interrupts(EXTI13_IRQn);
}


void user_set_led(void) {
    GPIOA->BSRR = USER_LED_PIN; // LED ON
}

void user_reset_led(void) {
    GPIOA->BSRR = (USER_LED_PIN << 16); //LED OFF
}

void user_toggle_led (void) {
    static bool led_is_on = 0;
    if (led_is_on){
        user_reset_led();
        led_is_on = 0;
    }
    else{
        user_set_led();
        led_is_on = 1;
    }
}

void user_auto_toggle_led_task (void *arg) {
    while (1){
        os_delay(300);
        user_toggle_led();
    }
}

static volatile bool user_btn_pressed = 0;
void user_set_button_flag(void) {
    user_btn_pressed = 1;
}

void user_button_toggle_led_task (void *arg) {
    static bool curr_btn_state = 1;
    static bool prev_btn_state = 1;
    while (1)
    {
        if (user_btn_pressed) {
            user_btn_pressed = 0;
            os_delay(USER_BTN_DEBOUNCE_MS);
            curr_btn_state = (GPIOC->IDR & USER_BTN_PIN) ? 0 : 1; //BTN is active low
        }

        if ((prev_btn_state == 1 ) & (curr_btn_state == 0)) {
           user_toggle_led();
        }

        prev_btn_state = curr_btn_state;
        os_delay(100);
    }
}


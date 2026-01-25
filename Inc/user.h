#ifndef USER_H_
#define USER_H_

#define PIN5                            (1U<<5)
#define USER_LED_PORT                   'A'
#define USER_LED_PIN                    PIN5

#define PIN13                           (1U<<13)
#define USER_BTN_PORT                   'C'
#define USER_BTN_PIN                    PIN13
#define USER_BTN_DEBOUNCE_MS            35

void user_set_led(void);
void user_reset_led(void);
void user_toggle_led (void);
void user_toggle_led (void);

void user_set_button_flag(void);

void user_auto_toggle_led_task(void *arg);
void user_button_toggle_led_task (void *arg);

#endif /* USER_H_ */

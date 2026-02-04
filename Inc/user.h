#ifndef USER_H_
#define USER_H_

#define USER_LED_PORT                   'A'
#define USER_LED_PIN                    5

#define USER_BTN_PORT                   'C'
#define USER_BTN_PIN                    13

void user_init(void);

void user_set_led(void);
void user_reset_led(void);
void user_toggle_led (void);
void user_toggle_led (void);

void user_set_button_flag(void);

void user_auto_toggle_led_task(void *arg);
void user_button_change_frequency_task(void *arg);

#endif /* USER_H_ */

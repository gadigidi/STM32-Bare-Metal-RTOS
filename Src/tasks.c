#include "tasks.h"
#include "user.h"
#include <stdint.h>

//User should fill this with tasks names
task_entry_t task_entry [TASKS_NUM] = {
        &user_auto_toggle_led_task,
        &user_button_toggle_led_task
};

//User should fill this with tasks arg in same order
task_arg_t task_arg[TASKS_NUM] = {
        0,
        0
};

//Tasks assignments:
//TASK_0: user_auto_toggle_led_task()
//TASK_1: user_button_toggle_led_task()

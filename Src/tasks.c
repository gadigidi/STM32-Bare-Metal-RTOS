#include "tasks.h"
#include "user.h"
#include "timebase.h"
#include <stdint.h>

//User should fill this with tasks names
task_entry_t task_entry [TASKS_NUM] = {
        &user_auto_toggle_led_task,
        &user_button_toggle_led_task,
        &idle_task //Do not remove!
};

//User should fill this with tasks arg in same order
task_arg_t task_arg[TASKS_NUM] = {
        0,
        0,
        0
};

void idle_task(void *arg){
    uint32_t time_now;
    (void) time_now;
    while(1){
        time_now = timebase_show_ms();
        //Dead end
    }
}

//Tasks assignments:
//TASK_0: user_auto_toggle_led_task()
//TASK_1: user_button_toggle_led_task()
//TASK_2: idle_task

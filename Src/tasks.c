#include "tasks.h"
#include "user.h"
#include "timebase.h"
#include <stdint.h>


//Tasks assignments:
//TASK_0: user_auto_toggle_led_task()
//TASK_1: user_button_toggle_led_task()

//User should fill this with tasks names
task_entry_t task_entry [USER_TASKS_NUM] = {
        &user_auto_toggle_led_task,
        &user_button_toggle_led_task,
};

//User should fill this with tasks arg in same order
task_arg_t task_arg[USER_TASKS_NUM] = {
        0,
        0,
};


///////////////////////////////////////////
// NOTE: idle_task should not be removed //
///////////////////////////////////////////
void os_idle_task(void *arg){
    uint32_t time_now;
    (void) time_now;
    while(1){
        time_now = timebase_show_ms();
        (void) time_now;
        //Dead end
    }
}

////////////////////////////////////////////
// NOTE: error_task should not be removed //
////////////////////////////////////////////
void os_error_task(void *arg){
    while(1){
        uint32_t time_now = timebase_show_ms();
        (void) time_now;
        //Dead end
    }
}






#include "tasks.h"
#include "stub_tasks.h"
#include "user.h"
#include "i2c_m.h"
#include "timebase.h"
#include "stm32f446xx.h"
#include <stdint.h>


//Tasks assignments:
//TASK 0: user_auto_toggle_led_task()
//TASK 1: user_button_toggle_led_task()
//TASK 2: i2c_master_write_task
//TASKS 3-15: stub tasks with different priorities

//User should fill this with tasks names
task_entry_t task_entry [USER_TASKS_NUM] = {
        &user_auto_toggle_led_task,
        &user_button_change_frequency_task,
        &i2c_master_task,
        &stub_task_0,
        &stub_task_1,
        &stub_task_2,
        &stub_task_3,
        &stub_task_4,
        &stub_task_5,
        &stub_task_6,
        &stub_task_7,
        &stub_task_8,
        &stub_task_9,
        &stub_task_10,
        &stub_task_11,
        &stub_task_12,
};

//User should fill this with tasks arg in same order
task_arg_t task_arg[USER_TASKS_NUM] = {
        0,
        0,
        I2C_MASTER,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
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






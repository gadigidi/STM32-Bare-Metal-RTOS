#include "rtos.h"
#include "tasks.h"
#include "stub_tasks.h"
#include "user.h"
#include "i2c_m.h"
#include "i2c_agents.h"
#include "spi_m.h"
#include "spi_agents.h"
#include "timebase.h"
#include "stm32f446xx.h"
#include <stdint.h>


//Tasks assignments:
//TASK 0: user_auto_toggle_led_task()
//TASK 1: user_button_toggle_led_task()
//TASK 2: i2c_master_task
//TASK_3: spi_master_task
//TASKS 4-15: stub tasks with different priorities

//User should fill this with tasks names
task_entry_t task_entry [USER_TASKS_NUM] = {
        &user_auto_toggle_led_task,
        &user_button_change_frequency_task,
        &i2c_m_task,
        &spi_m_task,
        &stub_task_4,
        &stub_task_5,
        &stub_task_6,
        &stub_task_7,
        &stub_task_8,
        &stub_task_9,
        &stub_task_10,
        &stub_task_11,
        &stub_task_12,
        &stub_task_13,
        &stub_task_14,
        &stub_task_15,
};

//User should fill this with tasks arg in same order
task_arg_t task_arg[USER_TASKS_NUM] = {
        0,
        0,
        &i2c1_cb,
        &spi2_cb,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
};

//User should fill this with tasks priorities in same order
int task_pri[USER_TASKS_NUM] = {
        5,//led
        2,//button
        5,//i2c
        5,//spi
        4,
        5,
        6,
        7,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
};
///////////////////////////////////////////
// NOTE: idle_task should not be removed //
///////////////////////////////////////////
void rtos_idle_task(void *arg){
    uint32_t time_now;
    (void) time_now;
    while(1){
        rtos_update_counter(current_tcb->id);
        time_now = timebase_show_ms();
        (void) time_now;
        //rtos_update_counter(OS_IDLE_TASK);
        //Dead end
    }
}

////////////////////////////////////////////
// NOTE: error_task should not be removed //
////////////////////////////////////////////
void rtos_error_task(void *arg){
    while(1){
        uint32_t time_now = timebase_show_ms();
        (void) time_now;
        //Dead end
    }
}






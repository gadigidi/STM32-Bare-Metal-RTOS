#include "rtos.h"
#include "user.h"
#include "i2c_agents.h"
#include "spi_agents.h"
#include "dwt.h"
#include <stdio.h>
#include <stdint.h>
#include "stm32f446xx.h"


int main(void){
    user_init(); //Enable user LED and user BTN

    rtos_init();

    dwt_init();

    i2c1_init();

    spi2_init();

    rtos_start();
    
    while (1) {

        //time_now = timebase_show_ms();
    }
}

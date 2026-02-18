#include "os.h"
#include "user.h"
#include "i2c.h"
#include "i2c_m.h"
#include <stdio.h>
#include <stdint.h>
#include "stm32f446xx.h"


int main(void){
    user_init(); //Enable user LED and user BTN

    os_init();

    i2c_master_init();

    os_run();
    
    while (1) {

        //time_now = timebase_show_ms();
    }
}

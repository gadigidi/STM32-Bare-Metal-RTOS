#include "os.h"
#include "timebase.h"
#include "user.h"
#include "exti.h"
#include <stdio.h>
#include <stdint.h>
#include "stm32f446xx.h"



user_init(); //Enable user LED and user BTN


int main(void){

    os_init();

    os_start();
    
    while (1) {
        //time_now = timebase_show_ms();
    }
}

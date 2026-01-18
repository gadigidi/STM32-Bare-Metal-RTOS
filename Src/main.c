#include "rfid.h"
#include "rc522.h"
#include "rc522_debug.h"
#include "seg7.h"
#include "timebase.h"
#include <stdio.h>
#include <stdint.h>
#include "stm32f446xx.h"

int main(void){

    timebase_init();
    
    seg7_init();

    seg7_set_fsm_state(SEG7_IDLE_ANIMATION);

    rfid_init();

    rfid_fsm_reset();

    //rc522_debug();

    while (1) {
        //time_now = timebase_show_ms();
        seg7_auto_refresh();

        seg7_fsm();

        rfid_fsm();

    }
}

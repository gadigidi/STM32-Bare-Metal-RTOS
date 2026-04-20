#include "dwt.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>

void dwt_init(void){
    //Enable trace
    CoreDebug->DEMCR |= TRCEN;

    DWT->CYCCNT = 0;

    //Start DWT count
    DWT->CTRL |= TRC_START;
}

uint32_t dwt_count(void){
    return DWT->CYCCNT;
}



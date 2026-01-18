#include "seg7.h"
#include "gpio.h"
#include "timebase.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>

const uint32_t seg_bsrr[27] = { 1610614752U, 1730150592U, 1149248352U,
        1174413792U, 1126180032U, 1111500192U, 1077946272U, 1728053472U,
        1073752032U, 1107305952U, 1090528992U, 1080043392U, 1623197472U,
        1142956992U, 1086334752U, 1103111712U, 1742733312U, 1740636192U,
        1205870592U, 1725956352U, 1738539072U, 1734344832U, 1675625472U,
        1709179392U, 1163928192U, 1612711872U, 1077946272U };

//0-9: 0..9
//10-15: A..F
//16: No display
//17: Top segment
//18: Middle segment
//19: Bottom segment
//20: Top right
//21: bottom right
//22: Top left
//23: Bottom left
//24: r / n
//25: U
//26: G

const uint32_t dig_bsrr[4] = { 65550U, 131085U, 262155U, 524295U };

void seg7_init(void){
    gpio_init(GPIOBEN);
    gpio_pin_mode(GPIOB, SEG_A_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_mode(GPIOB, SEG_B_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_mode(GPIOB, SEG_C_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_mode(GPIOB, SEG_D_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_mode(GPIOB, SEG_E_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_mode(GPIOB, SEG_F_PIN,GPIO_MODE_OUTPUT );
    gpio_pin_mode(GPIOB, SEG_G_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_mode(GPIOB, SEG_DEC_PIN, GPIO_MODE_OUTPUT);

    gpio_init(GPIOCEN);
    gpio_pin_mode(GPIOC, DIG1_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_mode(GPIOC, DIG2_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_mode(GPIOC, DIG3_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_mode(GPIOC, DIG4_PIN, GPIO_MODE_OUTPUT);

    seg7_deselect_all_digits();
}

void seg7_select_digit(uint32_t bsrr) {
    GPIOC->BSRR = bsrr;
}

void seg7_deselect_all_digits(void) {
    GPIOC->BSRR = SEG7_BSRR_DIGS_OFF;
}

void seg7_select_seg(uint32_t bsrr) {
    GPIOB->BSRR = bsrr;
}

void seg7_show_digit(int digit, int token) {
    seg7_deselect_all_digits();
    seg7_select_seg(seg_bsrr[token]);
    seg7_select_digit(dig_bsrr[digit]);
}

static uint8_t disp_nibble[4] = { 16, 16, 16, 16 };

static uint32_t refresh_start_time = 0;
void seg7_auto_refresh(void) {
    static int curr_digit = 0;
    uint32_t time_now = timebase_show_ms();
    uint32_t time_delta = time_now - refresh_start_time;
    if (time_delta >= 1) {
        seg7_show_digit(curr_digit, disp_nibble[curr_digit]);
        curr_digit = (curr_digit + 1) % 4;
        refresh_start_time = timebase_show_ms();
    }
}

static uint8_t scroll_buffer[12];
static int curr_scroll_digit = 0;
void seg7_set_buffer_for_scroll(volatile uint8_t *bytes) {
    scroll_buffer[0] = 16; //None
    scroll_buffer[1] = 16; //None
    scroll_buffer[2] = 16; //None
    scroll_buffer[3] = ((bytes[3] >> 4) & 0xFU);
    scroll_buffer[4] = (bytes[3] & 0xFU);
    scroll_buffer[5] = ((bytes[2] >> 4) & 0xFU);
    scroll_buffer[6] = (bytes[2] & 0xFU);
    scroll_buffer[7] = ((bytes[1] >> 4) & 0xFU);
    scroll_buffer[8] = (bytes[1] & 0xFU);
    scroll_buffer[9] = ((bytes[0] >> 4) & 0xFU);
    scroll_buffer[10] = (bytes[0] & 0xFU);
    scroll_buffer[11] = 16; //None

    curr_scroll_digit = 0; //Initialize scroll internal state
}

void seg7_scroll_digits(void) {
    int nibbel2show;
    nibbel2show = (curr_scroll_digit) % 12;
    disp_nibble[0] = scroll_buffer[nibbel2show];
    nibbel2show = (curr_scroll_digit + 1) % 12;
    disp_nibble[1] = scroll_buffer[nibbel2show];
    nibbel2show = (curr_scroll_digit + 2) % 12;
    disp_nibble[2] = scroll_buffer[nibbel2show];
    nibbel2show = (curr_scroll_digit + 3) % 12;
    disp_nibble[3] = scroll_buffer[nibbel2show];
}

const int idle_animation_buffer[12] = { 17, 17, 17, 17, 20, 21, 19, 19, 19, 19,
        23, 22 };
const int idle_animation_order[12] = { 0, 1, 2, 3, 3, 3, 3, 2, 1, 0, 0, 0 };
static int curr_idle_animation_digit = 0;
void seg7_show_idle_animation(void) {
    int digit = idle_animation_order[curr_idle_animation_digit];
    for (int i = 0; i < 4; i++) {
        if (i == digit) {
            disp_nibble[i] = idle_animation_buffer[curr_idle_animation_digit];
        } else {
            disp_nibble[i] = 16; //None
        }
    }
}

const int uid_blinking_buffer[4] = { 25, 1, 13, 16 }; //U1d
const int gadi_blinking_buffer[4] = { 26, 10, 13, 1 }; //GAd1
const int dan_blinking_buffer[4] = { 13, 10, 24, 16 }; //dAn
const int error_blinking_buffer[4] = { 14, 24, 24, 16 }; //Err
const int default_blinking_buffer[4] = { 16, 16, 16, 16 };
static const int *blinking_buffer = default_blinking_buffer;
static int blinking_stage = 0;
void seg7_set_blinking_text(seg7_blinking_text_t text) {
    switch (text) {
    case SEG7_UID:
        blinking_buffer = uid_blinking_buffer;
        break;
    case SEG7_GADI:
        blinking_buffer = gadi_blinking_buffer;
        break;
    case SEG7_DAN:
        blinking_buffer = dan_blinking_buffer;
        break;
    case SEG7_ERROR:
        blinking_buffer = error_blinking_buffer;
        break;
    default:

    }
    blinking_stage = 0;
}

void seg7_show_blinking_animation(void) {

    disp_nibble[0] = (blinking_stage) ? blinking_buffer[0] : 16; //Left digit
    disp_nibble[1] = (blinking_stage) ? blinking_buffer[1] : 16; //Second digit
    disp_nibble[2] = (blinking_stage) ? blinking_buffer[2] : 16; //Third digit
    disp_nibble[3] = (blinking_stage) ? blinking_buffer[3] : 16; //Right digit
}

static uint32_t start_time = 0;
bool seg7_non_blocking_delay(uint32_t delay_ms) {
    uint32_t time_now = timebase_show_ms();
    uint32_t time_delta = time_now - start_time;
    if (time_delta >= delay_ms) {
        return 1;
    } else {
        return 0;
    }
}

static seg7_state_t seg7_state = SEG7_OFF;
void seg7_set_fsm_state(seg7_state_t new_state) {
    seg7_state = new_state;
    start_time = timebase_show_ms();
}

//static uint32_t idle_animation_start_time = 0;
//static uint32_t blinking_animation_start_time = 0;
//static uint32_t scroll_start_time = 0;
void seg7_fsm(void) {
    switch (seg7_state) {
    case SEG7_OFF: {
        break;
    }

    case SEG7_IDLE_ANIMATION: {
        //uint32_t time_now = timebase_show_ms();
        //uint32_t time_delta = time_now - idle_animation_start_time;
        if (seg7_non_blocking_delay(80)) {
            seg7_show_idle_animation();
            curr_idle_animation_digit = (curr_idle_animation_digit + 1) % 12;
            start_time = timebase_show_ms();
        }
        break;
    }

    case SEG7_BLINKING_ANIMATION: {
        //uint32_t time_now = timebase_show_ms();
        //uint32_t time_delta = time_now - blinking_animation_start_time;
        if (seg7_non_blocking_delay(350)) {
            seg7_show_blinking_animation();
            blinking_stage = (blinking_stage + 1) % 2;
            start_time = timebase_show_ms();
        }
        break;
    }

    case SEG7_SCROLL: {
        //uint32_t time_now = timebase_show_ms();
        //uint32_t time_delta = time_now - scroll_start_time;
        if (seg7_non_blocking_delay(650)) {
            seg7_scroll_digits();
            curr_scroll_digit = (curr_scroll_digit + 1) % 12;
            start_time = timebase_show_ms();
        }
        break;
    }
    }        //switch
}

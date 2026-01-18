#ifndef SEG7_H_
#define SEG7_H_

#include <stdint.h>
#include <stdbool.h>

//All segments on GPIOB
#define SEG_A_PIN               5U
#define SEG_B_PIN               6U
#define SEG_C_PIN               7U
#define SEG_D_PIN               8U
#define SEG_E_PIN               9U
#define SEG_F_PIN               10U
#define SEG_G_PIN               13U
#define SEG_DEC_PIN             14U

//All digits on GPIOC
#define DIG1_PIN                0
#define DIG2_PIN                1U
#define DIG3_PIN                2U
#define DIG4_PIN                3U

#define SEG7_SEG_ACTIVE         0
#define SEG7_DIG_ACTIVE         1U

#define SEG7_BSRR_DIGS_OFF      15U

typedef enum {
    SEG7_OFF,
    SEG7_IDLE_ANIMATION,
    SEG7_BLINKING_ANIMATION,
    SEG7_SCROLL
} seg7_state_t;

typedef enum {
    SEG7_UID, SEG7_GADI, SEG7_DAN, SEG7_ERROR
} seg7_blinking_text_t;

void seg7_init (void);
void seg7_select_digit(uint32_t bsrr);
void seg7_deselect_all_digits(void);
void seg7_select_seg(uint32_t bsrr);
void seg7_show_digit(int digit, int token);
void seg7_auto_refresh(void);
void seg7_set_buffer_for_scroll(volatile uint8_t *bytes);
void seg7_show_idle_animation(void);
void seg7_set_blinking_text(seg7_blinking_text_t text);
void seg7_show_blinking_animation(void);
void seg7_scroll_digits(void);
void seg7_set_fsm_state(seg7_state_t new_state);
void seg7_fsm(void);


#endif /* SEG7_H_ */

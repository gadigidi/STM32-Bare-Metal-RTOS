#ifndef RFID_H_
#define RFID_H_

#include <stdint.h>
#include <stdbool.h>

#define RFID_POLL_PERIOD_MS             300
#define RFID_REQA_TIMEOUT_MS            100
#define RFID_UID_TIMEOUT_MS             100
#define RFID_CRC_TIMEOUT_MS             100
#define RFID_SHOW_UID_TIMEOUT_MS        8000 //8 sec
#define RFID_SHOW_STATUS_TIMEOUT_MS     3200 //3.2 sec
#define RFID_COOLDOWN_MS                500

#define RFID_GADI_CARD                  0x238D90B5U
#define RFID_DAN_CARD                   0xC2E5CD49U

void rfid_init(void);
//void rfid_enable_irq(void);
//void rfid_spi1_config(void);
//void rfid_send_reqa(void);
//void rfid_get_atqa(void);
//void rfid_send_anticol_cl1(void);
//void rfid_send_select_cl1(void);
void rfid_fsm(void);
void rfid_fsm_reset(void);
void rfid_update_irq_flag(void);
bool rfid_show_pending_irq_status(void);
bool rfid_check_error(void);

#endif /* RFID_H_ */

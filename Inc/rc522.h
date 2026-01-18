#ifndef RC522_H_
#define RC522_H_

#include <stdint.h>
#include <stdbool.h>

#define RC522_WR                    0
#define RC522_RD                    1U
#define RC522_ADDR_MASK             0x7EU //b01111110 because lsb must be 0 and msb is wr/re

//REGs handles
#define RC522_COMMAND_REG           0x1U
#define RC522_COMIEN_REG            0x2U
#define RC522_DIVIEN_REG            0x3U
#define RC522_COMIRQ_REG            0x4U
#define RC522_DIVIRQ_REG            0x5U
#define RC522_ERROR_REG             0x6U
#define RC522_STATUS1_REG           0x7U
#define RC522_STATUS2_REG           0x8U
#define RC522_FIFODATA_REG          0x9U
#define RC522_FIFOLEVEL_REG         0xAU
#define RC522_BITFRAMING_REG        0xDU
#define RC522_MODE_REG              0x11U
#define RC522_TXMODE_REG            0x12U
#define RC522_RXMODE_REG            0x13U
#define RC522_TXCONTROL_REG         0x14U
#define RC522_TXASK_REG             0x15U
#define RC522_CRC_MSB_REG           0x21U
#define RC522_CRC_LSB_REG           0x22U
#define RC522_T_MODE_REG            0x2AU
#define RC522_T_PRESCALER_REG       0x2BU
#define RC522_T_RELOAD_H_REG        0x2CU
#define RC522_T_RELOAD_L_REG        0x2DU
#define RC522_T_COUNTERVAL_H_REG    0x2EU
#define RC522_T_COUNTERVAL_L_REG    0x2FU

#define RC522_VERSION_REG           0x37U
#define RC522_VERSION               0x92U

//RC522 commands
#define RC522_COMMAND_IDLE          0x0
#define RC522_COMMAND_CRC_CALC      0x3U
#define RC522_COMMAND_TRANSCIEVE    0xCU
#define RC522_COMMAND_RESET         0xFU

//Timer constants
#define RC522_INT_CLK               13560000 //13.56MHz
#define RC522_PSC                   67 //Clk after PSC = 13.56MHz/(67*2+1) = ~100kHz
#define RC522_RELOAD_VAL            4999 //Timeout = 50ms

//Functions
bool rc522_chip_init(void);
void rc522_hard_reset(void);

void rc522_write_reg(uint8_t addr, uint8_t data);
uint8_t rc522_read_reg(uint8_t addr);
void rc522_modify_reg(uint8_t addr, uint8_t new_data, uint8_t mask);

void rc522_soft_reset(void);
bool rc522_alive_status(void);
void rc522_enable_irq(void);
void rc522_clean_flags(void);
void rc522_clean_fifo(void);

void rc522_load_fifo(volatile uint8_t *fifo, uint8_t length);
void rc522_transcieve(volatile uint8_t *fifo, uint8_t length,
        uint8_t tx_last_bits);
uint8_t rc522_read_fifo(volatile uint8_t *fifo);

#endif /* RC522_H_ */

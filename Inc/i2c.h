#ifndef I2C_H_
#define I2C_H_

#include "stm32f446xx.h"
#include <stdint.h>
#include "os.h"

typedef enum {
    I2C_MASTER,
    I2C_SLAVE,
} i2c_mode_t;

typedef enum {
    I2C_WR,
    I2C_RD,
    I2C_RDREG,
} i2c_trans_type_t;

typedef struct {
    uint8_t *tx_buffer;
    volatile uint8_t tx_length;
    uint8_t *rx_buffer;
    volatile uint8_t rx_length;
    uint8_t addr;
    i2c_trans_type_t trans_type;
} i2c_ctx_t;


#define I2C_BUF_DEPTH           64
#define I2C_WR_BIT              0
#define I2C_RD_BIT              (1U<<0)

//CR1
#define I2C_SWRST               (1U<<15)
#define I2C_PE                  (1U<<0)
#define I2C_START               (1U<<8)
#define I2C_STOP                (1U<<9)
//CR2
#define I2C_ERR_INT_EN          (1U<<8)
#define I2C_EVENT_INT_EN        (1U<<9)
#define I2C_TXE_INT_EN          (1U<<10)
//Timing Configuration
#define I2C_PCLK_MHZ            16U
#define I2C_FREQ_KHZ            100U //For standard mode
#define I2C_CCR                 (I2C_PCLK_MHZ*10000000)/(2*(I2C_FREQ_KHZ*1000))
#define I2C_TRISE               I2C_PCLK_MHZ+1 //For standard mode
//SR1
#define I2C_SB              (1U<<0)
#define I2C_ADDR            (1U<<1)
#define I2C_BTF             (1U<<2)
#define I2C_STOPF           (1U<<4)
#define I2C_RXNE            (1U<<6)
#define I2C_TXE             (1U<<7)
//SR2
#define I2C_BUSY                (1U<<1)

void i2c_enable_irq(I2C_TypeDef *I2C);
void i2c_sw_reset (I2C_TypeDef *I2C);
void i2c_recovery (I2C_TypeDef *I2C);
void i2c_config_clk (I2C_TypeDef *I2C);
i2c_ctx_t * i2c_master_prepare_transaction(void);
void i2c_start_transaction(I2C_TypeDef *I2C);
void i2c_send_tx_byte(I2C_TypeDef *I2C, uint8_t tx_byte);
void i2c_gen_stop(I2C_TypeDef *I2C);

#endif /* I2C_H_ */

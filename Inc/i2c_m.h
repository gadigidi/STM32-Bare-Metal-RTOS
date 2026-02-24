#ifndef I2C_M_H_
#define I2C_M_H_

#include "i2c.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include "os.h"

typedef enum {
    M_TASK_IDLE,
    M_TASK_INITIATE_TRANS,
    M_TASK_WAIT_NOT_BUSY,
    M_TASK_WAIT_TRANS_DONE,
    M_TASK_PROCEES_DATA,
    M_TASK_RECOVERY,
} i2c_master_task_state_t;

typedef enum {
    M_DRVR_IDLE,
    M_DRVR_SEND_ADDR_W,
    M_DRVR_WAIT_ADDR_W,
    M_DRVR_WAIT_TXE,
    M_DRVR_TX_SEND_BYTE,
    M_DRVR_TX_WAIT_BTF,
    M_DRVR_GEN_STOP,
    M_DRVR_ERROR,
} i2c_master_driver_state_t;

typedef struct {
    I2C_TypeDef *i2c_agent;
    i2c_mode_t mode;
    volatile i2c_master_driver_state_t *master_driver_state;
    semaphore_t *sem;
    i2c_ctx_t *ctx;
    uint8_t error;
} i2c_master_control_block_t;

extern i2c_master_control_block_t i2c_master_cb;

#define I2C_MASTER_INDEX        0
#define I2C_MASTER_AGENT        I2C1
#define I2C_MASTER_PORT         GPIOB
#define I2C_MASTER_PORT_EN      GPIOBEN
#define I2C_MASTER_AF           4
#define I2C_MASTER_SCL_PIN      8
#define I2C_MASTER_SDA_PIN      9
#define I2C_MASTER_CLK_EN       (1U<<21)


void i2c_master_init (void);
void i2c_master_task (void* arg);
void i2c_master_driver(void);

#endif /* I2C_M_H_ */

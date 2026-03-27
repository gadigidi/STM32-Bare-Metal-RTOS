#ifndef I2C_M_H_
#define I2C_M_H_

#include "rtos.h"
#include "i2c.h"
#include "stm32f446xx.h"
#include <stdint.h>

typedef enum {
    I2C_M_TASK_IDLE,
    I2C_M_TASK_INITIATE_TRANS,
    I2C_M_TASK_WAIT_NOT_BUSY,
    I2C_M_TASK_WAIT_TRANS_DONE,
    I2C_M_TASK_PROCEES_DATA,
    I2C_M_TASK_RECOVERY,
} i2c_master_task_state_t;

typedef enum {
    I2C_M_DRVR_IDLE,
    I2C_M_DRVR_SEND_ADDR_W,
    I2C_M_DRVR_WAIT_ADDR_W,
    I2C_M_DRVR_WAIT_TXE,
    I2C_M_DRVR_TX_SEND_BYTE,
    I2C_M_DRVR_TX_WAIT_BTF,
    I2C_M_DRVR_GEN_STOP,
    I2C_M_DRVR_ERROR,
} i2c_master_driver_state_t;

typedef struct {
    I2C_TypeDef *i2c_agent;
    i2c_mode_t mode;
    semaphore_t *i2c_sem;
    mutex_t *i2c_mutex;
    volatile i2c_master_task_state_t *master_task_state;
    volatile i2c_master_driver_state_t *master_driver_state;
    i2c_ctx_t *ctx;
    uint8_t error;
} i2c_master_control_block_t;


void i2c_m_task (void *arg);
void i2c_m_driver(i2c_master_control_block_t *i2c_cb);

#endif /* I2C_M_H_ */

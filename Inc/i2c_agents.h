#ifndef I2C_AGENTS_H_
#define I2C_AGENTS_H_

#include "i2c_m.h"

extern i2c_master_control_block_t i2c1_cb;

#define I2C1_AGENT        I2C1
#define I2C1_PORT         GPIOB
#define I2C1_PORT_EN      GPIOBEN
#define I2C1_AF           4
#define I2C1_SCL_PIN      8
#define I2C1_SDA_PIN      9
#define I2C1_CLK_EN       (1U<<21)


void i2c1_init (void);
void i2c1_recovery(void);


#endif /* I2C_AGENTS_H_ */

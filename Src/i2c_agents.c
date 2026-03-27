#include "rtos.h"
#include "i2c_agents.h"
#include "i2c_m.h"
#include "i2c.h"
#include "timebase.h"
#include "gpio.h"
#include "stm32f446xx.h"
#include <stdint.h>

i2c_master_control_block_t i2c1_cb;
static i2c_ctx_t *i2c1_ctx;
static uint8_t tx_buffer[I2C_BUF_DEPTH];
static uint8_t rx_buffer[I2C_BUF_DEPTH];
static i2c_master_task_state_t master_task_state = I2C_M_TASK_IDLE;
static volatile i2c_master_driver_state_t master_driver_state = I2C_M_DRVR_IDLE;

void i2c1_init (void) {

    i2c1_cb.mode = I2C_MASTER;
    i2c1_cb.i2c_agent = I2C1;
    i2c1_cb.master_task_state = &master_task_state;
    i2c1_cb.master_driver_state = &master_driver_state;
    i2c1_cb.i2c_sem = &i2c1_done_sem;
    i2c1_cb.i2c_mutex = NULL;
    i2c1_cb.ctx = i2c1_ctx;
    i2c1_cb.ctx->tx_buffer = tx_buffer;
    i2c1_cb.ctx->rx_buffer = rx_buffer;
    i2c1_cb.ctx->tx_length = 0;
    i2c1_cb.ctx->rx_length = 0;
    i2c1_cb.error = 0;

    //Enable clock and set GPIO modes
    gpio_init(I2C1_PORT_EN);
    gpio_pin_mode(I2C1_PORT, I2C1_SCL_PIN, GPIO_MODE_AF);
    gpio_pin_afr(I2C1_PORT, I2C1_SCL_PIN, I2C1_AF);
    gpio_pin_mode(I2C1_PORT, I2C1_SDA_PIN, GPIO_MODE_AF);
    gpio_pin_afr(I2C1_PORT, I2C1_SDA_PIN, I2C1_AF);

    //Enable clock to I2C
    RCC->APB1ENR |= I2C1_CLK_EN;

    i2c_config_clk(I2C1);

    //Enable Peripheral
    I2C1->CR1 |= I2C_PE;

    i2c_enable_irq(I2C1_AGENT);
}

void i2c1_recovery (void) {
    //i2c_master_clear_busy(I2C);
    i2c_sw_reset(I2C1);

    //Reconfigurations after soft reset
    I2C1->CR1 &= ~I2C_PE; //Disable peripheral
    i2c1_init();
    i2c_enable_irq(I2C1);
}


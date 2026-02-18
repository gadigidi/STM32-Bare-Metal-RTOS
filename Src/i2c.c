#include "i2c.h"
#include "isr.h"
#include "lfsr_simple.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>

void i2c_enable_irq(I2C_TypeDef *I2C) {
    I2C->CR2 |= I2C_ERR_INT_EN;
    I2C->CR2 |= I2C_EVENT_INT_EN;
    I2C->CR2 |= I2C_TXE_INT_EN;
    if (I2C == I2C1){
        isr_enable_interrupt(I2C1_EV_IRQn);
        isr_enable_interrupt(I2C1_ER_IRQn);
    }
    if (I2C == I2C2){
        isr_enable_interrupt(I2C2_EV_IRQn);
        isr_enable_interrupt(I2C2_ER_IRQn);
    }
    if (I2C == I2C3){
        isr_enable_interrupt(I2C3_EV_IRQn);
        isr_enable_interrupt(I2C3_ER_IRQn);
    }
}

void i2c_sw_reset (I2C_TypeDef *I2C) {
    I2C->CR1 &= ~I2C_PE; //Disable peripheral
    I2C->CR1 |= I2C_SWRST;
    __NOP(); //Wait 1 clk cycle
    I2C->CR1 &= ~I2C_SWRST;
    I2C->CR1 |= I2C_PE; //Re-enable peripheral
}

void i2c_recovery (I2C_TypeDef *I2C) {
    //i2c_master_clear_busy(I2C);
    i2c_sw_reset(I2C);

    //Reconfigurations after soft reset
    I2C->CR1 &= ~I2C_PE; //Disable peripheral
    i2c_config_clk(I2C);
    I2C->CR1 |= I2C_PE; //Enable Peripheral
    i2c_enable_irq(I2C);
}

void i2c_config_clk (I2C_TypeDef *I2C) {
    I2C->CR2 |= I2C_PCLK_MHZ; //Pclk in MHz
    I2C->CCR |= I2C_CCR; //Num of Pclk cycles for generated I2C clk (at 'High')
    I2C->TRISE = I2C_TRISE;
}

static i2c_ctx_t i2c_ctx;
i2c_ctx_t * i2c_master_prepare_transaction(void){
    i2c_ctx.trans_type = I2C_WR; //Should be rand in next version
    i2c_ctx.addr = (lfsr_next() % 128) | I2C_WR_BIT; //7bit addr
    uint8_t tx_length = (lfsr_next() % I2C_BUF_DEPTH);
    for (int i = 0; i < tx_length; i++){
        uint8_t data = lfsr_next() % 256;
        i2c_ctx.tx_buffer[i] = data;
    }
    i2c_ctx.tx_length = tx_length;
    return &i2c_ctx;
}

void i2c_start_transaction(I2C_TypeDef *I2C){
    I2C->CR1 |= I2C_START;
}


void i2c_send_tx_byte(I2C_TypeDef *I2C, uint8_t tx_byte){
    I2C->DR = tx_byte;
}

void i2c_gen_stop(I2C_TypeDef *I2C){
    I2C->CR1 |= I2C_STOP;
}

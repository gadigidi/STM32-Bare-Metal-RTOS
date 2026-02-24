#include "i2c_m.h"
#include "i2c.h"
#include "os.h"
#include "timebase.h"
#include "gpio.h"
#include "lfsr_simple.h"
#include "stm32f446xx.h"
#include <stdint.h>

static uint8_t tx_buffer[I2C_BUF_DEPTH];
static uint8_t rx_buffer[I2C_BUF_DEPTH];
i2c_master_control_block_t i2c_master_cb;
static volatile i2c_master_driver_state_t master_driver_state = M_TASK_IDLE;

void i2c_master_init (void) {

    i2c_master_cb.mode = I2C_MASTER;
    i2c_master_cb.i2c_agent = I2C_MASTER_AGENT;
    i2c_master_cb.master_driver_state = &master_driver_state;
    i2c_master_cb.sem = &i2c_master_done_sem;
    i2c_master_cb.ctx->tx_buffer = tx_buffer;
    i2c_master_cb.ctx->rx_buffer = rx_buffer;
    i2c_master_cb.ctx->tx_length = 0;
    i2c_master_cb.ctx->rx_length = 0;
    i2c_master_cb.error = 0;

    //Enable clock and set GPIO modes
    gpio_init(I2C_MASTER_PORT_EN);
    gpio_pin_mode(I2C_MASTER_PORT, I2C_MASTER_SCL_PIN, GPIO_MODE_AF);
    gpio_pin_afr(I2C_MASTER_PORT, I2C_MASTER_SCL_PIN, I2C_MASTER_AF);
    gpio_pin_mode(I2C_MASTER_PORT, I2C_MASTER_SDA_PIN, GPIO_MODE_AF);
    gpio_pin_afr(I2C_MASTER_PORT, I2C_MASTER_SDA_PIN, I2C_MASTER_AF);

    //Enable clock to I2C
    RCC->APB1ENR |= I2C_MASTER_CLK_EN;

    i2c_config_clk(I2C_MASTER_AGENT);

    //Enable Peripheral
    I2C_MASTER_AGENT->CR1 |= I2C_PE;

    i2c_enable_irq(I2C_MASTER_AGENT);
}

/*
//Used when BUSY held low by slave
void i2c_master_clear_busy(){
    //TODO v2
    //Need to change scl gpio of master to output open drain mode
    for (i = 0; i < 9; i++){
        //Generate 9 pulses to slave
    }
    //Generate STOP
    //Check if BUSY cleared
    //Need to reconf scl gpio of master to AF
}
*/

void i2c_master_task (void* arg) {
    static i2c_master_task_state_t master_task_state = M_TASK_IDLE;
    while (1){
        switch (master_task_state) {
        case M_TASK_IDLE: {
            uint32_t time_now = timebase_show_ms(); //Debug
            os_delay(5);
            time_now = timebase_show_ms(); //Debug
            (void) time_now;
            uint8_t event = lfsr_next() % 2;
            if (event>0){ //Get random event
                master_task_state = M_TASK_INITIATE_TRANS;
            }

            break;
        }

        case M_TASK_INITIATE_TRANS: {
            i2c_master_cb.ctx = i2c_master_prepare_transaction();
            //Fall through to next state
        }

        case M_TASK_WAIT_NOT_BUSY: {
            if (!(I2C_MASTER_AGENT->SR2 & I2C_BUSY)){
                i2c_start_transaction(I2C_MASTER_AGENT);
                master_task_state = M_TASK_WAIT_TRANS_DONE;
                //Fall through to next state
            }
            else{
                break;
            }
        }

        case M_TASK_WAIT_TRANS_DONE: {
            os_wait_sem(i2c_master_cb.sem);
            master_task_state = M_TASK_PROCEES_DATA;
            break;
        }

        case M_TASK_PROCEES_DATA: {
            if (i2c_master_cb.error){
                master_task_state = M_TASK_RECOVERY;
                break;
            }
            else{
                //i2c_clear_busy(I2C_MASTER_AGENT);
                master_task_state = M_TASK_IDLE;
                break;
            }
        }

        case M_TASK_RECOVERY: {
            i2c_recovery(I2C_MASTER_AGENT);
            i2c_master_cb.error = 0;
            master_task_state = M_TASK_IDLE;
            break;
        }

        }
    }
}

//This FSM function will be called directly from I2C ISRs (EV + ER)
//The fall through in some states is to avoid situations where flag is already set
//In situation like that no new interrupt will occur later for that flag
void i2c_master_driver(void) {
    static int i;
    switch (*i2c_master_cb.master_driver_state) {
    case M_DRVR_IDLE: {
        if (I2C_MASTER_AGENT->SR1 & I2C_SR1_SB){
            i = 0;
            *i2c_master_cb.master_driver_state = M_DRVR_SEND_ADDR_W;
            //Fall through to next state
        }
        else{
            break;
        }
    }

    case M_DRVR_SEND_ADDR_W: {
        uint8_t byte = i2c_master_cb.ctx->addr;
        i2c_send_tx_byte(I2C_MASTER_AGENT, byte);
        *i2c_master_cb.master_driver_state = M_DRVR_WAIT_ADDR_W;
        return;
    }

    case M_DRVR_WAIT_ADDR_W: {
        if (I2C_MASTER_AGENT->SR1 & I2C_SR1_ADDR){
            volatile uint32_t temp = I2C_MASTER_AGENT->SR1;
            temp = I2C_MASTER_AGENT->SR2; //Must read both SR1 and SR2 to clean ADDR flag
            (void) temp;
            *i2c_master_cb.master_driver_state = M_DRVR_WAIT_TXE;
            //Fall through to next state
        }
        else{
            break;
        }
    }

    case M_DRVR_WAIT_TXE: {
        if (I2C_MASTER_AGENT->SR1 & I2C_SR1_TXE){
            *i2c_master_cb.master_driver_state = M_DRVR_TX_SEND_BYTE;
            //Fall through to next state
        }
        else{
            break;
        }
    }

    case M_DRVR_TX_SEND_BYTE: {
        uint8_t byte = i2c_master_cb.ctx->tx_buffer[i];
        i2c_send_tx_byte(I2C_MASTER_AGENT, byte);
        i++;
        if (i < i2c_master_cb.ctx->tx_length){
            *i2c_master_cb.master_driver_state = M_DRVR_WAIT_TXE;
            return;
        }
        else{
            *i2c_master_cb.master_driver_state = M_DRVR_TX_WAIT_BTF;
            return;
        }
    }

    case M_DRVR_TX_WAIT_BTF: {
        if (I2C_MASTER_AGENT->SR1 & I2C_SR1_BTF){
            *i2c_master_cb.master_driver_state = M_DRVR_GEN_STOP;
            //Fall through to next state
        }
        else{
            break;
        }
    }

    case M_DRVR_GEN_STOP: {
        i2c_gen_stop(I2C_MASTER_AGENT);
        *i2c_master_cb.master_driver_state = M_DRVR_IDLE;
        os_sem_update(i2c_master_cb.sem);
        return;
    }

    case M_DRVR_ERROR: { //Landing in this state come from I2C_ER_Handler
        i2c_master_cb.error = 1; //Let task know error occur
        *i2c_master_cb.master_driver_state = M_DRVR_IDLE;
        os_sem_update(i2c_master_cb.sem);
        return;
    }

    }
}





#include "rtos.h"
#include "i2c_m.h"
#include "i2c_agents.h"
#include "i2c.h"
#include "timebase.h"
#include "lfsr_simple.h"
#include "stm32f446xx.h"
#include <stdint.h>

//////////////////////
//// I2C TASK FSM ////
//////////////////////
void i2c_m_task (void *arg) {
    i2c_master_control_block_t *i2c_cb = arg;//Get i2c_cb from arg;
    while (1){
        //rtos_update_counter(current_tcb->id);
        switch (*i2c_cb->master_task_state) {
        case I2C_M_TASK_IDLE: {
            uint32_t time_now = timebase_show_ms(); //Debug
            rtos_delay(5);
            time_now = timebase_show_ms(); //Debug
            (void) time_now;
            uint8_t event = lfsr_next() % 2;
            if (event>0){ //Get random event
                *i2c_cb->master_task_state = I2C_M_TASK_INITIATE_TRANS;
            }

            break;
        }

        case I2C_M_TASK_INITIATE_TRANS: {
            i2c_cb->ctx = i2c_master_prepare_transaction();
            *i2c_cb->master_task_state = I2C_M_TASK_WAIT_NOT_BUSY;
            continue;
        }

        case I2C_M_TASK_WAIT_NOT_BUSY: {
            if (!(i2c_cb->i2c_agent->SR2 & I2C_BUSY)){
                i2c_start_transaction(i2c_cb->i2c_agent);
                *i2c_cb->master_task_state = I2C_M_TASK_WAIT_TRANS_DONE;
                continue;
            }
            else{
                break;
            }
        }

        case I2C_M_TASK_WAIT_TRANS_DONE: {
            bool success = rtos_take_sem(i2c_cb->i2c_sem);
            (void) success;
            *i2c_cb->master_task_state = I2C_M_TASK_PROCEES_DATA;
            break;
        }

        case I2C_M_TASK_PROCEES_DATA: {
            if (i2c_cb->error){
                *i2c_cb->master_task_state = I2C_M_TASK_RECOVERY;
                break;
            }
            else{
                //i2c_clear_busy(I2C_MASTER_AGENT);
                *i2c_cb->master_task_state = I2C_M_TASK_IDLE;
                break;
            }
        }

        case I2C_M_TASK_RECOVERY: {
            i2c_recovery(i2c_cb->i2c_agent);
            i2c_cb->error = 0;
            *i2c_cb->master_task_state = I2C_M_TASK_IDLE;
            break;
        }

        }//switch
    }//while
}

////////////////////////
//// I2C DRIVER FSM ////
////////////////////////
//This FSM function will be called directly from I2C ISRs (EV + ER)
//The fall through in some states is to avoid situations where flag is already set
//In situation like that no new interrupt will occur later for that flag
void i2c_m_driver(i2c_master_control_block_t *i2c_cb) {
    while(1){
        switch (*i2c_cb->master_driver_state) {
        case I2C_M_DRVR_IDLE: {
            if (i2c_cb->i2c_agent->SR1 & I2C_SR1_SB) {
                i2c_cb->ctx->tx_byte_index = 0;
                *i2c_cb->master_driver_state = I2C_M_DRVR_SEND_ADDR_W;
                continue;
            } else {
                break;
            }
        }

        case I2C_M_DRVR_SEND_ADDR_W: {
            uint8_t byte = i2c_cb->ctx->addr;
            i2c_send_tx_byte(i2c_cb->i2c_agent, byte);
            *i2c_cb->master_driver_state = I2C_M_DRVR_WAIT_ADDR_W;
            break;
        }

        case I2C_M_DRVR_WAIT_ADDR_W: {
            if (i2c_cb->i2c_agent->SR1 & I2C_SR1_ADDR) {
                volatile uint32_t temp = i2c_cb->i2c_agent->SR1;
                temp = i2c_cb->i2c_agent->SR2; //Must read both SR1 and SR2 to clean ADDR flag
                (void) temp;
                *i2c_cb->master_driver_state = I2C_M_DRVR_WAIT_TXE;
                continue;
            } else {
                break;
            }
        }

        case I2C_M_DRVR_WAIT_TXE: {
            if (i2c_cb->i2c_agent->SR1 & I2C_SR1_TXE) {
                *i2c_cb->master_driver_state = I2C_M_DRVR_TX_SEND_BYTE;
                continue;
            } else {
                break;
            }
        }

        case I2C_M_DRVR_TX_SEND_BYTE: {
            uint8_t byte = i2c_cb->ctx->tx_buffer[i2c_cb->ctx->tx_byte_index];
            i2c_send_tx_byte(i2c_cb->i2c_agent, byte);
            i2c_cb->ctx->tx_byte_index++;
            if (i2c_cb->ctx->tx_byte_index < i2c_cb->ctx->tx_length) {
                *i2c_cb->master_driver_state = I2C_M_DRVR_WAIT_TXE;
                break;
            } else {
                *i2c_cb->master_driver_state = I2C_M_DRVR_TX_WAIT_BTF;
                break;
            }
        }

        case I2C_M_DRVR_TX_WAIT_BTF: {
            if (i2c_cb->i2c_agent->SR1 & I2C_SR1_BTF) {
                *i2c_cb->master_driver_state = I2C_M_DRVR_GEN_STOP;
                continue;
            } else {
                break;
            }
        }

        case I2C_M_DRVR_GEN_STOP: {
            i2c_gen_stop(i2c_cb->i2c_agent);
            *i2c_cb->master_driver_state = I2C_M_DRVR_IDLE;
            rtos_give_sem(i2c_cb->i2c_sem);
            break;
        }

        case I2C_M_DRVR_ERROR: { //Landing in this state come from I2C_ER_Handler
            i2c_cb->error = 1; //Let task know error occur
            *i2c_cb->master_driver_state = I2C_M_DRVR_IDLE;
            rtos_give_sem(i2c_cb->i2c_sem);
            break;;
        }

        }//switch
        return;
    }//while
}





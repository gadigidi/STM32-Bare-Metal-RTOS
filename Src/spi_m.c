#include "spi_m.h"
#include "spi.h"
#include "isr.h"
#include "lfsr_simple.h"
#include "rtos.h"
#include "timebase.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>


void spi_m_prepare_transaction(spi_m_control_block_t *spi_cb){
    spi_cb->ctx->command = (lfsr_next() % 256);//8bit command
    uint8_t length = 16;
    for (int i = 0; i < length; i++){
        uint8_t data = i;
        spi_cb->ctx->tx_buffer[i] = data;
    }
    spi_cb->ctx->length = length;
    spi_cb->ctx->byte_index = 0;
}

void spi_m_assert_cs(GPIO_TypeDef *GPIO, uint32_t bsrr) {
    GPIO->BSRR = bsrr;
}

void spi_m_deassert_cs(GPIO_TypeDef *GPIO, uint32_t bsrr) {
    GPIO->BSRR = bsrr;
}

void spi_m_start_transaction (spi_m_control_block_t *spi_cb){
    spi_m_assert_cs(spi_cb->spi_cs->gpio, spi_cb->spi_cs->bsrr_low);
    spi_cb->spi_agent->DR = spi_cb->ctx->command;
}

////////////////
/// TASK FSM ///
////////////////
void spi_m_task(void *arg) {
    spi_m_control_block_t *spi_cb = arg;//Get cb through arg and cast
    SPI_TypeDef *SPI = spi_cb->spi_agent;
    while (1){
        switch (*spi_cb->spi_task_state) {
        case SPI_M_TASK_IDLE: {
            uint32_t time_now = timebase_show_ms(); //Debug
            rtos_delay(3);
            time_now = timebase_show_ms(); //Debug
            (void) time_now;
            uint8_t event = lfsr_next() % 10; //Get random event
            if (event>7){
                *spi_cb->spi_task_state = SPI_M_TASK_PREPARE_TRANS;
            }
            break;
        }

        case SPI_M_TASK_PREPARE_TRANS: {
            spi_m_prepare_transaction(spi_cb);
            *spi_cb->spi_task_state = SPI_M_TASK_WAIT_RESOURCE;
            break;
        }

        case SPI_M_TASK_WAIT_RESOURCE: {
            bool success = 0;
            while (!success){
                success = rtos_mutex_try_lock(spi_cb->spi_mutex);
                if (!success){
                    rtos_delay(3);
                }
            }
            if (success){
                *spi_cb->spi_task_state = SPI_M_TASK_WAIT_NOT_BSY;
                break;
            }
        }

        case SPI_M_TASK_WAIT_NOT_BSY: {
            if (!(SPI->SR & SPI_BSY)){
                *spi_cb->spi_task_state = SPI_M_TASK_KICK_TRANS;
            }
            break;
        }

        case SPI_M_TASK_KICK_TRANS: {
            spi_m_start_transaction(spi_cb);
            *spi_cb->spi_task_state = SPI_M_TASK_WAIT_DONE;
            *spi_cb->spi_driver_state = SPI_M_DRVR_WAIT_DUMMY_RXNE; //Initiate driver
            break;
        }

        case SPI_M_TASK_WAIT_DONE: {
            spi_enable_irq(SPI, SPI_RXNEIE);
            bool success = rtos_take_sem(spi_cb->spi_sem);
            (void) success;
            //task will continue from this point after done_semaphore given from driver
            spi_cb->dbg_counter++;
            spi_m_deassert_cs(spi_cb->spi_cs->gpio, spi_cb->spi_cs->bsrr_high);
            *spi_cb->spi_task_state = SPI_M_TASK_PROCEES_DATA;
            break;
        }

        case SPI_M_TASK_PROCEES_DATA: {
            if (spi_cb->error){
                *spi_cb->spi_task_state = SPI_M_TASK_RECOVERY;
                break;
            }
            else{
                rtos_mutex_unlock(spi_cb->spi_mutex);
                *spi_cb->spi_task_state = SPI_M_TASK_IDLE;
                break;
            }
        }

        case SPI_M_TASK_RECOVERY: {
            spi_recovery(SPI);
            spi_cb->error = 0;
            rtos_mutex_unlock(spi_cb->spi_mutex);
            *spi_cb->spi_task_state = SPI_M_TASK_IDLE;
            break;
        }

        }//switch
    }//while
}


///////////////////
/// DRIVER FSM ////
///////////////////

//NOTE: when using 'break' it's exit from the switch and then hit the 'return' at the bottom and exit function
//when using 'continue' it's hit the 'while' again and then enter to the switch again in the same call
void spi_m_txrx_driver(spi_m_control_block_t *spi_cb) {
    SPI_TypeDef *SPI = spi_cb->spi_agent;
    //iteration++;
    while(1){
        switch (*spi_cb->spi_driver_state) {
        case SPI_M_DRVR_IDLE: {
            //When task want to initiate transaction the state will be changed to SPI_M_DRVR_WAIT_TXE
            //Then SPI ISR will continue to advance the FSM according to HW events
            break;
        }

        case SPI_M_DRVR_WAIT_DUMMY_RXNE: {
            if (SPI->SR & SPI_RXNE) { //Wait RXNE = 1
                spi_disable_irq(SPI, SPI_RXNEIE);
                uint32_t dummy = SPI->DR;
                (void) dummy;
                *spi_cb->spi_driver_state = SPI_M_DRVR_PRE_TXE;
                continue; //Continue to next state in this call
            } else {
                break;
            }
        }

        case SPI_M_DRVR_PRE_TXE: {
            *spi_cb->spi_driver_state = SPI_M_DRVR_WAIT_TXE;
            spi_enable_irq(SPI, SPI_TXEIE);
            break;
        }

        case SPI_M_DRVR_WAIT_TXE: {
            if ((SPI->SR & SPI_TXE)) { //Wait for TXE=1
                int byte_index = spi_cb->ctx->byte_index;
                uint32_t tx_byte = spi_cb->ctx->tx_buffer[byte_index];
                SPI->DR = tx_byte;
                spi_disable_irq(SPI, SPI_TXEIE);
                *spi_cb->spi_driver_state = SPI_M_DRVR_WAIT_RXNE;
                spi_enable_irq(SPI, SPI_RXNEIE);
            }
            break;
        }

        case SPI_M_DRVR_WAIT_RXNE: {
            if (SPI->SR & SPI_RXNE) { //Wait RXNE = 1
                spi_disable_irq(SPI, SPI_RXNEIE);
                *spi_cb->spi_driver_state = SPI_M_DRVR_READ_DR;
                continue;
            } else {
                break;
            }
        }

        case SPI_M_DRVR_READ_DR: {
            int byte_index = spi_cb->ctx->byte_index;
            uint8_t rx_byte = SPI->DR;
            spi_cb->ctx->rx_buffer[byte_index] = rx_byte; //Reda DR
            *spi_cb->spi_driver_state = SPI_M_DRVR_BYTE_DONE;
            continue; //Continue to next state in this call
        }

        case SPI_M_DRVR_BYTE_DONE: {
            spi_cb->ctx->byte_index++;
            if (spi_cb->ctx->byte_index < spi_cb->ctx->length) {
                *spi_cb->spi_driver_state = SPI_M_DRVR_PRE_TXE;
                continue;
            } else {
                *spi_cb->spi_driver_state = SPI_M_DRVR_FINISH;
                continue; //Continue to next state in this call
            }
        }

        case SPI_M_DRVR_ERROR_HANDLER: {
            spi_cb->error = 1;
            *spi_cb->spi_driver_state = SPI_M_DRVR_FINISH;
            continue; //Continue to next state in this call
        }

        case SPI_M_DRVR_FINISH: {
            *spi_cb->spi_driver_state = SPI_M_DRVR_IDLE;
            rtos_give_sem(spi_cb->spi_sem);
            break;
        }

        } //switch
        return;
    } //while
}





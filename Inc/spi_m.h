#ifndef SPI_M_H_
#define SPI_M_H_

#include <stdint.h>
#include <stdbool.h>
#include "rtos.h"
#include "stm32f446xx.h"

typedef struct {
    GPIO_TypeDef *gpio;
    uint32_t bsrr_low;
    uint32_t bsrr_high;
} spi_cs_t;

typedef enum {
    SPI_MASTER, SPI_SLAVE,
} spi_mode_t;

typedef enum {
    SPI_M_TASK_IDLE, SPI_M_TASK_PREPARE_TRANS,
    SPI_M_TASK_WAIT_RESOURCE, SPI_M_TASK_WAIT_NOT_BSY,
    SPI_M_TASK_KICK_TRANS, SPI_M_TASK_WAIT_DONE,
    SPI_M_TASK_PROCEES_DATA, SPI_M_TASK_RECOVERY,
} spi_m_task_state_t;

typedef enum {
    SPI_M_DRVR_IDLE,
    SPI_M_DRVR_WAIT_DUMMY_RXNE,
    SPI_M_DRVR_PRE_TXE,
    SPI_M_DRVR_WAIT_TXE,
    SPI_M_DRVR_WAIT_RXNE,
    SPI_M_DRVR_READ_DR,
    SPI_M_DRVR_BYTE_DONE,
    SPI_M_DRVR_ERROR_HANDLER,
    SPI_M_DRVR_FINISH,
} spi_m_driver_state_t;

typedef struct {
    uint8_t command;
    volatile uint8_t length;
    int byte_index;
    uint8_t *tx_buffer;
    uint8_t *rx_buffer;
} spi_ctx_t;

typedef struct {
    SPI_TypeDef *spi_agent;
    spi_cs_t *spi_cs;
    spi_mode_t spi_mode;
    int spi_irqn;
    volatile spi_m_task_state_t *spi_task_state;
    volatile spi_m_driver_state_t *spi_driver_state;
    semaphore_t *spi_sem;
    mutex_t *spi_mutex;
    spi_ctx_t *ctx;
    uint8_t error;
    uint32_t dbg_counter;
} spi_m_control_block_t;

void spi_m_assert_cs(GPIO_TypeDef *GPIO, uint32_t bsrr);
void spi_m_deassert_cs(GPIO_TypeDef *GPIO, uint32_t bsrr);

void spi_m_task(void *arg);
void spi_m_txrx_driver(spi_m_control_block_t *spi_cb);

void spi_m_prepare_transaction(spi_m_control_block_t *spi_cb);
void spi_m_start_transaction(spi_m_control_block_t *spi_cb);

#endif /* SPI_M_H_ */




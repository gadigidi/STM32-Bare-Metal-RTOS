#include "spi_agents.h"
#include "spi_m.h"
#include "spi.h"
#include "rtos.h"
#include "timebase.h"
#include "isr.h"
#include "gpio.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>


///////////////////////
////// SPI2 INIT //////
///////////////////////
spi_m_control_block_t spi2_cb;
static spi_cs_t spi2_cs;
static spi_ctx_t spi2_ctx;
static uint8_t tx_buffer[SPI_BUF_DEPTH];
static uint8_t rx_buffer[SPI_BUF_DEPTH];
static spi_m_task_state_t spi2_task_state = SPI_M_TASK_IDLE;
static volatile spi_m_driver_state_t spi2_driver_state = SPI_M_DRVR_IDLE;
void spi2_gpio_init(void) {
    gpio_init(GPIOBEN);

    //Config CS GPIO as output
    gpio_pin_mode(SPI2_CS_GPIO, SPI2_CS_PIN, GPIO_MODE_OUTPUT);

    //Config SPI2_SCK (PB13) as AF 5
    gpio_pin_mode(SPI2_GPIO, SPI2_SCK, GPIO_MODE_AF);
    gpio_pin_afr(SPI2_GPIO, SPI2_SCK, 5);

    //Config SPI2_MOSI (PB15) as AF 5
    gpio_pin_mode(SPI2_GPIO, SPI2_MOSI, GPIO_MODE_AF);
    gpio_pin_afr(SPI2_GPIO, SPI2_MOSI, 5);

    //Config SPI2_MISO (PB14) as AF 5
    gpio_pin_mode(SPI2_GPIO, SPI2_MISO, GPIO_MODE_AF);
    gpio_pin_afr(SPI2_GPIO, SPI2_MISO, 5);
}

void spi2_primitives_init(void){
    spi2_cb.spi_agent = SPI2;
    spi2_cb.spi_cs = &spi2_cs;
    spi2_cb.spi_cs->gpio = SPI2_CS_GPIO;
    spi2_cb.spi_cs->bsrr_low = SPI2_CS_LOW_BSRR;
    spi2_cb.spi_cs->bsrr_high = SPI2_CS_HIGH_BSRR;
    spi2_cb.spi_mode = SPI_MASTER;
    spi2_cb.spi_irqn = SPI2_IRQn;
    spi2_cb.spi_task_state = &spi2_task_state;
    spi2_cb.spi_driver_state = &spi2_driver_state;
    spi2_cb.spi_sem = &spi2_done_sem;
    spi2_cb.spi_mutex = &spi2_mutex;
    spi2_cb.ctx = &spi2_ctx;
    spi2_cb.ctx->tx_buffer = tx_buffer;
    spi2_cb.ctx->rx_buffer = rx_buffer;
    spi2_cb.ctx->length = 0;
    spi2_cb.error = 0;
    spi2_cb.dbg_counter = 0;
}

void spi2_init(void) {
    spi2_primitives_init();
    spi2_gpio_init();
    spi_enable_clock(SPI2, SPI2_CLK_EN);
    spi_config_baudrate(SPI2, SPI_BR_DIV64);
    spi_config_mode(SPI2, SPI_MASTER_CONF);
    spi_config_clock_mode(SPI2, SPI_MODE_0);
    spi_config_bit_order(SPI2, SPI_MSB_FIRST);
    spi_config_nss(SPI2, SPI_NSS_SOFTWARE);
    spi_config_data_size(SPI2, SPI_DATASIZE_8BIT);
    spi_enable(SPI2, SPI_ENABLE);

    //Enable interrupts
    isr_enable(SPI2_IRQn);
    //spi_enable_irq(SPI2, SPI_ERRIE);
}




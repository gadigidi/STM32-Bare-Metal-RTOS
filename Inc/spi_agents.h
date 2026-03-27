#ifndef SPI_AGENTS_H_
#define SPI_AGENTS_H_

#include "spi_m.h"

#define SPI2_GPIO               GPIOB
#define SPI2_SCK                13U //PA5
#define SPI2_MOSI               15U //PA7
#define SPI2_MISO               14U //PA6

#define SPI2_CS_GPIO            GPIOA
#define SPI2_CS_PIN             4U //CS output connected to PA4
#define SPI2_CS_HIGH_BSRR       (1U<<SPI2_CS_PIN)
#define SPI2_CS_LOW_BSRR        (1U<<(SPI2_CS_PIN+16))

//CR1 REG
#define SPI2_CLK_EN             (1U<<14)
#define SPI_BR_DIV64            7U //SPI2_CLK_HZ = APB2CLK_HZ/64

void spi2_gpio_init(void);
void spi2_init(void);
void spi2_primitives_init(void);

extern spi_m_control_block_t spi2_cb;
extern uint32_t spi2_dbg_counter;

#endif /* SPI_AGENTS_H_ */

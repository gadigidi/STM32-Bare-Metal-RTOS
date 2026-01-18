#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <stdbool.h>

#define APB2CLK_HZ          16000000U
//#define SPI1_PSC          (5U<<3) //SPI1_CLK_HZ = APB2CLK_HZ/64

#define SPI1_SCK            5U //PA5
#define SPI1_MOSI           7U //PA7
#define SPI1_MISO           6U //PA6

#define SPI_BR_DIV64        7U //SPI1_CLK_HZ = APB2CLK_HZ/64
#define SPI_MASTER          1U
#define SPI_SLAVE           0
#define SPI_MODE_0          0
#define SPI_MSB_FIRST       0
#define SPI_NSS_SOFTWARE    3U
#define SPI_DATASIZE_8BIT   0

#define SPI1EN              (1U<<12)


void spi1_init(void);
void spi1_config_baudrate(uint8_t br);
void spi1_enable_clock(void);
void spi1_config_mode(uint8_t mode);
void spi1_config_clock_mode(uint8_t clock_mode);
void spi1_config_bit_order(uint8_t order);
void spi1_config_nss(uint8_t nss);
void spi1_config_data_size(uint8_t data_size);
void spi1_enable(uint8_t enable);

uint8_t spi1_txrx_byte(uint8_t data);

#endif /* SPI_H_ */

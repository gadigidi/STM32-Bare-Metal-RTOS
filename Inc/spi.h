#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

#define APB2CLK_HZ              16000000U

//SR REG
#define SPI_OVR                 (1U<<6)
#define SPI_RXNE                (1U<<0)
#define SPI_TXE                 (1U<<1)
#define SPI_BSY                 (1U<<7)
//CR1
#define SPI_ENABLE              (1U<<6)

#define SPI_MASTER_CONF         1U
#define SPI_SLAVE_CONF          0
#define SPI_MODE_0              0
#define SPI_MSB_FIRST           0
#define SPI_NSS_SOFTWARE        3U
#define SPI_DATASIZE_8BIT       0

#define SPI_BUF_DEPTH           64

#define SPI_TXEIE               (1U<<7)
#define SPI_RXNEIE              (1U<<6)
#define SPI_ERRIE               (1U<<5)
#define SPI_IRQ_ENABLE          (7U<<5) //Enable all interrupts


void spi_config_baudrate(SPI_TypeDef *SPI, uint32_t br);
void spi_enable_clock(SPI_TypeDef *SPI, uint32_t enable);
void spi_config_mode(SPI_TypeDef *SPI, uint32_t mode);
void spi_config_clock_mode(SPI_TypeDef *SPI, uint32_t clock_mode);
void spi_config_bit_order(SPI_TypeDef *SPI, uint32_t order);
void spi_config_nss(SPI_TypeDef *SPI, uint32_t nss);
void spi_config_data_size(SPI_TypeDef *SPI, uint32_t data_size);
void spi_enable(SPI_TypeDef *SPI, uint32_t enable);

void spi_enable_irq(SPI_TypeDef *SPI, uint32_t value);
void spi_disable_irq(SPI_TypeDef *SPI, uint32_t value);

bool spi_check_error(SPI_TypeDef *SPI);
void spi_recovery(SPI_TypeDef *SPI);


uint8_t spi_txrx_byte(SPI_TypeDef *SPI, uint8_t data);



#endif /* SPI_H_ */




#include "spi.h"
#include "gpio.h"
#include "isr.h"
#include "timebase.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>

void spi_enable_clock(SPI_TypeDef *SPI, uint32_t enable) {
    //Enable clock access to SPI
    if (SPI == SPI1){
        RCC->APB2ENR |= enable;
    }
    else {
        RCC->APB1ENR |= enable;
    }
}

void spi_config_baudrate(SPI_TypeDef *SPI, uint32_t br) {
    //Config SPI1 clk frequency
    SPI->CR1 &= ~(7U << 3);
    SPI->CR1 |= (br << 3);
}

void spi_config_clock_mode(SPI_TypeDef *SPI, uint32_t clock_mode) {
    SPI->CR1 &= ~(3U << 0);
    SPI->CR1 |= (clock_mode << 0);
}

void spi_config_mode(SPI_TypeDef *SPI, uint32_t mode) {
    SPI->CR1 &= ~(1U << 2);
    SPI->CR1 |= (mode << 2);
}

void spi_config_bit_order(SPI_TypeDef *SPI, uint32_t order) {
    SPI->CR1 &= ~(1U << 7);
    SPI->CR1 |= (order << 7);
}

void spi_config_nss(SPI_TypeDef *SPI, uint32_t nss) {
    SPI->CR1 &= ~(3U << 8);
    SPI->CR1 |= (nss << 8);
}

void spi_config_data_size(SPI_TypeDef *SPI, uint32_t data_size) {
    SPI->CR1 &= ~(1U << 11);
    SPI->CR1 |= (data_size << 11);
}

void spi_enable(SPI_TypeDef *SPI, uint32_t enable) {
    //SPI->CR1 &= ~enable;
    SPI->CR1 |= enable;
}

void spi_enable_irq(SPI_TypeDef *SPI, uint32_t value){
    SPI->CR2 |= value;
}

void spi_disable_irq(SPI_TypeDef *SPI, uint32_t value){
   SPI->CR2 &= ~value;
}

bool spi_check_error(SPI_TypeDef *SPI){
    if (SPI->SR & SPI_OVR){
        return 1;
    }
    else{
        return 0;
    }
}

void spi_recovery(SPI_TypeDef *SPI){
    uint32_t dummy = SPI->DR;
    dummy = SPI->SR;
    (void) dummy;
}

//Blocking SPI transaction
//Used just for device init configurations before tasks start to run
uint8_t spi_txrx_byte(SPI_TypeDef *SPI, uint8_t data) {
    while (!(SPI->SR & (1U << 1))) {} //wait for TXE=1
    SPI->DR = data;
    while (!(SPI->SR & (1U << 0))) {} //wait for RXNE=1
    uint8_t rx = SPI->DR;
    //while ((SPI->SR & (1U << 0))) {} //wait for RXNE=0 to ensure flag was cleaned}
    while ((SPI->SR & (1U << 7))) {} //wait for BSY=0
    return rx;
}


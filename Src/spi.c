#include "spi.h"
#include "gpio.h"
#include "timebase.h"
#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>

void spi1_init(void) {
    gpio_init(GPIOAEN);

    //Config SPI1_SCK as AF 5
    gpio_pin_mode(GPIOA, SPI1_SCK, GPIO_MODE_AF);
    gpio_pin_afr(GPIOA, SPI1_SCK, 5);

    //Config SPI_MOSI as AF 5
    gpio_pin_mode(GPIOA, SPI1_MOSI, GPIO_MODE_AF);
    gpio_pin_afr(GPIOA, SPI1_MOSI, 5);

    //Config SPI_MISO as AF 5
    gpio_pin_mode(GPIOA, SPI1_MISO, GPIO_MODE_AF);
    gpio_pin_afr(GPIOA, SPI1_MISO, 5);
}

void spi1_config_baudrate(uint8_t br) {
    //Config SPI1 clk frequency
    SPI1->CR1 &= ~(7U << 3);
    SPI1->CR1 |= (br << 3);
}

void spi1_enable_clock(void) {
    //Enable clock access to SPI1
    RCC->APB2ENR |= SPI1EN;
}

void spi1_config_mode(uint8_t mode) {
    SPI1->CR1 &= ~(1U << 2);
    SPI1->CR1 |= (mode << 2);
}

void spi1_config_clock_mode(uint8_t clock_mode) {
    SPI1->CR1 &= ~(3U << 0);
    SPI1->CR1 |= (clock_mode << 0);
}

void spi1_config_bit_order(uint8_t order) {
    SPI1->CR1 &= ~(1U << 7);
    SPI1->CR1 |= (order << 7);
}

void spi1_config_nss(uint8_t nss) {
    SPI1->CR1 &= ~(3U << 8);
    SPI1->CR1 |= (nss << 8);
}

void spi1_config_data_size(uint8_t data_size) {
    SPI1->CR1 &= ~(1U << 11);
    SPI1->CR1 |= (data_size << 11);
}

void spi1_enable(uint8_t enable) {
    SPI1->CR1 &= ~(1U << 6);
    SPI1->CR1 |= (enable << 6);
}

uint8_t spi1_txrx_byte(uint8_t data) {
    while (!(SPI1->SR & (1U << 1))) { //wait for TXE=1
    }
    SPI1->DR = data;
    while (!(SPI1->SR & (1U << 0))) { //wait for RXNE=1
    }
    uint8_t rx = SPI1->DR;
    while ((SPI1->SR & (1U << 0))) { //wait for RXNE=0 to ensure flag was cleaned
    }
    while ((SPI1->SR & (1U << 7))) { //wait for BSY=0
    }
    return rx;
}


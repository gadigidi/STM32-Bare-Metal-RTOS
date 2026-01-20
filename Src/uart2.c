#include "gpio.h"
#include <stdint.h>
#include <uart2.h>
#include "stm32f446xx.h"

//Functions prototypes
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

int __io_putchar(int ch) {
    uart2_write(ch);
    return ch;
}

void uart2_tx_init(void) {

    ///////////////////////////////
    /***congigure uart gpio pin***/
    //set PA2 mode to alternate fuction mode
    gpioa_pin_mode(2, 1U);

    //set PA2 alternate function type to UART_TX (AF07)
    gpioa_pin_afr(2, 7U);
    /////////////////////////////

    /////////////////////////////
    /***congigure uart module***/

    //enable clk to uart2
    RCC->APB1ENR |= UART2EN;

    //configure baudrate
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    //configure the transfer direction and enable uart module
    USART2->CR1 |= CR1_TE;
    USART2->CR1 |= CR1_UE;
    /////////////////////////////
}

void uart2_rx_init(void) {
    ///////////////////////////////
    /***congigure uart gpio pin***/

    //set PA3 mode to alternate fuction mode
    gpioa_pin_mode(3, 1U);

    //set PA3 alternate function type to UART_TX (AF07)
    gpioa_pin_afr(3, 7U);
    /////////////////////////////

    /////////////////////////////
    /***congigure uart module***/

    //enable clk to uart2
    RCC->APB1ENR |= UART2EN;

    //configure baudrate
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    //configure the transfer direction and enable uart module
    USART2->CR1 |= CR1_RE;
    USART2->CR1 |= CR1_UE;
    /////////////////////////////
}

void uart2_txrx_init(void) {
    //Congigure uart gpio pin
    //Set PA2 mode to alternate fuction mode
    gpio_pin_mode(GPIOA, 2, 1U);

    //Set PA2 alternate function type to UART_TX (AF07)
    gpio_pin_afr(GPIOA, 2, 7U);

    //Set PA3 mode to alternate fuction mode
    gpio_pin_mode(GPIOA, 3, 1U);

    //Set PA3 alternate function type to UART_TX (AF07)
    gpio_pin_afr(GPIOA, 3, 7U);

    //Congigure uart module
    //Enable clk to uart2
    RCC->APB1ENR |= UART2EN;

    //Configure baudrate
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    //Configure the transfer direction and enable uart module
    USART2->CR1 |= CR1_TE;
    USART2->CR1 |= CR1_RE;
    USART2->CR1 |= CR1_UE;
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate) {
    USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate) {
    return ((PeriphClk + (BaudRate / 2U)) / BaudRate);
}

void uart2_write(int ch) {
    //Make sure the transmit data register is empty
    while (!(USART2->SR & SR_TXE)) {} //Wait for SR to be empty FIXME
    //Write to transmit data register
    USART2->DR = (ch & 0xFF);
}

char uart2_read(void) {
    //make sure the recieve data register is NOT empty
    while (!(USART2->SR & SR_RXNE)) {} //wait for SR to be empty FIXME
      //write to transmit data register
    return USART2->DR;
}


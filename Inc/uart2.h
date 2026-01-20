#ifndef UART2_H_
#define UART2_H_

#define UART2EN             (1U<<17)

#define CR1_TE              (1U<<3)
#define CR1_RE              (1U<<2)
#define CR1_UE              (1U<<13)

#define SR_TXE              (1U<<7)
#define SR_RXNE             (1U<<5)

#define SYS_FREQ            16000000
#define APB1_CLK            SYS_FREQ
#define UART_BAUDRATE       115200

void uart2_init(void);
void uart2_tx_init(void);
void uart2_rx_init(void);
void uart2_txrx_init(void);
void uart2_write(int ch);
char uart2_read(void);

#endif /* UART2_H_ */


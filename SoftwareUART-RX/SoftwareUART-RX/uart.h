#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define UART_RX_BUF_SIZE 64
#define UART_RX_BUF_MASK (UART_RX_BUF_SIZE - 1)

void Timer0_init();
void PCINT_init();

uint8_t uart_getc();

#endif /* UART_H_ */
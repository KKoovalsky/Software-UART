#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint8_t UART_TxHead;
extern volatile uint8_t UART_TxTail;

void Timer0_init();

void uart_send_byte(char data);

#endif /* UART_H_ */
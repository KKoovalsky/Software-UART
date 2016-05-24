#include <avr/io.h>
#include "uart.h"

int main(void)
{
	// PB0 as output.
	DDRB |= (1<<PB0);
	
	// Set PB0 pin.
	PORTB |= (1<<PB0);
	
	// Init Timer 0.
	Timer0_init();
	
	char data[] = "Kacper to musi dzialac bo jak nie to lipa. Szybkie miganie ma byc!";
	    
    while (1) {
		while(UART_TxHead != UART_TxTail);
		char *ptr = data;
		while(*ptr) {
			uart_send_byte(*ptr);
			ptr++;
		}
    }
}


#include "uart.h"
#include <util/atomic.h>


int main(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		CLKPR = (1<<CLKPCE);
		CLKPR = (0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
	}
	
	// PB0 as output.
	DDRB |= (1<<PB0);
	
	// Set PB0 pin.
	PORTB |= (1<<PB0);
	
	// Init Timer 0.
	Timer0_init();
	
	char data[] = "Kacper to musi dzialac bo jak nie to lipa. Szybkie miganie.";
	
	sei();
	    
    while (1) {
		while(UART_TxHead != UART_TxTail);
		char *ptr = data;
		while(*ptr) {
			uart_send_byte(*ptr);
			ptr++;
		}
    }
}


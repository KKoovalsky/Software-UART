#include "uart.h"

int main(void)
{
	// PB0 as input.
	DDRB &= ~(1<<PB0);
	
	// VCC pullup.
	PORTB |= (1<<PB0);
	
	Timer0_init();
	PCINT_init();
    
	sei();
	
    while (1) {
		char data = uart_getc();
		if(data) {
			// TODO
		}
    }
}


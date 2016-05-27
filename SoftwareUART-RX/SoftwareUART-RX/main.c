#include "uart.h"
#include <util/atomic.h>

int main(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		CLKPR = (1<<CLKPCE);
		CLKPR = (0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
	}
	
	// PB0 as input.
	DDRB &= ~(1<<PB0);
	
	// VCC pullup.
	//PORTB |= (1<<PB0);
	
	Timer0_init();
	PCINT_init();
    
	sei();
	
    while (1) {
		char data = uart_getc();
		char data_swp;
		if(data) {
			data_swp = data;
			data = 0x00;
		}
    }
}


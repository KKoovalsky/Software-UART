#include "uart.h"

#define UART_BAUD_RATE 9600.0

#define UART_NUM_DATA_BITS (8)

#define UART_STOP_BITS (1)

#define UART_NUM_BITS (1 + UART_NUM_DATA_BITS)

#define UART_PIN_STATE (PINB & (1<<PB0))

#define UART_TCNT_MARGIN (uint8_t)((F_CPU / 8.0 / UART_BAUD_RATE - 0.5) * 0.8)

#define TCNT_CLEAR (TCNT0 = 0)

#define CHECK_TIM_ENA (TIMSK & (1<<OCIE0A))

#define CHECK_OCM_FLAG_SET (TIFR & (1<<OCF0A))

#define CLEAR_OCM_FLAG (TIFR |= (1<<OCF0A))

volatile uint8_t cycles_not_int = 0xFF;
volatile uint8_t byte_rec;
volatile uint8_t bits_rec_cnt;

volatile uint8_t UART_RxHead;
volatile uint8_t UART_RxTail;
volatile uint8_t UART_RxBuf[UART_RX_BUF_SIZE];

inline void Timer0_start() {
	TIMSK |= (1<<OCIE0A);
	TCNT_CLEAR;
	CLEAR_OCM_FLAG;
}

inline void Timer0_stop() {
	TIMSK &= ~(1<<OCIE0A);
	TCNT_CLEAR;
	CLEAR_OCM_FLAG;
}

inline void UART_save(char * data) {
	UART_RxHead = (UART_RxHead + 1) & UART_RX_BUF_MASK;
	UART_RxBuf[UART_RxHead] = *data;
}

void Timer0_init() {
	
	// CTC mode.
	TCCR0A |= (1<<WGM01);
	
	// Clock prescaler : 8
	TCCR0B |= (1<<CS01);
	
	// OCR for 9600 Baudrate.
	OCR0A = (uint8_t)(F_CPU / 8.0 / UART_BAUD_RATE - 0.5);
}

void PCINT_init() {
	// Enable external interrupt.
	GIMSK |= (1<<PCIE);
	
	// Enable external interrupt on PCINT0 pin
	PCMSK |= (1<<PCINT0);
	
}

 ISR(PCINT0_vect) {
	 	//	Firstly read TCNT content to avoid inconsistency.
	 	uint8_t TCNT_content = TCNT0;
		
		//	Clear TCNT register.
		TCNT_CLEAR;
		 
		//	Start period count timer, when start bit's falling edge detected.
		if(!CHECK_TIM_ENA) {
			Timer0_start();
			return;
		}
	 
	 	//	If TCNT content is almost equal OCR content it is understood, that next bit was received.
	 	cycles_not_int += (TCNT_content > UART_TCNT_MARGIN ? 1 : 0);
		 
		//	If both edge and compare match took place:
		if(CHECK_OCM_FLAG_SET) {
			// To clear flag write logical one to it.
			CLEAR_OCM_FLAG;
			// When TCNT set flag and got cleared.
			if(TCNT_content < UART_TCNT_MARGIN) cycles_not_int++;
		}
	 
	 	//	Interpreting data sent.
	 	if(!UART_PIN_STATE) byte_rec |= ((1 << cycles_not_int) - 1) << bits_rec_cnt;
	 
	 	bits_rec_cnt += cycles_not_int;
	 	cycles_not_int = 0;
	 
	 	if(bits_rec_cnt == UART_NUM_DATA_BITS) {
		 		Timer0_stop();
		 		bits_rec_cnt = 0;
		 		cycles_not_int = 0xFF; //	Initialized with '-1', to avoid interpreting start bit as data bit.
				
				// Save data.
				UART_save((char*)&byte_rec);
				
				byte_rec = 0;
		 		return;
	 	}
 }
 
 ISR(TIMER0_COMPA_vect) {
	 	//	1 bit period ended
	 	cycles_not_int += 1;
	 
	 	//	When last bit logic zero.
	 	if(bits_rec_cnt + cycles_not_int == UART_NUM_DATA_BITS && UART_PIN_STATE) {
		 		Timer0_stop();
				byte_rec |= ((1 << cycles_not_int) - 1) << bits_rec_cnt;
		 		cycles_not_int = 0xFF;
		 		bits_rec_cnt = 0;
				
				UART_save((char *)&byte_rec);
				 
		 		byte_rec = 0;
	 	}
 }
 
 uint8_t uart_getc() {
	 if(UART_RxHead == UART_RxTail) return 0;
	 UART_RxTail = (UART_RxTail + 1) & UART_RX_BUF_MASK;
	 return UART_RxBuf[UART_RxTail];
 }


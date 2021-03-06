#include "uart.h"

#define UART_BAUD_RATE 9600.0

#define UART_NUM_DATA_BITS 8

#define UART_STOP_BITS 1

#define UART_NUM_BITS (1 + UART_NUM_DATA_BITS)

// MUST BE - multiple of two.
#define UART_TX_BUF_SIZE 64

#define UART_TX_BUF_MASK (UART_TX_BUF_SIZE - 1)

#define UART_PIN_SET (PORTB |= (1<<PB0))
#define UART_PIN_CLEAR (PORTB &= ~(1<<PB0))

#define TIM0_INT_EN TIMSK |= (1<<OCIE0A)
#define TIM0_INT_DIS TIMSK &= ~(1<<OCIE0A)

volatile uint8_t UART_TxHead;
volatile uint8_t UART_TxTail;
volatile char UART_TxBuf[UART_TX_BUF_SIZE];

volatile uint8_t now_br;

volatile uint8_t stop_bits;

void Timer0_init() {
	
	// CTC mode.
	TCCR0A |= (1<<WGM01);
	
	// Clock prescaler : 8
	TCCR0B |= (1<<CS01);
	
	// OCR for 9600 Baudrate.
	OCR0A = (uint8_t)(F_CPU / 8.0 / UART_BAUD_RATE - 0.5);
}

void uart_send_byte(char data) {
	if(UART_TxHead == UART_TxTail && !(TIMSK & (1<<OCIE0A))) {
		 stop_bits = UART_STOP_BITS;
		 TIM0_INT_EN;
	}
	UART_TxHead  = (UART_TxHead + 1) & UART_TX_BUF_MASK;
	UART_TxBuf[UART_TxHead] = data;
}

ISR(TIMER0_COMPA_vect) {
	static uint8_t bit_sent_cnt = UART_NUM_BITS;
	
	if(stop_bits)	{
		stop_bits--;
		return;
	}
	
	if(!bit_sent_cnt) {
		bit_sent_cnt = UART_NUM_BITS;
		UART_PIN_SET;
		if(UART_TxHead != UART_TxTail) {
			stop_bits = UART_STOP_BITS - 1;
			return;
		}
		stop_bits = UART_STOP_BITS;
		TIM0_INT_DIS;
		return;
	}
	
	if(bit_sent_cnt == UART_NUM_BITS) {
		
		UART_TxTail = (UART_TxTail + 1) & UART_TX_BUF_MASK;
		now_br = UART_TxBuf[UART_TxTail];
		
		bit_sent_cnt--;
		UART_PIN_CLEAR;
		return;
	}
	
	bit_sent_cnt --;
	(now_br & (1 << (UART_NUM_DATA_BITS - bit_sent_cnt - 1))) ? UART_PIN_SET : UART_PIN_CLEAR;		
}
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/interrupt.h>
#include <util/atomic.h>


#define REG_ACCESS(mem_addr)    (*(volatile uint8_t *)(mem_addr))
#define SET_DDRB(_val)           (REG_ACCESS(0x24) = _val)
#define SET_PORTB(_val)          (REG_ACCESS(0x25) = _val)

// 16MHz - the oscilator used in Arduino UNO
#define OSC     16000000UL
// Desired baud rate used in the serial port
#define BAUD    9600
// According to the datasheet
#define ubrr    ((uint8_t)((OSC/(16*(long)BAUD))-1))

#define TIMER_TOP ((OSC / 1000) / 8)

// expects 'data' to be a null-terminated string.
void write_serial(unsigned char data[]) {
    int i = 0;

	while(data[i] != 0) {
		while (!(UCSR0A & (1<<UDRE0))); 
		UDR0 = data[i];
		i++;
	}
	
	// terminate string with '\n\r'
	 while (!(UCSR0A & (1<<UDRE0)));
	 UDR0 = '\n';
	 while (!(UCSR0A & (1<<UDRE0)));
	 UDR0 = '\r';
	 //while (!(UCSR0A & (1<<UDRE0)));
}

void setup_uart(void) {
	// Set Baudrate
	UBRR0H = (ubrr>>8);
	UBRR0L = (ubrr);
	// frame format: 8bit data, 1 stop bit
	UCSR0C = 0x06;
	// enable TX
	UCSR0B = (1<<TXEN0);
}

void setup_io(void) {
	// PORTB Pin 5 as output
	SET_DDRB(1<<0x5);
}

void setup_timer1(void) {
	// set mode and prescaler clk/8
	TCCR1B |= (1<<WGM12) | (1<<CS11);

	// set the top value, first lower half and then high half
	OCR1AH = TIMER_TOP >> 8;
	OCR1AL = TIMER_TOP & 0xFF;

	// enable interrupt
	TIMSK1 |= 1 << OCIE1A;
}

volatile uint16_t counter_millis;

ISR(TIMER1_COMPA_vect) {
    counter_millis++;
}


uint16_t millis() {
    uint16_t cp;
    // Ensure the copy is atomic
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        cp = counter_millis;
    }
 
    return cp;
}

void toggle_led(void) {
	static int toggle;
	static uint16_t last_read;

	if((millis() - last_read) < 500) {
		return;
	}

	last_read = millis();
    
    if(toggle == 0) {
      SET_PORTB(0x00);
      toggle = 1;
    } else {
      SET_PORTB(1<<0x5);
      toggle = 0;
    }
}

int main() {
	setup_uart();
	setup_io();
	setup_timer1();

	// enable interrupts
	sei();

	uint16_t last_read = 0;
	
	char msg[7];

	sprintf(msg, "Begin\n");
	write_serial(msg);

	for(;;) {
		toggle_led();

		// do stuff
		sprintf(msg, "%u", millis());
		write_serial(msg);
	}
}
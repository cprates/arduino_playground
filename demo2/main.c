#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/interrupt.h>
#include <util/atomic.h>


#define REG_ACCESS(mem_addr)    (*(volatile uint8_t *)(mem_addr))
#define SET_DDRB(_val)           (REG_ACCESS(0x24) = _val)
#define SET_PORTB(_val)          (REG_ACCESS(0x25) = _val)

// 16MHz - chrystal value used in Arduino UNO
#define OSC         16000000UL
//                          Millis / Prescaler config                            
#define TIMER_TOP   ((OSC / 1000) / 8)

void setup_io(void) {
	// PORTB Pin 5 as output
	SET_DDRB(1<<0x5);
}

void setup_timer1(void) {
	// set mode and prescaler clk/8
	TCCR1B |= (1<<WGM12) | (1<<CS11);

	// set the TOP value, first lower half and then high half
	OCR1AH = TIMER_TOP >> 8;
	OCR1AL = TIMER_TOP & 0xFF;

	// enable interrupt
	TIMSK1 |= 1 << OCIE1A;

	// PC0/Analog 0 to Output
    //DDRC |= 1 << PC0;
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

int main(void) {
	setup_io();
	setup_timer1();

	// enable interrupts
	sei();
	
	for(;;) {
		toggle_led();

		// do stuff
	}	
}
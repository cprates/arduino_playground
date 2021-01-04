#include <stdint.h>


#define REG_ACCESS(mem_addr)    (*(volatile uint8_t *)(mem_addr))
#define SET_DDRB(_val)           (REG_ACCESS(0x24) = _val)
#define SET_PORTB(_val)          (REG_ACCESS(0x25) = _val)

void setup_io(void) {
	// PORTB Pin 5 as output
	SET_DDRB(1<<0x5);
}

int main() {
	setup_io();
	
	int toggle = 0;
	for(;;) {
	    if(toggle == 0) {
			SET_PORTB(0x00);
			toggle = 1;
	    } else {
			SET_PORTB(1<<0x5);
			toggle = 0;
	    }

	    for(long n = 0; n < 1500000; n++) {}
	}	
}

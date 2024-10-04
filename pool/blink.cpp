
#include <avr/io.h>	// for register definitions, like PORTB and so forth...
#include <util/delay.h>

#define F_CPU	16000000UL	// must be defined for _delay_ms and _delay_us functions to work properly

int blink(void)
{
	//	Enable output to LED port
	//DDRB |= 0b00100000;
	DDRB |= _BV(DDB5);

	//while(1){
		PORTB |= _BV(PORTB5);
		_delay_ms(125);
		PORTB &= ~_BV(PORTB5);
		_delay_ms(375);
	//}

	// Turn on LED
	//PORTB = 0b00100000;

	//_delay_ms(1250);

	//PORTB = 0b00000000;

	return 1;
}


#include <stdio.h>	// for data types, like uint8_t...

#include "ssd1306.h"

//#include <stdlib.h>
//#include <string.h>

//int blink(void);



int main(void)
{
	ssd1306		oled;

	uint8_t		lightUp				= 1;
	uint8_t		displayTwiAddress	= 0x3c;
	uint8_t		width				= 128;
	uint8_t		height				= 64;


  	//	Enable output to LED port
	DDRB |= 0b00100000;

	// Blink LED to visualise start of program
	oled.twi.blinkLED(0b00100000, 3, 1);
	_delay_us(500000);

	oled.initDisplay(displayTwiAddress, width, height);

	while(1){
	}
}

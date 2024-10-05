
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>


#include "ssd1306.h"



int main(void)
{
	ssd1306		oled;

	uint8_t		lightUp				= 1;		// we assume we're gonna be ok
	uint8_t		displayTwiAddress	= 0x3c;
	uint8_t		width				= 128;
	uint8_t		height				= 64;
	uint8_t 	contrast			= 48;		// 0-255


  	//	Enable output to LED port
	DDRB |= 0b00100000;

	// Blink LED to visualise start of program
	oled.twi.blinkLED(0b00100000, 7, 0);
	_delay_ms(500);
	oled.twi.blinkLED(0b00100000, 3, 1);
	_delay_ms(500);


	// initialise the display, and check the feedback...
	switch(oled.initDisplay(displayTwiAddress, width, height, contrast)){
		case 1:						oled.twi.blinkLED(0b00100000, 2, 1);
									lightUp = 0;
									break;

		case 2:						oled.twi.blinkLED(0b00100000, 3, 1);
									lightUp = 0;
									break;

		case 3:						oled.twi.blinkLED(0b00100000, 5, 1);
									lightUp = 0;
									break;

		case 4:						oled.twi.blinkLED(0b00100000, 7, 1);
									lightUp = 0;
									break;

		case 5:						oled.twi.blinkLED(0b00100000, 9, 1);
									lightUp = 0;
									break;

		case MEMORY_ALLOC_ERROR:	// if we're not able to allocate memory for the buffer, flash the LED 13 times fast
									oled.twi.blinkLED(0b00100000, 13, 0);
									lightUp = 0;
									break;

		case TWI_INIT_OK:			// we got through initDisplay without errors
									oled.twi.blinkLED(0b00100000, 1, 1);
									_delay_ms(500);
									break;

		default:					oled.twi.blinkLED(0b00100000, 20, 1);
									lightUp = 0;
									break;

	}

	// if we're good, light up the LED
	if(lightUp)
		PORTB = 0b00100000;



	uint8_t		text[25];

	oled.clearDisplay();
	sprintf((char*)text, "0: OLED-test program");
	oled.print(0, 0, text, strlen((char*)text));
	sprintf((char*)text, "1: by Jon Leithe - 2024");
	oled.print(0, 1, text, strlen((char*)text));
	sprintf((char*)text, "2: Barebone C++ on ardu");
	oled.print(0, 2, text, strlen((char*)text));
	sprintf((char*)text, "3: Waterflow x l/m");
	oled.print(0, 3, text, strlen((char*)text));

	oled.display();

	while(1){
	}
}

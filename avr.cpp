/*
 * main.c
 *
 *  Created on: 18 Oct 2020
 *      Author: jole
 */



#define F_CPU	16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

//#include "yfs401/yfs401.h"
#include "ssd1306/ssd1306.h"



volatile	uint64_t	reset			= 0;
volatile	float		totalTap		= 0;
volatile	float		lastTap			= 0;
static		double		pulsesPerLiter	= 5026.0;
			ssd1306		oled;
			uint8_t		text[64];
unsigned	char		liter[24];
volatile	bool		interrupted		= true;
			bool		sleepMode		= false;
volatile	bool		writeToEeprom	= false;
volatile	uint16_t	counter			= 0;



ISR(INT1_vect)
{
	//pulses++;
	//waterTotal	+= (1 / pulsesPerLiter);

	SMCR = 0b00000000;


	if(!oled.displayVisible){
		//oled.initDisplay(0x3c, 128, 64);
		oled.displayOn();
	}
	lastTap		+= (1 / pulsesPerLiter);
	totalTap	+= (1 / pulsesPerLiter);

	interrupted	= true;
	counter		= 0;
}



ISR(INT0_vect)
{
	SMCR = 0b00000000;

	if(!oled.displayVisible){
		//oled.initDisplay(0x3c, 128, 64);
		oled.displayOn();
	}

	lastTap			= 0;
	writeToEeprom	= true;

	reset++;
}



int main(void)
{
	uint8_t		lightUp				= 1;
	uint8_t		displayTwiAddress	= 0x3c;
	uint8_t		width				= 128;
	uint8_t		height				= 64;

	//yfs401		flowMeter;



	//	Enable output to LED port
	DDRB |= 0b00100000;

	// Blink LED to visualise start of program
	oled.twi.blinkLED(0b00100000, 13, 0);
	_delay_us(500000);


	// Here goes the main code...

	// initialise the display, and check the feedback...
	switch(oled.initDisplay(displayTwiAddress, width, height)){

		case 1:					oled.twi.blinkLED(0b00100000, 3, 1);
								lightUp = 0;
								break;
		case 2:					oled.twi.blinkLED(0b00100000, 4, 1);
								lightUp = 0;
								break;
		case 3:					oled.twi.blinkLED(0b00100000, 5, 1);
								lightUp = 0;
								break;
		case 4:					oled.twi.blinkLED(0b00100000, 6, 1);
								lightUp = 0;
								break;
		case 5:					oled.twi.blinkLED(0b00100000, 7, 1);
								lightUp = 0;
								break;

		case MEMORY_ALLOC_ERROR:	// if we're not able to allocate memory for the buffer, flash the LED 13 times fast
									oled.twi.blinkLED(0b00100000, 13, 0);
									lightUp = 0;
									break;

		case TWI_INIT_OK:			// we got through initDisplay without errors
									oled.twi.blinkLED(0b00100000, 3, 0);
									break;



		default:					oled.twi.blinkLED(0b00100000, 20, 1);
									lightUp = 0;
									break;

	}

	// read from eeprom
	totalTap = (eeprom_read_byte((uint8_t*)0) << 8 | eeprom_read_byte((uint8_t*)2)) + ((eeprom_read_byte((uint8_t*)4))/100.0);


	DDRC	= 0xff;
	DDRD	= 0xe0;

	// enable global interrupts - see p.20
	SREG |= 0b10000000;

	// enable interrupt INT0 and INT1 on rising edge - see p.80
	EICRA	|=	0b00001010;

	// enable external interrupt INT0 and INT1 - see p.81
	EIMSK	|=	0b00000011;

	// enable PCINT0 interrupt see p.83
	//PCMSK0	|= (1 << PCINT0);
	//PCIFR	|= (1 << PCIF0);

	// see p.82
	//PCICR	|= (1 << PCIE0);


	/* Enable change of Interrupt Vectors */
	//MCUCR |= (1<<IVCE);
	/* Move interrupts to Boot Flash section */
	//MCUCR |= (1<<IVSEL);

	sei();

	if(lightUp)
		PORTB = 0b00100000;

	while(1){
		interrupted	= false;
		sleepMode	= false;

		/*
		dtostrf(pulses, 11, 1, (char*)liter);
		sprintf((char*)text, "Pulses:    %s", liter);
		oled.print(0, 0, text, strlen((char*)text));
		*/

		// Update display
		dtostrf((totalTap), 9, 2, (char*)liter);
		sprintf((char*)text, "Total tapped: %s", liter);
		oled.print(0, 2, text, strlen((char*)text));

		dtostrf((lastTap), 8, 3, (char*)liter);
		sprintf((char*)text, "Last tap:  %s", liter);
		oled.print(4, 4, text, strlen((char*)text));

		sprintf((char*)text, "Sverdrup Research");
		oled.print(4, 0, text, strlen((char*)text));

		sprintf((char*)text, "*** Best i ALT ***");
		oled.print(3, 7, text, strlen((char*)text));

		if(!interrupted){
			counter++;
		}

		if(counter > 750){
			//oled.twi.blinkLED(0b00100000, 3, 0);
			counter = 751;
			if(oled.displayVisible)
				oled.displayOff();

			if(!sleepMode){
				//PRR = 0b01101111;
				// power down mode
				SMCR = 0b00000100;

				// enable  sleep
				SMCR |= 0b00000001;
				sleepMode = true;

				// call sleep mode in assembler
				__asm__  __volatile__("sleep");
			}

		}

		// update display if we should be visible
		if(oled.displayVisible)
			oled.display();

		if(writeToEeprom){

			cli();


			uint16_t	totalTapIntegerPart			= (uint16_t)floor(totalTap);
			uint8_t		totalTapIntegerPartHighByte	= totalTapIntegerPart >> 8;
			uint8_t		totalTapIntegerPartLowByte	= totalTapIntegerPart;
			uint8_t		totalTapDecimalPart			= ((totalTap - totalTapIntegerPart) * 100);

			eeprom_write_byte((uint8_t*)0, totalTapIntegerPartHighByte);
			eeprom_write_byte((uint8_t*)2, totalTapIntegerPartLowByte);
			eeprom_write_byte((uint8_t*)4, totalTapDecimalPart);

			/*
			oled.clearDisplay();

			dtostrf((totalTapIntegerPart), 4, 0, (char*)liter);
			sprintf((char*)text, "IntegerPart: %s", liter);
			oled.print(0, 0, text, strlen((char*)text));

			dtostrf((totalTapIntegerPartHighByte), 4, 0, (char*)liter);
			sprintf((char*)text, "HighByte: %s", liter);
			oled.print(0, 1, text, strlen((char*)text));

			dtostrf((totalTapIntegerPartLowByte), 4, 0, (char*)liter);
			sprintf((char*)text, "LowByte: %s", liter);
			oled.print(0, 2, text, strlen((char*)text));

			dtostrf((totalTapDecimalPart), 4, 0, (char*)liter);
			sprintf((char*)text, "DecimalPart: %s", liter);
			oled.print(0, 3, text, strlen((char*)text));

			// restoring value
			float totalWaterTapped = (totalTapIntegerPartHighByte << 8 | totalTapIntegerPartLowByte) +  totalTapDecimalPart/100.0;
			//totalWaterTapped += totalTapDecimalPart/100.;

			dtostrf((totalWaterTapped), 6, 2, (char*)liter);
			sprintf((char*)text, "Restored: %s", liter);
			oled.print(0, 5, text, strlen((char*)text));

			oled.display();
			_delay_ms(5250);
			*/

			/*
			eeprom_write_byte((uint8_t*)0, integerPart1);
			eeprom_write_byte((uint8_t*)1, integerPart2);
			eeprom_write_byte((uint8_t*)2, integerPart3);
			eeprom_write_byte((uint8_t*)3, integerPart4);

			eeprom_write_byte((uint8_t*)4, decimalPart1);
			eeprom_write_byte((uint8_t*)5, decimalPart2);
			eeprom_write_byte((uint8_t*)6, decimalPart3);
			eeprom_write_byte((uint8_t*)7, decimalPart4);




			*/
			/*
			dtostrf((integerPart3), 4, 0, (char*)liter);
			sprintf((char*)text, "intPart3: %s", liter);
			oled.print(0, 2, text, strlen((char*)text));
			dtostrf((integerPart4), 4, 0, (char*)liter);
			sprintf((char*)text, "intPart4: %s", liter);
			oled.print(0, 3, text, strlen((char*)text));
			dtostrf((langVerdi), 11, 0, (char*)liter);
			sprintf((char*)text, "decPart1 %s", liter);
			oled.print(0, 4, text, strlen((char*)text));
			oled.display();
			_delay_ms(5250);


			dtostrf((decimalPart1), 4, 0, (char*)liter);
			sprintf((char*)text, "decPart1 %s", liter);
			oled.print(0, 4, text, strlen((char*)text));
			dtostrf((decimalPart2), 4, 0, (char*)liter);
			sprintf((char*)text, "decPart2 %s", liter);
			oled.print(0, 5, text, strlen((char*)text));
			dtostrf((decimalPart3), 4, 0, (char*)liter);
			sprintf((char*)text, "decPart3 %s", liter);
			oled.print(0, 6, text, strlen((char*)text));
			dtostrf((decimalPart4), 4, 0, (char*)liter);
			sprintf((char*)text, "decPart4 %s", liter);
			oled.print(0, 7, text, strlen((char*)text));

			oled.display();
			*/




			//eepromWrite(0x00, integerPart1);
			//eepromWrite(0x01, integerPart2);
			//eepromWrite(0x02, integerPart3);
			//eepromWrite(0x03, integerPart4);

			//eepromWrite(0x04, decimalPart1);
			//eepromWrite(0x05, decimalPart2);
			//eepromWrite(0x06, decimalPart3);
			//eepromWrite(0x07, decimalPart4);

			writeToEeprom = false;
			//_delay_ms(3250);

			//oled.clearDisplay();
			//oled.display();

			sei();
		}

	}


	return 1;
}

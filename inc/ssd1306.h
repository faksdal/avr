/*
 *	ssd1306.h
 *
 *	Created on: 31 Oct 2020
 *	Author: jole
 *
 *	According to the data sheet 'SSD1306 Application Note' p.5, the initialization
 *	sequence of the display is as follows: (page numbers relates to 'SSD1306 Advance Information')
 *		- set MUX ratio
 *		- set display offset
 *		- set display start line
 *		- set segment re-map
 *		- set COM output scan direction
 *		- set COM pins hardware configuration
 *		- set contrast control
 *		- disable entire display On, 0xa4
 *		- set normal display
 *		- set oscillator frequency
 *		- enable charge pump regulator
 *		- display On
 *
 */

#ifndef __SSD1306_SSD1306_H__
#define __SSD1306_SSD1306_H__

#include <ssd1306/ssd1306CommandSet.h>
#include "twi328P/twi328P.h"


#define MEMORY_ALLOC_ERROR	0b10101010


class ssd1306 {

	uint8_t*	buffer;
	uint16_t	bufferSize, bufferIndex;
	uint8_t		slaveAddress, displayWidth, displayHeight;


public:
	volatile bool	displayVisible;

	twi328P twi;

	ssd1306();

	uint8_t	initDisplay(uint8_t _slaveAddress, uint8_t _displayWidth, uint8_t _displayHeight);



	void	drawPixel(int16_t _x, int16_t _y, uint16_t _color);
	void	print(int16_t _x, int16_t _y, uint8_t _text[], uint8_t len);
	void	clearDisplay(void);
	uint8_t getDisplayWidth(void) { return(displayWidth);}
	uint8_t getDisplayHeight(void) {return(displayHeight);}

	volatile void	displayOff(void);
	volatile void	displayOn(void);

	uint8_t	display(void);

};

#endif /* ATMEGA328P_WATERFLOWSVERDRUP_SRC_SSD1306_SSD1306_H_ */

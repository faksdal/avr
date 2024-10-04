/*
 * ssd1306.cpp
 *
 *  Created on: 31 Oct 2020
 *      Author: jole
 */

#include <stdlib.h>
#include <string.h>
#include "ssd1306/ssd1306.h"
#include "graphics/font5x7.h"



//
//	Function:		ssd1306::ssd1306()
//
//	Description:	- constructor for creating the object
//
//	arguments:		none
//
//	return value:	none
//
//
ssd1306::ssd1306()
{
}



volatile void ssd1306::displayOff(void)
{
	// Set display off, p.30
	uint8_t cmdList[] = {SET_DISPLAY_OFF};
	twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);

	displayVisible = false;

}



volatile void ssd1306::displayOn(void)
{
	// Set display on, p.30
	uint8_t cmdList[] = {SET_DISPLAY_ON};
	twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);

	displayVisible = true;
}



//
//	Function:		ssd1306::initDisplay()
//
//	Description:	- allocates memory for the display buffer, returning an error if we fail
//					- sets the private variables to hold the display width and height
//					- clears the display buffer by writing them all to zero
//					- calls twi.initBus() to initialise the twi bus
//
//	arguments:
//
//	return value:
//
//
uint8_t ssd1306::initDisplay(uint8_t _slaveAddress, uint8_t _displayWidth, uint8_t _displayHeight)
{

	bufferSize = (_displayWidth * (_displayHeight / 8));

	buffer = (uint8_t*) malloc(bufferSize);
	if(!buffer){
		twi.blinkLED(0b00100000, bufferSize, 0);
		return(MEMORY_ALLOC_ERROR);
	}

	displayWidth	= _displayWidth;
	displayHeight	= _displayHeight;
	slaveAddress	= _slaveAddress;
	bufferIndex		= 0;

	// Set up the twi bus
	twi.initBus(400000UL);

	//	Initialisation sequence...
	{	// Set display off, p.30
		uint8_t cmdList[] = {SET_DISPLAY_OFF};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set MUX ratio, p.33
		uint8_t cmdList[] = {CMD_SET_MUX_RATIO, MUX_RATIO_RESET};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set display offset, p.34
		uint8_t cmdList[] = {CMD_SET_DISPLAY_OFFSET, DISPLAY_OFFSET_RESET};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set memory addressing mode, p.33
		uint8_t cmdList[] = {CMD_SET_MEMORY_ADDRESSING_MODE, SET_HORIZONTAL_ADDRESSING_MODE};
		//uint8_t cmdList[] = {CMD_SET_MEMORY_ADDRESSING_MODE, SET_VERTICAL_ADDRESSING_MODE};
		//uint8_t cmdList[] = {CMD_SET_MEMORY_ADDRESSING_MODE, SET_PAGE_ADDRESSING_MODE};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set column start and end addresses, p.33
			uint8_t cmdList[] = {CMD_SET_COLUMN_ADDRESS, SET_START_COLUMN_RESET, SET_END_COLUMN_RESET};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set page start and end addresses, p.33
			uint8_t cmdList[] = {CMD_SET_PAGE_ADDRESS, SET_START_PAGE_RESET, SET_END_PAGE_RESET};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set segment re-map, p.33
		// We have the display in a re-mapped configuration
		uint8_t cmdList[] = {SET_SEGMENT_MAP_REMAPPED};
		//uint8_t cmdList[] = {SET_SEGMENT_MAP_NORMAL};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set COM output scan direction, p.34
		// We have the display in a re-mapped configuration
		//uint8_t cmdList[] = {SET_COM_OUTPUT_SCAN_NORMAL};
		uint8_t cmdList[] = {SET_COM_OUTPUT_SCAN_REMAPPED};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set display start line, or GDDRAM start page address, p.33
		uint8_t cmdList[] = {CMD_SET_GDDRAM_START_PAGE_0};
		//uint8_t cmdList[] = {CMD_SET_DISPLAY_START_LINE_7};
		//uint8_t cmdList[] = {64};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set COM pins hardware configuration, p.34
		uint8_t cmdList[] = {CMD_SET_COM_PINS_HWCONF, SET_COM_PINS_HWCONF_RESET};		// normal small letter, writes every line
		//uint8_t cmdList[] = {CMD_SET_COM_PINS_HWCONF, SET_COM_PINS_HWCONF_EN_SEQ};	// bigger letters, writes every alternate line
		//uint8_t cmdList[] = {CMD_SET_COM_PINS_HWCONF, SET_COM_PINS_HWCONF_ALT_EN};	// something happened to vertical offset
		//uint8_t cmdList[] = {CMD_SET_COM_PINS_HWCONF, SET_COM_PINS_HWCONF_DIS_SEQ};		// bigger letters, writes every alternate line

		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set contrast control, p.30
		uint8_t cmdList[] = {CMD_SET_CONTRAST_CONTROL, SET_CONTRAST_RESET};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Disable entire display on, p.30
		uint8_t cmdList[] = {DISPLAY_RAM_CONTENT};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set normal/inverse display, p.30
		uint8_t cmdList[] = {SET_NORMAL_DISPLAY};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set display clock divide ratio/oscillator frequency, p.34
		//uint8_t cmdList[] = {CMD_SET_CLOCK_DIVIDE_RATIO_OSC_FREQ, 0b10000000};
		uint8_t cmdList[] = {CMD_SET_CLOCK_DIVIDE_RATIO_OSC_FREQ, (CLOCK_DIVIDE_RATIO_RESET | (OSC_FREQ_RESET << 4))};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set charge pump regulator, p.35
		uint8_t cmdList[] = {CMD_SET_CHARGE_PUMP, CHARGE_PUMP_ENABLE, CHARGE_PUMP_DISPLAY_ON};
		//uint8_t cmdList[] = {CMD_SET_CHARGE_PUMP, 0x14, 0xaf};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	{	// Set display on, p.30
		uint8_t cmdList[] = {SET_DISPLAY_ON};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}

	displayVisible = true;

	clearDisplay();
	display();

	return(TWI_INIT_OK);
}
//	END ssd1306::initDisplay()



//
//	Function:		ssd1306::print()
//
//	Description:	writes out text at the specified location in the display buffer
//
//	arguments:		int16_t x:		start x-location
//					int16_t y:		start y-location
//					uint8_t *_text:	text to be written
//					uint8_t _len:	length of the string
//
//	return value:	none
//
//
void ssd1306::print(int16_t _x, int16_t _y, uint8_t _text[], uint8_t _len)
{
	//	position the bufferIndex at the co-ordinates x and y
	//	look up the text, letter by letter, in the font, an write out the
	//	five elements that makes up one character

	//	Calculate bufferIndex
	bufferIndex = ((5*_x) + (displayWidth * _y));

	//uint8_t *c = _text;

	//*c -= 32;
	//twi.blinkLED(0b00100000, c, 1);

	uint8_t	*buf = _text;
	uint8_t count = _len;

	while(count--){
		for(int i = 0; i < 5; i++){
			buffer[bufferIndex++]	= (font5x7[(uint8_t)*buf-32][i]);
		}
		*buf++;
	}


	//character = _text[0]-32;
/*
	buffer[bufferIndex]		= font5x7[c][0];
	buffer[bufferIndex++]	= font5x7[c][1];
	buffer[bufferIndex++]	= font5x7[8][2];
	buffer[bufferIndex++]	= font5x7[8][3];
	buffer[bufferIndex++]	= font5x7[8][4];


	buffer[bufferIndex++]	= font5x7[9][0];
	buffer[bufferIndex++]	= font5x7[9][1];
	buffer[bufferIndex++]	= font5x7[9][2];
	buffer[bufferIndex++]	= font5x7[9][3];
	buffer[bufferIndex++]	= font5x7[9][4];
	*/

	/*{	// Set display start line, or GDDRAM start page address, p.33
		uint8_t cmdList[] = {64};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}*/
/*
	// START condition
		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
		while(!(TWCR & (1 <<TWINT))){
			//twi.blinkLED(0b00100000, 3, 1);
		}
		if((TWSR & 0xf8) != 0x08)
			return;

		// SLA+W
		TWDR = TW_WRITE;
		TWDR |= (slaveAddress << 1);
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 4, 1);
		}
		if((TWSR & 0xf8) != 0x18)
			return;

		// Write to GDDRAM command
		TWDR = 0x40;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return;



		// Write out the display buffer to GDDRAM
		uint8_t		*buf	= buffer;
		uint16_t	count	= bufferSize;
		while(count--){
			TWDR = *buf++;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 3, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return;
		}

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);
*/

}



//
//	Function:		ssd1306::display()
//
//	Description:	refresh display, putting content of RAM onto the display
//
//	arguments:		none
//
//	return value:	none
//
//
uint8_t ssd1306::display(void)
{
	// TODO:	Send this to the twi module, should be in its own function
/*
	{	// Set display start line, p.33
			uint8_t cmdList[] = {CMD_SET_DISPLAY_START_LINE_0};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}
*/
			// START condition
			TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
			while(!(TWCR & (1 <<TWINT))){
				//twi.blinkLED(0b00100000, 3, 1);
			}
			if((TWSR & 0xf8) != 0x08)
				return(1);

			// SLA+W
			TWDR = TW_WRITE;
			TWDR |= (slaveAddress << 1);
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 4, 1);
			}
			if((TWSR & 0xf8) != 0x18)
				return(2);

			// Write to GDDRAM command
			TWDR = 0x40;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 5, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(4);



			// Write out the display buffer to GDDRAM
			uint8_t		*buf	= buffer;
			uint16_t	count	= bufferSize;
			while(count--){
				TWDR = *buf++;
				TWCR = (1 << TWINT) | (1 << TWEN);
				while(!(TWCR & (1 << TWINT))){
					//twi.blinkLED(0b00100000, 3, 1);
				}
				if((TWSR & 0xf8) != 0x28)
					return(5);
			}

			// STOP condition
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
			_delay_us(100);

			//
			return(0);

}
// END ssd1306::display()



//
//	Function:		ssd1306::clearDisplay()
//
//	Description:	clear display memory buffer, writing it to all 0's
//
//	arguments:		none
//
//	return value:	none
//
//
void ssd1306::clearDisplay(void)
{
	// clear the buffer
	memset((uint8_t*) buffer, 0, bufferSize);
}
// END ssd1306::clearDisplay(



//
//	Function:		ssd1306::drawPixel()
//
//	Description:	draw one pixel at location x, y in display ram buffer
//
//	arguments:		int16_t x:		x-location of pixel to be drawn
//					int16_t y:		y-location of pixel to be drawn
//					uint16_t color:	color of pixel to be drawn. The SSD1306 doesn't support colors. We can only draw them in WHITE or BLACK
//
//	return value:	none
//
//
void ssd1306::drawPixel(int16_t x, int16_t y, uint16_t color) {

	// if we have no buffer, we should return
	if(!buffer)
		return;

	//twi.blinkLED(0b00100000, 5, 1);
	//_delay_us(500000);
	//twi.blinkLED(0b00100000, 1, 0);

	if ((x >= 0) && (x <displayWidth) && (y >= 0) && (y < displayHeight)) {

		// Pixel is in-bounds. Rotate coordinates if needed.
		/*
		switch (getRotation()) {
			case 1:		ssd1306_swap(x, y);
						x = WIDTH - x - 1;
						break;

			case 2:		x = WIDTH - x - 1;
						y = HEIGHT - y - 1;
						break;

			case 3:		ssd1306_swap(x, y);
						y = HEIGHT - y - 1;
						break;
    	}
	    */
		switch (color){
			case 1 /*SSD1306_WHITE*/:	buffer[x + (y / 8) * displayWidth] |= (1 << (y & 7));
										break;

			case 0 /*SSD1306_BLACK*/:	buffer[x + (y / 8) * displayWidth] &= ~(1 << (y & 7));
										break;

			case 2 /*SSD1306_INVERSE*/:	buffer[x + (y / 8) * displayWidth] ^= (1 << (y & 7));
										break;
		}
	}


}
// END ssd1306::drawPixel()






/*
 // REFRESH DISPLAY ---------------------------------------------------------

//	!
//	@brief	Push data currently in RAM to SSD1306 display.
//	@return	None (void).
//	@note	Drawing operations are not visible until this function is
//			called. Call after each graphics command, or after a whole set
//			of graphics commands, as best needed by one's own application.

void Adafruit_SSD1306::display(void) {
  TRANSACTION_START
  static const uint8_t PROGMEM dlist1[] = {
    SSD1306_PAGEADDR,
    0,                         // Page start address
    0xFF,                      // Page end (not really, but works here)
    SSD1306_COLUMNADDR,
    0 };                       // Column start address
  ssd1306_commandList(dlist1, sizeof(dlist1));
  ssd1306_command1(WIDTH - 1); // Column end address

#if defined(ESP8266)
  // ESP8266 needs a periodic yield() call to avoid watchdog reset.
  // With the limited size of SSD1306 displays, and the fast bitrate
  // being used (1 MHz or more), I think one yield() immediately before
  // a screen write and one immediately after should cover it.  But if
  // not, if this becomes a problem, yields() might be added in the
  // 32-byte transfer condition below.
  yield();
#endif
  uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
  uint8_t *ptr   = buffer;
  if(wire) { // I2C
    wire->beginTransmission(i2caddr);
    WIRE_WRITE((uint8_t)0x40);
    uint8_t bytesOut = 1;
    while(count--) {
      if(bytesOut >= 32) { // Wire uses 32-byte transfer blocks max
        wire->endTransmission();
        wire->beginTransmission(i2caddr);
        WIRE_WRITE((uint8_t)0x40);
        bytesOut = 1;
      }
      WIRE_WRITE(*ptr++);
      bytesOut++;
    }
    wire->endTransmission();
  } else { // SPI
    SSD1306_MODE_DATA
    while(count--) SPIwrite(*ptr++);
  }
  TRANSACTION_END
#if defined(ESP8266)
  yield();
#endif
}

  */

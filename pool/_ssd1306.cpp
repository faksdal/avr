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




ssd1306::ssd1306()
{
	slaveAddress = 0b00000000;
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
uint8_t ssd1306::initDisplay(uint8_t _slaveAddress, uint16_t _displayWidth, uint16_t _displayHeight)
{

	bufferSize = (_displayWidth * ((_displayHeight /*+ 7*/) / 8));
	//twi.blinkLED(0b00100000, bufferSize, 0);

	buffer = (uint8_t*) malloc(bufferSize);
	if(!buffer){
		twi.blinkLED(0b00100000, bufferSize, 0);
		return(MEMORY_ALLOC_ERROR);
	}

	displayWidth	= _displayWidth;
	displayHeight	= _displayHeight;
	slaveAddress	= _slaveAddress;

	// Clear the buffer
	//clearDisplay();

	// Set up the twi bus
	twi.initBus(400000UL);
	//twi.blinkLED(0b00100000, 2, 1);


		// Set display off **************************************************

	{
		uint8_t cmdList[] = {SET_DISPLAY_OFF};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);

		//twi.blinkLED(0b00100000, 1, 1);
	}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set display on command
		TWDR = 0b10101110;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End Set display off
		//twi.blinkLED(0b00100000, 1, 1);
*/

	// Set MUX ratio **************************************************
	{
		uint8_t cmdList[] = {CMD_SET_MUX_RATIO, MUX_RATIO_RESET};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set MUX command
		TWDR = 0b10101000;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// MUX ratio
		TWDR = 0b111111;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 7, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(5);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End MUX
		//twi.blinkLED(0b00100000, 1, 1);
*/

	// Set display offset **************************************************
	{
		uint8_t cmdList[] = {CMD_SET_DISPLAY_OFFSET, DISPLAY_OFFSET_RESET};
		twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}
/*		// START condition
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set display offset command
		TWDR = 0b11010011;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// Display offset set to reset value
		TWDR = 0b000000;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 7, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(5);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);
*/
		// End display offset
		//twi.blinkLED(0b00100000, 1, 1);

		// Set memory addressing mode **************************************************
		{
			uint8_t cmdList[] = {0b00100000, 0b00000000};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
		// START condition
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set Set memory addressing mode command
		TWDR = 0b00100000;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// Memory addressing mode value
		TWDR = 0b00000000;		// horizontal addressing mode
		//TWDR = 0b00000001;	// vertical addressing mode
		//TWDR = 0b00000010;	// page addressing mode
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 7, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(5);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End Set memory addressing mode
		//twi.blinkLED(0b00100000, 1, 1);
*/



		// Set display start line **************************************************
		{
			uint8_t cmdList[] = {0b01000000};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
/*		// START condition
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set display start line command
		//TWDR = 0b01000000;
		TWDR = 128;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End set display start line
		//twi.blinkLED(0b00100000, 1, 1);
*/

		// Set segment re-map **************************************************
		{
			uint8_t cmdList[] = {0b10100001};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
					return(3);

		// Set segment re-map command
		TWDR = 0b10100001;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End set segment re-map
		//twi.blinkLED(0b00100000, 1, 1);
*/
		// Set COM output scan direction **************************************************
		{
			uint8_t cmdList[] = {0b11001000};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set COM output scan direction command
		TWDR = 0b11001000;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End Set COM output scan direction
		//twi.blinkLED(0b00100000, 1, 1);
*/
		// Set COM pins hardware configuration **************************************************
		{
			uint8_t cmdList[] = {0b11011010, 0b00010010};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set COM pins hardware configuration command
		TWDR = 0b11011010;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// COM pins hardware configuration reset value
		TWDR = 0b00010010;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End Set COM pins hardware configuration
		//twi.blinkLED(0b00100000, 1, 1);
*/

		// Set contrast control **************************************************
		{
			uint8_t cmdList[] = {0b10000001, 0x7f};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set contrast control command
		TWDR = 0b10000001;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// contrast reset value 0x7f, 0b1111111
		TWDR = 0x7f;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(5);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End set contrast control
		//twi.blinkLED(0b00100000, 1, 1);
*/

		// Disable entire display on **************************************************
		{
			uint8_t cmdList[] = {0b10100100};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Display RAM content
		TWDR = 0b10100100;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End Disable entire display on
		//twi.blinkLED(0b00100000, 1, 1);
*/

		// Set normal/inverse display **************************************************
		{
			uint8_t cmdList[] = {0b10100110};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set normal/inverse display command
		TWDR = 0b10100110;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End set normal display
		//twi.blinkLED(0b00100000, 1, 1);
*/

		// Set display clock divide ratio/oscillator frequency **************************************************
		{
			uint8_t cmdList[] = {0b11010101, 0b10000000};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set display clock divide ratio/oscillator frequency command
		TWDR = 0b11010101;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// display clock divide ratio/oscillator frequency value
		TWDR = 0b10000000;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(5);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End Set display clock divide ratio/oscillator frequency
		//twi.blinkLED(0b00100000, 1, 1);
*/

		// Set charge pump regulator **************************************************
		{
			uint8_t cmdList[] = {0b10001101, 0x14, 0xaf};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set charge pump regulator command
		TWDR = 0b10001101;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// charge pump enable value
		TWDR = 0x14;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(5);

		// charge pump display on value
		TWDR = 0xaf;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(6);


		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End Set charge pump regulator
		//twi.blinkLED(0b00100000, 1, 1);
*/
		// Set display on **************************************************
		{
			uint8_t cmdList[] = {0b10101111};
			twi.writeCommandList(slaveAddress, sizeof(cmdList), cmdList);
		}
/*
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

		// Command byte
		TWDR = 0x00;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(3);

		// Set display on command
		TWDR = 0b10101111;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End Set display on
		//twi.blinkLED(0b00100000, 1, 1);
*/
		//twi.blinkLED(0b00100000, 5, 1);
		/************************************************************************************************/
		// Try to write some data **************************************************
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


		// position the screen pointer at the right location

		// Write GDDRAM command
		TWDR = 0x40;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 5, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);


		// clear screen
		// TODO: should write to a screen buffer, and have a function to draw the entire buffer to GDDRAM
		for(int i = 0; i < 1024; i++){
			TWDR = 0b00000000;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
		}

		// Data to display
		//for(int i = 0; i < 42; i++){

			TWDR = 0b01111110;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b00010001;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);

			TWDR = 0b00010001;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b00010001;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b01111110;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);

			TWDR = 0b01000100;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b00101000;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b00010000;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b00101000;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b01000100;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);

			TWDR = 0b00000000;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b01111110;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b01010010;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b01000010;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b00000000;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);

			TWDR = 0b00000000;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b01111110;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b01000000;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b01000000;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);
			TWDR = 0b01000000;
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 << TWINT))){
				//twi.blinkLED(0b00100000, 6, 1);
			}
			if((TWSR & 0xf8) != 0x28)
				return(5);

		//}

		// STOP condition
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
		_delay_us(100);

		// End trying write data
		//twi.blinkLED(0b00100000, 1, 1);



		/************************************************************************************************/


	return(TWI_INIT_OK);
}
//	END ssd1306::initDisplay()



//
//	Function:		ssd1306::displayCommand()
//
//	Description:
//
//	arguments:
//
//	return value:
//
//
uint8_t ssd1306::displayCommand(uint16_t _size, uint8_t *_cmdList)
{
	return(0);
	/*
	// START condition
	//if(twi.sendStartCondition() == 1)
	//	return(1);
	//twi.blinkLED(0b00100000, 3, 1);

	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while(!(TWCR & (1 <<TWINT))){
		//twi.blinkLED(0b00100000, 3, 1);
	}
	if((TWSR & 0xf8) != 0x08)
		return(1);


	// SLA+W
	//if(twi.sendSlaW(slaveAddress) == 2)
	//	return(2);
	TWDR = TW_WRITE;
	TWDR |= (slaveAddress << 1);
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT))){
		//twi.blinkLED(0b00100000, 4, 1);
	}
	if((TWSR & 0xf8) != 0x18)
		return(2);


	//twi.blinkLED(0b00100000, 3, 1);

	// Command byte
	//twi.writeCommandList(_size, _cmdList);
	//twi.blinkLED(0b00100000, 3, 1);

	TWDR = 0x00;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT))){
		//twi.blinkLED(0b00100000, 5, 1);
	}
	if((TWSR & 0xf8) != 0x28)
		return(3);

	// _cmdList
	while(_size--){
		TWDR = *_cmdList++;
		TWCR = (1 << TWINT) | (1 << TWEN);
		while(!(TWCR & (1 << TWINT))){
			//twi.blinkLED(0b00100000, 6, 1);
		}
		if((TWSR & 0xf8) != 0x28)
			return(4);
	}



	// STOP condition
	//twi.sendStopCondition();
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

	_delay_us(100);

	return(0);
	*/
}




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
//	Function:		ssd1306::display()
//
//	Description:	refresh display, putting content of RAM onto the display
//
//	arguments:		none
//
//	return value:	none
//
//
void ssd1306::display(void)
{

	/*
	{
		uint8_t cmdList[] = {0x22, 0x00, 0xff};
		twi.transmitCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}
	{
		uint8_t cmdList[] = {0x21, 0x00, 127};
		twi.transmitCommandList(slaveAddress, sizeof(cmdList), cmdList);
	}
	*/

	//uint8_t cmdList[] = {0x40, buffer[0], buffer[1], buffer[2]};
	twi.write(slaveAddress, bufferSize, buffer);//cmdList);
	/*
	// steps for updating the display, writing memory content to the device

	// - set pageadddress SSD1306_PAGEADDR		0x22
	// - set page start						0x00
	// - set page end address					0xff
		twi.write(0, SET_PAGE_ADDRESS);
		twi.write(1, START_PAGE_RESET);
		twi.write(1, END_PAGE_RESET);

	// - set column address SSD1306_COLUMNADDR	0x21
	// - column start address					0
	// - column end address					(WIDTH - 1)
		twi.write(0, SET_COLUMN_ADDRESS);
		twi.write(1, START_COLUMN_RESET);
		twi.write(1, END_COLUMN_RESET);

	// - start i2c transmission, sending slave address as parameter, beginTransmission
	// - write the value 0x40
		twi.write(0, SET_COLUMN_ADDRESS);

	// - write the scren buffer, one byte at a time until done. There is a check here, if we exceed WIRE_MAX, the bus shuts down
		//uint16_t count = displayWidth * ((displayHeight + 7) / 8);
		uint16_t count = DISPLAY_BUFFER_SIZE;
		volatile uint8_t *ptr = buffer;
		twi.sendStart();
		twi.sendSlaW(slaveAddress);
		twi.write(1, 0x40);
		while(count--)
			twi.write(*ptr++);

	*/
	//   and restarts. This is probably some hardware thing. WIRE_MAX equals 32 in this piece of code
	//   uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
	//    uint8_t *ptr = buffer;
	//    if (wire) { // I2C
	//      wire->beginTransmission(i2caddr);
	//      WIRE_WRITE((uint8_t)0x40);
	//      uint8_t bytesOut = 1;
	//      while (count--) {
	//        if (bytesOut >= WIRE_MAX) {
	//          wire->endTransmission();
	//          wire->beginTransmission(i2caddr);
	//          WIRE_WRITE((uint8_t)0x40);
	//          bytesOut = 1;
	//        }
	//        WIRE_WRITE(*ptr++);
	//        bytesOut++;
	//    }
	//    wire->endTransmission();
	//
	//    } else { // SPI
	//      SSD1306_MODE_DATA
	//      while (count--)
	//        SPIwrite(*ptr++);
	//    }

	/*
	//uint8_t getDisplayWidth() = 128;
	//uint8_t getDisplayHeight() = 64;
	//uint8_t *buffer;


	uint8_t *ptr	= buffer;
	//uint16_t count	= WIDTH * ((HEIGHT + 7) / 8);
	//for(int i = 0; i < 1024; i++)
	//	buffer[i] |= (1 << (i/16 & 7));

	//	set page address
	twi.write(0, 0x22);
	twi.write(1, 0);
	twi.write(1, 0xff);

	//	set column address
	twi.write(0, 0x21);
	twi.write(1, 0);
	twi.write(1, 63);

	//	write out content, counting to zero according to formula uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
	 */


}
// END ssd1306::display()


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

/*
 * twi328P.h
 *
 *  Created on: 31 Oct 2020
 *      Author: jole
 */

//#ifndef __TWI328P_H__
//#define __TWI328P_H__

#ifndef F_CPU
	#define F_CPU			16000000UL
#endif




#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <stdint.h>

#define MAX_RESTARTS		30

#define	WRITE_COMMAND						0x00
#define	WRITE_DATA							0x01
#define TRANSMIT_SUCCESS					0x02
#define	TWI_INIT_OK							0b11000000
#define TWI_BUS_INITIALISE_SENT				0b11100000


#define	TWI_BUS_ERROR						0b10000000


//
//	twi status codes, to be read from the TWSR register
//
#define TWI_START_TRANSMITTED				0b00001000	//	0x08
#define TWI_REPEATED_START_TRANSMITTED		0b00010000	//	0x10
#define TWI_SLA_W_TRANSMITTED_ACK_RECEIVED	0b00011000	//	0x18
#define TWI_DATA_TRANSMITTED_ACK_RECEIVED	0b00101000	//	0x28


//
//
//
#define TWI_BUFFER_LENGTH		32



class twi328P{

	//
	//	used to keep track of which mode we're in
	//
	enum twiMode 	{	idle,
						busInitialiseSent,
						startConditionSent,
						slaWSent,
						masterTransmittingStart,
						masterTransmittingEnd,
						masterReceiver,
						slaveTransmitter,
						slaveReceiver,
						stopConditionSent
					};

	twiMode		twiBusMode;

	uint8_t		reStarts;			// keep tracks of number of bus restarts. We don't want the chip to hang in an infinite loop...
	uint32_t	busSpeed;

	//uint8_t		txMasterBuffer[TWI_BUFFER_LENGTH];
	//uint8_t		txMasterBufferIndex;


	void		twiBusError(void);
	void		twiPassedStatusCheck(void);
	void		waitForTWINTFlag(void);



public:
	twi328P(void);

	void	initBus(uint32_t _busSpeed);
	uint8_t	sendStartCondition(void);
	uint8_t	sendSlaW(uint8_t _slaveAddress);
	uint8_t	write(uint8_t _slaveAddress, uint8_t _size, uint8_t *_data);
	uint8_t	writeCommandList(uint8_t _address, uint16_t _size, uint8_t *_cmdList);
	void	sendStopCondition(void);

	void	blinkLED(uint8_t _PORT, uint16_t count, uint8_t blinkNormal);

	uint8_t	checkStatusRegister(uint8_t _status);
	//void	disableBus(void);

	//uint8_t	sendStart(void);

	//uint8_t	write(uint8_t data);

};


/*
 *	twi328P.cpp
 *
 *	Created on: 31 Oct 2020
 *	Author: jole
 */

//#include "twi328P/twi328P.h"




//
//	Function:		twi328P::twi328P()
//
//	Description:	constructor for creating the object
//
//
//	Arguments:		none
//
//	return value:	none
//
twi328P::twi328P(void)
{
	twiBusMode = idle;
}



//
//	Function:		twi328P::initBus()
//
//	Description:	initialises the i2c/twi bus, making it ready for transmission
//					sets TWBR according to the desired bus speed based on formula on p.222 in the data sheet
//					it also sets the prescaler bits in the TWSR, p.241 in the data sheet
//					This should be called only once by the application
//
//	arguments:		none
//
//	return value:	none
//
void	twi328P::initBus(uint32_t _busSpeed)
{
	//
	//	Sets the bus speed required by the application
	//
	busSpeed = _busSpeed;

	//
	//	Making sure the power reduction register (PRR), bit 7 is set to zero
	//	This is the power reduction for TWI and shuts down the bus if set to one
	//	See p.54 in the data sheet
	//
	//      	-------------------------------------------------------------------------
	//	PRR 	| PRTWI | PRTIM2 | PRTIM0 |   -   | PRTIM1 |  PRSPI  | PRUSART0 | PRADC |
	//      	-------------------------------------------------------------------------
	//      	|   0   |   x    |   x    |   x   |   x    |    x    |    x     |  x    |
	//      	-------------------------------------------------------------------------
	//
	//
	//PRR &= ~(1 << PRTWI);

	//
	//	Formula for Bit Rate Generator Unit, see p.221 in the data sheet
	//
	//	Here we calculate TWBR based on the desired i2c bus speed and
	//	clock frequency of the computer we use (F_CPU)
	//
	TWBR = ((F_CPU / busSpeed - 16) / (2));

	//
	//	Make SDA and SCL inputs by setting bit 4 and 5 in the
	//	data direction register of port C (DDRC) to one
	//	See p.100 in the data sheet
	//
	//      	-------------------------------------------------------------------------
	//	DDRC 	|   -    |  DDC6  |  DDC5  |  DDC4  |  DDC3  |  DDC2  |  DDC1  |  DDC0  |
	//      	-------------------------------------------------------------------------
	//      	|   x    |   x    |   1    |   1    |   x    |   x    |   x    |   x    |
	//      	-------------------------------------------------------------------------
	//
	//
	DDRC |= (3 << DDC4);

	//
	//	Disable internal pull-ups on SDA and SCL
	//      	-----------------------------------------------------------------------------------------
	//	PORTC	|    -     |  PORTC6  |  PORTC5  |  PORTC4  |  PORTC3  |  PORTC2  |  PORTC1  |  PORTC0  |
	//      	-----------------------------------------------------------------------------------------
	//      	|    x     |    x     |    0     |    0     |    x     |    x     |    x     |    x     |
	//      	-----------------------------------------------------------------------------------------
	//
	//
	//
	PORTC	|= ~(3 << DDC4);
	//PORTC	|= (3 << DDC4);

	//
	//	Clear the prescaler bits in the TWSR, setting the prescaler to 1
	//	See p.240 in data sheet
	//
	//	The prescaler bits are the two LSB in the TWCR, TWPS1 and TWPS0
	//
	//			-----------------------------------------------------------------
	//	TWSR	| TWS7  |  TWS6 | TWS5  | TWS4  | TWS3  |   -   | TWPS1 | TWPS0 |
	//			|   x   |   x   |   x   |   x   |   x   |   x   |   0   |   0   |
	//			-----------------------------------------------------------------
	//
	TWSR &= ~(0b00000011);

	//
	//	Enable acknowledgement
	//	Enable twi module
	//	Disable interrupts
	//
	TWCR = (1 << TWEN) | ~(1 << TWIE) | (1 << TWEA);

	// send STOP condition
	// NOTE that TWINT is NOT! set after transmitting a STOP condition
	//TWCR = (1 << TWINT) | (1 << TWSTO);

	twiBusMode = busInitialiseSent;
}
//	END twi328P::initBus()



//
//	Function:		twi328P::transmit()
//
//	Description:
//
//
//
//
//	Arguments:		uint8_t _slaveAddress	- address of slave device on twi bus
//					uint8_t _size			-
//					uint8_t data			- data to be written
//
//	return value:
//
uint8_t twi328P::write(uint8_t _slaveAddress, uint8_t _size, uint8_t *_data)
{

	// *** send START condition and wait for TWINT flag to be set *************
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	waitForTWINTFlag();

	// check TWSR for proper feedback, we need TWI_START_TRANSMITTED
	if((TWSR & 0xf8) != TWI_START_TRANSMITTED){
		blinkLED(0b00100000, 13, 0);
		_delay_us(500000);
		return(TWSR & 0xf8);
	}
	// flash the LED once to show we have successfully transmitted the START condition
	blinkLED(0b00100000, 1, 1);
	//_delay_us(500000);
	// *** END START condition *************************************************



	// *** send SLA+W and wait for TWINT flag to be set**************************
	TWDR = (_slaveAddress << 1) | TW_WRITE;
	TWCR = (1 << TWINT) | (1 << TWEN);
	waitForTWINTFlag();

	// check TWSR for proper feedback, we need TWI_SLA_W_TRANSMITTED_ACK_RECEIVED
	if((TWSR & 0xf8) != TWI_SLA_W_TRANSMITTED_ACK_RECEIVED){
		blinkLED(0b00100000, 3, 0);
		_delay_us(500000);
		return(TWSR & 0xf8);
	}
	// flash the LED to show we have made success transmitting SLA+W to the bus
	blinkLED(0b00100000, 2, 1);
	_delay_us(500000);
	// *** END SLA+W *****************************************************



		// writeData:
		// load data in TWDR, one byte at a time
		// clear TWINT in TWCR
		// wait for TWINT flag to be set
		// check TWSR for proper feedback, we need TWI_DATA_TRANSMITTED_ACK_RECEIVED


		/***************************************/
		TWDR = 0;
		TWCR = (1 << TWINT) | (1 << TWEN);
		waitForTWINTFlag();

		/**************************************/
		TWDR = 1;
		TWCR = (1 << TWINT) | (1 << TWEN);
		//waitForTWINTFlag();

		TWDR = 0x40;
		TWCR = (1 << TWINT) | (1 << TWEN);
		//waitForTWINTFlag();

		while(_size--){
			TWDR = 1;
			TWCR = (1 << TWINT) | (1 << TWEN);
			TWDR = *_data++;
			TWCR = (1 << TWINT) | (1 << TWEN);
			waitForTWINTFlag();
			blinkLED(0b00100000, 1, 0);
			//_delay_us(500000);
		}
		blinkLED(0b00100000, 7, 0);
		//_delay_us(500000);

		// goto writeData: until buffer is empty

		// send STOP condition
		// NOTE that TWINT is NOT! set after transmitting a STOP condition
		TWCR = (1 << TWINT) | (1 << TWSTO);
		blinkLED(0b00100000, 1, 0);
		//_delay_us(500000);

		return(TRANSMIT_SUCCESS);
}
// END twi328P::transmit()



//
//	Function:		twi328P::writeCommandList()
//
//	Description:
//
//
//
//
//	Arguments:		uint8_t _slaveAddress	- address of slave device on twi bus
//					uint16_t _size			- size of the command list
//					uint8_t _cmdList		- list of commands and data to be written
//					uint8_t _cmd			- 1 for command, 0 for data. Defaults to 1
//
//	return value:
//
uint8_t	twi328P::writeCommandList(uint8_t _address, uint16_t _size, uint8_t *_cmdList)
{
	// the bus is already initialised, so we can start transmitting


	// START condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while(!(TWCR & (1 <<TWINT))){
		//twi.blinkLED(0b00100000, 3, 1);
	}
	if((TWSR & 0xf8) != 0x08)
		return(1);

	// SLA+W
	TWDR = TW_WRITE;
	TWDR |= (_address << 1);
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT))){
		//twi.blinkLED(0b00100000, 4, 1);
	}
	if((TWSR & 0xf8) != 0x18)
		return(2);

	//	Command byte
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

	return(TRANSMIT_SUCCESS);
}
// END twi328P::transmitCommandList()



//
//	Function:		twi328P::sendStartCondition()
//
//	Description:
//
//
//
//	Arguments:
//
//	return value:
//
uint8_t twi328P::sendStartCondition(void)
{
	//	Send START condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	//	Wait for TWINT flag to be set by hardware
	while(!(TWCR & (1 <<TWINT))){
		//twi.blinkLED(0b00100000, 3, 1);
	}

	//	If we have the correct status in TWSR, return ok
	uint8_t status = (TWSR & 0xf8);
	if( status == 0x08){
		return(0);
	}

	//	Otherwise return an error
	return(1);
}



//
//	Function:		twi328P::sendSlaW()
//
//	Description:	transmit a SLA+W onto the twi bus
//					checking the status register for appropriate value, blinking LED's as necessary
//
//
//
//	Arguments:
//
//	return value:
//
uint8_t twi328P::sendSlaW(uint8_t _slaveAddress)
{
	//	Construct SLA+W
	TWDR = TW_WRITE;
	TWDR |= (_slaveAddress << 1);

	//	Send SLA+W
	TWCR = (1 << TWINT) | (1 << TWEN);

	//	Wait for TWINT flag to be set by hardware
	while(!(TWCR & (1 << TWINT))){
		//twi.blinkLED(0b00100000, 4, 1);
	}

	//	If we have the correct status in TWSR, return ok
	if((TWSR & 0xf8) == 0x18){
		return(0);
	}

	//	Otherwise return an error
	return(2);
}
// END twi328P::sendSlaW()



//
//	Function:		twi328P::stop()
//
//	Description:	sends STOP condition to the twi bus
//
//	Arguments:		none
//
//	return value:	none
//
void twi328P::sendStopCondition(void)
{
	// STOP condition
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

	twiBusMode = stopConditionSent;
}
// END twi328P::stop()








//
//	twi328P::disableBus()
//
//	disables the twi bus
//
//void twi328P::disableBus(void)
//{

//}

//	END twi328P::disableBus()






//
//	Function:		twi328P::sendStart()
//
//	Description:	transmit a START condition onto the twi bus
//					it keeps track of how many times we loop waiting for the twi hardware
//					to set flag and update the status register
//					it'll blink the LED to show error or ok
//
//	Arguments:		none
//
//	return value:	returns status register with prescaler bits masked out: TWSR & 0xf8
//
//uint8_t twi328P::sendStart(void)
//{
//	return 1;
//}




//
//	Function:		twi328P::waitForTWINTFlag()
//
//	Description:	wait for the TWINT flag to set after writing commands to the twi bus
//
//	Arguments:		none
//
//	return value:	none
//
void twi328P::waitForTWINTFlag(void) {
	//
	//	Now we wait for the hardware to:
	//		- set the TWINT flag in TWCR
	//		- update the TWSR with a status code
	//
	//
	//			-----------------------------------------------------------------
	//	TWCR	| TWINT |  TWEA | TWSTA | TWSTO | TWWC  | TWEN  |   -   | TWIE  |
	//	    	|   1   |   x   |   1   |   0   |   x   |   1   |   0   |   x   |
	//			-----------------------------------------------------------------
	//
	// Wait for the TWINT flag in TWCR to be set (e.g. write it to 0). This indicates successfull transmission
	//
	while(!(TWCR & (1 << TWINT))){
	}
}
// END twi328P::waitForTWINTFlag()









//
//	Function:		twi328P::write()
//
//	Description:
//
//
//
//
//	Arguments:
//
//
//
//	return value:
//
//uint8_t twi328P::write(uint8_t data)
//{
//	return 1;
//}
// END twi328P::write()



//
//	Function:
//	Description:
//	Arguments:
//	Return value:
//
void twi328P::twiBusError(void){

}
//	END twi328P::twiBusError()



//
//	Function:
//	Description:
//	Arguments:
//	Return value:
//
void twi328P::twiPassedStatusCheck(void){

}
//	END twi328P::twiPassedStatusCheck()



//
//	Function:
//	Description:
//	Arguments:
//	Return value:
//
void twi328P::blinkLED(uint8_t _PORT, uint16_t count, uint8_t blinkNormal)
{

	if(!blinkNormal){
		for(uint16_t i = 0; i < 2*count; i++){
			//PORTB |= _PORT;
			PORTB ^= _PORT;
			_delay_us(25000);
			//PORTB &= ~_PORT;
			//_delay_us(32000);
		}
	}
	else{
		for(uint16_t i = 0; i < 2*count; i++){
			PORTB ^= _PORT;
			_delay_us(250000);
			//PORTB &= ~_PORT;
			//_delay_us(255000);
		}
	}
	_delay_us(210000);
}
//	END twi328P::blinkLED()



/*
uint8_t twi328P::twsrStatus(void)
{


					—————————————————————————————————————————————————————————————————
			TWSR	| TWS7  |  TWS6 | TWS5  | TWS4  | TWS3  |   -   | TWPS1 | TWPS0 |
			    	|   0   |   0   |   0   |   0   |   1   |   0   |   0   |   0   |
					—————————————————————————————————————————————————————————————————

			In order to read the status codes from the TWSR we need to mask out the
			prescaler bits TWPS1 and TWPS0, see data sheet p.200



	//
	// create the mask
	//
	//uint8_t mask		= ~(0b00000011);
	//uint8_t twStatus	= TWSR & mask;

	return(TWSR & 0xf8);
}


uint8_t i2cPutByte(uint8_t byte)
{
	//
	// put the byte to send into the data register
	//
	TWDR = byte;

	TWCR = 1 << TWINT | 1 << TWEN;

	while((TWCR & _BV(TWINT)) == 0){
	}

	switch(TW_STATUS){
		case TW_MT_SLA_ACK:
		case TW_MT_DATA_ACK:
		case TW_MR_SLA_ACK:
		case TW_MR_DATA_ACK:	return(0);
								break;
		case TW_BUS_ERROR:
		case TW_MT_SLA_NACK:
		case TW_MT_DATA_NACK:
		case TW_MT_ARB_LOST:
		case TW_MR_SLA_NACK:
		case TW_MR_DATA_NACK:	return(1);
								break;
		default:				return(2);
								break;
	}
}






void blinkLED(uint8_t _PORT, uint8_t count, uint8_t blinkNormal)
{

	if(!blinkNormal){
		for(uint8_t i = 0; i < count; i++){
			PORTB |= _PORT;
			_delay_us(19000);
			PORTB &= ~_PORT;
			_delay_us(32000);
		}
	}
	else{
		for(uint8_t i = 0; i < count; i++){
			PORTB |= _PORT;
			_delay_us(125000);
			PORTB &= ~_PORT;
			_delay_us(255000);
		}
	}
}
*/

/*
 * ssd1306CommandSet.h
 *
 *  Created on: 31 Oct 2020
 *      Author: jole
 */



//#ifndef ATMEGA328P_WATERFLOWSVERDRUP_SRC_SSD1306_SSD1306COMMANDSET_H_
//#define ATMEGA328P_WATERFLOWSVERDRUP_SRC_SSD1306_SSD1306COMMANDSET_H_

#define CMD_SET_MUX_RATIO					0b10101000
#define MUX_RATIO_RESET						0b00111111

#define CMD_SET_DISPLAY_OFFSET				0b11010011
#define DISPLAY_OFFSET_RESET				0b00000000

#define CMD_SET_DISPLAY_START_LINE_0		0b01000000	// display start line from 0 - 63
#define CMD_SET_DISPLAY_START_LINE_1		0b01000001
#define CMD_SET_DISPLAY_START_LINE_2		0b01000010
#define CMD_SET_DISPLAY_START_LINE_3		0b01000011
#define CMD_SET_DISPLAY_START_LINE_4		0b01000100
#define CMD_SET_DISPLAY_START_LINE_5		0b01000101
#define CMD_SET_DISPLAY_START_LINE_6		0b01000110
#define CMD_SET_DISPLAY_START_LINE_7		0b01000111


#define SET_SEGMENT_MAP_NORMAL				0b10100000	//	maps column address 0 to SEG0	(reset)
#define SET_SEGMENT_MAP_REMAPPED			0b10100001	//	maps column address 127 to SEG0

#define SET_COM_OUTPUT_SCAN_NORMAL			0b11000000	//	reset
#define SET_COM_OUTPUT_SCAN_REMAPPED		0b11001000

#define	CMD_SET_COM_PINS_HWCONF					0b11011010
#define SET_COM_PINS_HWCONF_RESET				0b00010010	//	reset
#define SET_COM_PINS_HWCONF_EN_SEQ				0b00100010	// bigger letters, it's writing every alternate line
#define SET_COM_PINS_HWCONF_ALT_EN				0b00110010
#define SET_COM_PINS_HWCONF_DIS_SEQ				0b00000010

#define CMD_SET_CONTRAST_CONTROL			0b10000001
#define	SET_CONTRAST_RESET					0b01111111

#define	DISPLAY_RAM_CONTENT					0b10100100	//	reset
#define	IGNORE_RAM_CONTENT					0b10100101

#define	SET_NORMAL_DISPLAY					0b10100110
#define SET_INVERSE_DISPLAY					0b10100111

#define CMD_SET_CLOCK_DIVIDE_RATIO_OSC_FREQ	0b11010101
#define CLOCK_DIVIDE_RATIO_RESET			0b0000		//	reset, lower bits
#define OSC_FREQ_RESET						0b1000		//	reset, upper bits

#define CMD_SET_CHARGE_PUMP					0b10001101
#define	CHARGE_PUMP_ENABLE					0b00010100
#define	CHARGE_PUMP_DISPLAY_ON				0b10101111

#define SET_DISPLAY_ON						0b10101111
#define SET_DISPLAY_OFF						0b10101110	//	reset

#define	CMD_SET_PAGE_ADDRESS				0b00100010
#define SET_START_PAGE_RESET				0b0000000
#define SET_END_PAGE_RESET					0b0000111

#define	CMD_SET_COLUMN_ADDRESS				0b00100001
#define SET_START_COLUMN_RESET				0b00000000
#define SET_END_COLUMN_RESET				0b01111111

#define CMD_SET_PRE_CHARGE_PERIOD			0b11011001
#define SET_PRE_CHARGE_PERIOD_RESET			0b00100010

#define CMD_SET_V_COMH_DESELECT_LEVEL		0b11011011
#define SET_V_COMH_DESELECT_LEVEL_RESET		0b00100000

#define CMD_SET_MEMORY_ADDRESSING_MODE		0b00100000
#define SET_HORIZONTAL_ADDRESSING_MODE		0b00000000
#define SET_VERTICAL_ADDRESSING_MODE		0b00000001
#define SET_PAGE_ADDRESSING_MODE			0b00000010

#define CMD_SET_LOWER_COLUMN_START_ADDRESS	0b00000000
#define CMD_SET_HIGHER_COLUMN_START_ADDRESS	0b00010000
#define CMD_SET_GDDRAM_START_PAGE_0			0b10110000

//#endif


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

//#ifndef __SSD1306_SSD1306_H__
//#define __SSD1306_SSD1306_H__

//#include <ssd1306/ssd1306CommandSet.h>
//#include "twi328P/twi328P.h"


#define MEMORY_ALLOC_ERROR	0b10101010


class ssd1306 {

	uint8_t   buffer[1024];
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

//#endif /* ATMEGA328P_WATERFLOWSVERDRUP_SRC_SSD1306_SSD1306_H_ */
//#endif /* ATMEGA328P_WATERFLOWSVERDRUP_SRC_TWI_TWI328P_H_ */


//#ifndef __FONT_5X7_H__
//#define __FONT_5X7_H__

#include <avr/pgmspace.h>

const uint8_t font5x7[][5] /*PROGMEM*/ =
{
	{0x00, 0x00, 0x00, 0x00, 0x00},		//	32 	<space>

	{	0b00000000,
		0b00000000,
		0b01011111,
		0b00000000,
		0b00000000},		//	33 	!

	{	0b00000000,
		0b00000111,
		0b00000000,
		0b00000111,
		0b00000000},		//	34 	"

	{	0b00010100,
		0b01111110,
		0b00010100,
		0b01111110,
		0b00010100},		//	35 	#

	{	0b00100100,
		0b00101010,
		0b01111111,
		0b00101010,
		0b00010010},		//	36 	$

	{	0x23,
		0x13,
		0x08,
		0x64,
		0x62}, 	// 	37 	%

	{	0x36,
		0x49,
		0x55,
		0x22,
		0x50}, 	// 	38 	&

	{0x00, 0x05, 0x03, 0x00, 0x00}, 	// 	39 	'
	{0x00, 0x1c, 0x22, 0x41, 0x00}, 	// 	40 	(
	{0x00, 0x41, 0x22, 0x1c, 0x00}, 	// 	41 	)
	{0x14, 0x08, 0x3e, 0x08, 0x14}, 	// 	42 	*
	{0x08, 0x08, 0x3e, 0x08, 0x08}, 	// 	43 	+
	{0x00, 0x50, 0x30, 0x00, 0x00}, 	// 	44 	,
	{0x08, 0x08, 0x08, 0x08, 0x08}, 	// 	45 	-
	{0x00, 0x60, 0x60, 0x00, 0x00}, 	// 	46 	.
	{0x20, 0x10, 0x08, 0x04, 0x02}, 	// 	47 	/
	{0x3e, 0x51, 0x49, 0x45, 0x3e}, 	// 	48 	0
	{0x00, 0x42, 0x7f, 0x40, 0x00}, 	// 	49 	1
	{0x42, 0x61, 0x51, 0x49, 0x46}, 	// 	50 	2
	{0x21, 0x41, 0x45, 0x4b, 0x31}, 	// 	51 	3
	{0x18, 0x14, 0x12, 0x7f, 0x10}, 	// 	52 	4
	{0x27, 0x45, 0x45, 0x45, 0x39}, 	// 	53 	5
	{0x3c, 0x4a, 0x49, 0x49, 0x30}, 	// 	54 	6
	{0x01, 0x71, 0x09, 0x05, 0x03}, 	// 	55 	7
	{0x36, 0x49, 0x49, 0x49, 0x36}, 	// 	56 	8
	{0x06, 0x49, 0x49, 0x29, 0x1e}, 	// 	57 	9
	{0x00, 0x36, 0x36, 0x00, 0x00}, 	// 	58 	:
	{0x00, 0x56, 0x36, 0x00, 0x00}, 	// 	59 	;
	{0x08, 0x14, 0x22, 0x41, 0x00}, 	// 	60 	<
	{0x14, 0x14, 0x14, 0x14, 0x14}, 	// 	61 	=
	{0x00, 0x41, 0x22, 0x14, 0x08}, 	// 	62 	>
	{0x02, 0x01, 0x51, 0x09, 0x06}, 	// 	63 	?
	{0x32, 0x49, 0x79, 0x41, 0x3e}, 	// 	65 	@
	{0x7e, 0x11, 0x11, 0x11, 0x7e}, 	// 	65 	A
	{0x7f, 0x49, 0x49, 0x49, 0x36}, 	// 	66 	B
	{0x3e, 0x41, 0x41, 0x41, 0x22}, 	// 	67 	C
	{0x7f, 0x41, 0x41, 0x22, 0x1c}, 	// 	68 	D
	{0x7f, 0x49, 0x49, 0x49, 0x41}, 	// 	69 	E
	{0x7f, 0x09, 0x09, 0x09, 0x01}, 	// 	70 	F
	{0x3e, 0x41, 0x49, 0x49, 0x7a}, 	// 	71 	G
	{0x7f, 0x08, 0x08, 0x08, 0x7f}, 	// 	72 	H
	{0x00, 0x41, 0x7f, 0x41, 0x00}, 	// 	73 	I
	{0x20, 0x40, 0x41, 0x3f, 0x01}, 	// 	74 	J
	{0x7f, 0x08, 0x14, 0x22, 0x41}, 	// 	75 	K
	{0x7f, 0x40, 0x40, 0x40, 0x40}, 	// 	76 	L
	{0x7f, 0x02, 0x0c, 0x02, 0x7f}, 	// 	77 	M
	{0x7f, 0x04, 0x08, 0x10, 0x7f}, 	// 	78 	N
	{0x3e, 0x41, 0x41, 0x41, 0x3e}, 	// 	79 	O
	{0x7f, 0x09, 0x09, 0x09, 0x06}, 	// 	80 	P
	{0x3e, 0x41, 0x51, 0x21, 0x5e}, 	// 	81 	Q
	{0x7f, 0x09, 0x19, 0x29, 0x46},		// 	82 	R
	{0x46, 0x49, 0x49, 0x49, 0x31},		// 	83 	S
	{0x01, 0x01, 0x7f, 0x01, 0x01}, 	// 	84 	T
	{0x3f, 0x40, 0x40, 0x40, 0x3f}, 	// 	85 	U
	{0x1f, 0x20, 0x40, 0x20, 0x1f}, 	// 	86 	V
	{0x3f, 0x40, 0x38, 0x40, 0x3f}, 	// 	87 	W
	{0x63, 0x14, 0x08, 0x14, 0x63}, 	// 	88 	X
	{0x07, 0x08, 0x70, 0x08, 0x07}, 	// 	89 	Y
	{0x61, 0x51, 0x49, 0x45, 0x43}, 	// 	90 	Z
	{0x00, 0x7f, 0x41, 0x41, 0x00}, 	// 	91 	[
	{0x02, 0x04, 0x08, 0x10, 0x20}, 	// 	92 	backslash
	{0x00, 0x41, 0x41, 0x7f, 0x00}, 	// 	93 	]
	{0x04, 0x02, 0x01, 0x02, 0x04}, 	// 	94 	^
	{0x40, 0x40, 0x40, 0x40, 0x40}, 	// 	95 	_
	{0x00, 0x01, 0x02, 0x04, 0x00}, 	// 	96 	`
	{0x20, 0x54, 0x54, 0x54, 0x78}, 	// 	97 	a
	{0x7f, 0x48, 0x44, 0x44, 0x38}, 	// 	98 	b
	{0x38, 0x44, 0x44, 0x44, 0x20}, 	// 	99 	c
	{0x38, 0x44, 0x44, 0x48, 0x7f}, 	// 	100 d
	{0x38, 0x54, 0x54, 0x54, 0x18}, 	// 	101	e
	{0x08, 0x7e, 0x09, 0x01, 0x02}, 	// 	102	f
	{0x0c, 0x52, 0x52, 0x52, 0x3e}, 	// 	103	g
	{0x7f, 0x08, 0x04, 0x04, 0x78}, 	// 	104	h
	{0x00, 0x44, 0x7d, 0x40, 0x00}, 	// 	105	i
	{0x20, 0x40, 0x44, 0x3d, 0x00}, 	// 	106	j
	{0x7f, 0x10, 0x28, 0x44, 0x00}, 	// 	107	k
	{0x00, 0x41, 0x7f, 0x40, 0x00}, 	// 	108	l
	{0x7c, 0x04, 0x18, 0x04, 0x78}, 	// 	109	m
	{0x7c, 0x08, 0x04, 0x04, 0x78}, 	// 	110	n
	{0x38, 0x44, 0x44, 0x44, 0x38}, 	// 	111	o
	{0x7c, 0x14, 0x14, 0x14, 0x08}, 	// 	112	p
	{0x08, 0x14, 0x14, 0x18, 0x7c}, 	// 	113	q
	{0x7c, 0x08, 0x04, 0x04, 0x08}, 	// 	114	r
	{0x48, 0x54, 0x54, 0x54, 0x20}, 	// 	115	s
	{0x04, 0x3f, 0x44, 0x40, 0x20}, 	// 	116	t
	{0x3c, 0x40, 0x40, 0x20, 0x7c}, 	// 	117	u
	{0x1c, 0x20, 0x40, 0x20, 0x1c}, 	// 	118	v
	{0x3c, 0x40, 0x30, 0x40, 0x3c}, 	// 	119	w
	{0x44, 0x28, 0x10, 0x28, 0x44}, 	// 	120	x
	{0x0c, 0x50, 0x50, 0x50, 0x3c}, 	// 	121	y
	{0x44, 0x64, 0x54, 0x4c, 0x44}, 	// 	122	z
	{0x00, 0x08, 0x36, 0x41, 0x00}, 	// 	123	{
	{0x00, 0x00, 0x7f, 0x00, 0x00}, 	// 	124	|
	{0x00, 0x41, 0x36, 0x08, 0x00}, 	// 	125	}
	{0x10, 0x08, 0x08, 0x10, 0x08}		// 	126	~

};

//#endif



/*
 * ssd1306.cpp
 *
 *  Created on: 31 Oct 2020
 *      Author: jole
 */

#include <stdlib.h>
#include <string.h>
//#include "ssd1306/ssd1306.h"
//#include "graphics/font5x7.h"



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

	//buffer = (uint8_t*) malloc(bufferSize);
	
  //if(!buffer){
	//	twi.blinkLED(0b00100000, bufferSize, 0);
	//	return(MEMORY_ALLOC_ERROR);
	//}
  

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
	//if(!buffer)
	//	return;

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


ssd1306		oled;

uint8_t		lightUp				    = 1;
uint8_t		displayTwiAddress = 0x3c;
uint8_t		width				      = 128;
uint8_t		height				    = 64;

void setup()
{

  	//	Enable output to LED port
	DDRB |= 0b00100000;

	// Blink LED to visualise start of program
	oled.twi.blinkLED(0b00100000, 3, 1);
	_delay_us(500000);

  // initialise the display, and check the feedback...
	switch(oled.initDisplay(displayTwiAddress, width, height)){

		case 1:			oled.twi.blinkLED(0b00100000, 3, 1);
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
									oled.twi.blinkLED(0b00100000, 13, 1);
									lightUp = 0;
									break;

		case TWI_INIT_OK:			// we got through initDisplay without errors
									oled.twi.blinkLED(0b00100000, 9, 1);
									break;



		default:			oled.twi.blinkLED(0b00100000, 20, 1);
									lightUp = 0;
									break;

	}

  if(lightUp)
		PORTB = 0b00100000;
  
  
  uint8_t		text[64];

  oled.clearDisplay();
  sprintf((char*)text, "I LOVE VLBI");
	oled.print(5, 3, text, strlen((char*)text));
  oled.display();


}

void loop() {
  // put your main code here, to run repeatedly:

}

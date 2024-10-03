/*
 * twi328P.h
 *
 *  Created on: 31 Oct 2020
 *      Author: jole
 */

#ifndef __TWI328P_H__
#define __TWI328P_H__

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



#endif /* ATMEGA328P_WATERFLOWSVERDRUP_SRC_TWI_TWI328P_H_ */

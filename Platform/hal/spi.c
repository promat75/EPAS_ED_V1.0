
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2014-01-23
	- Version		: V2.2

	[2014-01-23] V2.2
	- HAL_SpiIntSet() : enhanced.

	[2013-12-20] V2.1
	- HAL_SpiIntSet() : enhanced.
	- SYS_SpiMasterPut() : enhanced.
	- SYS_SpiMasterPutMultiBytes() : enhanced.
	- SYS_SpiSlavePut() : enhanced.
	- SYS_SpiSlavePutMultipleBytes() : enhanced.

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "gpio.h"
#include "spi.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether SPI interrupt is generated.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8SpiMaster_IntFlag = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates the received data for the current SPI cycle in master mode.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8SpiMaster_RxData;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable SPI's RX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gau8SpiSlave_RxBuff[SPI_RXBUF_SIZE];

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates index number for writing SPI's RX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16SpiSlave_RxWrIdx = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates index number for reading SPI's RX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16SpiSlave_RxRdIdx = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable to be transmitted for next SPI cycle in slave mode.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8SpiSlave_NextTxData = 0x55;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for SPI.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SpiClockOn(UINT8 u8On)
{
	if(u8On)
	{
		SPI_CLK_ON;
	}
	else
	{
		SPI_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Selects a GPIO port as CSn. For example, P3.7 is used as CSn in this function.
///
///	@param	void
///	@return	void
///	@note	If other GPIO is used as CSn, it should be set as output port, here.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SpiMasterCSnSelect(void)
{
	// TODO: If other GPIO is used as CSn, it should be set as output port, here.
	GPIO_CLK_ON;
	SPI_CSN = 1;			// P3.7
	HAL_Gpio3InOutSet(7, GPIO_OUTPUT, GPIO_DRIVE_4mA);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Selects a GPIO port as CSn. For example, P3.7 is used as CSn in this function.
///
///	@param	u8High		: 0=low, 1=high.
///	@return	void
///	@note	If other GPIO is used as CSn, it should be driven as high or low
///	@note	refer to HAL_SpiMasterCSnSelect() for the specified CSn port
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SpiMasterCSnSet(UINT8 u8High)
{
	// If other GPIO is used as CSn, it should be driven as high or low
	SPI_CSN = u8High;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets SPI interrupt.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u8Mode		: Master/Slave mode, Phase, Polarity.
///						\n	bit[4] : 0=Slave, 1=Master.
///						\n	bit[3] : Clock polarity(CPOL). SCK's idle state. 0=Low, 1=High.
///						\n	bit[2] : Clock Phase(CPHA). Bit transition timing. 0=2nd edge, 1=1st edge.
///	@param	u8Speed		: SPI speed.(Refer to [note])
///						\n	0 = 1 MHz
///						\n	1 = 500 KHz
///						\n	2 = 250 KHz
///						\n	3 = 125 KHz
///						\n	4 = 62.5 KHz
///						\n	5 = 31.25 KHz
///						\n	6 = 15.62 KHz
///						\n	7 = 7.8 KHz
///						\n	8 = 3.9 KHz
///						\n	9 = 1.9 KHz
///	@return	UINT8. Status.
///	@note	Bit transition timing with CPOL and CPHA.
///		\n----------------------------------------------
///		\n				|	CPOL=1		| CPOL=0				
///		\n----------------------------------------------
///		\n		CPHA=1	|	Falling edge	| Rising edge
///		\n		CPHA=0	|	Rising edge	| Falling edge
///		\n----------------------------------------------
///	@note	The above SPI speed is valid only when MCU frequency is 8MHz. It is dependent on the MCU frequency.
///			The speed for another frequency is,
///				\n SPI speed = (Above speed) * (8MHz / MCU frequency)
///
///	@Example	HAL_SpiIntSet(ENABLE, PRIORITY_HIGH, SPI_MASTER | SPI_POLARITY_HIGH | SPI_PHASE_1ST, SPEED_1000KHz);
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_SpiIntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Mode, UINT8 u8Speed)
{
	UINT8	u8Divisior;
	UINT8	u8EA;
	UINT8	ib;

	if(SPI_CLK_STATE == 0)
	{
		assert_error(SPI_CODE | RETURN_SPI_CLOCK_OFF);
		return RETURN_SPI_CLOCK_OFF;
	}
	if(u8Speed > 9)
	{
		assert_error(SPI_CODE | RETURN_SPI_INVALID_PARAMETER);
		return RETURN_SPI_INVALID_PARAMETER;
	}

	// Divisor
	switch(u8Speed)
	{
		case	SPI_SPEED_500KHz	:	u8Divisior = 0x05;			break;
		case	SPI_SPEED_250KHz	:	u8Divisior = 0x03;			break;
		case	SPI_SPEED_125KHz	:	u8Divisior = 0x04;			break;
		case	SPI_SPEED_2KHz		:	u8Divisior = 0x0B;			break;
		default						:	u8Divisior = u8Speed + 2;	break;
	}

	xSPCR = 0x00;
	xSPER = 0x00;

	xSPCR |= u8Divisior & BIT_1N0 ;
	xSPER |= (u8Divisior & BIT_3N2) >> 2;	
	
	if(u8IntEna)
	{
		u8EA = EA;
		EA = 0;
		SPIIP = u8Priority;
		if(u8Mode & BIT4)	// Master
		{
			HAL_SpiMasterCSnSelect();
		}
	
		xSPCR |= (u8Mode & 0x1C) ;
		xSPCR |= BIT_7N6;		// bit[7;6] = 2'b11, if bit[6] is 1 from 0, RD/WR pointer is reset
		SPIIE = 1;

		if( (u8Mode & BIT4) == 0)	// Slave Mode
		{
			for(ib=0 ; ib<SPI_HW_FIFO_SIZE ; ib++)
			{
				xSPDR = gu8SpiSlave_NextTxData;	// Dummy Write
				DPH = 0;
			}
		}

		EA = u8EA;		
	}
	else
	{	
		xSPCR = 0x00;
		SPIIE = 0;
	}

	return RETURN_SPI_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sends 1-byte data from the SPI master device.
///
///	@param	u8Data	: Data to be sent.
///	@return	UINT8. Received data for this SPI cycle.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_SpiMasterPut(UINT8 u8Data)
{
	HAL_SpiMasterCSnSet(0);			// CSn = low

	EA = 0;
	xSPDR = u8Data;
	DPH = 0;
	EA = 1;
	while(!gu8SpiMaster_IntFlag);
	gu8SpiMaster_IntFlag = 0;

	HAL_SpiMasterCSnSet(1);			// CSn = high

	return	gu8SpiMaster_RxData;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sends multiple data from the SPI master device.
///
///	@param	pu8DataBuff	: Pointer to the buffer to be sent.
///	@param	u8PutLen		: Number of data to be sent.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_SpiMasterPutMultiBytes(UINT8* pu8DataBuff, UINT8 u8PutLen)
{
	UINT8	ib;
	
	if(u8PutLen > SPI_HW_FIFO_SIZE)
	{
		assert_error(SPI_CODE | RETURN_SPI_INVALID_PARAMETER);
		return RETURN_SPI_INVALID_PARAMETER;
	}

	gu8SpiMaster_IntFlag = 0;
	HAL_SpiMasterCSnSet(0);		// CSn = low

	for(ib=0 ; ib<u8PutLen ; ib++)
	{
		EA = 0;
		xSPDR = pu8DataBuff[ib];
		DPH = 0;
		EA = 1;
	}
	
	// Wait for completing [u8PutLen] bytes SPI data transfer
	while(!(u8PutLen == gu8SpiMaster_IntFlag));
	
	gu8SpiMaster_IntFlag = 0;

	HAL_SpiMasterCSnSet(1);		// CSn = high

	return RETURN_SPI_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets 1-byte data to be transmitted for next SPI cycle from the SPI slave device.
///
///	@param	u8NextSendData	: Data to be set.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void SYS_SpiSlavePut(UINT8 u8NextSendData)
{
	UINT8	ib;

	// Clear Write Buffer(xSPDR)
	xSPCR &= ~BIT6;
	xSPCR |= BIT6;

	// Set data to be sent for next SPI cycle from Slave to Master.
	for(ib=0 ; ib<SPI_HW_FIFO_SIZE ; ib++)
	{
		EA = 0;
		xSPDR = u8NextSendData;
		DPH = 0;
		EA = 1;
	}
	gu8SpiSlave_NextTxData = u8NextSendData;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets multiple data to be transmitted for next SPI cycles from the SPI slave device.
///
///	@param	pu8DataBuff	: Pointer to the buffer to be set.
///	@param	u8PutLen		: Number of data to be set.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_SpiSlavePutMultipleBytes(UINT8* pu8DataBuff, UINT8 u8PutLen)
{
	UINT8	ib;

	if(u8PutLen > SPI_HW_FIFO_SIZE)
	{
		assert_error(SPI_CODE | RETURN_SPI_INVALID_PARAMETER);
		return RETURN_SPI_INVALID_PARAMETER;
	}

	// Clear Write Buffer(xSPDR)
	xSPCR &= ~BIT6;
	xSPCR |= BIT6;

	// Set data to be sent for next SPI cycle from Slave to Master.
	for(ib=0 ; ib<SPI_HW_FIFO_SIZE ; ib++)
	{
		EA = 0;
		xSPDR = pu8DataBuff[ib];
		DPH = 0;
		EA = 1;
	}
	gu8SpiSlave_NextTxData = pu8DataBuff[0];

	return RETURN_SPI_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets a received data form SPI RX Buffer.
///
///	@param	pu8Data	: Pointer to the data to be got.
///	@return	UINT8. Status. 0=Buffer is empty, 1=Buffer is not empty and data is got.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_SpiSlaveGet(UINT8* pu8Data)
{
	UINT16 	u16RxLen;
	UINT16	u16NextNum;

	u16RxLen = (gu16SpiSlave_RxWrIdx - gu16SpiSlave_RxRdIdx) & (SPI_RXBUF_SIZE - 1);

	if(u16RxLen)
	{
		*pu8Data = gau8SpiSlave_RxBuff[gu16SpiSlave_RxRdIdx];
		u16NextNum = (gu16SpiSlave_RxRdIdx + 1) & (SPI_RXBUF_SIZE - 1);
		gu16SpiSlave_RxRdIdx = u16NextNum;
		return 1;
	}
	else
	{
		return 0;
	}		
}


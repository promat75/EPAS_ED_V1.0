
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2013-12-20
	- Version		: V2.2

	[2013-12-20] V2.2
	- HAL_I2CMaster_RandomWrite() : corrected.
	- HAL_I2CMaster_RandomRead() : corrected.
	- HAL_I2CMaster_SequentialWrite() : corrected.
	- HAL_I2CMaster_SequentialRead() : corrected.

	[2013-05-27] V2.1
	- HAL_I2CMaster_SequentialRead() : corrected.

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "i2c.h"
#include "gpio.h"
#include "wdt.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates the number of occurred Byte-Transfer interrupts. This is incremented in I2C_ISR()
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8I2CByteTransferIntCount = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether NACK interrupt is occurred. This is set in I2C_ISR()
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8I2CNackedFlag = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates the number of data to be read in I2C_ISR() before NACK interrupt
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8I2CMasterPreReadLength = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates the pointer to the buffer in which the read data is contained when sequential read operation.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8*	gpu8I2CMasterReadData;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clocks for I2C.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_I2CClockOn(UINT8 u8On)
{
	if(u8On)
	{
		I2C_CLK_ON;
	}
	else
	{
		I2C_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets I2C interrupt.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u8Mode		: Mode Select.
///						\n	bit[0] : 0=Slave, 1=Master.
///	@param	u8Speed		: I2C speed.(Refer to [note]). Speed= 8MHz / (u8Speed*2 + 4)
///	@return	UINT8. Status.
///	@note	The above I2C speed is valid only when MCU frequency is 8MHz. It is dependent on the MCU frequency.
///			The speed for another frequency is,
///				\n I2C speed = (Above speed) * (8MHz / MCU frequency)
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_I2CIntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Mode, UINT8 u8Speed)
{
	UINT8	u8EA;
	UINT8	u8ReadReg;

	if(I2C_CLK_STATE == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_CLOCK_OFF);
		return RETURN_I2C_CLOCK_OFF;
	}

	if(u8Speed < 18)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	if(u8IntEna)
	{
		u8EA = EA;
		EA = 0;

		xI2C_PRER = u8Speed;
		xI2C_TO = 0xFF;
		xI2C_HOLD = 0x00;
		u8ReadReg = xI2C_STR;
		u8ReadReg = xI2C_FINTVAL;
		if(u8Mode & BIT0)	// Master
		{
			xI2C_CTR = (I2C_CORE_OFF | I2C_MASTER | I2C_RXFIFO_RST | I2C_TXFIFO_RST);
		}
		else					// Slave
		{
			xI2C_CTR = (I2C_CORE_OFF | I2C_RXFIFO_RST | I2C_TXFIFO_RST);
		}

		EIP2 &= ~BIT1;
		if(u8Priority)
		{
			EIP2 |= BIT1;
		}
		EIE2 |= BIT1;
		EA = u8EA;
	}
	else
	{
		EIE2 &= ~BIT1;
		xI2C_CTR = I2C_CORE_OFF;		// core disable
	}

	return RETURN_I2C_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Makes a time-delay for one I2C clock time
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_I2COneClockDelay(void)
{
	static	UINT16	u16ClockDelay;

	for(u16ClockDelay=0 ; u16ClockDelay<10 ; u16ClockDelay++);
}


///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Writes I2C data with the random address
///
///	@param	u8DeviceAddr	: Device address of the I2C slave device. 0x00 ~ 0x7F.
///	@param	u8WordAddr	: Word address of the I2C slave device. 
///							\n The first data is written to the memory space addressed by "u8WordAddr"
///	@param	pu8DataBuff	: Pointer to the buffer of the data to be written.
///	@param	u8DataLen	: Number of data to be written
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_I2CMaster_RandomWrite(UINT8 u8DeviceAddr, UINT8 u8WordAddr, UINT8* pu8DataBuff, UINT8 u8DataLen)
{
	UINT8	u8ExpectedByteTransfer;
	UINT8	u8Status;
	UINT8	u8ReadReg;
	UINT8	u8Addr_Cmd;
	UINT8	ib;
	UINT8	u8EIE1;
	UINT8	u8EIE2;
	UINT8	u8IE;
	UINT8	loop_count = 0;

	if(I2C_CLK_STATE == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_CLOCK_OFF);
		return RETURN_I2C_CLOCK_OFF;
	}

	if(u8DataLen == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	if(u8DataLen > 15)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	SYS_WdtSet(0);

	u8EIE1 = EIE1;
	u8EIE2 = EIE2;
	u8IE = IE;

	EIE2 = BIT1;
	EIE1 = 0;
	IE = BIT7;

	xI2C_CTR = (I2C_CORE_OFF | I2C_RXFIFO_RST | I2C_TXFIFO_RST);
	u8ReadReg = xI2C_FINTVAL;
	EA = 0;
	u8ReadReg = xI2C_STR;
	DPH = 0;
	EA = 1;

	u8Addr_Cmd = (u8DeviceAddr & 0x7F) << 1;
	u8Addr_Cmd |= I2C_WRITE_BIT;

	xI2C_ADDR = u8Addr_Cmd;
	EA = 0;
	xI2C_DATA = u8WordAddr;
	DPH = 0;
	EA = 1;
	for(ib=0 ; ib<u8DataLen ; ib++)
	{
		EA = 0;
		xI2C_DATA = pu8DataBuff[ib];
		DPH = 0;
		EA = 1;
	}
	xI2C_IMSK = BIT6 | BIT2;

	xI2C_CTR &= ~I2C_CORE_OFF;

	gu8I2CNackedFlag = 0;
	u8ExpectedByteTransfer = 2 + u8DataLen;
	gu8I2CByteTransferIntCount = 0;

	xI2C_CTR |= (I2C_INT_ON | I2C_MASTER | I2C_START);

	u8Status = RETURN_I2C_SUCCESS;
	while(1)
	{
		loop_count++;
		if(gu8I2CNackedFlag) {
			u8Status = RETURN_I2C_NACK_FROM_SLAVE;
			break;
		}

		if(u8ExpectedByteTransfer == gu8I2CByteTransferIntCount)	break;
		if (loop_count >= 0x40) { u8Status = 0x80; break; }
		
	}

	if(u8Status == RETURN_I2C_SUCCESS)
	{
		HAL_I2COneClockDelay();
		xI2C_CTR &= ~I2C_START;			// 1-->0 : STOP
	}

	loop_count = 0;
	while(!(I2C_SCL && I2C_SDA)) {
		loop_count++;
			
		if (loop_count >= 0x20)  	{u8Status = 0x80;  break;}
	};		// wait until SCL=1 and SDA=1

	xI2C_CTR = I2C_CORE_OFF;

	EIE2 = u8EIE2;
	EIE1 = u8EIE1;
	IE = u8IE;

	return u8Status;	
}

#if 0
// Radiopulse org code

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Reads I2C data with the random address
///
///	@param	u8DeviceAddr	: Device address of the I2C slave device. 0x00 ~ 0x7F.
///	@param	u8WordAddr	: Word address of the I2C slave device. 
///							\n The first data is read from the memory space addressed by "u8WordAddr"
///	@param	pu8DataBuff	: Pointer to the buffer of the data to be read.
///	@param	u8DataLen	: Number of data to be read
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_I2CMaster_RandomRead(UINT8 u8DeviceAddr, UINT8 u8WordAddr, UINT8* pu8DataBuff, UINT8 u8DataLen)
{
	UINT8	u8ExpectedByteTransfer;
	UINT8	u8Status;
	UINT8	u8ReadReg;
	UINT8	u8Addr_Cmd;
	UINT8	ib;
	UINT8	u8EIE1;
	UINT8	u8EIE2;
	UINT8	u8IE;

	if(I2C_CLK_STATE == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_CLOCK_OFF);
		return RETURN_I2C_CLOCK_OFF;
	}

	if(u8DataLen == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	if(u8DataLen > 16)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	SYS_WdtSet(0);

	u8EIE1 = EIE1;
	u8EIE2 = EIE2;
	u8IE = IE;

	EIE2 = BIT1;
	EIE1 = 0;
	IE = BIT7;

	xI2C_CTR = (I2C_CORE_OFF | I2C_RXFIFO_RST | I2C_TXFIFO_RST);
	u8ReadReg = xI2C_FINTVAL;
	EA = 0;
	u8ReadReg = xI2C_STR;
	DPH = 0;
	EA = 1;

	u8Addr_Cmd = (u8DeviceAddr & 0x7F) << 1;
	u8Addr_Cmd |= I2C_WRITE_BIT;

	xI2C_ADDR = u8Addr_Cmd;
	EA = 0;
	xI2C_DATA = u8WordAddr;
	DPH = 0;
	EA = 1;
	xI2C_RXLVL = u8DataLen;
	xI2C_IMSK = BIT6 | BIT2;			// Master-NACK, Byte-Transfer

	xI2C_CTR &= ~I2C_CORE_OFF;		// core enable

	gu8I2CNackedFlag = 0;
	u8ExpectedByteTransfer = 2;
	gu8I2CByteTransferIntCount = 0;

	xI2C_CTR |= (I2C_INT_ON | I2C_MASTER | I2C_START | I2C_NACK | I2C_REP_START);

	u8Status = RETURN_I2C_SUCCESS;
	while(1)
	{
		if(gu8I2CNackedFlag)
		{
			u8Status = RETURN_I2C_NACK_FROM_SLAVE;
			break;
		}

		if(u8ExpectedByteTransfer == gu8I2CByteTransferIntCount)
		{
			break;
		}
	}

	// If failed,
	if(u8Status)
	{
		while(!(I2C_SCL && I2C_SDA));		// wait until SCL=1 and SDA=1

		xI2C_CTR = I2C_CORE_OFF;

		EIE2 = u8EIE2;
		EIE1 = u8EIE1;
		IE = u8IE;

		return	u8Status;
	}

	HAL_I2COneClockDelay();

	xI2C_CTR &= ~I2C_REP_START;		// clear repeated START 
	xI2C_ADDR |= BIT0;				// Read Operations

	while(!gu8I2CNackedFlag);

	for(ib=0 ; ib<u8DataLen ; ib++)
	{
		EA = 0;
		pu8DataBuff[ib] = xI2C_DATA;
		DPH = 0;
		EA = 1;
	}

	while(!(I2C_SCL && I2C_SDA));		// wait until SCL=1 and SDA=1

	xI2C_CTR = I2C_CORE_OFF;

	EIE2 = u8EIE2;
	EIE1 = u8EIE1;
	IE = u8IE;

	return u8Status;	
}

#else

// 측정된 loop count 이상 응답이 없으면 status 를 set 하고 강제 exit
UINT8 HAL_I2CMaster_RandomRead(UINT8 u8DeviceAddr, UINT8 u8WordAddr, UINT8* pu8DataBuff, UINT8 u8DataLen)
{
	UINT8	u8ExpectedByteTransfer;
	UINT8	u8Status;
	UINT8	u8ReadReg;
	UINT8	u8Addr_Cmd;
	UINT8	ib;
	UINT8	u8EIE1;
	UINT8	u8EIE2;
	UINT8	u8IE;

	UINT8	loop_count = 0;



	if(I2C_CLK_STATE == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_CLOCK_OFF);
		return RETURN_I2C_CLOCK_OFF;
	}

	if(u8DataLen == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	if(u8DataLen > 16)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	SYS_WdtSet(0);

	u8EIE1 = EIE1;
	u8EIE2 = EIE2;
	u8IE = IE;

	EIE2 = BIT1;
	EIE1 = 0;
	IE = BIT7;

	xI2C_CTR = (I2C_CORE_OFF | I2C_RXFIFO_RST | I2C_TXFIFO_RST);
	u8ReadReg = xI2C_FINTVAL;
	EA = 0;
	u8ReadReg = xI2C_STR;
	DPH = 0;
	EA = 1;

	u8Addr_Cmd = (u8DeviceAddr & 0x7F) << 1;
	u8Addr_Cmd |= I2C_WRITE_BIT;

	xI2C_ADDR = u8Addr_Cmd;
	EA = 0;
	xI2C_DATA = u8WordAddr;
	DPH = 0;
	EA = 1;
	xI2C_RXLVL = u8DataLen;
	xI2C_IMSK = BIT6 | BIT2;			// Master-NACK, Byte-Transfer

	xI2C_CTR &= ~I2C_CORE_OFF;		// core enable

	gu8I2CNackedFlag = 0;
	u8ExpectedByteTransfer = 2;
	gu8I2CByteTransferIntCount = 0;

	xI2C_CTR |= (I2C_INT_ON | I2C_MASTER | I2C_START | I2C_NACK | I2C_REP_START);


	u8Status = RETURN_I2C_SUCCESS;
	while(1)				// 0x0A : 여기서 i2c가 멈춤
	{
		loop_count++;				// 0x0b : normal loop count
		if(gu8I2CNackedFlag) {
			u8Status = RETURN_I2C_NACK_FROM_SLAVE;
			break;
		}

		if(u8ExpectedByteTransfer == gu8I2CByteTransferIntCount) 	break;
		if (loop_count >= 0x50) { u8Status = 0x80; break; }
	}

	// If failed,
	if(u8Status)
	{
	
		//while(!(I2C_SCL && I2C_SDA));		// wait until SCL=1 and SDA=1
		loop_count = 0;
		while(!(I2C_SCL && I2C_SDA)) {
			loop_count++;
			if (loop_count >= 0x20)  	{u8Status = 0x80;  break;}
		};		// wait until SCL=1 and SDA=1

		xI2C_CTR = I2C_CORE_OFF;

		EIE2 = u8EIE2;
		EIE1 = u8EIE1;
		IE = u8IE;

		return	u8Status;
	}

	HAL_I2COneClockDelay();

	xI2C_CTR &= ~I2C_REP_START;		// clear repeated START 
	xI2C_ADDR |= BIT0;				// Read Operations

	//----------------------------------------------------
	// original code
	//while(!gu8I2CNackedFlag);				// 0x43

	loop_count = 0;
	while(!gu8I2CNackedFlag) {				// 0x43 : normal loop count
		loop_count++;
		
		if (loop_count >= 0xE0) { u8Status = 0x80; break; }
	};		// wait until SCL=1 and SDA=1

	//----------------------------------------------------

	for(ib=0 ; ib<u8DataLen ; ib++)
	{
		EA = 0;
		pu8DataBuff[ib] = xI2C_DATA;
		DPH = 0;
		EA = 1;
	}

	//----------------------------------------------------
	// original code
	//while(!(I2C_SCL && I2C_SDA));		// wait until SCL=1 and SDA=1

	loop_count = 0;

	while(!(I2C_SCL && I2C_SDA)) {		// loop_count 0x02 or 0x00
		loop_count++;
		if (loop_count >= 0x20) { u8Status = 0x80; break; }
		
	};		// wait until SCL=1 and SDA=1
	//----------------------------------------------------

	xI2C_CTR = I2C_CORE_OFF;

	EIE2 = u8EIE2;
	EIE1 = u8EIE1;
	IE = u8IE;

	return u8Status;	
}


#endif

#if 0
// org code

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Writes I2C data sequentially
///
///	@param	u8DeviceAddr	: Device address of the I2C slave device. 0x00 ~ 0x7F.
///	@param	pu8DataBuff	: Pointer to the buffer of the data to be written.
///	@param	u8DataLen	: Number of data to be written
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_I2CMaster_SequentialWrite(UINT8 u8DeviceAddr, UINT8* pu8DataBuff, UINT8 u8DataLen)
{
	UINT8	u8ExpectedByteTransfer;
	UINT8	u8Status;
	UINT8	u8ReadReg;
	UINT8	u8Addr_Cmd;
	UINT8	ib;
	UINT8	u8EIE1;
	UINT8	u8EIE2;
	UINT8	u8IE;

	if(I2C_CLK_STATE == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_CLOCK_OFF);
		return RETURN_I2C_CLOCK_OFF;
	}

	if(u8DataLen == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	if(u8DataLen > 16)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	SYS_WdtSet(0);

	u8EIE1 = EIE1;
	u8EIE2 = EIE2;
	u8IE = IE;

	EIE2 = BIT1;
	EIE1 = 0;
	IE = BIT7;

	xI2C_CTR = (I2C_CORE_OFF | I2C_RXFIFO_RST | I2C_TXFIFO_RST);
	u8ReadReg = xI2C_FINTVAL;
	EA = 0;
	u8ReadReg = xI2C_STR;
	DPH = 0;
	EA = 1;

	u8Addr_Cmd = (u8DeviceAddr & 0x7F) << 1;
	u8Addr_Cmd |= I2C_WRITE_BIT;

	xI2C_ADDR = u8Addr_Cmd;	
	for(ib=0 ; ib<u8DataLen ; ib++)
	{
		EA = 0;
		xI2C_DATA = pu8DataBuff[ib];
		DPH = 0;
		EA = 1;
	}
	xI2C_IMSK = BIT6 | BIT2;

	xI2C_CTR &= ~I2C_CORE_OFF;

	gu8I2CNackedFlag = 0;
	u8ExpectedByteTransfer = 1 + u8DataLen;
	gu8I2CByteTransferIntCount = 0;

	xI2C_CTR |= (I2C_INT_ON | I2C_MASTER | I2C_START);

	u8Status = RETURN_I2C_SUCCESS;
	while(1)
	{
		if(gu8I2CNackedFlag)
		{
			u8Status = RETURN_I2C_NACK_FROM_SLAVE;
			break;
		}

		if(u8ExpectedByteTransfer == gu8I2CByteTransferIntCount)
		{
			break;
		}
	}

	if(u8Status == RETURN_I2C_SUCCESS)
	{
		HAL_I2COneClockDelay();

		xI2C_CTR &= ~I2C_START;			// 1-->0 : STOP
	}

	while(!(I2C_SCL && I2C_SDA));		// wait until SCL=1 and SDA=1

	xI2C_CTR = I2C_CORE_OFF;

	EIE2 = u8EIE2;
	EIE1 = u8EIE1;
	IE = u8IE;

	return u8Status;	
}

#else


// 측정된 loop count 이상 응답이 없으면 status 를 set 하고 강제 exit
UINT8 HAL_I2CMaster_SequentialWrite(UINT8 u8DeviceAddr, UINT8* pu8DataBuff, UINT8 u8DataLen)
{
	UINT8	u8ExpectedByteTransfer;
	UINT8	u8Status;
	UINT8	u8ReadReg;
	UINT8	u8Addr_Cmd;
	UINT8	ib;
	UINT8	u8EIE1;
	UINT8	u8EIE2;
	UINT8	u8IE;

	UINT8 	loop_count = 0;


	if(I2C_CLK_STATE == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_CLOCK_OFF);
		return RETURN_I2C_CLOCK_OFF;
	}

	if(u8DataLen == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	if(u8DataLen > 16)
	{
		assert_error(I2C_CODE | RETURN_I2C_INVALID_PARAMETER);
		return RETURN_I2C_INVALID_PARAMETER;
	}

	SYS_WdtSet(0);

	u8EIE1 = EIE1;
	u8EIE2 = EIE2;
	u8IE = IE;

	EIE2 = BIT1;
	EIE1 = 0;
	IE = BIT7;

	xI2C_CTR = (I2C_CORE_OFF | I2C_RXFIFO_RST | I2C_TXFIFO_RST);
	u8ReadReg = xI2C_FINTVAL;
	EA = 0;
	u8ReadReg = xI2C_STR;
	DPH = 0;
	EA = 1;

	u8Addr_Cmd = (u8DeviceAddr & 0x7F) << 1;
	u8Addr_Cmd |= I2C_WRITE_BIT;

	xI2C_ADDR = u8Addr_Cmd;	
	for(ib=0 ; ib<u8DataLen ; ib++)
	{
		EA = 0;
		xI2C_DATA = pu8DataBuff[ib];
		DPH = 0;
		EA = 1;
	}
	xI2C_IMSK = BIT6 | BIT2;

	xI2C_CTR &= ~I2C_CORE_OFF;

	gu8I2CNackedFlag = 0;
	u8ExpectedByteTransfer = 1 + u8DataLen;
	gu8I2CByteTransferIntCount = 0;

	xI2C_CTR |= (I2C_INT_ON | I2C_MASTER | I2C_START);

	u8Status = RETURN_I2C_SUCCESS;
	
	while(1)
	{
		loop_count++;
		if(gu8I2CNackedFlag) {
			u8Status = RETURN_I2C_NACK_FROM_SLAVE;
			break;
		}

		if(u8ExpectedByteTransfer == gu8I2CByteTransferIntCount) 	break;
		if (loop_count >= 0x40) { u8Status = 0x80; break;	}
	}

	if(u8Status == RETURN_I2C_SUCCESS)
	{
		HAL_I2COneClockDelay();
		xI2C_CTR &= ~I2C_START;			// 1-->0 : STOP
	}


	//----------------------------------------------------
	// original code
	//while(!(I2C_SCL && I2C_SDA));		// wait until SCL=1 and SDA=1

	loop_count = 0;

	while(!(I2C_SCL && I2C_SDA)) {		// loop_count 0x02 or 0x00
		if (loop_count++ >= 0x10) { u8Status = 0x80; 	break; }
	};		// wait until SCL=1 and SDA=1

	//----------------------------------------------------

	xI2C_CTR = I2C_CORE_OFF;

	EIE2 = u8EIE2;
	EIE1 = u8EIE1;
	IE = u8IE;

	return u8Status;	
}

#endif


///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Reads I2C data sequentially
///
///	@param	u8DeviceAddr	: Device address of the I2C slave device. 0x00 ~ 0x7F.
///	@param	pu8DataBuff	: Pointer to the buffer of the data to be read.
///	@param	u8DataLen	: Number of data to be read
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_I2CMaster_SequentialRead(UINT8 u8DeviceAddr, UINT8* pu8DataBuff, UINT8 u8DataLen)
{
	UINT8	u8Status;
	UINT8	u8ReadReg;
	UINT8	u8Addr_Cmd;
	UINT8	ib;
	UINT8	u8EIE1;
	UINT8	u8EIE2;
	UINT8	u8IE;
	UINT8	u8PostReadIdx;

	if(I2C_CLK_STATE == 0)
	{
		assert_error(I2C_CODE | RETURN_I2C_CLOCK_OFF);
		return RETURN_I2C_CLOCK_OFF;
	}

	SYS_WdtSet(0);

	u8EIE1 = EIE1;
	u8EIE2 = EIE2;
	u8IE = IE;

	EIE2 = BIT1;
	EIE1 = 0;
	IE = BIT7;

	xI2C_CTR = (I2C_CORE_OFF | I2C_RXFIFO_RST | I2C_TXFIFO_RST);
	u8ReadReg = xI2C_FINTVAL;
	EA = 0;
	u8ReadReg = xI2C_STR;
	DPH = 0;
	EA = 1;

	u8Addr_Cmd = (u8DeviceAddr & 0x7F) << 1;
	u8Addr_Cmd |= I2C_READ_BIT;

	xI2C_ADDR = u8Addr_Cmd;
	if(u8DataLen > 16)
	{
		xI2C_RXLVL = 16;
		gu8I2CMasterPreReadLength = u8DataLen - 16;
	}
	else
	{
		xI2C_RXLVL = u8DataLen;
		gu8I2CMasterPreReadLength = 0;
	}
	xI2C_IMSK = BIT6 | BIT2;			// Master-NACK, Byte-Transfer

	xI2C_CTR &= ~I2C_CORE_OFF;		// core enable

	gu8I2CNackedFlag = 0;
	u8PostReadIdx = gu8I2CMasterPreReadLength;
	gu8I2CByteTransferIntCount = 0;
	gpu8I2CMasterReadData = pu8DataBuff;

	xI2C_CTR |= (I2C_INT_ON | I2C_MASTER | I2C_START | I2C_NACK);

	u8Status = RETURN_I2C_SUCCESS;
	while(1)
	{
		if(gu8I2CNackedFlag)
		{
			break;
		}
	}

	for(ib=0 ; ib<xI2C_RXLVL ; ib++)
	{
		EA = 0;
		pu8DataBuff[u8PostReadIdx+ib] = xI2C_DATA;
		DPH = 0;
		EA = 1;
	}

	while(!(I2C_SCL && I2C_SDA));		// wait until SCL=1 and SDA=1

	xI2C_CTR = I2C_CORE_OFF;

	EIE2 = u8EIE2;
	EIE1 = u8EIE1;
	IE = u8IE;

	return u8Status;	
}




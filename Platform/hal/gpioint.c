
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "gpioint.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Controls GPIO Interrupt and the priority. 
///	\n	To use GPIO Interrupt , EIE2[2] should be set by this function.
///		And, the corresponding bit of Px_IRQ_EN should be also set by HAL_GpioIntxSet().
///
///	@param	u8IntEna	: 0=Disable, 1=Enable.
///	@param	u8Priority	: Priority of Interrupt. 0=Low, 1=High.
///	@return	void
///	@note	To enable each GPIO interrupt, refer to HAL_GpioIntxSet().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_GpioIntModeSet(UINT8 u8IntEna, UINT8 u8Priority)
{
	if(u8Priority)
	{
		EIP2 |= BIT2;
	}
	else
	{
		EIP2 &= ~BIT2;
	}

	if(u8IntEna)
	{
		EIE2 |= BIT2;
	}
	else
	{
		EIE2 &= ~BIT2;
	}
}



#ifdef __MG2470_GPIOINT0_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether GPIO interrupt of GPIO0 is generated.
///	When GPIO interrupt of GPIO0 is generated, the flag is set to this.
///	And after processing the interrupt, it should be cleared.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8GpioInt0Flag = 0x00;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for GPIO Interrupt of GPIO0.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_GpioInt0ClockOn(UINT8 u8On)
{
	if(u8On)
	{
		GPIOINT0_CLK_ON;
	}
	else
	{
		GPIOINT0_CLK_OFF;	
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets GPIO0 port to be used for GPIO Interrupt.
///	Additionally, it specifies the priority and triggering type of the interrupt.
///	When an interrupt is generated, GPIO_ISR() is called and P0_IRQ_STS specifies which GPIO causes the interrupt.
///	\n	When this is called with "u8IntEna=1", EIE[2] is also set to enable GPIO Interrupt. 
///		In other words, Calling HAL_GpioIntModeSet() is not needed.
///	\n	But if this is called with "u8IneEna=0", EIE[2] is not cleared. Because, another ports are still used
///		for GPIO Interrupt.
///
///	@param	u8Port		: 0~7=Port Number, 0xA=All Ports.
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of Interrupt. 0=Low, 1=High.
///	@param	u8TrigType	: Triggering type of GPIO Interrupt.
/// 							\n 0=Low level.
///							\n 1=Falling edge.
///							\n 2=High level.
///							\n 3=Rising edge.
///	@return	UINT8. Status.
///	@note	To control EIE[2] independently, use HAL_GpioIntModeSet().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_GpioInt0Set(UINT8 u8Port, UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8TrigType)
{
	UINT8	u8MaskOR;
	UINT8	u8MaskAND;
	UINT8	u8Status;

	if(GPIOINT0_CLK_STATE == 0)
	{
		assert_error(GPIOINT_CODE | RETURN_GPIOINT0_CLOCK_OFF);
		return RETURN_GPIOINT0_CLOCK_OFF;
	}

	if(u8TrigType > 3)
	{		
		assert_error(GPIOINT_CODE | RETURN_GPIOINT0_INVALID_TRIG_TYPE);
		return RETURN_GPIOINT0_INVALID_TRIG_TYPE;
	}

	u8Status = RETURN_GPIOINT_SUCCESS;
	switch(u8Port)
	{
		case 0x0	: u8MaskOR = BIT0;	u8MaskAND = ~BIT0;	break;
		case 0x1	: u8MaskOR = BIT1;	u8MaskAND = ~BIT1;	break;
		case 0x2	: u8MaskOR = BIT2;	u8MaskAND = ~BIT2;	break;
		case 0x3	: u8MaskOR = BIT3;	u8MaskAND = ~BIT3;	break;
		case 0x4	: u8MaskOR = BIT4;	u8MaskAND = ~BIT4;	break;
		case 0x5	: u8MaskOR = BIT5;	u8MaskAND = ~BIT5;	break;
		case 0x6	: u8MaskOR = BIT6;	u8MaskAND = ~BIT6;	break;
		case 0x7	: u8MaskOR = BIT7;	u8MaskAND = ~BIT7;	break;
		case 0xA	: u8MaskOR = 0xFF;	u8MaskAND = ~0xFF;	break;
		default	: u8Status = RETURN_GPIOINT0_INVALID_PORT;	break;
	}

	if(u8Status)
	{
		assert_error(GPIOINT_CODE | u8Status);
		return u8Status;
	}	

	if(u8IntEna)
	{
		if(u8TrigType & BIT1)			// TrigType = 2 or 3
		{
			P0_POL |= u8MaskOR;
		}
		else							// TrigType = 0 or 1
		{
			P0_POL &= u8MaskAND;
		}

		if(u8TrigType & BIT0)			// TrigType = 1 or 3. Edge
		{
			P0_EDGE |= u8MaskOR;
		}
		else							// TrigType = 0 or 2. Level
		{
			P0_EDGE &= u8MaskAND;
		}

		P0_IRQ_EN |= u8MaskOR;

		HAL_GpioIntModeSet(1, u8Priority);		
	}
	else
	{
		P0_IRQ_EN &= u8MaskAND;
	}

	return RETURN_GPIOINT_SUCCESS;
}

#endif		// __MG2470_GPIOINT0_INCLUDE__







#ifdef __MG2470_GPIOINT1_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether GPIO interrupt of GPIO1 is generated.
///	When GPIO interrupt of GPIO1 is generated, the flag is set to this.
///	And after processing the interrupt, it should be cleared.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8GpioInt1Flag = 0x00;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for GPIO Interrupt of GPIO1.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_GpioInt1ClockOn(UINT8 u8On)
{
	if(u8On)
	{
		GPIOINT1_CLK_ON;
	}
	else
	{
		GPIOINT1_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets GPIO1 port to be used for GPIO Interrupt.
///	Additionally, it specifies the priority and triggering type of the interrupt.
///	When an interrupt is generated, GPIO_ISR() is called and P1_IRQ_STS specifies which GPIO causes the interrupt.
///	\n	When this is called with "u8IntEna=1", EIE[2] is also set to enable GPIO Interrupt. 
///		In other words, Calling HAL_GpioIntModeSet() is not needed.
///	\n	But if this is called with "u8IneEna=0", EIE[2] is not cleared. Because, another ports are still used
///		for GPIO Interrupt.
///
///	@param	u8Port		: 0~7=Port Number, 0xA=All Ports.
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of Interrupt. 0=Low, 1=High.
///	@param	u8TrigType	: Triggering type of GPIO Interrupt.
/// 							\n 0=Low level.
///							\n 1=Falling edge.
///							\n 2=High level.
///							\n 3=Rising edge.
///	@return	UINT8. Status.
///	@note	To control EIE[2] independently, use HAL_GpioIntModeSet().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_GpioInt1Set(UINT8 u8Port, UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8TrigType)
{
	UINT8	u8MaskOR;
	UINT8	u8MaskAND;
	UINT8	u8Status;

	if(GPIOINT1_CLK_STATE == 0)
	{
		assert_error(GPIOINT_CODE | RETURN_GPIOINT1_CLOCK_OFF);
		return RETURN_GPIOINT1_CLOCK_OFF;
	}

	if(u8TrigType > 3)
	{		
		assert_error(GPIOINT_CODE | RETURN_GPIOINT1_INVALID_TRIG_TYPE);
		return RETURN_GPIOINT1_INVALID_TRIG_TYPE;
	}

	u8Status = RETURN_GPIOINT_SUCCESS;
	switch(u8Port)
	{
		case 0x0	: u8MaskOR = BIT0;	u8MaskAND = ~BIT0;	break;
		case 0x1	: u8MaskOR = BIT1;	u8MaskAND = ~BIT1;	break;
		case 0x2	: u8MaskOR = BIT2;	u8MaskAND = ~BIT2;	break;
		case 0x3	: u8MaskOR = BIT3;	u8MaskAND = ~BIT3;	break;
		case 0x4	: u8MaskOR = BIT4;	u8MaskAND = ~BIT4;	break;
		case 0x5	: u8MaskOR = BIT5;	u8MaskAND = ~BIT5;	break;
		case 0x6	: u8MaskOR = BIT6;	u8MaskAND = ~BIT6;	break;
		case 0x7	: u8MaskOR = BIT7;	u8MaskAND = ~BIT7;	break;
		case 0xA	: u8MaskOR = 0xFF;	u8MaskAND = ~0xFF;	break;
		default	: u8Status = RETURN_GPIOINT1_INVALID_PORT;	break;
	}

	if(u8Status)
	{
		assert_error(GPIOINT_CODE | u8Status);
		return u8Status;
	}	

	if(u8IntEna)
	{
		if(u8TrigType & BIT1)			// TrigType = 2 or 3
		{
			P1_POL |= u8MaskOR;
		}
		else							// TrigType = 0 or 1
		{
			P1_POL &= u8MaskAND;
		}

		if(u8TrigType & BIT0)			// TrigType = 1 or 3. Edge
		{
			P1_EDGE |= u8MaskOR;
		}
		else							// TrigType = 0 or 2. Level
		{
			P1_EDGE &= u8MaskAND;
		}

		P1_IRQ_EN |= u8MaskOR;

		HAL_GpioIntModeSet(1, u8Priority);		
	}
	else
	{
		P1_IRQ_EN &= u8MaskAND;
	}

	return RETURN_GPIOINT_SUCCESS;
}

#endif		// __MG2470_GPIOINT1_INCLUDE__









#ifdef __MG2470_GPIOINT3_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether GPIO interrupt of GPIO3 is generated.
///	When GPIO interrupt of GPIO3 is generated, the flag is set to this.
///	And after processing the interrupt, it should be cleared.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8GpioInt3Flag = 0x00;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for GPIO Interrupt of GPIO3.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_GpioInt3ClockOn(UINT8 u8On)
{
	if(u8On)
	{
		GPIOINT3_CLK_ON;
	}
	else
	{
		GPIOINT3_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets GPIO3 port to be used for GPIO Interrupt.
///	Additionally, it specifies the priority and triggering type of the interrupt.
///	When an interrupt is generated, GPIO_ISR() is called and P3_IRQ_STS specifies which GPIO causes the interrupt.
///	\n	When this is called with "u8IntEna=1", EIE[2] is also set to enable GPIO Interrupt. 
///		In other words, Calling HAL_GpioIntModeSet() is not needed.
///	\n	But if this is called with "u8IneEna=0", EIE[2] is not cleared. Because, another ports are still used
///		for GPIO Interrupt.
///
///	@param	u8Port		: 0~1, 4~7=Port Number, 0xA=All Ports except P3.2 and P3.3
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of Interrupt. 0=Low, 1=High.
///	@param	u8TrigType	: Triggering type of GPIO Interrupt.
/// 							\n 0=Low level.
///							\n 1=Falling edge.
///							\n 2=High level.
///							\n 3=Rising edge.
///	@return	UINT8. Status.
///	@note	To control EIE[2] independently, use HAL_GpioIntModeSet().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_GpioInt3Set(UINT8 u8Port, UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8TrigType)
{
	UINT8	u8MaskOR;
	UINT8	u8MaskAND;
	UINT8	u8Status;

	if(GPIOINT3_CLK_STATE == 0)
	{
		assert_error(GPIOINT_CODE | RETURN_GPIOINT3_CLOCK_OFF);
		return RETURN_GPIOINT3_CLOCK_OFF;
	}

	if(u8TrigType > 3)
	{		
		assert_error(GPIOINT_CODE | RETURN_GPIOINT3_INVALID_TRIG_TYPE);
		return RETURN_GPIOINT3_INVALID_TRIG_TYPE;
	}

	u8Status = RETURN_GPIOINT_SUCCESS;
	switch(u8Port)
	{
		case 0x0	: u8MaskOR = BIT0;	u8MaskAND = ~BIT0;	break;
		case 0x1	: u8MaskOR = BIT1;	u8MaskAND = ~BIT1;	break;
		//case 0x2	: 
		//case 0x3	: 
		case 0x4	: u8MaskOR = BIT4;	u8MaskAND = ~BIT4;	break;
		case 0x5	: u8MaskOR = BIT5;	u8MaskAND = ~BIT5;	break;
		case 0x6	: u8MaskOR = BIT6;	u8MaskAND = ~BIT6;	break;
		case 0x7	: u8MaskOR = BIT7;	u8MaskAND = ~BIT7;	break;
		case 0xA	: u8MaskOR = 0xFF;	u8MaskAND = ~0xFF;	break;
		default	: u8Status = RETURN_GPIOINT3_INVALID_PORT;	break;
	}

	if(u8Status)
	{
		assert_error(GPIOINT_CODE | u8Status);
		return u8Status;
	}	

	if(u8IntEna)
	{
		if(u8TrigType & BIT1)			// TrigType = 2 or 3
		{
			P3_POL |= u8MaskOR;
		}
		else							// TrigType = 0 or 1
		{
			P3_POL &= u8MaskAND;
		}

		if(u8TrigType & BIT0)			// TrigType = 1 or 3. Edge
		{
			P3_EDGE |= u8MaskOR;
		}
		else							// TrigType = 0 or 2. Level
		{
			P3_EDGE &= u8MaskAND;
		}

		P3_IRQ_EN |= u8MaskOR;

		HAL_GpioIntModeSet(1, u8Priority);		
	}
	else
	{
		P3_IRQ_EN &= u8MaskAND;
	}

	return RETURN_GPIOINT_SUCCESS;
}

#endif		// __MG2470_GPIOINT3_INCLUDE__





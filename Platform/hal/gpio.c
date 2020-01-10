
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "gpio.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for GPIO.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_GpioClockOn(UINT8 u8On)		// Default On
{
	if(u8On)
	{
		GPIO_CLK_ON;
	}
	else
	{
		GPIO_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Checks whether parameters is valid.
///
///	@param	u8Port	: 0~7=Port Number, 0xA=All Ports.
///	@param	u8OutEna	: 0=Input mode, 1=Output mode.
///	@param	u8Option	: Options for GPIO.
///				\n	When u8OutEna=1	:  Drive Strength. 0=4mA, 1=8mA.
///				\n	when u8OutEna=0	:  Pull-up/down or Hi-impedance. 0=Pull-up, 1=Pull-down, 2=High-impedance.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_GpioParameterCheck(UINT8 u8Port, UINT8 u8OutEna, UINT8 u8Option)
{
	if( (u8Port > 0x07) && (u8Port != 0x0A) )
	{
		return RETURN_GPIO_INVALID_PORT;
	}

	if(u8OutEna)
	{
		if(u8Option > 1)
		{
			return RETURN_GPIO_INVALID_PARAMETER;
		}		
	}
	else
	{
		if(u8Option > 2)
		{
			return RETURN_GPIO_INVALID_PARAMETER;
		}
	}	

	return RETURN_GPIO_SUCCESS;
}

#ifdef __MG2470_GPIO0_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the mode of GPIO0(0.x) ports.
///
///	@param	u8Port	: 0~7=Port Number, 0xA=All Ports.
///	@param	u8OutEna	: 0=Input mode, 1=Output mode.
///	@param	u8Option	: Options for GPIO.
///				\n	When u8OutEna=1	:  Drive Strength. 0=4mA, 1=8mA.
///				\n	when u8OutEna=0	:  Pull-up/down or Hi-impedance. 0=Pull-up, 1=Pull-down, 2=High-impedance.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Gpio0InOutSet(UINT8 u8Port, UINT8 u8OutEna, UINT8 u8Option)
{
	UINT8	u8MaskOR;
	UINT8	u8MaskAND;
	UINT8	u8Status;

	if(GPIO_CLK_STATE == 0)
	{
		assert_error(GPIO_CODE | RETURN_GPIO_CLOCK_OFF);
		return RETURN_GPIO_CLOCK_OFF;
	}

	u8Status = HAL_GpioParameterCheck(u8Port, u8OutEna, u8Option);
	if(u8Status)
	{
		assert_error(GPIO_CODE | u8Status);
		return u8Status;
	}	

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
	}

	if(u8OutEna)
	{
		P0OEN &= u8MaskAND;		// clear bits. Output Enable(Active low)
		xGPIOPE0 &= u8MaskAND;	// PEx[n]=0. High-impedance

		if(u8Option)
		{
			P0_DS |= u8MaskOR;	// set bits
		}
		else
		{
			P0_DS &= u8MaskAND;	// clear bits
		}
	}
	else
	{
		if(u8Option == 0)		// pull-up
		{
			xGPIOPE0 |= u8MaskOR;	// PEx[n]=1
			xGPIOPS0 |= u8MaskOR;	// PSx[n]=1				
		}
		else if(u8Option == 1)	// pull-down
		{
			xGPIOPE0 |= u8MaskOR;	// PEx[n]=1
			xGPIOPS0 &= u8MaskAND;	// PSx[n]=0
		}
		else		// high-impedance
		{
			xGPIOPE0 &= u8MaskAND;	// PEx[n]=0
		}
		P0OEN |= u8MaskOR;		// set bits. Output disable(Active low)
		P0_IE |= u8MaskOR;		// set bits. Input enable(Active high)
	}

	return RETURN_GPIO_SUCCESS;
}

#endif // #ifdef __MG2470_GPIO0_INCLUDE__




#ifdef __MG2470_GPIO1_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the mode of GPIO1(1.x) ports.
///
///	@param	u8Port	: 0~7=Port Number, 0xA=All Ports.
///	@param	u8OutEna	: 0=Input mode, 1=Output mode.
///	@param	u8Option	: Options for GPIO.
///				\n	When u8OutEna=1	:  Drive Strength. 0=4mA, 1=8mA.
///				\n	when u8OutEna=0	:  Pull-up/down or Hi-impedance. 0=Pull-up, 1=Pull-down, 2=High-impedance.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Gpio1InOutSet(UINT8 u8Port, UINT8 u8OutEna, UINT8 u8Option)
{
	UINT8	u8MaskOR;
	UINT8	u8MaskAND;
	UINT8	u8Status;

	if(GPIO_CLK_STATE == 0)	
	{
		assert_error(GPIO_CODE | RETURN_GPIO_CLOCK_OFF);
		return RETURN_GPIO_CLOCK_OFF;
	}

	u8Status = HAL_GpioParameterCheck(u8Port, u8OutEna, u8Option);
	if(u8Status)
	{
		assert_error(GPIO_CODE | u8Status);
		return u8Status;
	}	

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
	}

	if(u8OutEna)
	{
		P1OEN &= u8MaskAND;		// clear bits. Output Enable(Active low)
		xGPIOPE1 &= u8MaskAND;	// PEx[n]=0. High-impedance

		if(u8Option)
		{
			P1_DS |= u8MaskOR;	// set bits
		}
		else
		{
			P1_DS &= u8MaskAND;	// clear bits
		}
	}
	else
	{
		if(u8Option == 0)		// pull-up
		{
			xGPIOPE1 |= u8MaskOR;	// PEx[n]=1
			xGPIOPS1 |= u8MaskOR;	// PSx[n]=1				
		}
		else if(u8Option == 1)	// pull-down
		{
			xGPIOPE1 |= u8MaskOR;	// PEx[n]=1
			xGPIOPS1 &= u8MaskAND;	// PSx[n]=0				
		}
		else		// high-impedance
		{
			xGPIOPE1 &= u8MaskAND;	// PEx[n]=0
		}
		P1OEN |= u8MaskOR;		// set bits. Output disable(Active low)
		P1_IE |= u8MaskOR;		// set bits. Input enable(Active high)
	}

	return RETURN_GPIO_SUCCESS;
}

#endif // #ifdef __MG2470_GPIO1_INCLUDE__




#ifdef __MG2470_GPIO3_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the mode of GPIO3(3.x) ports.
///
///	@param	u8Port	: 0~7=Port Number, 0xA=All Ports.
///	@param	u8OutEna	: 0=Input mode, 1=Output mode.
///	@param	u8Option	: Options for GPIO.
///				\n	When u8OutEna=1	:  Drive Strength. 0=4mA, 1=8mA.
///				\n	when u8OutEna=0	:  Pull-up/down or Hi-impedance. 0=Pull-up, 1=Pull-down, 2=High-impedance.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Gpio3InOutSet(UINT8 u8Port, UINT8 u8OutEna, UINT8 u8Option)
{
	UINT8	u8MaskOR;
	UINT8	u8MaskAND;
	UINT8	u8Status;

	if(GPIO_CLK_STATE == 0)
	{
		assert_error(GPIO_CODE | RETURN_GPIO_CLOCK_OFF);
		return RETURN_GPIO_CLOCK_OFF;
	}

	u8Status = HAL_GpioParameterCheck(u8Port, u8OutEna, u8Option);
	if(u8Status)
	{
		assert_error(GPIO_CODE | u8Status);
		return u8Status;
	}

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
	}

	if(u8OutEna)
	{
		P3OEN &= u8MaskAND;		// clear bits. Output Enable(Active low)
		xGPIOPE3 &= u8MaskAND;	// PEx[n]=0. High-impedance

		if(u8Option)
		{
			P3_DS |= u8MaskOR;	// set bits
		}
		else
		{
			P3_DS &= u8MaskAND;	// clear bits
		}
	}
	else
	{
		if(u8Option == 0)		// pull-up
		{
			xGPIOPE3 |= u8MaskOR;	// PEx[n]=1
			xGPIOPS3 |= u8MaskOR;	// PSx[n]=1
		}
		else if(u8Option == 1)	// pull-down
		{
			xGPIOPE3 |= u8MaskOR;	// PEx[n]=1
			xGPIOPS3 &= u8MaskAND;	// PSx[n]=0
		}
		else		// high-impedance
		{
			xGPIOPE3 &= u8MaskAND;	// PEx[n]=0
		}
		P3OEN |= u8MaskOR;		// set bits. Output disable(Active low)
		P3_IE |= u8MaskOR;		// set bits. Input enable(Active high)
	}

	return RETURN_GPIO_SUCCESS;
}

#endif // #ifdef __MG2470_GPIO3_INCLUDE__








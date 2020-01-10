
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2014-01-23
	- Version		: V2.3

	[2014-01-23] V2.3
	- Remove compile warning message.

	[2013-03-19] V2.2
	- HAL_Pwm4Set_PwmMode() : bug fix.

	[2012-05-23] V2.1
	- SYS_Pwm0Set_PwmMode() : added
	- SYS_Pwm1Set_PwmMode() : added
	- SYS_Pwm2Set_PwmMode() : added
	- SYS_Pwm3Set_PwmMode() : added
	- SYS_Pwm4Set_PwmMode() : added

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "pwm.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Controls PWM Interrupt and the priority. 
///	\n	To use PWM Interrupt for timer , EIE2[3] should be set by this function.
///		And, PWMx_CTRL[7] should be also set by HAL_PwmxSet_TimerMode().
///
///	@param	u8IntEna	: 0=Disable, 1=Enable.
///	@param	u8Priority	: Priority of Interrupt. 0=Low, 1=High.
///	@return	void
///	@note	To enable each GPIO interrupt, refer to HAL_PwmxSet_TimerMode().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_PwmIntModeSet(UINT8 u8IntEna, UINT8 u8Priority)
{
	if(u8Priority)
	{
		EIP2 |= BIT3;
	}
	else
	{
		EIP2 &= ~BIT3;
	}

	if(u8IntEna)
	{
		EIE2 |= BIT3;
	}
	else
	{
		EIE2 &= ~BIT3;
	}
}


#ifdef __MG2470_PWM0_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for PWM0.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Pwm0ClockOn(UINT8 u8On)		// Default Off
{
	if(u8On)
	{
		PWM0_CLK_ON;
	}
	else
	{
		PWM0_CLK_OFF;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PWM0 as PWM mode. GPIO0_0 is output of PWM signal.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u16Period	: Period of one PWM signal. This should be equal to or more than "u16HighDuty".
///							\n The period = "u16Period" x 1 clock duty(Refer to @note).
///	@param	u16HighDuty	: Specifies duty time of each level. This should not be more than "u16Period".
///							\n High level duty time = "u16HighDuty" x 1 clock duty(Refer to @note).
///							\n Low level duty time = ("u16Period" - "u16HighDuty") x 1 clock duty(Refer to @note).
///	@param	u8Option		: reserved.
///	@return	UINT8. Status.
///	@note	1 clock duty is 125ns at 8MHz MCU frequency. It is dependent on MCU frequency. 
///			And, The MCU frequency is determined by HAL_McuPeriClockSet().
///				\n 1 clock duty = 125ns * (8MHz / MCU frequency)
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Pwm0Set_PwmMode(UINT8 u8Ena, UINT16 u16Period, UINT16 u16HighDuty, UINT8 u8Option)
{
	if(PWM0_CLK_STATE == 0)
	{
		assert_error(PWM_CODE | RETURN_PWM0_CLOCK_OFF);
		return RETURN_PWM0_CLOCK_OFF;
	}

	if(u16HighDuty > u16Period)
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	xPWM0_CTRL = BIT5;
	if(u8Ena)
	{
		xPWM0_HRCL = u16Period;
		xPWM0_HRCH = u16Period >> 8;
		xPWM0_LRCL = u16HighDuty;
		xPWM0_LRCH = u16HighDuty >> 8;

		xPWM0_CTRL = BIT3 | BIT0;

		xPWM0_HRCL = u16HighDuty;
		xPWM0_HRCH = u16HighDuty >> 8;
		xPWM0_LRCL = u16Period;
		xPWM0_LRCH = u16Period >> 8;
	}

	u8Option = 0;

	return RETURN_PWM_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PWM0 as timer mode. When counter is overflowed, PWM interrupt is generated and PWM_ISR() is called.
///	\n	When this is called with "u8IntEna=1", EIE[3] is also set to enable PWM Interrupt. 
///		In other words, Calling HAL_PwmIntModeSet() is not needed.
///	\n	But if this is called with "u8IneEna=0", EIE[3] is not cleared. Because, another PWM ports are still used
///		for PWM Interrupt.
///
///	@param	u8Ena			: 0=Disable, 1=Enable.
///	@param	u8Priority			: Priority of Interrupt. 0=Low, 1=High.
///	@param	u16Tick_125ns 	: Number of ticks for the interrupt in 1 clock duty(Refer to [note]). 800 ~ 65535.
///								\n Interrupt period = "u16Tick_125ns" x 1 clock duty(Refer to [note]). 
///	@param	u8Option			: reserved.
///	@return	UINT8. Status.
///	@note	1 clock duty is 125ns at 8MHz MCU frequency. It is dependent on MCU frequency. 
///			And, The MCU frequency is determined by HAL_McuPeriClockSet().
///				\n 1 clock duty = 125ns * (8MHz / MCU frequency)
///	@note	To control EIE[3] independently, use HAL_PwmIntModeSet().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Pwm0Set_TimerMode(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_125ns, UINT8 u8Option)
{
	if(PWM0_CLK_STATE == 0)
	{
		assert_error(PWM_CODE | RETURN_PWM0_CLOCK_OFF);
		return RETURN_PWM0_CLOCK_OFF;
	}

	if(u16Tick_125ns < 800)
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	xPWM0_CTRL = BIT5;
	if(u8IntEna)
	{	
		xPWM0_HRCL = u16Tick_125ns;
		xPWM0_HRCH = u16Tick_125ns >> 8;
		xPWM0_LRCL = xPWM0_HRCL;
		xPWM0_LRCH = xPWM0_HRCH;
		xPWM0_CTRL = BIT7 | BIT0;

		HAL_PwmIntModeSet(1, u8Priority);
	}

	u8Option = 0;

	return RETURN_PWM_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
/// 	Sets PWM0 as PWM mode. GPIO0_0 is output of PWM signal.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u32Frequency	: Frequency of PWM signal(Degree : Hz).
///							\n The frequency = "u32Frequency"Hz (Min : 123Hz ~ Max : 80,000Hz).
///	@param	u8HighDuty	: Percentage of "u32Frequency". (Min : 1% ~ Max : 99%)
///	@param	u8Option		: reserved.
///	@return	UINT8. Status.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Pwm0Set_PwmMode(UINT8 u8Ena, UINT32 u32Frequency, UINT8 u8HighDuty, UINT8 u8Option)
{
	UINT16	u16Period;
	UINT16	u16HighDuty;
	UINT8	u8Status;

	if( (u32Frequency > 80000) || (u32Frequency < 123) )
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	if( (u8HighDuty > 99) || (u8HighDuty < 1) )
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	u16Period = 8000000/u32Frequency;	// for 8MHz MCU Clock
	u16HighDuty = ((float)u16Period * ((float)u8HighDuty / 100));
	if(u16HighDuty == 0)	u16HighDuty = 1;

	u8Status = HAL_Pwm0Set_PwmMode(u8Ena, u16Period, u16HighDuty, u8Option);

	return u8Status;

}

#endif	// #ifdef __MG2470_PWM0_INCLUDE__





#ifdef __MG2470_PWM1_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for PWM1.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Pwm1ClockOn(UINT8 u8On)		// Default Off
{
	if(u8On)
	{
		PWM1_CLK_ON;
	}
	else
	{
		PWM1_CLK_OFF;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PWM1 as PWM mode. GPIO0_1 is output of PWM signal.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u16Period	: Period of one PWM signal. This should be equal to or more than "u16HighDuty".
///							\n The period = "u16Period" x 1 clock duty(Refer to @note).
///	@param	u16HighDuty	: Specifies duty time of each level. This should not be more than "u16Period".
///							\n High level duty time = "u16HighDuty" x 1 clock duty(Refer to @note).
///							\n Low level duty time = ("u16Period" - "u16HighDuty") x 1 clock duty(Refer to @note).
///	@param	u8Option		: reserved.
///	@return	UINT8. Status.
///	@note	1 clock duty is 125ns at 8MHz MCU frequency. It is dependent on MCU frequency. 
///			And, The MCU frequency is determined by HAL_McuPeriClockSet().
///				\n 1 clock duty = 125ns * (8MHz / MCU frequency)
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Pwm1Set_PwmMode(UINT8 u8Ena, UINT16 u16Period, UINT16 u16HighDuty, UINT8 u8Option)
{
	if(PWM1_CLK_STATE == 0)
	{
		assert_error(PWM_CODE | RETURN_PWM1_CLOCK_OFF);
		return RETURN_PWM1_CLOCK_OFF;
	}

	if(u16HighDuty > u16Period)
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	xPWM1_CTRL = BIT5;
	if(u8Ena)
	{
		xPWM1_HRCL = u16Period;
		xPWM1_HRCH = u16Period >> 8;
		xPWM1_LRCL = u16HighDuty;
		xPWM1_LRCH = u16HighDuty >> 8;
		
		xPWM1_CTRL = BIT3 | BIT0;

		xPWM1_HRCL = u16HighDuty;
		xPWM1_HRCH = u16HighDuty >> 8;
		xPWM1_LRCL = u16Period;
		xPWM1_LRCH = u16Period >> 8;
	}

	u8Option = 0;

	return RETURN_PWM_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PWM1 as timer mode. When counter is overflowed, PWM interrupt is generated and PWM_ISR() is called.
///	\n	When this is called with "u8IntEna=1", EIE[3] is also set to enable PWM Interrupt. 
///		In other words, Calling HAL_PwmIntModeSet() is not needed.
///	\n	But if this is called with "u8IneEna=0", EIE[3] is not cleared. Because, another PWM ports are still used
///		for PWM Interrupt.
///
///	@param	u8Ena			: 0=Disable, 1=Enable.
///	@param	u8Priority			: Priority of Interrupt. 0=Low, 1=High.
///	@param	u16Tick_125ns 	: Number of ticks for the interrupt in 1 clock duty(Refer to [note]). 800 ~ 65535.
///								\n Interrupt period = "u16Tick_125ns" x 1 clock duty(Refer to [note]). 
///	@param	u8Option			: reserved.
///	@return	UINT8. Status.
///	@note	1 clock duty is 125ns at 8MHz MCU frequency. It is dependent on MCU frequency. 
///			And, The MCU frequency is determined by HAL_McuPeriClockSet().
///				\n 1 clock duty = 125ns * (8MHz / MCU frequency)
///	@note	To control EIE[3] independently, use HAL_PwmIntModeSet().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Pwm1Set_TimerMode(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_125ns, UINT8 u8Option)
{
	if(PWM1_CLK_STATE == 0)
	{
		assert_error(PWM_CODE | RETURN_PWM1_CLOCK_OFF);
		return RETURN_PWM1_CLOCK_OFF;
	}

	if(u16Tick_125ns < 800)
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	xPWM1_CTRL = BIT5;
	if(u8IntEna)
	{
		xPWM1_HRCL = u16Tick_125ns;
		xPWM1_HRCH = u16Tick_125ns >> 8;
		xPWM1_LRCL = xPWM1_HRCL;
		xPWM1_LRCH = xPWM1_HRCH;
		xPWM1_CTRL = BIT7 | BIT0;
		HAL_PwmIntModeSet(1, u8Priority);
	}

	u8Option = 0;

	return RETURN_PWM_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
/// 	Sets PWM1 as PWM mode. GPIO0_1 is output of PWM signal.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u32Frequency	: Frequency of PWM signal(Degree : Hz).
///							\n The frequency = "u32Frequency"Hz (Min : 123Hz ~ Max : 80,000Hz).
///	@param	u8HighDuty	: Percentage of "u32Frequency". (Min : 1% ~ Max : 99%)
///	@param	u8Option		: reserved.
///	@return	UINT8. Status.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Pwm1Set_PwmMode(UINT8 u8Ena, UINT32 u32Frequency, UINT8 u8HighDuty, UINT8 u8Option)
{
	UINT16	u16Period;
	UINT16	u16HighDuty;
	UINT8	u8Status;

	if( (u32Frequency > 80000) || (u32Frequency < 123) )
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	if( (u8HighDuty > 99) || (u8HighDuty < 1) )
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	u16Period = 8000000/u32Frequency;	// for 8MHz MCU Clock
	u16HighDuty = ((float)u16Period * ((float)u8HighDuty / 100));
	if(u16HighDuty == 0)	u16HighDuty = 1;

	u8Status = HAL_Pwm1Set_PwmMode(u8Ena, u16Period, u16HighDuty, u8Option);

	return u8Status;

}

#endif	// #ifdef __MG2470_PWM1_INCLUDE__








#ifdef __MG2470_PWM2_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for PWM2.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Pwm2ClockOn(UINT8 u8On)		// Default Off
{
	if(u8On)
	{
		PWM2_CLK_ON;
	}
	else
	{
		PWM2_CLK_OFF;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PWM2 as PWM mode. GPIO0_2 is output of PWM signal.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u16Period	: Period of one PWM signal. This should be equal to or more than "u16HighDuty".
///							\n The period = "u16Period" x 1 clock duty(Refer to @note).
///	@param	u16HighDuty	: Specifies duty time of each level. This should not be more than "u16Period".
///							\n High level duty time = "u16HighDuty" x 1 clock duty(Refer to @note).
///							\n Low level duty time = ("u16Period" - "u16HighDuty") x 1 clock duty(Refer to @note).
///	@param	u8Option		: reserved.
///	@return	UINT8. Status.
///	@note	1 clock duty is 125ns at 8MHz MCU frequency. It is dependent on MCU frequency. 
///			And, The MCU frequency is determined by HAL_McuPeriClockSet().
///				\n 1 clock duty = 125ns * (8MHz / MCU frequency)
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Pwm2Set_PwmMode(UINT8 u8Ena, UINT16 u16Period, UINT16 u16HighDuty, UINT8 u8Option)
{
	if(PWM2_CLK_STATE == 0)
	{
		assert_error(PWM_CODE | RETURN_PWM2_CLOCK_OFF);
		return RETURN_PWM2_CLOCK_OFF;
	}

	if(u16HighDuty > u16Period)
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	xPWM2_CTRL = BIT5;
	if(u8Ena)
	{
		xPWM2_HRCL = u16Period;
		xPWM2_HRCH = u16Period >> 8;
		xPWM2_LRCL = u16HighDuty;
		xPWM2_LRCH = u16HighDuty >> 8;
		
		xPWM2_CTRL = BIT3 | BIT0;

		xPWM2_HRCL = u16HighDuty;
		xPWM2_HRCH = u16HighDuty >> 8;
		xPWM2_LRCL = u16Period;
		xPWM2_LRCH = u16Period >> 8;
	}

	u8Option = 0;

	return RETURN_PWM_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PWM2 as timer mode. When counter is overflowed, PWM interrupt is generated and PWM_ISR() is called.
///	\n	When this is called with "u8IntEna=1", EIE[3] is also set to enable PWM Interrupt. 
///		In other words, Calling HAL_PwmIntModeSet() is not needed.
///	\n	But if this is called with "u8IneEna=0", EIE[3] is not cleared. Because, another PWM ports are still used
///		for PWM Interrupt.
///
///	@param	u8Ena			: 0=Disable, 1=Enable.
///	@param	u8Priority			: Priority of Interrupt. 0=Low, 1=High.
///	@param	u16Tick_125ns 	: Number of ticks for the interrupt in 1 clock duty(Refer to [note]). 800 ~ 65535.
///								\n Interrupt period = "u16Tick_125ns" x 1 clock duty(Refer to [note]). 
///	@param	u8Option			: reserved.
///	@return	UINT8. Status.
///	@note	1 clock duty is 125ns at 8MHz MCU frequency. It is dependent on MCU frequency. 
///			And, The MCU frequency is determined by HAL_McuPeriClockSet().
///				\n 1 clock duty = 125ns * (8MHz / MCU frequency)
///	@note	To control EIE[3] independently, use HAL_PwmIntModeSet().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Pwm2Set_TimerMode(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_125ns, UINT8 u8Option)
{
	if(PWM2_CLK_STATE == 0)
	{
		assert_error(PWM_CODE | RETURN_PWM2_CLOCK_OFF);
		return RETURN_PWM2_CLOCK_OFF;
	}

	if(u16Tick_125ns < 800)
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	xPWM2_CTRL = BIT5;
	if(u8IntEna)
	{
		xPWM2_HRCL = u16Tick_125ns;
		xPWM2_HRCH = u16Tick_125ns >> 8;
		xPWM2_LRCL = xPWM2_HRCL;
		xPWM2_LRCH = xPWM2_HRCH;
		xPWM2_CTRL = BIT7 | BIT0;
		HAL_PwmIntModeSet(1, u8Priority);
	}

	u8Option = 0;

	return RETURN_PWM_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
/// 	Sets PWM2 as PWM mode. GPIO0_2 is output of PWM signal.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u32Frequency	: Frequency of PWM signal(Degree : Hz).
///							\n The frequency = "u32Frequency"Hz (Min : 123Hz ~ Max : 80,000Hz).
///	@param	u8HighDuty	: Percentage of "u32Frequency". (Min : 1% ~ Max : 99%)
///	@param	u8Option		: reserved.
///	@return	UINT8. Status.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Pwm2Set_PwmMode(UINT8 u8Ena, UINT32 u32Frequency, UINT8 u8HighDuty, UINT8 u8Option)
{
	UINT16	u16Period;
	UINT16	u16HighDuty;
	UINT8	u8Status;

	if( (u32Frequency > 80000) || (u32Frequency < 123) )
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	if( (u8HighDuty > 99) || (u8HighDuty < 1) )
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	u16Period = 8000000/u32Frequency;	// for 8MHz MCU Clock
	u16HighDuty = ((float)u16Period * ((float)u8HighDuty / 100));
	if(u16HighDuty == 0)	u16HighDuty = 1;

	u8Status = HAL_Pwm2Set_PwmMode(u8Ena, u16Period, u16HighDuty, u8Option);

	return u8Status;

}

#endif	// #ifdef __MG2470_PWM2_INCLUDE__








#ifdef __MG2470_PWM3_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for PWM3.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Pwm3ClockOn(UINT8 u8On)		// Default Off
{
	if(u8On)
	{
		PWM3_CLK_ON;
	}
	else
	{
		PWM3_CLK_OFF;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PWM3 as PWM mode. GPIO0_3 is output of PWM signal.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u16Period	: Period of one PWM signal. This should be equal to or more than "u16HighDuty".
///							\n The period = "u16Period" x 1 clock duty(Refer to @note).
///	@param	u16HighDuty	: Specifies duty time of each level. This should not be more than "u16Period".
///							\n High level duty time = "u16HighDuty" x 1 clock duty(Refer to @note).
///							\n Low level duty time = ("u16Period" - "u16HighDuty") x 1 clock duty(Refer to @note).
///	@param	u8Option		: reserved.
///	@return	UINT8. Status.
///	@note	1 clock duty is 125ns at 8MHz MCU frequency. It is dependent on MCU frequency. 
///			And, The MCU frequency is determined by HAL_McuPeriClockSet().
///				\n 1 clock duty = 125ns * (8MHz / MCU frequency)
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Pwm3Set_PwmMode(UINT8 u8Ena, UINT16 u16Period, UINT16 u16HighDuty, UINT8 u8Option)
{
	if(PWM3_CLK_STATE == 0)
	{
		assert_error(PWM_CODE | RETURN_PWM3_CLOCK_OFF);
		return RETURN_PWM3_CLOCK_OFF;
	}

	if(u16HighDuty > u16Period)
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	xPWM3_CTRL = BIT5;
	if(u8Ena)
	{
		xPWM3_HRCL = u16Period;
		xPWM3_HRCH = u16Period >> 8;
		xPWM3_LRCL = u16HighDuty;
		xPWM3_LRCH = u16HighDuty >> 8;
		
		xPWM3_CTRL = BIT3 | BIT0;

		xPWM3_HRCL = u16HighDuty;
		xPWM3_HRCH = u16HighDuty >> 8;
		xPWM3_LRCL = u16Period;
		xPWM3_LRCH = u16Period >> 8;
	}

	u8Option = 0;

	return RETURN_PWM_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PWM3 as timer mode. When counter is overflowed, PWM interrupt is generated and PWM_ISR() is called.
///	\n	When this is called with "u8IntEna=1", EIE[3] is also set to enable PWM Interrupt. 
///		In other words, Calling HAL_PwmIntModeSet() is not needed.
///	\n	But if this is called with "u8IneEna=0", EIE[3] is not cleared. Because, another PWM ports are still used
///		for PWM Interrupt.
///
///	@param	u8Ena			: 0=Disable, 1=Enable.
///	@param	u8Priority			: Priority of Interrupt. 0=Low, 1=High.
///	@param	u16Tick_125ns 	: Number of ticks for the interrupt in 1 clock duty(Refer to [note]). 800 ~ 65535.
///								\n Interrupt period = "u16Tick_125ns" x 1 clock duty(Refer to [note]). 
///	@param	u8Option			: reserved.
///	@return	UINT8. Status.
///	@note	1 clock duty is 125ns at 8MHz MCU frequency. It is dependent on MCU frequency. 
///			And, The MCU frequency is determined by HAL_McuPeriClockSet().
///				\n 1 clock duty = 125ns * (8MHz / MCU frequency)
///	@note	To control EIE[3] independently, use HAL_PwmIntModeSet().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Pwm3Set_TimerMode(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_125ns, UINT8 u8Option)
{
	if(PWM3_CLK_STATE == 0)
	{
		assert_error(PWM_CODE | RETURN_PWM3_CLOCK_OFF);
		return RETURN_PWM3_CLOCK_OFF;
	}

	if(u16Tick_125ns < 800)
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	xPWM3_CTRL = BIT5;
	if(u8IntEna)
	{
		xPWM3_HRCL = u16Tick_125ns;
		xPWM3_HRCH = u16Tick_125ns >> 8;
		xPWM3_LRCL = xPWM3_HRCL;
		xPWM3_LRCH = xPWM3_HRCH;
		xPWM3_CTRL = BIT7 | BIT0;
		HAL_PwmIntModeSet(1, u8Priority);
	}

	u8Option = 0;

	return RETURN_PWM_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
/// 	Sets PWM3 as PWM mode. GPIO0_4 is output of PWM signal.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u32Frequency	: Frequency of PWM signal(Degree : Hz).
///							\n The frequency = "u32Frequency"Hz (Min : 123Hz ~ Max : 80,000Hz).
///	@param	u8HighDuty	: Percentage of "u32Frequency". (Min : 1% ~ Max : 99%)
///	@param	u8Option		: reserved.
///	@return	UINT8. Status.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Pwm3Set_PwmMode(UINT8 u8Ena, UINT32 u32Frequency, UINT8 u8HighDuty, UINT8 u8Option)
{
	UINT16	u16Period;
	UINT16	u16HighDuty;
	UINT8	u8Status;

	if( (u32Frequency > 80000) || (u32Frequency < 123) )
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	if( (u8HighDuty > 99) || (u8HighDuty < 1) )
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	u16Period = 8000000/u32Frequency;	// for 8MHz MCU Clock
	u16HighDuty = ((float)u16Period * ((float)u8HighDuty / 100));
	if(u16HighDuty == 0)	u16HighDuty = 1;

	u8Status = HAL_Pwm3Set_PwmMode(u8Ena, u16Period, u16HighDuty, u8Option);

	return u8Status;

}

#endif	// #ifdef __MG2470_PWM3_INCLUDE__









#ifdef __MG2470_PWM4_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for PWM4.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Pwm4ClockOn(UINT8 u8On)		// Default Off
{
	if(u8On)
	{
		PWM4_CLK_ON;
	}
	else
	{
		PWM4_CLK_OFF;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PWM4 as PWM mode. GPIO0_4 is output of PWM signal.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u16Period	: Period of one PWM signal. This should be equal to or more than "u16HighDuty".
///							\n The period = "u16Period" x 1 clock duty(Refer to @note).
///	@param	u16HighDuty	: Specifies duty time of each level. This should not be more than "u16Period".
///							\n High level duty time = "u16HighDuty" x 1 clock duty(Refer to @note).
///							\n Low level duty time = ("u16Period" - "u16HighDuty") x 1 clock duty(Refer to @note).
///	@param	u8Option		: reserved.
///	@return	UINT8. Status.
///	@note	1 clock duty is 125ns at 8MHz MCU frequency. It is dependent on MCU frequency. 
///			And, The MCU frequency is determined by HAL_McuPeriClockSet().
///				\n 1 clock duty = 125ns * (8MHz / MCU frequency)
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Pwm4Set_PwmMode(UINT8 u8Ena, UINT16 u16Period, UINT16 u16HighDuty, UINT8 u8Option)
{
	if(PWM4_CLK_STATE == 0)
	{
		assert_error(PWM_CODE | RETURN_PWM4_CLOCK_OFF);
		return RETURN_PWM4_CLOCK_OFF;
	}

	if(u16HighDuty > u16Period)
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	xPWM4_CTRL = BIT5;
	if(u8Ena)
	{
		xPWM4_HRCL = u16Period;
		xPWM4_HRCH = u16Period >> 8;
		xPWM4_LRCL = u16HighDuty;
		xPWM4_LRCH = u16HighDuty >> 8;
		
		xPWM4_CTRL = BIT3 | BIT0;

		xPWM4_HRCL = u16HighDuty;
		xPWM4_HRCH = u16HighDuty >> 8;
		xPWM4_LRCL = u16Period;
		xPWM4_LRCH = u16Period >> 8;
	}

	u8Option = 0;

	return RETURN_PWM_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PWM4 as timer mode. When counter is overflowed, PWM interrupt is generated and PWM_ISR() is called.
///	\n	When this is called with "u8IntEna=1", EIE[3] is also set to enable PWM Interrupt. 
///		In other words, Calling HAL_PwmIntModeSet() is not needed.
///	\n	But if this is called with "u8IneEna=0", EIE[3] is not cleared. Because, another PWM ports are still used
///		for PWM Interrupt.
///
///	@param	u8Ena			: 0=Disable, 1=Enable.
///	@param	u8Priority			: Priority of Interrupt. 0=Low, 1=High.
///	@param	u16Tick_125ns 	: Number of ticks for the interrupt in 1 clock duty(Refer to [note]). 800 ~ 65535.
///								\n Interrupt period = "u16Tick_125ns" x 1 clock duty(Refer to [note]). 
///	@param	u8Option			: reserved.
///	@return	UINT8. Status.
///	@note	1 clock duty is 125ns at 8MHz MCU frequency. It is dependent on MCU frequency. 
///			And, The MCU frequency is determined by HAL_McuPeriClockSet().
///				\n 1 clock duty = 125ns * (8MHz / MCU frequency)
///	@note	To control EIE[3] independently, use HAL_PwmIntModeSet().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Pwm4Set_TimerMode(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_125ns, UINT8 u8Option)
{
	if(PWM4_CLK_STATE == 0)
	{
		assert_error(PWM_CODE | RETURN_PWM4_CLOCK_OFF);
		return RETURN_PWM4_CLOCK_OFF;
	}

	if(u16Tick_125ns < 800)
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	xPWM4_CTRL = BIT5;
	if(u8IntEna)
	{
		xPWM4_HRCL = u16Tick_125ns;
		xPWM4_HRCH = u16Tick_125ns >> 8;
		xPWM4_LRCL = xPWM4_HRCL;
		xPWM4_LRCH = xPWM4_HRCH;
		xPWM4_CTRL = BIT7 | BIT0;
		HAL_PwmIntModeSet(1, u8Priority);
	}

	u8Option = 0;

	return RETURN_PWM_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
/// 	Sets PWM4 as PWM mode. GPIO0_4 is output of PWM signal.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u32Frequency	: Frequency of PWM signal(Degree : Hz).
///							\n The frequency = "u32Frequency"Hz (Min : 123Hz ~ Max : 80,000Hz).
///	@param	u8HighDuty	: Percentage of "u32Frequency". (Min : 1% ~ Max : 99%)
///	@param	u8Option			: reserved.
///	@return	UINT8. Status.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Pwm4Set_PwmMode(UINT8 u8Ena, UINT32 u32Frequency, UINT8 u8HighDuty, UINT8 u8Option)
{
	UINT16	u16Period;
	UINT16	u16HighDuty;
	UINT8	u8Status;

	if( (u32Frequency > 80000) || (u32Frequency < 123) )
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	if( (u8HighDuty > 99) || (u8HighDuty < 1) )
	{
		assert_error(PWM_CODE | RETURN_PWM_INVALID_PARAMETER);
		return RETURN_PWM_INVALID_PARAMETER;
	}

	u16Period = 8000000/u32Frequency;	// for 8MHz MCU Clock
	u16HighDuty = ((float)u16Period * ((float)u8HighDuty / 100));
	if(u16HighDuty == 0)	u16HighDuty = 1;

	u8Status = HAL_Pwm4Set_PwmMode(u8Ena, u16Period, u16HighDuty, u8Option);

	return u8Status;

}

#endif	// #ifdef __MG2470_PWM4_INCLUDE__



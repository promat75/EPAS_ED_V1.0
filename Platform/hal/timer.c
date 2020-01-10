
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "timer.h"

#ifdef __MG2470_TIMER0_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable for reload value of TIMER0's TL
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8Timer0ReloadTL0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable for reload value of TIMER0's TH
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8Timer0ReloadTH0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for TIMER0. Default is Off.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Timer0ClockOn(UINT8 u8On)		// Default On
{
	if(u8On)
	{
		TIMER0_CLK_ON;
	}
	else
	{
		TIMER0_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets TIMER0 interrupt.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u8Run		: 0=Not run, 1=Run.
///	@param	u8Tick_ms	: Number of ticks for the interrupt in ms. 1 ~ 98.
///							\n Interrupt period = "u8Tick_ms" x 1ms.
///	@return	UINT8. Status.
///	@note	"u8Tick_ms" indicates 1ms when MCU frequency is 8MHz. The period for another frequency is,
///				\n Interrupt period = "u8Tick_ms" x 1ms x (8MHz / MCU frequency).
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Timer0IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT8 u8Tick_ms)
{
	UINT32	u32Reload32;
	UINT8	u8EA;

	if(TIMER0_CLK_STATE == 0)
	{
		assert_error(TIMER_CODE | RETURN_TIMER0_CLOCK_OFF);
		return RETURN_TIMER0_CLOCK_OFF;
	}
	if(u8Tick_ms > 98)
	{
		assert_error(TIMER_CODE | RETURN_TIMER0_INVALID_PARAMETER);
		return RETURN_TIMER0_INVALID_PARAMETER;
	}

	u8EA = EA;
	EA = 0;

	u32Reload32 = 2000;
	u32Reload32 = u32Reload32 * u8Tick_ms;
	u32Reload32 = (u32Reload32 / 3) & 0x0000FFFF;
	u32Reload32 = 0x0000FFFF - u32Reload32 + 1;

	gu8Timer0ReloadTL0 = u32Reload32;
	gu8Timer0ReloadTH0 = u32Reload32 >> 8;

	TL0 = gu8Timer0ReloadTL0;
	TH0 = gu8Timer0ReloadTH0;

	TMOD &= 0xF0;
	TMOD |= 0x01;
	
	PT0 = u8Priority;
	TR0 = u8Run;
	ET0 = u8IntEna;

	EA = u8EA;

	return RETURN_TIMER_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets TIMER0 interrupt for short period.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u8Run		: 0=Not run, 1=Run.
///	@param	u16Tick_1500ns : Number of ticks for the interrupt in 1500ns.
///							\n Interrupt period = "u16Tick_1500ns" x 1500ns.
///	@return	UINT8. Status.
///	@note	If too short period is set, The chip seems not to work because almost operation time is 
///			    used for processing TIMER0_ISR().
///	@note	"u16Tick_1500ns" indicates 1.5us when MCU frequency is 8MHz. The period for another frequencies is,
///				\n Interrupt period = "u16Tick_1500ns" x 1.5us x (8MHz / MCU frequency).
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Timer0IntSet_ShortPeriod(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT16 u16Tick_1500ns)
{
	UINT8	u8EA;
	UINT16	u16Reload;

	if(TIMER0_CLK_STATE == 0)
	{
		assert_error(TIMER_CODE | RETURN_TIMER0_CLOCK_OFF);
		return RETURN_TIMER0_CLOCK_OFF;
	}

	u8EA = EA;
	EA = 0;

	u16Reload = 0xFFFF - u16Tick_1500ns + 1;

	gu8Timer0ReloadTL0 = u16Reload;
	gu8Timer0ReloadTH0 = u16Reload >> 8;

	TL0 = gu8Timer0ReloadTL0;
	TH0 = gu8Timer0ReloadTH0;

	TMOD &= 0xF0;
	TMOD |= 0x01;
	
	PT0 = u8Priority;
	TR0 = u8Run;
	ET0 = u8IntEna;

	EA = u8EA;

	return RETURN_TIMER_SUCCESS;
}

#endif // #ifdef __MG2470_TIMER0_INCLUDE__




#ifdef __MG2470_TIMER1_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable for reload value of TIMER1's TL
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8Timer1ReloadTL1;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable for reload value of TIMER1's TH
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8Timer1ReloadTH1;	

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for TIMER1. Default is Off.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Timer1ClockOn(UINT8 u8On)
{
	if(u8On)
	{
		TIMER1_CLK_ON;
	}
	else
	{
		TIMER1_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets TIMER1 interrupt.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u8Run		: 0=Not run, 1=Run.
///	@param	u8Tick_ms	: Number of ticks for the interrupt in ms. 1 ~ 98.
///							\n Interrupt period = "u8Tick_ms" x 1ms.
///	@return	UINT8. Status.
///	@note	"u8Tick_ms" indicates 1ms when MCU frequency is 8MHz. The period for another frequency is,
///				\n Interrupt period = "u8Tick_ms" x 1ms x (8MHz / MCU frequency).
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Timer1IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT8 u8Tick_ms)
{
	UINT32	u32Reload32;
	UINT8	u8EA;

	if(TIMER1_CLK_STATE == 0)
	{
		assert_error(TIMER_CODE | RETURN_TIMER1_CLOCK_OFF);
		return RETURN_TIMER1_CLOCK_OFF;
	}

	if(u8Tick_ms > 98)
	{
		assert_error(TIMER_CODE | RETURN_TIMER1_INVALID_PARAMETER);
		return RETURN_TIMER1_INVALID_PARAMETER;
	}

	u8EA = EA;
	EA = 0;	
	
	u32Reload32 = 2000;
	u32Reload32 = u32Reload32 * u8Tick_ms;		
	u32Reload32 = (u32Reload32 / 3) & 0x0000FFFF;
	u32Reload32 = 0x0000FFFF - u32Reload32 + 1;

	gu8Timer1ReloadTL1 = u32Reload32;
	gu8Timer1ReloadTH1 = u32Reload32 >> 8;

	TL1 = gu8Timer1ReloadTL1;
	TH1 = gu8Timer1ReloadTH1;

	TMOD &= 0x0F;		// clear bit[7:4]
	TMOD |= 0x10;		// Mode=1(16 bit timer)	

	PT1 = u8Priority;
	TR1 = u8Run;
	ET1 = u8IntEna;

	EA = u8EA;

	return RETURN_TIMER_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets TIMER1 interrupt for short period.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u8Run		: 0=Not run, 1=Run.
///	@param	u16Tick_1500ns	: Number of ticks for the interrupt in 1500ns.
///							\n Interrupt period = "u16Tick_1500ns" x 1500ns.
///	@return	UINT8. Status.
///	@note	If too short period is set, The chip seems not to work because almost operation time is 
///			    used for processing TIMER1_ISR().
///	@note	"u16Tick_1500ns" indicates 1.5us when MCU frequency is 8MHz. The period for another frequencies is,
///				\n Interrupt period = "u16Tick_1500ns" x 1.5us x (8MHz / MCU frequency).
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Timer1IntSet_ShortPeriod(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT16 u16Tick_1500ns)
{
	UINT8	u8EA;
	UINT16	u16Reload;

	if(TIMER1_CLK_STATE == 0)
	{
		assert_error(TIMER_CODE | RETURN_TIMER1_CLOCK_OFF);
		return RETURN_TIMER1_CLOCK_OFF;
	}

	u8EA = EA;
	EA = 0;	

	u16Reload = 0xFFFF - u16Tick_1500ns + 1;

	gu8Timer1ReloadTL1 = u16Reload;
	gu8Timer1ReloadTH1 = u16Reload >> 8;

	TL1 = gu8Timer1ReloadTL1;
	TH1 = gu8Timer1ReloadTH1;
	
	TMOD &= 0x0F;		// clear bit[7:4]
	TMOD |= 0x10;		// Mode=1(16 bit timer)	

	PT1 = u8Priority;
	TR1 = u8Run;
	ET1 = u8IntEna;

	EA = u8EA;

	return RETURN_TIMER_SUCCESS;
}

#endif // #ifdef __MG2470_TIMER1_INCLUDE__








#ifdef __MG2470_TIMER2_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for TIMER2. Default is Off.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Timer2ClockOn(UINT8 u8On)
{
	if(u8On)
	{
		TIMER2_CLK_ON;
	}
	else
	{
		TIMER2_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets TIMER2 interrupt.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u8Run		: 0=Not run, 1=Run.
///	@param	u32Tick_us	: Number of ticks for the interrupt in us. 5us ~ 524.272ms.
///							\n Interrupt period = "u32Tick_us" x 1us.
///	@return	UINT8. Status.
///	@note	"u32Tick_us" indicates 1us when MCU frequency is 8MHz. The period for another frequency is,
///				\n Interrupt period = "u32Tick_us" x 1us x (8MHz / MCU frequency).
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Timer2IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT32 u32Tick_us)
{
	UINT8	u8EA;
	UINT16	u16Counter;
	UINT8	u8Division;
	UINT8	u8Divider;
	UINT8	u8PeriodHigh;
	UINT8	u8PeriodLow;

	if(TIMER2_CLK_STATE == 0)
	{
		assert_error(TIMER_CODE | RETURN_TIMER2_CLOCK_OFF);
		return RETURN_TIMER2_CLOCK_OFF;
	}

	if( (u32Tick_us < 5) && (u32Tick_us > 524272) )
	{
		assert_error(TIMER_CODE | RETURN_TIMER2_INVALID_PARAMETER);
		return RETURN_TIMER2_INVALID_PARAMETER;
	}

	if(u32Tick_us > 262136)		//	Division 64
	{
		u8Divider = 64;
		u8Division = TIMER2_DIVISION_64;
	}
	else if(u32Tick_us > 131068)	//	Division 32
	{
		u8Divider = 32;
		u8Division = TIMER2_DIVISION_32;
	}
	else if(u32Tick_us > 65534)	//	Division 16
	{
		u8Divider = 16;
		u8Division = TIMER2_DIVISION_16;
	}
	else							//	Division 8(default)
	{
		u8Divider = 8;
		u8Division = TIMER2_DIVISION_8;
	}

	u16Counter = ((float)( (u32Tick_us * 8) / u8Divider ) + 0.5) - 1;

	u8PeriodHigh = (u16Counter / 256);
	u8PeriodLow = (float)((float)(u16Counter % 256) + 0.5);

	u8EA = EA;
	EA = 0;	

	T23CON &= ~(BIT3 | BIT2 | BIT1 | BIT0);		// bit[3:0]=4'b0000
	if(u8IntEna)
	{
		TH2 = u8PeriodHigh;
		TL2 = u8PeriodLow;
		T2IP = u8Priority;
		if(u8Run)	T23CON |= (u8Division << 1) | BIT0;		// Run & Division
		else			T23CON &= ~BIT0;					// Not run
		T2IE = 1;
	}
	else
	{
		T2IE = 0;
	}

	EA = u8EA;

	return RETURN_TIMER_SUCCESS;
}
#endif		// #ifdef __MG2470_TIMER2_INCLUDE__






#ifdef __MG2470_TIMER3_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for TIMER3. Default is Off.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Timer3ClockOn(UINT8 u8On)
{
	if(u8On)
	{
		TIMER3_CLK_ON;
	}
	else
	{
		TIMER3_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets TIMER3 interrupt.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u8Run		: 0=Not run, 1=Run.
///	@param	u32Tick_us	: Number of ticks for the interrupt in us. 5us ~ 524.272ms.
///							\n Interrupt period = "u32Tick_us" x 1us.
///	@return	UINT8. Status.
///	@note	"u32Tick_us" indicates 1us when MCU frequency is 8MHz. The period for another frequency is,
///				\n Interrupt period = "u32Tick_us" x 1us x (8MHz / MCU frequency).
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Timer3IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT32 u32Tick_us)
{
	UINT8	u8EA;
	UINT16	u16Counter;
	UINT8	u8Division;
	UINT8	u8Divider;
	UINT8	u8PeriodHigh;
	UINT8	u8PeriodLow;

	if(TIMER3_CLK_STATE == 0)
	{
		assert_error(TIMER_CODE | RETURN_TIMER3_CLOCK_OFF);
		return RETURN_TIMER3_CLOCK_OFF;
	}

	if( (u32Tick_us < 5) && (u32Tick_us > 524272) )
	{
		assert_error(TIMER_CODE | RETURN_TIMER3_INVALID_PARAMETER);
		return RETURN_TIMER3_INVALID_PARAMETER;
	}

	if(u32Tick_us > 262136)		//	Division 64
	{
		u8Divider = 64;
		u8Division = TIMER3_DIVISION_64;
	}
	else if(u32Tick_us > 131068)	//	Division 32
	{
		u8Divider = 32;
		u8Division = TIMER3_DIVISION_32;
	}
	else if(u32Tick_us > 65534)	//	Division 16
	{
		u8Divider = 16;
		u8Division = TIMER3_DIVISION_16;
	}
	else							//	Division 8(default)
	{
		u8Divider = 8;
		u8Division = TIMER3_DIVISION_8;
	}

	u16Counter = ((float)( (u32Tick_us * 8) / u8Divider ) +0.5) - 1;

	u8PeriodHigh = (u16Counter / 256);
	u8PeriodLow = (float)((float)(u16Counter % 256) + 0.5);

	u8EA = EA;
	EA = 0;

	T23CON	&= ~(BIT7 | BIT6 | BIT5 | BIT4);	//bit[7:4]=4'b0000.
	if(u8IntEna)
	{
		TH3 = u8PeriodHigh;
		TL3 = u8PeriodLow;
		T3IP = u8Priority;
		if(u8Run)	T23CON |= (u8Division << 5) | BIT4;		// Run & Division
		else			T23CON &= ~BIT4;					// Not run
		T3IE = 1;
	}
	else
	{
		T3IE = 0;
	}

	EA = u8EA;

	return RETURN_TIMER_SUCCESS;
}
#endif		// #ifdef __MG2470_TIMER3_INCLUDE__


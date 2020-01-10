
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "wdt.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates the number of TIMER0's ticks before watchdog reset.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16WdtResetTick;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for Watchdog Timer.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_WdtClockOn(UINT8 u8On)		// Default On
{
	if(u8On)
	{
		WDT_CLK_ON;
	}
	else
	{
		WDT_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// Sets watchdog timer.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u8ResetDur	: Timeout duration. 0 ~ 15.
///						\n	0 = 30 us
///						\n	1 = 61 us
///						\n	2 = 122 us
///						\n	3 = 244 us
///						\n	4 = 488 us
///						\n	5 = 976 us
///						\n	6 = 1.953 ms
///						\n	7 = 3.906 ms
///						\n	8 = 7.812 ms
///						\n	9 = 15.625 ms
///						\n	10 = 31.25 ms
///						\n	11 = 62.5 ms
///						\n	12 = 125 ms
///						\n	13 = 250 ms
///						\n	14 = 500 ms
///						\n	15 = 1 sec
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_WdtSet(UINT8 u8Ena, UINT8 u8ResetDur)
{
	UINT8	u8EA;

	if(WDT_CLK_STATE == 0)
	{
		assert_error(WDT_CODE | RETURN_WDT_CLOCK_OFF);
		return RETURN_WDT_CLOCK_OFF;
	}

	if(u8ResetDur > 15)
	{
		assert_error(WDT_CODE | RETURN_WDT_INVALID_PARAMETER);
		return RETURN_WDT_INVALID_PARAMETER;
	}

	u8EA = EA;
	EA = 0;

	if(u8Ena)
	{	
		WDT = 0x55;
		WDT = 0xAA;	
		WDT = 0x40 | (u8ResetDur & 0x0F);
	}
	else
	{
		WDT = 0x55;
		WDT = 0xAA;	
		WDT = 0x80;
	}

	EA = u8EA;

	return RETURN_WDT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// Restarts watchdog timer to prevent watchdog timeout.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_WdtRestart(void) large reentrant
{
	UINT8	u8EA;

	u8EA = EA;
	EA = 0;

	WDT = 0x55;
	WDT = 0xAA;
	WDT |= 0x40;	

	EA = u8EA;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// Sets watchdog period. This function should be called periodically before watchdog reset.
///
///	@param	u16Timer0Tick	: Number of TIMER0's ticks before watchdog reset.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////	
void SYS_WdtSet(UINT16 u16Timer0Tick)
{
	UINT8	u8EA;

	u8EA = EA;
	EA = 0;

	if(u16Timer0Tick)	// WDT enable
	{
		gu16WdtResetTick = u16Timer0Tick;
		HAL_WdtSet(1, 0x0F);
	} 
	else					// WDT disable
	{
		gu16WdtResetTick = 0;
		HAL_WdtSet(0, 0);
	}

	EA = u8EA;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// Resets by disabling watchdog restart.
///
///	@param	u8ResetDur	: Duration before reset. 0 ~ 15.
///						\n	0 = 30 us
///						\n	1 = 61 us
///						\n	2 = 122 us
///						\n	3 = 244 us
///						\n	4 = 488 us
///						\n	5 = 976 us
///						\n	6 = 1.953 ms
///						\n	7 = 3.906 ms
///						\n	8 = 7.812 ms
///						\n	9 = 15.625 ms
///						\n	10 = 31.25 ms
///						\n	11 = 62.5 ms
///						\n	12 = 125 ms
///						\n	13 = 250 ms
///						\n	14 = 500 ms
///						\n	15 = 1 sec
///	@return	UINT8. Status when failed.
///	
///////////////////////////////////////////////////////////////////////////////////////////////	
UINT8 SYS_WdtReset(UINT8 u8ResetDur)
{
	if(u8ResetDur > 15)
	{
		assert_error(WDT_CODE | RETURN_WDT_INVALID_PARAMETER);
		return RETURN_WDT_INVALID_PARAMETER;
	}

	EA = 0;
	WDT = 0x55;
	WDT = 0xAA;
	WDT = u8ResetDur & 0x0F;
	WDT_CLK_ON;
	while(1);		// Watchdog Reset
}

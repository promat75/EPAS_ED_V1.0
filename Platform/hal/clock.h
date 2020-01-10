
/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_CLOCK_H__
#define __MG2470_CLOCK_H__

#define	CLOCK_MODE_16_MHz	0
#define	CLOCK_MODE_8_MHz		1
#define	CLOCK_MODE_4_MHz		2
#define	CLOCK_MODE_2_MHz		3
#define	CLOCK_MODE_1_MHz		4
#define	CLOCK_MODE_500_KHz	5

#define	CLOCK_CODE	0x0200

#define	RETURN_CLOCK_SUCCESS				0x00
#define	RETURN_CLOCK_INVALID_PARAMETER	0x11
#define	RETURN_CLOCK_INVALID_DIVISOR		0x12

extern	UINT8	gu8McuPeriClockMode;

UINT8 HAL_McuPeriClockSet(UINT8 u8ClockMode);
void HAL_PeriClock0Set(UINT8 u8PeriClk0);
void HAL_PeriClock1Set(UINT8 u8PeriClk1);
void HAL_PeriClock2Set(UINT8 u8PeriClk2);

#endif	// #ifndef __MG2470_CLOCK_H__




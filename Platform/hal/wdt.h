
/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_WDT_H__
#define __MG2470_WDT_H__

#define 	WDT_CLK_ON		(PERI_CLK_STP3 |= BIT0)
#define	WDT_CLK_OFF		(PERI_CLK_STP3 &= ~BIT0)
#define	WDT_CLK_STATE		(PERI_CLK_STP3 & BIT0)

#define	WDT_CODE	0x0E00

#define	RETURN_WDT_SUCCESS				0x00
#define	RETURN_WDT_CLOCK_OFF			0x10
#define	RETURN_WDT_INVALID_PARAMETER	0x11

extern	UINT16	gu16WdtResetTick;

void HAL_WdtClockOn(UINT8 u8On);
UINT8 HAL_WdtSet(UINT8 u8Ena, UINT8 u8ResetDur);
void HAL_WdtRestart(void) large reentrant;
void SYS_WdtSet(UINT16 u16Timer0Tick);
UINT8 SYS_WdtReset(UINT8 u8ResetDur);

#endif	// #ifndef __MG2470_WDT_H__



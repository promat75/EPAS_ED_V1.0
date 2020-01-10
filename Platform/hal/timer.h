
/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_TIMER_H__
#define __MG2470_TIMER_H__
#define __MG2470_TIMER0_INCLUDE__
#define __MG2470_TIMER1_INCLUDE__
#define __MG2470_TIMER2_INCLUDE__
#define __MG2470_TIMER3_INCLUDE__

#define	TIMER_CODE	0x0C00

#define	RETURN_TIMER_SUCCESS					0x00
#define	RETURN_TIMER0_CLOCK_OFF				0x13
#define	RETURN_TIMER1_CLOCK_OFF				0x14
#define	RETURN_TIMER2_CLOCK_OFF				0x15
#define	RETURN_TIMER3_CLOCK_OFF				0x16
#define	RETURN_TIMER0_INVALID_PARAMETER	0x17
#define	RETURN_TIMER1_INVALID_PARAMETER	0x18
#define	RETURN_TIMER2_INVALID_PARAMETER	0x19
#define	RETURN_TIMER3_INVALID_PARAMETER	0x1A

#ifdef __MG2470_TIMER0_INCLUDE__
	#define	TIMER0_CLK_ON			(PERI_CLK_STP0 |= BIT0)
	#define	TIMER0_CLK_OFF		(PERI_CLK_STP0 &= ~BIT0)
	#define	TIMER0_CLK_STATE		(PERI_CLK_STP0 & BIT0)

	extern	UINT8	gu8Timer0ReloadTL0;
	extern	UINT8	gu8Timer0ReloadTH0;

	void HAL_Timer0ClockOn(UINT8 u8On);
	UINT8 HAL_Timer0IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT8 u8Tick_ms);
	UINT8 HAL_Timer0IntSet_ShortPeriod(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT16 u16Tick_1500ns);
#endif	// #ifdef __MG2470_TIMER0_INCLUDE__

#ifdef __MG2470_TIMER1_INCLUDE__
	#define	TIMER1_CLK_ON			(PERI_CLK_STP0 |= BIT1)
	#define	TIMER1_CLK_OFF		(PERI_CLK_STP0 &= ~BIT1)
	#define	TIMER1_CLK_STATE		(PERI_CLK_STP0 & BIT1)

	extern	UINT8	gu8Timer1ReloadTL1;
	extern	UINT8	gu8Timer1ReloadTH1;

	void HAL_Timer1ClockOn(UINT8 u8On);
	UINT8 HAL_Timer1IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT8 u8Tick_ms);
	UINT8 HAL_Timer1IntSet_ShortPeriod(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT16 u16Tick_1500ns);
#endif	// #ifdef __MG2470_TIMER1_INCLUDE__

#ifdef __MG2470_TIMER2_INCLUDE__
	#define	TIMER2_CLK_ON			(PERI_CLK_STP0 |= BIT2)
	#define	TIMER2_CLK_OFF		(PERI_CLK_STP0 &= ~BIT2)
	#define	TIMER2_CLK_STATE		(PERI_CLK_STP0 & BIT2)

	#define	TIMER2_DIVISION_1		0
	#define	TIMER2_DIVISION_2		1
	#define	TIMER2_DIVISION_4		2
	#define	TIMER2_DIVISION_8		3
	#define	TIMER2_DIVISION_16	4
	#define	TIMER2_DIVISION_32	5
	#define	TIMER2_DIVISION_64	6

	void HAL_Timer2ClockOn(UINT8 u8On);
	UINT8 HAL_Timer2IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT32 u32Tick_us);
#endif	// #ifdef __MG2470_TIMER2_INCLUDE__

#ifdef __MG2470_TIMER3_INCLUDE__
	#define	TIMER3_CLK_ON			(PERI_CLK_STP0 |= BIT3)
	#define	TIMER3_CLK_OFF		(PERI_CLK_STP0 &= ~BIT3)
	#define	TIMER3_CLK_STATE		(PERI_CLK_STP0 & BIT3)	

	#define	TIMER3_DIVISION_1		0
	#define	TIMER3_DIVISION_2		1
	#define	TIMER3_DIVISION_3		2
	#define	TIMER3_DIVISION_4		3
	#define	TIMER3_DIVISION_8		4
	#define	TIMER3_DIVISION_16	5
	#define	TIMER3_DIVISION_32	6
	#define	TIMER3_DIVISION_64	7
	
	void HAL_Timer3ClockOn(UINT8 u8On);
	UINT8 HAL_Timer3IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Run, UINT32 u32Tick_us);
#endif	// #ifdef __MG2470_TIMER3_INCLUDE__




#endif	// #ifndef __MG2470_TIMER_H__



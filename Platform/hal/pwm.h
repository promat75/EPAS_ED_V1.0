
/*******************************************************************************
	[2013-03-19] Compatible with V2.2
*******************************************************************************/


#ifndef __MG2470_PWM_H__
#define __MG2470_PWM_H__
#define __MG2470_PWM0_INCLUDE__
#define __MG2470_PWM1_INCLUDE__
#define __MG2470_PWM2_INCLUDE__
#define __MG2470_PWM3_INCLUDE__
#define __MG2470_PWM4_INCLUDE__

#define	PWM_CODE	0x0900

#define	RETURN_PWM_SUCCESS				0x00
#define	RETURN_PWM_INVALID_PARAMETER	0x11
#define	RETURN_PWM0_CLOCK_OFF			0x12
#define	RETURN_PWM1_CLOCK_OFF			0x13
#define	RETURN_PWM2_CLOCK_OFF			0x14
#define	RETURN_PWM3_CLOCK_OFF			0x15
#define	RETURN_PWM4_CLOCK_OFF			0x16

void HAL_PwmIntModeSet(UINT8 u8IntEna, UINT8 u8Priority);

#ifdef __MG2470_PWM0_INCLUDE__
	#define	PWM0_CLK_ON		(PERI_CLK_STP2 |= BIT3)
	#define	PWM0_CLK_OFF		(PERI_CLK_STP2 &= ~BIT3)
	#define	PWM0_CLK_STATE	(PERI_CLK_STP2 & BIT3)

	void HAL_Pwm0ClockOn(UINT8 u8On);
	UINT8 HAL_Pwm0Set_PwmMode(UINT8 u8Ena, UINT16 u16Period, UINT16 u16HighDuty, UINT8 u8Option);
	UINT8 HAL_Pwm0Set_TimerMode(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_125ns, UINT8 u8Option);
	UINT8 SYS_Pwm0Set_PwmMode(UINT8 u8Ena, UINT32 u32Frequency, UINT8 u8HighDuty, UINT8 u8Option);
#endif	// #ifdef __MG2470_PWM0_INCLUDE__


#ifdef __MG2470_PWM1_INCLUDE__
	#define	PWM1_CLK_ON		(PERI_CLK_STP2 |= BIT4)
	#define	PWM1_CLK_OFF		(PERI_CLK_STP2 &= ~BIT4)
	#define	PWM1_CLK_STATE	(PERI_CLK_STP2 & BIT4)

	void HAL_Pwm1ClockOn(UINT8 u8On);
	UINT8 HAL_Pwm1Set_PwmMode(UINT8 u8Ena, UINT16 u16Period, UINT16 u16HighDuty, UINT8 u8Option);
	UINT8 HAL_Pwm1Set_TimerMode(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_125ns, UINT8 u8Option);
	UINT8 SYS_Pwm1Set_PwmMode(UINT8 u8Ena, UINT32 u32Frequency, UINT8 u8HighDuty, UINT8 u8Option);
#endif	// #ifdef __MG2470_PWM1_INCLUDE__


#ifdef __MG2470_PWM2_INCLUDE__
	#define	PWM2_CLK_ON		(PERI_CLK_STP2 |= BIT5)
	#define	PWM2_CLK_OFF		(PERI_CLK_STP2 &= ~BIT5)
	#define	PWM2_CLK_STATE	(PERI_CLK_STP2 & BIT5)

	void HAL_Pwm2ClockOn(UINT8 u8On);
	UINT8 HAL_Pwm2Set_PwmMode(UINT8 u8Ena, UINT16 u16Period, UINT16 u16HighDuty, UINT8 u8Option);
	UINT8 HAL_Pwm2Set_TimerMode(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_125ns, UINT8 u8Option);
	UINT8 SYS_Pwm2Set_PwmMode(UINT8 u8Ena, UINT32 u32Frequency, UINT8 u8HighDuty, UINT8 u8Option);
#endif	// #ifdef __MG2470_PWM2_INCLUDE__


#ifdef __MG2470_PWM3_INCLUDE__
	#define	PWM3_CLK_ON		(PERI_CLK_STP2 |= BIT6)
	#define	PWM3_CLK_OFF		(PERI_CLK_STP2 &= ~BIT6)
	#define	PWM3_CLK_STATE	(PERI_CLK_STP2 & BIT6)

	void HAL_Pwm3ClockOn(UINT8 u8On);
	UINT8 HAL_Pwm3Set_PwmMode(UINT8 u8Ena, UINT16 u16Period, UINT16 u16HighDuty, UINT8 u8Option);
	UINT8 HAL_Pwm3Set_TimerMode(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_125ns, UINT8 u8Option);
	UINT8 SYS_Pwm3Set_PwmMode(UINT8 u8Ena, UINT32 u32Frequency, UINT8 u8HighDuty, UINT8 u8Option);
#endif	// #ifdef __MG2470_PWM3_INCLUDE__


#ifdef __MG2470_PWM4_INCLUDE__
	#define	PWM4_CLK_ON		(PERI_CLK_STP2 |= BIT7)
	#define	PWM4_CLK_OFF		(PERI_CLK_STP2 &= ~BIT7)
	#define	PWM4_CLK_STATE	(PERI_CLK_STP2 & BIT7)

	void HAL_Pwm4ClockOn(UINT8 u8On);
	UINT8 HAL_Pwm4Set_PwmMode(UINT8 u8Ena, UINT16 u16Period, UINT16 u16HighDuty, UINT8 u8Option);
	UINT8 HAL_Pwm4Set_TimerMode(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_125ns, UINT8 u8Option);
	UINT8 SYS_Pwm4Set_PwmMode(UINT8 u8Ena, UINT32 u32Frequency, UINT8 u8HighDuty, UINT8 u8Option);
#endif	// #ifdef __MG2470_PWM4_INCLUDE__



#endif	// #ifndef __MG2470_PWM_H__



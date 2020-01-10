
/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_GPIO_INT_H__
#define	__MG2470_GPIO_INT_H__
#define	__MG2470_GPIOINT0_INCLUDE__
#define	__MG2470_GPIOINT1_INCLUDE__
#define	__MG2470_GPIOINT3_INCLUDE__

#define	GPIOINT_LOW_LEVEL		0
#define	GPIOINT_FALLING_EDGE	1
#define	GPIOINT_HIGH_LEVEL	2
#define	GPIOINT_RISING_EDGE	3

#define	GPIOINT_CODE	0x0F00

#define	RETURN_GPIOINT_SUCCESS				0x00
#define	RETURN_GPIOINT0_CLOCK_OFF			0x10
#define	RETURN_GPIOINT1_CLOCK_OFF			0x11
#define	RETURN_GPIOINT3_CLOCK_OFF			0x13
#define	RETURN_GPIOINT0_INVALID_TRIG_TYPE	0x20
#define	RETURN_GPIOINT1_INVALID_TRIG_TYPE	0x21
#define	RETURN_GPIOINT3_INVALID_TRIG_TYPE	0x23
#define	RETURN_GPIOINT0_INVALID_PORT		0x30
#define	RETURN_GPIOINT1_INVALID_PORT		0x31
#define	RETURN_GPIOINT3_INVALID_PORT		0x33

void HAL_GpioIntModeSet(UINT8 u8IntEna, UINT8 u8Priority);

#ifdef __MG2470_GPIOINT0_INCLUDE__
	#define	GPIOINT0_CLK_ON		(PERI_CLK_STP3 |= BIT1)
	#define	GPIOINT0_CLK_OFF		(PERI_CLK_STP3 &= ~BIT1)
	#define	GPIOINT0_CLK_STATE	(PERI_CLK_STP3 & BIT1)


	extern	UINT8	gu8GpioInt0Flag;

	void HAL_GpioInt0ClockOn(UINT8 u8On);
	UINT8 HAL_GpioInt0Set(UINT8 u8Port, UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8TrigType);
#endif	// #ifndef __MG2470_GPIOINT0_INCLUDE__





#ifdef __MG2470_GPIOINT1_INCLUDE__
	#define	GPIOINT1_CLK_ON		(PERI_CLK_STP3 |= BIT2)
	#define	GPIOINT1_CLK_OFF		(PERI_CLK_STP3 &= ~BIT2)
	#define	GPIOINT1_CLK_STATE	(PERI_CLK_STP3 & BIT2)

	extern	UINT8	gu8GpioInt1Flag;

	void HAL_GpioInt1ClockOn(UINT8 u8On);
	UINT8 HAL_GpioInt1Set(UINT8 u8Port, UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8TrigType);
#endif	// #ifndef __MG2470_GPIOINT1_INCLUDE__





#ifdef __MG2470_GPIOINT3_INCLUDE__
	#define	GPIOINT3_CLK_ON		(PERI_CLK_STP3 |= BIT3)
	#define	GPIOINT3_CLK_OFF		(PERI_CLK_STP3 &= ~BIT3)
	#define	GPIOINT3_CLK_STATE	(PERI_CLK_STP3 & BIT3)

	extern	UINT8	gu8GpioInt3Flag;

	void HAL_GpioInt3ClockOn(UINT8 u8On);
	UINT8 HAL_GpioInt3Set(UINT8 u8Port, UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8TrigType);	
#endif	// #ifndef __MG2470_GPIOINT3_INCLUDE__







#endif	// #ifndef __MG2470_GPIO_INT_H__


/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_GPIO_H__
#define	__MG2470_GPIO_H__
#define	__MG2470_GPIO0_INCLUDE__
#define	__MG2470_GPIO1_INCLUDE__
#define	__MG2470_GPIO3_INCLUDE__

#define	GPIO_CLK_ON		(PERI_CLK_STP0 |= BIT4)
#define	GPIO_CLK_OFF		(PERI_CLK_STP0 &= ~BIT4)
#define	GPIO_CLK_STATE	(PERI_CLK_STP0 & BIT4)

#define	GPIO_INPUT				0
#define	GPIO_OUTPUT			1

#define	GPIO_DRIVE_4mA		0
#define	GPIO_DRIVE_8mA		1

#define	GPIO_PULL_UP			0
#define	GPIO_PULL_DOWN		1
#define	GPIO_HIGH_IMPEDANCE	2

#define	GPIO_CODE	0x0500

#define	RETURN_GPIO_SUCCESS				0x00
#define	RETURN_GPIO_CLOCK_OFF			0x10
#define	RETURN_GPIO_INVALID_PARAMETER	0x11
#define	RETURN_GPIO_INVALID_PORT			0x12

void HAL_GpioClockOn(UINT8 u8On);
UINT8 HAL_GpioParameterCheck(UINT8 u8Port, UINT8 u8OutEna, UINT8 u8Option);

#ifdef __MG2470_GPIO0_INCLUDE__
	UINT8 HAL_Gpio0InOutSet(UINT8 u8Port, UINT8 u8OutEna, UINT8 u8Option);
#endif

#ifdef __MG2470_GPIO1_INCLUDE__
	UINT8 HAL_Gpio1InOutSet(UINT8 u8Port, UINT8 u8OutEna, UINT8 u8Option);
#endif

#ifdef __MG2470_GPIO3_INCLUDE__
	UINT8 HAL_Gpio3InOutSet(UINT8 u8Port, UINT8 u8OutEna, UINT8 u8Option);
#endif	

#endif	// #ifndef __MG2470_GPIO_H__

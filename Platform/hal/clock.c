
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "clock.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates the current frequency of clock for MCU and peripherals..
///		\n 	0 = 16 MHz
///		\n 	1 = 8 MHz
///		\n 	2 = 4 MHz
///		\n 	3 = 2 MHz
///		\n 	4 = 1 MHz
///		\n 	5 = 500 KHz
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8McuPeriClockMode = CLOCK_MODE_16_MHz;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Sets the frequency of clock for MCU and peripherals.
///	
///	@param	u8ClockMode	: Clock frequency mode
///							\n 	0 = 16 MHz
///							\n 	1 = 8 MHz
///							\n 	2 = 4 MHz
///							\n 	3 = 2 MHz
///							\n 	4 = 1 MHz
///							\n 	5 = 500 KHz
///	@return	void
///	@note	If the frequency is lower than 8MHz, RF does not work correctly.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_McuPeriClockSet(UINT8 u8ClockMode)
{
	UINT8	u8Status;
	UINT8	u8MaskOR;

	u8Status = RETURN_CLOCK_SUCCESS;

	switch(u8ClockMode)
	{
		case CLOCK_MODE_16_MHz		: u8MaskOR = BIT3;						break;
		case CLOCK_MODE_8_MHz		: u8MaskOR = 0;							break;
		case CLOCK_MODE_4_MHz		: u8MaskOR = BIT5;						break;
		case CLOCK_MODE_2_MHz		: u8MaskOR = BIT6;						break;
		case CLOCK_MODE_1_MHz		: u8MaskOR = (BIT6 | BIT5);				break;
		case CLOCK_MODE_500_KHz	: u8MaskOR = (BIT3 | BIT2);				break;
		default		: u8Status = RETURN_CLOCK_INVALID_DIVISOR;				break;
	}

	if(u8Status)
	{
		assert_error(CLOCK_CODE | u8Status);
		return u8Status;
	}

	gu8McuPeriClockMode = u8ClockMode;

	CLKCON1 &= ~(BIT7 | BIT6 | BIT5 | BIT3 | BIT2 | BIT1 | BIT0);		// bit[7:5]=0, bit[3:0]=0	
	CLKCON1 |= u8MaskOR;
	if(CLKCON1 & BIT3)		// MAC Mode : Half
	{
		xPHY_MPTOP_CLK_SEL |= BIT2;
	}
	else						// MAC Mode : Quarter
	{
		xPHY_MPTOP_CLK_SEL &= ~BIT2;
	}
	
	return RETURN_CLOCK_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Controls clocks for peripherals.
///	
///	@param	u8PeriClk0	: Each bit controls a clock for the corresponding peripheral.
///							\n 	bit[7] : Clock for SPI. 				0=Off, 1=On. 	Reset value is 0.
///							\n 	bit[6] : Clock for UART1. 			0=Off, 1=On. 	Reset value is 1.
///							\n 	bit[5] : Clock for UART0. 			0=Off, 1=On.	Reset value is 1.
///							\n 	bit[4] : Clock for GPIO.				0=Off, 1=On. 	Reset value is 1.
///							\n 	bit[3] : Clock for TIMER3. 			0=Off, 1=On. 	Reset value is 0.
///							\n 	bit[2] : Clock for TIMER2			0=Off, 1=On. 	Reset value is 0.
///							\n 	bit[1] : Clock for TIMER1. 			0=Off, 1=On. 	Reset value is 0.
///							\n 	bit[0] : Clock for TIMER0.		 	0=Off, 1=On. 	Reset value is 0.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_PeriClock0Set(UINT8 u8PeriClk0)
{
	PERI_CLK_STP0 = u8PeriClk0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Controls clocks for peripherals.
///	
///	@param	u8PeriClk1	: Each bit controls a clock for the corresponding peripheral.
///							\n 	bit[7] : Clock for I2C. 				0=Off, 1=On. Reset value is 0.
///							\n 	bit[6] : Clock for IRTX. 				0=Off, 1=On. Reset value is 0.
///							\n 	bit[5] : Clock for FLASH Controller. 	0=Off, 1=On. Reset value is 0.
///							\n 	bit[4] : Clock for VOICE.			0=Off, 1=On. Reset value is 0.
///							\n 	bit[3] : Clock for I2SRX. 			0=Off, 1=On. Reset value is 0.
///							\n 	bit[2] : Clock for I2STX.				0=Off, 1=On. Reset value is 0.
///							\n 	bit[1] : Clock for QUAD Decoder. 		0=Off, 1=On. Reset value is 0.
///							\n 	bit[0] : Clock for Random Generator. 	0=Off, 1=On. Reset value is 0.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_PeriClock1Set(UINT8 u8PeriClk1)
{
	PERI_CLK_STP1 = u8PeriClk1;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Controls clocks for peripherals.
///	
///	@param	u8PeriClk2	: Each bit controls a clock for the corresponding peripheral.
///							\n	bit[7] : 1=PWM_CH4.	0=Off, 1=On. Reset Value is 0.
///							\n	bit[6] : 1=PWM_CH3.	0=Off, 1=On. Reset Value is 0.
///							\n	bit[5] : 1=PWM_CH2.	0=Off, 1=On. Reset Value is 0.
///							\n	bit[4] : 1=PWM_CH1.	0=Off, 1=On. Reset Value is 0.
///							\n	bit[3] : 1=PWM_CH0.	0=Off, 1=On. Reset Value is 0.
///							\n 	bit[2] : Clock for MAC/PHY registers.	0=Off, 1=On. Reset value is 1.
///							\n 	bit[1] : Clock for LOGIC3V registers. 	0=Off, 1=On. Reset value is 1.
///							\n 	bit[0] : Clock for Clock and Reset Controller. 0=Off, 1=On. Reset value is 1.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_PeriClock2Set(UINT8 u8PeriClk2)
{
	PERI_CLK_STP2 = u8PeriClk2;
}


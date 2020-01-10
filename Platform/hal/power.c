
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2014-08-28
	- Version		: V2.1

	[2014-08-28] V2.1
	- HAL_McuIdleMode() : enhanced.

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "power.h"
#include "phy.h"
#include "wdt.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether Sleep Timer interrupt is generated.
///	When Sleep Timer interrupt is generated, it is set to 1.
///	And after processing the interrupt, it should be cleared.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8SleepTimerIntFlag = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether Wake-up interrupt is generated.
///	When Wake-up interrupt is generated, it is set to 1.
///	And after processing the interrupt, it should be cleared.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8WakeupIntFlag = 0;

#ifdef __MG2470_POWER_RTC_INCLUDE_
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets parameters of RTC interrupt.
///
///	@param	u8IntEna			: 0=Disable, 1=Enable.
///	@param	u8Priority			: Priority of Interrupt. 0=Disable, 1=Enable.
///	@param	u16Tick_1s		: Period in second.
///	@param	u8Tick_3906us	: Period in 3906 us.
///	@return	UINT8. Status
///	@note	Total power-down period = (1 second * u16Tick_1s) + (3.906 ms * u8Tick_3906us)
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_RtcIntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT16 u16Tick_1s, UINT8 u8Tick_3906us)
{
	RTCIP = u8Priority;
	if(u8IntEna)
	{
		if( (u16Tick_1s == 0) && (u8Tick_3906us == 0) )
		{
			assert_error(POWER_CODE | RETURN_POWER_INVALID_PARAMETER);
			return RETURN_POWER_INVALID_PARAMETER;
		}
	
		xRCOSC = 0x6E; 	// RC Calibration enabled
		xRTINT1 = u16Tick_1s >> 8;
		xRTINT2 = u16Tick_1s;
		xRTINT3 = u8Tick_3906us;
		xPDMON &= ~(BIT4 | BIT3);
		xPDMON |= BIT7;	// Sleep Timer mode bit[7] = 0 : 30us, bit[7] = 1 : 3.906ms.
		STIE = 1;
		xPDCON |= BIT5;		// bit[5]=1, Enable sleep timer.
	}
	else
	{
		STIE = 0;
		xPDCON &= ~BIT5;	// bit[5]=0, Disable sleep timer.
	}

	return	RETURN_POWER_SUCCESS;
}
#endif	// #ifdef __MG2470_POWER_RTC_INCLUDE_

#ifdef __MG2470_POWER_SLEEP_INCLUDE_
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets RF and Analog blocks to sleep mode.
///
///	@param	u8Option	: Option
///						\n	bit[7:1] : reserved
///						\n	bit[0] : 1=Use ADC during sleep state. 0=Not use ADC.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_RFAnalogSleep(UINT8 u8Option)
{
	if(u8Option & 0x01)		// ADC is enabled during sleep state.
	{
	}
	else
	{
		xPDCON = BIT7 | BIT4 | BIT3; 	// BOD=1, AVREG=0, ST=0, HRCOSC=1, RCOSC=1
	}
	xPHY_CLK_FR_EN1 = 0;

	HAL_ModemOff();

	xPHY_CLK_EN0 = 0x00;
	xPHY_CLK_EN1 = 0x00;

	if(u8Option & 0x01)		// ADC is enabled during sleep state.
	{
		xPLLPU &= ~BIT7;
	}

	return RETURN_POWER_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Wakes RF and Analog blocks from sleep mode.
///
///	@param	void
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_RFAnalogWakeup(void)
{
	UINT8	u8Status;

	xPDCON = 0xD8;            // BOD=1, AVREG=1, ST=0, HRCOSC=1, RCOSC=1

//	xPHY_CLK_FR_EN1 = BIT5;
	xPHY_CLK_EN0 = 0xF6;
	xPHY_CLK_EN1 = 0x23;

	HAL_ModemOff();
	u8Status = HAL_ModemOn(10000, 0);

	if(u8Status)
	{
		HAL_ModemOff();
		u8Status = HAL_ModemOn(10000, 0);
	}

	return u8Status;
}
#endif	// #ifdef __MG2470_POWER_SLEEP_INCLUDE_

#ifdef __MG2470_POWER_IDLE_INCLUDE_
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets MCU to IDLE mode. And, sets peripherals as wake-up sources.
///
///	@param	u8WakeIE	: Wake-up source in IE register. If each bit is set to 1, the corresponding interrupt is 
///							used for wake-up source.
///						\n	bit[7] : N/A
///						\n	bit[6] : 1=UART1
///						\n	bit[5] : reserved
///						\n	bit[4] : 1=UART0
///						\n	bit[3] : 1=TIMER1
///						\n	bit[2] : 1=External 1
///						\n	bit[1] : 1=TIMER0
///						\n	bit[0] : 1=External 0
///	@param	u8WakeEIE1	: Wake-up source in EIE1 register. If each bit is set to 1, the corresponding interrupt is 
///							used for wake-up source.
///						\n	bit[7] : 1=Wake-up
///						\n	bit[6] : 1=DMA
///						\n	bit[5] : 1=SPI
///						\n	bit[4] : 1=Sleep Timer
///						\n	bit[3] : should be 0.
///						\n	bit[2] : should be 0.
///						\n	bit[1] : should be 0.
///						\n	bit[0] : 1=PHY
///	@param	u8WakeEIE2	: Wake-up source in EIE2 register. If each bit is set to 1, the corresponding interrupt is 
///							used for wake-up source.
///						\n	bit[7:4] : reserved
///						\n	bit[3] : 1=PWM
///						\n	bit[2] : should be 0.
///						\n	bit[1] : 1=I2C
///						\n	bit[0] : 1=IRTX
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_McuIdleMode(UINT8 u8WakeIE, UINT8 u8WakeEIE1, UINT8 u8WakeEIE2)
{
	UINT8	u8IE;
	UINT8	u8EIE1;
	UINT8	u8EIE2;

	UINT8	u8CheckTemp;
	UINT8	ib;

	u8CheckTemp = 0;
	for(ib = 0; ib < 7; ib++)
	{
		u8CheckTemp += ((u8WakeIE >> ib) & BIT0);
		u8CheckTemp += ((u8WakeEIE1 >> ib) & BIT0);
		u8CheckTemp += ((u8WakeEIE2 >> ib) & BIT0);
	}

	if(u8CheckTemp >= 2)
	{
		assert_error(POWER_CODE | RETURN_POWER_TOO_MANY_INPUT);
		return RETURN_POWER_INVALID_PARAMETER;
	}

	SYS_WdtSet(0);		// Disable watchdog. If not, watchdog reset is caused.

	u8IE = IE;
	u8EIE1 = EIE1;
	u8EIE2 = EIE2;

	IE = u8WakeIE & 0x7F;
	EIE1 = u8WakeEIE1;
	EIE2 = u8WakeEIE2;

	IE |= BIT7;	// EA=1

	PCON = 0x01;
	_nop_();
	_nop_();

	IE = 0x00;	// EA=0
	
	EIE2 = u8EIE2;
	EIE1 = u8EIE1;
	IE = u8IE;

	return 0;
}
#endif	// #ifdef __MG2470_POWER_IDLE_INCLUDE_

#ifdef __MG2470_POWER_PM1_INCLUDE_
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Enters into power-down mode 1 (PM1).
///	\n Wake-up Source=H/W Reset, GPIO, Sleep timer.
///	\n In PM1, Analog Voltage Regulator of MG2470B is off, and Digital Regulator is on.
///
///	@param	u16Tick_1s		: Period in second. 
///							\n If bit[7] of "u8Opt" is 1, the period is changed to 7.8 ms(=1 second / 128)
///	@param	u8Tick_3906us	: Period in 3906 us. 
///							\n If bit[7] of "u8Opt" is 1, the period is changed to 30.5 us(=3906 us / 128)
///	@param	u8Tick_65536s	: Period in 65536 seconds. Valid range is 0 ~ 3. 
///							\n If bit[7] of "u8Opt" is 1, the period is changed to 512 second(= 65536 second / 128)
///	@param	u8Opt			: Options for power-down.
///							\n	bit[7]	: Specifies the period of 1 tick. If it is 1, the default period is divided by 128.
///							\n	bit[6:2]	: reserved
///							\n	bit[1]	: 1=External RCOSC(P1.3 & P1.4) is used for clock.
///							\n	bit[0]	: 1=Wake-up only by GPIOs.
///	@return	UINT8. Status.
///
///	@note	Total power-down period = (1 sec * u16Tick_1s) + (3.906 ms * u8Tick_3906us) + (65536 sec * u8Tick_65536s)
///		\n If bit[7] of "u8Opt" is 1, Total power-down period = { (1 sec * u16Tick_1s) + (3.906 ms * u8Tick_3906us) + (65536 sec * u8Tick_65536s) } / 128
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_PowerdownMode1(UINT16 u16Tick_1s, UINT8 u8Tick_3906us, UINT8 u8Tick_65536s, UINT8 u8Opt)
{
	UINT8	u8STP2;
	UINT8	u8IE;
	UINT8	u8EIE1;
	UINT8	u8EIE2;
	UINT8	u8xPDMON;
	UINT8	u8SetPDCON;
	UINT8	u8xPDCON;
	UINT8	u8xRCOSC;
	UINT8	u8Status;
	UINT8	u8ExpectedGPIO0;
	UINT8	u8ExpectedGPIO1;
	UINT8	u8ExpectedGPIO3;
	UINT8	u8xGPIOPE1;

	if( xGPIOMSK0 != (P0_IE & xGPIOMSK0) )
	{
		assert_error(POWER_CODE | RETURN_POWER_DISABLED_PORT_READ);
		return RETURN_POWER_DISABLED_PORT_READ;		
	}

	if( xGPIOMSK1 != (P1_IE & xGPIOMSK1) )
	{
		assert_error(POWER_CODE | RETURN_POWER_DISABLED_PORT_READ);
		return RETURN_POWER_DISABLED_PORT_READ;		
	}

	if( xGPIOMSK3 != (P3_IE & xGPIOMSK3) )
	{
		assert_error(POWER_CODE | RETURN_POWER_DISABLED_PORT_READ);
		return RETURN_POWER_DISABLED_PORT_READ;		
	}

	if(u8Tick_65536s > 3)
	{
		assert_error(POWER_CODE | RETURN_POWER_INVALID_PARAMETER);
		return RETURN_POWER_INVALID_PARAMETER;
	}

	HAL_ModemOff();
	
	u8Status = RETURN_POWER_SUCCESS;

	// Disable watchdog
	SYS_WdtSet(0);	

	u8IE = IE;
	IE = 0x00;

	u8xGPIOPE1 = xGPIOPE1;
	u8xRCOSC = xRCOSC;
	u8xPDCON = xPDCON;
	u8xPDMON = xPDMON;
	u8EIE1 = EIE1;
	u8EIE2 = EIE2;
	u8STP2 = PERI_CLK_STP2;
	PERI_CLK_STP2 = 0x07;
	EIE1 = 0x00;
	EIE2 = 0x00;

	xRCOSC = 0x6E; 		// RC Calibration enabled	
	xPDMON &= ~BIT2;	// bit[2]=0. Disable ADC current.	
	xPDMON &= ~(BIT4 | BIT3);		// RTINT Expand = 0
	if(u8Opt & BIT7)
	{
		xPDMON &= ~BIT7;	// bit[7] = 0. Divided-by-128 tick period.
	}
	else
	{
		xPDMON |= BIT7;		// bit[7] = 1. Default tick period.
	}
	xPDMON |= (u8Tick_65536s & 0x03) << 3;	// RTINT Expand
	xRTINT1 = u16Tick_1s >> 8;
	xRTINT2 = u16Tick_1s;
	xRTINT3 = u8Tick_3906us;
	xRTDLY  = 0x90;

	WUIF = 1;		// WUIF=1. clear interrupt flag
	EIE1 = 0x80;		// WUIE=1	
	IE = 0x80;		// EA=1

	u8SetPDCON = 0xB9;		// BOD=1, AVREG=0, ST=1, HSRCOSC=1, RCOSC=1, ExtRCOSC=0, Mode=2'b01
	// Option for power-down
	if(u8Opt & BIT0)	u8SetPDCON &= ~BIT5;	// bit[5]=0. Disable sleep timer.
	if(u8Opt & BIT1)
	{
		u8SetPDCON |= BIT2;		// bit[2]=1. External RCOSC(P1.3 & P1.4)
		xGPIOPE1 &= ~(BIT3 |BIT4);
	}
	xPDCON = u8SetPDCON;

	PERI_CLK_STP2 = 0x00;
	while(WDT & 0x20);

	u8ExpectedGPIO0 = xGPIOMSK0 & (P0 ^ xGPIOPOL0);
	u8ExpectedGPIO1 = xGPIOMSK1 & (P1 ^ xGPIOPOL1);
	u8ExpectedGPIO3 = xGPIOMSK3 & (P3 ^ xGPIOPOL3);

	if(	(xGPIOMSK0 == u8ExpectedGPIO0)
	&&	(xGPIOMSK1 == u8ExpectedGPIO1)
	&&	(xGPIOMSK3 == u8ExpectedGPIO3) )
	{
		PCON = 0x02;
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	}	
	else
	{
		u8Status = RETURN_POWER_WRONG_GPIO_VALUE;
	}

	PERI_CLK_STP2 = u8STP2;
	WUIF = 1;
	CLKCON2 &= ~BIT6;	// bit[6]=0. Clock source change HSRCOSC --> 32MHz crystal
	EIE1 = u8EIE1;
	EIE2 = u8EIE2;
	xPDCON = u8xPDCON;
	xPDMON = u8xPDMON;
	xRCOSC = u8xRCOSC;
	xGPIOPE1 = u8xGPIOPE1;

	IE = u8IE;

	if(u8Status)
	{
		assert_error(POWER_CODE | u8Status);
	}

	HAL_ModemOn(10000, 0);
	
	return u8Status;
}
#endif	// #ifdef __MG2470_POWER_PM1_INCLUDE_

#ifdef __MG2470_POWER_PM2_INCLUDE_
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Enters into power-down mode 2 (PM2).
///	\n Wake-up Source=H/W Reset, GPIO, Sleep timer. 
///	\n In PM2, Analog Voltage Regulator of MG2470B is off, and Digital Regulator is off.
///
///	@param	u16Tick_1s		: Period in second.
///							\n If bit[7] of "u8Opt" is 1, the period is changed to 7.8 ms(=1 second / 128)
///	@param	u8Tick_3906us	: Period in 3906 us.
///							\n If bit[7] of "u8Opt" is 1, the period is changed to 30.5 us(=3906 us / 128)
///	@param	u8Tick_65536s	: Period in 65536 seconds. Valid range is 0 ~ 3. 
///							\n If bit[7] of "u8Opt" is 1, the period is changed to 512 second(= 65536 second / 128)
///	@param	u8Opt			: Options for power-down.
///							\n	bit[7]	: Specifies the period of 1 tick. If it is 1, the default period is divided by 128.
///							\n	bit[6:2]	: reserved
///							\n	bit[1]	: 1=External RCOSC(P1.3 & P1.4) is used for clock.
///							\n	bit[0]	: 1=Wake-up only by GPIOs.
///	@return	UINT8. Status.
///
///	@note	Total power-down period = (1 sec * u16Tick_1s) + (3.906 ms * u8Tick_3906us) + (65536 sec * u8Tick_65536s)
///		\n If bit[7] of "u8Opt" is 1, Total power-down period = { (1 sec * u16Tick_1s) + (3.906 ms * u8Tick_3906us) + (65536 sec * u8Tick_65536s) } / 128
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_PowerdownMode2(UINT16 u16Tick_1s, UINT8 u8Tick_3906us, UINT8 u8Tick_65536s, UINT8 u8Opt)
{
	UINT8	u8STP2;
	UINT8	u8IE;
	UINT8	u8EIE1;
	UINT8	u8EIE2;
	UINT8	u8xPDMON;
	UINT8	u8SetPDCON;
	UINT8	u8xPDCON;
	UINT8	u8xRCOSC;
	UINT8	u8Status;
	UINT8	u8ExpectedGPIO0;
	UINT8	u8ExpectedGPIO1;
	UINT8	u8ExpectedGPIO3;
	UINT8	u8xGPIOPE1;

	if( xGPIOMSK0 != (P0_IE & xGPIOMSK0) )
	{
		assert_error(POWER_CODE | RETURN_POWER_DISABLED_PORT_READ);
		return RETURN_POWER_DISABLED_PORT_READ;		
	}

	if( xGPIOMSK1 != (P1_IE & xGPIOMSK1) )
	{
		assert_error(POWER_CODE | RETURN_POWER_DISABLED_PORT_READ);
		return RETURN_POWER_DISABLED_PORT_READ;		
	}

	if( xGPIOMSK3 != (P3_IE & xGPIOMSK3) )
	{
		assert_error(POWER_CODE | RETURN_POWER_DISABLED_PORT_READ);
		return RETURN_POWER_DISABLED_PORT_READ;		
	}

	if(u8Tick_65536s > 3)
	{
		assert_error(POWER_CODE | RETURN_POWER_INVALID_PARAMETER);
		return RETURN_POWER_INVALID_PARAMETER;
	}

	u8Status = RETURN_POWER_SUCCESS;

	// Watchdog for abnormal state
	SYS_WdtSet(10000);	

	u8IE = IE;
	IE = 0x00;

	u8xGPIOPE1 = xGPIOPE1;
	u8xRCOSC = xRCOSC;
	u8xPDCON = xPDCON;
	u8xPDMON = xPDMON;
	u8EIE1 = EIE1;
	u8EIE2 = EIE2;
	u8STP2 = PERI_CLK_STP2;
	PERI_CLK_STP2 = 0x07;
	EIE1 = 0x00;
	EIE2 = 0x00;
	
	xRCOSC = 0x6E; 		// RC Calibration enabled	
	xPDMON &= ~BIT2;	// bit[2]=0. Disable ADC current.	
	xPDMON &= ~(BIT4 | BIT3);		// RTINT Expand = 0
	if(u8Opt & BIT7)
	{
		xPDMON &= ~BIT7;	// bit[7] = 0. Divided-by-128 tick period.
	}
	else
	{
		xPDMON |= BIT7;		// bit[7] = 1. Default tick period.
	}	
	xPDMON |= (u8Tick_65536s & 0x03) << 3;	// RTINT Expand
	xRTINT1 = u16Tick_1s >> 8;
	xRTINT2 = u16Tick_1s;
	xRTINT3 = u8Tick_3906us;
	xRTDLY  = 0x90;

	WUIF = 1;		// WUIF=1. clear interrupt flag
	EIE1 = 0x80;		// WUIE=1	
	IE = 0x80;		// EA=1

	u8SetPDCON = 0xBA;		// BOD=1, AVREG=0, ST=1, HSRCOSC=1, RCOSC=1, ExtRCOSC=0, Mode=2'b10
	// Option for power-down
	if(u8Opt & BIT0)	u8SetPDCON &= ~BIT5;	// bit[5]=0. Disable sleep timer.
	if(u8Opt & BIT1)
	{
		u8SetPDCON |= BIT2;		// bit[2]=1. External RCOSC(P1.3 & P1.4)
		xGPIOPE1 &= ~(BIT3 |BIT4);	
	}
	xPDCON = u8SetPDCON;

	PERI_CLK_STP2 = 0x00;
	while(WDT & 0x20);

	u8ExpectedGPIO0 = xGPIOMSK0 & (P0 ^ xGPIOPOL0);
	u8ExpectedGPIO1 = xGPIOMSK1 & (P1 ^ xGPIOPOL1);
	u8ExpectedGPIO3 = xGPIOMSK3 & (P3 ^ xGPIOPOL3);

	if(	(xGPIOMSK0 == u8ExpectedGPIO0)
	&&	(xGPIOMSK1 == u8ExpectedGPIO1)
	&&	(xGPIOMSK3 == u8ExpectedGPIO3) )
	{
		PCON = 0x02;
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_();		
	}
	else
	{
		u8Status = RETURN_POWER_WRONG_GPIO_VALUE;
	}	

	PERI_CLK_STP2 = u8STP2;
	WUIF = 1;
	CLKCON2 &= ~BIT6;	// bit[6]=0. Clock source change HSRCOSC --> 32MHz crystal
	EIE1 = u8EIE1;
	EIE2 = u8EIE2;
	xPDCON = u8xPDCON;	
	xPDMON = u8xPDMON;
	xRCOSC = u8xRCOSC;
	xGPIOPE1 = u8xGPIOPE1;

	IE = u8IE;

	if(u8Status)
	{
		assert_error(POWER_CODE | u8Status);
	}
	return u8Status;
}
#endif	// #ifdef __MG2470_POWER_PM2_INCLUDE_

#ifdef __MG2470_POWER_PM3_INCLUDE_
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Enters into power-down mode 3 (PM3).
///	\n Wake-up Source=H/W Reset, GPIO. AVREG=Off. DVREG=Off
///	\n In PM3, Analog Voltage Regulator of MG2470B is off, and Digital Regulator is off.
///
///	@param	u8Opt			: Options for power-down.
///							\n	bit[7:2]	: reserved
///							\n	bit[1]	: 1=External RCOSC(P1.3 & P1.4) is used for clock.
///							\n	bit[0]	: reserved
///	@return	UINT8. Status.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_PowerdownMode3(UINT8 u8Opt)
{
	UINT8	u8STP2;
	UINT8	u8IE;
	UINT8	u8EIE1;
	UINT8	u8EIE2;
	UINT8	u8xPDMON;
	UINT8	u8SetPDCON;
	UINT8	u8xPDCON;
	UINT8	u8xRCOSC;
	UINT8	u8Status;
	UINT8	u8ExpectedGPIO0;
	UINT8	u8ExpectedGPIO1;
	UINT8	u8ExpectedGPIO3;
	UINT8	u8xGPIOPE1;

	if( xGPIOMSK0 != (P0_IE & xGPIOMSK0) )
	{
		assert_error(POWER_CODE | RETURN_POWER_DISABLED_PORT_READ);
		return RETURN_POWER_DISABLED_PORT_READ;		
	}

	if( xGPIOMSK1 != (P1_IE & xGPIOMSK1) )
	{
		assert_error(POWER_CODE | RETURN_POWER_DISABLED_PORT_READ);
		return RETURN_POWER_DISABLED_PORT_READ;		
	}

	if( xGPIOMSK3 != (P3_IE & xGPIOMSK3) )
	{
		assert_error(POWER_CODE | RETURN_POWER_DISABLED_PORT_READ);
		return RETURN_POWER_DISABLED_PORT_READ;		
	}

	u8Status = RETURN_POWER_SUCCESS;

	// Watchdog for abnormal state
	SYS_WdtSet(10000);

	u8IE = IE;
	IE = 0x00;

	u8xGPIOPE1 = xGPIOPE1;
	u8xRCOSC = xRCOSC;
	u8xPDCON = xPDCON;
	u8xPDMON = xPDMON;
	u8EIE1 = EIE1;
	u8EIE2 = EIE2;
	u8STP2 = PERI_CLK_STP2;
	PERI_CLK_STP2 = 0x07;
	EIE1 = 0x00;
	EIE2 = 0x00;
	
	xRCOSC = 0x6E; 		// RC Calibration enabled	
	xPDMON &= ~BIT2;	// bit[2]=0. Disable ADC current.	
	xPDMON &= ~(BIT4 | BIT3);		// RTINT Expand = 0
	xRTDLY  = 0x90;

	WUIF = 1;		// WUIF=1. clear interrupt flag
	EIE1 = 0x80;		// WUIE=1	
	IE = 0x80;		// EA=1

	u8SetPDCON = 0x9B;		// BOD=1, AVREG=0, ST=0, HSRCOSC=1, RCOSC=1, ExtRCOSC=0, Mode=2'b11
	// Option for power-down
	if(u8Opt & BIT1)
	{
		u8SetPDCON |= BIT2;		// bit[2]=1. External RCOSC(P1.3 & P1.4)
		xGPIOPE1 &= ~(BIT3 |BIT4);
	}
	xPDCON = u8SetPDCON;

	PERI_CLK_STP2 = 0x00;
	while(WDT & 0x20);

	u8ExpectedGPIO0 = xGPIOMSK0 & (P0 ^ xGPIOPOL0);
	u8ExpectedGPIO1 = xGPIOMSK1 & (P1 ^ xGPIOPOL1);
	u8ExpectedGPIO3 = xGPIOMSK3 & (P3 ^ xGPIOPOL3);

	if(	(xGPIOMSK0 == u8ExpectedGPIO0)
	&&	(xGPIOMSK1 == u8ExpectedGPIO1)
	&&	(xGPIOMSK3 == u8ExpectedGPIO3) )
	{
		PCON = 0x02;
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_();		
	}
	else
	{
		u8Status = RETURN_POWER_WRONG_GPIO_VALUE;
	}	

	PERI_CLK_STP2 = u8STP2;
	WUIF = 1;
	CLKCON2 &= ~BIT6;	// bit[6]=0. Clock source change HSRCOSC --> 32MHz crystal
	EIE1 = u8EIE1;
	EIE2 = u8EIE2;
	xPDCON = u8xPDCON;	
	xPDMON = u8xPDMON;
	xRCOSC = u8xRCOSC;
	xGPIOPE1 = u8xGPIOPE1;

	IE = u8IE;

	if(u8Status)
	{
		assert_error(POWER_CODE | u8Status);
	}
	return u8Status;
}
#endif	// #ifdef __MG2470_POWER_PM3_INCLUDE_

#ifdef __MG2470_POWER_GPIO_INCLUDE_
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets a port of GPIO0(0.x) as the wake-up source.
///
///	@param	u8Port		: 0~7=Port Number
///	@param	u8WakeupEna	: Specifies whether the port is used for wake-up source. 0=Disable, 1=Enable.
///	@param	u8RisingEdge	: Specifies the edge polarity for wake-up. 
///						\n 0=Wake-up on falling edge. In this case, the internal connection of the port is pull-up. 
///						\n 1=Wake-up on rising edge. In this case, the internal connection of the port is pull-down.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_PowerdownGpio0WakeupSourceSet(UINT8 u8Port, UINT8 u8WakeupEna, UINT8 u8RisingEdge)
{
	UINT8	u8MaskBit;

	if(u8Port > 7)
	{
		assert_error(POWER_CODE | RETURN_POWER_INVALID_GPIO_PORT);
		return RETURN_POWER_INVALID_GPIO_PORT;		
	}

	u8MaskBit = (0x01 << u8Port);

	xGPIOPE0 |= u8MaskBit;			// PE0[n]=1. Input mode.
	if(u8RisingEdge)
	{
		xGPIOPOL0 |= u8MaskBit;		// POL0[n]=1. Rising edge.
		xGPIOPS0 &= ~u8MaskBit;		// PS0[n]=0. Pull-down.
	}
	else
	{
		xGPIOPOL0 &= ~u8MaskBit;		// POL0[n]=0. Falling edge.
		xGPIOPS0 |= u8MaskBit;		// PS0[n]=1. Pull-up.
	}

	if(u8WakeupEna)
	{
		xGPIOMSK0 |= u8MaskBit;		// MSK0[n]=1. Enable as a wake-up source.
	}
	else
	{
		xGPIOMSK0 &= ~u8MaskBit;		// MSK0[n]=0. Disable
	}

	return RETURN_POWER_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets a port of GPIO1(1.x) as the wake-up source.
///
///	@param	u8Port		: 0~7=Port Number
///	@param	u8WakeupEna	: Specifies whether the port is used for wake-up source. 0=Disable, 1=Enable.
///	@param	u8RisingEdge	: Specifies the edge polarity for wake-up. 
///						\n 0=Wake-up on falling edge. In this case, the internal connection of the port is pull-up. 
///						\n 1=Wake-up on rising edge. In this case, the internal connection of the port is pull-down.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_PowerdownGpio1WakeupSourceSet(UINT8 u8Port, UINT8 u8WakeupEna, UINT8 u8RisingEdge)
{
	UINT8	u8MaskBit;

	if(u8Port > 7)
	{
		assert_error(POWER_CODE | RETURN_POWER_INVALID_GPIO_PORT);
		return RETURN_POWER_INVALID_GPIO_PORT;		
	}

	u8MaskBit = (0x01 << u8Port);

	xGPIOPE1 |= u8MaskBit;			// PE1[n]=1. Input mode.
	if(u8RisingEdge)
	{
		xGPIOPOL1 |= u8MaskBit;		// POL1[n]=1. Rising edge.
		xGPIOPS1 &= ~u8MaskBit;		// PS1[n]=0. Pull-down.
	}
	else
	{
		xGPIOPOL1 &= ~u8MaskBit;		// POL1[n]=0. Falling edge.
		xGPIOPS1 |= u8MaskBit;		// PS1[n]=1. Pull-up.
	}

	if(u8WakeupEna)
	{
		xGPIOMSK1 |= u8MaskBit;		// MSK1[n]=1. Enable as a wake-up source.
	}
	else
	{
		xGPIOMSK1 &= ~u8MaskBit;		// MSK1[n]=0. Disable
	}

	return RETURN_POWER_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets a port of GPIO3(3.x) as the wake-up source.
///
///	@param	u8Port		: 0~7=Port Number
///	@param	u8WakeupEna	: Specifies whether the port is used for wake-up source. 0=Disable, 1=Enable.
///	@param	u8RisingEdge	: Specifies the edge polarity for wake-up. 
///						\n 0=Wake-up on falling edge. In this case, the internal connection of the port is pull-up. 
///						\n 1=Wake-up on rising edge. In this case, the internal connection of the port is pull-down.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_PowerdownGpio3WakeupSourceSet(UINT8 u8Port, UINT8 u8WakeupEna, UINT8 u8RisingEdge)
{
	UINT8	u8MaskBit;

	if(u8Port > 7)
	{
		assert_error(POWER_CODE | RETURN_POWER_INVALID_GPIO_PORT);
		return RETURN_POWER_INVALID_GPIO_PORT;		
	}

	u8MaskBit = (0x01 << u8Port);

	xGPIOPE3 |= u8MaskBit;			// PE3[n]=1. Input mode.
	if(u8RisingEdge)
	{
		xGPIOPOL3 |= u8MaskBit;		// POL3[n]=1. Rising edge.
		xGPIOPS3 &= ~u8MaskBit;		// PS3[n]=0. Pull-down.
	}
	else
	{
		xGPIOPOL3 &= ~u8MaskBit;		// POL3[n]=0. Falling edge.
		xGPIOPS3 |= u8MaskBit;		// PS3[n]=1. Pull-up.
	}

	if(u8WakeupEna)
	{
		xGPIOMSK3 |= u8MaskBit;		// MSK3[n]=1. Enable as a wake-up source.
	}
	else
	{
		xGPIOMSK3 &= ~u8MaskBit;		// MSK3[n]=0. Disable
	}

	return RETURN_POWER_SUCCESS;
}
#endif	// #ifdef __MG2470_POWER_GPIO_INCLUDE_


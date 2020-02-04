
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2011.
	- Date		: 2013-01-14
	- Version		: V2.2

	[2013-01-14] V2.2
	- HAL_FlashPageRead() : enhanced // Check-sum added when flash page read done
	- zPrintf() : enhanced // UART interrupt enable/disable check
	- LIB update : 250Kbps, 1Mbps Datarate performance enhanced.
	- UART0_ISR() : Enhanced
	- UART1_ISR() : Enhanced

	[2012-09-06] V2.1a
	- HAL_FlashPageWrite() : enhanced // Check-sum added when flash page write done

	[2012-08-20] V2.1
	- LIB update : RX Mix current increases for RX performance enhancing.
	- Debug message enhanced when CHIPID error.

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "include_top.h"
#include "option_EVB.h"

#include "uart.h"
#include "timer.h"
#include "wdt.h"
#include "clock.h"
#include "gpio.h"
#include "phy.h"
#include "hmac.h"
#include "adc.h"
#include "power.h"
#include "spi.h"
#include "exti.h"

#include "util_app.h"
#include "util_sys.h"

#include "isr_power.c"

UINT8 TestLib_PacketTransmit(void)
{
	tMAC_DATA_REQ		tMacDataReq;
	UINT8	u8Status;
	UINT8	au8TxBuf[32];
	UINT16	iw;
	
	for(iw = 0; iw<32; iw++)
	{
		au8TxBuf[iw] = iw;
	}

	tMacDataReq.u8DstMode = maccAddrMode_16bit;
	tMacDataReq.u16DstPanID = gtMIB.u16DstPanID;
	tMacDataReq.uDstAddr.u16Short = gtMIB.u16DstShortAddr;
	tMacDataReq.u8SrcMode = maccAddrMode_16bit;
	tMacDataReq.u8TxOption = BIT0;
	tMacDataReq.u8MsduLen = 32;
	tMacDataReq.pu8Msdu = au8TxBuf;

	u8Status = AppLib_MacDataRequest(&tMacDataReq);
	return u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Callback function called in PHY_ISR() when OOB(Out-Of-Band) code in a received packet is not 0.
///	\n	OOB code is contained in the reserved fields of IEEE 802.15.4 frame format. And, on the specification,
///		the fiedls should be 0. In other words, OOB is out of the specification. It means that a OOB packet are not 
///		compatible to the packet for IEEE 802.15.4 specification.
///	\n	If a received packet is processed in this function, the return value should be 1. Then, the packet is 
///		discarded in the PHY_ISR(). It prevents duplicated processing for the packet.
///	\n	Otherwise, the return value should be 0. It means that the packet is processed in PHY_ISR().
///
///	@param	u8RxFifoStartIdx	: Start address of the received packet including the length field in MAC RXFIFO.
///	@return	UINT8. Status of processing. 0=Not processed, 1=Processed.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 Callback_OobRxFromPhyISR(UINT8 u8RxFifoStartIdx)
{
	u8RxFifoStartIdx = u8RxFifoStartIdx;
	
	return	0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Main function.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void main(void)
{
	UINT16	iw;
	UINT8	u8Status;
	UINT8	u8KeyIn;

	tHW_INFORMATION	tDataHIB;
	
	UINT8	u8LedCounter_Rx = 0;
	INT16	i16AdcRead;
	
	tPACKET_INFO*	ptRxPacket;
	tMAC_INFO		tMacInfo;
	UINT8		u8DeviceMode;

	UINT16	u16VerifyCmdID = 0;
	UINT16	u16EdgeTest = 0;
	UINT16	u16PreviousVerifyCmdID = 0;

	UINT8	u8LineFeedDisplayFlag = 0;
	UINT8	u8AgingTest = 0;	
	
	//	Watchdog limit for initialization
	SYS_WdtSet(3000);

	//	TIMER0 : Turn on the clock and Set
	HAL_Timer0ClockOn(ON);
	HAL_Timer0IntSet(ENABLE, PRIORITY_LOW, RUN, 1);			// Enable TIMER0 Interrupt

	// 	UART1 : Turn on the clock and Set
	HAL_Uart1ClockOn(ON);
	HAL_Uart1IntSet(ENABLE, PRIORITY_LOW, 115200, UART_MODE_8N1);

	//	Interrupt Enable
	HAL_SystemIntSet(ENABLE);			// EA=1

	// 	GPIO : Turn on the clock and Set
	// TODO: Set GPIOs to be used if needed.	
	HAL_GpioClockOn(ON);	
	HAL_Gpio0InOutSet(0, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP00 = 1;
	HAL_Gpio0InOutSet(1, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP01 = 0;
	HAL_Gpio0InOutSet(2, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP02 = 0;
	HAL_Gpio0InOutSet(3, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP03 = 0;
	HAL_Gpio3InOutSet(4, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP34 = 0;
	HAL_Gpio3InOutSet(5, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP35 = 0;

	//	Booting Message
	zPrintf(1, "\n");
	zPrintf(1, "\n #########################################");
	zPrintf(1, "\n # MG2470B-EVK");
	zPrintf(1, "\n # Version : V%c.%c%c", EVK_VER_MAIN, EVK_VER_SUB, EVK_VER_PATCH);
	zPrintf(1, " (Register V%c.%c)", LIB_VER_MAIN, LIB_VER_SUB);
	zPrintf(1, "\n # RadioPulse Inc, 2012");
	zPrintf(1, "\n # CHIP ID = 0x%02x", (short)xCHIPID);
	zPrintf(1, "\n #########################################");
	zPrintf(1, "\n");
	if(xCHIPID == 0x70)
	{
		zPrintf(1, "\n Failed. This software only supports CHIP ID over 0x70 !!!");
		while(SYS_Uart1TxWorking());
		SYS_WdtReset(0);
	}	
#ifdef DEBUG
	zPrintf(1, "\n >> Operating in DEBUG mode");
	zPrintf(1, "\n >> For RELEASE mode, remove \"DEBUG\"");
	zPrintf(1, "\n >> in Options->C51->Preprocessor Symbols of Keil Project");
	zPrintf(1, "\n");
#endif

	//	Register Configuration & Modem Initializing
	zPrintf(1, "\n");
	zPrintf(1, "\n >> HAL_ModemInit()");
	HAL_ModemInit(0);
	if(HAL_ModemOn(10000, 1))
	{
		SYS_WdtReset(0);
	}
	else
	{
		zPrintf(1, " => OK");
	}
	HAL_ModemPost();

	//	Initialize MIB
	memset(gtMIB.au8DstIEEEAddr, 0x00, 8);
	memset(gtMIB.au8IEEEAddr, 0x00, 8);
	gtMIB.u16DstPanID = maccBroadcast;
	gtMIB.u16DstShortAddr = maccBroadcast;
	gtMIB.u16PanID = maccBroadcast;
	gtMIB.u16ShortAddr = maccBroadcast;
	gtMIB.u8BackoffRetry = 3;
	gtMIB.u8Channel = 0x0B;
	gtMIB.u8DataRate = PHY_DATA_RATE_250K;
	gtMIB.u8DSN = 0xEE;
	gtMIB.u8NoAckRetry = 3;
	gtMIB.u8TxPowerLevel = 0;
	// TODO: Here, MIB parameter can be modified, especially using HIB.
	// Set HIB Information to MIB.
	if(SYS_HIBCopy((UINT8 *)&tDataHIB))
	{
		zPrintf(1, "\n >> HIB Copy OK !!! ");
		Display_HIB(&tDataHIB);

		gtMIB.u8TxPowerLevel = 0x00;
		gtMIB.u8Channel = tDataHIB.u8Channel;
		gtMIB.u16PanID = (tDataHIB.au8PanID[1] << 8) | tDataHIB.au8PanID[0];
		gtMIB.u16DstPanID = gtMIB.u16PanID;
		gtMIB.u16ShortAddr = (tDataHIB.au8NwkAddr[1] << 8) | tDataHIB.au8NwkAddr[0];
		rpmemcpy(gtMIB.au8IEEEAddr, tDataHIB.au8IEEEAddr, 8);
		gtMIB.u8DataRate = tDataHIB.u8DataRate;
		gtMIB.u16DstShortAddr = (tDataHIB.au8GeneralWord0[1] << 8) | tDataHIB.au8GeneralWord0[0];
		rpmemcpy(gtMIB.au8DstIEEEAddr, tDataHIB.au8Reserved_0, 8);
	}
	else
	{
		// When HIB is invalid, Network Parameters are selected by keyboard input.
		zPrintf(1, "\n >> HIB Copy FAIL !!!");
		zPrintf(1, "\n #########################################");		
		zPrintf(1, "\n >> 0 : PER Transmitter");
		zPrintf(1, "\n >> 1 : PER Receiver");
		zPrintf(1, "\n #########################################");
		zPrintf(1, "\n >> Select Device Mode (within 5 seconds) : ");

		// 	Wait for 5 seconds. If no key input, Device Mode is 0.
		u8DeviceMode = '0';
		AppLib_VirtualTimerSet(0, 5000);
		while(AppLib_VirtualTimerGet(0))
		{
			SYS_WdtSet(3000);
			if(SYS_Uart1Get(&u8KeyIn))
			{
				if(u8KeyIn == '1')
				{
					u8DeviceMode = '1';
				}
				break;
			}
		}
		SYS_Uart1Put(u8DeviceMode);

		if(u8DeviceMode == '0')
		{
			gtMIB.u8Channel = INIT_CHANNEL;
			gtMIB.u16PanID = INIT_PAN_ID;
			gtMIB.u16DstPanID = gtMIB.u16PanID;
			gtMIB.u16ShortAddr = INIT_DEV0_SHORTADDR;
			gtMIB.u16DstShortAddr = INIT_DEV1_SHORTADDR;
			for(iw=0 ; iw<8 ; iw++)
			{
				gtMIB.au8IEEEAddr[iw] = INIT_DEV0_IEEEADDR_LSB + iw;
				gtMIB.au8DstIEEEAddr[iw] = INIT_DEV1_IEEEADDR_LSB + iw;
			}
		}
		else if(u8DeviceMode == '1')
		{
			gtMIB.u8Channel = INIT_CHANNEL;
			gtMIB.u16PanID = INIT_PAN_ID;
			gtMIB.u16DstPanID = gtMIB.u16PanID;
			gtMIB.u16ShortAddr = INIT_DEV1_SHORTADDR;
			gtMIB.u16DstShortAddr = INIT_DEV0_SHORTADDR;
			for(iw=0 ; iw<8 ; iw++)
			{
				gtMIB.au8IEEEAddr[iw] = INIT_DEV1_IEEEADDR_LSB + iw;
				gtMIB.au8DstIEEEAddr[iw] = INIT_DEV0_IEEEADDR_LSB + iw;
			}
		}
	}
		
	Display_MIB(&gtMIB);

	//	Configure Network Parameters
	u8Status = HAL_ChannelSet(gtMIB.u8Channel);
	if(u8Status)
	{
		zPrintf(1, "\n >> Channel Set Failed (%02x)",(short)u8Status);
	}	
	HAL_PanIDSet(gtMIB.u16PanID);
	HAL_ShortAddrSet(gtMIB.u16ShortAddr);
	HAL_TxPowerSet(gtMIB.u8TxPowerLevel);	
	HAL_DataRateSet(gtMIB.u8DataRate);
	HAL_IEEEAddrSet(gtMIB.au8IEEEAddr);

	//	Initialize Hardware MAC
	HAL_MacInit();

	//	Initialize RF blocks and enable PHY interrupt.
	HAL_RxEnable(DISABLE);
	HAL_PhyIntSet(ENABLE, PRIORITY_HIGH);
	HAL_RxEnable(ENABLE);

	// 	Initialize Random Generator with SEED
	u8Status = SYS_AdcGet_Once(ADC_CHAN_TEMPERATURE, &i16AdcRead);
	if(u8Status)
	{
		zPrintf(1, "\n AdcGet Error. Sta = 0x%02x",(short)u8Status);
	}
	srand((UINT16)i16AdcRead);
	gtMIB.u8DSN = rand();

	//	Enable GPIO to control External Amplifier
	// TODO: If a external amplifier is used, GPOIs for that should be set.

	zPrintf(1, "\n");
	zPrintf(1, "\n >> MG2470B - [POWER] Sample Start");

	zPrintf(1, "\n");
	zPrintf(1, "\n ===>> HELP ===============================");
	zPrintf(1, "\n h : HELP Menu");
	zPrintf(1, "\n a : Again Previous VerifyCmd");
	zPrintf(1, "\n ==========================================");
	zPrintf(1, "\n");

	while(1)
	{	
		SYS_WdtSet(10000);		
		//
		// TODO: The application-specific routines are implemented here.
		//
		
		//----------------------------------------------------------------
		// Process for PHY interrupt		
		ptRxPacket = SYS_RxQueueCheck();		
		if(ptRxPacket)		// A packet is received
		{
			u8Status = SYS_RxQueueParse(ptRxPacket, &tMacInfo);
			if(u8Status)
			{
				if(u8Status == 1)
				{
					zPrintf(1, "\n RX-Packet : Parsing Failed");
				}
				else if(u8Status == 2)
				{
					zPrintf(1, "\n RX-Packet : Duplicated");
				}
			}
			else
			{
				// TODO: The processing routines for the received packet are implemented here.
				u8LedCounter_Rx++;
				if(u8LedCounter_Rx & 0x01)	GP00 = ~GP00;
				
				Display_MacInfo(&tMacInfo);
				zPrintf(1, "\n");
			}

			SYS_RxQueueClear();
		}
		//----------------------------------------------------------------

		if(SYS_Uart1Get(&u8KeyIn))
		{
			if(u8KeyIn == 'h')
			{	
				zPrintf(1, "\n ===>> HELP ===============================");
				zPrintf(1, "\n h : HELP Menu");
				zPrintf(1, "\n a : Again Previous VerifyCmd");
				zPrintf(1, "\n ------------------------------------");
				zPrintf(1, "\n [0x0800] PM1 : Wake-up source : 3sec");
				zPrintf(1, "\n [0x0801] PM1 : Wake-up source : P1.7");
				zPrintf(1, "\n [0x0802] PM1 : Wake-up source : EXT0");
				zPrintf(1, "\n [0x0803] PM1 : Wake-up source : EXT1");
				zPrintf(1, "\n [0x0804] PM1 : Wake-up source : EXT0/1 & P0.7");
				zPrintf(1, "\n [0x0805] PM1 : Tx after Wake-up");
				zPrintf(1, "\n ------------------------------------");
				zPrintf(1, "\n [0x0810] PM2 : Wake-up source : 3sec");
				zPrintf(1, "\n [0x0811] PM2 : Wake-up source : P0.7");
				zPrintf(1, "\n [0x0812] PM2 : Wake-up source : EXT0");
				zPrintf(1, "\n [0x0813] PM2 : Wake-up source : EXT1");
				zPrintf(1, "\n [0x0814] PM2 : Wake-up source : EXT0/1 & P0.7");
				zPrintf(1, "\n ------------------------------------");
				zPrintf(1, "\n [0x0820] PM3 : Wake-up source : EXT0");
				zPrintf(1, "\n [0x0821] PM3 : Wake-up source : EXT1");
				zPrintf(1, "\n [0x0822] PM3 : Wake-up source : EXT0/1");
				zPrintf(1, "\n ------------------------------------");
				zPrintf(1, "\n [0x0830] Idle : Wake-up source : UART1");
				zPrintf(1, "\n [0x0831] Idle : Wake-up source : TIMER0(98ms)");
				zPrintf(1, "\n [0x0832] Idle : Wake-up source : TIMER1(98ms)");
				zPrintf(1, "\n [0x0833] Idle : Wake-up source : EXT0/1");
				zPrintf(1, "\n [0x0834] Idle : Wake-up source : PHY");
				zPrintf(1, "\n ------------------------------------");
				zPrintf(1, "\n [0x0840] RtcInt : 1sec period");
				zPrintf(1, "\n ------------------------------------");
				zPrintf(1, "\n [0x0850] RF Sleep");
				zPrintf(1, "\n [0x0851] RF Wake-up");
				zPrintf(1, "\n [0x0852] RF Sleep continuously : Tx after Wake-up by"); zPrintf(1, " UART1 & EXT0/1");
				zPrintf(1, "\n [0x0853] RF Sleep continuously : OFF");				
				zPrintf(1, "\n ------------------------------------");
				zPrintf(1, "\n [0x08FF] Disable all Wake-up sources");
				zPrintf(1, "\n ==========================================");
				zPrintf(1, "\n >> Input Verify Command : 0x");
				zScanf(1, &u16VerifyCmdID);
				zPrintf(1, " => OK");
				u16PreviousVerifyCmdID = u16VerifyCmdID;
			}
			else if(u8KeyIn == '0')
			{
				zPrintf(1, "\n PM1 Wake-up by GPIO & Timer(5Sec)");

				HAL_PowerdownGpio0WakeupSourceSet(6, 1, 0);
				HAL_PowerdownGpio0WakeupSourceSet(7, 1, 0);

				HAL_PowerdownGpio3WakeupSourceSet(0, 1, 0);
				HAL_PowerdownGpio3WakeupSourceSet(1, 1, 0);
				HAL_PowerdownGpio3WakeupSourceSet(2, 1, 0);
				HAL_PowerdownGpio3WakeupSourceSet(3, 1, 0);
				HAL_PowerdownMode1(5, 0, 0, 0);
				zPrintf(1, " => Wake-up!");
			}
			else if(u8KeyIn == 'a')
			{
				u16VerifyCmdID = u16PreviousVerifyCmdID;
			}
			else
			{
				zPrintf(1, "\n >> Invalid Input");
				zPrintf(1, "\n");
			}
		}

		//
		// TODO: The application-specific routines are implemented here.
		u8LineFeedDisplayFlag = 1;
		switch(u16VerifyCmdID)
		{
			//==========================================================================
			case (POWER_CODE | 0x00)	:
				zPrintf(1, "\n [0x0800] PM1 : Wake-up source : 3sec");
				HAL_PowerdownMode1(3, 0, 0, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x01)	:
				zPrintf(1, "\n [0x0801] PM1 : Wake-up source : P1.7");
				HAL_PowerdownGpio1WakeupSourceSet(7, POWER_WAKEUP_ENABLE, POWER_WAKEUP_FALLING_EDGE);
				HAL_PowerdownMode1(3, 0, 0, POWER_WAKEUP_ONLY_GPIO);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x02)	:
				zPrintf(1, "\n [0x0802] PM1 : Wake-up source : Ext0");
				zPrintf(1, "\n Select Edge or Level");
				zPrintf(1, "\n ==================== ");
				zPrintf(1, "\n 0 = Low Level");
				zPrintf(1, "\n 1 = Falling Edge");
				zPrintf(1, "\n 2 = High Level");
				zPrintf(1, "\n 3 = Rising Edge");
				zPrintf(1, "\n ==================== ");
				zPrintf(1, "\n Input (0~3) :");
				zScanf(1, &u16EdgeTest);
				if(u16EdgeTest == 0)		zPrintf(1, "\n Low Level Test");
				else if(u16EdgeTest == 1)	zPrintf(1, "\n Falling Edge Test");
				else if(u16EdgeTest == 2)	zPrintf(1, "\n High Level Test");
				else if(u16EdgeTest == 3)	zPrintf(1, "\n Rising Edge Test");
				else
				{
					u16EdgeTest = 0;
					zPrintf(1, "\n Invalid Input");
					zPrintf(1, "\n Low Level Test");
				}
				zPrintf(1, " : %u", u16EdgeTest);
				zPrintf(1, "\n Power Down");
				while(SYS_Uart1TxWorking());
				HAL_Ext0IntSet(1, 0, u16EdgeTest);

				HAL_PowerdownMode1(0, 0, 1, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x03)	:
				zPrintf(1, "\n [0x0803] PM1 : Wake-up source : EXT1");
				HAL_Ext1IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_PowerdownMode1(0, 0, 1, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x04)	:
				zPrintf(1, "\n [0x0804] PM1 : Wake-up source : EXT0/1 & P0.7");
				HAL_PowerdownGpio0WakeupSourceSet(7, POWER_WAKEUP_ENABLE, POWER_WAKEUP_FALLING_EDGE);
				HAL_Ext0IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_Ext1IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_PowerdownMode1(0, 0, 1, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x05)	:
				zPrintf(1, "\n [0x0805] PM1 : Tx after Wake-up");
				zPrintf(1, "\n >> PM1");
				HAL_PowerdownMode1(0, 150, 0, 0);
				zPrintf(1, " => Wakeup");

				zPrintf(1, "\n >> Transmit");
				u8Status = TestLib_PacketTransmit();
				zPrintf(1, " Status=0x%02x", (short)u8Status);
				break;
			//==========================================================================
			case (POWER_CODE | 0x10)	:
				zPrintf(1, "\n [0x0810] PM2 : Wake-up source : 3sec");
				HAL_PowerdownMode2(3, 0, 0, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x11)	:
				zPrintf(1, "\n [0x0811] PM2 : Wake-up source : P0.7");
				HAL_PowerdownGpio0WakeupSourceSet(7, POWER_WAKEUP_ENABLE, POWER_WAKEUP_FALLING_EDGE);
				HAL_PowerdownMode2(3, 0, 0, POWER_WAKEUP_ONLY_GPIO);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x12)	:
				zPrintf(1, "\n [0x0812] PM2 : Wake-up source : EXT0");
				HAL_Ext0IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_PowerdownMode2(0, 0, 1, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x13)	:
				zPrintf(1, "\n [0x0813] PM2 : Wake-up source : EXT1");
				HAL_Ext1IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_PowerdownMode2(0, 0, 1, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x14)	:
				zPrintf(1, "\n [0x0814] PM2 : Wake-up source : EXT0/1 & P0.7");
				HAL_PowerdownGpio0WakeupSourceSet(7, POWER_WAKEUP_ENABLE, POWER_WAKEUP_FALLING_EDGE);
				HAL_Ext0IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_Ext1IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_PowerdownMode2(0, 0, 1, 0);
				zPrintf(1, " => Wakeup");
				break;
			//==========================================================================
			case (POWER_CODE | 0x20)	:
				zPrintf(1, "\n [0x0820] PM3 : Wake-up source : EXT0");
				HAL_Ext0IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_PowerdownMode3(0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x21)	:
				zPrintf(1, "\n [0x0821] PM3 : Wake-up source : EXT1");
				HAL_Ext1IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_PowerdownMode3(0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x22)	:
				zPrintf(1, "\n [0x0822] PM3 : Wake-up source : EXT0/1");
				HAL_Ext0IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_Ext1IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_PowerdownMode3(0);
				zPrintf(1, " => Wakeup");
				break;
			//==========================================================================
			case (POWER_CODE | 0x30)	:
				zPrintf(1, "\n [0x0830] Idle : Wake-up source : UART1");
				zPrintf(1, "\n >> Press any key for wake-up");
				while(SYS_Uart1TxWorking());		// Waiting for completing UART-TX in hardware
				HAL_McuIdleMode(POWER_IDLE_WAKEUP_UART1, 0, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x31)	:
				zPrintf(1, "\n [0x0831] Idle : Wake-up source : TIMER0(98ms)");
				HAL_Timer0ClockOn(ON);
				HAL_Timer0IntSet(ENABLE, PRIORITY_LOW, RUN, 98);
				HAL_McuIdleMode(POWER_IDLE_WAKEUP_TIMER0, 0, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x32)	:
				zPrintf(1, "\n [0x0832] Idle : Wake-up source : TIMER1(98ms)");
				HAL_Timer1ClockOn(ON);
				HAL_Timer1IntSet(ENABLE, PRIORITY_LOW, RUN, 98);
				HAL_McuIdleMode(POWER_IDLE_WAKEUP_TIMER1, 0, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x33)	:
				zPrintf(1, "\n [0x0833] Idle : Wake-up source : EXT0/1");
				HAL_Ext0IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_Ext1IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_McuIdleMode( (POWER_IDLE_WAKEUP_EXT0 | POWER_IDLE_WAKEUP_EXT1), 0, 0);
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x34)	:
				zPrintf(1, "\n [0x0834] Idle : Wake-up source : PHY");
				zPrintf(1, "\n >> A packet should be received for wake-up");
				HAL_McuIdleMode(0, POWER_IDLE_WAKEUP_PHY, 0);
				zPrintf(1, " => Wakeup");
				break;
			//==========================================================================
			case (POWER_CODE | 0x40)	:
				zPrintf(1, "\n [0x0840] RtcInt : 1sec period");
				zPrintf(1, "\n >> P0.2 is toggled in ST_ISR per 1second");
				HAL_RtcIntSet(ENABLE, PRIORITY_LOW, 1, 0);
				break;
			//==========================================================================
			case (POWER_CODE | 0x50)	:
				zPrintf(1, "\n [0x0850] RF Sleep");
				HAL_RFAnalogSleep(POWER_RF_SLEEP_ADC_ON);
				zPrintf(1, " => OK");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x51)	:
				zPrintf(1, "\n [0x0851] RF Wake-up");
				HAL_RFAnalogWakeup();
				zPrintf(1, " => Wakeup");
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x52)	:
				zPrintf(1, "\n [0x0852] RF Sleep continuously : Tx after Wake-up by"); zPrintf(1, " UART1 & EXT0/1");
				zPrintf(1, "\n >> Wake-up source : UART1 & EXT0/1");
				HAL_Ext0IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				HAL_Ext1IntSet(ENABLE, PRIORITY_LOW, EXTI_TYPE_LOW_LEVEL);
				u8AgingTest = 1;
				break;
			//--------------------------------------------------------------------------------------------
			case (POWER_CODE | 0x53)	:
				zPrintf(1, "\n [0x0853] RF Sleep continuously : OFF");
				u8AgingTest = 0;
				break;
			//==========================================================================
			case (POWER_CODE | 0xFF)	:
				zPrintf(1, "\n [0x08FF] Disable all Wake-up sources");
				HAL_PowerdownGpio0WakeupSourceSet(7, POWER_WAKEUP_DISABLE, POWER_WAKEUP_DISABLE);
				HAL_Ext0IntSet(DISABLE, 0, 0);
				HAL_Ext1IntSet(DISABLE, 0, 0);
				zPrintf(1, " => OK");
				break;
			//========================================================================== 
			default	: u8LineFeedDisplayFlag = 0;		break;
			//========================================================================== 
		}
		if(u8LineFeedDisplayFlag)	zPrintf(1, "\n");
		u16VerifyCmdID = 0;

		if(u8AgingTest == 1)
		{
			HAL_RFAnalogSleep(POWER_RF_SLEEP_ADC_ON);
			zPrintf(1, "\n >> Now Idle state. Press any key or EXT0/1 for wake-up");
			while(SYS_Uart1TxWorking());		// Waiting for completing UART-TX in hardware
			HAL_McuIdleMode((POWER_IDLE_WAKEUP_EXT0 | POWER_IDLE_WAKEUP_EXT1 | POWER_IDLE_WAKEUP_UART1), 0, 0);				
			HAL_RFAnalogWakeup();
			zPrintf(1, " => Wakeup\n");
			TestLib_PacketTransmit();			
		}
		// TODO: END
		//
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Reports the HAL code(Higher 2bytes) and the return code(Lower 2bytes) if a error is occured when calling HAL
///	functions. This is called only in DEBUG mode.
///
///	@param	u16ErrorCode		: bit[31:16]=HAL code. bit[15:0]=Return code.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
#ifdef  DEBUG
void assert_failed(UINT16 u16ErrorCode)
{
	// TODO:  The user can add his own implementation to report the error code. e.g. zPrintf().
#ifdef __MG2470_UART1_INCLUDE__
	if(ES1 && UART1_CLK_STATE)
	{
		zPrintf(1, "\n ## ErrorCode = 0x%04x ##", u16ErrorCode);
	}
#endif	
}
#endif





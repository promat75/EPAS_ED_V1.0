
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
#include "exti.h"
#include "irtx_NEC.h"
#include "i2c.h"
#include "spi.h"
#include "gpioint.h"
#include "flash.h"

#include "util_app.h"
#include "util_sys.h"
#include "app_flash.h"

#include "ISR.c"

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
	UINT8	u8Status;
	UINT8	u8KeyIn;
	UINT16	iw;

	tHW_INFORMATION	tDataHIB;

	INT16	i16AdcRead;

	UINT16	u16VerifyCmdID = 0;
	UINT16	u16PreviousVerifyCmdID = 0;

	UINT8	u8LineFeedDisplayFlag = 0;

	tNWK_INFO	tNwkInfoSet;
	tNWK_INFO	tNwkInfoGet;

	UINT8	au8FlashBuffer[512];

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
	zPrintf(1, "\n >> EVK Application Start");
	zPrintf(1, "\n");

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
	// 	Set HIB Information to MIB.
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
	// When HIB is invalid, Network Parameters are selected by keyboard input.
	else
	{	
		zPrintf(1, "\n #########################################");
		zPrintf(1, "\n >> MIB Auto Set !");
		zPrintf(1, "\n #########################################");
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

	zPrintf(1, "\n");
	zPrintf(1, "\n >> MG2470B - [FLASH] Sample Start");

	zPrintf(1, "\n");
	zPrintf(1, "\n ===>> HELP ===============================");
	zPrintf(1, "\n h : HELP Menu");
	zPrintf(1, "\n a : Again Previous VerifyCmd");
	zPrintf(1, "\n ==========================================");
	zPrintf(1, "\n");

	while(1)
	{		
		SYS_WdtSet(10000);		

		if(SYS_Uart1Get(&u8KeyIn))
		{
			if(u8KeyIn == 'h')
			{	
				zPrintf(1, "\n ===>> HELP ===============================");
				zPrintf(1, "\n h : HELP Menu");
				zPrintf(1, "\n a : Again Previous VerifyCmd");
				zPrintf(1, "\n ------------------------------------");
				zPrintf(1, "\n [0x0401] Save Network Info to FLASH");
				zPrintf(1, "\n [0x0402] Recall Network Info from FLASH");
				zPrintf(1, "\n [0x0403] Recall Network Info from FLASH");
				zPrintf(1, "\n [0x0404] Delete Network Info in FLASH");
				zPrintf(1, "\n [0x0405] Page(512 Bytes) Write : 0x00, 01, ...");
				zPrintf(1, "\n [0x0406] Page(512 Bytes) Write : 0x80, 81, ...");
				zPrintf(1, "\n [0x0407] Page(512 Bytes) Read");
				zPrintf(1, "\n =========================================");
				zPrintf(1, "\n >> Verification Command ID : 0x");
				zScanf(1, &u16VerifyCmdID);
				zPrintf(1, " => OK");
				zPrintf(1, "\n");
				u16PreviousVerifyCmdID = u16VerifyCmdID;
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

		//2 Function Test
		u8LineFeedDisplayFlag = 1;
		switch(u16VerifyCmdID)
		{
			//================================================================
			case (FLASH_CODE | 0x01) :
				zPrintf(1, "\n [0x0401] Save Network Info to FLASH");
				
				tNwkInfoSet.u8Channel = 0x10;
				tNwkInfoSet.u16PanID = 0x2021;
				tNwkInfoSet.u16ShortAddr = 0x3031;
				for(iw=0 ; iw<8 ; iw++)	tNwkInfoSet.au8IEEEAddr[iw] = 0x40 + iw;
				tNwkInfoSet.u8Reserved0 = 0x50;
				tNwkInfoSet.u8Reserved1 = 0x51;
				zPrintf(1, "\n >> AppLib_FlashDataDisplay()");
				AppLib_FlashDataDisplay(&tNwkInfoSet);
				
				zPrintf(1, "\n >> AppLib_FlashDataWrite()");				
				u8Status = AppLib_FlashDataWrite(&tNwkInfoSet);
				zPrintf(1, " => Status=0x%02x", (short)u8Status);
				break;
			//-----------------------------------------------------------------------
			case (FLASH_CODE | 0x02) :
				zPrintf(1, "\n [0x0402] Recall Network Info from FLASH");
				
				zPrintf(1, "\n >> AppLib_FlashDataRead()");
				AppLib_FlashDataRead(&tNwkInfoGet);
				zPrintf(1, " => OK");				
				
				zPrintf(1, "\n >> AppLib_FlashDataDisplay()");
				AppLib_FlashDataDisplay(&tNwkInfoGet);
				break;				
			//-----------------------------------------------------------------------
			case (FLASH_CODE | 0x03) :
				zPrintf(1, "\n [0x0403] Recall Network Info from FLASH");				
				
				zPrintf(1, "\n >> AppLib_FlashDataRead_2()");
				AppLib_FlashDataRead_2(&tNwkInfoGet);
				zPrintf(1, " => OK");				
				
				zPrintf(1, "\n >> AppLib_FlashDataDisplay()");
				AppLib_FlashDataDisplay(&tNwkInfoGet);
				break;
			//-----------------------------------------------------------------------
			case (FLASH_CODE | 0x04) :
				zPrintf(1, "\n [0x0404] Delete Network Info in FLASH");
				zPrintf(1, "\n >> AppLib_FlashDataDelete()");
				u8Status = AppLib_FlashDataDelete();
				zPrintf(1, " => Status=0x%02x", (short)u8Status);
				break;
			//-----------------------------------------------------------------------
			case (FLASH_CODE | 0x05) :
				zPrintf(1, "\n [0x0405] Page(512 Bytes) Write : 0x00, 01, ...");
				
				for(iw=0 ; iw<512 ; iw++)
				{
					au8FlashBuffer[iw] = iw;
				}

				zPrintf(1, "\n >> Data to write");
				Display_Buffer(au8FlashBuffer, 512, 32);
				
				zPrintf(1, "\n >> HAL_FlashPageWrite()");
				HAL_FlashWriteUnprotected();
				u8Status = HAL_FlashPageWrite(CODE_ADDRESS_USER0, au8FlashBuffer, 0);
				HAL_FlashWriteProtected();
				zPrintf(1, " => Status=0x%02x", (short)u8Status);


				zPrintf(1, "\n >> HAL_FlashRead()");
				u8Status = HAL_FlashRead(CODE_ADDRESS_USER0, au8FlashBuffer, 512);
				zPrintf(1, " => Status=0x%02x", (short)u8Status);
								
				zPrintf(1, "\n >> Read Data");
				Display_Buffer(au8FlashBuffer, 512, 32);
				break;
			//-----------------------------------------------------------------------
			case (FLASH_CODE | 0x06) :
				zPrintf(1, "\n [0x0406] Page(512 Bytes) Write : 0x80, 81, ...");
				
				for(iw=0 ; iw<512 ; iw++)
				{
					au8FlashBuffer[iw] = iw + 0x80;
				}

				zPrintf(1, "\n >> Data to write");
				Display_Buffer(au8FlashBuffer, 512, 32);
				
				zPrintf(1, "\n >> HAL_FlashPageWrite()");
				HAL_FlashWriteUnprotected();
				u8Status = HAL_FlashPageWrite(CODE_ADDRESS_USER0, au8FlashBuffer, 0);
				HAL_FlashWriteProtected();
				zPrintf(1, " => Status=0x%02x", (short)u8Status);


				zPrintf(1, "\n >> HAL_FlashRead()");
				u8Status = HAL_FlashRead(CODE_ADDRESS_USER0, au8FlashBuffer, 512);
				zPrintf(1, " => Status=0x%02x", (short)u8Status);
								
				zPrintf(1, "\n >> Read Data");
				Display_Buffer(au8FlashBuffer, 512, 32);
				break;
			//-----------------------------------------------------------------------
			case (FLASH_CODE | 0x07) :
				zPrintf(1, "\n [0x0407] Page(512 Bytes) Read");

				zPrintf(1, "\n >> HAL_FlashRead()");
				u8Status = HAL_FlashRead(CODE_ADDRESS_USER0, au8FlashBuffer, 512);
				zPrintf(1, " => Status=0x%02x", (short)u8Status);
								
				zPrintf(1, "\n >> Read Data");
				Display_Buffer(au8FlashBuffer, 512, 32);
				break;
			//================================================================
			default	: u8LineFeedDisplayFlag = 0;		break;
			//================================================================			
		}
		if(u8LineFeedDisplayFlag)	zPrintf(1, "\n");
		u16VerifyCmdID = 0;

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

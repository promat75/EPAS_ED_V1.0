
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
#include "option_security.h"

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
#include "security.h"

#include "util_app.h"
#include "util_sys.h"

typedef struct
{
	UINT8	u8AckReq;
	UINT16	u16Interval;
	UINT8	u8PacketLen;
	UINT16	u16NumToTx;
	UINT8	u8PacketPattern;

	UINT16	u16ResultTxNum;
	UINT16	u16ResultOK;	
	UINT16	u16ResultChanFail;
	UINT16	u16ResultNoAck;

	UINT16	u16Retry1st;
	UINT16	u16Retry2nd;
	UINT16	u16Retry3rd;
	
	UINT8	u8DisplayPktInfo;
	UINT16	u16RxCount;
	UINT16	u16MissCount;
	UINT16	u16DuplicationCount;
} tPER_PARAMETER;

#include "ISR.c"

UINT8 gu8Expected_CipherText[68] = 
	{
		0xD9, 0x07, 0x7E, 0x18, 0xB7, 0x9F, 0x22, 0x49, 0x4C, 0x42, 0x08, 0xB1, 0x59, 0xBB, 0x06, 0x88,
		0x15, 0x72, 0x49, 0xC0, 0x95, 0xF4, 0x6B, 0x10, 0x10, 0xF4, 0x36, 0x43, 0x98, 0x13, 0xCC, 0x5A,
		0x5D, 0x3B, 0x2D, 0xAB, 0xA1, 0x40, 0x0D, 0x00, 0xDC, 0xB7, 0x4F, 0x85, 0x11, 0x3C, 0xE1, 0x1B,
		0x85, 0x46, 0x19, 0x81, 0xE4, 0x5C, 0xC8, 0x0E, 0x4A, 0xE3, 0x16, 0x3E, 0x16, 0x72, 0x12, 0xCB,
		0xEC, 0x15, 0x05, 0x95
	};

UINT8 gu8Expected_PlainText[64] = 
	{
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F
	};

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
	UINT16	u16ScanIn;
	UINT16	iw;

	tHW_INFORMATION	tDataHIB;

	UINT8	u8DeviceMode;
	UINT16	u16LedCounter_Run = 0;
	UINT8	u8LedCounter_Tx = 0;
	UINT8	u8LedCounter_Rx = 0;
	INT16	i16AdcRead;
	
	tPACKET_INFO*	ptRxPacket;
	tMAC_INFO		tMacInfo;
	
	tPER_PARAMETER		tPER;
	UINT8	au8MSDU[127];
	tMAC_DATA_REQ		tMacDataReq;

	UINT8	u8LineFeedDisplayFlag;
	UINT16	u16VerifyCmdID = 0;
	UINT16	u16PreviousVerifyCmdID = 0;

	UINT8	au8Key0[16];
	UINT8	au8NonceExtAddr[8];
	UINT8	au8Text[128];
	UINT32	u32FrameCounter;
	UINT8	u8KeySeqNum;
	UINT8	u8PlainLen;
	UINT8	u8CipherLen;

	UINT8	u8SecModeEnable = 0;

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
	HAL_Gpio3InOutSet(7, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP37 = 1;

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

	//	Initialize structure variable for PER parameter
	tPER.u8AckReq = 1;
	tPER.u16ResultChanFail = 0;
	tPER.u16ResultNoAck = 0;
	tPER.u16ResultOK = 0;
	tPER.u16ResultTxNum = 0;	
	tPER.u8PacketLen = 127;
	tPER.u16Interval = 10;
	tPER.u16MissCount = 0;
	tPER.u16NumToTx = 1000;
	tPER.u16Retry1st = 0;
	tPER.u16Retry2nd = 0;
	tPER.u16Retry3rd = 0;
	tPER.u16RxCount = 0;
	tPER.u8DisplayPktInfo = 1;
	tPER.u8PacketPattern = 1;

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
	else
	{
		// When HIB is invalid, Network Parameters are selected by keyboard input.
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
#if (_OPTION_EXT_AMP_CONTROL == 2)
	zPrintf(1, "\n");
	zPrintf(1, "\n >> External Amplifier Control Signals are enabled.");
	zPrintf(1, "\n >> GPIO 1.7 is TRSW. (TX=High, RX=Low)");
	zPrintf(1, "\n >> GPIO 1.6 is nTRSW. (TX=Low, RX=High)");
	zPrintf(1, "\n");
	HAL_AmpCtrlGpio1Set(1);
#elif (_OPTION_EXT_AMP_CONTROL == 1)
	zPrintf(1, "\n");
	zPrintf(1, "\n >> External Amplifier Control Signals are enabled.");
	zPrintf(1, "\n >> GPIO 3.7 is TRSW. (TX=High, RX=Low)");
	zPrintf(1, "\n >> GPIO 3.6 is nTRSW. (TX=Low, RX=High)");
	zPrintf(1, "\n");
	HAL_AmpCtrlSignalSet(1, 0);
#else
	zPrintf(1, "\n");
	zPrintf(1, "\n >> No External Amplifier");
#endif

	// TODO: Application-Specific Initialization
	zPrintf(1, "\n");
	zPrintf(1, "\n >> MG2470B - [SECURITY] Sample Start");

	zPrintf(1, "\n");
	zPrintf(1, "\n ===>> HELP ===============================");
	zPrintf(1, "\n h : HELP Menu");
	zPrintf(1, "\n a : Again Previous VerifyCmd");
	zPrintf(1, "\n ==========================================");
	zPrintf(1, "\n 1 : TX Option");
	zPrintf(1, "\n 2 : PER Parameter");
	zPrintf(1, "\n 3 : Data Pattern");
	zPrintf(1, "\n ==========================================");
	zPrintf(1, "\n b : Address");
	zPrintf(1, "\n c : Channel");
	zPrintf(1, "\n d : Data Rate");
	zPrintf(1, "\n i : PAN ID");
	zPrintf(1, "\n p : Transmit Power");
	zPrintf(1, "\n ==========================================");
	zPrintf(1, "\n");

	while(1)
	{		
		SYS_WdtSet(10000);		

		//
		// PLL can be unlocked when the temperature is changed rapidly within short time.
		// Then, re-locking is required.
		HAL_LockStateCheck();
		//

		u16LedCounter_Run = (u16LedCounter_Run + 1) & 0x07FF;
		if(u16LedCounter_Run == 0)
		{
			GP03 = ~GP03;
		}
		
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
				u8LedCounter_Rx = (u8LedCounter_Rx + 1) & 0x07;
				if(u8LedCounter_Rx == 0)	GP00 = ~GP00;

				//1 Security Processing
				if(u8SecModeEnable == 1)
				{
					GP37 = 0;
					u8Status = HAL_SecDecryptionCCM32(tMacInfo.pu8MacPayload, tMacInfo.u8MacPayLen, u32FrameCounter, u8KeySeqNum, &u8PlainLen);					
					GP37 = 1;
					tMacInfo.u8MacPayLen = u8PlainLen;
				}

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
				zPrintf(1, "\n 1 : TX Option");
				zPrintf(1, "\n 2 : PER Parameter");
				zPrintf(1, "\n 3 : Data Pattern");
				zPrintf(1, "\n ==========================================");
				zPrintf(1, "\n b : Address");
				zPrintf(1, "\n c : Channel");
				zPrintf(1, "\n d : Data Rate");
				zPrintf(1, "\n i : PAN ID");
				zPrintf(1, "\n p : Transmit Power");
				zPrintf(1, "\n ==========================================");
				zPrintf(1, "\n h : HELP Menu");
				zPrintf(1, "\n a : Again Previous VerifyCmd");
				zPrintf(1, "\n ------------------------------------");
				zPrintf(1, "\n [0x0a01] Encrypt & Decrypt");
				zPrintf(1, "\n [0x0a02] Security Mode : ON");
				zPrintf(1, "\n [0x0a03] Security Mode : OFF");
				zPrintf(1, "\n [0x0a04] Transmit one packet");
				zPrintf(1, "\n ==========================================");
				zPrintf(1, "\n >> Verification Command ID : 0x");
				zScanf(1, &u16VerifyCmdID);
				zPrintf(1, " => OK");
				zPrintf(1, "\n");
				u16PreviousVerifyCmdID = u16VerifyCmdID;
			}
			else if(u8KeyIn == '1')
			{
				zPrintf(1, "\n >> [PER] Retry Option");
			
				//	Retry for NO ACK
				zPrintf(1, "\n >> Input NoAckRetry : 0x");
				zScanfByte(1, &gtMIB.u8NoAckRetry);
				zPrintf(1, " =>OK");

				//	Retry for Channel Access Failure
				zPrintf(1, "\n >> Input BackoffRetry : 0x");
				zScanfByte(1, &gtMIB.u8BackoffRetry);
				zPrintf(1, " =>OK");
				zPrintf(1, "\n");
			}			
			else if(u8KeyIn == '2')
			{
				zPrintf(1, "\n >> [PER] Parameter");

				//	Number of packets
				zPrintf(1, "\n >> Input the number of packets to transmit (0 ~ 65535) : ");
				zScanf_Decimal(1, &tPER.u16NumToTx);
				zPrintf(1, " =>OK");									

				//	Interval
				zPrintf(1, "\n >> Input the interval between two packets (0 ~ 65535, ms) : ");
				zScanf_Decimal(1, &tPER.u16Interval);
				zPrintf(1, " =>OK");					

				// 	Packet's Length	
				zPrintf(1, "\n >> Input the length of a packet (11 ~ 127) : ");
				zScanf_Decimal(1, &u16ScanIn);
				tPER.u8PacketLen = u16ScanIn & 0x7F;
				zPrintf(1, " =>OK");
								
				//	Ack Request
				zPrintf(1, "\n >> Input ACK request (0 or 1) : 0x");
				zScanfByte(1, &tPER.u8AckReq);
				zPrintf(1, " =>OK");
				zPrintf(1, "\n");				
			}
			else if(u8KeyIn == '3')
			{
				zPrintf(1, "\n #########################################");
				zPrintf(1, "\n 0x00 : 0x00 00 00 ...");
				zPrintf(1, "\n 0x01 : 0x00 01 02 ...");				
				zPrintf(1, "\n 0xFF : 0xFF FF FF ...");
				zPrintf(1, "\n #########################################");				
				zPrintf(1, "\n Input the pattern of a packet : 0x");
				zScanfByte(1, &tPER.u8PacketPattern);
				zPrintf(1, " =>OK");
				zPrintf(1, "\n");
			}
			else if(u8KeyIn == 'b')
			{
				zPrintf(1, "\n >> Current Source Address = 0x%04x", gtMIB.u16ShortAddr);
				zPrintf(1, "\n >> Input Source Address : 0x");	
				zScanf(1, &gtMIB.u16ShortAddr);
				HAL_ShortAddrSet(gtMIB.u16ShortAddr);
				zPrintf(1, " =>OK");

				zPrintf(1, "\n >> Current Destination Address = 0x%04x", gtMIB.u16DstShortAddr);
				zPrintf(1, "\n >> Input Destination Address : 0x");
				zScanf(1, &gtMIB.u16DstShortAddr);
				zPrintf(1, " =>OK");
				zPrintf(1, "\n");
			}
			else if(u8KeyIn == 'c')
			{
				zPrintf(1, "\n #########################################");
				zPrintf(1, "\n Ch11 : 2405 MHz     Ch19 : 2445 MHz");				
				zPrintf(1, "\n Ch12 : 2410 MHz     Ch20 : 2450 MHz");
				zPrintf(1, "\n Ch13 : 2415 MHz     Ch21 : 2455 MHz");
				zPrintf(1, "\n Ch14 : 2420 MHz     Ch22 : 2460 MHz");
				zPrintf(1, "\n Ch15 : 2425 MHz     Ch23 : 2465 MHz");
				zPrintf(1, "\n Ch16 : 2430 MHz     Ch24 : 2470 MHz");
				zPrintf(1, "\n Ch17 : 2435 MHz     Ch25 : 2475 MHz");
				zPrintf(1, "\n Ch18 : 2440 MHz     Ch26 : 2480 MHz");
				zPrintf(1, "\n #########################################");
				zPrintf(1, "\n >> Current Channel = %u", (short)gtMIB.u8Channel);
				zPrintf(1, "\n >> Input Channel (11 ~ 26) : ");
				zScanf_Decimal(1, &u16ScanIn);
				if( (u16ScanIn < 11) || (u16ScanIn > 26) )
				{
					zPrintf(1, " => Invalid Range");
				}
				else
				{
					zPrintf(1, " => OK");
					gtMIB.u8Channel = u16ScanIn;
					HAL_ChannelSet(gtMIB.u8Channel);
				}
				zPrintf(1, "\n");
			}
			else if(u8KeyIn == 'd')
			{
				zPrintf(1, "\n #########################################");
				zPrintf(1, "\n  0 : 31.25 Kbps");
				zPrintf(1, "\n  1 : 62.50 Kbps");
				zPrintf(1, "\n  2 : 125 Kbps");
				zPrintf(1, "\n  3 : 250 Kbps");
				zPrintf(1, "\n  5 : 1.0 Mbps");
				zPrintf(1, "\n #########################################");
				zPrintf(1, "\n >> Current DataRate = %u", (short)gtMIB.u8DataRate);
				zPrintf(1, "\n >> Input DataRate (0 ~ 3, 5) : ");
				zScanf_Decimal(1, &u16ScanIn);
				if( (u16ScanIn > 5) || (u16ScanIn == 4) )
				{
					zPrintf(1, " => Invalid Range");
				}
				else
				{
					zPrintf(1, " => OK");
					gtMIB.u8DataRate = u16ScanIn;
					HAL_DataRateSet(gtMIB.u8DataRate);
				}
				zPrintf(1, "\n");
			}
			else if(u8KeyIn == 'p')
			{
				zPrintf(1, "\n #########################################");
				zPrintf(1, "\n  0 : 9.0 dBm(Max)");
				zPrintf(1, "\n  4 : 5.0 dBm");
				zPrintf(1, "\n  9 : 0.0 dBm");
				zPrintf(1, "\n 14 : -5.0 dBm");
				zPrintf(1, "\n 18 : -10.0 dBm");
				zPrintf(1, "\n 19 : -23.0 dBm");
				zPrintf(1, "\n 22 : -64.0 dBm");
				zPrintf(1, "\n #########################################");
				zPrintf(1, "\n >> Current Transmit Power Level = %u", (short)gtMIB.u8TxPowerLevel);
				zPrintf(1, "\n >> Input Transmit Power Level (0 ~ 22) : ");
				zScanf_Decimal(1, &u16ScanIn);
				if(u16ScanIn > 22)
				{
					zPrintf(1, " => Invalid Range");
				}
				else
				{
					zPrintf(1, " => OK");
					gtMIB.u8TxPowerLevel = u16ScanIn;
					HAL_TxPowerSet(gtMIB.u8TxPowerLevel);
				}
				zPrintf(1, "\n");
			}	
			else if(u8KeyIn == 'i')
			{
				zPrintf(1, "\n >> Current PAN ID = 0x%04x", (short)gtMIB.u16PanID);
				zPrintf(1, "\n >> Input PAN ID : 0x");	
				zScanf(1, &gtMIB.u16PanID);
				HAL_PanIDSet(gtMIB.u16PanID);
				gtMIB.u16DstPanID = gtMIB.u16PanID;
				zPrintf(1, " =>OK");
				zPrintf(1, "\n");
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
			//=========================================================
			case (SEC_CODE | 0x01) :
				zPrintf(1, "\n [0x0a01] Encrypt & Decrypt");
				zPrintf(1, "\n Input Plain Len :");
				zScanf(1, &u16ScanIn);

				//2 ENC & DEC : Common Security Materials
				for(iw=0 ; iw<16 ; iw++)	au8Key0[iw] = 0xA0 + iw;
				for(iw=0 ; iw<8 ; iw++)	au8NonceExtAddr[iw] = 0xB0 + iw;
				u32FrameCounter = 0x11223344;
				u8KeySeqNum = 0x55;

				//2 ENC : KEY0
				HAL_SecKey0Set(au8Key0);
				HAL_SecTxKeyChoice(0);

				//2 ENC : TX NONCE's Extended Address
				HAL_SecTxNonceExtAddrSet(au8NonceExtAddr);

				//2 ENC : Length of Plain Text
				u8PlainLen = u16ScanIn;

				//2 ENC :  Plain Text
				memcpy(au8Text, gu8Expected_PlainText, u16ScanIn);

				//2 ENC :  Display Plain Text
				zPrintf(1, "\n");
				zPrintf(1, "\n >> Input Plain Text : Len=%u", u16ScanIn);
				Display_Buffer(au8Text, u16ScanIn, 32);
				
				//2 ENC :  Encryption
				zPrintf(1, "\n >> Encryption Start");
				u8Status = HAL_SecEncryptionCCM32(au8Text, u8PlainLen, u32FrameCounter, u8KeySeqNum, &u8CipherLen);
				zPrintf(1, " => Status=0x%02x", (short)u8Status);

				//2 ENC : Display Cipher Text
				zPrintf(1, "\n >> Output Cipher Text : Len=%u", (short)u8CipherLen);
				Display_Buffer(au8Text, u8CipherLen, 32);
#if 0
				//2 ENC : Display Expected Text
				zPrintf(1, "\n >> Expected Text : Len=%u", (short)u8CipherLen);
				Display_Buffer(gu8Expected_CipherText, u8CipherLen, 32);

				//2 ENC : Compare Cipher & Expected
				zPrintf(1, "\n >> Compre : Cipher & Expected");
				if(memcmp(au8Text, gu8Expected_CipherText, (u16ScanIn+4)))
				{
					zPrintf(1, " => Failed");
				}
				else
				{
					zPrintf(1, " => OK");
				}
#endif

				//2 DEC : KEY0
				HAL_SecRxKeyChoice(0);

				//2 DEC : RX NONCE's Extended Address
				HAL_SecRxNonceExtAddrSet(au8NonceExtAddr);

				//2 DEC :  Display Cipher Text
				zPrintf(1, "\n");
				zPrintf(1, "\n >> Input Cipher Text : Len=%u", (short)u8CipherLen);
				Display_Buffer(au8Text, u8CipherLen, 32);

				//2 DEC : Decryption
				zPrintf(1, "\n >> Decryption Start");
				u8Status = HAL_SecDecryptionCCM32(au8Text, u8CipherLen, u32FrameCounter, u8KeySeqNum, &u8PlainLen);
				zPrintf(1, " => Status=0x%02x", (short)u8Status);

				//2 DEC : Display Plain Text
				zPrintf(1, "\n >> Output Plain Text : Len=%u", (short)u8PlainLen);
				Display_Buffer(au8Text, (u16ScanIn), 32);

				//2 DEC : Display Expected Text
				zPrintf(1, "\n >> Expected Text : Len=%u", u16ScanIn);
				Display_Buffer(gu8Expected_PlainText, u16ScanIn, 32);

				//2 DEC : Compare Plain & Expected
				zPrintf(1, "\n >> Compre : Expected & Encrypted");
				if(memcmp(au8Text, gu8Expected_PlainText, u16ScanIn))
				{
					zPrintf(1, " => Failed");
				}
				else
				{					
					zPrintf(1, " => OK");
				}
				break;
			//--------------------------------------------------------------------------
			case (SEC_CODE | 0x02) :
				zPrintf(1, "\n [0x0a02] Security Mode : ON");
				zPrintf(1, " => To transmit a packet, VerifyCmdID '0x0a04'");

				u8SecModeEnable = 1;

				// Security Material
				for(iw=0 ; iw<16 ; iw++)	au8Key0[iw] = 0xA0 + iw;
				for(iw=0 ; iw<8 ; iw++)	au8NonceExtAddr[iw] = 0xB0 + iw;
				u32FrameCounter = 0x11223344;
				u8KeySeqNum = 0x55;

				HAL_SecKey0Set(au8Key0);
				HAL_SecTxKeyChoice(SEC_KEY0);
				HAL_SecRxKeyChoice(SEC_KEY0);

				HAL_SecTxNonceExtAddrSet(au8NonceExtAddr);
				HAL_SecRxNonceExtAddrSet(au8NonceExtAddr);
				break;
			//--------------------------------------------------------------------------
			case (SEC_CODE | 0x03) :
				zPrintf(1, "\n [0x0a03] Security Mode : OFF");
				zPrintf(1, " => To transmit a packet, VerifyCmdID '0x0a04'");
				u8SecModeEnable = 0;
				break;				
			//--------------------------------------------------------------------------
			case (SEC_CODE | 0x04) :
				zPrintf(1, "\n [0x0a04] Transmit one packet");
				for(iw=0 ; iw<maccMaxPHYPacketSize ; iw++)
				{
					if(tPER.u8PacketPattern == 0x00)
					{
						au8MSDU[iw] = 0x00;
					}
					else if(tPER.u8PacketPattern == 0xFF)
					{
						au8MSDU[iw] = 0xFF;
					}
					else
					{
						au8MSDU[iw] = iw;
					}
				}
				tMacDataReq.u8DstMode = maccAddrMode_16bit;
				tMacDataReq.u16DstPanID = gtMIB.u16DstPanID;
				tMacDataReq.uDstAddr.u16Short = gtMIB.u16DstShortAddr;
				tMacDataReq.u8SrcMode = maccAddrMode_16bit;
				tMacDataReq.u8TxOption = tPER.u8AckReq & 0x01;
				tMacDataReq.u8MsduLen = tPER.u8PacketLen - 9 - 2 -4;
				tMacDataReq.pu8Msdu = au8MSDU;

				//2 Security Processing
				if(u8SecModeEnable == 1)
				{
					GP37 = 0;
					u8Status = HAL_SecEncryptionCCM32(tMacDataReq.pu8Msdu, tMacDataReq.u8MsduLen, u32FrameCounter, u8KeySeqNum, &u8CipherLen);
					GP37 = 1;
					tMacDataReq.u8MsduLen = u8CipherLen;
				}
				
				zPrintf(1, "\n >> AppLib_MacDataRequest(16bit --> 16bit)");
				u8Status = AppLib_MacDataRequest(&tMacDataReq);
				zPrintf(1, " => Sta=0x%02x",(short)u8Status);
				zPrintf(1, "\n");
				break;
			//=========================================================
			default	: u8LineFeedDisplayFlag = 0;		break;
			//=========================================================
		}
		u16VerifyCmdID = 0;
		if(u8LineFeedDisplayFlag)	zPrintf(1, "\n");

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


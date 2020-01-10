
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2013-12-20
	- Version		: V2.1

	[2013-12-20] V2.1
	- HAL_TxState() : removed.
	- HAL_TxReady() : added.
	- HAL_TxFifoTransmit() : enhanced.
	- HAL_AckTransmit() : HAL_TxState() function removed.
	- HAL_RFTestMode() : HAL_TxState() function removed.

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "phy.h"
#include "wdt.h"
#include "clock.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether PHY interrupt is generated. Each bit has following meanings.
///		\n 	bit[7]	: RX-ACK. If an expected ACK packet is received, this bit is set to 1.
///		\n	bit[6:5]	: reserved
///		\n	bit[4]	: Modem-Fail. If modem is not turned on, this bit is set to 1.
///		\n	bit[3]	: reserved
///		\n	bit[2]	: RX-Start. If a packet is detected to be received, this bit is set to 1.
///		\n 	bit[1]	: TX-End. If transmitting a packet is completed, this bit is set to 1.
///		\n	bit[0]	: Modem-On. If Modem is turned on, this bit is set to 1.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8PhyIntFlag = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Global variable which indicates the current RF's data rate
///		\n	0 = 31.25 Kbps
///		\n	1 = 62.50 Kbps
///		\n	2 = 125 Kbps
///		\n	3 = 250 Kbps
///		\n	4 = reserved
///		\n	5 = 1.0 Mbps
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8DataRate = PHY_DATA_RATE_250K;

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Optional global variable which indicates whether the received packet is contained in lower area of RX-FIFO.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8   gu8RxInLowQ = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Constants which indicates the for() loop counter for waiting to complete transmitting a max-sized DATA packet.
///	\n	The loop counter is dependent on the RF data rate and MCU clock. Therefore, this is two-dimensional arrays.
///	\n	The column of the array indicates the RF data rate.
///			\n	0 = 31.25 Kbps
///			\n	1 = 62.50 Kbps
///			\n	2 = 125 Kbps
///			\n	3 = 250 Kbps
///			\n	4 = reserved
///			\n	5 = 1.0 Mbps
///	\n	The row of the array indicates the MCU clock mode.
///			\n 	0 = 16 MHz
///			\n 	1 = 8 MHz
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	code gcau16MaxWait_DataTxDone[6][2] = 		// [RF Data Rate][MCU Clock Mode]
	{
		5478, 	2739,	// 31.25 Kbps
		2738,	1369,	// 62.5 Kbps
		1716,	858,		// 125 Kbps
		860,		430,		// 250 Kbps
		450,		225,		// reserved
		260,		130		// 1 Mbps
	};

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Constants which indicates the for() loop counter for waiting to receive a ACK packet.
///	\n	The loop counter is dependent on the RF data rate and MCU clock. Therefore, this is two-dimensional arrays.
///	\n	The column of the array indicates the RF data rate.
///			\n	0 = 31.25 Kbps
///			\n	1 = 62.50 Kbps
///			\n	2 = 125 Kbps
///			\n	3 = 250 Kbps
///			\n	4 = reserved
///			\n	5 = 1.0 Mbps
///	\n	The row of the array indicates the MCU clock mode.
///			\n 	0 = 16 MHz
///			\n 	1 = 8 MHz
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	code gcau16MaxWait_AckRxDone[6][2] = 		// [RF Data Rate][MCU Clock Mode]
	{
		466,		233,		// 31.25 Kbps
		274,		137,		// 62.5 Kbps
		178,		89,		// 125 Kbps
		138,		69,		// 250 Kbps
		138,		69,		// reserved
		138,		69		// 1 Mbps
	};

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Constants which indicates the for() loop counter for waiting to complete transmitting a ACK packet.
///	\n	The loop counter is dependent on the RF data rate and MCU clock. Therefore, this is two-dimensional arrays.
///	\n	The column of the array indicates the RF data rate.
///			\n	0 = 31.25 Kbps
///			\n	1 = 62.50 Kbps
///			\n	2 = 125 Kbps
///			\n	3 = 250 Kbps
///			\n	4 = reserved
///			\n	5 = 1.0 Mbps
///	\n	The row of the array indicates the MCU clock mode.
///			\n 	0 = 16 MHz
///			\n 	1 = 8 MHz
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	code gcau16MaxWait_AckTxDone[6][2] = 		// [RF Data Rate][MCU Clock Mode]
	{
		560,		280,		// 31.25 Kbps
		280,		140,		// 62.5 Kbps
		150,		75,		// 125 Kbps
		70 ,		35,		// 250 Kbps
		60 ,		30,		// reserved
		60 ,		30		// 1 Mbps
	};

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Initializes registers on Modem, Analog and RF blocks.
///
///	@param	u8InitOption	: Option for initialization
///				\n bit[7:1]	: reserved
///				\n bit[0] 		: 1=Minimize interference, 0=Normal.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_ModemInit(UINT8 u8InitOption)
{
	if(u8InitOption & BIT0)
	{
		LIB_ModemInit(BIT0);
	}
	else
	{
		LIB_ModemInit(0x00);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Turns off Modem.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_ModemOff(void)
{
	REGCMD_MODEM_OFF;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Turns on Modem.
///
///	@param	u16WaitModemOnCnt	: time delay parameter till Modem is turned on.
///	@param	u8DccCalibOn			: DCC calibration. 0=Not performed, 1=Performed.
///								  Calibration is performed 1 time in booting sequence.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_ModemOn(UINT16 u16WaitModemOnCnt, UINT8 u8DccCalibOn)
{
	UINT8	u8EA;
	UINT8	u8xINTCON;
//	UINT8	u8xPCMD0;

	static	UINT16	su16Loop;
	UINT8	u8IntNum;
	UINT8	u8Status;

	if(u8DccCalibOn)
	{
		xPHY_CLK_FR_EN1 = 0x00;
		xDCCCON = 0x88;
	}

	u8EA = EA;
	EA = 0;

	u8xINTCON = xINTCON;
//	u8xPCMD0 = xPCMD0;

	HAL_PhyIntSourceSet(PHY_INT_MASK_MODEM_ON | PHY_INT_MASK_MODEM_FAIL);

	REGCMD_MODEM_ON;		// bit[4]=0, Modem-On, active low.

	u8Status = RETURN_PHY_MODEM_ON_TIMEOUT;
	for(su16Loop=0 ; su16Loop<u16WaitModemOnCnt ; su16Loop++)
	{
		if( (xINTCON & BIT6) == 0)		// If interrupt is occured,
		{
			u8IntNum = xINTIDX & PHY_INT_IDX_MASK;
			if(u8IntNum == PHY_INT_IDX_MODEM_ON)
			{
				u8Status = RETURN_PHY_SUCCESS;
				break;
			}
			else if(u8IntNum == PHY_INT_IDX_MODEM_FAIL)
			{
				u8Status = RETURN_PHY_MODEM_ON_FAILURE;
				break;
			}
		}
	}

	xINTSTS = 0xFF;
	xINTCON = u8xINTCON;

	EA = u8EA;

	if(u8DccCalibOn)
	{	
		if(u8Status == RETURN_PHY_SUCCESS)
		{
			xDCCCON = 0x84;
		}
	}

	if(u8Status)
	{
		assert_error(PHY_CODE | u8Status);
	}
	return u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets several registers after turning on Modem.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_ModemPost(void)
{
	HAL_PhyIntSourceSet(PHY_INT_MASK_RX_END | PHY_INT_MASK_TX_END);
	HAL_CCAModeSet(1, 0);	// CCA : Enable, Window Size=16us, Mode=0(ED)
	HAL_CorrelationSet(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets RF's data rate.
///
///	@param	u8DataRate	: Data rate to set.
///							\n	0 = 31.25 Kbps
///							\n	1 = 62.50 Kbps
///							\n	2 = 125 Kbps
///							\n	3 = 250 Kbps
///							\n	4 = reserved
///							\n	5 = 1.0 Mbps
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_DataRateSet(UINT8 u8DataRate)
{
	if(u8DataRate == 4)
	{
		assert_error(PHY_CODE | RETURN_PHY_INVALID_DATA_RATE);
		return RETURN_PHY_INVALID_DATA_RATE;
	}

	if(u8DataRate > PHY_DATA_RATE_1000K)
	{
		assert_error(PHY_CODE | RETURN_PHY_INVALID_DATA_RATE);
		return RETURN_PHY_INVALID_DATA_RATE;
	}

	REGCMD_TX_CANCEL;
	HAL_RxEnable(0);

	LIB_DataRateSet(u8DataRate);

	switch(u8DataRate)
	{	
		case PHY_DATA_RATE_31K		: xSETRATE = 0x09;	break;
		case PHY_DATA_RATE_62K		: xSETRATE = 0x05;	break;
		case PHY_DATA_RATE_125K	: xSETRATE = 0x03;	break;
		case PHY_DATA_RATE_250K	: xSETRATE = 0x02;	break;
		case PHY_DATA_RATE_1000K	: xSETRATE = 0x10;	break;
	}
	REGCMD_TX_CANCEL_CLEAR;
	HAL_RxEnable(1);

	gu8DataRate = u8DataRate;

	return RETURN_PHY_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets RF's data rate. This should be called only in ISR(Interrupt Service Routine).
///
///	@param	u8DataRate	: Data rate to set.
///							\n	0 = 31.25 Kbps
///							\n	1 = 62.50 Kbps
///							\n	2 = 125 Kbps
///							\n	3 = 250 Kbps
///							\n	4 = reserved
///							\n	5 = 1.0 Mbps
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_DataRateSet_ISR(UINT8 u8DataRate)
{
	LIB_DataRateSet(u8DataRate);

	switch(u8DataRate)
	{	
		case PHY_DATA_RATE_31K		: xSETRATE = 0x09;	break;
		case PHY_DATA_RATE_62K		: xSETRATE = 0x05;	break;
		case PHY_DATA_RATE_125K	: xSETRATE = 0x03;	break;
		case PHY_DATA_RATE_250K	: xSETRATE = 0x02;	break;
		case PHY_DATA_RATE_1000K	: xSETRATE = 0x10;	break;
	}

	gu8DataRate = u8DataRate;

	return RETURN_PHY_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Enables or disables Multiple Data Rate mode. If receiving packets with various data rate
///		over 250Kbps is needed, this should be called.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_MultiDataRateSet(UINT8 u8On)
{
	HAL_DataRateSet(PHY_DATA_RATE_250K);

	if(u8On)
	{
		xCORCNF4 |= BIT6;
		xPHY_MPTOP_CLK_SEL |= BIT3;
		xAGCCNF7 |= BIT7;
	}
	else
	{
		xCORCNF4 &= ~(BIT6 | BIT7);
		xPHY_MPTOP_CLK_SEL &= ~BIT3;
		xAGCCNF7 &= ~BIT7;
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the RF channel by number defined by IEEE 802.15.4.
///
///	@param	u8ChanNum	: Number of RF channel.
///							\n	11 = 2405 MHz.
///							\n	12 = 2410 MHz.
///							\n	13 = 2415 MHz.
///							\n	14 = 2420 MHz.
///							\n	15 = 2425 MHz.
///							\n	16 = 2430 MHz.
///							\n	17 = 2435 MHz.
///							\n	18 = 2440 MHz.
///							\n	19 = 2445 MHz.
///							\n	20 = 2450 MHz.
///							\n	21 = 2455 MHz.
///							\n	22 = 2460 MHz.
///							\n	23 = 2465 MHz.
///							\n	24 = 2470 MHz.
///							\n	25 = 2475 MHz.
///							\n	26 = 2480 MHz.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_ChannelSet(UINT8 u8ChanNum)
{
	UINT8	u8Status;

	if( (u8ChanNum < 11) || (u8ChanNum > 26) )
	{
		assert_error(PHY_CODE | RETURN_PHY_INVALID_CHANNEL);
		return RETURN_PHY_INVALID_CHANNEL;
	}

	REGCMD_TX_CANCEL;
	HAL_RxEnable(0);

	JP_FREQ_SET:

	switch(u8ChanNum)
	{
		case 11	: u8Status = LIB_FrequencySet(2405);	break;
		case 12	: u8Status = LIB_FrequencySet(2410);	break;
		case 13	: u8Status = LIB_FrequencySet(2415);	break;
		case 14	: u8Status = LIB_FrequencySet(2420);	break;
		case 15	: u8Status = LIB_FrequencySet(2425);	break;
		case 16	: u8Status = LIB_FrequencySet(2430);	break;
		case 17	: u8Status = LIB_FrequencySet(2435);	break;
		case 18	: u8Status = LIB_FrequencySet(2440);	break;
		case 19	: u8Status = LIB_FrequencySet(2445);	break;
		case 20	: u8Status = LIB_FrequencySet(2450);	break;
		case 21	: u8Status = LIB_FrequencySet(2455);	break;
		case 22	: u8Status = LIB_FrequencySet(2460);	break;
		case 23	: u8Status = LIB_FrequencySet(2465);	break;
		case 24	: u8Status = LIB_FrequencySet(2470);	break;
		case 25	: u8Status = LIB_FrequencySet(2475);	break;
		case 26	: u8Status = LIB_FrequencySet(2480);	break;
	}

	if(u8Status)
	{
		assert_error(PHY_CODE | u8Status);
		goto JP_FREQ_SET;
	}

	REGCMD_TX_CANCEL_CLEAR;
	HAL_RxEnable(1);

	if(u8Status)
	{
		assert_error(PHY_CODE | u8Status);
	}
	return u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the energy detection(ED) level.
///
///	@param	void
///	@return	INT8. Value of energy detection level. This is 2's complementary value.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
INT8 HAL_EnergyLevelGet(void)
{
	INT8		i8dBm;
	UINT8	ib;

	for(ib = 0; ib < 250; ib++)
	{
		i8dBm = xAGCSTS2;
		if(i8dBm != 0)
		{
			break;
		}
	}

	return	i8dBm;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Enables/Disables the source of PHY interrupts. If a bit is set to 1, the corresponding interrupt source is enabled.
///
///	@param	u8IntSource	: Interrupt source
///						\n	bit[7:5]	: reserved
///						\n	bit[4]	: If set to 1, Modem-Fail interrupt is enabled. This interrupt has lowest priority.
///						\n	bit[3]	: If set to 1, RX-End interrupt is enabled. This interrupt has low priority.
///						\n	bit[2]	: If set to 1, RX-Start interrupt is enabled. This interrupt has medium priority.
///						\n	bit[1]	: If set to 1, TX-End interrupt is enabled. This interrupt has high priority.
///						\n	bit[0]	: If set to 1, Modem-On interrupt is enabled. This interrupt has highest priority.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_PhyIntSourceSet(UINT8 u8IntSource)
{
	xINTCON = u8IntSource & 0x1F;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Enables/Disables the PHY interrupt.
///
///	@param	u8IntEna	: 0=Disable, 1=Enable.
///	@param	u8Priority	: Priority of interrupt. 0=Low, 1=High.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_PhyIntSet(UINT8 u8IntEna, UINT8 u8Priority)
{
	UINT8	u8EA;

	u8EA = EA;
	EA = 0;

	PHYIP = u8Priority;
	PHYIE = u8IntEna;

	EA = u8EA;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Stops or resumes receiving operations
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_RxEnable(UINT8 u8Ena)
{
	UINT8	u8EA;

	if(u8Ena)
	{
		REGCMD_PHY_INT_RESET_ON;
		REGCMD_PHY_INT_RESET_OFF;
		MCU_INT_FLAG_CLEAR_PHY;
		u8EA = EA;
		EA = 0;
		xMRFCSTA = 0x00;
		xMRFCWP = 0;
		xMRFCRP = 0;
		EA = u8EA;
		gu8RxInLowQ = 0; // Double FIFO
		REGCMD_RSM_RESET_ON;
		REGCMD_RSM_RESET_OFF;
		REGCMD_RX_ON;
	}
	else
	{
		REGCMD_RX_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the system interrupt(EA) of 8051.
///
///	@param	u8EA	: Value of EA to set. 0=Disable, 1=Enable.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SystemIntSet(UINT8 u8EA)
{
	EA = u8EA;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets several registers for TX.
///
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_TxReady(void)
{
	LIB_TxReady();
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether transmitting is undergoing.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8TxOperating = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Transmits a packet in TXFIFO. The starting address of the packet is 0x00.
///
///	@param	void
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_TxFifoTransmit(void)
{
	static	UINT16	su16MaxLoopCnt;
	static	UINT16	su16Loop;
	UINT8	u8Status;
	UINT8	u8RXSTS;

	xMTFCRP = 0;
	xMACDSN = xMTxFIFO(0+3);

	//
	// Check Modem Status before transmitting.
	//
	u8RXSTS = xPHYSTS0 & 0x70;
	if( (u8RXSTS != 0x00) && (u8RXSTS != 0x10) )
	{
		return RETURN_PHY_MODEM_BUSY;
	}
	//

	gu8TxOperating = 1;
	gu8PhyIntFlag &= ~(PHY_FLAG_MASK_TX_END | PHY_FLAG_MASK_RX_ACK);

	u8Status = RETURN_PHY_CCA_FAILURE;

	HAL_TxReady();
	REGCMD_TX_REQ;				// bit[2]=0

	su16MaxLoopCnt = gcau16MaxWait_DataTxDone[gu8DataRate][gu8McuPeriClockMode];
	for(su16Loop=0 ; su16Loop < su16MaxLoopCnt ; su16Loop++)
	{
		if(gu8PhyIntFlag & PHY_FLAG_MASK_TX_END)
		{
			u8Status = RETURN_PHY_SUCCESS;
			break;
		}
	}

	if(u8Status == RETURN_PHY_CCA_FAILURE)
	{
		REGCMD_TX_CANCEL;		// bit[3]=0
		xMTFCRP = 0;
		gu8TxOperating = 0;

		return RETURN_PHY_CCA_FAILURE;
	}

	gu8TxOperating = 0;

	u8Status = RETURN_PHY_SUCCESS;
	if(xMTxFIFO(0+1) & 0x20)			// If AckRequest=1
	{
		u8Status = RETURN_PHY_NO_ACK_FAILURE;
		su16MaxLoopCnt = gcau16MaxWait_AckRxDone[gu8DataRate][gu8McuPeriClockMode];
		for(su16Loop=0 ; su16Loop < su16MaxLoopCnt ; su16Loop++)
		{
			if(gu8PhyIntFlag & PHY_FLAG_MASK_RX_ACK)
			{
				u8Status = RETURN_PHY_SUCCESS;
				break;
			}
		}
	}

	xMTFCRP = 0;

	return u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Transmits an ACK packet.
///
///	@param	u8FramePend	: If this is 1, FramePend subfield of FrameControl field in ACK packet is set to 1.
///						\n	0 = Pended frame is not present.
///						\n	1 = Pended frame is present.
///	@param	u8DSN		: Value for sequence number field in ACK packet.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_AckTransmit(UINT8 u8FramePend, UINT8 u8DSN)
{
	static	UINT16	su16MaxLoopCnt;
	static	UINT16	su16Loop;
	UINT8	u8RXSTS;
	UINT8	u8CCA0;

	if(gu8TxOperating)
	{
		return;
	}

	xMTxFIFO(0x80) = 0x05;
	xMTxFIFO(0x81) = 0x02;
	if(u8FramePend)
	{
		xMTxFIFO(0x81) |= 0x10;
	}
	xMTxFIFO(0x82) = 0x00;
	xMTxFIFO(0x83) = u8DSN;	

	u8CCA0 = xCCA0;
	xCCA0 |= BIT5;		// bit[5]=1. Disable
	xMTFCRP = 0x80;

	//
	// Check Modem Status before transmitting.
	//
	u8RXSTS = xPHYSTS0 & 0x70;
	if( (u8RXSTS != 0x00) && (u8RXSTS != 0x10) )
	{
		goto JP_HAL_AckTransmit;
	}
	//


	REGCMD_TX_REQ;				// bit[2]=0
	su16MaxLoopCnt = gcau16MaxWait_AckTxDone[gu8DataRate][gu8McuPeriClockMode];
	for(su16Loop=0 ; su16Loop < su16MaxLoopCnt ; su16Loop++);
	REGCMD_TX_CANCEL;			// bit[3]=0

	JP_HAL_AckTransmit:

	xMTFCRP = 0x00;
	xCCA0 = u8CCA0;			// Enable CCA
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Enables/Disables CCA function. And, Sets CCA's mode.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u8CcaMode	: CCA mode. 0=ED, 1=CD, 2=FD, 3=Reserved.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_CCAModeSet(UINT8 u8Ena, UINT8 u8CcaMode)
{
	if(u8CcaMode > PHY_CCA_MODE_FD)
	{
		assert_error(PHY_CODE | RETURN_PHY_INVALID_PARAMETER);
		return RETURN_PHY_INVALID_PARAMETER;
	}	

	if(u8Ena)
	{
		xCCA0 = (0x10 | u8CcaMode);
	}
	else
	{
		xCCA0 = 0x30;
	}

	return RETURN_PHY_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets threshold of CCA. The packet is transmitted when energy detection level is lower than CCA threshold.
///
///	@param	i8Threshold_dBm	: The threshold value in dBm. This is 2's complementary value.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_CCAThresholdSet(INT8 i8Threshold_dBm)
{
	xCCA1 = i8Threshold_dBm;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Enables/Disables Correlation Indicator. The correlation value is acquired by reading LQICNF1 register(0x227F)
///		after receiving a packet.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_CorrelationSet(UINT8 u8Ena)
{
	if(u8Ena)
	{
		xLQICNF0 |= BIT3;
	}
	else
	{
		xLQICNF0 &= ~BIT3;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets mode for RF testing.
///
///	@param	u8TestMode	: Test Mode.
///						\n	0 = Disable test mode.
///						\n	1 = CW(Continuous Wave). Carrier Signal Generation.
///						\n	2 = CM(Continuous Modulated Spectrum). Modulated Signal Generation without 
///								IFS(Inter-Frame-Space)
///						\n	3 = CM(Continuous Modulated Spectrum). Modulated Signal Generation with 
///								IFS(Inter-Frame-Space)
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_RFTestMode(UINT8 u8TestMode)
{
	UINT8	u8Status;
	static	UINT8	u8CurrentTestMode = 0;

	if(u8TestMode > 3)
	{
		assert_error(PHY_CODE | RETURN_PHY_INVALID_PARAMETER);
		return RETURN_PHY_INVALID_PARAMETER;
	}

	if( (u8TestMode == 0) && (u8CurrentTestMode == 0) )
	{
		assert_error(PHY_CODE | RETURN_PHY_ILLEGAL_REQUEST);
		return RETURN_PHY_ILLEGAL_REQUEST;
	}	
	
	u8Status = RETURN_PHY_SUCCESS;
	if(u8TestMode == PHY_RF_TEST_CW)		// Continuous Wave(CW). Unmodulated Carrier.
	{
		HAL_ModemOff();
		LIB_RFTestMode(PHY_RF_TEST_CW);
		u8CurrentTestMode = PHY_RF_TEST_CW;
	}
	else if(u8TestMode == PHY_RF_TEST_CM_WITHOUT_IFS)	// Continuous Modulated Spectrum(CM) without IFS(Inter Frame Space)
	{
		HAL_ModemOff();
		LIB_RFTestMode(PHY_RF_TEST_CM_WITHOUT_IFS);
		u8CurrentTestMode = PHY_RF_TEST_CM_WITHOUT_IFS;
	}
	else if(u8TestMode == PHY_RF_TEST_CM_WITH_IFS)	// Continuous Modulated Spectrum(CM) with IFS(Inter Frame Space)
	{
		HAL_ModemOff();
		LIB_RFTestMode(PHY_RF_TEST_CM_WITH_IFS);
		u8CurrentTestMode = PHY_RF_TEST_CM_WITH_IFS;
	}	
	else
	{		
		LIB_RFTestMode(PHY_RF_TEST_DISABLE);
		u8Status = HAL_ModemOn(10000, 0);
		u8CurrentTestMode = 0;
	}

	return u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the transmitting power.
///
///	@param	u8PowerLevel	: level of transmitting power
///						\n	0 = 9.0 dBm.
///						\n	1 = 8.0 dBm.
///						\n	2 = 7.0 dBm.
///						\n	3 = 6.0 dBm.
///						\n	4 = 5.0 dBm.
///						\n	5 = 4.0 dBm.
///						\n	6 = 3.0 dBm.
///						\n	7 = 2.0 dBm.
///						\n	8 = 1.0 dBm.
///						\n	9 = 0.0 dBm.
///						\n	10 = -1 dBm.
///						\n	11 = -2 dBm.
///						\n	12 = -3 dBm.
///						\n	13 = -4 dBm.
///						\n	14 = -5 dBm.
///						\n	15 = -7 dBm.
///						\n	16 = -8 dBm.
///						\n	17 = -9 dBm.
///						\n	18 = -10 dBm.
///						\n	19 = -23 dBm.
///						\n	20 = -27 dBm.
///						\n	21 = -33 dBm.
///						\n	22 = -64 dBm.

///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_TxPowerSet(UINT8 u8PowerLevel)
{
	if(u8PowerLevel > 22)
	{
		assert_error(PHY_CODE | RETURN_PHY_INVALID_PARAMETER);
		return RETURN_PHY_INVALID_PARAMETER;
	}

	LIB_TxPowerSet(u8PowerLevel);

	return RETURN_PHY_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets signals to control an external amplifier when it is used. There are two kinds of signals.
///		\n TRSW is high in transmitting and low in receiving.
///		\n nTRSW is reversed TRSW. It's low in transmitting and high in receiving.
///		\n GPIO3_6 and GPIO3_7 are used for those and the polarity is dependent on "u8Polarity".
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u8Polarity	: Polarity of GPIO which is used for TRSW and nTRSW.
///		\n	0 = GPIO3_7 is TRSW. GPIO3_6 is nTRSW.
///		\n	1 = GPIO3_7 is nTRSW. GPIO3_6 is TRSW.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_AmpCtrlSignalSet(UINT8 u8Ena, UINT8 u8Polarity)
{
	if(u8Ena)
	{
		LIB_AmpCtrlSignalSet(u8Polarity);
		P3SRC_SEL |= (BIT6 | BIT7);
	}
	else
	{
		P3SRC_SEL &= ~(BIT6 | BIT7);
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets signals to control an external amplifier when it is used. There are two kinds of signals.
///		\n GPIO1_7 is TRSW. This is high in transmitting and low in receiving.
///		\n GPIO1_6 is nTRSW. This is reversed TRSW. It's low in transmitting and high in receiving.
///
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_AmpCtrlGpio1Set(UINT8 u8Ena)
{
	if(u8Ena)
	{
		LIB_AmpCtrlGpio1Set();
		P1SRC_SEL |= (BIT6 | BIT7);
	}
	else
	{
		P1SRC_SEL &= ~(BIT6 | BIT7);
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Checks the state of PLL locking. If it is not locked, make it to be locked again.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_LockStateCheck(void)
{
	UINT16	iw;
	UINT8	u8Status;

	if( (xPHYSTS1 & 0x1F) > 2)
	{
		if( (xPLLMON & BIT0) == 0)		// If PLL is unlocked
		{
			u8Status = 1;
			while(1)
			{
				xPLLCTRL |= BIT0;		// Re-lock
				for(iw=0 ; iw<1000 ; iw++)
				{
					if(xPLLMON & BIT0)
					{
						u8Status = 0;
						break;
					}
				}

				if(u8Status == 0)
				{
					break;
				}
			}
		}
	}
}


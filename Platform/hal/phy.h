
/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_PHY_H__
#define __MG2470_PHY_H__

#define	PHY_DATA_RATE_31K		0
#define	PHY_DATA_RATE_62K		1
#define	PHY_DATA_RATE_125K		2
#define	PHY_DATA_RATE_250K		3
#define	PHY_DATA_RATE_1000K		5

#define	PHY_CCA_MODE_ED	0
#define	PHY_CCA_MODE_CD	1
#define	PHY_CCA_MODE_FD	2

#define	PHY_FLAG_MASK_MODEM_ON		0x01
#define	PHY_FLAG_MASK_TX_END			0x02
#define	PHY_FLAG_MASK_RX_START		0x04
#define	PHY_FLAG_MASK_RX_END			0x08
#define	PHY_FLAG_MASK_MODEM_FAIL	0x10
#define	PHY_FLAG_MASK_TX_FAIL		0x20
#define	PHY_FLAG_MASK_RX_ACK			0x80

#define	PHY_RF_TEST_DISABLE			0
#define	PHY_RF_TEST_CW				1
#define	PHY_RF_TEST_CM_WITHOUT_IFS	2
#define	PHY_RF_TEST_CM_WITH_IFS		3

#define	PHY_CODE	0x0700

#define	RETURN_PHY_SUCCESS				0x00
#define	RETURN_PHY_INVALID_PARAMETER	0x11
#define	RETURN_PHY_MODEM_ON_FAILURE	0x12
#define	RETURN_PHY_MODEM_ON_TIMEOUT	0x13
#define	RETURN_PHY_LOCK_FAILURE			0x14
#define	RETURN_PHY_MODEM_BUSY			0x15
#define	RETURN_PHY_ILLEGAL_REQUEST		0x16
#define	RETURN_PHY_INVALID_CHANNEL		0x17
#define	RETURN_PHY_INVALID_DATA_RATE	0x18
#define	RETURN_PHY_CCA_FAILURE			0xE1
#define	RETURN_PHY_NO_ACK_FAILURE		0xE9

extern	UINT8	gu8PhyIntFlag;
extern	UINT8	gu8DataRate;
extern	UINT8   gu8RxInLowQ;

void HAL_ModemInit(UINT8 u8InitOption);
void HAL_ModemOff(void);
UINT8 HAL_ModemOn(UINT16 u16WaitModemOnCnt, UINT8 u8DccCalibOn);
void HAL_ModemPost(void);
UINT8 HAL_DataRateSet(UINT8 u8DataRate);
UINT8 HAL_DataRateSet_ISR(UINT8 u8DataRate);
void HAL_MultiDataRateSet(UINT8 u8On);
UINT8 HAL_ChannelSet(UINT8 u8ChanNum);
INT8 HAL_EnergyLevelGet(void);
void HAL_PhyIntSourceSet(UINT8 u8IntSource);
void HAL_PhyIntSet(UINT8 u8IntEna, UINT8 u8Priority);
void HAL_RxEnable(UINT8 u8Ena);
void HAL_SystemIntSet(UINT8 u8EA);
UINT8 HAL_TxFifoTransmit(void);
void HAL_AckTransmit(UINT8 u8FramePend, UINT8 u8DSN);
UINT8 HAL_CCAModeSet(UINT8 u8Ena, UINT8 u8CcaMode);
void HAL_CCAThresholdSet(INT8 i8Threshold_dBm);
void HAL_CorrelationSet(UINT8 u8Ena);
UINT8 HAL_RFTestMode(UINT8 u8TestMode);
UINT8 HAL_TxPowerSet(UINT8 u8PowerLevel);
void HAL_AmpCtrlSignalSet(UINT8 u8Ena, UINT8 u8Polarity);
void HAL_AmpCtrlGpio1Set(UINT8 u8Ena);
void HAL_LockStateCheck(void);

#endif	// #ifndef __MG2470_PHY_H__



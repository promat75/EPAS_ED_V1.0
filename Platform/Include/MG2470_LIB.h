

/*******************************************************************************
	[2013-12-20] Compatible with MG2470_LIB_V24.LIB	
*******************************************************************************/

#ifndef __MG2470_LIB_H__
#define __MG2470_LIB_H__

void LIB_ModemInit(UINT8 u8RegOption);
void LIB_DataRateSet(UINT8 u8DataRate);
UINT8 LIB_FrequencySet(UINT16 u16Freq_MHz);
UINT16 LIB_FrequencyGet(void);
void LIB_TxPowerSet(UINT8 u8PowerLevel);
void LIB_RFTestMode(UINT8 u8TestMode);
void LIB_AmpCtrlSignalSet(UINT8 u8Polarity);
void LIB_AmpCtrlGpio1Set();
void LIB_FlashParking(UINT8 u8Parking);
void LIB_TxReady(void);
UINT8 code* LIB_TagInfoGet(void);

#endif	// #ifndef __MG2470_LIB_H__




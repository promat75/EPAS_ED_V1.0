
/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_HMAC_H__
#define __MG2470_HMAC_H__

#define	HMAC_CODE	0x0600

void HAL_MacCtrlSet(UINT8 u8MacCtrl);
void HAL_AutoCrcSet(UINT8 u8Ena);
void HAL_AddrDecodeSet(UINT8 u8Ena);
void HAL_CoordinatorSet(UINT8 u8Ena);
void HAL_PanIDSet(UINT16 u16PanID);
void HAL_ShortAddrSet(UINT16 u16ShortAddr);
void HAL_IEEEAddrSet(UINT8* pu8IEEEAddrBuf);
void HAL_TxFifoReset(void);
void HAL_RxFifoReset(void);
void HAL_MacInit(void);

#endif	// #ifndef __MG2470_HMAC_H__


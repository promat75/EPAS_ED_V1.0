
/*******************************************************************************
	[2013-01-14] Compatible with V2.1
*******************************************************************************/

#ifndef __MG2470_UTIL_APP_H__
#define __MG2470_UTIL_APP_H__

#include "IEEE_802_15_4.h"
#include "HIB.h"

#define	KEYBOARD_BACK			0x08
#define	KEYBOARD_CarryRet			0x0D
#define	KEYBOARD_LineFeed		0x0A
#define	KEYBOARD_SPACE			0x20

extern	tMAC_INFO_BASE	gtMIB;

extern	UINT16	gu16VirtualTimer0;
extern	UINT16	gu16VirtualTimer1;
extern	UINT16	gu16VirtualTimer2;
extern	UINT16	gu16VirtualTimer3;
extern	UINT16	gu16VirtualTimer4;


void zPrintf(UINT8 u8Port, char *fmt, ...);
void zScanf(UINT8 u8Port, UINT16* pu16InValue);
void zScanfByte(UINT8 u8Port, UINT8* pu8InValue);
UINT8 zScanf_Decimal(UINT8 u8Port, UINT16 *pu16InValue);

void AppLib_VirtualTimerSet(UINT8 u8VirtualTimerNum, UINT16 u16Timer0Tick);
UINT16 AppLib_VirtualTimerGet(UINT8 u8VirtualTimerNum);
UINT8 AppLib_MacDataFrameMake(tMAC_DATA_REQ* ptMacDataReq, UINT8* pu8Psdu, UINT8* pu8PsduLen);
UINT8 AppLib_TxFifoTransmit(UINT8 u8NoAckRetry, UINT8 u8BackoffRetry);
UINT8 AppLib_MacDataRequest(tMAC_DATA_REQ* ptMacDataReq);

void AppLib_DelayFor1us(void);
void AppLib_DelayFor2us(void);
void AppLib_DelayFor10us(void);
void AppLib_DelayFor50us(void);
void AppLib_DelayFor500us(void);
void AppLib_DelayForus(UINT16 SBMilliSecond);

void AppLib_DelayFor100us(void);
void AppLib_DelayFor1ms(void);
void AppLib_Delay(UINT16 u16MilliSecond);

void Display_Buffer(UINT8* pu8Buff, UINT16 u16Len, UINT8 u8Linefeed);
void Display_MIB(tMAC_INFO_BASE* ptMIB);
void Display_MacInfo(tMAC_INFO* ptMac);
void Display_HIB(tHW_INFORMATION* ptHIB);

#endif	// #ifndef __MG2470_UTIL_APP_H__








/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/

#ifndef __MG2470_UTIL_SYS_H__
#define __MG2470_UTIL_SYS_H__

#define	RF_RXBUF_SIZE			10			// more than 1

typedef struct
{
	UINT8	au8Packet[128];
	INT8		i8RSSI;
	UINT8	u8Correlation;
} tPACKET_INFO;


INT8 SYS_LQITodBm(UINT8 u8LQI);
UINT8 SYS_dBmToLQI(INT8 i8dBm);

extern	tPACKET_INFO	gatRxQPacket[];
extern	UINT8		gu8RxQUsed;
extern	UINT8		gu8RxQWr;
extern	UINT8		gu8RxQRd;

tPACKET_INFO* SYS_RxQueueCheck(void);
void SYS_RxQueueClear(void);
UINT8 SYS_RxQueueParse(tPACKET_INFO* ptInPkt, tMAC_INFO *ptOutMac);

#endif	// #ifndef __MG2470_UTIL_SYS_H__



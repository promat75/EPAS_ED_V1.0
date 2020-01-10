
/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/

#ifndef __HIB_H__
#define __HIB_H__

typedef struct
{
	UINT8	au8IEEEAddr[8];		// IEEE Address
	UINT8	u8ChipID;
	UINT8	u8TxPower;			// 0x00 ~ 0xFF
	//-	u8DataRate
	//	0	= 31.25 Kbps
	//	1	= 62.50 Kbps
	//	2	= 125 Kbps
	//	3	= 250 Kbps
	//	4	= reserved
	//	5	= 1.0 Mbps
	UINT8	u8DataRate;
	//- u8StackID
	//	0x00	: None
	//	0x01	: IEEE 802.15.4
	//	0x02	: RF4CE
	//	0x10	: ZigBee 2004
	//	0x11	: ZigBee 2005
	//	0x12	: ZigBee 2006
	//	0x13	: ZigBee 2007/PRO
	UINT8	u8StackID;			// 0x10=ZigBee 2004, 0x11=2005, 0x12=2006, 0x13=2007
	UINT8	u8Channel;			// 0x0B~0x1A
	UINT8	au8PanID[2];			// PanID[0]=[7:0], PanID[1]=[15:8]
	UINT8	au8NwkAddr[2];		// NwkAddr[0]=[7:0], NwkAddr[1]=[15:8]
	UINT8	u8SecurityLevel;		// 0=No, 1=MIC32, 2=MIC64, 3=MIC128, 4=ENC, 5=ENCMIC32, 6=ENCMIC64, 7=ENCMIC128
	UINT8	u8PreConfigMode;		// 0x00 ~ 0xFF
	UINT8	au8NwkKey[16];		// Nwk Security Key
	UINT8	au8Reserved_0[8];		// reserved
	UINT8	au8EPID[8];			// Extended Pan ID	
	UINT8	au8Reserved_1[8];		// reserved
	UINT8	au8GeneralWord0[2];
	UINT8	au8GeneralWord1[2];
	UINT8	u8CheckSum;			// SUM(IEEE_ADDR + Reserved_1) + CheckSum = 0x00
} tHW_INFORMATION;

#define	HIB_ADDRESS			0x1000	// If HPIB is used, this value should not be modified.
#define	HIB_LENGTH			64

UINT8 SYS_HIBCopy(UINT8* pu8Buff);

#endif		// #ifndef __HIB_H__
	

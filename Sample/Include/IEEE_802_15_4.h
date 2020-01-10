

#ifndef __IEEE_802_15_4_H__
#define __IEEE_802_15_4_H__

typedef struct
{
	UINT8	u8FT			: 3	;	// bit[2:0]. 	FrameType
	UINT8	u8Sec		: 1	;	// bit[3]. 		SecurityUse
	UINT8	u8FP			: 1	;	// bit[4].		FramePend
	UINT8	u8AckReq		: 1	;	// bit[5].		AckRequest
	UINT8	u8IP			: 1	;	// bit[6].		IntraPan
	UINT8	u8Rsv		: 1	;	// bit[7]
} bMAC_FRAME_CONTROL_LOW_SUBFIELD;

typedef union
{
	UINT8	u8All;
	bMAC_FRAME_CONTROL_LOW_SUBFIELD	bBit;
} uMAC_FRAME_CONTROL_LOW;

typedef struct
{
	UINT8	u8RsvL		: 2	;	// bit[1:0]
	UINT8	u8DstMode	: 2	;	// bit[3:2]
	UINT8	u8RsvH		: 2	;	// bit[5:4]
	UINT8	u8SrcMode	: 2	;	// bit[7:6]
} bMAC_FRAME_CONTROL_HIGH_SUBFIELD;

typedef union
{
	UINT8	u8All;
	bMAC_FRAME_CONTROL_HIGH_SUBFIELD	bBit;
} uMAC_FRAME_CONTROL_HIGH;

typedef union
{
	UINT8	au8Ext[8];
	UINT16	u16Short;
} uMAC_ADDR;

typedef struct
{
	UINT8		u8PhyPayLen;
	uMAC_FRAME_CONTROL_LOW	uFC_L;
	uMAC_FRAME_CONTROL_HIGH	uFC_H;
	UINT8		u8SeqNum;
	UINT16		u16DstPanID;
	uMAC_ADDR	uDstAddr;
	UINT16		u16SrcPanID;
	uMAC_ADDR	uSrcAddr;	

	UINT8		u8MacHeadLen;
	UINT8		u8MacPayLen;
	UINT8*		pu8MacPayload;

	UINT8		u8OobCode;
	INT8			i8RSSI;				// in dBm
	UINT8		u8FcsLow;
	UINT8		u8FcsHigh;	
	UINT8		u8Correlation;
} tMAC_INFO;

typedef struct
{	
	UINT8		u8DstMode;
	UINT16		u16DstPanID;
	uMAC_ADDR	uDstAddr;
	UINT8		u8SrcMode;
	//-	u8TxOption
	//	b[7:4]	: Oob Code
	//	b[3:1]	: reserved
	//	b[0]		: AckReq. 1=ACK is requested.
	UINT8		u8TxOption;
	UINT8		u8MsduLen;
	UINT8*		pu8Msdu;
} tMAC_DATA_REQ;


typedef struct
{
	UINT8	u8Channel;
	UINT16	u16PanID;
	UINT16	u16ShortAddr;
	UINT8	au8IEEEAddr[8];
	UINT16	u16DstPanID;
	UINT16	u16DstShortAddr;
	UINT8	au8DstIEEEAddr[8];
	UINT8	u8DataRate;
	UINT8	u8TxPowerLevel;	
	UINT8	u8DSN;
	UINT8	u8NoAckRetry;
	UINT8	u8BackoffRetry;
} tMAC_INFO_BASE;

//-	IEEE 802.15.4 Constants
#define	maccMaxPHYPacketSize			127
#define	maccMinPHYPacketSize			5

//-	IEEE 802.15.4 Status Enumeration
#define	maccSTA_SUCCESS 				0x00
#define	maccSTA_BEACON_LOSS			0xE0
#define	maccSTA_CHAN_ACCESS_FAIL 		0xE1
#define	maccSTA_DENIED				0xE2
#define	maccSTA_DISABLE_TRX_FAIL		0xE3
#define	maccSTA_FAILED_SEC_CHECK		0xE4
#define	maccSTA_FRAME_TOO_LONG 		0xE5
#define	maccSTA_INVALID_GTS			0xE6
#define	maccSTA_INVALID_HANDLE		0xE7
#define	maccSTA_INVALID_PARAMETER	0xE8
#define	maccSTA_NO_ACK				0xE9
#define	maccSTA_NO_BEACON			0xEA
#define	maccSTA_NO_DATA				0xEB
#define	maccSTA_NO_SHORT_ADDR		0xEC
#define	maccSTA_OUT_OF_CAP			0xED
#define	maccSTA_PAN_ID_CONFLICT		0xEE
#define	maccSTA_REALIGNMENT			0xEF
#define	maccSTA_TRANSACTION_EXPIRE	0xF0
#define	maccSTA_TRANSACTION_OVER		0xF1
#define	maccSTA_TX_ACTIVE			0xF2
#define	maccSTA_UNAVAILABLE_KEY		0xF3
#define	maccSTA_UNSUPPOR_ATTRIBUTE	0xF4
#define	maccSTA_FRAME_TOO_SHORT 		0xF8

//-	IEEE 802.15.4 Frame Structure - FrameControl_Low Field
#define	maccFCL_FrameType				0x07
#define	maccFCL_Security				0x08
#define	maccFCL_FramePend			0x10
#define	maccFCL_AckReq				0x20
#define	maccFCL_IntraPan				0x40
#define	maccFCL_rsv					0x80

//-	IEEE 802.15.4 Frame Structure - FrameControl_High Field
#define	maccFCH_rsv01				0x03
#define	maccFCH_DstMode				0x0C
#define	maccFCH_rsv45				0x30
#define	maccFCH_SrcMode				0xC0

//-	MAC Frame
#define	maccFrame_Beacon				0
#define	maccFrame_Data				1
#define	maccFrame_Ack				2
#define	maccFrame_Command			3

//-	MAC Address Mode
#define	maccAddrMode_None			0
#define	maccAddrMode_Rsv				1
#define	maccAddrMode_16bit			2
#define	maccAddrMode_64bit			3

//-	MAC Address
#define	maccFFFE						0xFFFE
#define	maccBroadcast					0xFFFF

#endif		//	#ifndef __IEEE_802_15_4_H__
	

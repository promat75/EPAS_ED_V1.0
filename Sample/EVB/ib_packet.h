
#ifndef __MG2470_IB_PACKET_H__
#define __MG2470_IB_PACKET_H__


#define OPTION_MESSAGE_LEN				0x08
#define RF_DATA_PACKET_LENGTH				0x10
#define RF_PER_PACKET_LENGTH				0x10

#define 	PKT_HEADER_TAIL_LENGTH			5	// STX + CMD + LEN + CHKSUM + ETX
//#define PKT_HEADER_TAIL_LENGTH			7	// STX + CMD + SRT_ADDR(2) + LEN + CHKSUM + ETX
#define	CMD_DEFUALT						0xFF		


#if 0		// 0xFF 필드를 사용 하지 않고 전류 데이타 MSB에 Flag를 set 하기로 결정
// default 가 0xFF 이므로 상태 발생시 bit를 clear 시킴 
#define	BST_VCC_LOW						0x01	// vcc < 3.0v 
#define	BST_OVER_CURR					0x02		//  ??

#define    	BST_PACKET_STS					0x80	// 상태메세지 cmd 와 하위 bit 0x7F 상태 메세지를 1바이트로 전송  : 추후 수신기에서 처리 필요 

#define 	SET_BOARD_VCC_STS(X) 				( X < ADCV_MIN_VCC)?  (u8BoardStatus & ~BST_VCC_LOW) : (u8BoardStatus | BST_VCC_LOW)

#else

#define	BST_VCC_LOW						0x80	// vcc < 3.2v  
#define	BST_OVER_CURR					0x40	//  Over current flag 

#define 	SET_BOARD_VCC_STS(X) 				( X < ADCV_MIN_VCC)?  (u8BoardStatus |= BST_VCC_LOW) : (u8BoardStatus &= ~BST_VCC_LOW)
#define 	SET_OVER_CURR_STS() 				Porduct_Mode |= PM_OVER_CURR_MODE; u8BoardStatus |= BST_OVER_CURR
#define 	CLR_OVER_CURR_STS() 				Porduct_Mode &= ~PM_OVER_CURR_MODE; u8BoardStatus &= ~BST_OVER_CURR


#endif

typedef struct	
{
	UINT8	u8STX;
	UINT8	u8CMD;
	UINT8	u8Size;
	UINT16	u16SenShortAddr;					// 라우터를 통해 전달될 센서 IEEE short
	UINT8	u8Data[RF_DATA_PACKET_LENGTH];
	UINT8	u8Checksum;
	UINT8	u8ETX;
}	tRF_RT_PACKET_FORMAT;


typedef struct	_DataFormat
{
	UINT8	u8STX;
	UINT8	u8CMD;
	UINT8	u8Size;
	UINT8	u8Data[RF_DATA_PACKET_LENGTH];
	UINT8	u8Checksum;
	UINT8	u8ETX;
}	tRF_PACKET_FORMAT;



typedef struct{

	UINT8	u8DataMsgPanID;			// u8DataMsgBody[0]
	UINT8	u8DataMsgChannel;			// 
	UINT8	u8DataMsgSrcID;			// pairing format : src id
	UINT8	u8DataMsgDestID;			//  pairing format : src IEEE Addr[0] 
	UINT16	u16DataMsgTemp;			// 
	UINT16	u16DataMsgCurrent;		// 
	UINT8	u8DataMsgVibX;			// 
	UINT8	u8DataMsgVibY;			// 
	UINT8	u8DataMsgVibZ;			// u8DataMsgBody[11], pairing format : src IEEE Addr[7]
} uMSG_BODY;


// total LENGH = 16 : 센서 데이타 포맷 <= gtRF_PktMessage.u8Data[RF_DATA_PACKET_LENGTH]
typedef struct{
	UINT8	u8DataMsgSyncByte;		// 0x02
	UINT8	u8DataMsgBodySize;		// 0x10
	UINT8	u8DataMsgCMD;			// 0xFF
	
	uMSG_BODY	u8DataMsgBody;		// length 이후의 데이타를 모두 같이 처리 : checksum 일치만 체크
	UINT8	u8DataMsgChkSum;
	UINT8	u8DataMsgETX;			// 0x03
} tDATA_MESSAGE;		 //Serial Router Data Format


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


extern tPER_PARAMETER		tPER;
extern tRF_PACKET_FORMAT 	gtRF_PktMessage;

extern UINT8				u8BoardStatus;


void PER_Packet(UINT8* data_buff);
void	Init_PER(tPER_PARAMETER *tpPER);
UINT8 UserCtrl_Packet(UINT8* data_buff);


UINT8 Broadcast_Request(void);
UINT8 SendDataPacket(UINT8* u8Data);
UINT8 App_Data_Transmit(UINT8 length ,UINT8 *u8Data);
//UINT8 	App_Data_Response(UINT8 length ,UINT8 *u8Data);

UINT8 CalcCheckSum(UINT8* data_buff, UINT8 length);

#endif	// #ifndef __MG2470_IB_PACKET_H__
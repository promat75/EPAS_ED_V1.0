
#ifndef __MG2470_IB_ZIGBEE_H__
#define __MG2470_IB_ZIGBEE_H__


#define AUTO_PAIR_ID						0xA5

#define PER_PACKET_COUNT					1000
#define PAIR_RETRY_COUNT					10

#define PAIRING_BUTTON_CHECK_TIME   			3000
#define PAIRING_RETRY_TIME		   			500
#define PAIRING_LED_STANBY_TIME   			5000
#define PAIRING_LED_TIME		   			500



typedef enum {PAIRING_WAIT, PAIRING_CHECK, PAIRING_START, PAIRING_STANBY,
			PAIRING_SEND, PAIRING_RETRY, PAIRING_FAIL, PAIRING_OK, PAIRING_EXIT} PAIRING_STS;

typedef enum {SQ_WAIT, 	SQ_INIT_OVER_CHK, SQ_INIT_OVER_WAIT, SQ_RF_TX_INIT, SQ_TX_DELAY, SQ_RF_TX_START,   
			SQ_SLEEP_MODE, SQ_PAIR_WAIT_ST, SQ_RF_TX_PER } TX_SEQ_STS;

// SQ_INIT_OVER_CHK. SQ_INIT_OVER_WAIT, 

#define U8OPT3_NODIP_SW			0x01
#define U8OPT3_RESERVED1			0x02
#define U8OPT3_RESERVED2			0x04
#define U8OPT3_RESERVED3			0x08
#define U8OPT3_RESERVED4			0x10
#define U8OPT3_RESERVED5			0x20
#define U8OPT3_RESERVED6			0x40
#define U8OPT3_RESERVED7			0x80


typedef struct	_PairInfo
{
	UINT8	SrcID;				//sensor ID 
	UINT8	au8IEEEaddr[8];		// Dest addr 
}	tPAIRINFO;


typedef struct	_Option
{
	UINT8	u8ProductMode;		
	UINT8	u8Pid_Chan;				
	UINT8	u8Option3;			// sleep time 		
	UINT8	u8Option4;				
	UINT16	 u16Option1;			// Temp ADC offset value 	
	UINT16	 u16Option2;				
}	tOPTION;


typedef struct	_PairTable
{
	//UINT8	u8TableNum;			// 
	UINT8	uPairCount;			// 페어링 여부를 관리 : 1 = 페어링 OK, 
	UINT16	 u16PanID;			// 
	UINT8	u8Channel;			// channel number 
	tPAIRINFO	PairInfo;
	tOPTION	Option;
	UINT8	u8Sum;

}	PairTable;


extern UINT8 	u8AppRFSq;
extern UINT8	u8Sendstatus;

extern UINT8 	Retry_count;
extern UINT16 	u16RfPacketCount;
extern UINT16 	u16RfPacketCountH;
extern UINT16	u16NoAckCount;

extern UINT8	u8Task_pairing;
extern UINT8 	u8AppPairCount;
extern UINT8	u8AutoPairSrcID;

extern UINT8	u8KeySeqNum;
extern UINT32	u32FrameCounter;

extern UINT8	TxTimePeriod;

extern PairTable 	gtPIB;



void Pairing_Init(void);
void Pair_PIB_SAVE();
void Pair_Display_table(void);
#if 0	// _EPAS_MODE == 0 is Compile.
void ZAPP_PairingProcess(void);
#endif

void RF_Init_Wait(void);

void SetTxInitWaitTime(void);
void SetSensorReadState(void);
void SetTxWaitStatus(void);

void App_RF_TxProcess(void);
void App_RF_RxProcess(void);
void PER_RespProcess(void);
void SetTxWaitTimer(void);
void SetDipSwitch_ChannelPanID(void);
void Security_Material(void);
void SetOptionData(void);
void Set_ChannelPanID(void);
void PowerDown_WakeupProcess(void);
void StartSensorReadTimer(void);


void SetDestIEEEAddress(UINT8 *address);
void SetDestShortIEEEAddress(UINT8 *address);

UINT8 SetNextTxSequence(void);
UINT8 Pair_Table_Save(UINT8	u8Num, UINT8 *pu8DstIEEEAddr);
UINT8 Pair_PIB_COPY(UINT8 *pBuf);
UINT8 Compare_IEEEAddress(UINT8* pib_addr,  UINT8* pib_addr_new, UINT8 count);

UINT16 App_Get2ByteRandWait(void);


#endif	// #ifndef __MG2470_IB_ZIGBEE_H__
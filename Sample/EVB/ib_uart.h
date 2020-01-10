
#ifndef __MG2470_IB_UART_H__
#define __MG2470_IB_UART_H__



#define	SYNC_BYTE						0xFF  	//Sync 
#define	HEADER_PC_TO_ZIGBEE				0xAC  	//Header MCU to MG2470
#define	HEADER_ZIGBEE_TO_PC				0xCA 	// Header MG2470 to MCU
#define	END_BYTE						0x03  	//

#define	ZIGBEE_ETX1						0x0D  	//ETX1
#define	ZIGBEE_ETX2						0x0A  	//ETX2

// 페어링, flash control  
#define	GET_COMMAND_ID_1				0x01
#define	GET_COMMAND_ID_2				0x02
#define	GET_COMMAND_ID_3				0x03	// PER test cmd
#define	GET_COMMAND_ID_4				0x04
#define	GET_COMMAND_ID_5				0x05
#define	GET_COMMAND_ID_6				0x06

// 
#define	GET_COMMAND_ID_A0				0xA0
#define	GET_COMMAND_ID_A2				0xA2	// UART 페어링 진행  

#define	GET_COMMAND_ID_B0				0xB0		// sensor uart mode 설정                 

// cal parmater  
#define	GET_COMMAND_ID_C0				0xC0
#define	GET_COMMAND_ID_C1				0xC1
#define	GET_COMMAND_ID_C2				0xC2	//  set option    
#define	GET_COMMAND_ID_C3				0xC3	// change SLEEP TIME
#define	GET_COMMAND_ID_C4				0xC4	//  Set offset : 써미스터 adc offset ( 0xC2 option1 의 값만 단독 설정 하는 명령  ) 
#define	GET_COMMAND_ID_C5				0xC5	//  Get current zener ADC data : 현재 ADC 모니터 value 
#define	GET_COMMAND_ID_C6				0xC6	//  Set zener adc start / recovery  ADC data 

 #define	GET_UART_CMD_PAIR				0x11		// sensor pairing
#define	GET_UART_CMD_PARA				0x12
                               		

// RF  CMD 
#define STX								0xA0
#define ETX								0xAF
                                	
#define CMD_PAIRING_REQUEST				0xF0
#define CMD_PAIRING_RESPONSE				0xF1
#define CMD_DATA_REQUEST					0xA0
#define CMD_DATA_RESPONSE				0xA1

#define CMD_ID_RF_DATA_START				0x01		// 
#define CMD_ID_RF_DATA_STOP				0x02		// 
#define CMD_ID_RF_DATA_PER				0x03		// PER test 
#define CMD_ID_RF_DATA_RESTART				0x04		//  
#define CMD_ID_RF_SEN_DEBUG				0x05	//  
#define CMD_ID_RF_SEN_OPTION				0x06	//  
#define CMD_ID_RF_SEN_EXT2					0x07	//  


#define CMD_DATA_ALLRESET					0xB0



typedef struct{
UINT8	u8MessageSyncByte;
UINT8	u8MessageHeader;
UINT8	u8MessageBodySize;
UINT8	u8MessageBody[70];
UINT8	u8CheckSum;
UINT8	u8ETX1;
UINT8	u8ETX2;
} tSerialMessageFormat; //Serial Data Format


extern UINT8 	Sensor_Mode;

extern UINT8 	CurCal_DataBuff[512];
extern UINT8 	SRCID_DataBuff[512];



void ZAPP_CommandInterface(void);


UINT8 ZAPP_HostUartParser(void);
UINT8 ZAPP_HostUart0Parser(void);



#endif	// #ifndef __MG2470_IB_UART_H__
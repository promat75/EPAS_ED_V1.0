
#ifndef __MG2470_IB_TEMP_SEN_H__
#define __MG2470_IB_TEMP_SEN_H__


#include "ib_main_sen.h"       


#if 1	// Kong.sh 0 --> EVKIT 보드 테스트용 
#define START_TEMP_LEVEL					-40		// 테이블 시작 온도 : 0   
#else
#define START_TEMP_LEVEL					-30		// 테이블 시작 온도 : 0   
#endif

#define TEMP_LEVEL				    		10		// 테이블 온도 간격  


#define TEMP_ERROR_COUNT					2		// 연속 카운터 
#define TEMP_LOW_ERROR					0xFFD8	//  -20 : -55(0xFF92) 까지 검출 가능 
#define TEMP_HIGH_ERROR					0x00F0	//  120도, 125 까지 검출, 0xc8 = 100 , 0xAA = 85   

#define AVG_TEMP_BUFF_SIZE 				4		//  average count : 송신 주기 (1초 ?)로  4회 평균 <=IR 센서   
#define TEMP_COMM_ERROR_COUNT			10		// 연속 10회(5초 정도) ir 센서 통신 실패시 에러값 출력( -55도 )

typedef struct{
UINT16	u8TempAdc;								// 기준온도
float		fTempSlope1;								//   table_adc / slope
float		fTempSlope2;								//  1 /  slope
} tTempTable; //Serial Data Format



extern UINT8 	SenReadState; 

extern float 	Real_TempValue;
extern UINT8	u8TempSenErrCount;
extern INT16 	TempData;
extern INT16 	i16ReadTemp;
extern INT16	u16TempOffset;


void TempSensor_Init(void);

void Temp_dataout(void);
void onewire_reset(void);
void write_bit (unsigned char bitval);
void write_byte (unsigned char byte);

void AvgReadTempValue(void);
void wait_ready (void);    
void Temp_dataout(void);
void CalcTemperature(void);
void GetSensor_TempData(void);
void ReadTempSensor(void);
void InitGetSensor_TempData(void);


void TempConvert(void);

UINT8 read_byte (void);
UINT8 read_bit (void);



#endif	// #ifndef __MG2470_IB_SENSOR_H__
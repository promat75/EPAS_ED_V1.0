
#ifndef __MG2470_IB_SENSOR_H__
#define __MG2470_IB_SENSOR_H__


#include "ib_main_sen.h"       


#define AVG_CURR_BUFF_SIZE 				2		// average count :  송신 주기(1초 ?)로  4회  평균  

// ACCEL sensor I2C
#define GS_COMMAND_WRITE               	0x80		// Write command
#define GS_COMMAND_READ					0xA0	//  Read command

//**********  REF VCC 전압 AD 고정값 **************************//
// 전압 : 	0V		1.67V		1.76	1.8V	3V		3.3
// AD   : 	-1466	-613		-477	-480	136		314
// 작을수록 AD값은 증가함
//*************************************************************//
#define ADCV_CAL_VCC					-477
//#define ADCV_CAL_VCC					-613		//  vcc cal data : 1.67V ( FET 턴온시 걸리는 전압을 차감한 최대 입력 전압값 )
//#define ADCV_CAL_VCC					-525				//  vcc cal data : 1.84V ( AVCC 1.84V ) 
#define ADCV_CAL_GND					-1466		//  gnd cal data  
//#define ADCV_CAL_GND						-1470		//  gnd cal data  

//#define ADCV_CAL_VCC						470				//  vcc cal data  org
//#define ADCV_CAL_GND						-1470			//  gnd cal data  org

//#define ADCV_MIN_VCC						150				// vcc 3.0v  정도 값 : MIN_VCC 이하 일 경우 송신을 하지 않음 =>  reset 수행 

// 3.6V:447, 3.5:400, 3.4:335, 3.3:286, 3.2 229, 3.1:179, 3.0:127, 2.9:78,  2.8V 전에 reset
#define ADCV_MIN_VCC					280		// vcc 3.3 ~ 3.2v  정도 :  ADCV_MIN_VCC 보다 작을때 : OW VCC flag set

#define ADCV_OVER_CURR					0x40		// adc_ch1 over current 데이타가 이값보다 크면(제너 다이오드 동작 시점) 과입력 채널 전류 데이타 전송 
#define ADCV_RECV_CURR					0x30	// Recovery Histerisys : 1A adc value(최고 저전류 코어의 과입력 지점) 24



// 100ms 동안 다음 시간 간격으로 데이타를 수집  : sleep mode, wakeup mode 동작 모두 같은 주기로 읽도록 
#define ACCEL_CURR_SEN_READ_TIME		5		// 5ms 
#define ACCL_CURR_READ_CNT				3		// 100ms 동안 읽을수 있는 횟수 :  12@5ms 는 같은 데이타를 두번 읽음,  11@5ms 는 매 1번 마다 수행   // Kong.sh  Org 8

typedef enum {SEN_INIT_TEMP, SEN_IDLE_WAIT, SEN_READ_WAIT, SEN_READ_DATA, SEN_CALC_DATA } SEN_READ_STATE;


// data average
typedef struct{
	UINT8	u8AvgInitFlag;						// 최초 평균시 u8AvgCount 만큼 데이타가 없을때 데이타 처리 위해 사용 flag
	UINT8	u8AvgCount;							// 
	UINT16*	u16AvgBuffer;						// 
	float	fAverage;							//
	
} tAVERGAE_DATA;		


//extern UINT16 AccelerationSensor_setData[6];
//extern UINT8 AccelerationSensor_Status[6];
//extern signed char AccelerationSensor_NewData[6];
//extern signed char AccelerationSensor_OldData[6];
extern char AccelerationSensor_getData[6];
extern UINT8 AccelerationSensor_Scalefactor[3];
extern char AccelerationSensor_ThirdMaxData[6];


extern signed char AX_data;
extern signed char AY_data;
extern signed char AZ_data;


extern UINT8 	SenReadState; 


extern INT16 	ADC_DataBuff[2];
extern UINT16 	ADC_Voltage;
extern UINT16	u16ADC_OverCurrentValue;
extern UINT8	u8StartZenorADC;
extern UINT8	u8RecoveryZenorADC;


extern UINT8 	ConfigBit_value;

extern UINT8 	I2C_DataBuff[2];
extern UINT8 	AccelerationSensor_ScalData[6];
extern UINT8 	AccelerationSensor_ScalBuff[512];
extern UINT8 	u8I2CSenStatus;




extern float 	Current_Val;
extern INT32 	Test_int32;
extern UINT32 Test_uint32;
extern UINT16 Test_uint16;

extern float 	Inver_A;
extern float 	Inver_B;
extern float 	Inver_C;
extern float 	Inver_D;


extern tAVERGAE_DATA gtCurrentAvg;
extern tAVERGAE_DATA gtTempAvg;		// temperature average

//extern void Read_Temp_Sensor(void);		// EXT sensor


void SensorDataBuff_Init(void);
void AccelSensor_Init(void);
void AccelSensor_pd_control(UINT8 pdbit);
void Accelero_OUTPUT_DATA(void);
// void Read_AcclCurr_Sensor(void);
void Curr_Max(void);
void FloatToHEX(float f, char *pStr);
void AvgBufferInit(UINT8 avg_count, tAVERGAE_DATA* pt_current_avg);
void CalcCurrent_ADCVolt(void);
void GetSensor_CurrentData(void);
void GetSensor_AccelData(void);
void SetThirdMaxValueInit(UINT8 Num);
void GetInit_CurrentData(void);


void SensorReadingProcess(void);


UINT8 read_byte (void);
UINT8 read_bit (void);

UINT8 Min_MaxModify(UINT8 Num);
UINT8 SensorOut(void);
UINT8 DataOut(UINT8* data_buf);

void MovingAverage(UINT16 in_data, tAVERGAE_DATA* pt_current_avg);



//void TestMinMax(void);
//void TestMinMax2(void);



#endif	// #ifndef __MG2470_IB_SENSOR_H__

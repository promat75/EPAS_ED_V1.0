
#ifndef __MG2470_IB_SENSOR_H__
#define __MG2470_IB_SENSOR_H__


#include "ib_main_sen.h"       


#define AVG_CURR_BUFF_SIZE 				4		// average count :  �۽� �ֱ�(1�� ?)��  4ȸ  ���  

// ACCEL sensor I2C
#define GS_COMMAND_WRITE               	0x80		// Write command
#define GS_COMMAND_READ					0xA0	//  Read command

//**********  REF VCC ���� AD ������ **************************//
// ���� : 	0V		1.67V		1.76	1.8V	3V		3.3
// AD   : 	-1466	-613		-477	-480	136		314
// �������� AD���� ������
//*************************************************************//
#define ADCV_CAL_VCC					-477
//#define ADCV_CAL_VCC					-613		//  vcc cal data : 1.67V ( FET �Ͽ½� �ɸ��� ������ ������ �ִ� �Է� ���а� )
//#define ADCV_CAL_VCC					-525				//  vcc cal data : 1.84V ( AVCC 1.84V ) 
#define ADCV_CAL_GND					-1466		//  gnd cal data  
//#define ADCV_CAL_GND						-1470		//  gnd cal data  

//#define ADCV_CAL_VCC						470				//  vcc cal data  org
//#define ADCV_CAL_GND						-1470			//  gnd cal data  org

//#define ADCV_MIN_VCC						150				// vcc 3.0v  ���� �� : MIN_VCC ���� �� ��� �۽��� ���� ���� =>  reset ���� 

// 3.6V:447, 3.5:400, 3.4:335, 3.3:286, 3.2 229, 3.1:179, 3.0:127, 2.9:78,  2.8V ���� reset
#define ADCV_MIN_VCC					280		// vcc 3.3 ~ 3.2v  ���� :  ADCV_MIN_VCC ���� ������ : OW VCC flag set

#define ADCV_OVER_CURR					0x40		// adc_ch1 over current ����Ÿ�� �̰����� ũ��(���� ���̿��� ���� ����) ���Է� ä�� ���� ����Ÿ ���� 
#define ADCV_RECV_CURR					0x30	// Recovery Histerisys : 1A adc value(�ְ� ������ �ھ��� ���Է� ����) 24



// 100ms ���� ���� �ð� �������� ����Ÿ�� ����  : sleep mode, wakeup mode ���� ��� ���� �ֱ�� �е��� 
#define ACCEL_CURR_SEN_READ_TIME		5		// 5ms 
#define ACCL_CURR_READ_CNT				3		// 100ms ���� ������ �ִ� Ƚ�� :  12@5ms �� ���� ����Ÿ�� �ι� ����,  11@5ms �� �� 1�� ���� ����   // Kong.sh  Org 8

typedef enum {SEN_INIT_TEMP, SEN_IDLE_WAIT, SEN_READ_WAIT, SEN_READ_DATA, SEN_CALC_DATA } SEN_READ_STATE;


// data average
typedef struct{
	UINT8	u8AvgInitFlag;						// ���� ��ս� u8AvgCount ��ŭ ����Ÿ�� ������ ����Ÿ ó�� ���� ��� flag
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
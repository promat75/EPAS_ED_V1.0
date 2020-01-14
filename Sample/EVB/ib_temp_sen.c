
/*******************************************************************************
	- Chip		: MG2470B 
	- Vendor 		: RadioPulse Inc, 2011. 
	- Date		: 2017-12-12
	- Version		: V2.6

	- File			: ib_temp_sen.c    
*******************************************************************************/


#include "ib_main_sen.h"
#include "ib_temp_sen.h"

#include <stdio.h>


// 배열수 17개 맞출것... Flash Memory Address를 맞추기 위해..
#define THERM_TABLE_COUNT	17

#if (TEMP_SEN_MODE == THERM_SEN)
// 0 ~ 80도, 5도 간격 : 17개, NT Thermister Temperator Table .. Kong.sh
// ADC, adc / slope, 1 / slope
#if 1
// 1.8V REF VOLTAGE
const tTempTable gtTempTable[THERM_TABLE_COUNT] = {	
	{ 14270, 1285.6, 0.0901 }, 	// -40
	{ 14159, 1264.2, 0.0893 }, 	// -30
	{ 14047, 705.9 , 0.0503 }, 	// -20 
	{ 13848, 491.1 , 0.0355 }, 	// -10
	{ 13566, 303.5 , 0.0224 }, 	// 0
	{ 13119, 208.6 , 0.0159 }, 	// 10
	{ 12490, 150.7 , 0.0121 }, 	// 20 
	{ 11661, 111.8 , 0.0096 }, 	// 30
	{ 10618, 89.0  , 0.0084 }, 	// 40
	{ 9425 , 72.0  , 0.0076 }, 	// 50
	{ 8116 , 62.8  , 0.0077 }, 	// 60
	{ 6824 , 54.9  , 0.0081 }, 	// 70
	{ 5582 , 50.7  , 0.0091 }, 	// 80
	{ 4480 , 47.1  , 0.0105 }, 	// 90
	{ 3528 , 45.3  , 0.0128 }, 	// 100
	{ 2749 , 42.8  , 0.0156 }, 	// 110
	{ 2107 , 10.0  , 0.0047 }, 	// 120
};
#else
// 3.3V REF VOLTAGE
const tTempTable gtTempTable[THERM_TABLE_COUNT] = {	
	{ 11552,	73.0,	0.0063 }, 	// 0
	{ 10761,	62.7,	0.0058 }, 	// 5
	{ 9903, 	54.8,	0.0055 }, 	// 10 
	{ 8999, 	48.6,	0.0054 }, 	// 15
	{ 8073, 	43.7,	0.0054 }, 	// 20
	{ 7149, 	39.8,	0.0056 }, 	// 25
	{ 6251, 	36.6,	0.0059 }, 	// 30
	{ 5398, 	34.0,	0.0063 }, 	// 35
	{ 4605, 	31.8,	0.0069 }, 	// 40
	{ 3880, 	29.6,	0.0076 }, 	// 45
	{ 3225, 	28.0,	0.0087 }, 	// 50
	{ 2650, 	26.1,	0.0099 }, 	// 55
	{ 2143, 	24.2,	0.0113 }, 	// 60
	{ 1701, 	22.3,	0.0131 }, 	// 65
	{ 1319, 	20.1,	0.0152 }, 	// 70
	{ 990, 		17.5,	0.0177 }, 	// 75
	{ 708, 		14.7,	0.0207 }, 	// 80  
};
#endif
#endif

float 		Real_TempValue=0; 		
INT16		u16TempOffset = 0;			// 온도 보정 데이블 보정 위한 값 : adc 값을 그대로 보정 
INT16 		TempData=0; 			

// 접촉식 센서  
INT32	 	u32TempSum = 0;
UINT8 		u8TempAvgCount = 0;

// IR sensor
INT16 		i16ReadTemp;
UINT8 		u8TempSenErrCount = 0;


tAVERGAE_DATA gtTempAvg;		// temperature average


//----------------------------------------------------------------------------------------------
void TempSensor_Init(void)
{
	
	#if (TEMP_SEN_MODE == INT_SEN)
	onewire_reset();
	#endif
	
     	AvgBufferInit(AVG_TEMP_BUFF_SIZE, &gtTempAvg );

	//----------------------------------------------------------------------------------------
	// IR Temp sensor
  	#if 0		// IR Sen TMP007
	TMP007_Init();
	#endif
	//----------------------------------------------------------------------------------------

}

//------------------------------------------------------------------------------------------------


#if (TEMP_SEN_MODE == INT_SEN)

void onewire_reset()  // OK if just using a single permanently connected device 
{ 
	UINT8 loop_count =0;
	
	HAL_Gpio3InOutSet(0, GPIO_OUTPUT, GPIO_DRIVE_4mA); 
	GP30 = 0;//output_low(ONE_WIRE_PIN); 
	
	//AppLib_DelayFor500us();
	AppLib_DelayFor100us();AppLib_DelayFor100us();AppLib_DelayFor100us();AppLib_DelayFor100us();
	AppLib_DelayFor50us();
	
//	HAL_Gpio3InOutSet(0, GPIO_OUTPUT, GPIO_DRIVE_4mA);
	GP30 = 1;//output_float(ONE_WIRE_PIN); // float 1-wire high 
	
	HAL_Gpio3InOutSet(0, GPIO_INPUT, GPIO_DRIVE_4mA );
	AppLib_DelayFor50us();AppLib_DelayFor10us();

	while(!GP30) {
		 if (loop_count++ >= 2)	break;
		AppLib_DelayFor10us();
		
	// 100us delay 모두 제거 해도 loop count = 1	
	//AppLib_DelayFor100us();AppLib_DelayFor100us();AppLib_DelayFor100us();AppLib_DelayFor100us();
	}
	
}


unsigned char read_bit (void) 
{
	HAL_Gpio3InOutSet(0, GPIO_OUTPUT, GPIO_DRIVE_4mA);
	GP30 = 0;//output_low(ONE_WIRE_PIN); 
	
    	AppLib_DelayFor1us();//delay_us (1);
	
	HAL_Gpio3InOutSet(0, GPIO_INPUT, GPIO_DRIVE_4mA );//GPIO_HIGH_IMPEDANCE   GPIO_DRIVE_4mA
	AppLib_DelayFor2us();
	AppLib_DelayFor2us();
	AppLib_DelayFor1us();
	
    	if (!GP30)       //Abtastung innerhalb von 15탎
        	return 0;
    	else
        	return 1;
		
}


void write_bit (unsigned char bitval) 
{    
	HAL_Gpio3InOutSet(0, GPIO_OUTPUT, GPIO_DRIVE_4mA); 
	GP30 = 0;
	AppLib_DelayFor10us();	

    	if (bitval) 	GP30 = 1;     
	AppLib_DelayFor50us();  
	      
	HAL_Gpio3InOutSet(0, GPIO_OUTPUT, GPIO_DRIVE_4mA);
	GP30 = 1;      

}


unsigned char read_byte (void) 
{
	unsigned char byte = 0;
	UINT8 i;	
    	for (i=0; i<8; i++) 
	{
       		if (read_bit ())
		{
            		byte |= (1<<i);
		}
		AppLib_DelayFor2us();
		AppLib_DelayFor2us();	
		AppLib_DelayFor2us();	
    	}
   	return byte;
}


void write_byte (unsigned char byte) 
{
    	UINT8 i;	
    	
    	for (i=0; i<8; i++) 
	{
	        if (byte & (1<<i))
        	  write_bit (1);
        	else
              write_bit (0);

	      //temp = byte >> i;     // shifts val right 'i' spaces 
	      //temp &= 0x01;           // copy that bit to temp 
	      //DS1820_WriteBit(temp);  // write bit in temp into			  
	}
	AppLib_DelayFor2us();	
	AppLib_DelayFor2us();	
	AppLib_DelayFor1us();		
	// AppLib_DelayFor100us();  
	// AppLib_DelayFor10us();
	// AppLib_DelayFor10us();
} 


void wait_ready (void) 
{
	while (!(read_bit ()));
}



void Temp_dataout(void)
{
	UINT8	i=0;
	UINT8  	Temp1=0, Temp2=0;
	UINT8 	TempSensor_getData[9];

	onewire_reset();
	AppLib_DelayFor500us();
	AppLib_DelayFor500us();	
																				
	write_byte(0xCC);
	write_byte(0x44);
	
	onewire_reset();													
	AppLib_DelayFor500us();
	AppLib_DelayFor500us();


	write_byte(0xCC);
	write_byte(0xBE);

	for(i=0; i<2; i++)
	{
		TempSensor_getData[i]=0;					
	}	
						
	for(i=0; i<2; i++)
	{
		TempSensor_getData[i]=read_byte();	
		AppLib_Delay(5);				
	}
	Temp1=TempSensor_getData[0];
	Temp2=TempSensor_getData[1];

	i16ReadTemp =Temp1;
	i16ReadTemp |=(Temp2<<8);

	// 외부 루틴에서 체크 하도록 이동 
	// 온도 센서 값에 대한 오류 처리 : 이전 값을 사용 하도록 :   -20 <   Temp  < 120 
	//if ((i16ReadTemp > (INT16)0x00F0) || (i16ReadTemp < (INT16)0xFFD8))     // 0x00F0 = 120도, 0xFFD8=-20도,    920xFF92 = -55도 
	//	i16ReadTemp = TempData;							// 이전 평균 온도 센서 값을 사용 

}

#endif

//------------------------------------------------------------------------------------------------

void ReadTempSensor(void)
{
	GetSensor_TempData();						
	CalcTemperature();
		
}

#if (TEMP_SEN_MODE == THERM_SEN)

/***************************************************/
/* NTC THERMISTOR                                  */
/***************************************************/
void GetSensor_TempData(void)
{
	extern INT16 	ADC_DataBuff[2];

	#if _EPAS_MODE
	NTC_CONNECT = 1;
	SYS_AdcSet(ENABLE, 0, ADC_CHAN_3);				// adc channel 을 원상 복귀  
	//AppLib_DelayFor1ms();
	#endif

	// 고정 vcc,gnd 값을 사용 하는 SYS_AdcCalibGet() 이 좀 더 흔들림이  적게 발생   
	SYS_AdcCalibGet(14, &ADC_DataBuff);				// calibration 된 adc data를 read  : gi16ADC_VCC 값 사용
	u32TempSum += (UINT32)ADC_DataBuff[0];
	u8TempAvgCount++;

	#if _UARTDEBUG_MODE
	zPrintf(1,"Temp Adc %u %u\n",(short)ADC_DataBuff[1],(short)ADC_DataBuff[0]);
	#endif
	
	SYS_AdcSet(ENABLE, 0, ADC_CHAN_0);				// adc channel 을 원상 복귀  

	#if _EPAS_MODE
	NTC_CONNECT = 0;
	#endif
}

void TempConvert(void)
{
	UINT8 	u8i;
	float 	ftemp;
	
	i16ReadTemp += u16TempOffset;					// 오차 보정 위한 offset  : ADC 값을 그대로 적용 (현재 온도에 맞는 adc 값을 0.1도 1도 단위로 수정 )
	
	for( u8i = 0; u8i < THERM_TABLE_COUNT;  u8i++) {
		if (gtTempTable[u8i].u8TempAdc < i16ReadTemp)	break;
	}	
		
	if (u8i > 0) u8i -= 1;	
	ftemp = gtTempTable[u8i].fTempSlope2 * i16ReadTemp;
	ftemp = gtTempTable[u8i].fTempSlope1 - ftemp;
	ftemp += (u8i * TEMP_LEVEL + START_TEMP_LEVEL);
	i16ReadTemp = (INT16)(ftemp*2);					// scaled x 10을 -> scaled  x 2  로 변경 전송 (0.5도 단위 온도 데이타)

}	


// packet 전송시는 TempData 값을 전송 
void CalcTemperature(void)
{

	i16ReadTemp = (UINT32)u32TempSum / u8TempAvgCount;	
	#if _UARTDEBUG_MODE
	zPrintf(1,"Temp ADC AVG : %u\n",(short)i16ReadTemp);
	#endif

	TempConvert();								// adc 데이타를 실제 온도 값으로 계산 

	#if _EPAS_MODE
		TempData = i16ReadTemp;
		#if _UARTDEBUG_MODE
		Real_TempValue=(float)TempData/ 2 ;				
		zPrintf(1,"real Temp : %3.1f\n",(float)Real_TempValue);
		#endif
	#else
		MovingAverage(i16ReadTemp, &gtTempAvg );		// 한번더 평균을 구함 => 송신 주기 에 한번씩 평균,  4회 평균시 4번의 송신이 이루어져야 정상 평균값
		TempData = (INT16)gtTempAvg.fAverage;	
		Real_TempValue=(float)TempData / 2 ;				
	#endif

	u8TempAvgCount = 0;	
	u32TempSum = 0;
}	

#elif (TEMP_SEN_MODE == INT_SEN)

//UINT8 TestCount = 0;
UINT8 TempErrResetCount = 0;


// 초기 온도 센서 이상값 읽어 오는 부분을 처리 하기 위한  코드 : 한번 읽어 쓰레기 데이타를 버림 
void InitGetSensor_TempData(void)
{

	// read temp sensor data	
	HAL_SystemIntSet(DISABLE);			// EA=0	
	//Temperature detect//
	Temp_dataout();					// read temp data
	HAL_SystemIntSet(ENABLE);			// EA=1	
	i16ReadTemp = 0;					
	TempErrResetCount = 0;			

}



/***************************************************/
/* Digital Thermometer ( 1회 18msec 소요 )         */
/***************************************************/
void GetSensor_TempData(void)
{

	// read temp sensor data	
	HAL_SystemIntSet(DISABLE);			// EA=0	
	//Temperature detect//
	Temp_dataout();					// read temp data
	HAL_SystemIntSet(ENABLE);			// EA=1	

	if ((i16ReadTemp > (INT16)TEMP_HIGH_ERROR) || (i16ReadTemp < (INT16)TEMP_LOW_ERROR))  {   // 100도, -20도
		i16ReadTemp = TempData;		// 이전 평균 온도 센서 값을 사용 
		if (TempErrResetCount++ > TEMP_ERROR_COUNT) {
			// Temperature Init 	
			onewire_reset();
			TempErrResetCount = 0;
			//zPrintf(1, "\r Reset TempErrResetCount !!!");
		}	
	}
	else
		TempErrResetCount = 0;			// 연속으로 발생시 reset 하도록 

}


// packet 전송시는 TempData 값을 전송 
void CalcTemperature(void)
{
	
	#if 0		// 평균 없이 그대로 전송 
	
	// 온도값이 음수인 경우는 평균을 취하지 않도록 변경  
	if (i16ReadTemp >= (INT16)0 ) 
		MovingAverage(i16ReadTemp, &gtTempAvg );			// 한번더 평균을 구함 => 송신 주기 에 한번씩 평균,  4회 평균시 4번의 송신이 이루어져야 정상 평균값 
	else {
		// 음수일 경우 평균 루틴의 변수를 초기화 시켜 양수 일때 정상적으로 동작 하도록 
		gtTempAvg.fAverage = (float)i16ReadTemp;				// 평균 없이 그대로 사용
		gtTempAvg.u8AvgInitFlag  = 0;
	}	

	TempData = (UINT16)gtTempAvg.fAverage;	
	#else
	
	TempData = i16ReadTemp;
	
	#endif 
	
	
	Real_TempValue=(float)TempData / 2 ;			

	u8TempAvgCount = 0;	
	u32TempSum = 0;
		
}

#else

/***************************************************/
/* IR Thermometer (하동 변압기에만 적용)           */
/***************************************************/
void GetSensor_TempData(void)
{
	// 동해 : 평균 방법 변경  
	CalcTemperature();
		
	if (u8TempSenErrCount > TEMP_COMM_ERROR_COUNT) {
		TempData = 0xFF92;		// 5초 동안 read data 가 없으면 오류 온도 표시 
		u8TempSenErrCount = 0;
	}
}	


// packet 전송시는 TempData 값을 전송 
void CalcTemperature(void)
{
	//TempData = u32TempSum >> 1;						// 2회 평균 : 1초 
	Real_TempValue = ((float)gtTempAvg.fAverage - 1000)/10;
	TempData=Real_TempValue * 2;						// 내장 온도 센서와 같은 단위 값으로 변경  

	u8TempAvgCount = 0;	
	u32TempSum = 0;
		
}

#endif



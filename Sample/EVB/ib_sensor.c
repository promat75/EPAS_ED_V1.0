
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2011.
	- Date		: 2016-10-10
	- Version		: V2.6

	- File			: ib_sensor.c
*******************************************************************************/


#include "ib_main_sen.h"
#include "ib_sensor.h"
#include <stdio.h>

UINT8 		ConfigBit_value = 0;
UINT8 		I2C_DataBuff[2]={0};

signed char 	AX_data=0;
signed char 	AY_data=0;
signed char 	AZ_data=0;
signed char 	AX_Olddata=0;
signed char	AY_Olddata=0;
signed char 	AZ_Olddata=0;
float 		AX_Result=0;
float 		AY_Result=0;
float 		AZ_Result=0;
UINT8 		Accel_flag=0;

//UINT16 AccelerationSensor_setData[6]={0};

//UINT8 AccelerationSensor_Status[6]={0};
UINT8 		AccelerationSensor_ScalData[6]={0,0,0,0,0,0};
UINT8 		AccelerationSensor_ScalBuff[512]={0};
UINT8 		AccelerationSensor_Scalefactor[3]={0};

char 			AccelerationSensor_getData[6]={0,0,0,0,0,0};

//signed char AccelerationSensor_NewData[6]={0};
//signed char AccelerationSensor_OldMaxData[6]={0};
//signed char AccelerationSensor_ThirdMaxData[6]={0};

char 			AccelerationSensor_NewData[6]={0};
char			AccelerationSensor_OldMaxData[6]={0};
char 			AccelerationSensor_ThirdMaxData[6]={0};


UINT8 		u8I2CSenStatus = 0;


INT32 		Test_int32=0;
UINT32 		Test_uint32=0;
UINT16 		Test_uint16=0;

float 		Current_Val=0;
float 		Inver_A=0;
float 		Inver_B=0;
float 		Inver_C=0;
float 		Inver_D=0;


INT16 		ADC_DataBuff[2]={0};
UINT16 		ADC_Voltage=0;

INT16		i16ADC_VccValue = 0;
UINT16		u16ADC_OverCurrentValue = 0;

UINT8		u8StartZenorADC = ADCV_OVER_CURR;
UINT8		u8RecoveryZenorADC = ADCV_RECV_CURR;



tAVERGAE_DATA gtCurrentAvg;	// current average

//UINT16 AvgBuff[AVG_BUFF_SIZE];
//float  gAverage = 0;


void AvgBufferInit(UINT8 avg_count, tAVERGAE_DATA* pt_current_avg)
{
	//UINT16*	pmem;
	pt_current_avg->u8AvgInitFlag = 0;
	pt_current_avg->u8AvgCount = avg_count;
	pt_current_avg->u16AvgBuffer = (UINT16*) malloc(sizeof(UINT16)*pt_current_avg->u8AvgCount);
	
	memset(pt_current_avg->u16AvgBuffer, 0x00, avg_count*2);	// int 변수를 바이트 변수 size로
	pt_current_avg->fAverage = 0;

}	



void SensorDataBuff_Init(void)
{
	#if !_ACCEL_USE
	// 가속도센서 제거후 지그비 통신 동작상태를 유지할 수 있도록 하기 위하여 추가한 임시 코드 끝
	AccelerationSensor_getData[1]  = 0x01;						
	AccelerationSensor_getData[3] = 0x02;						
	AccelerationSensor_getData[5] = 0x03;
	ADC_DataBuff[0] =0x01;
	#endif
	
	// 현재는 사용 하지 않는 변수 ; 추후 사용시 수정 필요 ???
	AccelerationSensor_ScalData[1] = AccelerationSensor_getData[1];						
	AccelerationSensor_ScalData[3] = AccelerationSensor_getData[3];						
	AccelerationSensor_ScalData[5] = AccelerationSensor_getData[5];		
					
					
	AccelerationSensor_getData[1]  = 0;						
	AccelerationSensor_getData[3] = 0;						
	AccelerationSensor_getData[5] = 0;						
	//AccelerationSensor_setData[1] = 0;
	ADC_DataBuff[0] =0;							
}	


//----------------------------------------------------------------------------------------------
// 
UINT8 SensorOut(void)
{
	float Voltage_Result=0;


#if 0
	
	// zPrintf 보다 uart 직접 출력이 더 빨라 데이타가 섞여 출력됨 => 그래서 진동 데이타를 먼저 출력
	//zPrintf(1,"accl : %d  %d  %d",(short)AX_data, (short)AY_data, (short)AZ_data);
    	Accelero_OUTPUT_DATA();			
    	OutData(AX_Result);//Y	
    	OutData(AY_Result);//Y
    	OutData(AZ_Result);//Z

	//OutData(ADC_Voltage);
	
	zPrintf(1," %3.1f  ",(float)Real_TempValue);
	
	Voltage_Result = (float)ADC_Voltage/16384;
	Voltage_Result *= 59.2;		// 59.2 = 3.7V * 16(분배 저항 계수)  
	zPrintf(1," %3.1f  \r",(float)Voltage_Result);
#else

	zPrintf(1,"T: %3.1f  ",(float)Real_TempValue);		// 온도 
	zPrintf(1,"C: %d  ",(short)ADC_Voltage);			// 전류
	zPrintf(1,"V: %d  %d  %d  \n",(short)AX_data, (short)AY_data, (short)AZ_data);	
	
#endif

    	return RETURN_PHY_SUCCESS;
}


UINT8 DataOut(UINT8* data_buf)
{
	UINT8 ii;
	
	for(ii = 0; ii < RF_DATA_PACKET_LENGTH; ii++)
		SYS_Uart1Put(*data_buf++);
	
	SYS_Uart1Put(0x0D);
	SYS_Uart1Put(0x0A);	
    	
    	return RETURN_PHY_SUCCESS;
}

//----------------------------------------------------------------------------------------------


void Accelero_OUTPUT_DATA(void)
{

	signed char n16Value[3]={0};  
	signed char AX_Newdata=0; 
	signed char AY_Newdata=0;
	signed char AZ_Newdata=0;	
		
	AX_Olddata=AX_data;
	AY_Olddata=AY_data;
	AZ_Olddata=AZ_data;	

 	n16Value[0]= AX_data-AccelerationSensor_Scalefactor[0];  
	if (n16Value[0]&0x80)	
	{
		n16Value[0]|=0x80;
	}   

 	n16Value[1]= AY_data - AccelerationSensor_Scalefactor[1];  
	if (n16Value[1]&0x80)
	{
		n16Value[1]|=0x80;
	}  

 	n16Value[2]= AZ_data - AccelerationSensor_Scalefactor[2];  
	if (n16Value[2]&0x80)
	{
		n16Value[2]|=0x80;
	}  
   
    	AX_data =(signed char)n16Value[0];
    	AY_data =(signed char)n16Value[1]; 
    	AZ_data =(signed char)n16Value[2];

	AX_Result=(float)AX_data*19;			// 	AX_Result=(float)AX_data*0.019;	
	AY_Result=(float)AY_data*19;			//  	AY_Result=(float)AY_data*0.019;	
	AZ_Result=(float)AZ_data*19;			// 	AZ_Result=(float)AZ_data*0.019;	

	/* 일정 진동 이상 변경시 알람 설정을 위한 루틴 
	if(AX_Olddata>=AX_data)
		AX_Newdata=AX_Olddata-AX_data;
	else
		AX_Newdata=AX_data-AX_Olddata;	
		
	if(AY_Olddata>=AY_data)
		AY_Newdata=AY_Olddata-AY_data;
	else
		AY_Newdata=AY_data-AY_Olddata;	
		
	if(AZ_Olddata>=AZ_data)
		AZ_Newdata=AZ_Olddata-AZ_data;
	else
		AZ_Newdata=AZ_data-AZ_Olddata;					
		
		
	if((AX_Newdata>=10)||(AY_Newdata>=10)||(AZ_Newdata>=10))
		Accel_flag=1;
	else
		Accel_flag=0;
	*/
			
}


/***************************************************************
* Function : Acceleration Sensor(LIS302DL) Init    
* Parameter : Void
* Write By Kong
***************************************************************/
void AccelSensor_Init(void)
{
	// reset 을 먼저 하고 초기화를 수행 하도록
	ACCELSEN_RESET = RST_ON;
	//AppLib_DelayFor1us();			// 초기화 delay 이므로 read 속도에 영향 없음  : 기존 1ms 사용 
	AppLib_Delay(1);				// 1ms
	ACCELSEN_RESET = RST_OFF;
	//AppLib_DelayFor1us();			// 초기화 delay 이므로 read 속도에 영향 없음  
	AppLib_Delay(1);				// 1ms
	
	
	//--- CTRL_REG1 SET
	//--- Reg 	--> 	DR				|	PD				|	FS					|	STP			STM		|	Zen						|	Yen						|	Xen
	//--- Value	-->		1					|	0					|	0						|	0				0			|	1							|	1							|	1
	//--- Action-->		DR400Hz 	|	PD mode 	|	Scale +-2g	|	normal mode		|	Z axis Enable	|	Y axis Enable	|	X axis Enable 
	//--- X Sensitivity = 16.2mg
	I2C_DataBuff[0] = 0x20;
	I2C_DataBuff[1] = 0x07;
	HAL_I2CMaster_SequentialWrite(0x1C, I2C_DataBuff, 2);  
	
	I2C_DataBuff[0] = 0x21;//CTRL_REG2        
	I2C_DataBuff[1] = 0x02;//0x01;
	HAL_I2CMaster_SequentialWrite(0x1C, I2C_DataBuff, 2); 

	I2C_DataBuff[0] = 0x27;//STATUS_REG        
	I2C_DataBuff[1] = 0x0F;
	u8I2CSenStatus = HAL_I2CMaster_SequentialWrite(0x1C, I2C_DataBuff, 2); 	

#if 0	
	if (u8I2CSenStatus & 0x80) {
		ACCELSEN_RESET = RST_ON;
		AppLib_Delay(100);			// 100ms ; 핀연결이 되어 있지 않음
		ACCELSEN_RESET = RST_OFF;
		
	//	SYS_WdtSet(100);			// 계속 무한 루프를 돌게됨  
	//	while(1);
	}	
#endif	
		
}

/***************************************************************
* Function : Acceleration Sensor(LIS302DL) Power Down Control
* Parameter : Pown Down Control Bit (0:Power Down Mode  1: Active Mode)
* Write By Kong
***************************************************************/
void AccelSensor_pd_control(UINT8 pdbit)
{
	//--- CTRL_REG1 SET
	I2C_DataBuff[0] = 0x20;

	if(pdbit)
	{
		I2C_DataBuff[1] = 0x47;
		AppLib_Delay(10);		//--- active mode 활성화후 유효데이터 취득시 3sec 필요.
	}
	else
		I2C_DataBuff[1] = 0x07;
		
	//--- I2C Write
	HAL_I2CMaster_SequentialWrite(0x1C, I2C_DataBuff, 2);  
}


UINT32 u32AdcSum = 0;
UINT32 u32AdcVccSum = 0;	// vcc 값을 read  
UINT8  u8ADCAvgCount = 0;

 
// 20ms 주기 동작 :  최종 1초에 전류 센서 32 or 31회 avg 되고 있음
void GetSensor_CurrentData(void)
{
	INT16 	i16adc_value;
	
	//-----------------------------------------------------
	//SYS_AdcCalibGet_Once(ADC_CHAN_0, 14, &ADC_DataBuff);	
	
	// 고정 vcc,gnd 값을 사용 하는 SYS_AdcCalibGet() 이 좀 더 흔들림이  적게 발생   
	SYS_AdcCalibGet(14, &ADC_DataBuff);			// calibration 된 adc data를 read  : gi16ADC_VCC 값 사용
	
	//SYS_AdcGet(&ADC_DataBuff);				// adc 그대로 데이타를 읽음 => 0X5555 에서 데이타가 0으로 넘어가서 읽혀짐  
	//-----------------------------------------------------
	
	u32AdcSum += (UINT32)ADC_DataBuff[0];
	u8ADCAvgCount++;
	
	//--------------------------------------
	// vcc 값을 별도 reading  
 	SYS_AdcGet_Once(7, &i16adc_value); 	
 	#if _UARTDEBUG_MODE
 	//zPrintf(1,"VCC is %6d\n",i16adc_value);
 	#endif
	u32AdcVccSum += (UINT32)i16adc_value;
	//--------------------------------------

	#if (ZENER_BOARD)
	SYS_AdcSet(ENABLE, 0, ADC_CHAN_1);			// adc channel 을 온도센서 reading 으로 변경   
	#else

		#if (TEMP_SEN_MODE == THERM_SEN)
		SYS_AdcSet(ENABLE, 0, ADC_CHAN_3);			// adc channel 을 온도센서 reading 으로 변경   
		#else
		SYS_AdcSet(ENABLE, 0, ADC_CHAN_0);			// adc channel 을 원상 복귀  
		#endif	
	#endif
}


void CalcCurrent_ADCVolt(void)
{

	//---------------------------------------------------------------------------------------
	// make vcc data  
	i16ADC_VccValue = (UINT32)u32AdcVccSum / u8ADCAvgCount;	
	u32AdcVccSum = 0;
	//---------------------------------------------------------------------------------------
		
	ADC_DataBuff[0] = (UINT32)u32AdcSum / u8ADCAvgCount;	
	
	// TxTimePeriod : 1 일때만 평균을 구함  
	if ( TxTimePeriod == 1)
		ADC_Voltage = ADC_DataBuff[0];
	else {
		MovingAverage(ADC_DataBuff[0], &gtCurrentAvg );	// 한번더 평균을 구함 => 송신 주기 에 한번씩 평균,  4회 평균시 4번의 송신이 이루어져야 정상 평균값
		ADC_Voltage = (UINT16)gtCurrentAvg.fAverage;	
	}

	//SYS_AdcSet(ENABLE, 0, ADC_CHAN_1);			// adc channel 을 Thermistor 로 변경  

	u32AdcSum = 0;
	u8ADCAvgCount = 0;	

}


UINT32 u32AdcOverSum = 0;	// vcc 값을 read  
UINT8  u8ADCOverAvgCount = 0;

void GetSensor_OverCurrentData(void)
{
	
	// 고정 vcc,gnd 값을 사용 하는 SYS_AdcCalibGet() 이 좀 더 흔들림이  적게 발생   
	SYS_AdcCalibGet(14, &ADC_DataBuff);				// calibration 된 adc data를 read  : gi16ADC_VCC 값 사용
	
	u32AdcOverSum += (UINT32)ADC_DataBuff[0];
	u8ADCOverAvgCount++;

#if (TEMP_SEN_MODE == THERM_SEN)
	SYS_AdcSet(ENABLE, 0, ADC_CHAN_3);				// adc channel 을 온도센서 reading 으로 변경   
#else
	SYS_AdcSet(ENABLE, 0, ADC_CHAN_0);				// adc channel 을 원상 복귀  
#endif	

}


void CalcOverCurrent_ADCVolt(void)
{

	u16ADC_OverCurrentValue = (UINT32)u32AdcOverSum / u8ADCOverAvgCount;	

	//-------------------------------------------------------------------------------------	
	// 과입력 adc 채널 데이타 전송 
	if (u8BoardStatus & BST_OVER_CURR) {
		if (u8RecoveryZenorADC > u16ADC_OverCurrentValue) // histerisys 처리 
			u8BoardStatus &= ~BST_OVER_CURR;
		else 
			ADC_Voltage = u16ADC_OverCurrentValue; 	// histerysys 를 넘기 전까지는 계속 과입력 전류 data 전송
	}	
	else {
		if (u8StartZenorADC <= u16ADC_OverCurrentValue) {
			u8BoardStatus |= BST_OVER_CURR;			// GUI에서 파라메터 계산시 사용할 flag set
			ADC_Voltage = u16ADC_OverCurrentValue; 	// 전송할 ADC 데이타를 변경함 
		}	
	}
	//-------------------------------------------------------------------------------------	
	// PM_OVER_CURR_MODE 일때는 과입력 전류가 흐르고 있어 위 루틴에서 처리 하게됨 
	// if (Porduct_Mode & PM_OVER_CURR_MODE)
	//	ADC_Voltage = u16ADC_OverCurrentValue; 
	
	u32AdcOverSum = 0;
	u8ADCOverAvgCount = 0;	

}


void GetSensor_AccelData(void)
{
	#if _ACCEL_USE
	UINT8 u8status;
		
	if (u8I2CSenStatus & 0x80) {				// u8I2CSenStatus == 0x80 : 진동 센서 오류 상황  
		AccelSensor_Init();	
		return;
	}	
								
	ConfigBit_value = (GS_COMMAND_WRITE | 0x28);

	//----------------------------------------------------------------------------------------
	// HAL_I2CIntSet(,,,20) 1회 read : 360us
	// HAL_I2CIntSet(,,,28) 1회 read : 440us
	u8status = HAL_I2CMaster_RandomRead(0x1C, ConfigBit_value, AccelerationSensor_getData, 0x06); 
	//----------------------------------------------------------------------------------------
	
	// Accl 센서가 알수없는 오류로 먹통이 될 경우 reset 기능이 없음
	if (u8status == 0x80) {	
		zPrintf(1,"Accelsen occur!\n");
		ACCELSEN_RESET = RST_ON;
		SYS_WdtSet(100);
		while(1);		
	}	

		
	// 1초 동안 읽어 들인 값중에서 최고 값을 표시, 1초 마다 송신후 센서값 초기화됨 
	// Min_MaxModify() 를 수행 하면 AccelerationSensor_getData[] 에는 Max 값이 저장됨
	Min_MaxModify(1);		// AccelerationSensor_Status(old) 와 AccelerationSensor_getData(new) 중 절대 변화가 큰값을 return
	Min_MaxModify(3);
	Min_MaxModify(5);
	#endif

}	


//--- 온도센서가 NTC 서미스터일경우 Read Task는 SEN_IDLE_WAIT로 시작한다. 
#if (TEMP_SEN_MODE == INT_SEN)
UINT8	SenReadState = SEN_INIT_TEMP; 
#else
UINT8	SenReadState = SEN_IDLE_WAIT; 
#endif

UINT8	SenReadCount = 0; 
// wakeup 100ms 동안 몇회 읽을 것인지 체크  
void CheckSensorReadCount(void)
{
	if (SenReadCount++ >= ACCL_CURR_READ_CNT) {	// 100ms(4회, 20ms) 동안 센서 read  
		SenReadState = SEN_CALC_DATA;
		SenReadCount = 0;
	}
	else
		SenReadState = SEN_READ_WAIT;

	//#if !_EPAS_MODE
	AppLib_VirtualTimerSet(WAIT_TIMER_SHORT, ACCEL_CURR_SEN_READ_TIME);		
	//#endif
}	

// sleep mode 동작 : sleep wakeup 후  sen read 동작 수행    
// wakeup 동작 : 송신주기와 는 별도로 1초에 한번씩 read 동작 수행 하도록 


// - sleep mode wakeup time 100ms 동안 센서 데이타를 읽어 100ms 동안의 평균을 구함  
// - wakeup 시간 동안 센서 read를 완료 하여 평균 및 데이타 처리가 되어야됨 
// - 진동 센서는 무선 송신 주기 동안의  절대 Max 값을 취함 
// 진동, 전류, 온도 센서(conversion time 이 길어 1초 또는 wakeup시 한번) 를 모두 처리  
void SensorReadingProcess(void)
{

	extern INT16 adc_buf[32];

	
	//static UINT8 read_cnt = 0;
	
	switch(SenReadState) {
		case SEN_INIT_TEMP :
			// read vcc  and clear data  
			#if _EPAS_MODE
			SYS_AdcSet(ENABLE, 0, ADC_CHAN_3);			// adc channel 을 온도센서 reading 으로 변경   
			#else
			GetSensor_CurrentData();
			CalcCurrent_ADCVolt();			
			#endif
			
			#if (TEMP_SEN_MODE == INT_SEN)
				// temp sensor 를 한번 읽어 쓰레기값 초기화 동작   
				InitGetSensor_TempData();

				if (i16ADC_VccValue < ADCV_MIN_VCC) {
					if (Porduct_Mode & PM_SLEEP_MODE) {
						//HAL_PowerdownGpio1WakeupSourceSet(4, 1, 0);		// wakeup interrupt pin : p1.4 (pairing button)   
						#if _POWERDOWN_USE
						HAL_PowerdownMode1(5, 0, 0, 0);					//  5 sec  : wiat for normal power 
						#endif
					}
					InitGetSensor_TempData();
				}	
			#endif

			#if _EPAS_MODE
			zPrintf(1,"SEN_INIT_TEMP\n");
			#endif
			SenReadState = SEN_IDLE_WAIT;
			break;
		
		case SEN_IDLE_WAIT :
			#if _EPAS_MODE
			zPrintf(1,"SEN_IDLE_WAIT\n");
			#endif
			// sleep wakeup, tx finish 동작에서  next state로 이동 
			break;
			
		case SEN_READ_WAIT :
			#if _EPAS_MODE
			zPrintf(1,"SEN_READ_WAIT\n");
			#endif
			#if _EPAS_MODE
				SenReadState = SEN_READ_DATA;
			#else
				if (AppLib_VirtualTimerGet(WAIT_TIMER_SHORT) == 0) 	// 5ms
					SenReadState = SEN_READ_DATA;
			#endif
			break;
			
		case SEN_READ_DATA :						// 5ms 주기로 100ms 안에 11회 정도 수행 가능, 
			//---------------------------------------------
			// 전류센서  adc : ADC 채널 변경 시간을 위해 사이에 진동 센서를 읽음   
			#if _EPAS_MODE
			SYS_AdcSet(ENABLE, 0, ADC_CHAN_3);			// adc channel 을 온도센서 reading 으로 변경   
			#if _EPAS_MODE
			zPrintf(1,"SEN_READ_DATA\n");
			#endif
			#else
			GetSensor_CurrentData();
			#endif
			
			// 진동센서  
			#if _ACCEL_USE
			AccelSensor_pd_control(AccelSensor_Activemode);			// AccelSensor Active mode.. By Kong
			GetSensor_AccelData();															// 1회 550us  - 진동 센서는 수행 주기 동안 max 값을 계속 갱신하여 보유 
			AccelSensor_pd_control(AccelSensor_Powerdownmode);	// AccelSensor Powerdown mode.. By Kong
			#endif
		
			#if (ZENER_BOARD)
			GetSensor_OverCurrentData();
			#endif
					
			// 온도센서 adc  : 써미스터
			#if (TEMP_SEN_MODE == THERM_SEN)
			GetSensor_TempData();					//  전류 센서와 같은 속도(같은 코드), 써미스터를 adc 를 통해 read 할 경우 여러번 읽어 평균 사용 
			#endif
			
			CheckSensorReadCount();				// 5ms 주기로  100ms 동안 읽을수 있는 횠수 지정 되어 있음, 10회후  SEN_CALC_DATA(평균을 구함)    수행 

			break;
			
		case SEN_CALC_DATA :						// sleep mode = 26ms 남음(120ms중), ACCEL_CURR_SEN_READ_TIME = 5, 현재는 변경 
			#if _EPAS_MODE
			//CalcCurrent_ADCVolt();
			#else
			CalcCurrent_ADCVolt();
			#endif
			
			#if (ZENER_BOARD)
			CalcOverCurrent_ADCVolt();
			//zPrintf(1, "i16ADC_OverCurrentValue = %d : ", (short)i16ADC_OverCurrentValue );
			#endif 
			
			#if _EPAS_MODE
			//SET_BOARD_VCC_STS(i16ADC_VccValue);	
			#else
			SET_BOARD_VCC_STS(i16ADC_VccValue);	
			#endif
			
			#if (TEMP_SEN_MODE == THERM_SEN)
			CalcTemperature(); 
			#else
			ReadTempSensor();						// 센서 반응 속도가 느려 1회만 read : 온도센서 conversion time = 750  ms 로 1초 주기에 한번만 reading 하면 됨 
			#endif

			SenReadState = SEN_IDLE_WAIT;	
			SetTxWaitStatus();						// 센서 reading 이 끝난후 송신상태로 가도록 status set
			
			break;
			
		default :
			break;
	}
								
}	



signed char AccelerationSensor_SecondMaxData[6]={0};

// 절대값 기준 변화량이 큰 max 값을 return
// 세번째 Max data를 사용 하기 위해 이전 Max value를 계속 shift 시킴  
UINT8 Min_MaxModify(UINT8 Num)
{

	//AccelerationSensor_OldMaxData[Num] = AccelerationSensor_Status[Num]; 				// Status : 이전 Max데이타
	// UINT8 -> singed char 로 형 변환
	AccelerationSensor_NewData[Num] = (INT16)AccelerationSensor_getData[Num];					// getData : 현재 데이타
	
/*
	AccelerationSensor_NewData[Num] = AccelerationSensor_getData[Num];			// getData : 현재 데이타 

	if(((AccelerationSensor_OldMaxData[Num]<0))&&(AccelerationSensor_NewData[Num]<0)) 	// '-' --> '-' 변화 
	{
		if(AccelerationSensor_getData[Num] < AccelerationSensor_Status[Num])
			AccelerationSensor_getData[Num] =AccelerationSensor_NewData[Num];	 	// max = -new 
		else
			AccelerationSensor_getData[Num] =AccelerationSensor_OldMaxData[Num];		// max = -old   	 	
	}
	else if((AccelerationSensor_OldMaxData[Num]<0)&&(AccelerationSensor_NewData[Num]>0)) // '-' --> '+' 변화   
	{
		if(AccelerationSensor_getData[Num] < -AccelerationSensor_Status[Num])			// 이전 -를 +로 하여 비교  
			AccelerationSensor_getData[Num] =AccelerationSensor_OldMaxData[Num];		// max = -old  
		else	
			AccelerationSensor_getData[Num] =AccelerationSensor_NewData[Num];		// max = new  
	}				
	else if((AccelerationSensor_OldMaxData[Num]>0)&&(AccelerationSensor_NewData[Num]<0)) // '+' --> '-' 변화  
	{
		if(-AccelerationSensor_getData[Num] < AccelerationSensor_Status[Num])
			AccelerationSensor_getData[Num] =AccelerationSensor_OldMaxData[Num];		// max = old  
		else	
			AccelerationSensor_getData[Num] =AccelerationSensor_NewData[Num];		// max = -new  	 	
	}
	else 	// '+' --> '+' 변화
	{
		if(AccelerationSensor_getData[Num] < AccelerationSensor_Status[Num])
			AccelerationSensor_getData[Num]=AccelerationSensor_OldMaxData[Num];		// max = old  
		else	
			AccelerationSensor_getData[Num]=AccelerationSensor_NewData[Num];		// max = new   
	}
*/

	// AccelerationSensor_OldMaxData[] 바로 이전 Max 값   :  전송후에는 항상 초기화(SetThirdMaxValueInit() ) 를  수행 하여 wakeup 순간의 3번째 진동값을 취하고 있음   
	if (abs((int)AccelerationSensor_OldMaxData[Num])  < abs((int)AccelerationSensor_NewData[Num])) {
		AccelerationSensor_ThirdMaxData[Num] = AccelerationSensor_SecondMaxData[Num];
		AccelerationSensor_SecondMaxData[Num] = AccelerationSensor_OldMaxData[Num];
		AccelerationSensor_OldMaxData[Num] = AccelerationSensor_NewData[Num];
		
	}	
	else if (abs((int)AccelerationSensor_SecondMaxData[Num])  < abs((int)AccelerationSensor_NewData[Num])) {
		AccelerationSensor_ThirdMaxData[Num] = AccelerationSensor_SecondMaxData[Num];
		AccelerationSensor_SecondMaxData[Num] = AccelerationSensor_NewData[Num];
	}		
	else if (abs((int)AccelerationSensor_ThirdMaxData[Num])  < abs((int)AccelerationSensor_NewData[Num]))
			AccelerationSensor_ThirdMaxData[Num] = AccelerationSensor_NewData[Num];
	
	return AccelerationSensor_getData[Num];		// max 값  
}



// Third max data 관련 변수 초기화
void SetThirdMaxValueInit(UINT8 Num)
{
	AccelerationSensor_getData[Num] = 0;
	AccelerationSensor_OldMaxData[Num] = 0;
	AccelerationSensor_SecondMaxData[Num] = 0;
	AccelerationSensor_ThirdMaxData[Num] = 0;
	
}	


void FloatToHEX(float f, char *pStr)
{
	UINT32 n;
	UINT16 w1,w2;

	memcpy(&n, &f, sizeof(float));
	w1=n>>16;
	w2=n;
	
	sprintf(pStr, "%08X", n);

}


#if 1
// 흔들림이 있을때 평균값이 흐르는 문제 발생 
// shift 하여 전체 평균으로 수정 
void MovingAverage(UINT16 in_data, tAVERGAE_DATA* pt_current_avg)
{
    	UINT8  i8;
  	INT32 avg_sum = 0;
    
    	if (pt_current_avg->u8AvgInitFlag == 0) {
    		pt_current_avg->u8AvgInitFlag  = 1;
 		
 		// 처음 읽은 값으로 버퍼를 초기화 시킴 
     		for (i8 = 0; i8 < pt_current_avg->u8AvgCount; i8++) 
			pt_current_avg->u16AvgBuffer[i8] = in_data;		
    	}	
    	
    	// shift 하면서 sum
    	for (i8 = 0; i8 < pt_current_avg->u8AvgCount - 1; i8++) {
	        	avg_sum += pt_current_avg->u16AvgBuffer[i8+1];
	        	pt_current_avg->u16AvgBuffer[i8] = pt_current_avg->u16AvgBuffer[i8+1];
	}
	pt_current_avg->u16AvgBuffer[i8] = in_data;
	
    	avg_sum += in_data;
  	pt_current_avg->fAverage = avg_sum / pt_current_avg->u8AvgCount;

    //return average;
}

#else 
// 버그 : 평균값이 조금식 누적되고 있음
void MovingAverage(UINT16 in_data, tAVERGAE_DATA* pt_current_avg)
{
    	UINT8  i;
  	INT16 itemp;
  	
  	itemp = in_data - pt_current_avg->u16AvgBuffer[0];			// uint16 변수라 음수 계산을 위해 int 형 처리 
    	pt_current_avg->fAverage = pt_current_avg-> fAverage + itemp/pt_current_avg->u8AvgCount;
    
    	for (i = 0; i < pt_current_avg->u8AvgCount - 1; i++)
        	pt_current_avg->u16AvgBuffer[i] = pt_current_avg->u16AvgBuffer[i+1];
        
    	pt_current_avg->u16AvgBuffer[i] = in_data;

    //return average;
}

#endif

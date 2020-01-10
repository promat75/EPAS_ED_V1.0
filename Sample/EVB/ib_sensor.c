
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
	
	memset(pt_current_avg->u16AvgBuffer, 0x00, avg_count*2);	// int ������ ����Ʈ ���� size��
	pt_current_avg->fAverage = 0;

}	



void SensorDataBuff_Init(void)
{
	#if !_ACCEL_USE
	// ���ӵ����� ������ ���׺� ��� ���ۻ��¸� ������ �� �ֵ��� �ϱ� ���Ͽ� �߰��� �ӽ� �ڵ� ��
	AccelerationSensor_getData[1]  = 0x01;						
	AccelerationSensor_getData[3] = 0x02;						
	AccelerationSensor_getData[5] = 0x03;
	ADC_DataBuff[0] =0x01;
	#endif
	
	// ����� ��� ���� �ʴ� ���� ; ���� ���� ���� �ʿ� ???
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
	
	// zPrintf ���� uart ���� ����� �� ���� ����Ÿ�� ���� ��µ� => �׷��� ���� ����Ÿ�� ���� ���
	//zPrintf(1,"accl : %d  %d  %d",(short)AX_data, (short)AY_data, (short)AZ_data);
    	Accelero_OUTPUT_DATA();			
    	OutData(AX_Result);//Y	
    	OutData(AY_Result);//Y
    	OutData(AZ_Result);//Z

	//OutData(ADC_Voltage);
	
	zPrintf(1," %3.1f  ",(float)Real_TempValue);
	
	Voltage_Result = (float)ADC_Voltage/16384;
	Voltage_Result *= 59.2;		// 59.2 = 3.7V * 16(�й� ���� ���)  
	zPrintf(1," %3.1f  \r",(float)Voltage_Result);
#else

	zPrintf(1,"T: %3.1f  ",(float)Real_TempValue);		// �µ� 
	zPrintf(1,"C: %d  ",(short)ADC_Voltage);			// ����
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

	/* ���� ���� �̻� ����� �˶� ������ ���� ��ƾ 
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
	// reset �� ���� �ϰ� �ʱ�ȭ�� ���� �ϵ���
	ACCELSEN_RESET = RST_ON;
	//AppLib_DelayFor1us();			// �ʱ�ȭ delay �̹Ƿ� read �ӵ��� ���� ����  : ���� 1ms ��� 
	AppLib_Delay(1);				// 1ms
	ACCELSEN_RESET = RST_OFF;
	//AppLib_DelayFor1us();			// �ʱ�ȭ delay �̹Ƿ� read �ӵ��� ���� ����  
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
		AppLib_Delay(100);			// 100ms ; �ɿ����� �Ǿ� ���� ����
		ACCELSEN_RESET = RST_OFF;
		
	//	SYS_WdtSet(100);			// ��� ���� ������ ���Ե�  
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
		AppLib_Delay(10);		//--- active mode Ȱ��ȭ�� ��ȿ������ ���� 3sec �ʿ�.
	}
	else
		I2C_DataBuff[1] = 0x07;
		
	//--- I2C Write
	HAL_I2CMaster_SequentialWrite(0x1C, I2C_DataBuff, 2);  
}


UINT32 u32AdcSum = 0;
UINT32 u32AdcVccSum = 0;	// vcc ���� read  
UINT8  u8ADCAvgCount = 0;

 
// 20ms �ֱ� ���� :  ���� 1�ʿ� ���� ���� 32 or 31ȸ avg �ǰ� ����
void GetSensor_CurrentData(void)
{
	INT16 	i16adc_value;
	
	//-----------------------------------------------------
	//SYS_AdcCalibGet_Once(ADC_CHAN_0, 14, &ADC_DataBuff);	
	
	// ���� vcc,gnd ���� ��� �ϴ� SYS_AdcCalibGet() �� �� �� ��鸲��  ���� �߻�   
	SYS_AdcCalibGet(14, &ADC_DataBuff);			// calibration �� adc data�� read  : gi16ADC_VCC �� ���
	
	//SYS_AdcGet(&ADC_DataBuff);				// adc �״�� ����Ÿ�� ���� => 0X5555 ���� ����Ÿ�� 0���� �Ѿ�� ������  
	//-----------------------------------------------------
	
	u32AdcSum += (UINT32)ADC_DataBuff[0];
	u8ADCAvgCount++;
	
	//--------------------------------------
	// vcc ���� ���� reading  
 	SYS_AdcGet_Once(7, &i16adc_value); 	
 	#if _UARTDEBUG_MODE
 	//zPrintf(1,"VCC is %6d\n",i16adc_value);
 	#endif
	u32AdcVccSum += (UINT32)i16adc_value;
	//--------------------------------------

	#if (ZENER_BOARD)
	SYS_AdcSet(ENABLE, 0, ADC_CHAN_1);			// adc channel �� �µ����� reading ���� ����   
	#else

		#if (TEMP_SEN_MODE == THERM_SEN)
		SYS_AdcSet(ENABLE, 0, ADC_CHAN_3);			// adc channel �� �µ����� reading ���� ����   
		#else
		SYS_AdcSet(ENABLE, 0, ADC_CHAN_0);			// adc channel �� ���� ����  
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
	
	// TxTimePeriod : 1 �϶��� ����� ����  
	if ( TxTimePeriod == 1)
		ADC_Voltage = ADC_DataBuff[0];
	else {
		MovingAverage(ADC_DataBuff[0], &gtCurrentAvg );	// �ѹ��� ����� ���� => �۽� �ֱ� �� �ѹ��� ���,  4ȸ ��ս� 4���� �۽��� �̷������ ���� ��հ�
		ADC_Voltage = (UINT16)gtCurrentAvg.fAverage;	
	}

	//SYS_AdcSet(ENABLE, 0, ADC_CHAN_1);			// adc channel �� Thermistor �� ����  

	u32AdcSum = 0;
	u8ADCAvgCount = 0;	

}


UINT32 u32AdcOverSum = 0;	// vcc ���� read  
UINT8  u8ADCOverAvgCount = 0;

void GetSensor_OverCurrentData(void)
{
	
	// ���� vcc,gnd ���� ��� �ϴ� SYS_AdcCalibGet() �� �� �� ��鸲��  ���� �߻�   
	SYS_AdcCalibGet(14, &ADC_DataBuff);				// calibration �� adc data�� read  : gi16ADC_VCC �� ���
	
	u32AdcOverSum += (UINT32)ADC_DataBuff[0];
	u8ADCOverAvgCount++;

#if (TEMP_SEN_MODE == THERM_SEN)
	SYS_AdcSet(ENABLE, 0, ADC_CHAN_3);				// adc channel �� �µ����� reading ���� ����   
#else
	SYS_AdcSet(ENABLE, 0, ADC_CHAN_0);				// adc channel �� ���� ����  
#endif	

}


void CalcOverCurrent_ADCVolt(void)
{

	u16ADC_OverCurrentValue = (UINT32)u32AdcOverSum / u8ADCOverAvgCount;	

	//-------------------------------------------------------------------------------------	
	// ���Է� adc ä�� ����Ÿ ���� 
	if (u8BoardStatus & BST_OVER_CURR) {
		if (u8RecoveryZenorADC > u16ADC_OverCurrentValue) // histerisys ó�� 
			u8BoardStatus &= ~BST_OVER_CURR;
		else 
			ADC_Voltage = u16ADC_OverCurrentValue; 	// histerysys �� �ѱ� �������� ��� ���Է� ���� data ����
	}	
	else {
		if (u8StartZenorADC <= u16ADC_OverCurrentValue) {
			u8BoardStatus |= BST_OVER_CURR;			// GUI���� �Ķ���� ���� ����� flag set
			ADC_Voltage = u16ADC_OverCurrentValue; 	// ������ ADC ����Ÿ�� ������ 
		}	
	}
	//-------------------------------------------------------------------------------------	
	// PM_OVER_CURR_MODE �϶��� ���Է� ������ �帣�� �־� �� ��ƾ���� ó�� �ϰԵ� 
	// if (Porduct_Mode & PM_OVER_CURR_MODE)
	//	ADC_Voltage = u16ADC_OverCurrentValue; 
	
	u32AdcOverSum = 0;
	u8ADCOverAvgCount = 0;	

}


void GetSensor_AccelData(void)
{
	#if _ACCEL_USE
	UINT8 u8status;
		
	if (u8I2CSenStatus & 0x80) {				// u8I2CSenStatus == 0x80 : ���� ���� ���� ��Ȳ  
		AccelSensor_Init();	
		return;
	}	
								
	ConfigBit_value = (GS_COMMAND_WRITE | 0x28);

	//----------------------------------------------------------------------------------------
	// HAL_I2CIntSet(,,,20) 1ȸ read : 360us
	// HAL_I2CIntSet(,,,28) 1ȸ read : 440us
	u8status = HAL_I2CMaster_RandomRead(0x1C, ConfigBit_value, AccelerationSensor_getData, 0x06); 
	//----------------------------------------------------------------------------------------
	
	// Accl ������ �˼����� ������ ������ �� ��� reset ����� ����
	if (u8status == 0x80) {	
		zPrintf(1,"Accelsen occur!\n");
		ACCELSEN_RESET = RST_ON;
		SYS_WdtSet(100);
		while(1);		
	}	

		
	// 1�� ���� �о� ���� ���߿��� �ְ� ���� ǥ��, 1�� ���� �۽��� ������ �ʱ�ȭ�� 
	// Min_MaxModify() �� ���� �ϸ� AccelerationSensor_getData[] ���� Max ���� �����
	Min_MaxModify(1);		// AccelerationSensor_Status(old) �� AccelerationSensor_getData(new) �� ���� ��ȭ�� ū���� return
	Min_MaxModify(3);
	Min_MaxModify(5);
	#endif

}	


//--- �µ������� NTC ���̽����ϰ�� Read Task�� SEN_IDLE_WAIT�� �����Ѵ�. 
#if (TEMP_SEN_MODE == INT_SEN)
UINT8	SenReadState = SEN_INIT_TEMP; 
#else
UINT8	SenReadState = SEN_IDLE_WAIT; 
#endif

UINT8	SenReadCount = 0; 
// wakeup 100ms ���� ��ȸ ���� ������ üũ  
void CheckSensorReadCount(void)
{
	if (SenReadCount++ >= ACCL_CURR_READ_CNT) {	// 100ms(4ȸ, 20ms) ���� ���� read  
		SenReadState = SEN_CALC_DATA;
		SenReadCount = 0;
	}
	else
		SenReadState = SEN_READ_WAIT;

	//#if !_EPAS_MODE
	AppLib_VirtualTimerSet(WAIT_TIMER_SHORT, ACCEL_CURR_SEN_READ_TIME);		
	//#endif
}	

// sleep mode ���� : sleep wakeup ��  sen read ���� ����    
// wakeup ���� : �۽��ֱ�� �� ������ 1�ʿ� �ѹ��� read ���� ���� �ϵ��� 


// - sleep mode wakeup time 100ms ���� ���� ����Ÿ�� �о� 100ms ������ ����� ����  
// - wakeup �ð� ���� ���� read�� �Ϸ� �Ͽ� ��� �� ����Ÿ ó���� �Ǿ�ߵ� 
// - ���� ������ ���� �۽� �ֱ� ������  ���� Max ���� ���� 
// ����, ����, �µ� ����(conversion time �� ��� 1�� �Ǵ� wakeup�� �ѹ�) �� ��� ó��  
void SensorReadingProcess(void)
{

	extern INT16 adc_buf[32];

	
	//static UINT8 read_cnt = 0;
	
	switch(SenReadState) {
		case SEN_INIT_TEMP :
			// read vcc  and clear data  
			#if _EPAS_MODE
			SYS_AdcSet(ENABLE, 0, ADC_CHAN_3);			// adc channel �� �µ����� reading ���� ����   
			#else
			GetSensor_CurrentData();
			CalcCurrent_ADCVolt();			
			#endif
			
			#if (TEMP_SEN_MODE == INT_SEN)
				// temp sensor �� �ѹ� �о� �����Ⱚ �ʱ�ȭ ����   
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
			// sleep wakeup, tx finish ���ۿ���  next state�� �̵� 
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
			
		case SEN_READ_DATA :						// 5ms �ֱ�� 100ms �ȿ� 11ȸ ���� ���� ����, 
			//---------------------------------------------
			// ��������  adc : ADC ä�� ���� �ð��� ���� ���̿� ���� ������ ����   
			#if _EPAS_MODE
			SYS_AdcSet(ENABLE, 0, ADC_CHAN_3);			// adc channel �� �µ����� reading ���� ����   
			#if _EPAS_MODE
			zPrintf(1,"SEN_READ_DATA\n");
			#endif
			#else
			GetSensor_CurrentData();
			#endif
			
			// ��������  
			#if _ACCEL_USE
			AccelSensor_pd_control(AccelSensor_Activemode);			// AccelSensor Active mode.. By Kong
			GetSensor_AccelData();															// 1ȸ 550us  - ���� ������ ���� �ֱ� ���� max ���� ��� �����Ͽ� ���� 
			AccelSensor_pd_control(AccelSensor_Powerdownmode);	// AccelSensor Powerdown mode.. By Kong
			#endif
		
			#if (ZENER_BOARD)
			GetSensor_OverCurrentData();
			#endif
					
			// �µ����� adc  : ��̽���
			#if (TEMP_SEN_MODE == THERM_SEN)
			GetSensor_TempData();					//  ���� ������ ���� �ӵ�(���� �ڵ�), ��̽��͸� adc �� ���� read �� ��� ������ �о� ��� ��� 
			#endif
			
			CheckSensorReadCount();				// 5ms �ֱ��  100ms ���� ������ �ִ� Ø�� ���� �Ǿ� ����, 10ȸ��  SEN_CALC_DATA(����� ����)    ���� 

			break;
			
		case SEN_CALC_DATA :						// sleep mode = 26ms ����(120ms��), ACCEL_CURR_SEN_READ_TIME = 5, ����� ���� 
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
			ReadTempSensor();						// ���� ���� �ӵ��� ���� 1ȸ�� read : �µ����� conversion time = 750  ms �� 1�� �ֱ⿡ �ѹ��� reading �ϸ� �� 
			#endif

			SenReadState = SEN_IDLE_WAIT;	
			SetTxWaitStatus();						// ���� reading �� ������ �۽Ż��·� ������ status set
			
			break;
			
		default :
			break;
	}
								
}	



signed char AccelerationSensor_SecondMaxData[6]={0};

// ���밪 ���� ��ȭ���� ū max ���� return
// ����° Max data�� ��� �ϱ� ���� ���� Max value�� ��� shift ��Ŵ  
UINT8 Min_MaxModify(UINT8 Num)
{

	//AccelerationSensor_OldMaxData[Num] = AccelerationSensor_Status[Num]; 				// Status : ���� Max����Ÿ
	// UINT8 -> singed char �� �� ��ȯ
	AccelerationSensor_NewData[Num] = (INT16)AccelerationSensor_getData[Num];					// getData : ���� ����Ÿ
	
/*
	AccelerationSensor_NewData[Num] = AccelerationSensor_getData[Num];			// getData : ���� ����Ÿ 

	if(((AccelerationSensor_OldMaxData[Num]<0))&&(AccelerationSensor_NewData[Num]<0)) 	// '-' --> '-' ��ȭ 
	{
		if(AccelerationSensor_getData[Num] < AccelerationSensor_Status[Num])
			AccelerationSensor_getData[Num] =AccelerationSensor_NewData[Num];	 	// max = -new 
		else
			AccelerationSensor_getData[Num] =AccelerationSensor_OldMaxData[Num];		// max = -old   	 	
	}
	else if((AccelerationSensor_OldMaxData[Num]<0)&&(AccelerationSensor_NewData[Num]>0)) // '-' --> '+' ��ȭ   
	{
		if(AccelerationSensor_getData[Num] < -AccelerationSensor_Status[Num])			// ���� -�� +�� �Ͽ� ��  
			AccelerationSensor_getData[Num] =AccelerationSensor_OldMaxData[Num];		// max = -old  
		else	
			AccelerationSensor_getData[Num] =AccelerationSensor_NewData[Num];		// max = new  
	}				
	else if((AccelerationSensor_OldMaxData[Num]>0)&&(AccelerationSensor_NewData[Num]<0)) // '+' --> '-' ��ȭ  
	{
		if(-AccelerationSensor_getData[Num] < AccelerationSensor_Status[Num])
			AccelerationSensor_getData[Num] =AccelerationSensor_OldMaxData[Num];		// max = old  
		else	
			AccelerationSensor_getData[Num] =AccelerationSensor_NewData[Num];		// max = -new  	 	
	}
	else 	// '+' --> '+' ��ȭ
	{
		if(AccelerationSensor_getData[Num] < AccelerationSensor_Status[Num])
			AccelerationSensor_getData[Num]=AccelerationSensor_OldMaxData[Num];		// max = old  
		else	
			AccelerationSensor_getData[Num]=AccelerationSensor_NewData[Num];		// max = new   
	}
*/

	// AccelerationSensor_OldMaxData[] �ٷ� ���� Max ��   :  �����Ŀ��� �׻� �ʱ�ȭ(SetThirdMaxValueInit() ) ��  ���� �Ͽ� wakeup ������ 3��° �������� ���ϰ� ����   
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
	
	return AccelerationSensor_getData[Num];		// max ��  
}



// Third max data ���� ���� �ʱ�ȭ
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
// ��鸲�� ������ ��հ��� �帣�� ���� �߻� 
// shift �Ͽ� ��ü ������� ���� 
void MovingAverage(UINT16 in_data, tAVERGAE_DATA* pt_current_avg)
{
    	UINT8  i8;
  	INT32 avg_sum = 0;
    
    	if (pt_current_avg->u8AvgInitFlag == 0) {
    		pt_current_avg->u8AvgInitFlag  = 1;
 		
 		// ó�� ���� ������ ���۸� �ʱ�ȭ ��Ŵ 
     		for (i8 = 0; i8 < pt_current_avg->u8AvgCount; i8++) 
			pt_current_avg->u16AvgBuffer[i8] = in_data;		
    	}	
    	
    	// shift �ϸ鼭 sum
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
// ���� : ��հ��� ���ݽ� �����ǰ� ����
void MovingAverage(UINT16 in_data, tAVERGAE_DATA* pt_current_avg)
{
    	UINT8  i;
  	INT16 itemp;
  	
  	itemp = in_data - pt_current_avg->u16AvgBuffer[0];			// uint16 ������ ���� ����� ���� int �� ó�� 
    	pt_current_avg->fAverage = pt_current_avg-> fAverage + itemp/pt_current_avg->u8AvgCount;
    
    	for (i = 0; i < pt_current_avg->u8AvgCount - 1; i++)
        	pt_current_avg->u16AvgBuffer[i] = pt_current_avg->u16AvgBuffer[i+1];
        
    	pt_current_avg->u16AvgBuffer[i] = in_data;

    //return average;
}

#endif


/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2011.
	- Date		: 2016-11-15
	- Version		: V2.6

	- File			: ib_ir_sen.c
	-                       : 적외선 온도센서 TMP007 제어 코드
*******************************************************************************/


#include "ib_main_sen.h"
//#include "ib_ir_sen.h"

INT16 IR_Temp;

// Acceleration Sensor Init//
void TMP007_Init(void)
{
	UINT16 u16read_data;
	
	u8I2CSenStatus = 0;
	// 0x02, 0x1940
	u8I2CSenStatus = I2C_uInt16Write(TMP007_I2CADDR, TMP007_CONFIG, 
	                                                       (TMP007_CFG_MODEON | TMP007_CFG_ALERTEN |  TMP007_CFG_TRANSC | TMP007_CFG_1SAMPLE));
	// 0x05,  0xC000                                                   
	u8I2CSenStatus = I2C_uInt16Write(TMP007_I2CADDR, TMP007_STATMASK, 
	                                                       (TMP007_STAT_ALERTEN |TMP007_STAT_CRTEN));
 	
	u8I2CSenStatus= I2C_uInt16Read(TMP007_I2CADDR, TMP007_DEVID, &u16read_data);
  	if (u16read_data != 0x78) 
		zPrintf(1, "\n >> TMP007 I2C Initializse Fail !!!");

	#if _ACCEL_USE
	if (u8I2CSenStatus & 0x80) {
		zPrintf(1, "\n >> TMP007  I2C function Fail !!!");
		ACCELSEN_RESET = RST_ON;
		AppLib_Delay(100);			// 100ms ; 핀연결이 되어 있지 않음
		ACCELSEN_RESET = RST_OFF;
	}		
	#endif
  	
  	IR_Temp= ReadIRSensor(TMP007_TOBJ);
    	zPrintf(1, "\n >> IR_Temp = %3.1f", (float)IR_Temp/2);
	
    	IR_Temp= ReadIRSensor(TMP007_TDIE);
   	zPrintf(1, "\n >> IR_Temp = %3.1f", (float)IR_Temp/2);
  		
}


INT16 ReadIRSensor(UINT8 u8cmd)
{
	UINT16 u16read_data;
	float fcelsius = 0.5f;

  	u8I2CSenStatus= I2C_uInt16Read(TMP007_I2CADDR, u8cmd, &u16read_data);
	
	if (u16read_data & 0x8000) {
		// negatvie temp
	         u16read_data = u16read_data ^ 0x7FFF + 0x02;	// 2's compliments를 위해 +1온도값을  +2배 온도값으로 
            	//temp_c_shift  = ((temp_c_2s)>>2);
            	fcelsius = -0.5f;								// 2배 온도값으로 
        }    	
            	
        fcelsius = ((float)u16read_data * fcelsius) * 0.03125f;		// 2배 온도값으로 

	return ((INT16)fcelsius);
}


UINT8 I2C_uInt16Write(UINT8 addr, UINT8 u8cmd, UINT16 word_data)
{
	UINT8 u8buffer[2];
		
	u8buffer[0] = (UINT8)(word_data >> 8);                // MSB first       
	u8buffer[1] = (UINT8)word_data;             	// LSB
	
	return (HAL_I2CMaster_RandomWrite(addr, u8cmd, u8buffer, 2));  

}	


UINT8 I2C_uInt16Read(UINT8 addr, UINT8 u8cmd, UINT16* u16read_data)
{
	UINT8 u8buffer[2];
	UINT8 u8status;

	u8status = HAL_I2CMaster_RandomRead(addr, u8cmd, u8buffer, 0x02); 
	
	*u16read_data = (UINT16)u8buffer[0] << 8 | u8buffer[1];                // MSB first       
	
	return u8status;
}	



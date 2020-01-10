
#ifndef __MG2470_IB_IR_SEN_H__
#define __MG2470_IB_IR_SEN_H__


//#include "ib_main_sen.h"

//-----------------------------------------------
// IR Sensor TMP007
//-----------------------------------------------



#define TMP007_VOBJ       			0x00
#define TMP007_TDIE       			0x01
#define TMP007_CONFIG     			0x02
#define TMP007_TOBJ       			0x03
#define TMP007_STATUS     			0x04
#define TMP007_STATMASK   			0x05

#define TMP007_CFG_RESET    		0x8000
#define TMP007_CFG_MODEON   		0x1000
#define TMP007_CFG_1SAMPLE  		0x0000
#define TMP007_CFG_2SAMPLE  		0x0200
#define TMP007_CFG_4SAMPLE  		0x0400
#define TMP007_CFG_8SAMPLE  		0x0600
#define TMP007_CFG_16SAMPLE 		0x0800
#define TMP007_CFG_ALERTEN  		0x0100
#define TMP007_CFG_ALERTF   		0x0080
#define TMP007_CFG_TRANSC  		0x0040

#define TMP007_STAT_ALERTEN 		0x8000
#define TMP007_STAT_CRTEN   		0x4000

#define TMP007_I2CADDR 			0x40			// ADR0, ADR1 = 0
#define TMP007_DEVID 				0x1F




void TMP007_Init(void);
UINT8 I2C_uInt16Write(UINT8 addr, UINT8 u8cmd, UINT16 word_data);
UINT8 I2C_uInt16Read(UINT8 addr, UINT8 u8cmd, UINT16* u16read_data);
INT16 ReadIRSensor(UINT8 u8cmd);


#endif	// #ifndef __MG2470_IB_IR_SEN_H__
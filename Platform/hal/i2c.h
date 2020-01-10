
/*******************************************************************************
	[2013-05-27] Compatible with V2.1
*******************************************************************************/


#ifndef __MG2470_I2C_H__
#define __MG2470_I2C_H__

#define	I2C_SCL		MG2470_PORT16_I2CSCL
#define	I2C_SDA	MG2470_PORT17_I2CSDA

#define 	I2C_CLK_ON			(PERI_CLK_STP1 |= BIT7)
#define	I2C_CLK_OFF		(PERI_CLK_STP1 &= ~BIT7)
#define	I2C_CLK_STATE		(PERI_CLK_STP1 & BIT7)

#define	I2C_CODE	0x1100

#define	RETURN_I2C_SUCCESS				0x00
#define	RETURN_I2C_CLOCK_OFF				0x10
#define	RETURN_I2C_INVALID_PARAMETER	0x11
#define	RETURN_I2C_NACK_FROM_SLAVE		0x12
#define	RETURN_I2C_NACK_FROM_MASTER	0x13

#define	I2C_WRITE_BIT		0x00
#define	I2C_READ_BIT		0x01

#define	I2C_CORE_OFF		0x80
#define	I2C_INT_ON			0x40
#define	I2C_MASTER			0x20
#define	I2C_START			0x10
#define	I2C_REP_START		0x08
#define	I2C_NACK			0x04
#define	I2C_RXFIFO_RST		0x02		// auto-cleared
#define	I2C_TXFIFO_RST		0x01		// auto-cleared

extern	UINT8	gu8I2CByteTransferIntCount;
extern	UINT8	gu8I2CNackedFlag;
extern	UINT8	gu8I2CMasterPreReadLength;
extern	UINT8*	gpu8I2CMasterReadData;

void HAL_I2CClockOn(UINT8 u8On);
UINT8 HAL_I2CIntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Mode, UINT8 u8Speed);
UINT8 HAL_I2CMaster_RandomWrite(UINT8 u8DeviceAddr, UINT8 u8WordAddr, UINT8* pu8DataBuff, UINT8 u8DataLen);
UINT8 HAL_I2CMaster_RandomRead(UINT8 u8DeviceAddr, UINT8 u8WordAddr, UINT8* pu8DataBuff, UINT8 u8DataLen);
UINT8 HAL_I2CMaster_SequentialWrite(UINT8 u8DeviceAddr, UINT8* pu8DataBuff, UINT8 u8DataLen);
UINT8 HAL_I2CMaster_SequentialRead(UINT8 u8DeviceAddr, UINT8* pu8DataBuff, UINT8 u8DataLen);



#endif	// #ifndef __MG2470_I2C_H__



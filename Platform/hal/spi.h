
/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_SPI_H__
#define __MG2470_SPI_H__

#define	SPI_RXBUF_SIZE		0x0040

#define	SPI_HW_FIFO_SIZE	16		// Fixed value. Do not modify.
#define	SPI_CSN		MG2470_PORT37_SPICSN
#define	SPI_CLK		MG2470_PORT36_SPICLK
#define	SPI_DO		MG2470_PORT35_QUADYB_SPIDO_T1
#define	SPI_DI		MG2470_PORT34_QUADYA_SPIDI_T0

#define	SPI_MASTER			BIT4
#define	SPI_SLAVE			0
#define	SPI_POLARITY_HIGH	BIT3
#define	SPI_POLARITY_LOW	0
#define	SPI_PHASE_1ST		BIT2
#define	SPI_PHASE_2ND		0

#define	SPI_SPEED_1MHz	0
#define	SPI_SPEED_500KHz	1
#define	SPI_SPEED_250KHz	2
#define	SPI_SPEED_125KHz	3
#define	SPI_SPEED_62KHz	4
#define	SPI_SPEED_31KHz	5
#define	SPI_SPEED_15KHz	6
#define	SPI_SPEED_8KHz		7
#define	SPI_SPEED_4KHz		8
#define	SPI_SPEED_2KHz		9

#define	SPI_CLK_ON		(PERI_CLK_STP0 |= BIT7)
#define	SPI_CLK_OFF	(PERI_CLK_STP0 &= ~BIT7)
#define	SPI_CLK_STATE	(PERI_CLK_STP0 & BIT7)

#define	SPI_CODE	0x0B00

#define	RETURN_SPI_SUCCESS				0x00
#define	RETURN_SPI_CLOCK_OFF				0x10
#define	RETURN_SPI_INVALID_PARAMETER	0x11
#define	RETURN_SPI_GPIO_CLOCK_OFF		0x12
	
extern	UINT8	gu8SpiMaster_IntFlag;
extern	UINT8	gu8SpiMaster_RxData;
extern	UINT8	gau8SpiSlave_RxBuff[];
extern	UINT16	gu16SpiSlave_RxWrIdx;
extern	UINT16	gu16SpiSlave_RxRdIdx;
extern	UINT8	gu8SpiSlave_NextTxData;

void HAL_SpiClockOn(UINT8 u8On);
void HAL_SpiMasterCSnSelect(void);
void HAL_SpiMasterCSnSet(UINT8 u8High);
UINT8 HAL_SpiIntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8Mode, UINT8 u8Speed);	
UINT8 SYS_SpiMasterPut(UINT8 u8Data);
UINT8 SYS_SpiMasterPutMultiBytes(UINT8* pu8DataBuff, UINT8 u8PutLen);
void SYS_SpiSlavePut(UINT8 u8NextSendData);
UINT8 SYS_SpiSlavePutMultipleBytes(UINT8* pu8DataBuff, UINT8 u8PutLen);
UINT8 SYS_SpiSlaveGet(UINT8* pu8Data);

#endif	// #ifndef __MG2470_SPI_H__



/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_IRTX_NEC_H__
#define __MG2470_IRTX_NEC_H__

#define 	IRTX_CLK_ON		(PERI_CLK_STP1 |= BIT6)
#define 	IRTX_CLK_OFF		(PERI_CLK_STP1 &= ~BIT6)
#define 	IRTX_CLK_STATE		(PERI_CLK_STP1 & BIT6)

#define	IRTX_NEC_ERROR_CODE	0x1000

#define	RETURN_IRTX_NEC_SUCCESS			0x00
#define	RETURN_IRTX_NEC_CLOCK_OFF		0x10
#define	RETURN_IRTX_NEC_CONT_ERROR		0x11
#define	RETURN_IRTX_NEC_DATA_ERROR		0x12

extern	UINT8 gu8IRTxIntFlag;

void HAL_IRTxClockOn(UINT8 u8On);
UINT8 HAL_IRTxNecIntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8CarrEna, UINT8 u8CustomCode);
UINT8 HAL_IRTxNecTxData(UINT8 u8DataCode);
UINT8 HAL_IRTxNecTxCont(void);

#endif	// #ifndef __MG2470_IRTX_NEC_H__


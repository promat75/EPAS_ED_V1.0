
/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_ADC_H__
#define __MG2470_ADC_H__
#define __MG2470_ADC_CALIBRATION_INCLUDE_

#define	ADC_CHAN_0						0
#define	ADC_CHAN_1						1
#define	ADC_CHAN_2						2
#define	ADC_CHAN_3						3
#define	ADC_CHAN_DIFFERENCE_01			4
#define	ADC_CHAN_DIFFERENCE_23			5
#define	ADC_CHAN_TEMPERATURE			6
#define	ADC_CHAN_VCC					7
#define	ADC_CHAN_GND					8

#define	ADC_RESOLUTION_BIT_10				10
#define	ADC_RESOLUTION_BIT_11				11
#define	ADC_RESOLUTION_BIT_12				12
#define	ADC_RESOLUTION_BIT_13				13
#define	ADC_RESOLUTION_BIT_14				14
#define	ADC_RESOLUTION_BIT_15				15

#define	ADC_REFERENCE_BANDGAP			0		// Bandgap reference(1.25V)
#define	ADC_REFERENCE_RESERVED			1
#define	ADC_REFERENCE_AVDD3V			2		// AVDD3V1/3
#define	ADC_REFERENCE_VOL_REG			3		// Internal voltage regulator

#define	ADC_SAMPLE_RATE_1MHz			0
#define	ADC_SAMPLE_RATE_2MHz			1
#define	ADC_SAMPLE_RATE_4MHz			2
#define	ADC_SAMPLE_RATE_8MHz			3

#define	ADC_OVER_SAMPLE_RATIO_32X		0
#define	ADC_OVER_SAMPLE_RATIO_64X		1
#define	ADC_OVER_SAMPLE_RATIO_128X		2
#define	ADC_OVER_SAMPLE_RATIO_256X		3

#define	ADC_CODE						0x0100

#define	RETURN_ADC_SUCCESS				0x00
#define	RETURN_ADC_INVALID_PARAMETER		0x11
#define	RETURN_ADC_INVALID_CHANNEL		0x12
#define	RETURN_ADC_READ_FAILURE			0x13
#define	RETURN_ADC_OUT_OF_RANGE			0x14

void HAL_AdcDecimatorReset(void);
UINT8 HAL_AdcSet(UINT8 u8Ena, UINT8 u8Ref, UINT8 u8AdcChan);
UINT8 HAL_AdcSampleRateSet(UINT8 u8SampleRate, UINT8 u8OverSampleRatio);
UINT8 HAL_AdcBuffModeSet(UINT8 u8FifoEna, UINT8 u8BufferCnt);
INT16 HAL_AdcGet(void);
UINT8 HAL_AdcDataCountGet(void);
UINT8 SYS_AdcGet(INT16* pi16ReadADC);
UINT8 SYS_AdcSet(UINT8 u8Ena, UINT8 u8Ref, UINT8 u8AdcChan);
UINT8 SYS_AdcGet_Once(UINT8 u8AdcChan, INT16 *pi16ReadADC);

#ifdef __MG2470_ADC_CALIBRATION_INCLUDE_
	UINT8 SYS_AdcCalib(INT16 i16MaxADC, INT16 i16MinADC, INT16 i16InputADC, UINT8 u8ResBitNum, INT16 *pi16CalibADC);
	UINT8 SYS_AdcGet_VCCForCalib(void);
	UINT8 SYS_AdcGet_GNDForCalib(void);
	UINT8 SYS_AdcCalibGet(UINT8 u8ResBitNum, INT16 *pi16CalibADC);
	UINT8 SYS_AdcCalibGet_Once(UINT8 u8AdcChan, UINT8 u8ResBitNum, INT16 *pi16CalibADC);
#endif	// #ifdef __MG2470_ADC_CALIBRATION_INCLUDE_

#endif	// #ifndef __MG2470_ADC_H__



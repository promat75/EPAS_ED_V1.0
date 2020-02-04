
#ifndef __MG2470_VERIFY_ADC_H__
#define __MG2470_VERIFY_ADC_H__

typedef struct
{
	INT16	i16Max;
	INT16	i16Min;
	INT16	i16Avg;
	INT32	i32Sum;	
	UINT16	u16MaxMinGap;
	UINT16	u16NumOfSample;
} tADC_RESULT;

extern	tADC_RESULT		gtADC0;
extern	tADC_RESULT		gtADC1;
	
INT16 AppLib_MaxValueGet(INT16 i16Value, INT16 i16CurrentMax);
INT16 AppLib_MinValueGet(INT16 i16Value, INT16 i16CurretMin);


void AppLib_AdcResultClear(tADC_RESULT* ptADC);
void AppLib_AdcResultUpdate(tADC_RESULT* ptADC, INT16 i16ReadValue);
void AppLib_AdcResultCalculate(tADC_RESULT* ptADC);
void AppLib_AdcResultDisplay(tADC_RESULT* ptADC, UINT8 u8All);


#endif	// __MG2470_VERIFY_ADC_H__

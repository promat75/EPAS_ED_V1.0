

#include "INCLUDE_TOP.h"
#include "app_adc.h"
#include "util_app.h"

tADC_RESULT		gtADC0;
tADC_RESULT		gtADC1;

INT16 AppLib_MaxValueGet(INT16 i16Value, INT16 i16CurrentMax)
{
	if(i16Value > i16CurrentMax)
	{
		return i16Value;
	}
	else
	{
		return i16CurrentMax;
	}
}

INT16 AppLib_MinValueGet(INT16 i16Value, INT16 i16CurrentMin)
{
	if(i16Value < i16CurrentMin)
	{
		return i16Value;
	}
	else
	{
		return i16CurrentMin;
	}
}

void AppLib_AdcResultClear(tADC_RESULT* ptADC)
{
	ptADC->i16Avg = 0;
	ptADC->i16Max = -32768;
	ptADC->i16Min = 32767;
	ptADC->i32Sum = 0;
	ptADC->u16MaxMinGap = 0;
	ptADC->u16NumOfSample = 0;
}

void AppLib_AdcResultUpdate(tADC_RESULT* ptADC, INT16 i16ReadValue)
{
	if(ptADC->u16NumOfSample != 0xFFFF)
	{
		if(i16ReadValue > ptADC->i16Max)
		{
			ptADC->i16Max = i16ReadValue;
		}

		if(i16ReadValue < ptADC->i16Min)
		{
			ptADC->i16Min = i16ReadValue;
		}

		ptADC->u16NumOfSample++;

		ptADC->i32Sum += i16ReadValue;

		ptADC->u16MaxMinGap = ptADC->i16Max - ptADC->i16Min;
	}
}

void AppLib_AdcResultCalculate(tADC_RESULT* ptADC)
{
	ptADC->i16Avg = ptADC->i32Sum / ptADC->u16NumOfSample;
}

void AppLib_AdcResultDisplay(tADC_RESULT* ptADC, UINT8 u8All)
{
	if(u8All)
	{
		zPrintf(1, " :Avg=%7d", ptADC->i16Avg);
	}
	zPrintf(1, " :Min=%7d", ptADC->i16Min);
	zPrintf(1, " :Max=%7d", ptADC->i16Max);
	zPrintf(1, " :Gap=%6u", ptADC->u16MaxMinGap);

	if(u8All)
	{
		zPrintf(1, " :Sum=%ld", ptADC->i32Sum);
		zPrintf(1, " :NumOfSample=%7u", ptADC->u16NumOfSample);
	}
}

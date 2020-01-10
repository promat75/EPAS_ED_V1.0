
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "adc.h"
#include "clock.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Constants which indicates the for() loop counter for waiting to complete passing the internal filter.
///	\n	The loop counter is dependent on the MCU clock.
///	\n	The index of the array indicates the MCU clock mode.
///			\n 	0 = 16 MHz
///			\n 	1 = 8 MHz
///			\n 	2 = 4 MHz
///			\n 	3 = 2 MHz
///			\n 	4 = 1 MHz
///			\n 	5 = 500 KHz
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	code gcau16MaxWait_FilterPass[6] = 		// [MCU Clock Mode]
	{
		360, 170, 90, 45, 21, 11
	};

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Global variable which has the ADC data for VCC channel. This is used for calibration.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
INT16	gi16ADC_VCC = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Global variable which has the ADC data for GND channel. This is used for calibration.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
INT16	gi16ADC_GND = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Resets decimator blocks in MG2470B. 
///	After calling HAL_AdcSet(), this should be called to remove remaining data for the previous 
///	ADC channel. When this is called, ADC value is not correct for 2ms because some time is 
///	needed for passing filters. So, after that time, ADC should be read
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_AdcDecimatorReset(void)		// Decimator Reset
{
	xPHY_SW_RSTB &= ~BIT6;	// bit[6]=0. Enable reset. Active low.
	_nop_();
	_nop_();
	xPHY_SW_RSTB |= BIT6;	// bit[6]=1. Disable reset. Active low.
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Sets an ADC channel and its reference.
///	
///	@param	u8Ena 		: 0=Disable, 1=Enable.
///	@param	u8Ref 		: 0=Band gap reference(1.25V), 2=1/3 of AVDD3V, 3=Internal voltage regulator output.
///	@param	u8AdcChan	: ADC channel to use.
///							\n 	0=ACH0.
///							\n 	1=ACH1. 
///							\n 	2=ACH2. 
///							\n 	3=ACH3. 
///							\n 	4=difference of ACH0 and ACH1.
///							\n 	5=difference of ACH2 and ACH3.
///							\n 	6=Temperature Sensor.
///							\n 	7=Battery Monitoring.
///							\n 	8=GND(for Calibration).
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_AdcSet(UINT8 u8Ena, UINT8 u8Ref, UINT8 u8AdcChan)
{
	if(u8AdcChan > ADC_CHAN_GND)
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_CHANNEL);
		return RETURN_ADC_INVALID_CHANNEL;
	}
	if( (u8Ref > ADC_REFERENCE_VOL_REG) || (u8Ref == ADC_REFERENCE_RESERVED) )
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_PARAMETER);
		return RETURN_ADC_INVALID_PARAMETER;
	}

	if(u8Ena)
	{
		xPDMON |= BIT2;			// bit[2]=1. Enable ADC current.
		xPHY_CLK_EN1 |= 0x0C;	// bit[3:2]=2'b11
		xADCCNF1 |= BIT7;		// bit[7]=1. ADCEN
		xADCCNF1 = (xADCCNF1 & 0xF0) | u8AdcChan;		// bit[3:0]
		xADCCNF1 = (xADCCNF1 & 0x9F) | (u8Ref << 5);		// bit[6:5]
	}
	else
	{
		xPDMON &= ~BIT2;		// bit[2]=0. Disable ADC current.
		xADCCNF1 &= ~BIT7;		// bit[7]=0. ADCEN
		xPHY_CLK_EN1 &= ~0x0C;	// bit[3:2]=2'b00
	}

	return RETURN_ADC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Sets a sampling rate.
///	
///	@param	u8SampleRate			: 0=1MHz, 1=2MHz, 2=4MHz, 3=8MHz. Reset value is 2.
///	@param	u8OverSampleRatio	: 0=32x, 1=64x, 2=128x, 3=256x. Reset value is 3.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_AdcSampleRateSet(UINT8 u8SampleRate, UINT8 u8OverSampleRatio)
{
	if( 	(u8SampleRate > ADC_SAMPLE_RATE_8MHz)
	|| 	(u8OverSampleRatio > ADC_OVER_SAMPLE_RATIO_256X) )
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_PARAMETER);
		return RETURN_ADC_INVALID_PARAMETER;
	}

	xPHY_ADC_CLK_SEL = (xPHY_ADC_CLK_SEL & 0xFC) | u8SampleRate;
	xADCCNF2 = (xADCCNF2 & 0xF3) | (u8OverSampleRatio << 2);

	return RETURN_ADC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Sets whether FIFO mode is used.
///	
///	@param	u8FifoEna		: 0=Non-FIFO mode, 1=FIFO mode.
///	@param	u8BufferCnt	: Number of buffers to be used. 1~14.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_AdcBuffModeSet(UINT8 u8FifoEna, UINT8 u8BufferCnt)
{
	UINT8	u8BuffThreshold;

	if( (u8BufferCnt == 0) || (u8BufferCnt > 14) )
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_PARAMETER);
		return RETURN_ADC_INVALID_PARAMETER;
	}

	u8BuffThreshold = u8BufferCnt + 1;

	xADCCNF2 = (xADCCNF2 & 0x0F) | (u8BuffThreshold << 4);		// bit[7:4]

	if(u8FifoEna)
	{
		xADCCNF2 |= BIT1;		// bit[1]=1
	}
	else
	{
		xADCCNF2 &= ~BIT1;		// bit[1]=0
	}

	return RETURN_ADC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Gets a value of the selected ADC channel.
///	
///	@param	void
///	@return	INT16. Value of read ADC. -2048 ~ 2047.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
INT16 HAL_AdcGet(void)
{
	UINT8	u8LowByte;
	UINT8	u8HighByte;
	INT16	i16ReadADC;
	UINT8	u8EA;

	u8EA = EA;
	EA = 0;	
	u8LowByte = xADVAL;
	u8HighByte = xADVAL;
	EA = u8EA;
	i16ReadADC = u8HighByte << 8;
	i16ReadADC |= u8LowByte;
	i16ReadADC = i16ReadADC / 16;

	return  i16ReadADC;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Gets the number of currently acquired ADC values.
///	
///	@param	void
///	@return	UINT8. Number of currently acquired ADC values.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_AdcDataCountGet(void)
{
	UINT8	u8NumOfData;

	u8NumOfData = (xADCSTS & 0x78) >> 3;	// bit[6:3]
	if(u8NumOfData)	u8NumOfData--;

	return	u8NumOfData;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Gets an ADC data when acquiring is done.
///
///	@param	pi16ReadADC	: Pointer to the read ADC data.
///	@return	UINT8. Status.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_AdcGet(INT16* pi16ReadADC)
{
	static	UINT16	su16Loop;
	UINT8	u8Status;

	*pi16ReadADC = 0xFFFF;
	u8Status = RETURN_ADC_READ_FAILURE;
	for(su16Loop=0; su16Loop < 2000; su16Loop++)
	{
		if(xADCSTS & 0x70)
		{
			*pi16ReadADC = HAL_AdcGet();
			u8Status = RETURN_ADC_SUCCESS;
			break;
		}
	}

	if(u8Status)
	{
		assert_error(ADC_CODE | u8Status);
	}
	return u8Status;	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Sets an ADC channel to use and initialize the channel.
///	
///	@param	u8Ena		: 0=Disable, 1=Enable.
///	@param	u8Ref 		: 0=Band gap reference(1.25V), 2=1/3 of AVDD3V, 3=Internal voltage regulator output.
///	@param	u8AdcChan	: ADC channel to use.
///							\n 	0=ACH0.
///							\n 	1=ACH1. 
///							\n 	2=ACH2. 
///							\n 	3=ACH3. 
///							\n 	4=difference of ACH0 and ACH1.
///							\n 	5=difference of ACH2 and ACH3.
///							\n 	6=Temperature Sensor.
///							\n 	7=Battery Monitoring.
///							\n 	8=GND(for Calibration).
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_AdcSet(UINT8 u8Ena, UINT8 u8Ref, UINT8 u8AdcChan)
{
	UINT8	u8Status;
	static	UINT16	su16MaxLoopCnt;
	static	UINT16	su16Loop;

	if(u8AdcChan > ADC_CHAN_GND)
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_CHANNEL);
		return RETURN_ADC_INVALID_CHANNEL;
	}
	if(u8Ref > ADC_REFERENCE_VOL_REG)
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_PARAMETER);
		return RETURN_ADC_INVALID_PARAMETER;
	}
	
	if(u8Ena)
	{
		HAL_AdcBuffModeSet(0, 2);
		u8Status = HAL_AdcSet(1, u8Ref, u8AdcChan);
		if(u8Status == RETURN_ADC_SUCCESS)
		{
			HAL_AdcDecimatorReset();
			su16MaxLoopCnt = gcau16MaxWait_FilterPass[gu8McuPeriClockMode];
			for(su16Loop=0 ; su16Loop < su16MaxLoopCnt ; su16Loop++);	// 2ms delay
		}		
	}
	else
	{
		u8Status = HAL_AdcSet(0, 0, 0);
	}

	return	u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
///	Gets an ADC data once after selecting a channel. After getting, ADC is disabled.
///	
///	@param	u8AdcChan : ADC channel to use.
///						\n 	0=ACH0.
///						\n 	1=ACH1. 
///						\n 	2=ACH2. 
///						\n 	3=ACH3. 
///						\n 	4=difference of ACH0 and ACH1.
///						\n 	5=difference of ACH2 and ACH3.
///						\n 	6=Temperature Sensor.
///						\n 	7=Battery Monitoring.
///						\n 	8=GND(for Calibration).
///	@param	pi16ReadADC	: Pointer to the read ADC data.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_AdcGet_Once(UINT8 u8AdcChan, INT16 *pi16ReadADC)
{
	UINT8	u8Status;

	if(u8AdcChan > ADC_CHAN_GND)
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_CHANNEL);
		return RETURN_ADC_INVALID_CHANNEL;
	}

	u8Status = SYS_AdcSet(1, 0, u8AdcChan);
	if(u8Status)
	{
		return u8Status;
	}

	u8Status = SYS_AdcGet(pi16ReadADC);
	if(u8Status)
	{
		return u8Status;
	}

	SYS_AdcSet(0, 0, 0);

	return RETURN_ADC_SUCCESS;
}

#ifdef __MG2470_ADC_CALIBRATION_INCLUDE_
///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Calibrates an input data comparing to Max/Min values.
///	
///	@param	i16MaxADC	: Maximum value for calibration.
///	@param	i16MinADC	: Minimum value for calibration.
///	@param	i16InputADC	: ADC data to be calibrated
///	@param	u8ResBitNum	: Number of resolution bits. 10 ~ 15.
///	@param	pi16CalibADC	: Pointer to the calibrated ADC data.
///							\n	if u8ResBitNum = 10, the range of calibrated data is 0 ~ 1023.
///							\n 	if u8ResBitNum = 11, the range of calibrated data is 0 ~ 2047.
///							\n	if u8ResBitNum = 12, the range of calibrated data is 0 ~ 4095.
///							\n	if u8ResBitNum = 13, the range of calibrated data is 0 ~ 8191.
///							\n	if u8ResBitNum = 14, the range of calibrated data is 0 ~ 16383.
///							\n	if u8ResBitNum = 15, the range of calibrated data is 0 ~ 32767.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_AdcCalib(INT16 i16MaxADC, INT16 i16MinADC, INT16 i16InputADC, UINT8 u8ResBitNum, INT16 *pi16CalibADC)
{
	UINT16	u16Difference;
	UINT16	u16ADCOffset;
	UINT16	u16Resolution;
	UINT32	u32Temp;

	if( (u8ResBitNum < 10) || (u8ResBitNum > 15) )
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_PARAMETER);
		return RETURN_ADC_INVALID_PARAMETER;
	}	

	if(i16MinADC >= i16MaxADC)
	{
		assert_error(ADC_CODE | RETURN_ADC_OUT_OF_RANGE);
		return RETURN_ADC_OUT_OF_RANGE;
	}

	u16Difference = i16MaxADC - i16MinADC;

	if(i16InputADC < i16MinADC)
	{
		u16ADCOffset = 0;
	}
	else if(i16InputADC > i16MaxADC)
	{
		u16ADCOffset = u16Difference;
	}
	else
	{
		u16ADCOffset = i16InputADC - i16MinADC;
	}

	u16Resolution = (1 << u8ResBitNum) - 1;
	u32Temp = u16Resolution;
	u32Temp = u32Temp * u16ADCOffset;
	u32Temp = u32Temp / u16Difference;

	*pi16CalibADC = u32Temp & 0x00007FFF;

	return RETURN_ADC_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Gets an ADC data of VCC channel for calibration.
///
///	@param	void
///	@return	UINT8. Status.
///	@note	The acquired data is written to gi16ADC_VCC.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_AdcGet_VCCForCalib(void)
{
	UINT8	u8Status;

	u8Status = SYS_AdcGet_Once(ADC_CHAN_VCC, &gi16ADC_VCC);

	return u8Status;	
}


///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Gets an ADC data of GND channel for calibration.
///
///	@param	void
///	@return	UINT8. Status.
///	@note	The acquired data is written to gi16ADC_GND.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_AdcGet_GNDForCalib(void)
{
	UINT8	u8Status;

	u8Status = SYS_AdcGet_Once(ADC_CHAN_GND, &gi16ADC_GND);

	return u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Gets a calibrated ADC data.
///
///	@param	u8ResBitNum	: Number of resolution bits. 10 ~ 15.
///	@param	pi16CalibADC	: Pointer to the calibrated ADC data.
///							\n	if u8ResBitNum = 10, the range of calibrated data is 0 ~ 1023.
///							\n 	if u8ResBitNum = 11, the range of calibrated data is 0 ~ 2047.
///							\n	if u8ResBitNum = 12, the range of calibrated data is 0 ~ 4095.
///							\n	if u8ResBitNum = 13, the range of calibrated data is 0 ~ 8191.
///							\n	if u8ResBitNum = 14, the range of calibrated data is 0 ~ 16383.
///							\n	if u8ResBitNum = 15, the range of calibrated data is 0 ~ 32767.
///	@return	UINT8. Status.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_AdcCalibGet(UINT8 u8ResBitNum, INT16 *pi16CalibADC)
{
	INT16	i16ReadADC;
	UINT8	u8Status;

	if( (u8ResBitNum < 10) || (u8ResBitNum > 15) )
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_PARAMETER);
		return RETURN_ADC_INVALID_PARAMETER;
	}

	u8Status = SYS_AdcGet(&i16ReadADC);
	if(u8Status)
	{
		return u8Status;
	}

	u8Status = SYS_AdcCalib(gi16ADC_VCC, gi16ADC_GND, i16ReadADC, u8ResBitNum, pi16CalibADC);
	if(u8Status)
	{
		return u8Status;
	}

	return RETURN_ADC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Gets a calibrated ADC data once after selecting a channel. After getting, ADC is disabled.
///
///	@param	u8AdcChan	: ADC channel to use.
///							\n 	0=ACH0.
///							\n 	1=ACH1. 
///							\n 	2=ACH2. 
///							\n 	3=ACH3. 
///							\n 	4=difference of ACH0 and ACH1.
///							\n 	5=difference of ACH2 and ACH3.
///							\n 	6=Temperature Sensor.
///							\n 	7=Battery Monitoring.
///							\n 	8=GND(for Calibration).
///	@param	u8ResBitNum	: Number of resolution bits. 10 ~ 15.
///	@param	pi16CalibADC	: Pointer to the calibrated ADC data.
///							\n	if u8ResBitNum = 10, the range of calibrated data is 0 ~ 1023.
///							\n 	if u8ResBitNum = 11, the range of calibrated data is 0 ~ 2047.
///							\n	if u8ResBitNum = 12, the range of calibrated data is 0 ~ 4095.
///							\n	if u8ResBitNum = 13, the range of calibrated data is 0 ~ 8191.
///							\n	if u8ResBitNum = 14, the range of calibrated data is 0 ~ 16383.
///							\n	if u8ResBitNum = 15, the range of calibrated data is 0 ~ 32767.
///	@return	UINT8. Status.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_AdcCalibGet_Once(UINT8 u8AdcChan, UINT8 u8ResBitNum, INT16 *pi16CalibADC)
{
	INT16	i16ReadGND;
	INT16	i16ReadVCC;
	INT16	i16ReadADC;
	UINT8	u8Status;

	if(u8AdcChan > ADC_CHAN_GND)
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_CHANNEL);
		return RETURN_ADC_INVALID_CHANNEL;
	}

	if( (u8ResBitNum < 10) || (u8ResBitNum > 15) )
	{
		assert_error(ADC_CODE | RETURN_ADC_INVALID_PARAMETER);
		return RETURN_ADC_INVALID_PARAMETER;
	}

	u8Status = SYS_AdcGet_Once(ADC_CHAN_VCC, &i16ReadVCC);
	if(u8Status)
	{
		return u8Status;
	}
	u8Status = SYS_AdcGet_Once(ADC_CHAN_GND, &i16ReadGND);
	if(u8Status)
	{
		return u8Status;
	}
	u8Status = SYS_AdcGet_Once(u8AdcChan, &i16ReadADC);
	if(u8Status)
	{
		return u8Status;
	}
	u8Status = SYS_AdcCalib(i16ReadVCC, i16ReadGND, i16ReadADC, u8ResBitNum, pi16CalibADC);
	if(u8Status)
	{
		return u8Status;
	}

	return RETURN_ADC_SUCCESS;
}
#endif	// #ifdef __MG2470_ADC_CALIBRATION_INCLUDE_

/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2013-12-20
	- Version		: V2.1

	[2013-12-20] V2.1
	- HAL_IRTxNecTxData() : corrected.
	- HAL_IRTxNecTxCont() : corrected.

	[2012-04-25] V2.0
	- Initial Version
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "irtx_NEC.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether IR-Tx interrupt is generated.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 gu8IRTxIntFlag = 0;






///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for IR-Tx.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_IRTxClockOn(UINT8 u8On)
{
	if(u8On)
	{
		IRTX_CLK_ON;
	}
	else
	{
		IRTX_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets IR-Tx interrupt.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u8CarrEna	: Carrier Enable. 0=Disable, 1=Enable.
///	@param	u8CustomCode: Customer code
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_IRTxNecIntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8CarrEna, UINT8 u8CustomCode)
{
	if(IRTX_CLK_STATE == 0)
	{
		assert_error(IRTX_NEC_ERROR_CODE | RETURN_IRTX_NEC_CLOCK_OFF);
		return RETURN_IRTX_NEC_CLOCK_OFF;
	}

	gu8IRTxIntFlag = 0;

	// carrier freq: xTCCNTx = 8.0e6/38.0e3 = 210.5263 = 0x00D3
	xPPM_TCCNT1 = 0x01;
	xPPM_TCCNT0 = 0xA5;

	// carrier duty: xHCCNTx = (1/3)*8.0e6/38.0e3 = 70.1754 = 0x0046
	if (u8CarrEna)
	{
		xPPM_HCCNT1 = 0x00;
		xPPM_HCCNT0 = 0x8C;
	}
	else
	{
		xPPM_HCCNT1 = xPPM_TCCNT1;
		xPPM_HCCNT0 = xPPM_TCCNT0;
	}

	// xPPM_T1CNT = 4.0 * xPPM_H1CNT = 4.0 * xPPM_H0CNT
	xPPM_CDIV1 = 0x00;
	xPPM_CDIV0 = 0x64;
	xPPM_H0CNT = 0x5A;
	xPPM_H1CNT = 0xB4;	
	xPPM_T0CNT = 0xB4;
	xPPM_T1CNT = 0x5A;

	xPPM_CCODE = u8CustomCode;
	xPPM_CTL = 0x2F;

	if (u8Priority)
	{
		EIP2 |=  BIT0;
	}
	else
	{
		EIP2 &= ~BIT0;
	}

	if (u8IntEna)
	{
		EIE2 |=  BIT0;
	}
	else
	{
		EIE2 &= ~BIT0;
	}

	return	RETURN_IRTX_NEC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sends an IR-Tx data.
///
///	@param	u8DataCode	: Data code to send
///	@return	UINT8. Status
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_IRTxNecTxData(UINT8 u8DataCode)
{
	UINT16	iw;
	UINT8	u8Status;

	if(IRTX_CLK_STATE == 0)
	{
		assert_error(IRTX_NEC_ERROR_CODE | RETURN_IRTX_NEC_CLOCK_OFF);
		return RETURN_IRTX_NEC_CLOCK_OFF;		
	}

	xPPM_DCODE = u8DataCode;

	xPPM_POSST = 8;
	xPPM_POSSP = 41;
	EA = 0;
	xPPM_CTL |= BIT7;
	DPH = 0;
	EA = 1;
	gu8IRTxIntFlag = 0;

	u8Status = RETURN_IRTX_NEC_DATA_ERROR;
	for(iw=0 ; iw<30000 ; iw++)
	{
		if(gu8IRTxIntFlag)
		{
			u8Status = RETURN_IRTX_NEC_SUCCESS;
			break;
		}
	}

	if(u8Status)
	{
		assert_error(IRTX_NEC_ERROR_CODE | u8Status);
	}	

	return	u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	
///
///	@param	void
///	@return	UINT8. Status
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_IRTxNecTxCont(void)
{
	UINT16	iw;
	UINT8	u8Status;

	if(IRTX_CLK_STATE == 0)
	{
		assert_error(IRTX_NEC_ERROR_CODE | RETURN_IRTX_NEC_CLOCK_OFF);
		return RETURN_IRTX_NEC_CLOCK_OFF;		
	}

	xPPM_POSST = 6;
	xPPM_POSSP = 7;
	EA = 0;
	xPPM_CTL |= BIT7;
	DPH = 0;
	EA = 1;
	gu8IRTxIntFlag = 0;

	u8Status = RETURN_IRTX_NEC_CONT_ERROR;
	for(iw=0 ; iw<30000 ; iw++)
	{
		if(gu8IRTxIntFlag)
		{
			u8Status = RETURN_IRTX_NEC_SUCCESS;
			break;
		}
	}	

	if(u8Status)
	{
		assert_error(IRTX_NEC_ERROR_CODE | u8Status);
	}

	return	u8Status;
}


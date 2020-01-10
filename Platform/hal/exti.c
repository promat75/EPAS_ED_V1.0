
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "exti.h"

#ifdef __MG2470_EXTI0_INCLUDE_
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether EXT0(External 0) interrupt is generated.
///	When EXT0 interrupt is generated, it is set to 1.
///	And after processing the interrupt, it should be cleared.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8Ext0IntFlag = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Controls EXT0(External 0) Interrupt.
///	Additionally, it specifies the priority and triggering type of EXT0.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of Interrupt. 0=Low, 1=High.
///	@param	u8TrigType	: Triggering type of EXT0.
/// 							\n 0=Low level.
///							\n 1=Falling edge.
///							\n 2=High level.
///							\n 3=Rising edge.
///	@return	void
///	@note	The state of P3.2 should be input mode.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Ext0IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8TrigType)
{
	UINT8	u8EA;

	if( (P3OEN & BIT2) == 0)
	{
		assert_error(EXTI_CODE | RETURN_EXTI_P32_OUTPUT_STATE);
		return RETURN_EXTI_P32_OUTPUT_STATE;
	}

	u8EA = EA;
	EA = 0;

	if(u8TrigType & BIT1)		// reverse polarity
	{
		xGPIOPOL3 |= BIT2;
	}
	else
	{
		xGPIOPOL3 &= ~BIT2;
	}
	
	IT0 = (u8TrigType & BIT0)? 1 : 0;
	PX0 = u8Priority;
	EX0 = u8IntEna;

	if(EX0)
	{
		xGPIOMSK3 |= BIT2;
	}
	else
	{
		xGPIOMSK3 &= ~BIT2;
	}

	EA = u8EA;

	return RETURN_EXTI_SUCCESS;
}
#endif		// #ifdef __MG2470_EXTI0_INCLUDE_


#ifdef __MG2470_EXTI1_INCLUDE_
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether EXT1(External 1) interrupt is generated.
///	When EXT1 interrupt is generated, it is set to 1.
///	And after processing the interrupt, it should be cleared.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8Ext1IntFlag = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Controls EXT1(External 1) Interrupt.
///	Additionally, it specifies the priority and triggering type of EXT1.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of Interrupt. 0=Low, 1=High.
///	@param	u8TrigType	: Triggering type of EXT1.
/// 							\n 0=Low level.
///							\n 1=Falling edge.
///							\n 2=High level.
///							\n 3=Rising edge.
///	@return	void
///	@note	The state of P3.3 should be input mode.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Ext1IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8TrigType)
{
	UINT8	u8EA;

	if( (P3OEN & BIT3) == 0)
	{
		assert_error(EXTI_CODE | RETURN_EXTI_P33_OUTPUT_STATE);
		return RETURN_EXTI_P33_OUTPUT_STATE;
	}

	u8EA = EA;
	EA = 0;	

	if(u8TrigType & BIT1)		// reverse polarity
	{
		xGPIOPOL3 |= BIT3;
	}
	else
	{
		xGPIOPOL3 &= ~BIT3;
	}

	IT1 = (u8TrigType & BIT0)? 1 : 0;
	PX1 = u8Priority;
	EX1 = u8IntEna;

	if(EX1)
	{
		xGPIOMSK3 |= BIT3;
	}
	else
	{
		xGPIOMSK3 &= ~BIT3;
	}
	
	EA = u8EA;

	return RETURN_EXTI_SUCCESS;
}
#endif		// #ifdef __MG2470_EXTI1_INCLUDE_

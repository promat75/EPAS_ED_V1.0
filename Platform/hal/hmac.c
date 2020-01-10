
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "hmac.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets MACCTRL register.
///
///	@param	u8MacCtrl	: MACCTRL register's value to write.
///		\n 	bit[7:5] : reserved
///		\n 	bit[4] : prevent_ack_packet.	1=Enabled. Default value is 0.
///		\n 	bit[3] : pan_coordinator.	1=Enabled. Default value is 0.
///		\n 	bit[2] : addr_decode.		1=Enabled. Default value is 1.
///		\n	bit[1] : auto_crc.			1=Enabled. Default value is 1.
///		\n	bit[0] : reserved(should be 0).
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_MacCtrlSet(UINT8 u8MacCtrl)
{
	xMACCTRL = u8MacCtrl;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets auto_crc bit in MACCTRL register. If enabled, a packet which has a wrong CRC field is not accepted.
///
///	@param	u8Ena	: 0=Disable, 1=Enable.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_AutoCrcSet(UINT8 u8Ena)
{
	if(u8Ena)
	{
		xMACCTRL |= BIT1;		// bit[1]=1
	}
	else
	{
		xMACCTRL &= ~BIT1;		// bit[1]=0
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets addr_decode bit in MACCTRL register. If enabled, a packet which has a wrong addressing field is not 
///	accepted.
///
///	@param	u8Ena	: 0=Disable, 1=Enable.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_AddrDecodeSet(UINT8 u8Ena)
{
	if(u8Ena)
	{
		xMACCTRL |= BIT2;		// bit[2]=1
	}
	else
	{
		xMACCTRL &= ~BIT2;		// bit[2]=0
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets pan_coordinator bit in MACCTRL register. If enabled, a packet which is accepted only for PAN coordinator
///	is received.
///
///	@param	u8Ena	: 0=Disable, 1=Enable.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_CoordinatorSet(UINT8 u8Ena)
{	
	if(u8Ena)
	{
		xMACCTRL |= BIT3;		// bit[3]=1
	}
	else
	{
		xMACCTRL &= ~BIT3;		// bit[3]=0
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets prevent_ack_packet bit in MACCTRL register. If enabled, an unexpected ACK packet is not accepted.
///
///	@param	u8Ena	: 0=Disable, 1=Enable.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_PreventAckSet(UINT8 u8Ena)
{	
	if(u8Ena)
	{
		xMACCTRL |= BIT4;		// bit[4]=1
	}
	else
	{
		xMACCTRL &= ~BIT4;		// bit[4]=0
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets PAN identifier.
///
///	@param	u16PanID	: PAN identifier to set.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_PanIDSet(UINT16 u16PanID)
{
	xPANID(0) = (UINT8) (u16PanID) ;
	xPANID(1) = (UINT8) (u16PanID >> 8) ;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets 16-bit short address.
///
///	@param	u16ShortAddr	: short address to set.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_ShortAddrSet(UINT16 u16ShortAddr)
{
	xSHORTADDR(0) = (UINT8) (u16ShortAddr) ;
	xSHORTADDR(1) = (UINT8) (u16ShortAddr >> 8) ;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets 64-bit extended address(IEEE address).
///
///	@param	pu8IEEEAddrBuf	: Pointer to the buffer of extended address to set.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_IEEEAddrSet(UINT8* pu8IEEEAddrBuf)
{
	rpmemcpy(&xEXTADDR(0), pu8IEEEAddrBuf, 8);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Resets TXFIFO. The read/write pointer is cleared to 0.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_TxFifoReset(void)
{
	UINT8	u8EA;
	
	//	Reset Pointer
	u8EA = EA;
	EA = 0;
	xMTFCSTA = 0x00;
	xMTFCWP = 0x00;
	xMTFCRP = 0x00;
	EA = u8EA;
	
	//	Reset State Machine
	REGCMD_TSM_RESET_ON;		// xMACRST |= 0x40
	REGCMD_TSM_RESET_OFF;		// xMACRST &= ~0x40
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Resets RXFIFO. The read/write pointer is cleared to 0.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_RxFifoReset(void)
{
	UINT8	u8EA;

	//	Reset Pointer
	u8EA = EA;
	EA = 0;
	xMRFCSTA = 0x00;
	xMRFCWP = 0x00;
	xMRFCRP = 0x00;
	EA = u8EA;
	
	//	Reset State Machine
	REGCMD_RSM_RESET_ON;		// xMACRST |= 0x20
	REGCMD_RSM_RESET_OFF;		// xMACRST &= ~0x20
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Initializes H/W MAC blocks.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_MacInit(void)
{
	HAL_MacCtrlSet(0x16);
	HAL_TxFifoReset();
	HAL_RxFifoReset();
}


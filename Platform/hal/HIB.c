
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "hib.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates code memory for HIB(Hardware Information Base)
///	Because this is synchronized with RadioPulse's Device Programmer, the start address, 0x1000, should not
///	be changed.
///	If HIB is not needed, this may be removed.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 code 	gcHIB_FLASH[512] _at_ HIB_ADDRESS;			// 512Bytes aligned

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Copies HIB(Hardware Information Base) from code memory whose address is from 0x1000 to 0x103F.
///	Refer to "tHW_INFORMATION" for structure of HIB.
///
///	@param	pu8Buff	: 0=Disable, 1=Enable.
///	@return	UINT8. Status. 0=HIB is not valid, 1=HIB is valid.
///	@note	HIB is valid only if it is written by RadioPulse's Device Programmer when the binary is programmed.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_HIBCopy(UINT8* pu8Buff)
{
	UINT8	ib;
	UINT8	code * pc8CodeAddr;
	UINT8	u8Sum;

	pc8CodeAddr = HIB_ADDRESS;
	u8Sum = 0;
	for(ib=0 ; ib<HIB_LENGTH ; ib++)
	{
		u8Sum += pc8CodeAddr[ib];
		pu8Buff[ib] = pc8CodeAddr[ib];
	}

	if(u8Sum == 0)
	{
		return	1;
	}
	else
	{
		return	0;	
	}
}



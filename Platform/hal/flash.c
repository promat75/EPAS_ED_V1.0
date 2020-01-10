
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2014-10-21
	- Version		: V2.5

	[2014-10-21] V2.5
	- HAL_FlashPageWrite() : enhanced.
	- HAL_FlashWrite() : added.

	[2014-08-28] V2.4
	- HAL_FlashPageRead() : Change naming to HAL_FlashRead(), check-sum meathod enhanced.

	[2014-01-23] V2.3
	- HAL_FlashPageWrite() : enhanced.
	- HAL_FlashPageRead() : enhanced.

	[2013-01-14] V2.2
	- HAL_FlashPageRead() : enhanced. // Check-sum added when flash page read done

	[2012-09-06] V2.1
	- HAL_FlashPageWrite() : enhanced. // Check-sum added when flash page write done

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "flash.h"
#include "clock.h"
#include "wdt.h"
#include "phy.h"
#include "timer.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether flash-writing is protected.
///	When this is 0x80, flash-writing is not permitted. (Protection mode)
///	So, before writing, it should be cleared to 0x00. And after writing, it should be backed to 0x80 for protection.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8FlashWriteProtect = 0x80;			// 0x00=Unprotected, 0x80=Protected

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Unprotects flash-writing by setting gu8FlashWriteProtect to 0x00.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_FlashWriteUnprotected(void)
{
	gu8FlashWriteProtect = 0x00;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Protects flash-writing by setting gu8FlashWriteProtect and corresponding register to 0x80.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_FlashWriteProtected(void)
{
	gu8FlashWriteProtect = 0x80;
	//xFCN_CMD = 0x80;	// Protection for flash-writing.
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Erases flash by 1 page(512 Bytes).
///
///	@param	void
///	@return	void
///	@note	When this function is called directly, the flash is not erased. 
///			It is valid only when called in HAL_FlashPageWrite().
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_FlashPageErase(UINT16 u16FlashWordAddr)
{
	xFCN_NOWAIT = 0x00;

	while(xFCN_STS0 & BIT0);

	_nop_();
	_nop_();
	_nop_();
	_nop_();

	xFCN_ADR0 = u16FlashWordAddr & 0xFF;
	xFCN_ADR1 = (u16FlashWordAddr>>8);
	xFCN_LEN1 = 0;
	xFCN_LEN0 = 1;
	xFCN_CMD = (gu8FlashWriteProtect | BIT1);	// Page Erase

	_nop_();
	_nop_();
	_nop_();
	_nop_();

	while(xFCN_STS0 & BIT0);
	xFCN_CMD = 0x80;	// Write-Protected

}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Writes flash by 1 page(512 Bytes).
///	\n	This function should be called after unprotecting by HAL_FlashWriteUnprotected().
///	\n	And, after writing, HAL_FlashWriteProtected() should be called to protect unintended flash writing.
///
///	@param	u16CodeAddr		: Code address from which flash is written. For 512 bytes aligning, 
///								bit[8:0] of this parameter should be all zeros.
///	@param	pu8ByteBuf		: Pointer to the buffer to be written.
///	@param	u8Option			: Option for writing
///					\n	bit[7:1]	: reserved
///					\n	bit[0]	: 1=Erasing only. Writing is not permitted.
///	@return	UINT8. Status.
///	@note	The sequence to write flash is below.
///		\n	HAL_FlashWriteUnprotected();
///		\n	HAL_FlashPageWrite();
///		\n	HAL_FlashWriteProtected();
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_FlashPageWrite(UINT16 u16CodeAddr, UINT8* pu8ByteBuf, UINT8 u8Option)
{
	UINT16 	u16ByteAddr;
	UINT16	u16WordLen;
	UINT16	u16CurrWordAddr;
	UINT16	u16StartWordAddr;
	UINT8	u8Status;

	UINT8	code * pu8CodeBuf;
	UINT16	iw;
	UINT8	u8WriteLoop;

	if(u16CodeAddr & 0x01FF)					// 512 bytes align
	{
		assert_error(FLASH_CODE | RETURN_FLASH_ALIGN_FAILURE);
		return RETURN_FLASH_ALIGN_FAILURE;
	}

	u16StartWordAddr = u16CodeAddr >> 2;		// 1 word = 4 bytes

	FLASH_CLK_ON;

	xFCN_NOWAIT = 0x00;

	if(gu8McuPeriClockMode == CLOCK_MODE_16_MHz)
	{
		LIB_FlashParking(0x80);
	}
	else
	{
		LIB_FlashParking(0);
	}

//
//	STEP-1 : Check registers
//
	if( 	(gu8FlashWriteProtect)
	||	(xFCN_CMD != 0x80)
	||	(xFCN_LEN1 != 0x00)
	||	(xFCN_LEN0 != 0x00)
	||	(xFCN_ADR1 != 0x70)
	||	(xFCN_ADR0 != 0x00) )
	{
		u8Status = RETURN_FLASH_PROTECTED;
		goto JP_HAL_FlashPageWrite;
	}

	//	RF RX Off
	HAL_RxEnable(0);

	for(u8WriteLoop = 0; u8WriteLoop < 4; u8WriteLoop++)
	{
//
//	STEP-2 : Erase Page
//
		HAL_WdtRestart();
		HAL_FlashPageErase(u16StartWordAddr);

		if(u8Option & BIT0)
		{
			u8Status = RETURN_FLASH_SUCCESS;
			TL0 = gu8Timer0ReloadTL0;
			TH0 = gu8Timer0ReloadTH0;
			goto JP_HAL_FlashPageWrite;
		}

//
//	STEP-3 : IDLE CHECK
//
		while(xFCN_STS0 & BIT0);

//
//	STEP-4 : PAGE WRITE
//
		xFCN_LEN0 = 0x01;
		xFCN_LEN1 = 0x00;

		u16ByteAddr = 0;
		u16CurrWordAddr = u16StartWordAddr;
		u16WordLen = 512/4;			// 512 bytes / 1 word(4bytes) = 128 words
		while(u16WordLen)
		{
			HAL_WdtRestart();

			xFCN_ADR0 = u16CurrWordAddr & 0xFF;
			xFCN_ADR1 = (u16CurrWordAddr >> 8);

			xFCN_DAT0 = pu8ByteBuf[u16ByteAddr + 0];
			xFCN_DAT1 = pu8ByteBuf[u16ByteAddr + 1];
			xFCN_DAT2 = pu8ByteBuf[u16ByteAddr + 2];
			xFCN_DAT3 = pu8ByteBuf[u16ByteAddr + 3];

			xFCN_CMD  = (gu8FlashWriteProtect | BIT0);

			_nop_();
			_nop_();
			_nop_();
			_nop_();

			u16ByteAddr += 4; 		/* byte addr */
			u16CurrWordAddr += 1; 	/* word addr */
			u16WordLen -= 1; 			/* word length */
		}
		while(xFCN_STS0 & BIT0);

		TL0 = gu8Timer0ReloadTL0;
		TH0 = gu8Timer0ReloadTH0;

//
//	STEP-5 : CHECK WRITTEN PAGE
//
		pu8CodeBuf = (UINT8 code *)u16CodeAddr;

		u8Status = RETURN_FLASH_SUCCESS;
		for(iw = 0; iw < 512; iw++)
		{
			if(pu8ByteBuf[iw] != pu8CodeBuf[iw])
			{
				u8Status = RETURN_FLASH_PAGE_WRITE_FAIL;
				break;
			}
		}

		// Exit from Write-loop
		if(u8Status == RETURN_FLASH_SUCCESS)
		{
			break;
		}
	}

	JP_HAL_FlashPageWrite :

	//
	xFCN_CMD = 0x80;		/* protect bit set */
	xFCN_ADR1 = 0x70;		/* address register parking at 0x6000-0x7FFF */
	xFCN_ADR0 = 0x00;
	xFCN_LEN1 = 0x00;		/* length register parking with 0x0000 */
	xFCN_LEN0 = 0x00;
	LIB_FlashParking(1);	
	//

	FLASH_CLK_OFF;

	//	RF RX On
	HAL_RxEnable(1);

	if(u8Status)
	{
		assert_error(FLASH_CODE | u8Status);
	}

	return u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Writes flash(Erase by 1 page(512 Bytes)).
///	\n	This function should be called after unprotecting by HAL_FlashWriteUnprotected().
///	\n	And, after writing, HAL_FlashWriteProtected() should be called to protect unintended flash writing.
///
///	@param	u16CodeAddr		: Code address from which flash is written. For 512 bytes aligning, 
///								bit[8:0] of this parameter should be all zeros.
///	@param	pu8ByteBuf		: Pointer to the buffer to be written.
///	@param	u16ByteLen		: Number of bytes to be written. 0 ~ 512.
///								If u16ByteLen = 0, Erasing only. Writing is not permitted.
///	@return	UINT8. Status.
///	@note	The sequence to write flash is below.
///		\n	HAL_FlashWriteUnprotected();
///		\n	HAL_FlashWrite();
///		\n	HAL_FlashWriteProtected();
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_FlashWrite(UINT16 u16CodeAddr, UINT8* pu8ByteBuf, UINT16 u16ByteLen)
{
	UINT16 	u16ByteAddr;
	UINT16	u16WordLen;
	UINT16	u16CurrWordAddr;
	UINT16	u16StartWordAddr;
	UINT8	u8Status;

	UINT8	code * pu8CodeBuf;
	UINT16	iw;
	UINT8	u8WriteLoop;

	if(u16CodeAddr & 0x01FF)					// 512 bytes align
	{
		assert_error(FLASH_CODE | RETURN_FLASH_ALIGN_FAILURE);
		return RETURN_FLASH_ALIGN_FAILURE;
	}

	u16StartWordAddr = u16CodeAddr >> 2;		// 1 word = 4 bytes

	FLASH_CLK_ON;
	xFCN_NOWAIT = 0x00;

	if(gu8McuPeriClockMode == CLOCK_MODE_16_MHz)
	{
		LIB_FlashParking(0x80);
	}
	else
	{
		LIB_FlashParking(0);
	}

//
//	STEP-1 : Check registers
//
	if( 	(gu8FlashWriteProtect)
	||	(xFCN_CMD != 0x80)
	||	(xFCN_LEN1 != 0x00)
	||	(xFCN_LEN0 != 0x00)
	||	(xFCN_ADR1 != 0x70)
	||	(xFCN_ADR0 != 0x00) )
	{
		u8Status = RETURN_FLASH_PROTECTED;
		goto JP_HAL_FlashPageWrite;
	}

	//	RF RX Off
	HAL_RxEnable(0);

	for(u8WriteLoop = 0; u8WriteLoop < 4; u8WriteLoop++)
	{
//
//	STEP-2 : Erase Page
//
		HAL_WdtRestart();
		HAL_FlashPageErase(u16StartWordAddr);

		if(u16ByteLen == 0)
		{
			u8Status = RETURN_FLASH_SUCCESS;
			TL0 = gu8Timer0ReloadTL0;
			TH0 = gu8Timer0ReloadTH0;
			break;
		}

	//
	//	STEP-3 : IDLE CHECK
	//
		while(xFCN_STS0 & BIT0);

	//
	//	STEP-4 : PAGE WRITE
	//
		xFCN_LEN0 = 0x01;
		xFCN_LEN1 = 0x00;

		u16ByteAddr = 0;
		u16CurrWordAddr = u16StartWordAddr;
		u16WordLen = u16ByteLen/4;			// u16ByteLen / 1 word(4bytes) = MAX 128 words
		if( (u16ByteLen%4) != 0)
		{
			u16WordLen += 1;
		}

		while(u16WordLen)
		{
			HAL_WdtRestart();

			xFCN_ADR0 = u16CurrWordAddr & 0xFF;
			xFCN_ADR1 = (u16CurrWordAddr >> 8);

			xFCN_DAT0 = pu8ByteBuf[u16ByteAddr + 0];
			xFCN_DAT1 = pu8ByteBuf[u16ByteAddr + 1];
			xFCN_DAT2 = pu8ByteBuf[u16ByteAddr + 2];
			xFCN_DAT3 = pu8ByteBuf[u16ByteAddr + 3];

			xFCN_CMD  = (gu8FlashWriteProtect | BIT0);

			_nop_();
			_nop_();
			_nop_();
			_nop_();

			u16ByteAddr += 4; 		/* byte addr */
			u16CurrWordAddr += 1; 	/* word addr */
			u16WordLen -= 1; 			/* word length */
		}

		while(xFCN_STS0 & BIT0);

		TL0 = gu8Timer0ReloadTL0;
		TH0 = gu8Timer0ReloadTH0;

	//
	//	STEP-5 : CHECK WRITTEN PAGE
	//
		pu8CodeBuf = (UINT8 code *)u16CodeAddr;

		u8Status = RETURN_FLASH_SUCCESS;
		for(iw = 0; iw < u16ByteLen; iw++)
		{
			if(pu8ByteBuf[iw] != pu8CodeBuf[iw])
			{
				u8Status = RETURN_FLASH_PAGE_WRITE_FAIL;
				break;
			}
		}

		// Exit from Write-loop
		if(u8Status == RETURN_FLASH_SUCCESS)
		{
			break;
		}
	}

	JP_HAL_FlashPageWrite :

	//
	xFCN_CMD = 0x80;		/* protect bit set */
	xFCN_ADR1 = 0x70;		/* address register parking at 0x6000-0x7FFF */
	xFCN_ADR0 = 0x00;
	xFCN_LEN1 = 0x00;		/* length register parking with 0x0000 */
	xFCN_LEN0 = 0x00;
	LIB_FlashParking(1);	
	//

	FLASH_CLK_OFF;

	//	RF RX On
	HAL_RxEnable(1);

	if(u8Status)
	{
		assert_error(FLASH_CODE | u8Status);
	}

	return u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
/// 	Reads flash.
///
///	@param	u16CodeAddr	: Code address from which flash is read.
///	@param	pu8ReadBuf	: Pointer to the buffer in which read flash is stored.
///	@param	u16ByteLen	: Number of bytes to read.
///	@return	UINT8. Status.
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_FlashRead(UINT16 u16CodeAddr, UINT8* pu8ReadBuf, UINT16 u16ByteLen)
{
	UINT16	iw;
	UINT8	u8Retry;
	UINT8	u8Status;
	UINT32	u32LastCodeAddr;
	UINT8	code * pu8CodeBuf;

	u32LastCodeAddr = u16CodeAddr;
	u32LastCodeAddr += (u16ByteLen - 1);

	if(u32LastCodeAddr & 0xFFFF0000)
	{
		assert_error(FLASH_CODE | RETURN_FLASH_INVALID_ADDRESS);
		return RETURN_FLASH_INVALID_ADDRESS;
	}

	u8Status = RETURN_FLASH_SUCCESS;
	pu8CodeBuf = (UINT8 code *)u16CodeAddr;

	for(u8Retry = 0; u8Retry < 4; u8Retry++)
	{
		for(iw = 0; iw < u16ByteLen; iw++)
		{
			pu8ReadBuf[iw] = pu8CodeBuf[iw];
		}

		for(iw = 0; iw < u16ByteLen; iw++)
		{
			if(pu8ReadBuf[iw] != pu8CodeBuf[iw])
			{
				u8Status = RETURN_FLASH_PAGE_READ_FAIL;
				break;
			}
		}

		if(u8Status == RETURN_FLASH_SUCCESS)
		{
			break;
		}
	}

	return u8Status;
}



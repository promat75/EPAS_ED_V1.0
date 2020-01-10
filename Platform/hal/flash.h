
/*******************************************************************************
	[2014-10-21] Compatible with V2.5
*******************************************************************************/


#ifndef __MG2470_FLASH_H__
#define __MG2470_FLASH_H__

#define	FLASH_CLK_ON		(PERI_CLK_STP1 |= BIT5)
#define	FLASH_CLK_OFF		(PERI_CLK_STP1 &= ~BIT5)
#define	FLASH_CLK_STATE	(PERI_CLK_STP1 & BIT5)

#define	FLASH_CODE	0x0400

#define	RETURN_FLASH_SUCCESS				0x00
#define	RETURN_FLASH_CLOCK_OFF			0x10
#define	RETURN_FLASH_INVALID_PARAMETER	0x11
#define	RETURN_FLASH_INVALID_ADDRESS	0x12
#define	RETURN_FLASH_PROTECTED			0x13
#define	RETURN_FLASH_ALIGN_FAILURE		0x14
#define	RETURN_FLASH_PAGE_WRITE_FAIL	0x15
#define	RETURN_FLASH_PAGE_READ_FAIL		0x16

#define	FLASH_READ_BYTE(ADDRESS)		*(UINT8 code *)(ADDRESS)

extern	UINT8	gu8FlashWriteProtect;

void HAL_FlashWriteUnprotected(void);
void HAL_FlashWriteProtected(void);
void HAL_FlashPageErase(UINT16 u16FlashWordAddr);
UINT8 HAL_FlashPageWrite(UINT16 u16CodeAddr, UINT8* pu8ByteBuf, UINT8 u8Option);
UINT8 HAL_FlashWrite(UINT16 u16CodeAddr, UINT8* pu8ByteBuf, UINT16 u16ByteLen);
UINT8 HAL_FlashRead(UINT16 u16CodeAddr, UINT8* pu8ReadBuf, UINT16 u16ByteLen);

#endif	// #ifndef __MG2470_FLASH_H__





#include "INCLUDE_TOP.h"
#include "flash.h"
#include "app_flash.h"
#include "util_app.h"

UINT8	code gcau8UserFlash[512]		_at_ CODE_ADDRESS_USER0;
UINT8	code gcau8NwkInfoFlash[512]	_at_ CODE_ADDRESS_NWK_INFO;

UINT8 AppLib_FlashDataWrite(tNWK_INFO* ptNwkInfo)
{
	UINT8	u8Status;

	HAL_FlashWriteUnprotected();
	u8Status = HAL_FlashPageWrite(CODE_ADDRESS_NWK_INFO, (UINT8 *)ptNwkInfo, 0);
	HAL_FlashWriteProtected();

	return u8Status;
}

void AppLib_FlashDataRead(tNWK_INFO* ptNwkInfo)
{
	ptNwkInfo->u8Channel = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 0);
	ptNwkInfo->u16PanID = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 1) << 8;
	ptNwkInfo->u16PanID |= FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 2);	
	ptNwkInfo->u16ShortAddr = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 3) << 8;
	ptNwkInfo->u16ShortAddr |= FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 4);	
	ptNwkInfo->au8IEEEAddr[0] = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 5);
	ptNwkInfo->au8IEEEAddr[1] = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 6);
	ptNwkInfo->au8IEEEAddr[2] = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 7);
	ptNwkInfo->au8IEEEAddr[3] = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 8);
	ptNwkInfo->au8IEEEAddr[4] = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 9);
	ptNwkInfo->au8IEEEAddr[5] = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 10);
	ptNwkInfo->au8IEEEAddr[6] = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 11);
	ptNwkInfo->au8IEEEAddr[7] = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 12);
	ptNwkInfo->u8Reserved0 = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 13);
	ptNwkInfo->u8Reserved1 = FLASH_READ_BYTE(CODE_ADDRESS_NWK_INFO + 14);	
}

void AppLib_FlashDataRead_2(tNWK_INFO* ptNwkInfo)
{	
	HAL_FlashRead(CODE_ADDRESS_NWK_INFO, (UINT8 *)ptNwkInfo, sizeof(tNWK_INFO));
}

UINT8 AppLib_FlashDataDelete()
{
	tNWK_INFO	tNwkInfoInit;
	UINT8	u8Status;

	tNwkInfoInit.u8Channel = 0xFF;
	tNwkInfoInit.u16PanID = 0xFFFF;
	tNwkInfoInit.u16ShortAddr = 0xFFFF;
	tNwkInfoInit.au8IEEEAddr[0] = 0xFF;
	tNwkInfoInit.au8IEEEAddr[1] = 0xFF;
	tNwkInfoInit.au8IEEEAddr[2] = 0xFF;
	tNwkInfoInit.au8IEEEAddr[3] = 0xFF;
	tNwkInfoInit.au8IEEEAddr[4] = 0xFF;
	tNwkInfoInit.au8IEEEAddr[5] = 0xFF;
	tNwkInfoInit.au8IEEEAddr[6] = 0xFF;
	tNwkInfoInit.au8IEEEAddr[7] = 0xFF;
	tNwkInfoInit.u8Reserved0 = 0xFF;
	tNwkInfoInit.u8Reserved1 = 0xFF;

	u8Status = AppLib_FlashDataWrite(&tNwkInfoInit);

	return u8Status;
}

void AppLib_FlashDataDisplay(tNWK_INFO* ptNwkInfo)
{
	zPrintf(1, "\n Ch=0x%02x", (short)ptNwkInfo->u8Channel);
	zPrintf(1, " :PanID=0x%04x", (short)ptNwkInfo->u16PanID);
	zPrintf(1, " :ShortAddr=0x%04x", (short)ptNwkInfo->u16ShortAddr);
	zPrintf(1, "\n IEEEAddr(0~7) :");
	Display_Buffer(ptNwkInfo->au8IEEEAddr, 8, 0);
	zPrintf(1, "\n Rsv0=0x%02x", (short)ptNwkInfo->u8Reserved0);
	zPrintf(1, " :Rsv1=0x%02x", (short)ptNwkInfo->u8Reserved1);
}





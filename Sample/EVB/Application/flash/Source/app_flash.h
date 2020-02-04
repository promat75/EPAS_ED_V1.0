
#ifndef __MG2470_APP_FLASH_H__
#define __MG2470_APP_FLASH_H__

typedef struct
{
	UINT8	u8Channel;
	UINT16	u16PanID;	
	UINT16	u16ShortAddr;
	UINT8	au8IEEEAddr[8];
	UINT8	u8Reserved0;
	UINT8	u8Reserved1;
//	UINT8	au8Buffer[512];
} tNWK_INFO;

#define	CODE_ADDRESS_NWK_INFO	0x2000
#define	CODE_ADDRESS_USER0		0x2200

UINT8 AppLib_FlashDataWrite(tNWK_INFO* ptNwkInfo);
void AppLib_FlashDataRead(tNWK_INFO* ptNwkInfo);
void AppLib_FlashDataRead_2(tNWK_INFO* ptNwkInfo);
UINT8 AppLib_FlashDataDelete();
void AppLib_FlashDataDisplay(tNWK_INFO* ptNwkInfo);

#endif	// __MG2470_APP_FLASH_H__


/*******************************************************************************
	[2013-05-27] Compatible with V2.2
*******************************************************************************/


#ifndef __MG2470_SECURITY_H__
#define __MG2470_SECURITY_H__

#define	SEC_CLK_ON		(xPHY_CLK_EN0 |= BIT0)
#define	SEC_CLK_OFF	(xPHY_CLK_EN0 &= ~BIT0)
#define	SEC_CLK_STATE	(xPHY_CLK_EN0 & BIT0)

#define	SEC_MODE_NONE					0
#define	SEC_MODE_CBCMAC				1
#define	SEC_MODE_CTR					2
#define	SEC_MODE_CCM					3

#define	SEC_KEY0							0
#define	SEC_KEY1							1

#define	SEC_CODE						0x0A00

#define	RETURN_SEC_SUCCESS				0x00
#define	RETURN_SEC_INVALID_PARAMETER		0x11
#define	RETURN_SEC_ENCRYPT_TIMEOUT		0x12
#define	RETURN_SEC_DECRYPT_TIMEOUT		0x13
#define	RETURN_SEC_MIC_FAILURE			0x14
#define	RETURN_SEC_KEY_NUM				0x15
#define	RETURN_SEC_TOO_LONG_TEXT			0x16
#define	RETURN_SEC_TOO_SHORT_TEXT		0x17
#define	RETURN_SEC_WRONG_MEM_ADDR		0x18
#define	RETURN_SEC_WRONG_MIC			0x19
#define	RETURN_SEC_WRONG_MIC_LENGTH		0x1A

extern	UINT8	gu8AesIntFlag;

void		HAL_SecKey0Set(UINT8* pu8Key0Buff);
void		HAL_SecKey1Set(UINT8* pu8Key1Buff);
void		HAL_SecTxNonceKeySeqSet(UINT8 u8KeySeq);
void		HAL_SecTxNonceFrameCntSet(UINT32 u32FrameCnt);
void		HAL_SecTxNonceExtAddrSet(UINT8* pu8ExtAddrBuff);

void		HAL_SecRxNonceKeySeqSet(UINT8 u8KeySeq);
void		HAL_SecRxNonceFrameCntSet(UINT32 u32FrameCnt);
void		HAL_SecRxNonceExtAddrSet(UINT8* pu8ExtAddrBuff);
UINT8	HAL_SecTxNonceKeySeqGet(void);
UINT32	HAL_SecTxNonceFrameCntGet(void);
void		HAL_SecTxNonceExtAddrGet(UINT8* pu8ExtAddrBuff);
UINT8	HAL_SecRxNonceKeySeqGet(void);
UINT32	HAL_SecRxNonceFrameCntGet(void);
void		HAL_SecRxNonceExtAddrGet(UINT8* pu8ExtAddrBuff);
void		HAL_SecTXLSet(UINT8 u8TXL);
void		HAL_SecRXLSet(UINT8 u8RXL);
void		HAL_SecTxKeyChoice(UINT8 u8KeyNum);
void		HAL_SecRxKeyChoice(UINT8 u8KeyNum);
UINT8	HAL_SecMicLengthSet(UINT8 u8MicLength);
UINT8	HAL_SecModeSet(UINT8 u8Mode);
UINT8	HAL_SecEncryptionCCM32(UINT8* pu8TextBuff, UINT8 u8InLen, UINT32 u32FCnt, UINT8 u8KSeq, UINT8* pu8EncLen);
UINT8	HAL_SecDecryptionCCM32(UINT8* pu8TextBuff, UINT8 u8InLen, UINT32 u32FCnt, UINT8 u8KSeq, UINT8* pu8DecLen);

#endif	// #ifndef __MG2470_SECURITY_H__

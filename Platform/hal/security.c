
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2013-05-27
	- Version		: V2.2

	[2013-05-27] V2.2
	- HAL_SecEncryptionCCM32() : bug fix.

	[2012-08-20] V2.1
	- HAL_SecDecryptionCCM32() : corrected.
	- Comment : corrected.

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "security.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether AES interrupt is generated.
///	When AES interrupt is generated, it is set to 1.
///	And after processing the interrupt, it should be cleared.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8AesIntFlag = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets registers for KEY0.
///
///	@param	pu8Key0Buff		: Pointer to the buffer of KEY0 to be written.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecKey0Set(UINT8* pu8Key0Buff)
{
	UINT8	u8ClockOff;

	u8ClockOff = 0;
	if(SEC_CLK_STATE == 0)
	{
		SEC_CLK_ON;
		u8ClockOff = 1;
	}

	rpmemcpy(&xKEY0(0), pu8Key0Buff, 16);

	if(u8ClockOff)
	{
		SEC_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets registers for KEY1.
///
///	@param	pu8Key1Buff		: Pointer to the buffer of KEY1 to be written.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecKey1Set(UINT8* pu8Key1Buff)
{
	UINT8	u8ClockOff;

	u8ClockOff = 0;
	if(SEC_CLK_STATE == 0)
	{
		SEC_CLK_ON;
		u8ClockOff = 1;
	}

	rpmemcpy(&xKEY1(0), pu8Key1Buff, 16);

	if(u8ClockOff)
	{
		SEC_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the key sequence number for TX-NONCE.
///
///	@param	u8KeySeq		: Key sequence number to be written.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecTxNonceKeySeqSet(UINT8 u8KeySeq)
{
	UINT8	u8ClockOff;

	u8ClockOff = 0;
	if(SEC_CLK_STATE == 0)
	{
		SEC_CLK_ON;
		u8ClockOff = 1;
	}

	xTXNONCE(0) = u8KeySeq;

	if(u8ClockOff)
	{
		SEC_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the frame counter for TX-NONCE.
///
///	@param	u32FrameCnt		: 32-bits frame counter to be written.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecTxNonceFrameCntSet(UINT32 u32FrameCnt)
{
	UINT8	u8ClockOff;

	u8ClockOff = 0;
	if(SEC_CLK_STATE == 0)
	{
		SEC_CLK_ON;
		u8ClockOff = 1;
	}

	xTXNONCE(1) = (UINT8)u32FrameCnt;	u32FrameCnt >>= 8;
	xTXNONCE(2) = (UINT8)u32FrameCnt;	u32FrameCnt >>= 8;
	xTXNONCE(3) = (UINT8)u32FrameCnt;	u32FrameCnt >>= 8;
	xTXNONCE(4) = (UINT8)u32FrameCnt;

	if(u8ClockOff)
	{
		SEC_CLK_OFF;
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the extended address for TX-NONCE.
///
///	@param	pu8ExtAddrBuff		: Pointer to the buffer of 64-bits extended address to be written.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecTxNonceExtAddrSet(UINT8* pu8ExtAddrBuff)
{
	UINT8	u8ClockOff;

	u8ClockOff = 0;
	if(SEC_CLK_STATE == 0)
	{
		SEC_CLK_ON;
		u8ClockOff = 1;
	}

	rpmemcpy(&xTXNONCE(5), pu8ExtAddrBuff, 8);

	if(u8ClockOff)
	{
		SEC_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the key sequence number for RX-NONCE.
///
///	@param	u8KeySeq		: Key sequence number to be written.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecRxNonceKeySeqSet(UINT8 u8KeySeq)
{
	UINT8	u8ClockOff;

	u8ClockOff = 0;
	if(SEC_CLK_STATE == 0)
	{
		SEC_CLK_ON;
		u8ClockOff = 1;
	}

	xRXNONCE(0) = u8KeySeq;

	if(u8ClockOff)
	{
		SEC_CLK_OFF;
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the frame counter for RX-NONCE.
///
///	@param	u32FrameCnt		: 32-bits frame counter to be written.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecRxNonceFrameCntSet(UINT32 u32FrameCnt)
{
	UINT8	u8ClockOff;

	u8ClockOff = 0;
	if(SEC_CLK_STATE == 0)
	{
		SEC_CLK_ON;
		u8ClockOff = 1;
	}

	xRXNONCE(1) = (UINT8)u32FrameCnt;	u32FrameCnt >>= 8;
	xRXNONCE(2) = (UINT8)u32FrameCnt;	u32FrameCnt >>= 8;
	xRXNONCE(3) = (UINT8)u32FrameCnt;	u32FrameCnt >>= 8;
	xRXNONCE(4) = (UINT8)u32FrameCnt;

	if(u8ClockOff)
	{
		SEC_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
/// 	Sets the extended address for RX-NONCE.
///
///	@param	pu8ExtAddrBuff		: Pointer to the buffer of 64-bits extended address to be written.
///	@return	void
///
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecRxNonceExtAddrSet(UINT8* pu8ExtAddrBuff)
{
	UINT8	u8ClockOff;

	u8ClockOff = 0;
	if(SEC_CLK_STATE == 0)
	{
		SEC_CLK_ON;
		u8ClockOff = 1;
	}

	rpmemcpy(&xRXNONCE(5), pu8ExtAddrBuff, 8);

	if(u8ClockOff)
	{
		SEC_CLK_OFF;
	}	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the key sequence number for TX-NONCE.
///
///	@param	void
///	@return	UINT8. The sequence number.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_SecTxNonceKeySeqGet(void)
{
	return	xTXNONCE(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the frame counter for TX-NONCE.
///
///	@param	void
///	@return	UINT32. The frame counter.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT32 HAL_SecTxNonceFrameCntGet(void)
{
	UINT32	u32FrameCnt;

	u32FrameCnt = xTXNONCE(4);	u32FrameCnt <<= 8;
	u32FrameCnt |= xTXNONCE(3);	u32FrameCnt <<= 8;
	u32FrameCnt |= xTXNONCE(2);	u32FrameCnt <<= 8;
	u32FrameCnt |= xTXNONCE(1);

	return	u32FrameCnt;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the extended address for TX-NONCE.
///
///	@param	pu8ExtAddrBuff	: Pointer to the buffer of 64-bits extended address to be read.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecTxNonceExtAddrGet(UINT8* pu8ExtAddrBuff)
{
	rpmemcpy(pu8ExtAddrBuff, &xTXNONCE(5), 8);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the key sequence number for RX-NONCE.
///
///	@param	void
///	@return	UINT8. The sequence number.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_SecRxNonceKeySeqGet(void)
{
	return	xRXNONCE(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the frame counter for RX-NONCE.
///
///	@param	void
///	@return	UINT32. The frame counter.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT32 HAL_SecRxNonceFrameCntGet(void)
{
	UINT32	u32FrameCnt;

	u32FrameCnt = xRXNONCE(4);	u32FrameCnt <<= 8;
	u32FrameCnt |= xRXNONCE(3);	u32FrameCnt <<= 8;
	u32FrameCnt |= xRXNONCE(2);	u32FrameCnt <<= 8;
	u32FrameCnt |= xRXNONCE(1);

	return	u32FrameCnt;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the extended address for RX-NONCE.
///
///	@param	pu8ExtAddrBuff	: Pointer to the buffer of 64-bits extended address to be read.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecRxNonceExtAddrGet(UINT8* pu8ExtAddrBuff)
{
	rpmemcpy(pu8ExtAddrBuff, &xRXNONCE(5), 8);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the TXL register. It has different meanings depending on the security mode.
///		\n	CTR mode 		: The number of bytes between length byte and the data to be encrypted.
///		\n 	CBC-MAC mode 	: The number of bytes between length byte and the data to be authenticated.
///		\n	CCM mode		: The number of bytes authenticated but not encrypted.
///	@param	u8TXL	: The value of TXL to be written.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecTXLSet(UINT8 u8TXL)
{
	xTXL = u8TXL & 0x7F;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the RXL register. It has different meanings depending on the security mode.
///		\n	CTR mode 		: The number of bytes between length byte and the data to be decrypted.
///		\n 	CBC-MAC mode 	: The number of bytes between length byte and the data to be authenticated.
///		\n	CCM mode		: The number of bytes authenticated but not decrypted.
///	@param	u8TXL	: The value of RXL to be written.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecRXLSet(UINT8 u8RXL)
{
	xRXL = u8RXL & 0x7F;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Selects the key to be used for outgoing packets.
///
///	@param	u8KeyNum	: The number of key. 0=KEY0, 1=KEY1.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecTxKeyChoice(UINT8 u8KeyNum)
{
	if(u8KeyNum)
	{
		xMACSEC |= BIT6;
	}
	else
	{
		xMACSEC &= ~BIT6;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Selects the key to be used for incoming packets.
///
///	@param	u8KeyNum	: The number of key. 0=KEY0, 1=KEY1.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_SecRxKeyChoice(UINT8 u8KeyNum)
{
	if(u8KeyNum)
	{
		xMACSEC |= BIT5;
	}
	else
	{
		xMACSEC &= ~BIT5;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the length of MIC(Message Integration Code). 
///	MIC is generated after being secured by CBC-MAC or CCM mode.
///
///	@param	u8MicLength	: The length of MIC. Valid number is 2/4/6/8/10/12/14/16.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_SecMicLengthSet(UINT8 u8MicLength)
{
	UINT8	u8M;		// M value

	if(u8MicLength == 0)
	{
		assert_error(SEC_CODE | RETURN_SEC_WRONG_MIC_LENGTH);
		return	RETURN_SEC_WRONG_MIC_LENGTH;
	}
	if(u8MicLength > 16)
	{
		assert_error(SEC_CODE | RETURN_SEC_WRONG_MIC_LENGTH);
		return	RETURN_SEC_WRONG_MIC_LENGTH;
	}
	if(u8MicLength & 0x01)	// if odd number, error
	{
		assert_error(SEC_CODE | RETURN_SEC_WRONG_MIC_LENGTH);
		return	RETURN_SEC_WRONG_MIC_LENGTH;
	}

	u8M = u8MicLength/2 - 1;	// 0 ~ 7
	xMACSEC &= ~0x1C;		// bit[4:2]=0
	xMACSEC |= (u8M << 2);
	
	return	RETURN_SEC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the security mode to be used.
///
///	@param	u8Mode	: The security mode. 0=None, 1=CBC-MAC, 2=CTR, 3=CCM.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_SecModeSet(UINT8 u8Mode)
{
	if(u8Mode > SEC_MODE_CCM)
	{
		assert_error(SEC_CODE | RETURN_SEC_INVALID_PARAMETER);
		return RETURN_SEC_INVALID_PARAMETER;
	}	

	xMACSEC &= ~0x03;	// bit[1:0]=0
	xMACSEC |= (u8Mode & 0x03);

	return RETURN_SEC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///
/// 	Encrypts data with CCM mode whose MIC is 4-Bytes.
///
///	@param	pu8TextBuff	: Pointer to the buffer for plain-text and cipher-text.
///	@param	u8InLen		: The length of plain-text to be encrypted.
///	@param	u32FCnt		: 32-bits frame counter for TX-NONCE.
///	@param	u8KSeq		: Key sequence number for TX-NONCE.
///	@param	pu8EncLen	: Pointer to the length of encrypted cipher-text.
///	@return	UINT8. Status.
///	@note	The example of this function is as followings.
///
///		\n	< Prerequisite >
///		\n	KEY[0~15]		: A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF
///		\n	TX-NONCE's extended address[0~7] : B0 B1 B2 B3 B4 B5 B6 B7
///
///		\n	< Input >
///		\n	u8InLen  		: 64
///		\n	u32FCnt  		: 0x11223344
///		\n	u8KSeq  		: 0x55
///		\n	pu8TextBuff[0~63] :	
///		\n						10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 
///		\n					   	20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 
///		\n					   	30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 
///		\n					   	40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F
///
///		\n	< Output >
///		\n	*pu8EncLen	: 68
///		\n	pu8TextBuff[0~67] :	
///		\n						D9 07 7E 18 B7 9F 22 49 4C 42 08 B1 59 BB 06 88
///		\n						15 72 49 C0 95 F4 6B 10 10 F4 36 43 98 13 CC 5A
///		\n						5D 3B 2D AB A1 40 0D 00 DC B7 4F 85 11 3C E1 1B
///		\n						85 46 19 81 E4 5C C8 0E 4A E3 16 3E 16 72 12 CB
///		\n						EC 15 05 95 --> 4-bytes MIC
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_SecEncryptionCCM32(UINT8* pu8TextBuff, UINT8 u8InLen, UINT32 u32FCnt, UINT8 u8KSeq, UINT8* pu8EncLen)
{
	static	UINT16	su16Loop;
	UINT8	u8Status;
	UINT16	u16MemAddr;

	u16MemAddr = (UINT16)pu8TextBuff;
	if( (u16MemAddr >= 0x2300) && (u16MemAddr <= 0x24FF) )
	{
		assert_error(SEC_CODE | RETURN_SEC_WRONG_MEM_ADDR);
		return RETURN_SEC_WRONG_MEM_ADDR;
	}

	*pu8EncLen = 0;	
	if(u8InLen > 121)
	{
		assert_error(SEC_CODE | RETURN_SEC_TOO_LONG_TEXT);
		return RETURN_SEC_TOO_LONG_TEXT;
	}

	if(u8InLen == 1)
	{
		assert_error(SEC_CODE | RETURN_SEC_TOO_SHORT_TEXT);
		return RETURN_SEC_TOO_SHORT_TEXT;
	}

	SEC_CLK_ON;

	// Enable Mode
	HAL_SecModeSet(SEC_MODE_CCM);
	HAL_SecTxNonceFrameCntSet(u32FCnt);
	HAL_SecTxNonceKeySeqSet(u8KSeq);
	HAL_SecTXLSet(0);
	HAL_SecMicLengthSet(4);

	// Packet Length and PlainText
	xSECMAP = BIT0;
	xSTxFIFO(0) = u8InLen + 2;
	rpmemcpy(&xSTxFIFO(1), pu8TextBuff, u8InLen);

	//########################################################
	// Encryption
	//########################################################
	#define	EncDone_WaitLoopCnt  	600

	xSTFCSECBASE = 0;
	xSTFCSECLEN = u8InLen + 2;

	u8Status = RETURN_SEC_ENCRYPT_TIMEOUT;
	xMACSTS &= ~BIT2;
	xSTFCCTL |= BIT2;	// bit[2]=1, Start encryption
	xSECMAP = 0;

	for(su16Loop=0 ; su16Loop<EncDone_WaitLoopCnt ; su16Loop++)
	{
		if(xMACSTS & BIT2)
		{
			u8Status = RETURN_SEC_SUCCESS;
			break;
		}
	}

	if(u8Status == RETURN_SEC_SUCCESS)
	{
		*pu8EncLen = u8InLen + 4;
		xSECMAP = BIT0;
		rpmemcpy(pu8TextBuff, &xSTxFIFO(1), *pu8EncLen);
		xSECMAP = 0;
	}

	SEC_CLK_OFF;

	if(u8Status)
	{
		assert_error(SEC_CODE | u8Status);
	}
	
	return u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Decrypts data with CCM mode whose MIC is 4-Bytes.
///
///	@param	pu8TextBuff	: Pointer to the buffer for plain-text and cipher-text.
///	@param	u8InLen		: The length of cipher-text to be decrypted.
///	@param	u32FCnt		: 32-bits frame counter for RX-NONCE.
///	@param	u8KSeq		: Key sequence number for RX-NONCE.
///	@param	pu8DecLen	: Pointer to the length of decrypted plain-text.
///	@return	UINT8. Status.
///	@note	The example of this function is as followings.
///
///		\n	< Prerequisite >
///		\n	KEY[0~15]		: A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF
///		\n	RX-NONCE's extended address[0~7] : B0 B1 B2 B3 B4 B5 B6 B7
///
///		\n	< Input >
///		\n	u8InLen  		: 68
///		\n	u32FCnt  		: 0x11223344
///		\n	u8KSeq  		: 0x55
///		\n	pu8TextBuff[0~67] :	
///		\n						D9 07 7E 18 B7 9F 22 49 4C 42 08 B1 59 BB 06 88
///		\n						15 72 49 C0 95 F4 6B 10 10 F4 36 43 98 13 CC 5A
///		\n						5D 3B 2D AB A1 40 0D 00 DC B7 4F 85 11 3C E1 1B
///		\n						85 46 19 81 E4 5C C8 0E 4A E3 16 3E 16 72 12 CB
///		\n						EC 15 05 95 --> 4-bytes MIC
///
///		\n	< Output >
///		\n	*pu8DecLen	: 64
///		\n	pu8TextBuff[0~63] :	
///		\n						10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 
///		\n					   	20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F 
///		\n					   	30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 
///		\n					   	40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_SecDecryptionCCM32(UINT8* pu8TextBuff, UINT8 u8InLen, UINT32 u32FCnt, UINT8 u8KSeq, UINT8* pu8DecLen)
{
	static	UINT16	su16Loop;
	UINT8	u8Status;
	UINT16	u16MemAddr;

	u16MemAddr = (UINT16)pu8TextBuff;
	if( (u16MemAddr >= 0x2300) && (u16MemAddr <= 0x24FF) )
	{
		assert_error(SEC_CODE | RETURN_SEC_WRONG_MEM_ADDR);
		return RETURN_SEC_WRONG_MEM_ADDR;
	}

	*pu8DecLen = 0;
	if( (u8InLen > 125) || (u8InLen < 4) )
	{
		assert_error(SEC_CODE | RETURN_SEC_TOO_LONG_TEXT);
		return	RETURN_SEC_TOO_LONG_TEXT;
	}

	SEC_CLK_ON;

	// Enable Mode
	HAL_SecModeSet(SEC_MODE_CCM);
	HAL_SecRxNonceFrameCntSet(u32FCnt);
	HAL_SecRxNonceKeySeqSet(u8KSeq);
	HAL_SecRXLSet(0);
	HAL_SecMicLengthSet(4);

	// Packet Length
	xSECMAP = BIT0;
	xSRxFIFO(0) = u8InLen + 2;
	// Ciphertext Packet
	rpmemcpy(&xSRxFIFO(1), pu8TextBuff, u8InLen);
	//########################################################
	// Decryption
	//########################################################
	
	#define	DecDone_WaitLoopCnt  	600

	// Decryption
	xSRFCSECBASE = 0x00;
	xSRFCSECLEN = u8InLen + 2;

	u8Status = RETURN_SEC_DECRYPT_TIMEOUT;
	xMACSTS &= ~BIT1;
	xSRFCCTL |= BIT2;
	xSECMAP = 0;

	for(su16Loop=0 ; su16Loop<DecDone_WaitLoopCnt ; su16Loop++)
	{
		if(xMACSTS & BIT1)
		{
			u8Status = RETURN_SEC_SUCCESS;
			break;
		}
	}

	if(u8Status == RETURN_SEC_SUCCESS)
	{		
		// Copy Decrypted text (Plaintext)
		xSECMAP = BIT0;
		rpmemcpy(pu8TextBuff, &xSRxFIFO(1), u8InLen);		
		if(xSRxFIFO(1+u8InLen-1) == 0x00)
		{
			*pu8DecLen = u8InLen - 4;
		}
		else
		{
			u8Status = RETURN_SEC_MIC_FAILURE;
		}
		xSECMAP = 0;
	}

	SEC_CLK_OFF;

	if(u8Status)
	{
		assert_error(SEC_CODE | u8Status);
	}

	return u8Status;
}



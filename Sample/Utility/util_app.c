
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2014-01-23
	- Version		: V2.3

	[2014-01-23] V2.3
	- zPrintf_UartInterruptCheckPut() : enhanced.

	[2013-12-20] V2.2
	- zPrintf_UartInterruptCheckPut() : corrected.

	[2013-01-14] V2.1
	- zPrintf() : enhanced // UART interrupt enable/disable check

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "include_top.h"

#include "util_app.h"
#include "util_sys.h"

#include "uart.h"
#include "wdt.h"
#include "phy.h"

//SB//
#include "ib_sensor.h"


#ifdef __MG2470_UART0_INCLUDE__
void zPrintf_Uart0Put(UINT8 u8Data)
{
	UINT8	u8PutData;

	if(xU0_IER & BIT1)
	{
		SYS_Uart0Put(u8Data);
	}
	else
	{
		u8PutData = u8Data;		
		SYS_Uart0Put_Poll(&u8PutData, 1);
	}	
}

UINT8 zScanf_Uart0Get(UINT8* pu8Data)
{
	UINT8	u8Status;

	if(xU0_IER & BIT0)
	{
		u8Status = SYS_Uart0Get(pu8Data);
	}
	else
	{
		u8Status = SYS_Uart0Get_Poll(pu8Data);
	}

	return u8Status;
}
#endif

#ifdef __MG2470_UART1_INCLUDE__
void zPrintf_Uart1Put(UINT8 u8Data)
{
	UINT8	u8PutData;

	if(xU1_IER & BIT1)
	{
		SYS_Uart1Put(u8Data);
	}
	else
	{
		u8PutData = u8Data;		
		SYS_Uart1Put_Poll(&u8PutData, 1);
	}
}

UINT8 zScanf_Uart1Get(UINT8* pu8Data)
{
	UINT8	u8Status;

	if(xU1_IER & BIT0)
	{
		u8Status = SYS_Uart1Get(pu8Data);
	}
	else
	{
		u8Status = SYS_Uart1Get_Poll(pu8Data);
	}

	return u8Status;
}
#endif

void zPrintf_UartInterruptCheckPut(char* pu8Str)
{
	UINT8	u8Loop;
	UINT8	u8EA;

	u8EA = EA;
	EA = 0;
	xU0_THR = 0x0A; xU0_THR = 0x0D;	// LFCR
	xU1_THR = 0x0A; xU1_THR = 0x0D;	// LFCR
	DPH = 0;
	EA = u8EA;

	while(!(xU0_LSR & BIT5));	while(!(xU1_LSR & BIT5));	// Checks UART0,1 Status

	for(u8Loop = 0; u8Loop < strlen(pu8Str); u8Loop++)
	{
		u8EA = EA;
		EA = 0;
		xU0_THR = pu8Str[u8Loop];
		xU1_THR = pu8Str[u8Loop];
		DPH = 0;
		EA = u8EA;
	}
}

void zPrintf_UartInterruptCheck(UINT8 u8Port)
{
	char*	pu8Str;

	if(EA == 0)
	{
		pu8Str = " ## EA=0, Interrupt Disabled ##";
		zPrintf_UartInterruptCheckPut(pu8Str);

		if( (u8Port == 0) && (ES0 == 0) )
		{
			pu8Str = " ## UART0:Interrupt Disabled ##";
			zPrintf_UartInterruptCheckPut(pu8Str);
		}
		else if( (u8Port == 1) && (ES1 == 0) )
		{
			pu8Str = " ## UART1:Interrupt Disabled ##";
			zPrintf_UartInterruptCheckPut(pu8Str);
		}

		EA = 0;
		WDT = 0x55;
		WDT = 0xAA;
		WDT = 0x0A;
		WDT_CLK_ON;
		while(1);		// Watchdog Reset
	}
	else
	{
		if( (u8Port == 0) && (ES0 == 0) )
		{
			pu8Str = " ## UART0 : Interrupt Disabled ##";
			zPrintf_UartInterruptCheckPut(pu8Str);
		}
		else if( (u8Port == 1) && (ES1 == 0) )
		{
			pu8Str = " ## UART1 : Interrupt Disabled ##";
			zPrintf_UartInterruptCheckPut(pu8Str);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Formats a series of strings and numeric values. It builds a string to write via UART.
///	This is similar to the printf() of the general C-Language.
///
///	@param	u8Port	: UART port. 0=UART0, 1=UART1.
///	@param	fmt		: Pointer to a format string.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void zPrintf(UINT8 u8Port, char *fmt, ...)
{
	va_list	arg_ptr;
	char		acLocalText[64];
	UINT8	ib;

//	SYS_WdtSet(10000);
	
	zPrintf_UartInterruptCheck(u8Port);

	for(ib=0 ; ib<64 ; ib++) 
	{
		acLocalText[ib] = 0x00;
	}

	va_start(arg_ptr, fmt);
	vsprintf(acLocalText, fmt, arg_ptr);
	va_end(arg_ptr);

	if(u8Port & 0x01)		// SERIAL PORT1
	{
		#ifdef __MG2470_UART1_INCLUDE__
		for(ib=0 ; ib<64 ; ib++) 
		{
			if(acLocalText[ib] != 0)
			{
				#if	_UARTDEBUG_MODE
				zPrintf_Uart1Put(acLocalText[ib]);
				#endif
				if(acLocalText[ib] == KEYBOARD_LineFeed)	
				{
					#if	_UARTDEBUG_MODE
					zPrintf_Uart1Put(KEYBOARD_CarryRet);			
					#endif
				}
			}
			else
			{
				break;
			}
		}	
		#endif
	}
	else 					// SERIAL PORT0
	{
		#ifdef __MG2470_UART0_INCLUDE__
		for(ib=0 ; ib<64 ; ib++)
		{
			if(acLocalText[ib] != 0)
			{			
				zPrintf_Uart0Put(acLocalText[ib]);
				if(acLocalText[ib] == KEYBOARD_LineFeed)	
				{
					zPrintf_Uart0Put(KEYBOARD_CarryRet);
				}
			}
			else	
			{
				break;
			}
		}	
		#endif
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Stores 16-bit input hexa-decimal data by keyboard. When "ENTER" is pressed, the value is stored.
///	This is similar to the scanf() of the general C-Language.
///
///	@param	u8Port		: UART port. 0=UART0, 1=UART1.
///	@param	pu16InValue	: Pointer to the variable in which input data is stored.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void zScanf(UINT8 u8Port, UINT16* pu16InValue)
{
	char		cInteger;
	UINT8	u8KeyInput;
	UINT8	ib;

	ib = 0;
	*pu16InValue = 0;

	while(1) 
	{		
		SYS_WdtSet(10000);

		if(u8Port & 0x01)
		{
			#ifdef __MG2470_UART1_INCLUDE__
			if(zScanf_Uart1Get(&u8KeyInput)) 
			{
				cInteger = toint(u8KeyInput);

				if(cInteger != -1) 
				{
					if(ib != 4)
					{
						zPrintf_Uart1Put(u8KeyInput);
						*pu16InValue <<= 4;
						*pu16InValue |= cInteger;
						ib++;
					}
				}
				else 
				{
					if( (u8KeyInput == KEYBOARD_BACK) && (ib) ) 	// BackSpace
					{
						zPrintf_Uart1Put(KEYBOARD_BACK);
						zPrintf_Uart1Put(KEYBOARD_SPACE);
						zPrintf_Uart1Put(KEYBOARD_BACK);
						*pu16InValue = *pu16InValue >> 4;
						ib--;
					}
				
					if(u8KeyInput == KEYBOARD_CarryRet)
					{
						break;
					}
				}
			}	
			#endif		
		}
		else
		{
			#ifdef __MG2470_UART0_INCLUDE__
			if(zScanf_Uart0Get(&u8KeyInput)) 
			{
				cInteger = toint(u8KeyInput);

				if(cInteger != -1) 
				{
					if(ib != 4)
					{
						zPrintf_Uart0Put(u8KeyInput);
						*pu16InValue <<= 4;				
						*pu16InValue |= cInteger;
						ib++;
					}
				}
				else 
				{				
					if( (u8KeyInput == KEYBOARD_BACK) && (ib) ) 	// BackSpace
					{
						zPrintf_Uart0Put(KEYBOARD_BACK);
						zPrintf_Uart0Put(KEYBOARD_SPACE);
						zPrintf_Uart0Put(KEYBOARD_BACK);
						*pu16InValue = *pu16InValue >> 4;
						ib--;
					}

					if(u8KeyInput == KEYBOARD_CarryRet) 	// Enter
					{
						break;
					}				
				}
			}			
			#endif
			
		}
	
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Stores 8-bit input hexa-decimal data by keyboard. When "ENTER" is pressed, the value is stored.
///	This is similar to the scanf() of the general C-Language.
///
///	@param	u8Port		: UART port. 0=UART0, 1=UART1.
///	@param	pu8InValue	: Pointer to the variable in which input data is stored.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void zScanfByte(UINT8 u8Port, UINT8* pu8InValue)
{
	char		cInteger;
	UINT8	u8KeyInput;
	UINT8	u8DigitLocation;

	u8DigitLocation = 0;
	*pu8InValue = 0;

	while(1) 
	{		
		SYS_WdtSet(10000);

		if(u8Port & 0x01)
		{
			#ifdef __MG2470_UART1_INCLUDE__
			if(zScanf_Uart1Get(&u8KeyInput)) 
			{
				cInteger = toint(u8KeyInput);

				if(cInteger != -1) 
				{
					if(u8DigitLocation != 2)
					{
						zPrintf_Uart1Put(u8KeyInput);
						*pu8InValue <<= 4;
						*pu8InValue |= cInteger;
						u8DigitLocation++;
					}
				}
				else 
				{
					if( (u8KeyInput == KEYBOARD_BACK) && (u8DigitLocation) ) 	// BackSpace
					{
						zPrintf_Uart1Put(KEYBOARD_BACK);
						zPrintf_Uart1Put(KEYBOARD_SPACE);
						zPrintf_Uart1Put(KEYBOARD_BACK);
						*pu8InValue = *pu8InValue >> 4;
						u8DigitLocation--;
					}
				
					if(u8KeyInput == KEYBOARD_CarryRet)
					{
						break;
					}
				}
			}	
			#endif		
		}
		else
		{
			#ifdef __MG2470_UART0_INCLUDE__
			if(zScanf_Uart0Get(&u8KeyInput)) 
			{
				cInteger = toint(u8KeyInput);

				if(cInteger != -1) 
				{
					if(u8DigitLocation != 2)
					{
						zPrintf_Uart0Put(u8KeyInput);
						*pu8InValue <<= 4;
						*pu8InValue |= cInteger;
						u8DigitLocation++;
					}
				}
				else 
				{				
					if( (u8KeyInput == KEYBOARD_BACK) && (u8DigitLocation) ) 	// BackSpace
					{
						zPrintf_Uart0Put(KEYBOARD_BACK);
						zPrintf_Uart0Put(KEYBOARD_SPACE);
						zPrintf_Uart0Put(KEYBOARD_BACK);
						*pu8InValue = *pu8InValue >> 4;
						u8DigitLocation--;
					}
				
					if(u8KeyInput == KEYBOARD_CarryRet)
					{
						break;
					}
				}
			}			
			#endif
			
		}
	
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Stores 16-bit input decimal data by keyboard. When "ENTER" is pressed, the value is stored.
///	This is similar to the scanf() of the general C-Language.
///
///	@param	u8Port		: UART port. 0=UART0, 1=UART1.
///	@param	pu16InValue	: Pointer to the variable in which input data is stored.
///	@return	UINT8. Status. 1=Success, 0=Failed(Input data is out of 16-bit range).
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 zScanf_Decimal(UINT8 u8Port, UINT16 *pu16InValue)
{
	char		cInteger;
	UINT8	u8KeyInput;
	UINT8	ib;
	UINT8	au8Digit[5];
	UINT32	u32Temp;

	ib = 0;
	*pu16InValue = 0;
	au8Digit[0] = 0;
	au8Digit[1] = 0;
	au8Digit[2] = 0;
	au8Digit[3] = 0;
	au8Digit[4] = 0;

	while(1)
	{
		SYS_WdtSet(10000);

		if(u8Port & 0x01)
		{
			#ifdef __MG2470_UART1_INCLUDE__
			if(zScanf_Uart1Get(&u8KeyInput)) 
			{
				if( (u8KeyInput >= '0') && (u8KeyInput <= '9') )
				{
					cInteger = u8KeyInput - '0';

					if(ib < 4)
					{
						zPrintf_Uart1Put(u8KeyInput);
						au8Digit[4] = au8Digit[3];
						au8Digit[3] = au8Digit[2];
						au8Digit[2] = au8Digit[1];
						au8Digit[1] = au8Digit[0];
						au8Digit[0] = cInteger;
						ib++;
					}
					else if(ib == 4)
					{					
						u32Temp = au8Digit[3];	
						u32Temp *= 10000;
						u32Temp += (au8Digit[2] * 1000);
						u32Temp += (au8Digit[1] * 100);
						u32Temp += (au8Digit[0] * 10);						
						u32Temp += cInteger;

						if(u32Temp & 0xFFFF0000)
						{
						}
						else
						{
							zPrintf_Uart1Put(u8KeyInput);
							au8Digit[4] = au8Digit[3];
							au8Digit[3] = au8Digit[2];
							au8Digit[2] = au8Digit[1];
							au8Digit[1] = au8Digit[0];
							au8Digit[0] = cInteger;
							ib++;
						}					
					}
				}
				else
				{
					if( (u8KeyInput == KEYBOARD_BACK) && (ib) ) 	// BackSpace
					{					
						zPrintf_Uart1Put(KEYBOARD_BACK);
						zPrintf_Uart1Put(KEYBOARD_SPACE);
						zPrintf_Uart1Put(KEYBOARD_BACK);

						au8Digit[0] = au8Digit[1];
						au8Digit[1] = au8Digit[2];
						au8Digit[2] = au8Digit[3];
						au8Digit[3] = au8Digit[4];
						au8Digit[4] = 0;
						ib--;
					}
				
					if(u8KeyInput == KEYBOARD_CarryRet)	
					{
						u32Temp = au8Digit[4];	
						u32Temp *= 10000;
						u32Temp += (au8Digit[3] * 1000);
						u32Temp += (au8Digit[2] * 100);
						u32Temp += (au8Digit[1] * 10);
						u32Temp += (au8Digit[0] * 1);
						if(u32Temp & 0xFFFF0000)
						{
							return	0;
						}
						else	
						{
							*pu16InValue = u32Temp;
							return	1;
						}
					}				
				}			
			}	
			#endif
		}
		else
		{
			#ifdef __MG2470_UART0_INCLUDE__
			if(zScanf_Uart0Get(&u8KeyInput)) 
			{
				if( (u8KeyInput >= '0') && (u8KeyInput <= '9') )
				{
					cInteger = u8KeyInput - '0';

					if(ib < 4)
					{
						zPrintf_Uart0Put(u8KeyInput);
						au8Digit[4] = au8Digit[3];
						au8Digit[3] = au8Digit[2];
						au8Digit[2] = au8Digit[1];
						au8Digit[1] = au8Digit[0];
						au8Digit[0] = cInteger;
						ib++;
					}
					else if(ib == 4)
					{					
						u32Temp = au8Digit[3];	
						u32Temp *= 10000;
						u32Temp += (au8Digit[2] * 1000);
						u32Temp += (au8Digit[1] * 100);
						u32Temp += (au8Digit[0] * 10);						
						u32Temp += cInteger;

						if(u32Temp & 0xFFFF0000)
						{
						}
						else
						{
							zPrintf_Uart0Put(u8KeyInput);
							au8Digit[4] = au8Digit[3];
							au8Digit[3] = au8Digit[2];
							au8Digit[2] = au8Digit[1];
							au8Digit[1] = au8Digit[0];
							au8Digit[0] = cInteger;
							ib++;
						}					
					}
				}
				else
				{
					if( (u8KeyInput == KEYBOARD_BACK) && (ib) ) 	// BackSpace
					{					
						zPrintf_Uart0Put(KEYBOARD_BACK);
						zPrintf_Uart0Put(KEYBOARD_SPACE);
						zPrintf_Uart0Put(KEYBOARD_BACK);

						au8Digit[0] = au8Digit[1];
						au8Digit[1] = au8Digit[2];
						au8Digit[2] = au8Digit[3];
						au8Digit[3] = au8Digit[4];
						au8Digit[4] = 0;
						ib--;
					}
				
					if(u8KeyInput == KEYBOARD_CarryRet)	
					{
						u32Temp = au8Digit[4];	
						u32Temp *= 10000;
						u32Temp += (au8Digit[3] * 1000);
						u32Temp += (au8Digit[2] * 100);
						u32Temp += (au8Digit[1] * 10);
						u32Temp += (au8Digit[0] * 1);
						if(u32Temp & 0xFFFF0000)
						{
							return	0;
						}
						else	
						{
							*pu16InValue = u32Temp;
							return	1;
						}
					}				
				}			
			}	
			#endif
		}	
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable for virtual timer(Number 0). This is the number of remained ticks.
///	This is decreased by 1 every TIMER0 interrupts when it is not 0.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16VirtualTimer0 = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable for virtual timer(Number 1). This is the number of remained ticks.
///	This is decreased by 1 every TIMER0 interrupts when it is not 0.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16VirtualTimer1 = 0;
UINT16	gu16VirtualTimer2 = 0;
UINT16	gu16VirtualTimer3 = 0;
UINT16	gu16VirtualTimer4 = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets the number of ticks of virtual timers which is counted down.
///
///	@param	u8VirtualTimerNum		: 0=Virtual timer 0, 1=Virtual timer 1.
///	@param	u16Timer0Tick		: Number of ticks to be counted down by TIMER0 interrupts.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void AppLib_VirtualTimerSet(UINT8 u8VirtualTimerNum, UINT16 u16Timer0Tick)
{
	UINT8	u8ET0;

	u8ET0 = ET0;
	ET0 = 0;

	if(u8VirtualTimerNum == 0)
	{
		gu16VirtualTimer0 = u16Timer0Tick;
	}
	else if(u8VirtualTimerNum == 1)
	{
		gu16VirtualTimer1 = u16Timer0Tick;
	}
	else if(u8VirtualTimerNum == 2)
	{
		gu16VirtualTimer2 = u16Timer0Tick;
	}
	else if(u8VirtualTimerNum == 3)
	{
		gu16VirtualTimer3 = u16Timer0Tick;
	}
	else if(u8VirtualTimerNum == 4)
	{
		gu16VirtualTimer4 = u16Timer0Tick;
	}
	
	ET0 = u8ET0;	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the number of current ticks of virtual timers.
///
///	@param	u8VirtualTimerNum		: 0=Virtual timer 0, 1=Virtual timer 1.
///	@return	UINT16. Number of current ticks.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16 AppLib_VirtualTimerGet(UINT8 u8VirtualTimerNum)
{
	UINT16	u16TimerValue;

	if(u8VirtualTimerNum == 0)
	{
		u16TimerValue = gu16VirtualTimer0;
	}
	else if(u8VirtualTimerNum == 1)
	{
		u16TimerValue = gu16VirtualTimer1;
	}
	else if(u8VirtualTimerNum == 2)
	{
		u16TimerValue = gu16VirtualTimer2;
	}
	else if(u8VirtualTimerNum == 3)
	{
		u16TimerValue = gu16VirtualTimer3;
	}
	else
	{
		u16TimerValue = 0;
	}

	return u16TimerValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which contains parameters for MIB(MAC Information Base).
///	MIB has parameters for IEEE 802.15.4 network. It consists of channel, network(short) address, etc.
///	If not needed, this may be removed.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
tMAC_INFO_BASE	gtMIB;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Makes an IEEE 802.15.4 data packet.
///
///	@param	ptMacDataReq		: Pointer to the parameter variable for data packet to be made.
///	@param	pu8Psdu			: Pointer to the buffer in which the data packet is constructed.
///	@param	pu8PsduLen		: Pointer to the variable which indicates the length of constructed packet.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 AppLib_MacDataFrameMake(tMAC_DATA_REQ* ptMacDataReq, UINT8* pu8Psdu, UINT8* pu8PsduLen)
{
	UINT8	u8Len;
	UINT8	u8IntraPanSubfield;

	// Get u8IntraPanSubfield
	if( (ptMacDataReq->u8DstMode == maccAddrMode_16bit) || (ptMacDataReq->u8DstMode == maccAddrMode_64bit) )
	{
		if(ptMacDataReq->u16DstPanID == gtMIB.u16PanID)
		{
			u8IntraPanSubfield = 1;
		}
		else
		{
			u8IntraPanSubfield = 0;
		}
	}
	else
	{
		u8IntraPanSubfield = 0;
	}

	//-------------------------------------------------------------------------------
	// FrameControl_Low Field
	//-------------------------------------------------------------------------------
	//	b7		b6		b5		b4			b3		b2-b0
	//-------------------------------------------------------------------------------
	//	reserved	IntraPan	AckReq	FramePend	Security	FrameType
	//-------------------------------------------------------------------------------	
	pu8Psdu[0] = maccFrame_Data;
	if(ptMacDataReq->u8TxOption & BIT0)
	{
		pu8Psdu[0] |= maccFCL_AckReq;
	}
	if(u8IntraPanSubfield)
	{
		pu8Psdu[0] |= maccFCL_IntraPan;
	}

	//-------------------------------------------------------------------------------	
	// FrameControl_High Field
	//-------------------------------------------------------------------------------	
	//	b7-b6		b5-b4			b3-b2		b1-b0
	//-------------------------------------------------------------------------------	
	//	SrcMode		OobCode[3:2]		DstMode		OobCode[1:0]
	//-------------------------------------------------------------------------------
	pu8Psdu[1] = (ptMacDataReq->u8TxOption & BIT_5N4) >> 4;
	pu8Psdu[1] |= (ptMacDataReq->u8TxOption & BIT_7N6) >> 2;
	if(ptMacDataReq->u8DstMode == maccAddrMode_None)
	{
	}
	else if(ptMacDataReq->u8DstMode == maccAddrMode_16bit)
	{
		pu8Psdu[1] |= 0x08;
	}
	else if(ptMacDataReq->u8DstMode == maccAddrMode_64bit)
	{
		pu8Psdu[1] |= 0x0C;
	}
	else
	{
		return	maccSTA_INVALID_PARAMETER;
	}
	if(ptMacDataReq->u8SrcMode == maccAddrMode_None)
	{
	}
	else if(ptMacDataReq->u8SrcMode == maccAddrMode_16bit)
	{
		pu8Psdu[1] |= 0x80;
	}
	else if(ptMacDataReq->u8SrcMode == maccAddrMode_64bit)
	{
		pu8Psdu[1] |= 0xC0;
	}
	else
	{
		return	maccSTA_INVALID_PARAMETER;
	}
	
	// Sequence Number Field
	pu8Psdu[2] = ++gtMIB.u8DSN;

	u8Len = 3;

	// Destination Addressing Field
	if(ptMacDataReq->u8DstMode == maccAddrMode_16bit)
	{
		pu8Psdu[u8Len + 0] = ptMacDataReq->u16DstPanID;
		pu8Psdu[u8Len + 1] = ptMacDataReq->u16DstPanID >> 8;
		pu8Psdu[u8Len + 2] = ptMacDataReq->uDstAddr.u16Short;
		pu8Psdu[u8Len + 3] = ptMacDataReq->uDstAddr.u16Short >> 8;
		u8Len += 4;
	}
	else if(ptMacDataReq->u8DstMode == maccAddrMode_64bit)
	{
		pu8Psdu[u8Len + 0] = ptMacDataReq->u16DstPanID;
		pu8Psdu[u8Len + 1] = ptMacDataReq->u16DstPanID >> 8;
		rpmemcpy(&pu8Psdu[u8Len + 2], ptMacDataReq->uDstAddr.au8Ext, 8);
		u8Len += 10;
	}

	// Source Addressing Field
	if(ptMacDataReq->u8SrcMode == maccAddrMode_16bit)
	{
		if(u8IntraPanSubfield)
		{
			pu8Psdu[u8Len + 0] = gtMIB.u16ShortAddr;
			pu8Psdu[u8Len + 1] = gtMIB.u16ShortAddr >> 8;
			u8Len += 2;
		}
		else		// If IntraPanSubField = 1,, SrcPanID Field is not present
		{
			pu8Psdu[u8Len + 0] = gtMIB.u16PanID;
			pu8Psdu[u8Len + 1] = gtMIB.u16PanID >> 8;
			pu8Psdu[u8Len + 2] = gtMIB.u16ShortAddr;
			pu8Psdu[u8Len + 3] = gtMIB.u16ShortAddr >> 8;
			u8Len += 4;
		}
	}
	else if(ptMacDataReq->u8SrcMode == maccAddrMode_64bit)
	{
		if(u8IntraPanSubfield)
		{
			rpmemcpy(&pu8Psdu[u8Len + 0], gtMIB.au8IEEEAddr, 8);
			u8Len += 8;
		}
		else		// If IntraPanSubField = 1,, SrcPanID Field is not present
		{
			pu8Psdu[u8Len + 0] = gtMIB.u16PanID;
			pu8Psdu[u8Len + 1] = gtMIB.u16PanID >> 8;
			rpmemcpy(&pu8Psdu[u8Len + 2], gtMIB.au8IEEEAddr, 8);
			u8Len += 10;
		}
	}

	// Check Frame's Length
	*pu8PsduLen = u8Len;						// MAC Header's Length
	*pu8PsduLen += ptMacDataReq->u8MsduLen;	// Add MAC Payload(MSDU)'s Length
	*pu8PsduLen += 2;							// Add MAC Footer(FCS)'s Length. This field is added automatically by MG2410 when transmitting
	if(*pu8PsduLen > 127)
	{
		return	maccSTA_FRAME_TOO_LONG;
	}
	// Copy MAC Payload
	rpmemcpy(&pu8Psdu[u8Len + 0], ptMacDataReq->pu8Msdu, ptMacDataReq->u8MsduLen);

	return	maccSTA_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Transmits a packet in TXFIFO.
///
///	@param	u8NoAckRetry		: Number of retries when an ACK is not received.
///	@param	u8BackoffRetry	: Number of retries when CCA(Channel Clear Assessment) is failed.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 AppLib_TxFifoTransmit(UINT8 u8NoAckRetry, UINT8 u8BackoffRetry)
{
	UINT8	u8Status;
	UINT8	u8LoopNoAck;
	UINT8	u8LoopBackoff;
	UINT8	u8NoAckTxNum;
	UINT8	u8BackoffTxNum;
	
	Retry_count=0;
	
	u8NoAckTxNum = u8NoAckRetry + 1;
	u8BackoffTxNum = u8BackoffRetry + 1;

	for(u8LoopNoAck=0 ; u8LoopNoAck < u8NoAckTxNum ; u8LoopNoAck++)
	{	
		// 무선 상태 체크 Delay : 무선 상태 사용 가능 여부 
		for(u8LoopBackoff=0 ; u8LoopBackoff < u8BackoffTxNum ; u8LoopBackoff++)
		{
			u8Status = HAL_TxFifoTransmit();			// 함수 내부에서 자체 delay를 수행 하고 있음
			if(	(u8Status == RETURN_PHY_MODEM_BUSY)
			||	(u8Status == RETURN_PHY_CCA_FAILURE) )
			{
				// Here, Backoff Delay
				//Retry_count++;
			}
			else
			{
				break;
			}
		}

		if(u8Status == RETURN_PHY_SUCCESS)
		{
			Retry_count = u8LoopNoAck;		
			break;
		}
		else
		{
			// Here, NoAck Delay
		}
	}

	return u8Status;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Makes a MAC packet and transmits it.
///
///	@param	ptMacDataReq	: Parameter of the MAC packet to be transmitted.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 AppLib_MacDataRequest(tMAC_DATA_REQ* ptMacDataReq)
{
	UINT8	u8Status;
	UINT8	u8ResultedPsduLen;

	u8Status = AppLib_MacDataFrameMake(ptMacDataReq, &xMTxFIFO(1), &u8ResultedPsduLen);
	if(u8Status == maccSTA_SUCCESS)
	{
		xMTxFIFO(0) = u8ResultedPsduLen;
		u8Status = AppLib_TxFifoTransmit(gtMIB.u8NoAckRetry, gtMIB.u8BackoffRetry);
	}
	return u8Status;
}
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Delays for 1 micro-seconds.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void AppLib_DelayFor1us(void)
{
	_nop_();
}
void AppLib_DelayForus(UINT16 SBMilliSecond)
{
	static	UINT16	u16Count;

	for(u16Count=0 ; u16Count < SBMilliSecond ; u16Count++)
	{
		AppLib_DelayFor2us();
	}	
}
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Delays for 2 micro-seconds.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void AppLib_DelayFor2us(void)
{
	static	UINT8	ib;

	for(ib=0 ; ib<1 ; ib++);
}
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Delays for 10 micro-seconds.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void AppLib_DelayFor10us(void)
{
	static	UINT8	ib;

	for(ib=0 ; ib<9 ; ib++);
}
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Delays for 50 micro-seconds.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void AppLib_DelayFor50us(void)
{
	static	UINT8	ib;

	for(ib=0 ; ib<48 ; ib++);
}
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Delays for 100 micro-seconds.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void AppLib_DelayFor100us(void)
{
	static	UINT8	ib;

	for(ib=0 ; ib<96 ; ib++);
}
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Delays for 500 micro-seconds.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void AppLib_DelayFor500us(void)
{
	static	UINT16	iw;

	for(iw=0 ; iw<105 ; iw++);
}
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Delays for 1 milli-seconds.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void AppLib_DelayFor1ms(void)
{
	static	UINT16	iw;

	for(iw=0 ; iw<210 ; iw++);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Delays for "u16MilliSecond" milli-seconds.
///
///	@param	u16MilliSecond	: Number of milli-seconds.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void AppLib_Delay(UINT16 u16MilliSecond)
{
	static	UINT16	u16Count;

	for(u16Count=0 ; u16Count < u16MilliSecond ; u16Count++)
	{
		AppLib_DelayFor1ms();
	}	
}
 
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Displays 8-bit data array.
///
///	@param	pu8Buff		: Pointer to the first data to be displayed.
///	@param	u16Len		: Length of data to be displayed.
///	@param	u8Linefeed	: The number of data for linefeed.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void Display_Buffer(UINT8* pu8Buff, UINT16 u16Len, UINT8 u8Linefeed)
{
	UINT16	iw;

	for(iw=0 ; iw<u16Len ; iw++)
	{	
		if(u8Linefeed)
		{
			if( (iw % u8Linefeed) == 0)		zPrintf(1, "\n");
		}
		zPrintf(1, " %02x",(short)pu8Buff[iw]);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Displays information of a MIB(MAC Information Base).
///
///	@param	ptMIB	: Pointer to the structure variable of MIB.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void Display_MIB(tMAC_INFO_BASE* ptMIB)
{
	zPrintf(1, "\n");
	zPrintf(1, "\n [MIB]");
	zPrintf(1, "\n Chan=%u (%u MHz), ",(short)ptMIB->u8Channel,(2405 + (ptMIB->u8Channel - 11)*5));
	zPrintf(1, "  PanID=0x%04x",ptMIB->u16PanID);
	zPrintf(1, "\n ShortAddr=0x%04x",ptMIB->u16ShortAddr);
	zPrintf(1, " :IEEEAddr(0~7)=0x");
	Display_Buffer(ptMIB->au8IEEEAddr, 8, 0);
	zPrintf(1, "\n DstShortAddr=0x%04x",ptMIB->u16DstShortAddr);
	zPrintf(1, " :DstIEEEAddr(0~7)=0x");
	Display_Buffer(ptMIB->au8DstIEEEAddr, 8, 0);	
	zPrintf(1, "\n DataRate=0x%02x, ",(short)ptMIB->u8DataRate);
	zPrintf(1, " :TxPwrLevel=0x%02x, ",(short)ptMIB->u8TxPowerLevel);
	zPrintf(1, "  NoAckRetry=%u",(short)ptMIB->u8NoAckRetry);
	zPrintf(1, " :BackoffRetry=%u",(short)ptMIB->u8BackoffRetry);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Displays information of a MAC packet.
///
///	@param	ptMac	: Pointer to the structure variable of MAC packet.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void Display_MacInfo(tMAC_INFO* ptMac)
{
	zPrintf(1, "\n");
	zPrintf(1, "\n [RX-MAC-PKT]");
	zPrintf(1, "\n DSN=0x%02x, OOB=0x%02x",(short)ptMac->u8SeqNum, (short)ptMac->u8OobCode);	
	zPrintf(1, " :dBm=%d",(short)ptMac->i8RSSI);
	zPrintf(1, " :Corr=%d",(short)ptMac->u8Correlation);
	zPrintf(1, "\n Dst. Mode=%02x",(short)ptMac->uFC_H.bBit.u8DstMode);
	if(ptMac->uFC_H.bBit.u8DstMode == 2)
	{
		zPrintf(1, " :PanID=0x%04x",ptMac->u16DstPanID);
		zPrintf(1, " :ShortAddr=0x%04x",ptMac->uDstAddr.u16Short);
	}
	else if(ptMac->uFC_H.bBit.u8DstMode == 3)
	{
		zPrintf(1, " :PanID=0x%04x",ptMac->u16DstPanID);
		zPrintf(1, " :IEEE(0~7)=0x");
		Display_Buffer(ptMac->uDstAddr.au8Ext, 8, 0);
	}
	zPrintf(1, "\n Src. Mode=%02x",(short)ptMac->uFC_H.bBit.u8SrcMode);
	if(ptMac->uFC_H.bBit.u8SrcMode == 2)
	{
		zPrintf(1, " :PanID=0x%04x",ptMac->u16SrcPanID);
		zPrintf(1, " :ShortAddr=0x%04x",ptMac->uSrcAddr.u16Short);
	}
	else if(ptMac->uFC_H.bBit.u8SrcMode == 3)
	{
		zPrintf(1, " :PanID=0x%04x",ptMac->u16SrcPanID);
		zPrintf(1, " :IEEE(0~7)=0x");
		Display_Buffer(ptMac->uSrcAddr.au8Ext, 8, 0);
	}
	zPrintf(1, "\n MSDU Len=%02u : ",(short)ptMac->u8MacPayLen);	
	zPrintf(1, " :MSDU =");
	Display_Buffer(ptMac->pu8MacPayload, ptMac->u8MacPayLen, 16);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Displays information of a HIB(Hardware Information Base).
///
///	@param	ptMIB	: Pointer to the structure variable of HIB.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void Display_HIB(tHW_INFORMATION* ptHIB)
{
	zPrintf(1, "\n");
	zPrintf(1, "\n === HIB Display ===");
	zPrintf(1, "\n IEEE ADDR(LSB~MSB) :");	
	Display_Buffer(ptHIB->au8IEEEAddr, 8, 0);
	zPrintf(1, "\n ChipIndex = 0x%02x",(short)ptHIB->u8ChipID);
	if(ptHIB->u8ChipID == 1)			{zPrintf(1, " (MG2400)");}
	else if(ptHIB->u8ChipID == 2)		{zPrintf(1, " (MG245X)");}
	else if(ptHIB->u8ChipID == 3)		{zPrintf(1, " (MG2470)");}
	else if(ptHIB->u8ChipID == 4)		{zPrintf(1, " (MG2460)");}
	else if(ptHIB->u8ChipID == 0x80)	{zPrintf(1, " (MG2410)");}	// STM32F
	else								{zPrintf(1, " (Unclassified)");}
	zPrintf(1, "\n Channel   = %u (%u MHz)",(short)ptHIB->u8Channel,(2405 + (ptHIB->u8Channel - 11)*5));
	zPrintf(1, "\n StackID   = 0x%02x",(short)ptHIB->u8StackID);
	zPrintf(1, "\n TxPower   = 0x%02x",(short)ptHIB->u8TxPower);
	zPrintf(1, "    DataRate  = 0x%02x",(short)ptHIB->u8DataRate);
	zPrintf(1, "\n PanID     = 0x%02x%02x",(short)ptHIB->au8PanID[1], (short)ptHIB->au8PanID[0]);
	zPrintf(1, "  NwkAddr  = 0x%02x%02x",(short)ptHIB->au8NwkAddr[1], (short)ptHIB->au8NwkAddr[0]);
	zPrintf(1, "\n SecLevel  = 0x%02x",(short)ptHIB->u8SecurityLevel);
	zPrintf(1, "    PreConfig = 0x%02x",(short)ptHIB->u8PreConfigMode);
	zPrintf(1, "\n NWK Key (LSB~MSB) :");
	Display_Buffer(ptHIB->au8NwkKey, 16, 0);
	zPrintf(1, "\n Reserv_0(LSB~MSB) :");
	Display_Buffer(ptHIB->au8Reserved_0, 8, 0);
	zPrintf(1, "\n EPID    (LSB~MSB) :");
	Display_Buffer(ptHIB->au8EPID, 8, 0);
	zPrintf(1, "\n Reserv_1(LSB~MSB) :");
	Display_Buffer(ptHIB->au8Reserved_1, 8, 0);
	zPrintf(1, "\n GeneralWord0 = 0x%02x%02x",(short)ptHIB->au8GeneralWord0[1],(short)ptHIB->au8GeneralWord0[0]);
	zPrintf(1, "\n GeneralWord1 = 0x%02x%02x",(short)ptHIB->au8GeneralWord1[1],(short)ptHIB->au8GeneralWord1[0]);
	//zPrintf(1, "\n SUM = 0x%02x",(short)ptHIB->CSUM);
}


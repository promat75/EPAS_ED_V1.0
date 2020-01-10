
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2014-01-23
	- Version		: V2.3

	[2014-01-23] V2.3
	- SYS_Uart0Put_Poll() : enhanced.
	- SYS_Uart0Get_Poll() : enhanced.
	- SYS_Uart1Put_Poll() : enhanced.
	- SYS_Uart1Get_Poll() : enhanced.

	[2013-12-20] V2.2
	- SYS_Uart0Get_Poll() : corrected
	- SYS_Uart1Get_Poll() : corrected

	[2013-07-24] V2.1
	- SYS_Uart1Put() : corrected & enhanced
	- SYS_Uart1Put_Poll() : corrected & enhanced

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "INCLUDE_MG2470.h"
#include "uart.h"
#include "clock.h"

#if defined __MG2470_UART0_INCLUDE__ || defined __MG2470_UART1_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Constants which indicates XCR parameter which is required to set the UART baud rate.
///	\n	The XCR is dependent on the baud rate and MCU clock. Therefore, this is two-dimensional arrays.
///	\n	The column of the array indicates the baud rate.
///			\n	0 = 300 bps
///			\n	1 = 600 bps
///			\n	2 = 1200 bps
///			\n	3 = 2400 bps
///			\n	4 = 4800 bps
///			\n	5 = 9600 bps
///			\n	6 = 14400 bps
///			\n	7 = 19200 bps
///			\n	8 = 38400 bps
///			\n	9 = 56000 bps
///			\n	10 = 57600 bps
///			\n	11 = 115200 bps
///			\n	12 = 230400 bps
///			\n	13 = 460800 bps
///			\n	14 = 576000 bps
///			\n	15 = 1152000 bps
///	\n	The row of the array indicates the MCU clock mode.
///			\n 	0 = 16 MHz
///			\n 	1 = 8 MHz
///			\n 	2 = 4 MHz
///			\n 	3 = 2 MHz
///			\n 	4 = 1 MHz
///			\n 	5 = 500 KHz
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	code gcau8XCR[16][6] = 		// [Baud Rate][MCU Clock Mode]
	{
		0x07, 0x09, 0x43, 0x3B, 0x0B, 0x07,
		0x09, 0x43, 0x3B, 0x0B, 0x07, 0x07,
		0x43, 0x3B, 0x0B, 0x07, 0x07, 0x8B,
		0x3B, 0x0B, 0x07, 0x07, 0x8B, 0x04,
		0x0B, 0x07, 0x07, 0x8B, 0x04, 0x04,
		0x07, 0x07, 0x8B, 0x04, 0x04, 0x04,
		0x0B, 0x04, 0x8B, 0x06, 0x17, 0x05,
		0x07, 0x8B, 0x04, 0x04, 0x04, 0x0D,
		0x8B, 0x04, 0x04, 0x04, 0x0D, 0x00,
		0x0B, 0x0B, 0x04, 0x04, 0x00, 0x00,
		0x8B, 0x06, 0x17, 0x05, 0x00, 0x00,
		0x06, 0x17, 0x05, 0x00, 0x00, 0x00,
		0x17, 0x05, 0x00, 0x00, 0x00, 0x00,
		0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x04, 0x07, 0x00, 0x00, 0x00, 0x00,
		0x07, 0x00, 0x00, 0x00, 0x00, 0x00
	};

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Constants which indicates High Divisor parameter which is required to set the UART baud rate.
///	\n	The High Divisor is dependent on the baud rate and MCU clock. Therefore, this is two-dimensional arrays.
///	\n	The column of the array indicates the baud rate.
///			\n	0 = 300 bps
///			\n	1 = 600 bps
///			\n	2 = 1200 bps
///			\n	3 = 2400 bps
///			\n	4 = 4800 bps
///			\n	5 = 9600 bps
///			\n	6 = 14400 bps
///			\n	7 = 19200 bps
///			\n	8 = 38400 bps
///			\n	9 = 56000 bps
///			\n	10 = 57600 bps
///			\n	11 = 115200 bps
///			\n	12 = 230400 bps
///			\n	13 = 460800 bps
///			\n	14 = 576000 bps
///			\n	15 = 1152000 bps
///	\n	The row of the array indicates the MCU clock mode.
///			\n 	0 = 16 MHz
///			\n 	1 = 8 MHz
///			\n 	2 = 4 MHz
///			\n 	3 = 2 MHz
///			\n 	4 = 1 MHz
///			\n 	5 = 500 KHz
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	code gcau8HighDiv[16][6] = 		// [Baud Rate][MCU Clock Mode]
	{
		0x1D, 0x0B, 0x00, 0x00, 0x01, 0x00,
		0x0B, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00		
	};

///////////////////////////////////////////////////////////////////////////////////////////////
///
///	Constants which indicates Low Divisor parameter which is required to set the UART baud rate.
///	\n	The Low Divisor is dependent on the baud rate and MCU clock. Therefore, this is two-dimensional arrays.
///	\n	The column of the array indicates the baud rate.
///			\n	0 = 300 bps
///			\n	1 = 600 bps
///			\n	2 = 1200 bps
///			\n	3 = 2400 bps
///			\n	4 = 4800 bps
///			\n	5 = 9600 bps
///			\n	6 = 14400 bps
///			\n	7 = 19200 bps
///			\n	8 = 38400 bps
///			\n	9 = 56000 bps
///			\n	10 = 57600 bps
///			\n	11 = 115200 bps
///			\n	12 = 230400 bps
///			\n	13 = 460800 bps
///			\n	14 = 576000 bps
///			\n	15 = 1152000 bps
///	\n	The row of the array indicates the MCU clock mode.
///			\n 	0 = 16 MHz
///			\n 	1 = 8 MHz
///			\n 	2 = 4 MHz
///			\n 	3 = 2 MHz
///			\n 	4 = 1 MHz
///			\n 	5 = 500 KHz
///
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	code gcau8LowDiv[16][6] = 		// [Baud Rate][MCU Clock Mode]
	{
		0xC3, 0x93, 0xC7, 0x71, 0x2F, 0xEE,
		0x93, 0xC7, 0x71, 0x2F, 0xEE, 0x77,
		0xC7, 0x71, 0x2F, 0xEE, 0x77, 0x03,
		0x71, 0x2F, 0xEE, 0x77, 0x03, 0x34,
		0x2F, 0xEE, 0x77, 0x03, 0x34, 0x1A,
		0xEE, 0x77, 0x03, 0x34, 0x1A, 0x0D,
		0x65, 0x8B, 0x02, 0x17, 0x03, 0x07,
		0x77, 0x03, 0x34, 0x1A, 0x0D, 0x02,
		0x03, 0x34, 0x1A, 0x0D, 0x02, 0x00,
		0x1A, 0x0D, 0x12, 0x09, 0x00, 0x00,
		0x02, 0x17, 0x03, 0x07, 0x00, 0x00,
		0x17, 0x03, 0x07, 0x00, 0x00, 0x00,
		0x03, 0x07, 0x00, 0x00, 0x00, 0x00,
		0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x07, 0x02, 0x00, 0x00, 0x00, 0x00,
		0x02, 0x00, 0x00, 0x00, 0x00, 0x00
	};

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets parameters required to set UART baud rate.
///
///	@param	u32BaudRate	: Baud rate to set.
///	@param	pu8XCR		: Pointer to XCR to be obtained.
///	@param	pu8HighDiv	: Pointer to High Divisor to be obtained.
///	@param	pu8LowDiv	: Pointer to Low Divisor to be obtained.
///	@return	UINT8. Status.
///	
//////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_UartBaudRateParameterGet(UINT32 u32BaudRate, UINT8* pu8XCR, UINT8* pu8HighDiv, UINT8* pu8LowDiv)
{
	UINT8	u8Status;
	UINT8	u8BaudRateIndex;

	u8Status = RETURN_UART_SUCCESS;
	switch(u32BaudRate)
	{
		case 300		: u8BaudRateIndex = 0;	break;
		case 600		: u8BaudRateIndex = 1;	break;
		case 1200	: u8BaudRateIndex = 2;	break;
		case 2400	: u8BaudRateIndex = 3;	break;
		case 4800	: u8BaudRateIndex = 4;	break;
		case 9600	: u8BaudRateIndex = 5;	break;
		case 14400	: u8BaudRateIndex = 6;	break;
		case 19200	: u8BaudRateIndex = 7;	break;
		case 38400	: u8BaudRateIndex = 8;	break;
		case 56000	: u8BaudRateIndex = 9;	break;
		case 57600	: u8BaudRateIndex = 10;	break;
		case 115200	: u8BaudRateIndex = 11;	break;
		case 230400	: u8BaudRateIndex = 12;	break;
		case 460800	: u8BaudRateIndex = 13;	break;
		case 576000	: u8BaudRateIndex = 14;	break;
		case 1152000	: u8BaudRateIndex = 15;	break;
		default		: u8Status = RETURN_UART_INVALID_BAUDRATE;		break;
	}

	if(u8Status == RETURN_UART_SUCCESS)
	{
		*pu8XCR = gcau8XCR[u8BaudRateIndex][gu8McuPeriClockMode];
		*pu8HighDiv = gcau8HighDiv[u8BaudRateIndex][gu8McuPeriClockMode];
		*pu8LowDiv = gcau8LowDiv[u8BaudRateIndex][gu8McuPeriClockMode];

		if(*pu8LowDiv == 0x00)
		{
			u8Status = RETURN_UART_INVALID_BAUDRATE;
		}
	}

	return u8Status;	
}
#endif


#ifdef __MG2470_UART0_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable for UART0's RX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gau8Uart0Rx_Buff[UART0_RXBUF_SIZE];	

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates index number for writing UART0's RX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16Uart0Rx_WrIdx = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates index number for reading UART0's RX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16Uart0Rx_RdIdx = 0;		

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable for UART0's TX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gau8Uart0Tx_Buff[UART0_TXBUF_SIZE];

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates index number for writing UART0's TX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16Uart0Tx_WrIdx = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates index number for reading UART0's TX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16Uart0Tx_RdIdx = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether UART0's TX Buffer is empty
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8Uart0Tx_Empty = 1;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for UART0.
///
///	@param	u8On	: 0=Off, 1=On
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Uart0ClockOn(UINT8 u8On)		// Default ON
{
	if(u8On)
	{
		UART0_CLK_ON;
	}
	else
	{
		UART0_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets UART0 interrupt.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u32BaudRate	: Baud rate to set.
///						\n The real baud rate is dependent on the MCU frequency. And, The MCU frequency is 
///							determined by HAL_McuPeriClockSet(). Therefore, some baud rate is not supported 
///							on some MCU frequency.
///
///						\n Here is the table which indicates the baud rate supported on each MCU frequency.
///						\n	-----------------------------------------------------------------
///						\n				|				MCU Frequency (Hz)
///						\n	                     |----------------------------------------------------
///						\n				| 16M	| 8M (Default)	| 4M		| 2M		| 1M		| 500K	|
///						\n	-----------------------------------------------------------------
///						\n 	u32BaudRate	|
///						\n		300		| O		| O			| O		| O		| O		| O		|
///						\n		600		| O		| O			| O		| O		| O		| O		|
///						\n		1200	| O		| O			| O		| O		| O		| O		|
///						\n		2400	| O		| O			| O		| O		| O		| O		|
///						\n		4800	| O		| O			| O		| O		| O		| O		|
///						\n		9600	| O		| O			| O		| O		| O		| O		|
///						\n		14400	| O		| O			| O		| O		| O		| O		|
///						\n		19200	| O		| O			| O		| O		| O		| O		|
///						\n		38400	| O		| O			| O		| O		| O		| -		|
///						\n		56000	| O		| O			| O		| O		| -		| -		|
///						\n		57600	| O		| O			| O		| O		| -		| -		|
///						\n		115200	| O		| O			| O		| -		| -		| -		|
///						\n		230400	| O		| O			| -		| -		| -		| -		|
///						\n		460800	| O		| -			| -		| -		| -		| -		|
///						\n		576000	| O		| O			| -		| -		| -		| -		|
///						\n		1152000	| O		| -			| -		| -		| -		| -		|
///						\n	-----------------------------------------------------------------
///	@param	u8Mode		: UART mode
///						\n	bit[7] 	: 1=Disable UART-TX interrupt, 0=Enable.
///						\n	bit[6] 	: 1=Disable UART-RX interrupt, 0=Enable.
///						\n	bit[5:3]	: Parity Selection. N=No, O=Odd, E=Even, M=Mark, S=Space.
///						\n		b[5]		b[4]		b[3]
///						\n		1		1		1	: Space(parity bit is '0')
///						\n		1		0		1	: Mark(parity bit is '1')
///						\n		0		1		1	: Even Parity(if the data has an even number of '1', the parity bit is '0')
///						\n		0		0		1	: Odd Parity(if the data has an even number of '1', the parity bit is '1')
///						\n		X		X		0	: No Parity
///						\n	bit[2]	: length of stop bits. 0=1 stop bit. 1=1.5 stop bits(5bit-character) or 2 stop bits(6/7/8bit-character)
///						\n	bit[1:0]	: bits in each character. 0=5bits, 1=6bits, 2=7bits, 3=8bits
///
///						\n	--------------------------------------------
///						\n		Example for u8Mode (Character-Parity-Stop)
///						\n	--------------------------------------------
///						\n		8N1		8O1		8E1		8M1		8S1
///						\n		0x03	0x0B	0x1B	0x2B	0x3B
///						\n	--------------------------------------------
///						\n		8N2		8O2		8E2		8M2		8S2
///						\n		0x07	0x0F		0x1F		0x2F		0x3F
///						\n	--------------------------------------------
///						\n		7N1		7O1		7E1		7M1		7S1
///						\n		0x02	0x0A	0x1A	0x2A	0x3A
///						\n	--------------------------------------------
///						\n		7N2		7O2		7E2		7M2		7S2
///						\n		0x06	0x0E	0x1E	0x2E	0x3E
///						\n	--------------------------------------------
///	@return	UINT8. Status.
///	
//////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Uart0IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT32 u32BaudRate, UINT8 u8Mode)
{
	UINT8	u8EA;
	UINT8	u8HighDiv;
	UINT8	u8LowDiv;
	UINT8	u8XCR;
	UINT8	u8Status;

	if(UART0_CLK_STATE == 0)
	{
		assert_error(UART_CODE | RETURN_UART_CLOCK_OFF);
		return RETURN_UART_CLOCK_OFF;
	}

	u8Status = RETURN_UART_SUCCESS;
	if(u8IntEna)
	{		
		u8Status = HAL_UartBaudRateParameterGet(u32BaudRate, &u8XCR, &u8HighDiv, &u8LowDiv);
	}
	else
	{
		u8XCR = 0;	
		u8HighDiv = 0;
		u8LowDiv = 0;
	}	

	if(u8Status)
	{
		assert_error(UART_CODE | u8Status);
		return u8Status;
	}

	u8EA = EA;
	EA = 0;

	xU0_ECR = 0x00;
	xU0_LCR = BIT7;		// bit[7]=1
	xU0_DLL = u8LowDiv;
	xU0_DLM = u8HighDiv;
	xU0_XCR = u8XCR;
	xU0_LCR = 0x00;		// bit[7]=0
	xU0_XCR = 0;

	if(u8IntEna)
	{	
		xU0_FCR = 0x81;
		xU0_LCR = u8Mode & (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
		xU0_IER = 0x00;
		if( (u8Mode & BIT6) == 0)	xU0_IER |= BIT0;		// Enable Data Available interrupt
		if( (u8Mode & BIT7) == 0)	xU0_IER |= BIT1;		// Enable Holding Register interrupt
		
		ES0 = 1;
		PS0 = u8Priority;
	}
	else
	{
		xU0_IER = 0x00;
		ES0 = 0;
		PS0 = 0;
	}

	EA = u8EA;

	return RETURN_UART_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the number of pended bytes in UART0 RX Buffer.
///
///	@param	void
///	@return	UINT16. The number of bytes.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16 SYS_Uart0RxLenGet(void)
{
	UINT8	u8ES0;
	UINT8	u8PHYIE;
	UINT16 	u16RxLen;

	u8PHYIE = PHYIE;
	PHYIE = 0;
	u8ES0 = ES0;
	ES0 = 0;
	
	u16RxLen = (gu16Uart0Rx_WrIdx - gu16Uart0Rx_RdIdx) & (UART0_RXBUF_SIZE - 1);

	ES0 = u8ES0;
	PHYIE = u8PHYIE;

	return u16RxLen;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the number of pended bytes in UART0 TX Buffer.
///
///	@param	void
///	@return	UINT16. The number of bytes.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16 SYS_Uart0TxLenGet(void)
{
	UINT8	u8ES0;
	UINT8	u8PHYIE;
	UINT16	u16TxLen;

	u8PHYIE = PHYIE;
	PHYIE = 0;
	u8ES0 = ES0;
	ES0 = 0;	

	u16TxLen = (gu16Uart0Tx_WrIdx - gu16Uart0Tx_RdIdx) & (UART0_TXBUF_SIZE - 1);

	ES0 = u8ES0;
	PHYIE = u8PHYIE;

	return u16TxLen;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Puts a byte to UART0 TX Buffer.
///
///	@param	u8Data	: data to put.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void SYS_Uart0Put(UINT8 u8Data)
{
	UINT8	u8ES0;
	UINT8	u8PHYIE;
	UINT16	u16TxEmptyLen;

	while(1)
	{
		u16TxEmptyLen = UART0_TXBUF_SIZE - SYS_Uart0TxLenGet();
		if(u16TxEmptyLen > 3)
		{
			break;
		}
	}

	u8PHYIE = PHYIE;
	PHYIE = 0;
	u8ES0 = ES0;
	ES0 = 0;	

	if(gu8Uart0Tx_Empty)
	{
		gu8Uart0Tx_Empty = 0;
		EA = 0;
		xU0_THR = u8Data;
		DPH = 0;
		EA = 1;
	}
	else
	{
		gau8Uart0Tx_Buff[gu16Uart0Tx_WrIdx] = u8Data;
		gu16Uart0Tx_WrIdx++; 
		gu16Uart0Tx_WrIdx = gu16Uart0Tx_WrIdx & (UART0_TXBUF_SIZE-1);
	}

	ES0 = u8ES0;
	PHYIE = u8PHYIE;	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets a byte from UART0 RX Buffer.
///
///	@param	pu8Data	: Pointer to the data to be got.
///	@return	UINT8. Status. 0=Buffer is empty, 1=Buffer is not empty and data is got.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Uart0Get(UINT8* pu8Data)
{
	UINT8	u8ES0;
	UINT8	u8PHYIE;

	if(SYS_Uart0RxLenGet() == 0)
	{
		return 0;
	}

	u8PHYIE = PHYIE;
	PHYIE = 0;
	u8ES0 = ES0;
	ES0 = 0;
	
	*pu8Data = gau8Uart0Rx_Buff[gu16Uart0Rx_RdIdx++];	
	gu16Uart0Rx_RdIdx = gu16Uart0Rx_RdIdx & (UART0_RXBUF_SIZE - 1);

	ES0 = u8ES0;
	PHYIE = u8PHYIE;
	
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Checks whether UART0 is transmitting.
///
///	@param	void
///	@return	UINT8. Status. 0=Not transmitting, 1=Transmitting.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Uart0TxWorking(void)
{
	UINT8	u8Working;

	u8Working = 0;
	if(gu8Uart0Tx_Empty == 0)
	{
		u8Working = 1;
	}
	if( (xU0_LSR & BIT5) == 0)
	{
		u8Working = 1;
	}

	return u8Working;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Puts a byte to UART0 H/W FIFO with non-interrupt
///
///	@param	pu8DataBuff	: Pointer to the buffer to be sent.
///	@param	u8PutLen		: Number of data to be sent. 0 ~ 16.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Uart0Put_Poll(UINT8* pu8DataBuff, UINT8 u8PutLen)
{
	UINT8	ib;
	UINT8	u8EA;

	if(xU0_IER & BIT1)
	{
		return RETURN_UART_INTERRUPT_ENABLED;
	}

	if(u8PutLen > 16)
	{
		return RETURN_UART_FIFO_OVERFLOW;	
	}
	
	while(!(xU0_LSR & BIT5));
	for(ib=0 ; ib<u8PutLen ; ib++)
	{
		u8EA = EA;
		EA = 0;
		xU0_THR = pu8DataBuff[ib];
		DPH = 0;
		EA = u8EA;
	}

	return RETURN_UART_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets a byte from UART0 H/W FIFO with non-interrupt
///
///	@param	pu8Data	: Pointer to the data to be got.
///	@return	UINT8. Status. 0=No received data, 1=Data is received.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Uart0Get_Poll(UINT8* pu8Data)
{
	UINT8	u8EA;

	if(xU0_LSR & 0x01)
	{
		u8EA = EA;
		EA = 0;
		*pu8Data = xU0_RBR;
		DPH = 0;
		EA = u8EA;
		return 1;
	}
	else
	{
		return 0;
	}	
}

#endif	// #ifdef __MG2470_UART0_INCLUDE__


#ifdef __MG2470_UART1_INCLUDE__

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable for UART1's RX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gau8Uart1Rx_Buff[UART1_RXBUF_SIZE];	

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates index number for writing UART1's RX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16Uart1Rx_WrIdx = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates index number for reading UART1's RX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16Uart1Rx_RdIdx = 0;		

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable for UART1's TX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gau8Uart1Tx_Buff[UART1_TXBUF_SIZE];

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates index number for writing UART1's TX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16Uart1Tx_WrIdx = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates index number for reading UART1's TX Buffer
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16	gu16Uart1Tx_RdIdx = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates whether UART1's TX Buffer is empty
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8Uart1Tx_Empty = 1;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets clock for UART1.
///
///	@param	u8On	: 0=Off, 1=On.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void HAL_Uart1ClockOn(UINT8 u8On)		// Default ON
{
	if(u8On)
	{
		UART1_CLK_ON;
	}
	else
	{
		UART1_CLK_OFF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Sets UART1 interrupt.
///
///	@param	u8IntEna		: 0=Disable, 1=Enable.
///	@param	u8Priority		: Priority of interrupt. 0=Low, 1=High.
///	@param	u32BaudRate	: Baud rate to set.
///						\n The real baud rate is dependent on the MCU frequency. And, The MCU frequency is 
///							determined by HAL_McuPeriClockSet(). Therefore, some baud rate is not supported 
///							on some MCU frequency.
///
///						\n Here is the table which indicates the baud rate supported on each MCU frequency.
///						\n	-----------------------------------------------------------------
///						\n				|				MCU Frequency (Hz)
///						\n	                     |----------------------------------------------------
///						\n				| 16M	| 8M (Default)	| 4M		| 2M		| 1M		| 500K	|
///						\n	-----------------------------------------------------------------
///						\n 	u32BaudRate	|
///						\n		300		| O		| O			| O		| O		| O		| O		|
///						\n		600		| O		| O			| O		| O		| O		| O		|
///						\n		1200	| O		| O			| O		| O		| O		| O		|
///						\n		2400	| O		| O			| O		| O		| O		| O		|
///						\n		4800	| O		| O			| O		| O		| O		| O		|
///						\n		9600	| O		| O			| O		| O		| O		| O		|
///						\n		14400	| O		| O			| O		| O		| O		| O		|
///						\n		19200	| O		| O			| O		| O		| O		| O		|
///						\n		38400	| O		| O			| O		| O		| O		| -		|
///						\n		56000	| O		| O			| O		| O		| -		| -		|
///						\n		57600	| O		| O			| O		| O		| -		| -		|
///						\n		115200	| O		| O			| O		| -		| -		| -		|
///						\n		230400	| O		| O			| -		| -		| -		| -		|
///						\n		460800	| O		| -			| -		| -		| -		| -		|
///						\n		576000	| O		| O			| -		| -		| -		| -		|
///						\n		1152000	| O		| -			| -		| -		| -		| -		|
///						\n	-----------------------------------------------------------------
///	@param	u8Mode		: UART mode
///						\n	bit[7] 	: 1=Disable UART-TX interrupt, 0=Enable.
///						\n	bit[6] 	: 1=Disable UART-RX interrupt, 0=Enable.
///						\n	bit[5:3]	: Parity Selection. N=No, O=Odd, E=Even, M=Mark, S=Space.
///						\n		b[5]		b[4]		b[3]
///						\n		1		1		1	: Space(parity bit is '0')
///						\n		1		0		1	: Mark(parity bit is '1')
///						\n		0		1		1	: Even Parity(if the data has an even number of '1', the parity bit is '0')
///						\n		0		0		1	: Odd Parity(if the data has an even number of '1', the parity bit is '1')
///						\n		X		X		0	: No Parity
///						\n	bit[2]	: length of stop bits. 0=1 stop bit. 1=1.5 stop bits(5bit-character) or 2 stop bits(6/7/8bit-character)
///						\n	bit[1:0]	: bits in each character. 0=5bits, 1=6bits, 2=7bits, 3=8bits
///
///						\n	--------------------------------------------
///						\n		Example for u8Mode (Character-Parity-Stop)
///						\n	--------------------------------------------
///						\n		8N1		8O1		8E1		8M1		8S1
///						\n		0x03	0x0B	0x1B	0x2B	0x3B
///						\n	--------------------------------------------
///						\n		8N2		8O2		8E2		8M2		8S2
///						\n		0x07	0x0F		0x1F		0x2F		0x3F
///						\n	--------------------------------------------
///						\n		7N1		7O1		7E1		7M1		7S1
///						\n		0x02	0x0A	0x1A	0x2A	0x3A
///						\n	--------------------------------------------
///						\n		7N2		7O2		7E2		7M2		7S2
///						\n		0x06	0x0E	0x1E	0x2E	0x3E
///						\n	--------------------------------------------
///	@return	UINT8. Status.
///	
//////////////////////////////////////////////////////////////////////////////////////////////
UINT8 HAL_Uart1IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT32 u32BaudRate, UINT8 u8Mode) 
{
	UINT8	u8EA;
	UINT8	u8HighDiv;
	UINT8	u8LowDiv;
	UINT8	u8XCR;
	UINT8	u8Status;

	if(UART1_CLK_STATE == 0)
	{
		assert_error(UART_CODE | RETURN_UART_CLOCK_OFF);
		return RETURN_UART_CLOCK_OFF;
	}

	u8Status = RETURN_UART_SUCCESS;
	if(u8IntEna)
	{		
		u8Status = HAL_UartBaudRateParameterGet(u32BaudRate, &u8XCR, &u8HighDiv, &u8LowDiv);
	}
	else
	{
		u8XCR = 0;
		u8HighDiv = 0;
		u8LowDiv = 0;
	}

	if(u8Status)
	{
		assert_error(UART_CODE | u8Status);
		return u8Status;
	}

	u8EA = EA;
	EA = 0;

	xU1_ECR = 0x00;
	xU1_LCR = BIT7;		// bit[7]=1
	xU1_DLL = u8LowDiv;
	xU1_DLM = u8HighDiv;
	xU1_XCR = u8XCR;
	xU1_LCR = 0x00;		// bit[7]=0
	xU1_XCR = 0;

	if(u8IntEna)
	{
		xU1_FCR = 0x81;
		xU1_LCR = u8Mode & (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
		xU1_IER = 0x00;
		if( (u8Mode & BIT6) == 0)	xU1_IER |= BIT0;		// Enable Data Available interrupt
		if( (u8Mode & BIT7) == 0)	xU1_IER |= BIT1;		// Enable Holding Register interrupt
		
		ES1 = 1;
		PS1 = u8Priority;
	}
	else
	{
		xU1_IER = 0x00;
		ES1 = 0;
		PS1 = 0;
	}

	EA = u8EA;

	return RETURN_UART_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the number of pended bytes in UART1 RX Buffer.
///
///	@param	void
///	@return	UINT16. The number of bytes.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16 SYS_Uart1RxLenGet(void)
{
	UINT8	u8ES1;
	UINT8	u8PHYIE;
	UINT16 	u16RxLen;

	u8PHYIE = PHYIE;
	PHYIE = 0;
	u8ES1 = ES1;
	ES1 = 0;

	u16RxLen = (gu16Uart1Rx_WrIdx - gu16Uart1Rx_RdIdx) & (UART1_RXBUF_SIZE - 1);

	ES1 = u8ES1;
	PHYIE = u8PHYIE;

	return u16RxLen;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets the number of pended bytes in UART1 TX Buffer.
///
///	@param	void
///	@return	UINT16. The number of bytes.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT16 SYS_Uart1TxLenGet(void)
{
	UINT8	u8ES1;
	UINT8	u8PHYIE;
	UINT16	u16TxLen;

	u8PHYIE = PHYIE;
	PHYIE = 0;
	u8ES1 = ES1;
	ES1 = 0;

	u16TxLen = (gu16Uart1Tx_WrIdx - gu16Uart1Tx_RdIdx) & (UART1_TXBUF_SIZE - 1);

	ES1 = u8ES1;
	PHYIE = u8PHYIE;

	return u16TxLen;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Puts a byte to UART1 TX Buffer.
///
///	@param	u8Data	: data to put.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void SYS_Uart1Put(UINT8 u8Data)
{
	UINT8	u8ES1;
	UINT8	u8PHYIE;
	UINT16	u16TxEmptyLen;

	while(1)
	{
		u16TxEmptyLen = UART1_TXBUF_SIZE - SYS_Uart1TxLenGet();
		if(u16TxEmptyLen > 3)
		{
			break;
		}
	}

	u8PHYIE = PHYIE;
	PHYIE = 0;
	u8ES1 = ES1;
	ES1 = 0;	

	if(gu8Uart1Tx_Empty)
	{
		gu8Uart1Tx_Empty = 0;
		EA = 0;
		xU1_THR = u8Data;
		DPH = 0;
		EA = 1;
	}
	else
	{
		gau8Uart1Tx_Buff[gu16Uart1Tx_WrIdx] = u8Data;
		gu16Uart1Tx_WrIdx++; 
		gu16Uart1Tx_WrIdx = gu16Uart1Tx_WrIdx & (UART1_TXBUF_SIZE-1) ;
	}

	ES1 = u8ES1;
	PHYIE = u8PHYIE;	
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets a byte from UART1 RX Buffer.
///
///	@param	pu8Data	: Pointer to the data to be got.
///	@return	UINT8. Status. 0=Buffer is empty, 1=Buffer is not empty and data is got.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Uart1Get(UINT8* pu8Data)
{
	UINT8	u8ES1;
	UINT8	u8PHYIE;
	
	if(SYS_Uart1RxLenGet() == 0)
	{
		return 0;
	}

	u8PHYIE = PHYIE;
	PHYIE = 0;
	u8ES1 = ES1;
	ES1 = 0;

	*pu8Data = gau8Uart1Rx_Buff[gu16Uart1Rx_RdIdx++]; 
	gu16Uart1Rx_RdIdx = gu16Uart1Rx_RdIdx & (UART1_RXBUF_SIZE - 1);

	ES1 = u8ES1;
	PHYIE = u8PHYIE;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Checks whether UART1 is transmitting.
///
///	@param	void
///	@return	UINT8. Status. 0=Not transmitting, 1=Transmitting.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Uart1TxWorking(void)
{
	UINT8	u8Working;

	u8Working = 0;
	if(gu8Uart1Tx_Empty == 0)
	{
		u8Working = 1;
	}
	if( (xU1_LSR & BIT5) == 0)
	{
		u8Working = 1;
	}

	return	u8Working;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Puts a byte to UART1 H/W FIFO with non-interrupt
///
///	@param	pu8DataBuff	: Pointer to the buffer to be sent.
///	@param	u8PutLen		: Number of data to be sent. 0 ~ 16.
///	@return	UINT8. Status.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Uart1Put_Poll(UINT8* pu8DataBuff, UINT8 u8PutLen)
{
	UINT8	ib;
	UINT8	u8EA;

	if(xU1_IER & BIT1)
	{
		return RETURN_UART_INTERRUPT_ENABLED;
	}

	if(u8PutLen > 16)
	{
		return RETURN_UART_FIFO_OVERFLOW;
	}
	
	while(!(xU1_LSR & BIT5));
	for(ib=0 ; ib<u8PutLen ; ib++)
	{
		u8EA = EA;
		EA = 0;
		xU1_THR = pu8DataBuff[ib];
		DPH = 0;
		EA = u8EA;
	}

	return RETURN_UART_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Gets a byte from UART1 H/W FIFO with non-interrupt
///
///	@param	pu8Data	: Pointer to the data to be got.
///	@return	UINT8. Status. 0=No received data, 1=Data is received.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_Uart1Get_Poll(UINT8* pu8Data)
{
	UINT8	u8EA;

	if(xU1_LSR & 0x01)
	{
		u8EA = EA;
		EA = 0;
		*pu8Data = xU1_RBR;
		DPH = 0;
		EA = u8EA;
		return 1;
	}
	else
	{
		return 0;
	}	
}

INT32 OutData(INT32 data1)
{
	if(data1 < 0)
	 {
		data1 = -data1;
		xU1_THR = '-';
		while(!(xU1_LSR & BIT5));
	 }
	 else
		 xU1_THR = ' ';
	 
	 xU1_THR =(unsigned char)(data1/10000)+'0'; data1%=10000;
	while(!(xU1_LSR & BIT5));
	 xU1_THR = (unsigned char)(data1/1000)+'0'; data1%=1000;
	while(!(xU1_LSR & BIT5));
	 xU1_THR = (unsigned char)(data1/100)+'0'; data1%=100;
	while(!(xU1_LSR & BIT5));
	 xU1_THR = (unsigned char)(data1/10)+'0'; data1%=10;
	while(!(xU1_LSR & BIT5));
	 xU1_THR = (unsigned char)(data1)+'0';
	while(!(xU1_LSR & BIT5));
	 xU1_THR = ' ';
	 
	while(!(xU1_LSR & BIT5));
	
	 return 1;
}

#endif	// #ifdef __MG2470_UART1_INCLUDE__


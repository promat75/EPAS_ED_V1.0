
/*******************************************************************************
	[2013-07-24] Compatible with V2.1
*******************************************************************************/


#ifndef __MG2470_UART_H__
#define __MG2470_UART_H__
#define __MG2470_UART0_INCLUDE__
#define __MG2470_UART1_INCLUDE__

#define	UART_CODE						0x0D00

#define	UART_MODE_8N1					0x03	// 8 bits, No Parity, 1 stop bits
#define	UART_MODE_8O1					0x0B	// 8 bits, Odd Parity, 1 stop bits
#define	UART_MODE_8E1					0x1B	// 8 bits, Even Parity, 1 stop bits
#define	UART_MODE_8M1					0x2B	// 8 bits, Mark Parity, 1 stop bits
#define	UART_MODE_8S1					0x3B	// 8 bits, Space Parity, 1 stop bits
                            			
#define	UART_MODE_8N2					0x07	// 8 bits, No Parity, 2 stop bits
#define	UART_MODE_8O2					0x0F	// 8 bits, Odd Parity, 2 stop bits
#define	UART_MODE_8E2					0x1F	// 8 bits, Even Parity, 2 stop bits
#define	UART_MODE_8M2					0x2F	// 8 bits, Mark Parity, 2 stop bits
#define	UART_MODE_8S2					0x3F	// 8 bits, Space Parity, 2 stop bits
                            			
#define	UART_MODE_7N1					0x02	// 7 bits, No Parity, 1 stop bits
#define	UART_MODE_7O1					0x0A	// 7 bits, Odd Parity, 1 stop bits
#define	UART_MODE_7E1					0x1A	// 7 bits, Even Parity, 1 stop bits
#define	UART_MODE_7M1					0x2A	// 7 bits, Mark Parity, 1 stop bits
#define	UART_MODE_7S1					0x3A	// 7 bits, Space Parity, 1 stop bits
                            			
#define	UART_MODE_7N2					0x06	// 7 bits, No Parity, 2 stop bits
#define	UART_MODE_7O2					0x0E	// 7 bits, Odd Parity, 2 stop bits
#define	UART_MODE_7E2					0x1E	// 7 bits, Even Parity, 2 stop bits
#define	UART_MODE_7M2					0x2E	// 7 bits, Mark Parity, 2 stop bits
#define	UART_MODE_7S2					0x3E	// 7 bits, Space Parity, 2 stop bits

#define	RETURN_UART_SUCCESS				0x00
#define	RETURN_UART_CLOCK_OFF			0x10
#define	RETURN_UART_INVALID_PARAMETER	0x11
#define	RETURN_UART_INVALID_BAUDRATE		0x12
#define	RETURN_UART_INTERRUPT_ENABLED		0x13
#define	RETURN_UART_FIFO_OVERFLOW		0x14
#define	RETURN_UART_INTERRUPT_DISABLED	0x15

#ifdef __MG2470_UART0_INCLUDE__
	#define	UART0_TXBUF_SIZE				0x0040	// 64 Byte
	#define	UART0_RXBUF_SIZE				0x0040	// 64 Byte

	#define	UART0_CLK_ON		(PERI_CLK_STP0 |= BIT5)
	#define	UART0_CLK_OFF		(PERI_CLK_STP0 &= ~BIT5)
	#define	UART0_CLK_STATE	(PERI_CLK_STP0 & BIT5)

	extern	UINT8	gau8Uart0Rx_Buff[];	
	extern	UINT16	gu16Uart0Rx_WrIdx;
	extern	UINT16	gu16Uart0Rx_RdIdx;		
	extern	UINT8	gau8Uart0Tx_Buff[];
	extern	UINT16	gu16Uart0Tx_WrIdx;
	extern	UINT16	gu16Uart0Tx_RdIdx;
	extern	UINT8	gu8Uart0Tx_Empty;

	void HAL_Uart0ClockOn(UINT8 u8On);
	UINT8 HAL_Uart0IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT32 u32BaudRate, UINT8 u8Mode);
	UINT16 SYS_Uart0RxLenGet(void);
	UINT16 SYS_Uart0TxLenGet(void);
	void SYS_Uart0Put(UINT8 u8Data);
	UINT8 SYS_Uart0Get(UINT8* pu8Data);
	UINT8 SYS_Uart0TxWorking(void);	
	UINT8 SYS_Uart0Put_Poll(UINT8* pu8DataBuff, UINT8 u8PutLen);
	UINT8 SYS_Uart0Get_Poll(UINT8* pu8Data);
#endif	// #ifdef __MG2470_UART0_INCLUDE__


#ifdef __MG2470_UART1_INCLUDE__
	#define	UART1_TXBUF_SIZE		0x0040	// 64 Byte
	#define	UART1_RXBUF_SIZE		0x0040	// 64 Byte

	#define	UART1_CLK_ON		(PERI_CLK_STP0 |= BIT6)
	#define	UART1_CLK_OFF		(PERI_CLK_STP0 &= ~BIT6)
	#define	UART1_CLK_STATE	(PERI_CLK_STP0 & BIT6)

	extern	UINT8	gau8Uart1Rx_Buff[];	
	extern	UINT16	gu16Uart1Rx_WrIdx;
	extern	UINT16	gu16Uart1Rx_RdIdx;		
	extern	UINT8	gau8Uart1Tx_Buff[];
	extern	UINT16	gu16Uart1Tx_WrIdx;
	extern	UINT16	gu16Uart1Tx_RdIdx;
	extern	UINT8	gu8Uart1Tx_Empty;

	void HAL_Uart1ClockOn(UINT8 u8On);
	UINT8 HAL_Uart1IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT32 u32BaudRate, UINT8 u8Mode);
	UINT16 SYS_Uart1RxLenGet(void);
	UINT16 SYS_Uart1TxLenGet(void);
	void SYS_Uart1Put(UINT8 u8Data);
	UINT8 SYS_Uart1Get(UINT8* pu8Data);
	UINT8 SYS_Uart1TxWorking(void);
	UINT8 SYS_Uart1Put_Poll(UINT8* pu8DataBuff, UINT8 u8PutLen);
	UINT8 SYS_Uart1Get_Poll(UINT8* pu8Data);

    // SB ADD //   
    INT32 OutData(INT32); 	
	
#endif	// #ifdef __MG2470_UART1_INCLUDE__	

#endif	// #ifndef __MG2470_UART_H__

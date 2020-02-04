
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2011.
	- Date		: 2013-01-14
	- Version		: V2.2

	[2013-01-14] V2.2
	- HAL_FlashPageRead() : enhanced // Check-sum added when flash page read done
	- zPrintf() : enhanced // UART interrupt enable/disable check
	- LIB update : 250Kbps, 1Mbps Datarate performance enhanced.
	- UART0_ISR() : Enhanced
	- UART1_ISR() : Enhanced

	[2012-09-06] V2.1a
	- HAL_FlashPageWrite() : enhanced // Check-sum added when flash page write done

	[2012-08-20] V2.1
	- LIB update : RX Mix current increases for RX performance enhancing.
	- Debug message enhanced when CHIPID error.

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#ifndef __OPTION_EVB_H__
#define __OPTION_EVB_H__
	
	//----------------------------------------------	
	#define	INIT_CHANNEL				0x1A		// Chan 26 (2480 MHz)
	#define	INIT_PAN_ID				0x2470		// PAN ID = 0x2470
	#define	INIT_DEV0_SHORTADDR		0x0000		// Short Address of DEVICE0 is 0x0000
	#define	INIT_DEV0_IEEEADDR_LSB	0x00		// IEEE Address of DEVICE0 is 0x0706_0504_0302_0100
	#define	INIT_DEV1_SHORTADDR		0x0001		// Short Address of DEVICE1 is 0x0001
	#define	INIT_DEV1_IEEEADDR_LSB	0x10		// IEEE Address of DEVICE0 is 0x1716_1514_1312_1110
	#define	INIT_DEV2_SHORTADDR		0x0002		// Short Address of DEVICE1 is 0x0002
	#define	INIT_DEV2_IEEEADDR_LSB	0x20		// IEEE Address of DEVICE0 is 0x2726_2524_2322_2120

	// EVK_VERSION = V2.4d
	#define	EVK_VER_MAIN		'2'
	#define	EVK_VER_SUB		'4'
	#define	EVK_VER_PATCH		'd'

	// Library_VERSION = V2.4
	#define	LIB_VER_MAIN		'2'
	#define	LIB_VER_SUB		'4'

	//-	Option for the external amplifier
	// 0	: None
	// 1	: GPIO3_7=TRSW, GPIO3_6=nTRSW
	// 2 : GPIO1_7=TRSW, GPIO1_6=nTRSW
	#define	_OPTION_EXT_AMP_CONTROL		0

	//-- Option for multiple data rate
	// 0	: Disable
	// 1	: Enable
	#define	_OPTION_MULTI_DATA_RATE		0
	//----------------------------------------------

#endif		// __OPTION_EVB_H__



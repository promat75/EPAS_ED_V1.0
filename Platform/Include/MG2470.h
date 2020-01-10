
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#ifndef __MG2470_H__
#define __MG2470_H__


//--------------------------
//	8051 base registers
//--------------------------
sfr 	P0    	= 0x80;		// GPIO Clock is not needed to read. But, if P0_IE is 0x00, the read value is always 0x00.
sfr 	SP    	= 0x81;
sfr 	DPL   	= 0x82;
sfr 	DPH   	= 0x83;
sfr 	PCON  	= 0x87;

sfr 	TCON  	= 0x88;
sfr 	TMOD  	= 0x89;
sfr 	TL0   	= 0x8A;
sfr 	TL1   	= 0x8B;
sfr 	TH0   	= 0x8C;
sfr 	TH1   	= 0x8D;

//-	CLKCON1
//	bit[7:5]	: CLK8M_DIV. Divisor for 8MHz clock.
//	bit[4]	: HSRCOSC_SEL. 0=32MHz crystal. 1=HSRCOSC
//	bit[3]	: CLK16M_SEL. 0=8MHz, 1=16MHz.
//	bit[2:0]	: CLK16M_DIV. Divisor for 16MHz clock.
sfr	CLKCON1 = 0x8E;		// MCU subsystem reference clock on/off

//-	CLKCON2
//	bit[7]	: MCU_INIT_WAIT
//	bit[6]	: HSRCOSC_STS. Clearing to '0' changes the clock source(HSRCOSC --> 32MHz crystal)
//	bit[5:0]	: reserved
sfr	CLKCON2 = 0x8F;		// MCU subsystem reference clock on/off

sfr	P1    	= 0x90;		// GPIO Clock is not needed to read. But, if P1_IE is 0x00, the read value is always 0x00.

//-	EXIF1
//	bit[7]	: T3IF
//	bit[6]	: AESIF
//	bit[5]	: T2IF
//	bit[4]	: PHYIF
//	bit[3]	: ADCIF
//	bit[2:0]	: reserved
sfr	EXIF1	= 0x91;

//-	PERI_CLK_STP0
//	bit[7]	: 1=SPI On			Reset Default = 0
//	bit[6]	: 1=UART1 On			Reset Default = 1
//	bit[5]	: 1=UART0 On			Reset Default = 1
//	bit[4]	: 1=GPIO On			Reset Default = 1
//	bit[3]	: 1=TIMER3 On		Reset Default = 0
//	bit[2]	: 1=TIMER2 On		Reset Default = 0
//	bit[1]	: 1=TIMER1 On		Reset Default = 0
//	bit[0]	: 1=TIMER0 On		Reset Default = 0
sfr	PERI_CLK_STP0 = 0x98;	// Peri Clock On/Off

//-	PERI_CLK_STP1
//	bit[7]	: 1=I2C On			Reset Default = 0
//	bit[6]	: 1=IRTX On			Reset Default = 0
//	bit[5]	: 1=FLASH Controller	Reset Default = 0
//	bit[4]	: 1=VOICE On			Reset Default = 0
//	bit[3]	: 1=I2SRX On			Reset Default = 0
//	bit[2]	: 1=I2STX On			Reset Default = 0
//	bit[1]	: 1=QUAD On			Reset Default = 0
//	bit[0]	: 1=RNG On			Reset Default = 0
sfr	PERI_CLK_STP1 = 0x99;	// Peri Clock On/Off

//-	PERI_CLK_STP2
//	bit[7]	: 1=PWM_CH4 On							Reset Default = 0
//	bit[6]	: 1=PWM_CH3 On							Reset Default = 0
//	bit[5]	: 1=PWM_CH2 On							Reset Default = 0
//	bit[4]	: 1=PWM_CH1 On							Reset Default = 0
//	bit[3]	: 1=PWM_CH0 On							Reset Default = 0
//	bit[2]	: 1=MAC/PHY registers interface block On		Reset Default = 1
//	bit[1]	: 1=LOGIC3V registers interface block On		Reset Default = 1
//	bit[0]	: 1=Clock & Reset controller On				Reset Default = 1
sfr	PERI_CLK_STP2 = 0x9A;	// Peri Clock On/Off

//-	PERI_CLK_STP3
//	bit[7:4]	: reserved
//	bit[3]	: 1=P3_INTCTL On		Reset Default = 0
//	bit[2]	: 1=P1_INTCTL On		Reset Default = 0
//	bit[1]	: 1=P0_INTCTL On		Reset Default = 0
//	bit[0]	: 1=WDT On			Reset Default = 1
sfr	PERI_CLK_STP3 = 0x92;	// Peri Clock On/Off

sfr	P0SRC_SEL = 0x9B;	// P0 Source. 0=MCU, 1=Baseband signal
sfr	P1SRC_SEL = 0x9C;	// P1 Source. 0=MCU, 1=Baseband signal
sfr	P3SRC_SEL = 0x9E;	// P3 Source. 0=MCU, 1=Baseband signal

//-	FBANK
//	bit[7:2]	: reserved
//	bit[1:0]	: Bank Select
sfr 	FBANK	= 0xA1;

//-	AUXR1
//	bit[7:1]	: reserved
//	bit[0]	: DPS. Dual DPTR Select.
sfr	AUXR1	= 0xA2;

//-	IE
//	bit[7]	: EA
//	bit[6]	: ES1
//	bit[5]	: reserved
//	bit[4]	: ES0
//	bit[3]	: ET1
//	bit[2]	: EX1
//	bit[1]	: ET0
//	bit[0]	: EX0
sfr 	IE    	= 0xA8;

//-	T23CON
//	bit[7:5]	: T3 Divider
//	bit[4]	: TR3(Run)
//	bit[3:1]	: T2 Divider
//	bit[0]	: TR2(Run)
sfr	T23CON	= 0xA9;
sfr	TH2		= 0xAA;
sfr	TH3		= 0xAB;
sfr	TL2		= 0xAC;
sfr	TL3		= 0xAD;

sfr 	P3    	= 0xB0;		// GPIO Clock is not needed to read. But, if P3_IE is 0x00, the read value is always 0x00.
sfr	P0OEN	= 0xB1;		// P0 Output Enable. Active Low. Default is 0xFF. GPIO Clock is needed to write.
sfr	P1OEN	= 0xB2;		// P1 Output Enable. Active Low. Default is 0xFF. GPIO Clock is needed to write.
sfr	P3OEN	= 0xB4;		// P3 Output Enable. Active Low. Default is 0xFF. GPIO Clock is needed to write.

//-	IP
//	bit[7]	: reserved
//	bit[6]	: PS1
//	bit[5]	: reserved
//	bit[4]	: PS0
//	bit[3]	: PT1
//	bit[2]	: PX1
//	bit[1]	: PT0
//	bit[0]	: PX0
sfr 	IP    	= 0xB8;

sfr	P0_IE	= 0xB9;		// P0 Input Enable. Active high. Default is 0xFF. GPIO Clock is needed to write.
sfr	P1_IE	= 0xBA;		// P1 Input Enable. Active high. Default is 0xFF. GPIO Clock is needed to write.
sfr	P3_IE	= 0xBC;		// P3 Input Enable. Active high. Default is 0xFF. GPIO Clock is needed to write.

//-	WCON
//	bit[2]	: ISP Mode
//	bit[1]	: ENROM
sfr	WCON	= 0xC0;

sfr	P0_DS	= 0xC1;		// P0 Drive Strength. 0=4mA, 1=8mA. Default is 0x00. GPIO Clock is needed to write.
sfr	P1_DS	= 0xC2;		// P1 Drive Strength. 0=4mA, 1=8mA. Default is 0x00. GPIO Clock is needed to write.
sfr	P3_DS	= 0xC4;		// P2 Drive Strength. 0=4mA, 1=8mA. Default is 0x00. GPIO Clock is needed to write.

//-	PSW
//	bit[7]	: CY
//	bit[6]	: Aux Carry Flag
//	bit[5]	: F0
//	bit[4:3]	: RS(Register Bank)
//	bit[2]	: OV
//	bit[1]	: Flag1. User-defined.
//	bit[0]	: P
sfr 	PSW   	= 0xD0;

//-	WDT
//	bit[7]	: ENB. Timer Enable. Active Low
//	bit[6]	: CLR. Timer Clear. 
//	bit[5]	: SYNCBUSY. 1=WDT register is updating now.
//	bit[3:0]	: DUR. Duration.
sfr	WDT	= 0xD2;

//-	EXIF2
//	bit[7:2]	: reserved
//	bit[1]	: STIF. Sleep Timer. Writing '1' clears the interrupt
//	bit[0]	: WUIF. Wake-up. Writing '1' clears the interrupt
sfr	EXIF2	= 0xD8;

sfr	ACC		= 0xE0;

//-	EIE1
//	bit[7]	: WUIE
//	bit[6]	: DMAIE
//	bit[5]	: SPIIE
//	bit[4]	: STIE
//	bit[3]	: T3IE
//	bit[2]	: AESIE
//	bit[1]	: T2IE
//	bit[0]	: PHYIE
sfr	EIE1	= 0xE8;

//-	EIE2
//	bit[7:4]	: reserved(should be 0)
//	bit[3]	: PWMIE
//	bit[2]	: GPIOIE
//	bit[1]	: I2CIE
//	bit[0]	: IRTXIE
sfr	EIE2	= 0xE9;

//-	P0_POL
//	bit[7]	: Polarity of P0.7 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[6]	: Polarity of P0.6 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[5]	: Polarity of P0.5 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[4]	: Polarity of P0.4 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[3]	: Polarity of P0.3 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[2]	: Polarity of P0.2 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[1]	: Polarity of P0.1 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[0]	: Polarity of P0.0 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
sfr	P0_POL	= 0xEA;

//-	P0_EDGE
//	bit[7]	: Edge/Level Selection of P0.7 Interrupt. 0=Level, 1=Edge.
//	bit[6]	: Edge/Level Selection of P0.6 Interrupt. 0=Level, 1=Edge.
//	bit[5]	: Edge/Level Selection of P0.5 Interrupt. 0=Level, 1=Edge.
//	bit[4]	: Edge/Level Selection of P0.4 Interrupt. 0=Level, 1=Edge.
//	bit[3]	: Edge/Level Selection of P0.3 Interrupt. 0=Level, 1=Edge.
//	bit[2]	: Edge/Level Selection of P0.2 Interrupt. 0=Level, 1=Edge.
//	bit[1]	: Edge/Level Selection of P0.1 Interrupt. 0=Level, 1=Edge.
//	bit[0]	: Edge/Level Selection of P0.0 Interrupt. 0=Level, 1=Edge.
sfr	P0_EDGE = 0xEB;

//-	P0_IRQ_EN
//	bit[7]	: Enable P0.7 Interrupt. 0=Disable, 1=Enable.
//	bit[6]	: Enable P0.6 Interrupt. 0=Disable, 1=Enable.
//	bit[5]	: Enable P0.5 Interrupt. 0=Disable, 1=Enable.
//	bit[4]	: Enable P0.4 Interrupt. 0=Disable, 1=Enable.
//	bit[3]	: Enable P0.3 Interrupt. 0=Disable, 1=Enable.
//	bit[2]	: Enable P0.2 Interrupt. 0=Disable, 1=Enable.
//	bit[1]	: Enable P0.1 Interrupt. 0=Disable, 1=Enable.
//	bit[0]	: Enable P0.0 Interrupt. 0=Disable, 1=Enable.
sfr	P0_IRQ_EN = 0xEC;

//-	P0_IRQ_STS
//	bit[7]	: Status of P0.7 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[6]	: Status of P0.6 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[5]	: Status of P0.5 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[4]	: Status of P0.4 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[3]	: Status of P0.3 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[2]	: Status of P0.2 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[1]	: Status of P0.1 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[0]	: Status of P0.0 Interrupt. 1=Interrupt is pended. Write '1' to clear.
sfr	P0_IRQ_STS = 0xED;

sfr 	B    		= 0xF0;

//-	GPIO_IRQ_PEND
//	bit[7:3]	: reserved
//	bit[2]	: 1=Interrupt by P3.X is pended.
//	bit[1]	: 1=Interrupt by P1.X is pended.
//	bit[0]	: 1=Interrupt by P0.X is pended.
sfr	GPIO_IRQ_PEND = 0xF1;

//-	P1_POL
//	bit[7]	: Polarity of P1.7 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[6]	: Polarity of P1.6 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[5]	: Polarity of P1.5 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[4]	: Polarity of P1.4 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[3]	: Polarity of P1.3 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[2]	: Polarity of P1.2 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[1]	: Polarity of P1.1 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[0]	: Polarity of P1.0 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
sfr	P1_POL	= 0xF2;

//-	P1_EDGE
//	bit[7]	: Edge/Level Selection of P1.7 Interrupt. 0=Level, 1=Edge.
//	bit[6]	: Edge/Level Selection of P1.6 Interrupt. 0=Level, 1=Edge.
//	bit[5]	: Edge/Level Selection of P1.5 Interrupt. 0=Level, 1=Edge.
//	bit[4]	: Edge/Level Selection of P1.4 Interrupt. 0=Level, 1=Edge.
//	bit[3]	: Edge/Level Selection of P1.3 Interrupt. 0=Level, 1=Edge.
//	bit[2]	: Edge/Level Selection of P1.2 Interrupt. 0=Level, 1=Edge.
//	bit[1]	: Edge/Level Selection of P1.1 Interrupt. 0=Level, 1=Edge.
//	bit[0]	: Edge/Level Selection of P1.0 Interrupt. 0=Level, 1=Edge.
sfr	P1_EDGE = 0xF3;

//-	P1_IRQ_EN
//	bit[7]	: Enable P1.7 Interrupt. 0=Disable, 1=Enable.
//	bit[6]	: Enable P1.6 Interrupt. 0=Disable, 1=Enable.
//	bit[5]	: Enable P1.5 Interrupt. 0=Disable, 1=Enable.
//	bit[4]	: Enable P1.4 Interrupt. 0=Disable, 1=Enable.
//	bit[3]	: Enable P1.3 Interrupt. 0=Disable, 1=Enable.
//	bit[2]	: Enable P1.2 Interrupt. 0=Disable, 1=Enable.
//	bit[1]	: Enable P1.1 Interrupt. 0=Disable, 1=Enable.
//	bit[0]	: Enable P1.0 Interrupt. 0=Disable, 1=Enable.
sfr	P1_IRQ_EN = 0xF4;

//-	P1_IRQ_STS
//	bit[7]	: Status of P1.7 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[6]	: Status of P1.6 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[5]	: Status of P1.5 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[4]	: Status of P1.4 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[3]	: Status of P1.3 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[2]	: Status of P1.2 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[1]	: Status of P1.1 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[0]	: Status of P1.0 Interrupt. 1=Interrupt is pended. Write '1' to clear.
sfr	P1_IRQ_STS = 0xF5;

//-	EIP1
//	bit[7]	: reserved
//	bit[6]	: DMAIP
//	bit[5]	: SPIIP
//	bit[4]	: RTCIP
//	bit[3]	: T3IP
//	bit[2]	: AESIP
//	bit[1]	: T2IP
//	bit[0]	: PHYIP
sfr	EIP1	= 0xF8;

//-	EIP2
//	bit[7:4]	: reserved
//	bit[3]	: PWMIP
//	bit[2]	: GPIOIP
//	bit[1]	: I2CIP
//	bit[0]	: IRTXIP
sfr	EIP2	= 0xF9;

//-	P3_POL
//	bit[7]	: Polarity of P3.7 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[6]	: Polarity of P3.6 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[5]	: Polarity of P3.5 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[4]	: Polarity of P3.4 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[3:2]	: reserved
//	bit[1]	: Polarity of P3.1 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
//	bit[0]	: Polarity of P3.0 Interrupt. 0=Low Level/Falling Edge, 1=High Level/Rising Edge.
sfr	P3_POL	= 0xFA;

//-	P3_EDGE
//	bit[7]	: Edge/Level Selection of P3.7 Interrupt. 0=Level, 1=Edge.
//	bit[6]	: Edge/Level Selection of P3.6 Interrupt. 0=Level, 1=Edge.
//	bit[5]	: Edge/Level Selection of P3.5 Interrupt. 0=Level, 1=Edge.
//	bit[4]	: Edge/Level Selection of P3.4 Interrupt. 0=Level, 1=Edge.
//	bit[3:2]	: reserved
//	bit[1]	: Edge/Level Selection of P3.1 Interrupt. 0=Level, 1=Edge.
//	bit[0]	: Edge/Level Selection of P3.0 Interrupt. 0=Level, 1=Edge.
sfr	P3_EDGE = 0xFB;

//-	P3_IRQ_EN
//	bit[7]	: Enable P3.7 Interrupt. 0=Disable, 1=Enable.
//	bit[6]	: Enable P3.6 Interrupt. 0=Disable, 1=Enable.
//	bit[5]	: Enable P3.5 Interrupt. 0=Disable, 1=Enable.
//	bit[4]	: Enable P3.4 Interrupt. 0=Disable, 1=Enable.
//	bit[3:2]	: reserved
//	bit[1]	: Enable P3.1 Interrupt. 0=Disable, 1=Enable.
//	bit[0]	: Enable P3.0 Interrupt. 0=Disable, 1=Enable.
sfr	P3_IRQ_EN = 0xFC;

//-	P3_IRQ_STS
//	bit[7]	: Status of P3.7 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[6]	: Status of P3.6 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[5]	: Status of P3.5 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[4]	: Status of P3.4 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[3:2]	: reserved
//	bit[1]	: Status of P3.1 Interrupt. 1=Interrupt is pended. Write '1' to clear.
//	bit[0]	: Status of P3.0 Interrupt. 1=Interrupt is pended. Write '1' to clear.
sfr	P3_IRQ_STS = 0xFD;

sfr	CODE_GPIO = 0xFF;		// Instructioin Fetch from P0 or P3

//--------------------------
// bit accessable sfr
//--------------------------

// EIP1 - Extended Interrupt Priority
//			= EIP1^7;
sbit	DMAIP	= EIP1^6;
sbit	SPIIP	= EIP1^5;
sbit	RTCIP	= EIP1^4;			// realtime clock
sbit	T3IP	= EIP1^3;
sbit	AESIP	= EIP1^2;
sbit	T2IP	= EIP1^1;
sbit	PHYIP	= EIP1^0;

// B

// EIE1 - Extended Interrupt Enable
sbit	WUIE	= EIE1^7;			// wake-up interrupt
sbit	DMAIE	= EIE1^6;
sbit	SPIIE	= EIE1^5;
sbit	STIE	= EIE1^4;			// sleep timer interrupt
sbit	T3IE	= EIE1^3;			// timer3 interrupt
sbit	AESIE	= EIE1^2;			// AES interrupt
sbit	T2IE	= EIE1^1;			// timer2 interrupt
sbit	PHYIE	= EIE1^0;			// RF transmit/receive interrupt

// ACC

// EXIF2
sbit	STIF	= EXIF2^1;			// sleep timer. Writing '1' clears the interrupt.
sbit	WUIF	= EXIF2^0;			// wake-up. Writing '1' clears the interrupt.

// PSW
sbit 	CY    	= PSW^7;
sbit 	AC    	= PSW^6;
sbit 	F0    	= PSW^5;
sbit 	RS1   	= PSW^4;
sbit 	RS0   	= PSW^3;
sbit	OV    	= PSW^2;
sbit	F1    	= PSW^1;
sbit	PF 	 	= PSW^0;		// Parity Flag

// WCON
//			= WCON^7;
//			= WCON^6;
//			= WCON^5;
//			= WCON^4;
//			= WCON^3;
sbit	ISPMODE= WCON^2;
sbit	ENROM	= WCON^1;
//sbit	CW		= WCON^0;

// IP
//			= IP^7;
sbit	PS1		= IP^6;
//			= IP^5;
sbit 	PS0    	= IP^4;
sbit 	PT1   	= IP^3;
sbit 	PX1   	= IP^2;
sbit 	PT0   	= IP^1;
sbit 	PX0   	= IP^0;

// P3
sbit	P3_7	= P3^7;		// SPICSN
sbit	P3_6	= P3^6;		// SPICLK
sbit	P3_5	= P3^5;		// SPIDO
sbit	P3_4	= P3^4;		// SPIDI
sbit	P3_3	= P3^3;		// INT1
sbit	P3_2	= P3^2;		// INT0
sbit	P3_1	= P3^1;		// TXD0
sbit	P3_0	= P3^0;		// RXD0

// IE
sbit 	EA    	= IE^7;		// interrupt enable
sbit	ES1		= IE^6;		// serial1 interrupt
//			= IE^5;
sbit 	ES0    	= IE^4;		// serial0 interrupt
sbit 	ET1   	= IE^3;		// timer1 interrupt
sbit 	EX1   	= IE^2;		// ext1 interrupt
sbit 	ET0   	= IE^1;		// timer0 interrupt
sbit 	EX0   	= IE^0;		// ext0 interrupt

// P1
sbit	P1_7	= P1^7;
sbit	P1_6	= P1^6;
sbit	P1_5	= P1^5;
sbit	P1_4	= P1^4;
sbit	P1_3	= P1^3;
sbit	P1_2	= P1^2;
sbit	P1_1	= P1^1;		// TXD1
sbit	P1_0	= P1^0;		// RXD1

// TCON
sbit 	TF1   	= TCON^7;
sbit 	TR1   	= TCON^6;
sbit 	TF0   	= TCON^5;
sbit	TR0   	= TCON^4;
sbit 	IE1   	= TCON^3;
sbit	IT1   	= TCON^2;
sbit 	IE0   	= TCON^1;
sbit 	IT0   	= TCON^0;

// P0
sbit	P0_7	= P0^7;
sbit	P0_6	= P0^6;
sbit	P0_5	= P0^5;
sbit	P0_4	= P0^4;
sbit	P0_3	= P0^3;
sbit	P0_2	= P0^2;
sbit	P0_1	= P0^1;
sbit	P0_0	= P0^0;

//----------------------------------------------------
// MAC TX FIFO Control (When xSECMAP.secmap=0)
//----------------------------------------------------
#define	xMTFCPUSH	XBYTE[0x2000]
#define	xMTFCWP	XBYTE[0x2001]
#define	xMTFCRP	XBYTE[0x2002]
//-	xMTFCSTA
// bit[7] 		: MTFCWP[8]
// bit[6] 		: MTFCRP[8]
// bit[5:2] 	: reserved
// bit[1] 		: Full. Read only.
// bit[0] 		: Empty. Read only.
#define	xMTFCSTA	XBYTE[0x2004]
#define	xMTFCSIZE	XBYTE[0x2005]
//----------------------------------------------------
// MAC RX FIFO Control (When xSECMAP.secmap=0)
//----------------------------------------------------
#define	xMRFCPOP	XBYTE[0x2080]
#define	xMRFCWP	XBYTE[0x2081]
#define	xMRFCRP	XBYTE[0x2082]
//-	xMRFCSTA
// bit[7] 		: MRFCWP[8]
// bit[6] 		: MRFCRP[8]
// bit[5:2] 	: reserved
// bit[1] 		: Full. Read only.
// bit[0] 		: Empty. Read only.
#define	xMRFCSTA	XBYTE[0x2084]
#define	xMRFCSIZE	XBYTE[0x2085]
//----------------------------------------------------
// SEC TX FIFO Control (When xSECMAP.secmap=1)
//----------------------------------------------------
//-	xSTFCCTL
// bit[7:3]	: Status. Read Only.
// bit[2]		: ENC. Active High. Read/Write.
// bit[1]		: Enable. Active High. Read/Write.
// bit[0] 		: Clear. Active High. Read/Write.
#define	xSTFCCTL		XBYTE[0x2003]
#define	xSTFCSECBASE	XBYTE[0x2007]
#define	xSTFCSECLEN	XBYTE[0x2008]
//----------------------------------------------------
// SEC RX FIFO Control (When xSECMAP.secmap=1)
//----------------------------------------------------
//-	xSRFCCTL
// bit[7:3]	: Status. Read Only.
// bit[2]		: DEC. Active High. Read/Write.
// bit[1]		: Enable. Active High. Read/Write.
// bit[0] 		: Clear. Active High. Read/Write.
#define	xSRFCCTL		XBYTE[0x2083]
#define	xSRFCSECBASE	XBYTE[0x2087]
#define	xSRFCSECLEN	XBYTE[0x2088]
//----------------------------------------------------
// MAC Control : 0x2100 ~ 0x2195
//----------------------------------------------------
#define 	xKEY0(n)			XBYTE[0x2100+n]	// MSB = 0x210F
#define 	xRXNONCE(n)		XBYTE[0x2110+n]	// MSB = 0x211C
#define 	xKEY1(n)			XBYTE[0x2130+n]	// MSB = 0x213F
#define 	xTXNONCE(n)		XBYTE[0x2140+n]	// MSB = 0x214C
#define 	xEXTADDR(n)		XBYTE[0x2150+n]	// MSB = 0x2157
#define 	xPANID(n)			XBYTE[0x2158+n]	// MSB = 0x2159
#define 	xSHORTADDR(n)		XBYTE[0x215A+n]	// MSB = 0x215B

//-----------------------------
// MAC STATUS
//-----------------------------
// bit[7] : enc/dec			: default= 0
// bit[6] : tx_busy			: default= 0
// bit[5] : rx_busy			: default= 0
// bit[4] : reserved		: default= 0
// bit[3] : decode_ok		: default= 1
// bit[2] ; enc_done		: default= 0
// bit[1] : dec_done		: default= 0
// bit[0] : crc_ok			: default= 1
#define 	xMACSTS		XBYTE[0x2180]
//-----------------------------

//-----------------------------
// MAC MAIN
//-----------------------------
// bit[7] : rst_fifo
// bit[6] : rst_tsm
// bit[5] : rst_rsm
// bit[4] : rst_aes
// bit[3:0] : reserved
#define 	xMACRST		XBYTE[0x2190]
//-----------------------------

//-----------------------------
// MAC MODEM
//-----------------------------
// bit[7:5] : reserved			: default=0
// bit[4] : prevent_ack_packet	: default=0	//	Look up xMACDSN register
// bit[3] : pan_coordinator		: default=0
// bit[2] ; addr_decode			: default=1
// bit[1] : auto_crc				: default=1
// bit[0] : reserved.			: default=0
#define 	xMACCTRL		XBYTE[0x2191]
//-----------------------------
#define 	xMACDSN		XBYTE[0x2192]

//-----------------------------
// MAC SEC
//-----------------------------
// bit[7] : sa_keysel
// bit[6] : tx_keysel
// bit[5] : rx_keysel
// bit[4:2] : sec_m[2:0]
// bit[1:0] : sec_mode[1:0].		0=None, 1=CBC-MAC, 2=CTR, 3=CCM
#define 	xMACSEC		XBYTE[0x2193]
//-----------------------------
#define 	xTXL			XBYTE[0x2194]		// bit[7] : reserved
#define 	xRXL			XBYTE[0x2195]		// bit[7] : reserved

//-----------------------------
// SEC MAP
//-----------------------------
// bit[7:1]	: reserved
// bit[0]		: secmap. 1=SEC FIFO access.
#define	xSECMAP		XBYTE[0x219F]

//----------------------------------------------------
// MAC TX FIFO : 0x2300 ~ 0x23FF (When xSECMAP.secmap=1)
//----------------------------------------------------
#define	xMTxFIFO(n)			XBYTE[0x2300+n]
//----------------------------------------------------
// MAC RX FIFO : 0x2400 ~ 0x24FF (When xSECMAP.secmap=1)
//----------------------------------------------------
#define	xMRxFIFO(n)			XBYTE[0x2400+n]
//----------------------------------------------------
// MAC TX FIFO : 0x2300 ~ 0x23FF (When xSECMAP.secmap=0)
//----------------------------------------------------
#define	xSTxFIFO(n)			XBYTE[0x2300+n]
//----------------------------------------------------
// MAC RX FIFO : 0x2400 ~ 0x24FF (When xSECMAP.secmap=0)
//----------------------------------------------------
#define	xSRxFIFO(n)			XBYTE[0x2400+n]

//----------------------------------------------------
// UART0
//----------------------------------------------------
#define	xU0_RBR	XBYTE[0x2500]	// When LCR[7]=0 && Reading
#define	xU0_THR	XBYTE[0x2500]	// When LCR[7]=0 && Writing
#define	xU0_DLL	XBYTE[0x2500]	// When LCR[7]=1
//-	xU0_IER
//	bit[7:4]	: reserved. Should be 0.
//	bit[3]	: Modem Status Interrupt. 		0=Disable, 1=Enable.
//	bit[2]	: Receive Line Status Interrupt. 	0=Disable, 1=Enable.
//	bit[1]	: Transmit Holding Interrupt. 	0=Disable, 1=Enable.
//	bit[0]	: Receive Data Interrupt. 		0=Disable, 1=Enable.
#define	xU0_IER		XBYTE[0x2501]	// When LCR[7]=0
#define	xU0_DLM	XBYTE[0x2501]	// When LCR[7]=1
#define	xU0_IIR		XBYTE[0x2502]	// When ECR[2:1]=2'b00 and Reading
#define	xU0_FCR	XBYTE[0x2502]	// When ECR[2:1]=2'b00 and writing
#define	xU0_LCR	XBYTE[0x2503]	// When ECR[2:1]=2'b00
#define	xU0_TLC	XBYTE[0x2503]	// When ECR[2:0]=3'b101
#define	xU0_RLC	XBYTE[0x2504]	// When ECR[2:0]=3'b101
#define	xU0_RIL		XBYTE[0x2504]	// When ECR[2:1]=2'b01
#define	xU0_LSR	XBYTE[0x2505]	// Reading
#define	xU0_ECR	XBYTE[0x2505]	// Writing
#define	xU0_XCR	XBYTE[0x2507]	// When LCR[7]=1
//----------------------------------------------------
// UART1
//----------------------------------------------------
#define	xU1_RBR	XBYTE[0x2510]	// When LCR[7]=0 && Reading
#define	xU1_THR	XBYTE[0x2510]	// When LCR[7]=0 && Writing
#define	xU1_DLL	XBYTE[0x2510]	// When LCR[7]=1
//-	xU1_IER
//	bit[7:4]	: reserved. Should be 0.
//	bit[3]	: Modem Status Interrupt. 		0=Disable, 1=Enable.
//	bit[2]	: Receive Line Status Interrupt. 	0=Disable, 1=Enable.
//	bit[1]	: Transmit Holding Interrupt. 	0=Disable, 1=Enable.
//	bit[0]	: Receive Data Interrupt. 		0=Disable, 1=Enable.
#define	xU1_IER		XBYTE[0x2511]	// When LCR[7]=0
#define	xU1_DLM	XBYTE[0x2511]	// When LCR[7]=1
#define	xU1_IIR		XBYTE[0x2512]	// When ECR[2:1]=2'b00 and Reading
#define	xU1_FCR	XBYTE[0x2512]	// When ECR[2:1]=2'b00 and writing
#define	xU1_LCR	XBYTE[0x2513]	// When ECR[2:1]=2'b00
#define	xU1_TLC	XBYTE[0x2513]	// When ECR[2:0]=3'b101
#define	xU1_RLC	XBYTE[0x2514]	// When ECR[2:0]=3'b101
#define	xU1_RIL		XBYTE[0x2514]	// When ECR[2:1]=2'b01
#define	xU1_LSR	XBYTE[0x2515]	// Reading
#define	xU1_ECR	XBYTE[0x2515]	// Writing
#define	xU1_XCR	XBYTE[0x2517]	// When LCR[7]=1
//----------------------------------------------------
// I2STX
//----------------------------------------------------
#define	xSTXAIC		XBYTE[0x2528]
#define	xSTXSDIV	XBYTE[0x252A]
#define	xSTXMDIV	XBYTE[0x252B]
#define	xSTXBDIV	XBYTE[0x252C]
#define	xSTXMODE	XBYTE[0x252D]
//----------------------------------------------------
// I2SRX
//----------------------------------------------------
#define	xSRXAIC		XBYTE[0x2538]
#define	xSRXSDIV	XBYTE[0x253A]
#define	xSRXMDIV	XBYTE[0x253B]
#define	xSRXBDIV	XBYTE[0x253C]
#define	xSRXMODE	XBYTE[0x253D]
//----------------------------------------------------
// SPI
//----------------------------------------------------
//-	SPCR
//	bit[7]	: R/W.	SPIE. Interrupt Enable.
//	bit[6]	: R/W.	SPE. Peripheral Enable.
//	bit[5]	: reserved.
//	bit[4]	: R/W.	MSTR. 1=Master, 0=Slave.
//	bit[3]	: R/W.	CPOL. Clock Polarity.
//	bit[2]	: R/W.	CPHA. Clock Phase.
//	bit[1:0]	: R/W.	SPR. SPI Clock Rate Select.
#define	xSPCR		XBYTE[0x2540]
//-	SPSR
//	bit[7]	: R/W. 	Interrupt Flag. To clear the flag, write '0'.
//	bit[6]	: R/W.	Write Collision. 1=If SPDR is written while Write FIFO is full. To clear the flag, write '0'.
//	bit[5:4]	: reserved
//	bit[3]	: R/O.	Write FIFO Full. 	1=Full.
//	bit[2]	: R/O.	Write FIFO Empty. 	1=Empty.
//	bit[1]	: R/O.	Read FIFO Full. 	1=Full
//	bit[0]	: R/O.	Read FIFO Empty. 	1=Empty.
#define	xSPSR		XBYTE[0x2541]	// When SPER.FSEL=0
#define	xSPDR		XBYTE[0x2542]
//-	SPER
//	bit[7:6]	: Interrupt Count
//	bit[5:3]	: reserved
//	bit[2]	: FSEL
//	bit[1:0]	: ESPR. Extended SPI Clock Rate Select
#define	xSPER		XBYTE[0x2543]
//----------------------------------------------------
// RNG
//----------------------------------------------------
#define	xRNGD3		XBYTE[0x2550]
#define	xRNGD2		XBYTE[0x2551]
#define	xRNGD1		XBYTE[0x2552]
#define	xRNGD0		XBYTE[0x2553]
#define	xSEED3		XBYTE[0x2554]
#define	xSEED2		XBYTE[0x2555]
#define	xSEED1		XBYTE[0x2556]
#define	xSEED0		XBYTE[0x2557]
#define	xRNGC		XBYTE[0x2558]
//----------------------------------------------------
// QUAD Decoder
//----------------------------------------------------
#define	xUDX		XBYTE[0x2560]
#define	xCNTX		XBYTE[0x2561]
#define	xUDY		XBYTE[0x2562]
#define	xCNTY		XBYTE[0x2563]
#define	xUDZ		XBYTE[0x2564]
#define	xCNTZ		XBYTE[0x2565]
#define	xQCTL		XBYTE[0x2566]
//----------------------------------------------------
// PWMX_INTR
//----------------------------------------------------
//-	xPWMX_INTR
//	bit[7:5]	: reserved
//	bit[4]	: PWM CH4 Interrupt Flag
//	bit[3]	: PWM CH3 Interrupt Flag
//	bit[2]	: PWM CH2 Interrupt Flag
//	bit[1]	: PWM CH1 Interrupt Flag
//	bit[0]	: PWM CH0 Interrupt Flag
#define	xPWMX_INTR	XBYTE[0x257F]
//----------------------------------------------------
// PWM0
//----------------------------------------------------
#define	xPWM0_CNTRH	XBYTE[0x2580]
#define	xPWM0_CNTRL	XBYTE[0x2581]
#define	xPWM0_HRCH	XBYTE[0x2582]
#define	xPWM0_HRCL	XBYTE[0x2583]
#define	xPWM0_LRCH	XBYTE[0x2584]
#define	xPWM0_LRCL	XBYTE[0x2585]
//-	xPWM0_CTRL
//	bit[7]	: Interrupt Enable
//	bit[6]	: Capture Enable
//	bit[5]	: CNTR Reset. Not auto-cleared.
//	bit[4]	: 0=Continuous Mode, 1=Single Mode.
//	bit[3]	: Output Enable.
//	bit[2]	: 0=Increment on Positivie Edge, 1=Increment on Negative Edge.
//	bit[1]	: Gate Enable.
//	bit[0]	: CNTR Increment Enable.
#define	xPWM0_CTRL	XBYTE[0x2586]
//----------------------------------------------------
// PWM1
//----------------------------------------------------
#define	xPWM1_CNTRH	XBYTE[0x2588]
#define	xPWM1_CNTRL	XBYTE[0x2589]
#define	xPWM1_HRCH	XBYTE[0x258A]
#define	xPWM1_HRCL	XBYTE[0x258B]
#define	xPWM1_LRCH	XBYTE[0x258C]
#define	xPWM1_LRCL	XBYTE[0x258D]
#define	xPWM1_CTRL	XBYTE[0x258E]
//----------------------------------------------------
// PWM2
//----------------------------------------------------
#define	xPWM2_CNTRH	XBYTE[0x2590]
#define	xPWM2_CNTRL	XBYTE[0x2591]
#define	xPWM2_HRCH	XBYTE[0x2592]
#define	xPWM2_HRCL	XBYTE[0x2593]
#define	xPWM2_LRCH	XBYTE[0x2594]
#define	xPWM2_LRCL	XBYTE[0x2595]
#define	xPWM2_CTRL	XBYTE[0x2596]
//----------------------------------------------------
// PWM3
//----------------------------------------------------
#define	xPWM3_CNTRH	XBYTE[0x2598]
#define	xPWM3_CNTRL	XBYTE[0x2599]
#define	xPWM3_HRCH	XBYTE[0x259A]
#define	xPWM3_HRCL	XBYTE[0x259B]
#define	xPWM3_LRCH	XBYTE[0x259C]
#define	xPWM3_LRCL	XBYTE[0x259D]
#define	xPWM3_CTRL	XBYTE[0x259E]
//----------------------------------------------------
// PWM4
//----------------------------------------------------
#define	xPWM4_CNTRH	XBYTE[0x25A0]
#define	xPWM4_CNTRL	XBYTE[0x25A1]
#define	xPWM4_HRCH	XBYTE[0x25A2]
#define	xPWM4_HRCL	XBYTE[0x25A3]
#define	xPWM4_LRCH	XBYTE[0x25A4]
#define	xPWM4_LRCL	XBYTE[0x25A5]
#define	xPWM4_CTRL	XBYTE[0x25A6]
//----------------------------------------------------
// FLASH Control
//----------------------------------------------------
#define	xFCN_CMD		XBYTE[0x2700]
#define	xFCN_ADR1		XBYTE[0x2702]
#define	xFCN_ADR0		XBYTE[0x2703]
#define	xFCN_DAT3		XBYTE[0x2704]
#define	xFCN_DAT2		XBYTE[0x2705]
#define	xFCN_DAT1		XBYTE[0x2706]
#define	xFCN_DAT0		XBYTE[0x2707]
#define	xFCN_LEN1		XBYTE[0x2708]
#define	xFCN_LEN0		XBYTE[0x2709]
#define	xFCN_STS1		XBYTE[0x270C]
#define	xFCN_STS0		XBYTE[0x270D]
#define	xFCN_NOWAIT	XBYTE[0x270E]
//----------------------------------------------------
// CODEC Control
//----------------------------------------------------
#define  xENCMUT1		XBYTE[0x2740]
#define  xENCMUT0		XBYTE[0x2741]
#define  xENCPCM1		XBYTE[0x2742]
#define  xENCPCM0		XBYTE[0x2743]
#define  xENCCDC			XBYTE[0x2744]
#define  xENCCTL			XBYTE[0x2745]
#define  xDECMUT1		XBYTE[0x2748]
#define  xDECMUT0		XBYTE[0x2749]
#define  xDECPCM1		XBYTE[0x274A]
#define  xDECPCM0		XBYTE[0x274B]
#define  xDECCDC			XBYTE[0x274C]
#define  xDECCTL			XBYTE[0x274D]

#define  VCE_BASE                    0x2740

#define	xENC_IMA_PRED1	XBYTE[VCE_BASE+0x00]
#define	xENC_IMA_PRED0	XBYTE[VCE_BASE+0x01]
#define	xENC_IMA_INDEX	XBYTE[VCE_BASE+0x03]
#define	xDEC_IMA_PRED1	XBYTE[VCE_BASE+0x08]
#define	xDEC_IMA_PRED0	XBYTE[VCE_BASE+0x09]
#define	xDEC_IMA_INDEX	XBYTE[VCE_BASE+0x0B]
#define	xENC_IMA_PRED		XWORD[(VCE_BASE+0x00)>>1]
#define	xDEC_IMA_PRED		XWORD[(VCE_BASE+0x08)>>1]
//----------------------------------------------------
// VOICE TX FIFO Control (From I2S to MAC TX FIFO)
//----------------------------------------------------
#define xVTF_DAT		XBYTE[0x2750]
#define xVTF_MUT		XBYTE[0x2751]
#define xVTF_CTL			XBYTE[0x2752]
#define xVTF_RP			XBYTE[0x2753]
#define xVTF_WP			XBYTE[0x2754]
#define xVTF_SIZE		XBYTE[0x2755]
#define xVTF_HTHR		XBYTE[0x2756]
#define xVTF_LTHR		XBYTE[0x2757]
#define xVTF_ROOM		XBYTE[0x2758]
#define xVTF_STS		XBYTE[0x275A]
#define xVTD_SIZE		XBYTE[0x275B]
#define xNEVOS			XBYTE[0x275C]
#define xNEVNGT1		XBYTE[0x275D]
#define xNEVNGT0		XBYTE[0x275E]
#define xNEVVOL			XBYTE[0x275F]
//----------------------------------------------------
// VOICE RX FIFO Control (From I2S to MAC TX FIFO)
//----------------------------------------------------
#define xVRF_DAT		XBYTE[0x2760]
#define xVRF_MUT		XBYTE[0x2761]
#define xVRF_CTL			XBYTE[0x2762]
#define xVRF_RP			XBYTE[0x2763]
#define xVRF_WP			XBYTE[0x2764]
#define xVRF_SIZE		XBYTE[0x2765]
#define xVRF_HTHR		XBYTE[0x2766]
#define xVRF_LTHR		XBYTE[0x2767]
#define xVRF_ROOM		XBYTE[0x2768]
#define xVRF_STS		XBYTE[0x276A]
#define xVRD_SIZE		XBYTE[0x276B]
#define  xFEVOS			XBYTE[0x276C]
#define  xFEVNGT1		XBYTE[0x276D]
#define  xFEVNGT0		XBYTE[0x276E]
#define  xFEVVOL			XBYTE[0x276F]
//----------------------------------------------------
// VOICE IF
//----------------------------------------------------
#define  xVTFINTENA		XBYTE[0x2770]
#define  xVRFINTENA		XBYTE[0x2771]
#define  xVDMINTENA		XBYTE[0x2772]
#define  xVTFINTSRC		XBYTE[0x2773]
#define  xVRFINTSRC		XBYTE[0x2774]
#define  xVDMINTSRC		XBYTE[0x2775]
#define  xVTFINTVAL		XBYTE[0x2776]
#define  xVRFINTVAL		XBYTE[0x2777]
#define  xVDMINTVAL		XBYTE[0x2778]
#define  xVCECFG			XBYTE[0x2779]
#define  xSRCCTL			XBYTE[0x277A]
#define  xVSPMUT1		XBYTE[0x277C]
#define  xVSPMUT0		XBYTE[0x277D]
#define  xVSPCTL			XBYTE[0x277E]
#define  xVSPMUX			XBYTE[0x277F]
//----------------------------------------------------
// I2C
//----------------------------------------------------
#define 	xI2C_DATA		XBYTE[0x2790]
#define 	xI2C_ADDR		XBYTE[0x2791]
//-	xI2C_CTR - Control Register
//	bit[7]	: Core Enable. 	0=Enable 	1=Disable 	Reset Value=1
//	bit[6]	: Interrupt Enable. 	0=Disable 	1=Enable 	Reset Value=0
//	bit[5]	: Master.			0=Slave	 	1=Master		Reset Value=0
//	bit[4]	: START/STOP.	0->1=START,  1->0=STOP	Reset Value=0
//	bit[3]	: Repeated START.							Reset Value=0
//	bit[2]	: NACK.									Reset Value=0
//	bit[1]	: RXFIFO Reset. auto-cleared				Reset Value=0
//	bit[0]	: TXFIFO Reset. auto-cleared				Reset Value=0
#define 	xI2C_CTR			XBYTE[0x2792]
//-	xI2C_STR - Status Register
//	bit[7]	: Addressed flag. 		R/O. Cleared@Read
//	bit[6]	: Master Nacked flag. 	R/O. Cleared@Read
//	bit[5]	: Slave Nacked flag.	R/O. Cleared@Read
//	bit[4]	: FIFO INT flag.		R/O. 
//	bit[3]	: Bus Busy flag.		R/O. Cleared@Read
//	bit[2]	: Byte Transfered flag.	R/O. Cleared@Read. 1=Completed.
//	bit[1]	: reserved
//	bit[0]	: Time-out.			R/O.
#define 	xI2C_STR			XBYTE[0x2793]
#define 	xI2C_PRER		XBYTE[0x2794]
#define 	xI2C_HOLD		XBYTE[0x2795]
#define 	xI2C_TO			XBYTE[0x2796]
#define 	xI2C_RXLVL		XBYTE[0x2797]
#define 	xI2C_RXCNT		XBYTE[0x2798]
#define 	xI2C_RXSTS		XBYTE[0x2799]
#define 	xI2C_TXLVL		XBYTE[0x279A]
#define 	xI2C_TXCNT		XBYTE[0x279B]
#define 	xI2C_TXSTS		XBYTE[0x279C]
//-	xI2C_FINTMSK - FIFO INT Masking
//	bit[7:4]	: reserved
//	bit[3]	: 1=TX FIFO Level Interrupt is enabled
//	bit[2]	: 1=TX FIFO Empty Interrupt is enabled
//	bit[1]	: 1=RX FIFO Level Interrupt is enabled
//	bit[0]	: 1=RX FIFO Data Available Interrupt is enabled
#define 	xI2C_FINTMSK		XBYTE[0x279D]
//-	xI2C_FINTVAL - FIFO INT Value
//	bit[7:4]	: reserved
//	bit[3]	: 1=TX FIFO Level Interrupt Flag.			R/O. Cleared@Read
//	bit[2]	: 1=TX FIFO Empty Interrupt Flag.		R/O. Cleared@Read
//	bit[1]	: 1=RX FIFO Level Interrupt Flag.			R/O. Cleared@Read
//	bit[0]	: 1=RX FIFO Data Available Interrupt Flag.	R/O. Cleared@Read
#define 	xI2C_FINTVAL		XBYTE[0x279E]
//-	xI2C_IMSK - INT Mask
//	bit[7]	: 1=Addressed Interrupt is enabled
//	bit[6]	: 1=Master Nacked Interrupt is enabled
//	bit[5]	: 1=Slave Nacked Interupt is enabled
//	bit[4]	: 1=FIFO Level Interrupt is enabled
//	bit[3]	: 1=Bus Busy Interrupt is enabled
//	bit[2]	: 1=Byte Transfer Interrupt is enabled
//	bit[1]	: reserved
//	bit[0]	: 1=Time-out Interrupt is enabled
#define 	xI2C_IMSK		XBYTE[0x279F]
//----------------------------------------------------
// IR Modulator
//----------------------------------------------------
#define	xLCODE			XBYTE[0x27A0]
#define	xPPM_SCODE		XBYTE[0x27A1]
#define	xPPM_CCODE		XBYTE[0x27A2]
#define	xPPM_CCODB		XBYTE[0x27A3]
#define	xPPM_DCODE		XBYTE[0x27A4]
#define	xPPM_DCODB		XBYTE[0x27A5]
#define	xPPM_CTL		XBYTE[0x27A6]
#define	xPPM_POSST		XBYTE[0x27A7]
#define	xPPM_POSSP		XBYTE[0x27A8]
#define	xPPM_TCCNT0		XBYTE[0x27A9]
#define	xPPM_TCCNT1		XBYTE[0x27AA]
#define	xPPM_HCCNT0		XBYTE[0x27AB]
#define	xPPM_HCCNT1		XBYTE[0x27AC]
#define	xPPM_T0CNT		XBYTE[0x27AD]
#define	xPPM_T1CNT		XBYTE[0x27AE]
#define	xPPM_H0CNT		XBYTE[0x27AF]
#define	xPPM_H1CNT		XBYTE[0x27B0]
#define	xPPM_CDIV0		XBYTE[0x27B1]
#define	xPPM_CDIV1		XBYTE[0x27B2]
//----------------------------------------------------
// Voice TX/RX FIFO
//----------------------------------------------------
#define 	xVTxFIFO(n)		XBYTE[0x2800+n]		// 0x2800 ~ 0x28FF
#define 	xVRxFIFO(n)		XBYTE[0x2900+n]		// 0x2900 ~ 0x29FF





//----------------------------------------------------
//	PHY/RF/Analog Register Set
//----------------------------------------------------
#define	PHY_BASE	0x2200
//----------------------------------------------------
#define	xPCMD0			XBYTE[PHY_BASE + 0x00]
#define	xPCMD1			XBYTE[PHY_BASE + 0x01]
#define	xPLLPU			XBYTE[PHY_BASE + 0x03]

//---------------------------------------------------------------
#define	xSETRATE		XBYTE[PHY_BASE + 0x11]
//---------------------------------------------------------------
//-	xCORCNF4
//	bit[7:6]	: Auto Detect. 0=Off, 1=High Rate On, 2=Low Rate On, 3=All Rate On
#define	xCORCNF4		XBYTE[PHY_BASE + 0x24]
#define	xCORCNF6		XBYTE[PHY_BASE + 0x26]
//-	xCCA0
//	bit[7]	: reserved
//	bit[6]	: AACK_CCA. 0=Disable, 1=Enable.
//	bit[5]	: CCA. 0=CCA is enabled, 1=CCA is disabled.
//	bit[4:2]	: CCA Accumulation Window Size. 0=1us, 1=2us, 2=4us, 3=8us, 4~7=16us.
//	bit[1:0]	: CCA Mode. 0=ED, 1=CD, 2=FD, 3=reserved.
#define	xCCA0			XBYTE[PHY_BASE + 0x2C]
#define	xCCA1			XBYTE[PHY_BASE + 0x2D]
//---------------------------------------------------------------
#define	xAGCCNF7		XBYTE[PHY_BASE + 0x37]
//---------------------------------------------------------------
#define	xTST0			XBYTE[PHY_BASE + 0x70]
#define	xLQICNF0			XBYTE[PHY_BASE + 0x7E]	// bit[7]:Valid, bit[3]=Ena
#define	xLQICNF1			XBYTE[PHY_BASE + 0x7F]	// read only
//---------------------------------------------------------------
#define	xPHYSTS0			XBYTE[PHY_BASE + 0x80]
#define	xPHYSTS1			XBYTE[PHY_BASE + 0x81]
#define	xAGCSTS2		XBYTE[PHY_BASE + 0x84]
#define	xAGCSTS3		XBYTE[PHY_BASE + 0x85]
//-	xINTCON
//	bit[7]	: reserved
//	bit[6]	: 0=PHY interrupt is occured.
//	bit[5]	: 1=Tx-Fail interrupt enabled
//	bit[4]	: 1=Modem-Fail interrupt enabled
//	bit[3]	: 1=RX-End interrupt enabled
//	bit[2]	: 1=RX-Start interrupt enabled
//	bit[1]	: 1=TX-End interrupt enabled
//	bit[0]	: 1=Modem-On interrupt enabled
#define	xINTCON		XBYTE[PHY_BASE + 0x8D]
	#define	PHY_INT_MASK_MODEM_ON	0x01	// If set to '1', interrupt is enabled. Highest priority
	#define	PHY_INT_MASK_TX_END		0x02	// If set to '1', interrupt is enabled. High priority
	#define	PHY_INT_MASK_RX_START	0x04	// If set to '1', interrupt is enabled. Medium priority
	#define	PHY_INT_MASK_RX_END		0x08	// If set to '1', interrupt is enabled. Low priority
	#define	PHY_INT_MASK_MODEM_FAIL	0x10	// If set to '1', interrupt is enabled. Lowest priority
	#define	PHY_INT_MASK_TX_FAIL		0x20	// If set to '1', interrupt is enabled.
//-	xINTIDX
//	bit[3]	: All interrupt clear. Active low.
//	bit[2:0]	: Interrupt table index
#define	xINTIDX			XBYTE[PHY_BASE + 0x8E]
	#define	PHY_INT_IDX_MODEM_ON		0
	#define	PHY_INT_IDX_TX_END		1
	#define	PHY_INT_IDX_RX_START		2
	#define	PHY_INT_IDX_RX_END		3
	#define	PHY_INT_IDX_MODEM_FAIL	4
	#define	PHY_INT_IDX_TX_FAIL		5
	#define	PHY_INT_IDX_MASK			0x07
//-	xINTSTS
//	bit[5]	: 0=TX-Fail interrupt is pending
//	bit[4]	: 0=Modem-Fail interrupt is pending
//	bit[3]	: 0=RX-End interrupt is pending
//	bit[2]	: 0=RX-Start interrupt is pending
//	bit[1]	: 0=TX-End interrupt is pending
//	bit[0]	: 0=Modem-On interrupt is pending
#define	xINTSTS			XBYTE[PHY_BASE + 0x8F]
//---------------------------------------------------------------
#define	xPLLCTRL			XBYTE[PHY_BASE + 0xB0]
#define	xPLLMON			XBYTE[PHY_BASE + 0xBD]
//---------------------------------------------------------------
#define	xDCCCON			XBYTE[PHY_BASE + 0xD0]
#define	xADCCNF1		XBYTE[PHY_BASE + 0xD4]
#define	xADCCNF2		XBYTE[PHY_BASE + 0xD5]
#define	xADCSTS			XBYTE[PHY_BASE + 0xD6]
#define	xADVAL			XBYTE[PHY_BASE + 0xD7]
#define	xCHIPID			XBYTE[PHY_BASE + 0xD9]
//---------------------------------------------------------------
//-	xPDCON
//	bit[7]	: BOD enable.
//	bit[6]	: Analog voltage regulator enable.
//	bit[5]	: Sleep timer enable.
//	bit[4]	: HSRCOSC enable.
//	bit[3]	: RCOSC enable.
//	bit[2]	: Select realtime clock. 1=External
//	bit[1:0]	: Power-down mode.
#define	xPDCON			XBYTE[PHY_BASE + 0xE0]
//-	xRCOSC
//	bit[7]	: HSRCOSC calibration done. Read Only.
//	bit[6]	: HSRCOSC calibration enable.
//	bit[5]	: HSRCOSC calibration reset. Active low.
//	bit[4]	: RCOSC calibration done. Read Only.
//	bit[3]	: RCOSC calibration enable.
//	bit[2]	: RCOSC reset. Active low.
//	bit[1]	: RCOSC calibration hysterysis.
//	bit[0]	: RCOSC divide by 2.
#define	xRCOSC			XBYTE[PHY_BASE + 0xE1]
#define	xRTDLY			XBYTE[PHY_BASE + 0xE2]
#define	xRTINT1			XBYTE[PHY_BASE + 0xE3]
#define	xRTINT2			XBYTE[PHY_BASE + 0xE4]
#define	xRTINT3			XBYTE[PHY_BASE + 0xE5]
//-	xPDMON
//	bit[7]	: RTC Period. 0=30us, 1=3.9ms.
//	bit[6]	: OSC OK. Read Only.
//	bit[5]	: PD FLAG. Read Only.
//	bit[4:3]	: RTINT Expand.
//	bit[2]	: ACHMEN. 1=ADC Enable, 0=When Power-down.
//	bit[1:0]	: ACHMSEL[1:0]. 0=PLL, 1=RMIX, 2=VLPF, 3=SADC.
#define	xPDMON			XBYTE[PHY_BASE + 0xE6]
#define	xGPIOPS0		XBYTE[PHY_BASE + 0xE7]
#define	xGPIOPS1		XBYTE[PHY_BASE + 0xE8]
#define	xGPIOPS3		XBYTE[PHY_BASE + 0xE9]
#define	xGPIOPE0		XBYTE[PHY_BASE + 0xEA]
#define	xGPIOPE1		XBYTE[PHY_BASE + 0xEB]
#define	xGPIOPE3		XBYTE[PHY_BASE + 0xEC]
#define	xGPIOPOL0		XBYTE[PHY_BASE + 0xED]
#define	xGPIOPOL1		XBYTE[PHY_BASE + 0xEE]
#define	xGPIOPOL3		XBYTE[PHY_BASE + 0xEF]
//---------------------------------------------------------------
#define	xGPIOMSK0		XBYTE[PHY_BASE + 0xF0]
#define	xGPIOMSK1		XBYTE[PHY_BASE + 0xF1]
#define	xGPIOMSK3		XBYTE[PHY_BASE + 0xF2]
//---------------------------------------------------------------







//----------------------------------------------------
// Clock & Reset Control
//----------------------------------------------------
//-	xPHY_CLK_EN0
//	bit[7]	: CLK_MPI.	1=enable
//	bit[6]	: CLK_RX.	1=enable
//	bit[5]	: CLK_REG.	1=enable
//	bit[4]	: CLK_TX.	1=enable
//	bit[3]	: CLK_TST.	1=enable
//	bit[2]	: CLK_MR.	1=enable
//	bit[1]	: CLK_MT.	1=enable
//	bit[0]	: CLK_AES.	1=enable
#define	xPHY_CLK_EN0			XBYTE[0x2780]		// Default 0x20
//-	xPHY_CLK_EN1
//	bit[7:6]	: reserved
//	bit[5]	: DCCLK(16MHz).	1=enable
//	bit[4]	: OSCLK(32MHz).	1=enable
//	bit[3]	: CLK_ADC.		1=enable
//	bit[2]	: CLK_DECI.		1=enable
//	bit[1]	: CLK_DMCAL.	1=enable
//	bit[0]	: CLK_RXADC.	1=enable
#define	xPHY_CLK_EN1			XBYTE[0x2781]		// Default 0x00
//-	xPHY_CLK_FR_EN0
//	bit[7]	: CLK_MPI forced enable.	1=enable
//	bit[6]	: CLK_RX forced enable.	1=enable
//	bit[5]	: CLK_REG forced enable.	1=enable
//	bit[4]	: CLK_TX forced enable.	1=enable
//	bit[3]	: CLK_TST forced enable.	1=enable
//	bit[2]	: CLK_MR forced enable.	1=enable
//	bit[1]	: CLK_MT forced enable.	1=enable
//	bit[0]	: CLK_AES forced enable.	1=enable
#define	xPHY_CLK_FR_EN0		XBYTE[0x2782]		// Default 0x00
//-	xPHY_CLK_FR_EN1
//	bit[7:6]	: reserved
//	bit[5]	: DCCLK(16MHz). 			1=enable
//	bit[4]	: OSCLK(32MHz). 			1=enable
//	bit[3]	: CLK_ADC activation. 		1=enable
//	bit[2]	: CLK_DECI forced enable. 	1=enable
//	bit[1]	: CLK_DMCAL forced enable.	1=enable
//	bit[0]	: CLK_RXADC forced enable.	1=enable
#define	xPHY_CLK_FR_EN1		XBYTE[0x2783]		// Default 0x00
#define	xPHY_SW_RSTB			XBYTE[0x2784]		// Default 0xFF
//-	xPHY_MPTOP_CLK_SEL
//	bit[7:4]	: reserved
//	bit[3]	: CLK_AGC's frequency.		If 2Mcps, 0=8MHz, 1=16MHz. If 4Mcps, 0=16MHz, 1=32MHz.
//	bit[2]	: MAC_CLK's frequency.		0=8MHz, 1=16MHz.
//	bit[1:0]	: PHY_CLK's frequency.		3=4Mcps, Others=2Mcps.
#define	xPHY_MPTOP_CLK_SEL	XBYTE[0x2785]		// Default 0x00
//-	xPHY_ADC_CLK_SEL
//	bit[7:2]	: reserved
//	bit[1:0]	: ADC sampling clock. 0=1MHz, 1=2MHz, 2=4MHz, 3=invalid.
#define	xPHY_ADC_CLK_SEL		XBYTE[0x2786]		// Default 0x02
//-	xPHY_EXT_CLK_CTL
//	bit[7]	: Clock output via P1.3		1=enable
//	bit[6:3]	: reserved
//	bit[2:0]	: Clock frequency. 0=500K, 1=1M, 2=4M, 3=4M, 4=8M, 5=16M, 6=32M, 7=Off
#define	xPHY_EXT_CLK_CTL		XBYTE[0x2787]		// Default 0x00

//----------------------------------------------------
// End of Register Set
//----------------------------------------------------








//----------------------------------------------------
// MG2470 GPIO Port 
//----------------------------------------------------
#define	MG2470_PORT00_I2SRXDI_PWM0						P0_0
#define	MG2470_PORT01_I2SRXLRCLK_PWM1					P0_1
#define	MG2470_PORT02_I2SRXBCLK_PWM2					P0_2
#define	MG2470_PORT03_I2SRXMCLK_PWM3					P0_3
#define	MG2470_PORT04_I2STXDO_PWM4						P0_4
#define	MG2470_PORT05_I2STXLRCLK_PTCGATE0				P0_5
#define	MG2470_PORT06_I2STXBCLK_PTCGATE1					P0_6
#define	MG2470_PORT07_I2STXMCLK_PTCGATE2				P0_7

#define	MG2470_PORT10_UART1RXD							P1_0
#define	MG2470_PORT11_UART1TXD							P1_1
#define	MG2470_PORT12									P1_2
#define	MG2470_PORT13_QUADZA_IRTX_CLKOUT_PTCGATE3		P1_3	// MG2470-B72 Only
#define	MG2470_PORT14_QUADZB_EXTRTCCLK_PTCGATE4			P1_4
#define	MG2470_PORT15									P1_5	// MG2470-B72 Only
#define	MG2470_PORT16_I2CSCL							P1_6
#define	MG2470_PORT17_I2CSDA							P1_7

#define	MG2470_PORT30_UART0RXD_QUADXA					P3_0
#define	MG2470_PORT31_UART0TXD_QUADXB					P3_1
#define	MG2470_PORT32_EXT0								P3_2
#define	MG2470_PORT33_EXT1								P3_3
#define	MG2470_PORT34_QUADYA_SPIDI_T0					P3_4
#define	MG2470_PORT35_QUADYB_SPIDO_T1					P3_5
#define	MG2470_PORT36_SPICLK								P3_6
#define	MG2470_PORT37_SPICSN							P3_7

#define	GP0A		P0
#define	GP00		MG2470_PORT00_I2SRXDI_PWM0
#define	GP01		MG2470_PORT01_I2SRXLRCLK_PWM1
#define	GP02		MG2470_PORT02_I2SRXBCLK_PWM2
#define	GP03		MG2470_PORT03_I2SRXMCLK_PWM3
#define	GP04		MG2470_PORT04_I2STXDO_PWM4
#define	GP05		MG2470_PORT05_I2STXLRCLK_PTCGATE0
#define	GP06		MG2470_PORT06_I2STXBCLK_PTCGATE1
#define	GP07		MG2470_PORT07_I2STXMCLK_PTCGATE2

#define	GP1A		P1
#define	GP10		MG2470_PORT10_UART1RXD
#define	GP11		MG2470_PORT11_UART1TXD
#define	GP12		MG2470_PORT12
#define	GP13		MG2470_PORT13_QUADZA_IRTX_CLKOUT_PTCGATE3
#define	GP14		MG2470_PORT14_QUADZB_EXTRTCCLK_PTCGATE4
#define	GP15		MG2470_PORT15
#define	GP16		MG2470_PORT16_I2CSCL
#define	GP17		MG2470_PORT17_I2CSDA

#define	GP3A		P3
#define	GP30		MG2470_PORT30_UART0RXD_QUADXA
#define	GP31		MG2470_PORT31_UART0TXD_QUADXB
#define	GP32		MG2470_PORT32_EXT0
#define	GP33		MG2470_PORT33_EXT1
#define	GP34		MG2470_PORT34_QUADYA_SPIDI_T0
#define	GP35		MG2470_PORT35_QUADYB_SPIDO_T1
#define	GP36		MG2470_PORT36_SPICLK
#define	GP37		MG2470_PORT37_SPICSN

//----------------------------------------------------
// Command Definition
//----------------------------------------------------
#define	REGCMD_MODEM_OFF				(xPCMD0 = 0x1C)		// Auto Cleared Command
#define	REGCMD_MODEM_ON				(xPCMD0 = 0x2C)		// Auto Cleared Command
#define	REGCMD_TX_REQ					(xPCMD0 = 0x38)		// Auto Cleared Command
#define	REGCMD_TX_CANCEL				(xPCMD0 = 0x34)		// Auto Cleared Command
#define	REGCMD_TX_CANCEL_CLEAR			(xPCMD0 = 0x3C)
#define	REGCMD_RX_ON					(xPCMD1 &= ~0x01)
#define	REGCMD_RX_OFF					(xPCMD1 |= 0x01)
#define	REGCMD_RSM_RESET_ON			(xMACRST |= 0x20)
#define	REGCMD_RSM_RESET_OFF			(xMACRST &= ~0x20)
#define	REGCMD_TSM_RESET_ON				(xMACRST |= 0x40)
#define	REGCMD_TSM_RESET_OFF			(xMACRST &= ~0x40)
#define	REGCMD_PHY_INT_RESET_ON			(xINTIDX &= ~0x08)
#define	REGCMD_PHY_INT_RESET_OFF			(xINTIDX |= 0x08)


#define	MCU_INT_FLAG_CLEAR_PHY			(EXIF1 = 0x10)

#endif	// #ifndef __MG2470_H__

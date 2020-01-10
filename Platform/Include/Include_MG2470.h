

#ifndef __INCLUDE_MG2470_H__
#define __INCLUDE_MG2470_H__

#include <STDIO.h>
#include <STRING.h>		// memcpy()
#include <STDLIB.h>		// rand(), srand()
#include <ABSACC.h>		// XBYTE
#include <INTRINS.h>		// _nop_()
#include <CTYPE.h>		// toint, toupper
#include <STDARG.h>		// va_list, va_start(), vsprintf(), va_end();

#include "MG2470.h"
#include "rpmemcpy.h"

//-	Data Type
typedef signed long  INT32;
typedef signed short INT16;
typedef signed char  INT8;

typedef volatile signed long  VINT32;
typedef volatile signed short VINT16;
typedef volatile signed char  VINT8;

typedef unsigned long  UINT32;
typedef unsigned short UINT16;
typedef unsigned char  UINT8;

typedef volatile unsigned long  VUINT32;
typedef volatile unsigned short VUINT16;
typedef volatile unsigned char  VUINT8;

//- Bitmap Definition
#define	BIT0	0x01
#define	BIT1	0x02
#define	BIT2	0x04
#define	BIT3	0x08
#define	BIT4	0x10
#define	BIT5	0x20
#define	BIT6	0x40
#define	BIT7	0x80

#define	BIT_1N0		0x03
#define	BIT_3N2		0x0C
#define	BIT_5N4		0x30
#define	BIT_7N6		0xC0

//-	Bit Operation
#define	BIT_SET(Variable, Bitmap)	( Variable |= Bitmap)
#define	BIT_CLR(Variable, Bitmap)	( Variable &= ~Bitmap)
#define	BIT_INV(Variable, Bitmap)	( Variable ^= Bitmap)
#define	BIT_CHK(Variable, Bitmap)	( Variable & Bitmap )

//- Priority
#define	PRIORITY_LOW	0
#define	PRIORITY_HIGH	1

//- Clcok On/Off & Interrupt Enable/Disable
#define	OFF			0
#define	ON			1
#define	RUN			1
#define	DISABLE		0
#define	ENABLE		1

#include "MG2470_LIB.h"

#ifdef DEBUG
	#define assert_error(expr) ((expr) ? assert_failed(expr) : (_nop_()) )
	void assert_failed(UINT16 u16DebugCode);
#else
	#define assert_error(expr) (_nop_())
#endif

#endif	// #ifndef __INCLUDE_MG2470_H__	

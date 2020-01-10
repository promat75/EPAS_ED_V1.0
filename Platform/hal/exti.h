
/*******************************************************************************
	[2012-04-25] Compatible with V2.0
*******************************************************************************/


#ifndef __MG2470_EXTI_H__
#define __MG2470_EXTI_H__
#define __MG2470_EXTI0_INCLUDE_
#define __MG2470_EXTI1_INCLUDE_

#define	EXTI_TYPE_LOW_LEVEL	0
#define	EXTI_TYPE_FALL_EDGE	1
#define	EXTI_TYPE_HIGH_LEVEL	2
#define	EXTI_TYPE_RISE_EDGE	3

#define	EXTI_CODE	0x0300

#define	RETURN_EXTI_SUCCESS				0x00
#define	RETURN_EXTI_P32_OUTPUT_STATE	0x10
#define	RETURN_EXTI_P33_OUTPUT_STATE	0x11

#ifdef __MG2470_EXTI0_INCLUDE_
	extern	UINT8	gu8Ext0IntFlag;
	UINT8 HAL_Ext0IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8TrigType);
#endif

#ifdef __MG2470_EXTI1_INCLUDE_
	extern	UINT8	gu8Ext1IntFlag;
	UINT8 HAL_Ext1IntSet(UINT8 u8IntEna, UINT8 u8Priority, UINT8 u8TrigType);
#endif

#endif	// #ifndef __MG2470_EXTI_H__



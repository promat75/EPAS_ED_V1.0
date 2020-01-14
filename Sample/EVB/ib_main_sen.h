
#ifndef __MG2470_ZED_MAIN_H__
#define __MG2470_ZED_MAIN_H__


#include <stdlib.h>

#include "include_MG2470.h"

#include "include_top.h"
#include "option_EVB.h"

//#include "isr.h"

#include "uart.h"
#include "timer.h"
#include "wdt.h"
#include "clock.h"
#include "gpio.h"
#include "phy.h"
#include "hmac.h"
#include "adc.h"
#include "power.h"
#include "exti.h"
#include "irtx_NEC.h"
#include "i2c.h"
#include "spi.h"
#include "gpioint.h"
#include "flash.h"
#include "security.h"

#include "util_app.h"
#include "util_sys.h"

//--------------------------
#include "ib_sensor.h"
#include "ib_packet.h"

#include "ib_uart.h"
#include "ib_zigbee.h"

#include "ib_ir_sen.h"
#include "ib_temp_sen.h"

//--------------------------


#define	FW_UPDATE_DATE						"20190409"
// Smart Sensor End Device VERSION  		
#define	SSE_VER_MAIN							'1'
#define	SSE_VER_SUB1							'6'	// ver �����  Rename_HexFile.bat ������ ver �� ���� ���� �Ұ�  !!!!!!  
#define	SSE_VER_SUB2							' '

                                   		
// Library_VERSION = V2.5           		
#define	LIB_VER_MAIN							'2'
#define	LIB_VER_SUB							'5'


#define TEST_SENSOR							0	// Multi comm �׽�Ʈ�� ���� ���� ����

// �����Ϸ� ���� ����  
#ifdef	__ZENER_BOARD
	#define ZENER_BOARD							1	// �ű� ���� ���� ����    
#else
	#define ZENER_BOARD							0	// �ű� ���� ���� �� ����    
#endif


/*******************************************************************************/
/* COMPILE Option define For EPAS V1.0                   By Kong.sh  */
#define _EPAS_MODE								1			// EPAS ���� ���� ��� ����  ( 0:NOT USE, 1:USE )

#define _ACCEL_USE								0			// ���ӵ� ���� ��� ����  ( 0:NOT USE , 1:USE )

#define _POWERDOWN_USE							1			// POWERDOWN MODE ��� ���� ( 0:NOT USE , 1:USE )
#define _POWERDOWN_MODE							1			// POWERDOWN MODE SELECT ( 1-POWERDOWN1 / 2-POWERDOWN2 )

#define POWER_DOWN_TIME_INIT					30			//  sec : �ʱ�  Power on�� wiat time Org 30  --> 0���� ���ý� POWER DOWN ���� �ȵ� (Remark!!)
#define POWER_DOWN_TIME							60			//  sec : ������ sleep mode  �۽� �ֱ� Kong Org 60

#define _TIMER0_USE								0			// TIMER0 ��� ����  ( 0:NOT USE , 1:USE )

#define	_UARTDEBUG_MODE							0			// UART1 DEBUG MODE ( 0:NOT USE , 1:USE )
#define	_SENSOR_PACKET_TXMODE					0			// RF Transmit : 0			UART Transmit : 1


#define AccelSensor_Powerdownmode				0
#define AccelSensor_Activemode					1

/// Sets the transmitting power.
///	@param	u8PowerLevel	: level of transmitting power
///	\n	0 = 9.0 dBm.   1 = 8.0 dBm.		2 = 8.0 dBm.		3 = 8.0 dBm....		9 = 0.0 dBm.
///	\n	10 = -1 dBm....				18 = -10 dBm.
///	\n	19 = -23 dBm.	 20 = -27 dBm.	21 = -33 dBm.		22 = -64 dBm.
#define	RF_TX_POWERLEVEL						18

//--- NTC Thermister ADC Offset Value
#define TEMP_OFFSET								0			//--- Org -302
/*******************************************************************************/



//--------------------------------------------------------------------------------
// ������ ID 
#define	PWR_ST_ID_CNT						6

//typedef enum {PWR_ST_HADONG, PWR_ST_DONGHAE } PWR_STATION;
#define	PWR_ST_DEBUG						1
#define	PWR_ST_HADONG						2
#define	PWR_ST_DONGHAE						3
#define	PWR_ST_BORYUNG						4
#define	PWR_ST_PWR_LAB						5	// ���� ������ 
#define	PWR_ST_SEBU							6	 

//#define	PWR_ST_HADONG						"81F801511500"
//#define	PWR_ST_DONGHAE					"82F801511500"  // "00155101F882"

//#define	PWR_ST_ID							PWR_ST_HADONG	// 1sec
//#define	PWR_ST_ID							PWR_ST_DONGHAE	// sleep mode(10sec) 
//#define	PWR_ST_ID							PWR_ST_BORYUNG	// sleep mode (10sec) 
//#define	PWR_ST_ID							PWR_ST_DEBUG
//#define	PWR_ST_ID							PWR_ST_PWR_LAB	// 5sec 
#define	PWR_ST_ID						PWR_ST_DONGHAE		// sleep mode(10sec) 


#if (PWR_ST_ID == PWR_ST_HADONG)
	#define EXTENDED_SENSOR_COUNT				0	// 1 : sensor ID ������ 60 ���� Ŭ��, 0 : sensor ID �� 60�� �ϴ� 
#else

	#define EXTENDED_SENSOR_COUNT				1	// 1 : sensor ID ������ 60 ���� Ŭ��, 0 : sensor ID �� 120�� �ϴ� 
	// #define EXTENDED_SENSOR_COUNT				0	//  1 : sensor ID ������ 60 ���� Ŭ��, 0 : sensor ID �� 120�� �ϴ�   <=  �ϵ� ŻȲ ���� ���� ������ 21r�� �ۿ� ���� ����, ������  0���� ����  
	
#endif


#define TEST_NO_PAIR_MODE						1	// 1 : ���� ���� �ʰ� ������ �Ҵ�� dest addr ���

//--------------------------------------------------------------------------------
#define THERM_SEN								1
#define INT_SEN									2
#define EXT_SEN									3
                                   		
#ifdef __INTERNAL_MODE              		

	// Kong ���� �µ�����  		
	// ���� �ϳ��� ��� : 17.12�� ���Ĵ� ��� ��̽��� ��� 
	//---------------------------------------------------------------------------------------
	//#define TEMP_SEN_MODE					INT_SEN   	// DS1208S   
	#define TEMP_SEN_MODE					THERM_SEN  	// 100K NTC thermistor   
	//---------------------------------------------------------------------------------------


#else
	// ���ܼ� �µ����� ����  : UART ��� 
	#define TEMP_SEN_MODE					EXT_SEN
#endif


// ���� ����� ��̽��ʹ� ���� ��� ���� ����(���������)                                    		
#if (TEMP_SEN_MODE == THERM_SEN)
	#define	SSE_VER_PATCH						'T'  		//  Thermistor ���� ����
#elif 	(ZENER_BOARD == 1)
	#define	SSE_VER_PATCH						'Z'    	//  ���� ���� ���� 
#else
	#define	SSE_VER_PATCH						' '           	// default 
#endif


//--------------------------------------------------------------------------------
#ifdef __OLD_BOARD					
#define	NEW_BOARD_DIP_SW					0
#else                               		
#define	NEW_BOARD_DIP_SW					1
#endif                              		
 //--------------------------------------------------------------------------------
                                    		
#define	_USE_DIPSWITCH						0		// 1 = dip switch ����, 0 = no dipswitch 
#define	NO_RPT_MODE							1		//  dip switch msb �� repeater mode�� ��� ���� ���� 


#define	DEFAULT_PANID						0x1201
#define	DEFAULT_CHANNEL						0x1A	// 26


#if TEST_SENSOR
	#define RED_LED							GP00
	#define BLUE_LED						GP01
#else                               		
	#define BLUE_LED						GP36
	#define RED_LED							GP37
#endif                              		
 
#define UART_DBG_MODE_PIN					GP10	// UART_RXD1
#define ACCELSEN_RESET						GP13	// TP5
#define NTC_CONNECT							GP35
#define PAIRING_BUTTON						GP14	// TP4
#define SLEEP_FAST_MODE						GP00	// dip switch MSB  : sleep mode  ���¿��� 1�� �۽�  
#define WAKEUP_FAST_MODE					GP01	// dip switch second MSB  : wakeup ���¿��� 1�� �۽�, NO init 30sec wait     

// board V4.2II �̻� ����                                    		
#define OVER_CURRENT_CHECK					GP32	// ���� �̻��� voer current ���� �߻��� Tr turn on �Ǹ鼭 port LOW �� ������      
                  
                                   		
#define I_PORT_ON							0
#define I_PORT_OFF							1
                                    		
#define LED_ON								0
#define LED_OFF								1
                                    		
#define BTN_ON								0		// port in 
#define BTN_OFF								1

#define RST_ON								0		// port out 
#define RST_OFF								1


// �ű� �߰� : bit7 =1 �̸� ������ �ּ��� �����Ϳ� ���� �ϵ���
#define PM_SAVE_MASK						0xE0   	// ���� 3��Ʈ�� ���� ��带 flash�� �����Ͽ� ���ý� ��� ���� ���� üũ �ϵ���  

#define PM_NOR_DBG_MODE						0x01	// �� ��ư�� ���� ����� ��� : no sleep mode, 1sec �۽� 
#define PM_UART_DBG_MODE					0x02	// ���׺��� ������ ����� ��� : sleep mode, 5sec �۽�  
#define PM_TRX_TEST_MODE                    0x04	// �ۼ��� packet count �� ǥ�� �ϴ� ��� 
#define PM_OVER_CURR_MODE					0x08	// ���� ���� ���� ���Է½� sleep delay ���� �ٷ� �۽� : PM_UART_DBG_MODE �� ���� ���� sleep �� 1�ʷ� �ٸ� 

#define PM_SLEEP_MODE						0x10

// ���⼭ ���� �÷��� ���� ��� ��� :  �߰� �ɶ�  PM_SAVE_MASK �� ���� �Ͽ� �־�ߵ� !! 
#define PM_RESERVED_MODE1					0x20	
#define PM_NO_DIP_SWITCH					0x40	

#if (NO_RPT_MODE)
// ���� PM_RPT_MODE �� ���� ���� 
#define PM_USE_OPTION_VALUE					0x80	

#else
#define PM_RPT_MODE							0x80	// ������ ��� ��� => 17.07.10 : ������ ��� ��� ���� �ʵ��� remark  
#endif


#define _IS_PM_DBG_MODE						(Porduct_Mode & (PM_NOR_DBG_MODE | PM_UART_DBG_MODE))	
#define _IS_NO_SLEEP_MODE					(~Porduct_Mode &  PM_SLEEP_MODE)
#define _IS_NO_FLS_DEFAULT					(gtPIB.Option.u8ProductMode != 0xFF)	// flash ���� ff�� �ƴϰ� �ǹ� �ִ� ���� write �Ǿ�����
#define _IS_NO_DIP_SWITCH					((gtPIB.Option.u8ProductMode & PM_NO_DIP_SWITCH) && _IS_NO_FLS_DEFAULT)
#define _USE_DIP_SWITCH						(_IS_NO_DIP_SWITCH == 0)
#define _USE_OPTION_VALUE					((gtPIB.Option.u8ProductMode & PM_USE_OPTION_VALUE) && _IS_NO_FLS_DEFAULT)


// wait timer 
//timer &tick : max 4��  - �� �ʿ�� util_app ���� ����
#define SQ_WAIT_TIMER						0			// tx process timer
#define COMON_TIMER							1			// pairing timer 
#define WAIT_TIMER_SHORT					2			// sensor read timer  
#define LED_CONT_TIMER						3			// led timer
#define ADC_WAIT_TIMER						4			// not used  


#define TIME_100MS							100			// ms
#define TIME_500MS							500		
#define TIME_1SEC							1000	
#define TIME_2SEC							2000	
#define TIME_5SEC							5000	

#define TIME_PER_TEST						TIME_100MS	//default(org) 10ms, -> 100ms
#define TIME_INIT_TX_DELAY					TIME_2SEC	// 2sec

//----------------------------------------------------------------------------------------------
#define TX_RANDTIME_MASK						0x0001F

#define SEN_READ_TIME							120		//  ms : tx �۽� �ֱ� �Ⱓ ����  ���� ����Ÿ�� �о� ó���ϴ� �ð� 
														// ª�� �ð�(120ms) ���� ������ �о� �۽��ϰ� sleep mode �� �� 
//-----------------------------------------------------------------------------------------------
// wakeup mode ���۽�  �۽� �ֱ⸦ ���� 
#if (PWR_ST_ID == PWR_ST_PWR_LAB)
	#define TX_TIME_PERIOD						1		//  �۽� �ֱ� : 1��(�⺻ �ֱ�)�� �ѹ��� �۽�, ( 5= 5sec, 10=10sec ), sleep time �� ������ ���� 
#else
	// ���� 400A ���ܱ� ���� cal ���� �׻� wakeup mode �� ��� : default 10��, ���� ���� ���� �Ͽ� 2�� ���� ��� �Ͽ� ��� �ϵ��� 
	#define TX_TIME_PERIOD						60		//  �۽� �ֱ� : 1��(�⺻ �ֱ�)�� �ѹ��� �۽�, ( 5= 5sec, 10=10sec ), sleep time �� ������ ���� 
#endif

// wakeup mode ���۽� time 
#define TX_TIME								TIME_1SEC	// 1sec : tx �۽� �ֱ� �Ⱓ ����  ���� ����Ÿ�� �о� ó���ϴ� �ð�

//-----------------------------------------------------------------------------------------------
// sleep mode ���۽�  	  
#define POWER_DOWN_TIME_UART_DBG				5		//  sec : ddduart debug ������ sleep mode �۽� �ֱ� 


// sleep mode ���۽ô� �׻�  TX_TIME_PERIOD = 1
//----------------------------------------------------------------------------------------------
// Flash address 
#define CODE_ADDRESS_USER0             			0x1200	// PairTable 
#define CODE_ADDRESS_USER1             			0x1400	// AccelerationSensor_ScalBuff
#define CODE_ADDRESS_USER2             			0x1600	// CurCal_DataBuff

#if EXTENDED_SENSOR_COUNT
#define MAX_END_DEVICE							125
#else                               		
#define MAX_END_DEVICE							60
#endif

typedef enum { LED_IDLE_STS, LED_NO_PAIRED_STS, LED_NO_PAIRED_STS1, LED_PAIRED_STS, LED_PAIRING_STS, 
 			LED_PAIRING_STS1,	LED_TX_STS, LED_TX_OK_STS, LED_TX_FAIL_STS, LED_NORMAL_STS} LED_STATE;
	
typedef enum { TX_WAIT_IDLE, TX_WAIT_INIT, TX_WAIT_SEN_READ_STS, TX_WAIT_SEN_END_READ_STS, TX_WAIT_STS, 
			TX_WAIT_TIME_SET_STS, TX_SLEEP_SEN_READ_STS, TX_SLEEP_STS, TX_WAKEUP_STS } TX_WAIT_PROC_STS;
	
			
extern UINT8 	u8LedState;


extern UINT8 Porduct_Mode;	
//extern UINT8 u8_TEMP_SEN_R_TIME;
//extern UINT8 u8_ACCEL_CURR_SEN_R_TIME;
extern UINT8 u8_SLEEP_TIME, u8_SLEEP_TIME_Prev;
extern UINT16 u16_TX_TIME;
extern UINT16 gTestCount;

void Init_FlashDefault(void);
void Init_SetHAL(void);
void Init_ExtAmp(void);

void LED_Process(void);
void TX_Wait_Process(void);
void Init_SysConfig(void);
 
#endif	// #ifndef __MG2470_CONTROLLER_MAIN_H__

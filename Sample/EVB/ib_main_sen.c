
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2011.
	- Date		: 2014-08-28
	- Version		: V2.6

//----------------------------------------------------------------------------------------------
// DIP Sw  0 = 11 ch

// !!!!!!!!! ID �� ��� 1 ~ 60 �ƴϸ� ���� ����  !!!!!!!!!!!!!!!!!!
// - source IEEE address ������ ����Ʈ�� ID �� ��� 1 ~ 60
//---------------------------------------------------------------------------------------------- <= old version : �ڵ� �� ���� ����

-  USE_PAIRING_CODE = 0 �϶�
   : �ٿ�ε�� IEEE�� ���� 1����Ʈ ���� �ϰ� ������ ������ �ٿ�ε� �ؾߵ�

-  Router ����
   : channel DIP sw : router or coordi�� ���� ������ ����(router ��� ���� 2���� ũ�ų� ���������� ���� �ؾߵ�, ������ ch 26 ��� ���)
 //----------------------------------------------------------------------------------------------

[2019-04-09] V01.6  :  �ϵ� ������ ���� �µ� ���� NO Avg ���� 
[2018-10-22] V01.5Z  :  �ϵ� ŻȲ - ���� ���� ���� ����Ÿ  ���� 
[2018-09-27] V01.5  :  ���� Incomming  Release - Tehrmistor (No  ���ʺ���)
	- ���� remark, Thermistor ON   
[2018-06-19] V01.5  :  �ϵ� 3�� ��ǰ  ���� Release
	- ZENER_BOARD  0  : ��� ����  
[2018-04-03] V01.5T  :  ���� ����  overcurretn wakeup 
	- �µ� ���� data packet�� ������ ����Ÿ�� �Ǿ� ���� �ϵ���   
[2018-03-05] V01.5 :  Low VCC ó�� Flag ó��   
[2018-01-22] V01.5 :  ���οµ������� ������  Release  ����   
	- DIP switch ���� ��� ����  
[2017-12-14] V01.5 : �µ� ���� NTC ��̽��� ���   
[2017-10-18] V01.4  :
	- LEDProcess()
[2017-10-16] V01.4 : vcc low ó��  
[2017-09-07] V01.3 : ���� 3�� Release
	-  ���� ������ 0, 0, 0 �� ��� �۽��� ���� �ʵ���
        -  wakeup mode �۽��ֱ� 10 -> 5��
[2017-09-04] V01.3 : �µ����� ���� ����
[2017-08-07] V01.2 : ������ Release - ����, �ȵ�, �¾�, ����    
[2017-07-18] V01.10  :  ���� ����Ÿ read �ֱ⸦ ���� ��忡 ���� ���� ��� �Ȱ���
[2017-07-14] V01.10  :  �µ� ���� while �� ���� 	
[2017-07-10] V01.10 -  �ڵ� ����   :  sleep mode ���θ� dip switch �� ��� �Ͽ� ���� �ϵ��� 
[2017-06-13] V01.10 -  ���ο�  Release 
[2017-06-09] V01.10 -  ���ο����� ���� ����    
[2017-06-08] V01.09 -  ����� ���� �۾� �ڵ�
[2017-05-08] V01.09 -  ����  Release
[2017-03-23] V01.08 - ���ؿ����� �ٽ� ������ ����   
[2017-02-22] V01.08 - SleepMode II  code  ������� 1.06  hisotry copy   
[2016-12-09] V01.06 :  ���� �ɼ� ��� ���� ó��
[2016-11-04] V01.05  :  Multi comm test routine ������Ʈ
[2016-11-01] V01.05 : ���� �ִ� 60 -> 125 �� ���� ���� �ϵ���
[2016-10-18] V01.04 :  ���� ���� ����
[2016-10-07] V01.03 :  �ϵ� 1�� ��ġ�� ������
[2016-09-06] V01.02 :-��� ���� ���� ���� ���� 
[2016-08-24] V01.02 :  �� ��ư ��� �߰�
[2016-08-11] V01.02 :  ���ܼ� �µ����� ����Ÿ ó�� �������  
[2016-08-08] V01.02 :  PER test ��ƾ 
[2016-07-20] V01.02 :  �ڵ� �� ��� �߰�	
*******************************************************************************/


#include "ib_main_sen.h"


UINT8 	Porduct_Mode = 0;
UINT16 	gTestCount = 0;

// powerdown mode conatant  ���� default   
// UINT8 u8_TEMP_SEN_R_TIME = TEMP_SEN_R_TIME_PD;
//UINT8 u8_ACCEL_CURR_SEN_R_TIME = ACCEL_CURR_SEN_R_TIME_PD;
UINT8 	u8_SLEEP_TIME = POWER_DOWN_TIME;	
UINT8 	u8_SLEEP_TIME_Prev;	

UINT16 	u16_TX_TIME = TX_TIME - SEN_READ_TIME;			// �⺻ �ֱ� 1sec�� �׻� ���� : ��Ȯ�� �Ϸ���  => TIME_1SEC - SEN_READ_TIME


//UINT8	COORDI_IEEE_ADDRESS[8] = {0x00, 0x15, 0x51, 0x01, 0x00, 0x01, 0xFF, 0xF8};
//UINT8	ROUTER_IEEE_ADDRESS[8] = {0x00, 0x15, 0x51, 0x01, 0x00, 0x01, 0xFF, 0xF0};
UINT8	COORDI_IEEE_ADDRESS[8] = {0x00, 0x00, 0xFF, 0xF8, 0x01, 0x51, 0x15, 0x00};
//UINT8	ROUTER_IEEE_ADDRESS[8] = {0x00, 0x01, 0x01, 0x00, 0x01, 0x51, 0x15, 0x00};

#include "ISR.c"

UINT8	code gcau8BufferFlash[512]	_at_ CODE_ADDRESS_USER0;
UINT8	code gcau8BufferFlash1[512]	_at_ CODE_ADDRESS_USER1;
UINT8	code gcau8BufferFlash2[512]	_at_ CODE_ADDRESS_USER2;

// mem �Ҵ翡 ������ ������ data �� ���� ������ ���� ���� ����, �̻����� ( ����� +6 ��ŭ ������������)
#if ( _EPAS_MODE == 0 )
xdata UINT16 malloc_mem[AVG_CURR_BUFF_SIZE + AVG_TEMP_BUFF_SIZE + 6];	
#endif
	
void 	Init_ExtAmp(void)
{
	
	//----------------------------------------------------------------------------------------
	//	Enable GPIO to control External Amplifier
	// TODO: If a external amplifier is used, GPOIs for that should be set.
#if (_OPTION_EXT_AMP_CONTROL == 2)
	zPrintf(1, "\n");
	zPrintf(1, "\n >> External Amplifier Control Signals are enabled.");
	zPrintf(1, "\n >> GPIO 1.7 is TRSW. (TX=High, RX=Low)");
	zPrintf(1, "\n >> GPIO 1.6 is nTRSW. (TX=Low, RX=High)");
	zPrintf(1, "\n");
	HAL_AmpCtrlGpio1Set(1);
#elif (_OPTION_EXT_AMP_CONTROL == 1)
	zPrintf(1, "\n");
	zPrintf(1, "\n >> External Amplifier Control Signals are enabled.");
	zPrintf(1, "\n >> GPIO 3.7 is TRSW. (TX=High, RX=Low)");
	zPrintf(1, "\n >> GPIO 3.6 is nTRSW. (TX=Low, RX=High)");
	zPrintf(1, "\n");
	HAL_AmpCtrlSignalSet(1, 0);
#else
	/*
	//	Booting Message	
	zPrintf(1, "\n");
	zPrintf(1, "\n >> No External Amplifier");
	*/	
#endif
	//----------------------------------------------------------------------------------------
}


void Init_SysConfig(void)
{
	//	TIMER0 : Turn on the clock and Set
	HAL_Timer0ClockOn(ON);
	HAL_Timer0IntSet(ENABLE, PRIORITY_LOW, RUN, 2);			// Enable TIMER0 Interrupt

	//  UART1 : Turn on the clock and Set
	#if _UARTDEBUG_MODE
	HAL_Uart1ClockOn(ON);
	HAL_Uart1IntSet(ENABLE, PRIORITY_LOW, 115200, UART_MODE_8N1);
	#endif

	// Kong - ���������� I2C Init --> Turn on the clock and Set
	#if _ACCEL_USE
	HAL_I2CClockOn(1);
	HAL_I2CIntSet(1,1,1,28);//20	
	#endif
	
	//	Interrupt Enable
	HAL_SystemIntSet(ENABLE);			// EA=1

	#if _UARTDEBUG_MODE
	zPrintf(1, "\n\r>> Reset Init..\n");
	#endif
	
	// 	GPIO : Turn on the clock and Set
	// TODO: Set GPIOs to be used if needed.	
	HAL_GpioClockOn(ON);
	
	// DIP switch MSB  : SLEEP_MODE_PIN  = 0(ON) : sleep mode	
 	//HAL_Gpio0InOutSet(0, GPIO_INPUT, GPIO_PULL_UP );		//--- Kong Current Down
	AppLib_DelayFor1us();
	
	// org
	//HAL_Gpio0InOutSet(1, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP01 = 0; // pen id 4  : 0x1F ���� 
	// WAKEUP_TX_PERIOD ������ ��� 
	#if (_USE_DIPSWITCH==1)
	HAL_Gpio0InOutSet(1, GPIO_INPUT, GPIO_PULL_UP);	
	HAL_Gpio0InOutSet(2, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP02 = 0;
	HAL_Gpio0InOutSet(3, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP03 = 0;
	HAL_Gpio0InOutSet(4, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP04 = 0;	
	HAL_Gpio0InOutSet(5, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP05 = 0; // pen id 0  
	HAL_Gpio0InOutSet(6, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP06 = 0;	// ch 3  
	HAL_Gpio0InOutSet(7, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP07 = 0;
	HAL_Gpio3InOutSet(3, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP33 = 0; 	
	HAL_Gpio3InOutSet(4, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP34 = 0;	// ch 0  
	#endif

	#if (_EPAS_MODE == 1)
	HAL_Gpio0InOutSet(0, GPIO_INPUT, GPIO_PULL_UP);					// 0���� ������ ���� TEST MODE�� ���۵�. ���� ���..
	HAL_Gpio0InOutSet(1, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(2, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(3, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(4, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(5, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(6, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(7, GPIO_INPUT, GPIO_PULL_UP);

	// P1.0 : booting �� ����� ��� üũ input pin���� ���
	HAL_Gpio1InOutSet(0, GPIO_INPUT, GPIO_PULL_UP);		//UART_RXD1 : UART_DBG_MODE_PIN  
	//HAL_Gpio1InOutSet(3, GPIO_INPUT, GPIO_PULL_UP);	
	HAL_Gpio1InOutSet(3, GPIO_OUTPUT, GPIO_DRIVE_4mA);	ACCELSEN_RESET = 0;	//TP5	
	HAL_Gpio1InOutSet(4, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio1InOutSet(6, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio1InOutSet(7, GPIO_INPUT, GPIO_PULL_UP);

	HAL_Gpio3InOutSet(0, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(1, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(2, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(3, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(4, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(5, GPIO_OUTPUT, GPIO_DRIVE_4mA);	NTC_CONNECT = 0;		// NTC Thermostor Ref Line GND Connection..
	HAL_Gpio3InOutSet(6, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(7, GPIO_INPUT, GPIO_PULL_UP);

	// P1.0 : booting �� ����� ��� üũ input pin���� ���
	//HAL_Gpio1InOutSet(0, GPIO_INPUT, GPIO_PULL_UP);		//UART_RXD1 : UART_DBG_MODE_PIN  
	//HAL_Gpio1InOutSet(3, GPIO_OUTPUT, GPIO_DRIVE_4mA);	ACCELSEN_RESET = RST_OFF;	//TP5	
	//HAL_Gpio1InOutSet(4, GPIO_INPUT, GPIO_PULL_UP);		PAIRING_BUTTON = BTN_OFF;	// TP4

	#elif (_EPAS_MODE == 0)
	HAL_Gpio3InOutSet(6, GPIO_OUTPUT, GPIO_DRIVE_4mA);	BLUE_LED = LED_OFF;	//BLUE LED OFF
	HAL_Gpio3InOutSet(7, GPIO_OUTPUT, GPIO_DRIVE_4mA);	RED_LED = LED_OFF;	//RED  LED OFF
	#endif
	
	#if (TEMP_SEN_MODE == THERM_SEN)	
	//HAL_Gpio3InOutSet(0, GPIO_INPUT, GPIO_PULL_UP); 		// Thermistor ��� �� ���� ��(TEMP_OUT)�� input���� ���� 	Kong.sh �ּ� 191225
	#endif

	//HAL_Gpio3InOutSet(2, GPIO_INPUT, GPIO_PULL_UP);		// OVER_CURRENT_CHECK	Kong.sh �ּ� 191225
	AppLib_DelayFor1us();
	
	//HAL_Gpio3InOutSet(5, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP35 = 0;		// Kong.sh �ּ�	191225
	
	// P1.0 : booting �� ����� ��� üũ input pin���� ���
	//HAL_Gpio1InOutSet(0, GPIO_INPUT, GPIO_PULL_UP);		//UART_RXD1 : UART_DBG_MODE_PIN  
	//HAL_Gpio1InOutSet(3, GPIO_OUTPUT, GPIO_DRIVE_4mA);	ACCELSEN_RESET = RST_OFF;	//TP5	
	//HAL_Gpio1InOutSet(4, GPIO_INPUT, GPIO_PULL_UP);		PAIRING_BUTTON = BTN_OFF;	// TP4

	if(xCHIPID == 0x70)
	{
		zPrintf(1, "\n Failed. This software only supports CHIP ID over 0x70 !!!");
		while(SYS_Uart1TxWorking());
		SYS_WdtReset(0);
	}
}

void Init_RF_HAL(void)
{
	// sleep mode ������ waiting �� ���� ���� RF �� �ѵ��� �Ǿ� ���� : sleep mode �� �ƴϰų� �׽�Ʈ�� RF�� �̸�  �ѵ���
	#if 0
	if ( (_POWERDOWN_USE==0) || (_IS_NO_SLEEP_MODE)  || (gtPIB.uPairCount == 0) ) {		//  normal mode �̸� �ʱ� waiting time ���� rf ON  
		// sleep mode  :  no pairing  �Ǵ� degug mode   =>  RF ON 
//	 	RED_LED = LED_ON; 		//RED  LED ON : pair OK
		zPrintf(1,"Init RF HAL..\n"
);
		Init_SetHAL();
		Init_ExtAmp();		
	}
	#endif

	#if 1
	if ( (_IS_NO_SLEEP_MODE)  || (gtPIB.uPairCount == 0) ) {		//  normal mode �̸� �ʱ� waiting time ���� rf ON  
		// sleep mode  :  no pairing  �Ǵ� degug mode   =>  RF ON 
//	 	RED_LED = LED_ON; 		//RED  LED ON : pair OK
		zPrintf(1,"Init RF HAL..\n"
);
		Init_SetHAL();
		Init_ExtAmp();		
	}
	#endif
}	



void Init_HAL_Modem(void)
{		
	HAL_ModemInit(0);

	if(HAL_ModemOn(10000, 1))
	{
		zPrintf(1, "HAL_Modem Reset...\n");
		SYS_WdtReset(0);
	}
	else
	{
		#if _UARTDEBUG_MODE
		zPrintf(1, "HAL_ModemOn => OK\n");
		#endif
	}
	
	HAL_ModemPost();


	#if _EPAS_MODE
	//HAL_RFAnalogSleep(POWER_RF_SLEEP_ADC_ON);	// RF Sleep.. 
	#endif
}
	
	
	
void Init_SetHAL(void)
{
	UINT8 u8Status;
		
	//----------------------------------------------------------------------------------------
	// Set HAL value
	//Configure Network Parameters
	u8Status = HAL_ChannelSet(gtMIB.u8Channel);
	if(u8Status)
	{
		zPrintf(1, "\n >> Channel Set Failed (%02x)",(short)u8Status);
	}
	
	HAL_PanIDSet(gtMIB.u16PanID);
	HAL_ShortAddrSet(gtMIB.u16ShortAddr);
	HAL_TxPowerSet(gtMIB.u8TxPowerLevel);
	HAL_DataRateSet(gtMIB.u8DataRate);
	HAL_IEEEAddrSet(gtMIB.au8IEEEAddr);	// source IEEE addr

#if (_OPTION_MULTI_DATA_RATE)
	HAL_MultiDataRateSet(ON);
#endif

	// Initialize Hardware MAC
	HAL_MacInit();

	// Initialize RF blocks and enable PHY interrupt. 
	HAL_RxEnable(OFF);
	HAL_PhyIntSet(ENABLE, PRIORITY_HIGH);
	HAL_RxEnable(ON);
	//----------------------------------------------------------------------------------------
}


//--------------------------------------------------------------------------------------------
// ���� HIB data �� u8DeviceMode �� ���� Ȱ��  ��� ����
//--------------------------------------------------------------------------------------------
void Init_HIB(void)
{
	UINT8				u8DeviceMode, iw;
	UINT8				u8KeyIn;

	tHW_INFORMATION		tDataHIB;


	// TODO: Here, MIB parameter can be modified, especially using HIB.
	// Set HIB Information to MIB.
	if(SYS_HIBCopy((UINT8 *)&tDataHIB))			// HIB_ADDRESS(0x1000) �� ���� �Ǿ� �ִ� ����Ÿ�� tDataHIB ���ۿ� ������)
	{										// gtMIB �� ���� set
		gtMIB.u8Channel = tDataHIB.u8Channel;
		gtMIB.u16PanID = (tDataHIB.au8PanID[1] << 8) | tDataHIB.au8PanID[0];
		gtMIB.u16DstPanID = gtMIB.u16PanID;
		
		rpmemcpy(gtMIB.au8IEEEAddr, tDataHIB.au8IEEEAddr, 8);
		// src short addr copy
		//gtMIB.u16ShortAddr = (tDataHIB.au8NwkAddr[1] << 8) | tDataHIB.au8NwkAddr[0];
		gtMIB.u16ShortAddr = tDataHIB.au8IEEEAddr[1];
		gtMIB.u16ShortAddr = gtMIB.u16ShortAddr << 8 | tDataHIB.au8IEEEAddr[0];
		
		
		gtMIB.u8DataRate = tDataHIB.u8DataRate;
		gtMIB.u16DstShortAddr = (tDataHIB.au8GeneralWord0[1] << 8) | tDataHIB.au8GeneralWord0[0];
		
		
		rpmemcpy(gtMIB.au8DstIEEEAddr, tDataHIB.au8Reserved_0, 8);

		#if _UARTDEBUG_MODE
			zPrintf(1,"gtMIB.u8Channel : %u\n", (short)gtMIB.u8Channel);
			zPrintf(1,"gtMIB.u16PanID : 0x%04x\n", (short)gtMIB.u16PanID);
			zPrintf(1,"gtMIB.u16DstPanID : 0x%04x\n", (short)gtMIB.u16DstPanID);
			zPrintf(1,"gtMIB.au8IEEEAddr : 0x%02x%02x%02x%02x%02x%02x%02x%02x\n", (short)gtMIB.au8IEEEAddr[7],(short)gtMIB.au8IEEEAddr[6],(short)gtMIB.au8IEEEAddr[5],(short)gtMIB.au8IEEEAddr[4],(short)gtMIB.au8IEEEAddr[3],(short)gtMIB.au8IEEEAddr[2],(short)gtMIB.au8IEEEAddr[1],(short)gtMIB.au8IEEEAddr[0]);
			zPrintf(1,"gtMIB.u16ShortAddr : 0x%04x\n", (short)gtMIB.u16ShortAddr);
			zPrintf(1,"gtMIB.u8DataRate : %u\n", (short)gtMIB.u8DataRate);
		#endif
	}
	
	//----------------------------------------------------------------------------------------------
	// When HIB is invalid, Network Parameters are selected by keyboard input.
	//----------------------------------------------------------------------------------------------
	else
	{
		
		zPrintf(1, "\n >> HIB Copy FAIL !!!");
		zPrintf(1, "\n #########################################");
		zPrintf(1, "\n >> 0 : PER Transmitter");
		zPrintf(1, "\n >> 1 : PER Receiver");
		zPrintf(1, "\n #########################################");
		zPrintf(1, "\n >> Select Device Mode (within 5 seconds) : ");
		
		//----------------------------------------------------------------------------------------
		// ���� ��� �ϰ� ���� ����, ���� Ȱ�� ����
		//----------------------------------------------------------------------------------------
		
		// Wait for 5 seconds. If no key input, Device Mode is 0.
		// 5�� ��ٸ��� PER test mode ���� �ǵ���
		u8DeviceMode = '0';
		
		AppLib_VirtualTimerSet(0, 5000);
		while(AppLib_VirtualTimerGet(0))
		{
			#if !_EPAS_MODE
			SYS_WdtSet(3000);
			#endif
			if(SYS_Uart1Get(&u8KeyIn))
			{
				if(u8KeyIn == '1')
				{
					u8DeviceMode = '1';
				}
				break;
			}
		}
		
		SYS_Uart1Put(u8DeviceMode);

		// u8DeviceMode�� ���� �̸� ������ default MIB ���� load 
		if(u8DeviceMode == '0')
		{
			gtMIB.u8Channel = INIT_CHANNEL;				// INIT_CHANNEL : 0x1A
			gtMIB.u16PanID = INIT_PAN_ID;				// INIT_PAN_ID : 0x2470
			gtMIB.u16DstPanID = gtMIB.u16PanID;
			gtMIB.u16ShortAddr = INIT_DEV0_SHORTADDR;
			gtMIB.u16DstShortAddr = INIT_DEV1_SHORTADDR;
			for(iw=0 ; iw<8 ; iw++)
			{
				gtMIB.au8IEEEAddr[iw] = INIT_DEV0_IEEEADDR_LSB + iw;
				gtMIB.au8DstIEEEAddr[iw] = INIT_DEV1_IEEEADDR_LSB + iw;
			}
		}
		else if(u8DeviceMode == '1')
		{
			gtMIB.u8Channel = INIT_CHANNEL;
			gtMIB.u16PanID = INIT_PAN_ID;
			gtMIB.u16DstPanID = gtMIB.u16PanID;
			gtMIB.u16ShortAddr = INIT_DEV1_SHORTADDR;
			gtMIB.u16DstShortAddr = INIT_DEV0_SHORTADDR;
			for(iw=0 ; iw<8 ; iw++)
			{
				gtMIB.au8IEEEAddr[iw] = INIT_DEV1_IEEEADDR_LSB + iw;
				gtMIB.au8DstIEEEAddr[iw] = INIT_DEV0_IEEEADDR_LSB + iw;
			}
		}
				
	}


	//--- Destinaton MAC SET
	#if (TEST_NO_PAIR_MODE)

		gtPIB.uPairCount = 1;
		gtPIB.PairInfo.SrcID = gtMIB.au8IEEEAddr[0];
		gtPIB.u16PanID = gtMIB.u16PanID;
		
		// dest addre�� �ڱ��ּ��� ���� 7 �ڸ��� ���� �ϰ�, ������ �ڸ��� 0x00���� ���� ����
		rpmemcpy(gtMIB.au8DstIEEEAddr, gtMIB.au8IEEEAddr, 8);
		gtMIB.au8DstIEEEAddr[0] = 0x00;

		// copy short address
		gtMIB.u16DstShortAddr = gtMIB.au8DstIEEEAddr[1];
		gtMIB.u16DstShortAddr = gtMIB.u16DstShortAddr << 8 | gtMIB.au8DstIEEEAddr[0];
		
		u8AppRFSq = SQ_RF_TX_INIT; 	

		#if _UARTDEBUG_MODE
		zPrintf(1,"gtMIB.au8DstIEEEAddr : 0x%02x%02x%02x%02x%02x%02x%02x%02x\n", (short)gtMIB.au8DstIEEEAddr[7],(short)gtMIB.au8DstIEEEAddr[6],(short)gtMIB.au8DstIEEEAddr[5],(short)gtMIB.au8DstIEEEAddr[4],(short)gtMIB.au8DstIEEEAddr[3],(short)gtMIB.au8DstIEEEAddr[2],(short)gtMIB.au8DstIEEEAddr[1],(short)gtMIB.au8DstIEEEAddr[0]);
		#endif
		
	#else

		// flash�� save �� �� ����Ÿ�� ������	
		if (Pair_PIB_COPY((UINT8*)&gtPIB) == 0) {
			gtPIB.uPairCount = 0;								// flash read(gtPIB.uPairCount)�ϸ鼭 faile �� 0xff ���� �������Ե� 	
			u8AppRFSq = SQ_WAIT; 								// pairing�� �Ǿ� ���� ������ UART�� ����  ����  pairing �� ���� 
			u8LedState = LED_NO_PAIRED_STS;

			//zPrintf(1, "\n >> uPairCount = %d", (short)gtPIB.uPairCount );
			zPrintf(1, "\n  No Pair data : Do Pairing !!!");
			AppLib_VirtualTimerSet(SQ_WAIT_TIMER, TIME_500MS);
		} 
		else {
			Pair_Display_table();
			SetDestIEEEAddress(gtPIB.PairInfo.au8IEEEaddr);

			#if (ZENER_BOARD)
			u8AppRFSq = SQ_INIT_OVER_CHK;						// �ٸ� �׽�Ʈ ��� #define ���� ���� ���� ����, ���� ���� ��� 
			AppLib_VirtualTimerSet(SQ_WAIT_TIMER,  1000);			// 1000ms ���� over current pin check 
															// 2000 : 150A���� over current ���� , 1000 : 160A���� over current ����
			#else
			u8AppRFSq = SQ_RF_TX_INIT; 			
			#endif
		}	
	#endif
}
//--------------------------------------------------------------------------------------------


// Flash�� ����� HIB �� ������ MIB �� set
void	Init_MIB(void)
{

	#if _EPAS_MODE
	gtMIB.u8NoAckRetry = 1;
	#else
	gtMIB.u8NoAckRetry = 3;
	#endif
	
	// 0 =9dBm, ... 9=0, 10=-1, ... 18=-10dBm, 19=-23dBm, 20=-27dBm
	gtMIB.u8TxPowerLevel = RF_TX_POWERLEVEL;		// power level ����  

	//Initialize MIB : default value
	memset(gtMIB.au8DstIEEEAddr, 0x00, 8);
	memset(gtMIB.au8IEEEAddr, 0x00, 8);
	gtMIB.u16DstPanID = maccBroadcast;			// maccBroadcast = 0xFFFF
	gtMIB.u16DstShortAddr = maccBroadcast;		// maccBroadcast = 0xFFFF
	gtMIB.u16PanID = maccBroadcast;				// maccBroadcast = 0xFFFF
	gtMIB.u16ShortAddr = maccBroadcast;			// maccBroadcast = 0xFFFF
	#if _EPAS_MODE
	gtMIB.u8BackoffRetry = 3;
	#else
	gtMIB.u8BackoffRetry = 10;
	#endif
	gtMIB.u8Channel = 0x0B;
	gtMIB.u8DataRate = PHY_DATA_RATE_250K;
	gtMIB.u8DSN = 0xEE;

	Init_HIB();		// MAC SETUP
	

	// ���� ���� �ʴ� �׽�Ʈ ��� �϶�  destination address �� ���� ���� (�ٿ�ε�� ���� 1����Ʈ ���� �ϰ� ������ ������ �ٿ�ε� �ؾߵ�)
#if  TEST_SENSOR
	// ä��, �Ҿ��̵� ����	
	gtMIB.u8Channel = 11;						// ch=11
	gtMIB.u16PanID = 0x1210;
	gtMIB.u16DstPanID=gtMIB.u16PanID;				// dest PAN ID


	SetDestIEEEAddress((UINT8*) &COORDI_IEEE_ADDRESS);
	zPrintf(1, "\n Start Fixed Coordi Mode");		
	zPrintf(1, " :DstIEEEAddr(0~7)=0x");
	Display_Buffer(gtMIB.au8DstIEEEAddr, 8, 0);	

	// !!!!!!!!! ID �� ��� 1 ~ 60 �ƴϸ� ���� ����  !!!!!!!!!!!!!!!!!!
	// source IEEE address ������ ����Ʈ�� ID �� ��� 1 ~ 60
	if ( gtMIB.au8IEEEAddr[0] > 0 && gtMIB.au8IEEEAddr[0] <= MAX_END_DEVICE) {
		gtPIB.PairInfo.SrcID = gtMIB.au8IEEEAddr[0];
		gtPIB.uPairCount = 1;
		u8AppRFSq = SQ_RF_TX_INIT; 	
		//Display_MIB(&gtMIB);
	}
	else
		zPrintf(1, "\n Multi Comm test Sensor ID ERROR !!!!");		
		
			
	//Fixed_PanIDChannel_Address();
	
	Porduct_Mode |= PM_NOR_DBG_MODE;
//	RED_LED = LED_ON; 								//RED  LED ON : pair OK
	
#else

	#if (_USE_DIPSWITCH)	
	SetDipSwitch_ChannelPanID();								//	Return==>Port_ChPenID
	#endif
	
#endif
	
	
	gtMIB.u8DSN = rand();

}

void Init_Adc(void)
{
	INT16	i16AdcRead;
	UINT8	u8Status;
	extern INT16	gi16ADC_VCC, gi16ADC_GND;

	//----------------------------------------------------------------------------------------
    	// ADC Init : maybe for Current sensor
	//----------------------------------------------------------------------------------------
	// Initialize Random Generator with SEED
	u8Status = SYS_AdcGet_Once(ADC_CHAN_TEMPERATURE, &i16AdcRead);

	#if _UARTDEBUG_MODE
		u8Status = SYS_AdcGet_Once(ADC_CHAN_GND, &i16AdcRead);
		zPrintf(1, "GND Adc = %6d\n",i16AdcRead);
		AppLib_Delay(2);
		u8Status = SYS_AdcGet_Once(ADC_CHAN_VCC, &i16AdcRead);
		zPrintf(1, "VCC Adc = %6d\n",i16AdcRead);
		AppLib_Delay(2);
		u8Status = SYS_AdcGet_Once(ADC_CHAN_TEMPERATURE, &i16AdcRead);
		zPrintf(1, "Inner TEMP Adc = %6d\n",i16AdcRead);
	#endif
	
	if(u8Status)
	{
		zPrintf(1, "\n AdcGet Error. Sta = 0x%02x",(short)u8Status);
	}
	srand((UINT16)i16AdcRead);
	
    AvgBufferInit(AVG_CURR_BUFF_SIZE, &gtCurrentAvg );
   	   	
	#if 0	
	SYS_AdcGet_VCCForCalib();
	SYS_AdcGet_GNDForCalib();
	#else	
	gi16ADC_VCC = ADCV_CAL_VCC;		// ��������� ����  
	gi16ADC_GND = ADCV_CAL_GND;		// ��������� ����  
	#endif
	
	//--- Kong.sh 190107 Modify..  Org --> SYS_AdcSet(ENABLE, 0, ADC_CHAN_0);
	#if _EPAS_MODE
	SYS_AdcSet(ENABLE, 0, ADC_CHAN_3); 	//--- Kong.sh ADC_CHAN_0 --> ADC_CHAN_3 ���� 19.12.27
	#else
	SYS_AdcSet(ENABLE, 0, ADC_CHAN_0); 	
	#endif


	//GetInit_CurrentData(); 				// ������ ���ۿ� ���� ����� ��� ���� 
	
}
	

void 	Init_Sensor(void)
{
	// flash read : accel factor00000000
	// Kong test ���ӵ����� ������ ���׺� ��� ���ۻ��¸� ������ �� �ֵ��� �ϱ� ���Ͽ� �ּ�ó��
	#if _ACCEL_USE
	HAL_FlashRead(CODE_ADDRESS_USER1, AccelerationSensor_ScalBuff, 511); //0x1400
	if((!(AccelerationSensor_ScalBuff[0]==0xFF))&&(!(AccelerationSensor_ScalBuff[1]==0xFF))&&
	(!(AccelerationSensor_ScalBuff[2]==0xFF)))
	{
		AccelerationSensor_Scalefactor[0]=AccelerationSensor_ScalBuff[0];
		AccelerationSensor_Scalefactor[1]=AccelerationSensor_ScalBuff[1];
		AccelerationSensor_Scalefactor[2]=AccelerationSensor_ScalBuff[2];		
	}
	else
	{
		AccelerationSensor_Scalefactor[0]=0;
		AccelerationSensor_Scalefactor[1]=0;
		AccelerationSensor_Scalefactor[2]=0;		
	}
	#else
	// ���ӵ����� ������ ���׺� ��� ���ۻ��¸� ������ �� �ֵ��� �ϱ� ���Ͽ� �߰��� �ӽ� �ڵ� ����
	AccelerationSensor_Scalefactor[0]=0x01;
	AccelerationSensor_Scalefactor[1]=0x02;
	AccelerationSensor_Scalefactor[2]=0x03;		
	#endif
	
	// flash read : cal data
	#if 0 // org code
	HAL_FlashRead(CODE_ADDRESS_USER2, CurCal_DataBuff, 511);
	
	if(((CurCal_DataBuff[0]==0xFF))&&((CurCal_DataBuff[4]==0xFF))&&((CurCal_DataBuff[8]==0xFF))
	&&((CurCal_DataBuff[12]==0xFF))&&((CurCal_DataBuff[16]==0xFF)))
	{
		Inver_A = 0;	
		Inver_B = 0;	
		Inver_C = 1;
		Inver_D = 0;	
	}
	else
	{
		memcpy(&Test_int32, &CurCal_DataBuff, 4);
		Inver_A = *((float*)&Test_int32);	
		memcpy(&Test_int32, &CurCal_DataBuff[4], 4);
		Inver_B = *((float*)&Test_int32);	
		memcpy(&Test_int32, &CurCal_DataBuff[8], 4);
		Inver_C = *((float*)&Test_int32);
		memcpy(&Test_int32, &CurCal_DataBuff[12], 4);
		Inver_D = *((float*)&Test_int32);
	}		
	#else
	// zener board : ���� ���� ��� �Ͽ� ���� ���� ����Ÿ ��� ���� �Ķ��Ÿ
	HAL_FlashRead(CODE_ADDRESS_USER2, CurCal_DataBuff, 511);
	
	if ((CurCal_DataBuff[0] !=0xFF) && (CurCal_DataBuff[1] !=0xFF)){
		u8StartZenorADC = CurCal_DataBuff[0];
		u8RecoveryZenorADC = CurCal_DataBuff[1];
		if (u8StartZenorADC <= u8RecoveryZenorADC)
			u8RecoveryZenorADC = u8StartZenorADC - 0x08;		// recovery hysterisys 
	}
	
	#if  (ZENER_BOARD == 1)
		if (_IS_NO_SLEEP_MODE)
			zPrintf(1," Zener ADC Start / Recovery Value : 0x%02x   0x%02x \r",  (short)u8StartZenorADC, (short)u8RecoveryZenorADC);
	#endif
	
	#endif	

	//----------------------------------------------------------------------------------------
	// Accelerometer Init
	//----------------------------------------------------------------------------------------
	// Kong ���ӵ����� ������ ���׺� ��� ���ۻ��¸� ������ �� �ֵ��� �ϱ� ���Ͽ� �ּ�ó��
	#if _ACCEL_USE
	AccelSensor_Init();
	#endif

	//----------------------------------------------------------------------------------------
	// Temperature Init	
	//----------------------------------------------------------------------------------------
	TempSensor_Init();

	//----------------------------------------------------------------------------------------
	//  Current Sensor(ADC) Init	
	//----------------------------------------------------------------------------------------
	Init_Adc();
	
}


void Init_VerDisplay(void)
{

	if ((_IS_NO_SLEEP_MODE) || ( _IS_PM_DBG_MODE)) {

		zPrintf(1, "\n ************************************************");
		zPrintf(1, "\n [HELP] : FF AC 02 01 03 B1 0D 0A");
		zPrintf(1, "\n End Device");
		zPrintf(1, "\n I.B Technology Co., Ltd. , 2014");    		

		#if (ZENER_BOARD == 1)
			zPrintf(1, "\n SW Ver= %s_SSDAE_ZENER ", (UINT8*)&FW_UPDATE_DATE); 
		#elif (TEMP_SEN_MODE == THERM_SEN)
			zPrintf(1, "\n SW Ver= %s_SSDAE_THERM ", (UINT8*)&FW_UPDATE_DATE); 
		#elif (TEMP_SEN_MODE == INT_SEN)	
			zPrintf(1, "\n SW Ver= %s_SSDAE_INT ", (UINT8*)&FW_UPDATE_DATE); 
		#else
			zPrintf(1, "\n SW Ver= %s_SSDAE_EXT ", (UINT8*)&FW_UPDATE_DATE); 
		#endif
	
		zPrintf(1, "V%c.%c%c%c ", SSE_VER_MAIN, SSE_VER_SUB1, SSE_VER_SUB2, SSE_VER_PATCH);
		//zPrintf(1, "\n Power Station ID = %d", (short)PWR_ST_ID);
		zPrintf(1, "\n ************************************************");	
	}

}


void Init_GetMode(void)
{
	
	//--------------------------------------------------------------------------
	// ���� ������ ID�� �о� �鿩 booting�� ��� ������ �Ҽ� �ֵ��� : �ּ� LSB 3��°�� �����Һ� �Ҵ� �Ǿ� ���� !!
	// ���� ���� �� ��ɽ� ������ ID�� �����Ͽ� default ���� ��带 ���� �ϵ���
	// Pair_PIB_COPY((UINT8*)&gtPIB);
	//--------------------------------------------------------------------------
	#if (_EPAS_MODE == 1 )

		Porduct_Mode |= PM_SLEEP_MODE;					// default OFF : sleep mode  

		#if _POWERDOWN_USE
			//HAL_PowerdownGpio1WakeupSourceSet(4, 1, 0);				// wakeup interrupt pin : p1.4 (pairing button)   
			//HAL_PowerdownGpio3WakeupSourceSet(2, 1, 0);				// ���� ���忡 �� ���� overcurrent wakeup interrupt pin : p3.2    

			#if (_POWERDOWN_MODE==1)
				#if _UARTDEBUG_MODE
				zPrintf(1,"getMode->PWDown1\n");
				AppLib_DelayFor1ms();
				#endif
				HAL_PowerdownMode1(POWER_DOWN_TIME_INIT, 0, 0, 0);		//  30 sec  : wiat for normal power 
			#elif (_POWERDOWN_MODE==2)
				#if _UARTDEBUG_MODE
				zPrintf(1,"getMode->PWDown2\n");
				AppLib_DelayFor1ms();
				#endif
				HAL_PowerdownMode1(POWER_DOWN_TIME_INIT, 0, 0, 0);		//  MODE2�� ��� SW Reset ��..
			#endif
			
			//HAL_PowerdownGpio3WakeupSourceSet(2, 0, 0);				// ���� ���忡 �� ���� overcurrent wakeup interrupt pin : p3.2    
		#endif
		
	#elif (_EPAS_MODE == 0 )
	
		if (UART_DBG_MODE_PIN == 0) 
			Porduct_Mode |= (PM_UART_DBG_MODE |  PM_SLEEP_MODE) ;	// ���׺��带 ���� uart pin ����� ���  : sleep mode ����   
			
		else if (PAIRING_BUTTON == BTN_ON)  		   	
			Porduct_Mode |= PM_NOR_DBG_MODE; 						// booting �� �� ��ư ���� ó��  : DBG mode �� sleep mode ��� ���� ����	
			
		else
		{
			if (WAKEUP_FAST_MODE == I_PORT_ON) 					// DIP SWITCH bit9  : bit9�� ON  �̸�  1sec  wakeup mode ����, No init wait(30sec)   	
				Porduct_Mode |= PM_OVER_CURR_MODE;
	       		else
		      		Porduct_Mode |= PM_SLEEP_MODE;					// default OFF : sleep mode  
		}	
			
		if (SLEEP_FAST_MODE == I_PORT_ON)							// bit9�� ON �̶� bit10�� ON �̸� sleep mode ���� 
		{
			Porduct_Mode |= PM_SLEEP_MODE;
			u8_SLEEP_TIME = 1;										// DIP SWITCH MSB(bit10)  : bit10�� ON  �̸�  sleep mode 1sec  ����     
		}	

		// booting �� �� ��ư ���� ó��  : DBG mode �� sleep mode ��� ���� ����	
		if (_IS_PM_DBG_MODE)  		
			zPrintf(1, "\n >>  DEBUG MODE  ..."); 

		//else if ((Porduct_Mode &  PM_SLEEP_MODE) && (~Porduct_Mode & PM_OVER_CURR_MODE))  {
		else if (Porduct_Mode & PM_SLEEP_MODE)  
		{
			#if _UARTDEBUG_MODE
			zPrintf(1,"getMode->DBG MODE\n");
			AppLib_DelayFor1ms();
			#endif

			HAL_PowerdownGpio1WakeupSourceSet(4, 1, 0);				// wakeup interrupt pin : p1.4 (pairing button)   
			HAL_PowerdownGpio3WakeupSourceSet(2, 1, 0);				// ���� ���忡 �� ���� overcurrent wakeup interrupt pin : p3.2    

			AppLib_DelayFor1ms();
			HAL_PowerdownMode1(POWER_DOWN_TIME_INIT, 0, 0, 0);		//  30 sec  : wiat for normal power 
			HAL_PowerdownGpio3WakeupSourceSet(2, 0, 0);				// ���� ���忡 �� ���� overcurrent wakeup interrupt pin : p3.2    
		}	  	
	#endif
}

// PM_UART_DBG_MODE : ���׺��� uart pin, sleep mode 
// PM_NOR_DBG_MODE : pairing button, wakeup mode 
void Init_FlashDefault(void)
{

	TxTimePeriod = TX_TIME_PERIOD;						//  �۽� �ֱ�  : 5ȸ(5sec �ֱ� �۽�) 
	//  WAKEUP_TX_PERIOD(dip seitch 2nd MSB): �׽�Ʈ�� ���� ������ġ ������ ���� Ȯ�� �� �׽�Ʈ�� ���� 1�� �������� ���� �ϵ��� ��� 
	if ((Porduct_Mode &  PM_SLEEP_MODE) || (Porduct_Mode &  PM_NOR_DBG_MODE) ||  (WAKEUP_FAST_MODE == 0) ) 
		TxTimePeriod = 1;								// sleep mode �� ��� TxTimePeriod �� �׻� 1�� �ؾߵ� 
		
	//--------------------------------------------------------------------------
	// option ���� ���� ��� �ɼ� ���� ����� ��� 

	if (_USE_OPTION_VALUE) {
		#if _UARTDEBUG_MODE
		zPrintf(1,"_USE OPTION VALUE is True\n");
		#endif
	
		if (gtPIB.Option.u8Option3 <= POWER_DOWN_TIME)	// 10��(1sec 10ȸ )
			TxTimePeriod = gtPIB.Option.u8Option3;
		if (gtPIB.Option.u8Option4 <= POWER_DOWN_TIME)	// 10��
			u8_SLEEP_TIME = gtPIB.Option.u8Option4;

		if (_IS_NO_SLEEP_MODE) {
			zPrintf(1," 0x80 :  Use Option Value");
			zPrintf(1,"(Tx Time  = %02d, Sleep Time = %02d)",(short)TxTimePeriod, (short)u8_SLEEP_TIME);
		}			
	}

	// Get PANID and RF Channel
	if (_IS_NO_DIP_SWITCH) {
		// dip seitch ������ �������� �Ҽ� �ֵ��� 
		gtPIB.u16PanID = (gtMIB.u16PanID & 0xFF00) | ((gtPIB.Option.u8Pid_Chan >> 4) & 0x0F); // pan id 
		gtPIB.u8Channel = (gtPIB.Option.u8Pid_Chan  & 0x0F) + 11;						   // channel 

		#if _EPAS_MODE		
		gtMIB.u16PanID = gtPIB.u16PanID;
		gtMIB.u16DstPanID=gtMIB.u16PanID;	
		gtMIB.u8Channel = gtPIB.u8Channel; 
		#endif
		
		Set_ChannelPanID();		// ����� pan id, channel ��� 
		
	}	
	
	#if _EPAS_MODE
		// Thermistor adc offset
		u16TempOffset = TEMP_OFFSET;	
		
		#if _UARTDEBUG_MODE
		zPrintf(1,"u16TempOffset = %6d\n",u16TempOffset);
		#endif
	#else
	if (gtPIB.Option.u16Option1 != 0xFF)
	{
		// Thermistor adc offset
		u16TempOffset = gtPIB.Option.u16Option1;

		#if _UARTDEBUG_MODE
		zPrintf(1,"u16TempOffset = %6d\n",u16TempOffset);
		#endif
	}
	
	#endif
	//--------------------------------------------------------------------------

	u8_SLEEP_TIME_Prev = u8_SLEEP_TIME;					// PM_OVER_CURR_MODE mode�� �ƴϸ� u8_SLEEP_TIME_Prev �� ��� ���� ����

	// sleep mode���� debug mode ���� : sleep time �� ���̱� ����   	
	if (Porduct_Mode & PM_UART_DBG_MODE)
		u8_SLEEP_TIME = POWER_DOWN_TIME_UART_DBG;	// default 10 -> 5��  

}	


///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Callback function called in PHY_ISR() when OOB(Out-Of-Band) code in a received packet is not 0.
///	\n	OOB code is contained in the reserved fields of IEEE 802.15.4 frame format. And, on the specification,
///		the fiedls should be 0. In other words, OOB is out of the specification. It means that a OOB packet are not 
///		compatible to the packet for IEEE 802.15.4 specification.
///	\n	If a received packet is processed in this function, the return value should be 1. Then, the packet is 
///		discarded in the PHY_ISR(). It prevents duplicated processing for the packet.
///	\n	Otherwise, the return value should be 0. It means that the packet is processed in PHY_ISR().
///
///	@param	u8RxFifoStartIdx	: Start address of the received packet including the length field in MAC RXFIFO.
///	@return	UINT8. Status of processing. 0=Not processed, 1=Processed.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 Callback_OobRxFromPhyISR(UINT8 u8RxFifoStartIdx)
{
	u8RxFifoStartIdx = 0;

	return	0;
}


///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Reports the HAL code(Higher 2bytes) and the return code(Lower 2bytes) if a error is occured when calling HAL
///	functions. This is called only in DEBUG mode.
///
///	@param	u16ErrorCode		: bit[31:16]=HAL code. bit[15:0]=Return code.
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
#ifdef  DEBUG
void assert_failed(UINT16 u16ErrorCode)
{
	// TODO:  The user can add his own implementation to report the error code. e.g. zPrintf().
#ifdef __MG2470_UART1_INCLUDE__
	if(ES1 && UART1_CLK_STATE)
	{
		zPrintf(1, "\n ## ErrorCode = 0x%04x ##", u16ErrorCode);
	}
#endif
}
#endif
 
  
UINT8 u8LedState = LED_PAIRED_STS;
// ----------------------------------------------------------------------------------------------------
// - �� ���� : red, blue blinking
//  - 1. Sleep mode,   2. wakeup mode
//   > No pair data : 1 = red ON,  2 = red OFF 
//   > pair data : 1 = red OFF,      2 = red ON 
//  > �۽� OK : 1,2 = blue blink
//  > �۽� Fail : 1 = blue blink, led blink,  2 = blue on 
//   > Normal :  1 = red = OFF, blue = OFF,  2 = red ON, blue = OFF 
// ----------------------------------------------------------------------------------------------------
 
// LedState �� ���� ���� LED_IDLE_STS ���� wait  
void LED_Process(void)
 {
 	if (Porduct_Mode &  PM_SLEEP_MODE) {
 		switch(u8LedState) {
 			case LED_IDLE_STS :
 				
 				break;
 			// led, led blinking   		
 			case LED_NO_PAIRED_STS :			// when pairing FAIL 
   				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					RED_LED = LED_ON;
					AppLib_VirtualTimerSet(LED_CONT_TIMER, TIME_100MS);
			  		u8LedState = LED_NO_PAIRED_STS1;
			  	}	
  				break;
  				
    			case LED_NO_PAIRED_STS1 :
   				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					RED_LED = LED_OFF;
					AppLib_VirtualTimerSet(LED_CONT_TIMER, TIME_1SEC);
				  	u8LedState = LED_NO_PAIRED_STS;
				  }	
  				break;
  				
   			case LED_PAIRING_STS :
   				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					BLUE_LED = LED_ON;
					RED_LED = LED_OFF;
					AppLib_VirtualTimerSet(LED_CONT_TIMER, TIME_500MS);
			  		u8LedState = LED_PAIRING_STS1;
			  	}	
  				break;
  				
    			case LED_PAIRING_STS1 :
   				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					BLUE_LED = LED_OFF;
					RED_LED = LED_ON;
					AppLib_VirtualTimerSet(LED_CONT_TIMER, TIME_500MS);
				  	u8LedState = LED_PAIRING_STS;
				  }	
  				break;
				
   			case LED_TX_STS :
				BLUE_LED = LED_ON;
				AppLib_VirtualTimerSet(LED_CONT_TIMER, 10);
			  	u8LedState = LED_IDLE_STS;
  				break;

    			case LED_TX_OK_STS :
    				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					BLUE_LED = LED_OFF;
				  	u8LedState = LED_IDLE_STS;
				 }	
  				break;
				
     			case LED_TX_FAIL_STS :
    				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					BLUE_LED = LED_OFF;
					RED_LED = LED_ON;		// sleep mode ���Խ� ��� led OFF �� ���� 
				  	u8LedState = LED_IDLE_STS;
				 }	
  				break;
 				
 			case LED_NORMAL_STS :
				BLUE_LED = LED_OFF;
				RED_LED = LED_OFF;
			
			  	u8LedState = LED_IDLE_STS;
 				break;
 		}
 	}
 	else {	
 		//  wakeup mode
 		switch(u8LedState) {
 			case LED_IDLE_STS :
 				
 				break;
 			// led, led blinking   		
 			case LED_NO_PAIRED_STS :			// when pairing FAIL 
   				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					RED_LED = LED_ON;
					AppLib_VirtualTimerSet(LED_CONT_TIMER, TIME_500MS);
			  		u8LedState = LED_NO_PAIRED_STS1;
			  	}	
  				break;
  				
    			case LED_NO_PAIRED_STS1 :
   				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					RED_LED = LED_OFF;
					AppLib_VirtualTimerSet(LED_CONT_TIMER, TIME_500MS);
				  	u8LedState = LED_NO_PAIRED_STS;
				  }	
  				break;
 			
  			case LED_PAIRED_STS :				// when pairing OK  
				RED_LED = LED_ON;
				BLUE_LED = LED_OFF;
				
			  	u8LedState = LED_NORMAL_STS;
  				break;
  			
  			// led, blue led blinking   				
   			case LED_PAIRING_STS :
   				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					BLUE_LED = LED_ON;
					RED_LED = LED_OFF;
					AppLib_VirtualTimerSet(LED_CONT_TIMER, TIME_500MS);
			  		u8LedState = LED_PAIRING_STS1;
			  	}	
  				break;
  				
    			case LED_PAIRING_STS1 :
   				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					BLUE_LED = LED_OFF;
					RED_LED = LED_ON;
					AppLib_VirtualTimerSet(LED_CONT_TIMER, TIME_500MS);
				  	u8LedState = LED_PAIRING_STS;
				  }	
  				break;
				
   			case LED_TX_STS :
				BLUE_LED = LED_ON;
				AppLib_VirtualTimerSet(LED_CONT_TIMER, 10);

			  	u8LedState = LED_IDLE_STS;
  				break;

    			case LED_TX_OK_STS :
    				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					BLUE_LED = LED_OFF;
				  	u8LedState = LED_IDLE_STS;
				 }	
  				break;
				
     			case LED_TX_FAIL_STS :
    				if (AppLib_VirtualTimerGet(LED_CONT_TIMER) == 0) {
					BLUE_LED = LED_ON;
					// RED_LED = LED_ON;			// pair ���¿��� �׻� ON 
						
				  	u8LedState = LED_IDLE_STS;
				 }	
  				break;
 				
 			case LED_NORMAL_STS : 
				BLUE_LED = LED_OFF;
				RED_LED = LED_ON;
			
			  	u8LedState = LED_IDLE_STS;
 				break;
 		}		
 	}		
  }	


  
///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Main function.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
#if 0
void main(void)
{
	HAL_McuPeriClockSet(CLOCK_MODE_16_MHz);

	//	TIMER0 : Turn on the clock and Set
	HAL_Timer0ClockOn(OFF);	// Org ON
	HAL_Timer0IntSet(DISABLE, PRIORITY_LOW, RUN, 2);	// Org ENABLE

	//  UART1 : Turn on the clock and Set
	#if _UARTDEBUG_MODE
	HAL_Uart1ClockOn(ON);
	HAL_Uart1IntSet(ENABLE, PRIORITY_LOW, 115200, UART_MODE_8N1);
	#endif

	// Kong - ���������� I2C Init --> Turn on the clock and Set
	#if _ACCEL_USE
	HAL_I2CClockOn(1);
	HAL_I2CIntSet(1,1,1,28);//20	
	#endif

	// Interrupt Enable
	#if _UARTDEBUG_MODE
	HAL_SystemIntSet(ENABLE);			// EA=1
	zPrintf(1, "\n\r>> Reset Init..\n");
	#endif

	// 	GPIO : Turn on the clock and Set
	// TODO: Set GPIOs to be used if needed.	
	HAL_GpioClockOn(OFF);

	HAL_Gpio0InOutSet(0, GPIO_INPUT, GPIO_PULL_UP);					// 0���� ������ ���� TEST MODE�� ���۵�. ���� ���..
	HAL_Gpio0InOutSet(1, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(2, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(3, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(4, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(5, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(6, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio0InOutSet(7, GPIO_INPUT, GPIO_PULL_UP);

	// P1.0 : booting �� ����� ��� üũ input pin���� ���
	HAL_Gpio1InOutSet(0, GPIO_INPUT, GPIO_PULL_UP);		//UART_RXD1 : UART_DBG_MODE_PIN  
	HAL_Gpio1InOutSet(1, GPIO_INPUT, GPIO_PULL_UP);		//UART_RXD1 : UART_DBG_MODE_PIN  
	HAL_Gpio1InOutSet(3, GPIO_OUTPUT, GPIO_DRIVE_4mA);	ACCELSEN_RESET = RST_OFF;	//TP5	
	HAL_Gpio1InOutSet(4, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio1InOutSet(6, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio1InOutSet(7, GPIO_INPUT, GPIO_PULL_UP);

	HAL_Gpio3InOutSet(0, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(1, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(2, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(3, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(4, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(5, GPIO_OUTPUT, GPIO_DRIVE_4mA);	GP35 = 1;		// NTC Thermostor Ref Line GND Connection..
	HAL_Gpio3InOutSet(6, GPIO_INPUT, GPIO_PULL_UP);
	HAL_Gpio3InOutSet(7, GPIO_INPUT, GPIO_PULL_UP);

	if(xCHIPID == 0x70)
	{
		zPrintf(1, "\n Failed. This software only supports CHIP ID over 0x70 !!!");
		while(SYS_Uart1TxWorking());
		SYS_WdtReset(0);
	}

	init_mempool(&malloc_mem, sizeof(malloc_mem));
	Init_HAL_Modem();
	//Init_GetMode();
	Init_MIB();							// RF Init..
	Init_GetPIB();
	//Init_FlashDefault();
	Security_Material();				// Security Init  	

	SYS_AdcSet(ENABLE, 0, ADC_CHAN_3); 	

	while(1)
	{
		// RF Init....
		// NT Thermistor Read
		// RF Send
		// Sleep (Power Down Mode2)

		#if _UARTDEBUG_MODE
		zPrintf(1,"getMode->PWDown2\n");
		AppLib_DelayFor1ms();
		#endif
		HAL_PowerdownMode2(3, 0, 0, 0);		//  MODE2�� ��� SW Reset ��..
		
	}
}


#endif

#if 1
void main(void)
{
	HAL_McuPeriClockSet(gu8McuPeriClockMode);

	//	Watchdog Disable
	SYS_WdtSet(0);
	
	Init_SysConfig();
	#if ( _EPAS_MODE == 0 )
	init_mempool(&malloc_mem, sizeof(malloc_mem));
	#endif

	#if !_EPAS_MODE
	SYS_WdtSet(2000);
	#endif

	Init_HAL_Modem();					// power down ���� �ʱ�ȭ�� �̷�� ���� �� 
	Init_GetMode();					// ����� ��尡 �ƴϸ� Powerdown  :  init 30 sec
	
	//Init_PER(&tPER);		     			// Not used 
	//Init_VerDisplay();
	Init_MIB();							// RF Init..
	//Init_GetPIB();
	
	Init_FlashDefault();
	
	Security_Material();					// Security Init  	
	
	// T1 	
	Init_RF_HAL();							// T1
	Init_Sensor();

	Sensor_Mode = _SENSOR_PACKET_TXMODE;	// Packet RF ��¸�� 0 , Packet UART ��¸�� 1
	//-------------------------------------------------------------------------
	while(1)
	{
		#if !_EPAS_MODE
		SYS_WdtSet(10000);
		#else
		SYS_WdtSet(0);
		#endif

		#if (_POWERDOWN_USE==0)
		AppLib_Delay(1);
		#endif

		//----------------------------------------------------------------
		// PLL can be unlocked when the temperature is changed rapidly within short time.
		// Then, re-locking is required.
		HAL_LockStateCheck();				// T1 	
		//----------------------------------------------------------------
		
		SensorReadingProcess();
		TX_Wait_Process();
		App_RF_TxProcess();

		// Kong.sh 191225 --> �ϱ� 3�� �Լ��� �������� �����ÿ� �� 5~6�� Loop ������ ���ߴ� ������ �߻�.. Why?
		#if _EPAS_MODE
		//--- No Function
		#else
		ZAPP_CommandInterface();			// UART cmd ó��
		ZAPP_PairingProcess();
		LED_Process();
		#endif
	}
}
#endif 

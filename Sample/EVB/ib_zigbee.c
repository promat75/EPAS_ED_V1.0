
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2011.
	- Date		: 2016-10-10
	- Version		: V2.6

	- File			: ib_zigbee.c
*******************************************************************************/

#include <stdlib.h>

#include "ib_main_sen.h"
#include "ib_zigbee.h"


UINT8 	u8AppRFSq;
UINT8	u8Sendstatus;

UINT8 	Retry_count=0;
UINT16 	u16RfPacketCount=0, u16RfPacketCountH=0;
UINT16	u16NoAckCount=0;
UINT16	u16DelayTime = 0;


UINT8	u8Task_pairing = 0;
UINT8 	u8AppPairCount = 0;
UINT8	u8AutoPairSrcID = 0;

UINT8 	TX_WaitState = TX_WAIT_IDLE;			// �ٸ� �Լ������� ���� ���� �ʵ��� , extern �� ������ ���� !!!! 

UINT8	TxTimePeriod;

PairTable 	gtPIB;



//----------------------------------------------------------------------------------------------
// Pairing ���� 

//-- RETURN
//	1	: Success. HIB is valid.
//	0	: Failed. HIB is not valid.
UINT8 Pair_PIB_COPY(UINT8 *pBuf)
{
	UINT8	i;
	UINT8	code *pCodeBuf;
	UINT8	SUM;

	pCodeBuf = CODE_ADDRESS_USER0;
	SUM = 0;
	for(i=0 ; i< sizeof(gtPIB); i++)		// sizeof(gtPIB) �Լ�  : ���ϴ� ����� �������� ���� ??? => ������ ������ size�� ���� ���Ƿ� ���� ��ġ�ҵ�
	{
		SUM += pCodeBuf[i];
		pBuf[i] = pCodeBuf[i];
		//xU1_THR = pCodeBuf[i];
	}
	
	if(SUM == 0)	return	1;
	else			return	0;	
}


void Pair_PIB_SAVE()
{
	UINT8 ib;
	UINT8 *pu8Buf;

	pu8Buf = (UINT8*)&gtPIB;

	gtPIB.u8Sum = 0;
	for(ib = 0; ib < (sizeof(gtPIB)- 1); ib++)
	{
		gtPIB.u8Sum += pu8Buf[ib];	
	}

	gtPIB.u8Sum = (~gtPIB.u8Sum) + 1;

	HAL_FlashWriteUnprotected();
	HAL_FlashPageWrite(CODE_ADDRESS_USER0, (UINT8 *)&gtPIB, 0);
	HAL_FlashWriteProtected();

	#ifdef __DEBUG
	zPrintf(1, "PairTable saved \r");					// ? �� ��µǸ� ����
	#endif
}


//Table save
// �ڵ������ Broadcast_Response() ���� u8Num �� 1�� set �Ͽ� ����
UINT8 Pair_Table_Save(UINT8 u8SenID, UINT8 *pu8DstIEEEAddr)
{
	UINT8	u8Status = 0;
	
	if (gtPIB.PairInfo.SrcID == AUTO_PAIR_ID) {	// button �� ���� auto pairing
		if((u8SenID > 0x00) && (u8SenID <= MAX_END_DEVICE))
			gtPIB.PairInfo.SrcID = u8SenID;
	}
		
	if (gtPIB.PairInfo.SrcID == u8SenID) {
		u8Task_pairing = PAIRING_OK;
		gtPIB.uPairCount = 1;
		//gtPIB.Option.u8ProductMode = Porduct_Mode & PM_SAVE_MASK;	// ���� 4��Ʈ�� ����  
		gtPIB.u8Channel = gtMIB.u8Channel;
		gtPIB.u16PanID = gtMIB.u16PanID;
	
		rpmemcpy(&gtPIB.PairInfo.au8IEEEaddr[0],&pu8DstIEEEAddr[0],8);
		Pair_PIB_SAVE();
			
		// temp for test
		Pair_Display_table();
		u8Status = 1;
	}
	else
		zPrintf(1, "\n Fail !   Responsed Sensor ID = %02d ",(short)u8SenID);		
		
	return 	u8Status;
}


// table debug
void Pair_Display_table(void)
{
	UINT8	iw;
	UINT8*	ptu8;
	
	if (_IS_NO_SLEEP_MODE == 0) return;
		
	zPrintf(1,"\r [Pair Info]");
	//zPrintf(1,"\r Paired = %02x",(short)gtPIB.uPairCount);
	zPrintf(1,"\r Pair SRC ID = 0x%02x, ",(short)gtPIB.PairInfo.SrcID);
	zPrintf(1,"   PAN ID = 0x%02x,",(short)gtPIB.u16PanID);
	zPrintf(1,"   Channel = %02d",(short)gtPIB.u8Channel);
	
	zPrintf(1,"\r Pair DstIEEE = ");
	
	for(iw = 0 ; iw < 8 ; iw++)
	{
		zPrintf(1," 0x%02x",(short)gtPIB.PairInfo.au8IEEEaddr[7-iw]);
	}
	
	zPrintf(1, "\n Sensor ID Addr=0x%02x (Procuct  Mode : 0x%02x) ",(short)gtPIB.PairInfo.SrcID, (short)Porduct_Mode);		


	Display_MIB(&gtMIB);
	
	// option value 
	ptu8 = (UINT8*)&gtPIB.Option;
	zPrintf(1,"\r Option Value : ");
	for(iw = 0 ; iw < 8 ; iw++)
	{
		zPrintf(1," 0x%02x ",(short)*ptu8++);
	}
	
	zPrintf(1,"\r ");
	if (_IS_NO_DIP_SWITCH)
		zPrintf(1,"0x40 : No DIP Switch, ");
	
}

	
void Pairing_Init(void)
{

	zPrintf(1, "\n Pairing Start");
	gtPIB.uPairCount = 0;
	u8AppPairCount = 0;
	
	// �� ���۽� red, blue ���� ������ 	
        AppLib_VirtualTimerSet(COMON_TIMER, PAIRING_LED_STANBY_TIME);		// 5000ms
 
   	u8Task_pairing = PAIRING_STANBY;
	u8LedState = LED_PAIRING_STS;

}	


//==============================================================================
//	Function : PAIRING_OK - 
//==============================================================================

void Pairing_OK(void)
{
	u8LedState = LED_PAIRED_STS;
	u8Task_pairing = PAIRING_EXIT;
	
	AppLib_VirtualTimerSet(SQ_WAIT_TIMER, TIME_1SEC);	// TxTick	1000 : 1000ms
	xRTINT1=1;	
}


//==============================================================================
//	Function : PAIRING_FAIL - 
//==============================================================================
void Pairing_Fail(void)
{
	
	zPrintf(1, "\r  Pairing failed, Try Restart  Power !!"); 
	u8LedState = LED_NO_PAIRED_STS;
	u8Task_pairing = PAIRING_EXIT;
	
	// u8AppRFSq = SQ_WAIT;
	
}


//==============================================================================
//	Function : PAIRING_RETRY
//==============================================================================
void Pairing_Retry(void)
{
	if (AppLib_VirtualTimerGet(COMON_TIMER) == 0)	// �� �õ� ��ƾ : 0,5s�� �ѹ��� ��� �� ����Ÿ�� ���� �ϰ� ����
	{
		u8Task_pairing = PAIRING_SEND;		
		u8AppPairCount++;		
	}
	
	// 10ȸ 5�� ���� �� ������ ���н� ��� ����(���� ����)
	if (u8AppPairCount >= PAIR_RETRY_COUNT) {
		u8Task_pairing = PAIRING_FAIL;

	}	
	
}

//==============================================================================
//	Function : PAIRING_SEND
//==============================================================================
void Pairing_Send(void)
{
	
	SYS_Uart1Put('+');
	
	Broadcast_Request();
	AppLib_VirtualTimerSet(COMON_TIMER, PAIRING_RETRY_TIME);

	u8Task_pairing = PAIRING_RETRY;
}

//==============================================================================
//	Function : PAIRING_START
//==============================================================================
void Pairing_Start(void)
{
	 if(PAIRING_BUTTON)
	{
    		u8Task_pairing = PAIRING_EXIT;
	}
    	else if(AppLib_VirtualTimerGet(COMON_TIMER) == 0)
    	{	
     		Pairing_Init();

    		// ��ư���� �� �õ��� ID�� �ڵ� ID �Ҵ� ������ ����
		if ( gtMIB.au8IEEEAddr[0] > 0 && gtMIB.au8IEEEAddr[0] <= MAX_END_DEVICE) {
			gtPIB.PairInfo.SrcID = gtMIB.au8IEEEAddr[0] ;
			u8AutoPairSrcID = 0; 		
		}	
		else 	{
    			u8AutoPairSrcID = AUTO_PAIR_ID;
	    		gtPIB.PairInfo.SrcID = u8AutoPairSrcID;
	    	}	
    	}
}


//==============================================================================
//	Function : PAIRING_CHECK
//==============================================================================
void Pairing_Check(void)
{
    	if(!PAIRING_BUTTON)
    	{
        	AppLib_VirtualTimerSet(COMON_TIMER, PAIRING_BUTTON_CHECK_TIME);		// 3sec
        	u8Task_pairing = PAIRING_START;
    	}
	else
	{
        	u8Task_pairing = PAIRING_EXIT;
	}
}


//==============================================================================
//	Function : Pairing Countinuate
//==============================================================================
void ZAPP_PairingProcess(void)
{
	// extern UINT8 	TX_WaitState;
	
	switch(u8Task_pairing)
	{
		case PAIRING_WAIT:	// PAIRING_STEP0:
			//PairingSeq0();
   			 if(!PAIRING_BUTTON) {
        			u8Task_pairing = PAIRING_CHECK;
        			TX_WaitState = TX_WAIT_IDLE;
        			u8AppRFSq = SQ_PAIR_WAIT_ST;
  			}        			
			break;
		case PAIRING_CHECK: // PAIRING_STEP1:
			Pairing_Check();
			break;
		case PAIRING_START: // PAIRING_STEP2:
			Pairing_Start();
			break;
		case PAIRING_STANBY: // 
			// LED ǥ�÷� �� ������ �˸��� ���� 5�� ���� ��ٸ��� �� ����
		 	if(AppLib_VirtualTimerGet(COMON_TIMER) == 0) {
		 		u8Task_pairing = PAIRING_SEND;
 			}	
			break;
		case PAIRING_SEND: // PAIRING_STEP3:
			Pairing_Send();
			break;
		case PAIRING_RETRY: // PAIRING_STEP4
			Pairing_Retry();
			break;
		case PAIRING_FAIL: // PAIRING_STEP5
			Pairing_Fail();
			break;
		case PAIRING_OK:
			Pairing_OK();
			break;
		case 	PAIRING_EXIT :
        		if ( u8LedState == LED_PAIRED_STS)
        			u8AppRFSq = SQ_RF_TX_INIT;		// �� ���а� �ƴϸ� �� ���ͽ� rf sq �� ó�� ���� ���� �ϵ��� 
			
			u8Task_pairing = PAIRING_WAIT;		
			break;
		default:
			u8Task_pairing = PAIRING_WAIT;
			break;
	}	
	
}



void PER_RespProcess(void)
{
	UINT8	u8Data[30];
	UINT8	u8status;


	BLUE_LED ^= 1; //BLUE LED toggle : 100ms				
	PER_Packet(u8Data);		
	
	u8status = App_Data_Transmit(RF_PER_PACKET_LENGTH, u8Data);//App_Data_Transmit(20,u8Data);
	
	if (u16RfPacketCount == PER_PACKET_COUNT - 1){		// recevied END command
		u8AppRFSq = SQ_RF_TX_INIT;
		u16RfPacketCount = 0;
		SYS_Uart1Put('>');
	}	
	else	{
		SYS_Uart1Put('*');
		u8AppRFSq = SQ_WAIT;				// 1ȸ ���� 
		TX_WaitState = TX_WAIT_IDLE;
	}		

}	


void SetTxInitWaitTime(void)
{
	UINT16	init_wait_time;

	init_wait_time = App_Get2ByteRandWait();
	init_wait_time %=100;
	init_wait_time += TIME_INIT_TX_DELAY;	
	 			 	
	zPrintf(1, "\n\n  Init Wait[ %d msec ]... \n", init_wait_time);	
	AppLib_VirtualTimerSet(SQ_WAIT_TIMER, init_wait_time);	
}


void SleepStart(void)
{
	BLUE_LED = LED_OFF;
	RED_LED = LED_OFF;
	
	#if _POWERDOWN_USE
		#if _UARTDEBUG_MODE
		zPrintf(1,"Sleep Start()\n");
		#endif
		AppLib_Delay(3);
		//HAL_PowerdownGpio1WakeupSourceSet(4, 1, 0);		// button wakeup ����  
		#if (_POWERDOWN_MODE==1)
		HAL_PowerdownMode1(u8_SLEEP_TIME, 0, 0, 0); 		// sec timer 
		#elif (_POWERDOWN_MODE==2)
		HAL_PowerdownMode2(u8_SLEEP_TIME, 0, 0, 0); 		// sec timer 
		#endif
	#endif

	TX_WaitState = TX_WAKEUP_STS;
}	

// sleep mode ���¿��� �����  :  ���� wakeup, button wakeup 
void WakeupSet(void)
{
	HAL_RFAnalogSleep(POWER_RF_SLEEP_ADC_ON);	

	#if _EPAS_MODE
		SYS_AdcSet(ENABLE, 0, ADC_CHAN_3);				// !! sleep mode �����ϰ� adc set �� ������ ������ ���� �о� ���� ����
	#else
		SYS_AdcSet(ENABLE, 0, ADC_CHAN_0);				// !! sleep mode �����ϰ� adc set �� ������ ������ ���� �о� ���� ����
	#endif

    	
	// pair burron �� ���� wakeup ó���� �� �������� �Ѿ� ������ 
	if(PAIRING_BUTTON == BTN_ON)  {
		u8AppRFSq = SQ_PAIR_WAIT_ST;				// pairing�� �� ���¿�����  ���� �ϵ��� 
		TX_WaitState = TX_WAIT_IDLE;
		HAL_RFAnalogWakeup();
 	}
 	else			// if (TX_WaitState != SQ_PAIR_WAIT_ST)			// pairing ���°�  �� ������ ���� ���  
 		TX_WaitState = TX_WAIT_INIT;
 	
}

// �ѹ� sleep mode ���۽ô� �ּ� 5ȸ(5��) �̻� ȸ�� ��ȣ�� ���� �ɶ� ���� 
UINT8 u8RecoveryCount = 0;
 
void CheckOverCurrent(void)
{
	// Overcurrent ����   	
	if (OVER_CURRENT_CHECK == I_PORT_OFF) {
		if (u8RecoveryCount++ >= 5) {
	
			//zPrintf(1, "SLEEP_TIME = %d  %d ", (short)u8_SLEEP_TIME, (short)u8_SLEEP_TIME_Prev );
			CLR_OVER_CURR_STS();
	
			//Porduct_Mode &= ~PM_OVER_CURR_MODE;
			u8_SLEEP_TIME = u8_SLEEP_TIME_Prev;
		}	
	}
	else
		u8RecoveryCount = 0;

}

void InitSleepWait(void)
{
	#if _POWERDOWN_USE
		//HAL_PowerdownGpio1WakeupSourceSet(4, 1, 0);		// wakeup interrupt pin : p1.4 (pairing button)   

		//zPrintf(1,"Init Sleep Wait()\n");
		//#if (_POWERDOWN_MODE==1)
		//HAL_PowerdownMode1(POWER_DOWN_TIME_INIT, 0, 0, 0);					//  5 sec  : wiat for normal power 
		//#elif (_POWERDOWN_MODE==2)
		//HAL_PowerdownMode2(POWER_DOWN_TIME_INIT, 0, 0, 0);					//  5 sec  : wiat for normal power 
		//#endif
	#endif
		
	Init_SetHAL();				
	Init_ExtAmp();			
	
	SleepStart();
	WakeupSet();
}
	
void RF_Init_Wait(void)
{

	if (Porduct_Mode & PM_SLEEP_MODE) {
		// No wait Sleep mode  
		if ((Porduct_Mode & PM_OVER_CURR_MODE) ||  (Porduct_Mode & PM_UART_DBG_MODE)) {
			Init_SetHAL();				
			Init_ExtAmp();			
		}
		else	
			// Sleep mode Normal ���� : sleep wait 
			InitSleepWait();
	}	
	else {	
		
		// PM_UART_DBG_MODE �� PM_SLEEP_MODE �� ������ : �Ʒ� ��ƾ�� ���� ���� ����  !!! 
		// if (Porduct_Mode & PM_UART_DBG_MODE){
		//	Init_SetHAL();				
		//	Init_ExtAmp();			
		//}    
		
		SetTxInitWaitTime();
		
		// init wait before TX 
		while (AppLib_VirtualTimerGet(SQ_WAIT_TIMER) == 0);
	
		zPrintf(1, "\n -- START -- " );	
	}
		
}	

void SetTxWaitStatus(void)
{
	if (Porduct_Mode &  PM_SLEEP_MODE )  
		TX_WaitState = TX_SLEEP_SEN_READ_STS;		// sleep mode tx   
	else
		TX_WaitState = TX_WAIT_SEN_END_READ_STS;	// NO sleep moe tx   
}


// RF_TX Process() �տ���  �۽� �ֱ⸦ ���� 
void TX_Wait_Process(void)
{
	static  UINT8 	TxTimeCount = 0;
	extern UINT16	u16DelayTime;

	switch(TX_WaitState) {
		case TX_WAIT_IDLE :
			break;
			
		//---------------------------------------------------------------------------------------
		// wakeup mode wait  
		//---------------------------------------------------------------------------------------
		case TX_WAIT_INIT :
			//AppLib_VirtualTimerSet(SQ_WAIT_TIMER,  SEN_READ_TIME);	 
			SenReadState = SEN_READ_WAIT;
			TX_WaitState = TX_WAIT_SEN_READ_STS;
			
			//if (Porduct_Mode &  PM_SLEEP_MODE )  
			//	TX_WaitState = TX_SLEEP_SEN_READ_STS;
			//else
			//	TX_WaitState = TX_WAIT_SEN_READ_STS;
				
			break;
		case TX_WAIT_SEN_READ_STS :
			// wait for end of sensor reading status : SEN_CALC_DATA
			
			break;
		case TX_WAIT_SEN_END_READ_STS : 	
			// sensor reading wiat  : �־��� �ð� ���� sensor reading �۾��� �Ϸ� �ɶ����� ��ٸ�  
			//if (AppLib_VirtualTimerGet(SQ_WAIT_TIMER) == 0) 	{  
				TX_WaitState = TX_WAIT_STS;
				AppLib_VirtualTimerSet(SQ_WAIT_TIMER,  u16_TX_TIME+u16DelayTime);	 
			//}    	
			break;
		
		case TX_WAIT_STS : 									// TX time wait   : �۽� �ֱ� �ּ� 1�� �Ǵ� ���̻��� ��ٸ� 				
			if (AppLib_VirtualTimerGet(SQ_WAIT_TIMER) == 0) {	// 1sec �� ��ٸ� :    1sec ����  SEN_READ_TIME �� ������ ���� �ϵ��� 
				if (++TxTimeCount >= TxTimePeriod) {			// data �۽� �ֱ� üũ  
					TxTimeCount = 0;
					u8AppRFSq = SQ_RF_TX_START;	
					TX_WaitState = TX_WAIT_TIME_SET_STS;
				}
				else
					TX_WaitState = TX_WAIT_INIT;
			}
				
			break;
		case TX_WAIT_TIME_SET_STS :							// �۽��� ����� ��ٸ� 
			// send status üũ ???? 
			if (u8AppRFSq == SQ_WAIT) {						// check send finished 
				SetTxWaitTimer();
				TX_WaitState = TX_WAIT_INIT;
			}		
			break;
	
		//---------------------------------------------------------------------------------------
		// Sleep mode wait  : ���� rading time ������ wakeup��  ���� ��ƾ ��� 
		//---------------------------------------------------------------------------------------
		case TX_SLEEP_SEN_READ_STS :
			// sensor reading wiat : SQ_WAIT_TIMER check �� �ʿ� ����, ���� ����Ÿ ó��(SEN_CALC_DATA) �Ϸ��� ���⼭ ���� �ϵ��� �Ǿ� ����
			//if (AppLib_VirtualTimerGet(SQ_WAIT_TIMER) == 0) { 	// SEN_READ_TIMER  :  senor data �� �б� ���� �־��� �ð� ( tx time �ֱ��� 1sec ���� �۾ƾߵ�) 
				u8AppRFSq = SQ_RF_TX_START;				// tx send state 
				TX_WaitState = TX_SLEEP_STS;
				
				HAL_RFAnalogWakeup();						
				AppLib_VirtualTimerSet(SQ_WAIT_TIMER,  5);	 	//  wait for tx complete before  sleeping 
			//}	
			break;

		case TX_SLEEP_STS :
			if (AppLib_VirtualTimerGet(SQ_WAIT_TIMER) == 0) {	//  wait for tx completed   
				if (Porduct_Mode & PM_OVER_CURR_MODE) 	CheckOverCurrent();
				SleepStart();									// Kong - Sleep mode ����
				SetTxWaitTimer();							// Kong - Sleep mode ��  wakeup �Ŀ� �۽��� ��� ó�� (�۽ſ����� ��� �̼� delay �߰� ) 
			}
			
			break;
 
		case TX_WAKEUP_STS :
			WakeupSet();									// SleepStart()���� wakeup �� ����   
			
			break;
		//--------------------------------------------------------------------------------------- 
	
		default :
			break;
		
	}
}	


#define OVER_CURR_STAY_TIME				300		// 300ms
#define OVER_CURR_CHK_WAIT_TIME			1000	// 1000ms

void App_RF_TxProcess(void)
{
	UINT8	u8Data[30];
	
	switch (u8AppRFSq)
	{		
		case SQ_WAIT:

			break;
		case SQ_INIT_OVER_CHK:
			
			if (AppLib_VirtualTimerGet(SQ_WAIT_TIMER) == 0) 			// ���� 200ms ���� wiat : Over Current ��ȣ �߻� ���θ� ��ٸ�  
				u8AppRFSq = SQ_RF_TX_INIT;
			
			// 100ms �ȿ� �ѹ��̶� OVER_CURRENT_CHECK �߻��� 1 ~2�� ���� ��� ���� ���� üũ ����
			if (OVER_CURRENT_CHECK == I_PORT_ON) {
				u8AppRFSq = SQ_INIT_OVER_WAIT;
				AppLib_VirtualTimerSet(SQ_WAIT_TIMER,  OVER_CURR_STAY_TIME);	 	// 300ms ���� over current ���¸�  ���� �ؾߵ� 
				AppLib_VirtualTimerSet(COMON_TIMER,  OVER_CURR_CHK_WAIT_TIME);	// 1�� ���� over current ���� üũ 
			}
			break;
			
		case SQ_INIT_OVER_WAIT:
			if (OVER_CURRENT_CHECK == I_PORT_ON) {
				if (AppLib_VirtualTimerGet(SQ_WAIT_TIMER) == 0) {	
					//Porduct_Mode |= PM_OVER_CURR_MODE;  		// over current ���¸� 2�� ��ٸ��� ���� 400ms ���� ���� : PM_OVER_CURR_MODE 
					SET_OVER_CURR_STS();						// // over current ���¸� 1�� ��ٸ��� ���� 300ms ���� ���� : PM_OVER_CURR_MODE 
					u8_SLEEP_TIME = 1;							// overcurrent �����̸� 1�ʿ� �ѹ��� ���� �ϵ��� ����   
					u8AppRFSq = SQ_RF_TX_INIT;					// PM_OVER_CURR_MODE�� PM_UART_DBG_MODE �� ���� ������ ���� 
				}	
			}		 
			else	
				AppLib_VirtualTimerSet(SQ_WAIT_TIMER,  OVER_CURR_STAY_TIME);	 	// over current ���¸� 2�� ��ٸ��� ���� 400ms ���� ���� ���� ���ϸ� reset 
			
			if (AppLib_VirtualTimerGet(COMON_TIMER) == 0) 	
				u8AppRFSq = SQ_RF_TX_INIT;

			break;
		
		case SQ_RF_TX_INIT :	
			RF_Init_Wait();
			
			#if ( _EPAS_MODE == 0 )
			GetSensor_TempData();	// reset temp sensor  by init reading : �ʱ� �µ����� ������ �� �о� ���� ���� ���� ���� �߰�  
			#endif

			//SenReadState = SEN_READ_WAIT;
			u8AppRFSq = SQ_WAIT;			
			TX_WaitState = TX_WAIT_INIT;					// start sensor read timer 
		
			break;
		
		case SQ_RF_TX_START:
			if (UserCtrl_Packet(u8Data))					//  make RF payload frame : data ���� üũ ������ ������ RF ������ ���� ����
				u8Sendstatus = 0xFF;						// RETURN_PHY_SUCCESS = 0x00  
			else	
				u8Sendstatus = SendDataPacket(u8Data);		//  RF, uart ascii, uart hex mode �� ����,  1ȸ(1��)�� �ѹ��� ����
			
			u8LedState = LED_TX_STS;
			u8AppRFSq = SQ_WAIT;					
			
			break;
		case SQ_PAIR_WAIT_ST :
			// �� ������ ������ 1ȸ �۽��� sleep mode �� ������  
			// �� �߿��� �ٸ� �۾��� ���� ���� 
			if ((u8Task_pairing == PAIRING_WAIT) &&  (gtPIB.uPairCount)) {	// �� ���нÿ��� ��� ���  
				u8AppRFSq = SQ_RF_TX_INIT;				// �� ���������� ������ �ʱ� ���� ���� 
			}	
				
			break;

		case SQ_RF_TX_PER:
			PER_RespProcess();
		
			break;	
							
		default :
			break;			
	}

	//----------------------------------------------------------------------------------------
	// ���� ���� ����Ÿ ó�� ��ƾ
	App_RF_RxProcess();
	//----------------------------------------------------------------------------------------
	
}

// �۽� ������ delay �߰� : ������ ���� ����  	
void SetTxWaitTimer(void)
{

	if (u16RfPacketCount++ == 0xffff)	u16RfPacketCountH++;	// packet count��  4����Ʈ ����ϱ� ���� 
	u8TempSenErrCount++;								// ���ܼ� ���� read error count : normal read clear 
	
	if (u8Sendstatus == RETURN_PHY_SUCCESS)
	{
		u8LedState = LED_TX_OK_STS;
		if (Porduct_Mode &  PM_SLEEP_MODE) 	SYS_Uart1Put('~');
		else								SYS_Uart1Put('.');
	}
	else
	{
		u8LedState = LED_TX_FAIL_STS;
		
		// ���� 1���� ������ �ð��� ���� 
		u16DelayTime = rand() & TX_RANDTIME_MASK;		// rand() : 2 byte int type randum number
		u16NoAckCount++;
	}
	
	AppLib_VirtualTimerSet(SQ_WAIT_TIMER, u16_TX_TIME + u16DelayTime);	//  �۽� delay �� �۽� error �� �߻� ���� ������ ���� ���� ��� ��� 
		
}


// ���� ����(RX) ����Ÿ ó��  
// STX CMD LEN   |   cmd sen_id dat2 dat3 dat4   |   chk  ETX  
// A0    A0    07         03      0       0      0      0          xx    AF
// CMD : CMD_PAIRING_RESPONSE(0xF1), CMD_DATA_REQUEST(0xA0), CMD_DATA_ALLRESET(0xB0) 
// cmd  : per _test(0x03)
// PER test cmd format : A0 A0 05 03 id 0 0 0 chk AF  

UINT8	u8PlainLen;

void App_RF_RxProcess(void)
{
	tPACKET_INFO*			ptRxPacket;
	tMAC_INFO				tMacInfo;

	tRF_RT_PACKET_FORMAT* 	ptMsgBody;
	tDATA_MESSAGE*			ptMsgFormat;

	UINT8	u8Status;
	//UINT16	iw;
		
	ptRxPacket = SYS_RxQueueCheck();
	if(ptRxPacket)		// A packet is received
	{
		u8Status = SYS_RxQueueParse(ptRxPacket, &tMacInfo);
		if(!u8Status)
		{
			//u8LedCounter_Rx = (u8LedCounter_Rx + 1) & 0x07;
			//1 Security Processing
			u8Status = HAL_SecDecryptionCCM32(tMacInfo.pu8MacPayload, tMacInfo.u8MacPayLen, u32FrameCounter, u8KeySeqNum, &u8PlainLen);					
			tMacInfo.u8MacPayLen = u8PlainLen;
			
			//SYS_Uart1Put(tMacInfo.pu8MacPayload[4]);
			//SYS_Uart1Put(tMacInfo.pu8MacPayload[5]);
				
			ptMsgBody = (tRF_RT_PACKET_FORMAT*)tMacInfo.pu8MacPayload;
			ptMsgFormat =  (tDATA_MESSAGE*)ptMsgBody->u8Data;				
			
			// **************************
			// Test code
			//Display_Buffer(tMacInfo.pu8MacPayload, ptMsgBody->u8Size+PKT_HEADER_TAIL_LENGTH, ptMsgBody->u8Size+PKT_HEADER_TAIL_LENGTH);
			//zPrintf(1, "\n ptMsgBody->u8CMD = %x,   sen id = %x", (short)ptMsgBody->u8CMD,  (short)ptMsgFormat->u8DataMsgBody.u8DataMsgSrcID);
			// **************************

			// �ڱ� ���� �� ��ɸ� ó�� �ϵ���
			if (ptMsgBody->u8STX == STX)  {
				
				// ���� ID ��ɸ� ����, pairing �϶��� ID �� �޶� ���� 
				if (     (ptMsgBody->u8CMD == CMD_PAIRING_RESPONSE)
				      ||  (ptMsgFormat->u8DataMsgBody.u8DataMsgSrcID == gtPIB.PairInfo.SrcID)
			 	      ||  (ptMsgFormat->u8DataMsgBody.u8DataMsgSrcID == 0xFF))	// 0xFF : all sensor ID 
			 	{ 
				
				switch(ptMsgBody->u8CMD)
				{
					case CMD_PAIRING_RESPONSE:		// CMD_PAIRING_RESPONSE : 0xF1
						BLUE_LED = LED_ON; 			
						RED_LED = LED_ON;
						
						//Display_Buffer(&ptMsgFormat->u8DataMsgBody.u8DataMsgPanID, ptMsgBody->u8Size+PKT_HEADER_TAIL_LENGTH, ptMsgBody->u8Size+PKT_HEADER_TAIL_LENGTH);
						//zPrintf(1, "\n ptMsgFormat->u8DataMsgCMD = %x ", (short)ptMsgFormat->u8DataMsgCMD); 
						
						
						if(ptMsgFormat->u8DataMsgCMD == GET_UART_CMD_PAIR) {
							gtPIB.PairInfo.SrcID = AUTO_PAIR_ID;			// Pair_Table_Save  ���� �ڵ� ó�� �ϱ� ���� 
						}	
						
						SetDestIEEEAddress(tMacInfo.uSrcAddr.au8Ext);
						Pair_Table_Save(ptMsgFormat->u8DataMsgBody.u8DataMsgSrcID, tMacInfo.uSrcAddr.au8Ext);
						
						
						break;
					case CMD_DATA_REQUEST:

						if(ptMsgFormat->u8DataMsgCMD == CMD_ID_RF_DATA_START)
						{
							zPrintf(1, "\n >> ----- START ----- ");
							zPrintf(1, "\n");
							// per test END comnand �� ���� ��� 
							u8AppRFSq = SQ_RF_TX_PER;				// SQ_RF_TX_PER ��ƾ���� status �� SQ_TX_DELAY ���� 
							u16RfPacketCount = PER_PACKET_COUNT - 1;	// ������ end cmd reponse 
						}
						else if(ptMsgFormat->u8DataMsgCMD == CMD_ID_RF_DATA_STOP)
						{
							zPrintf(1, "\n >> ----- STOP ----- ");
							zPrintf(1, "\n");
							u8AppRFSq = SQ_WAIT;
						}							
						else if(ptMsgFormat->u8DataMsgCMD == CMD_ID_RF_DATA_PER)	// PER Test Start
						{
							if(u8AppRFSq != SQ_WAIT) {	// ���� �ѹ��� ���
								zPrintf(1, "\n >> START PER Test ");
								u16RfPacketCount = 0;
							}	
							
							tPER.u8DisplayPktInfo = tMacInfo.i8RSSI;
							
							u8AppRFSq = SQ_RF_TX_PER;
							u16RfPacketCount++;
						}
						else if(ptMsgFormat->u8DataMsgCMD == CMD_ID_RF_DATA_RESTART)
						{
							zPrintf(1, "\n >> RESTART(�ٽý���...)");
							zPrintf(1, "\n");
							AppLib_Delay(10);
							xRTINT1=1;			
							SYS_WdtReset(0);
						}							
						else if(ptMsgFormat->u8DataMsgCMD == CMD_ID_RF_SEN_DEBUG)	// DEBUG cmd 
						{
							zPrintf(1, "\n >>  SENSOR DEBUG MODE START ");
							zPrintf(1, "\n >>  Changed  TxTimePeriod  = 1   ");
							TxTimePeriod = 1;
						}
						else if(ptMsgFormat->u8DataMsgCMD == CMD_ID_RF_SEN_OPTION)
						{	
							// 02 10 06  panid  chennel  srcid  op12345678  xx  03	
							// received option data
							if (u8AppRFSq == SQ_WAIT) {		// �۽� ������ ���� ���¿��� �� option ����� �޾� ���̵���
								rpmemcpy((UINT8*)&gtPIB.Option, (UINT8*)&ptMsgFormat->u8DataMsgBody.u8DataMsgDestID, OPTION_MESSAGE_LEN);
								SetOptionData();
							}	
							else {
								zPrintf(1, "\n >>  Stop!!  Sensor  Transmittion.");
							}	
						}														
						else if(ptMsgFormat->u8DataMsgCMD == CMD_ID_RF_SEN_EXT2)
						{
							
						}							
						break;
						
					case CMD_DATA_ALLRESET:		// CMD_ID_RF_DATA_RESTART ���� reset�� ����
						zPrintf(1, "\n >> RESET(�ٽý���...)");
						zPrintf(1, "\n");
						AppLib_Delay(10);
						xRTINT1=0;			
						SYS_WdtReset(0);
						break;																											
					default :
						break;
					
				} // end of switch
				}
			}
		}
		SYS_RxQueueClear();
	}
}


// ���ŵ� �ɼ� ����Ÿ�� ���� ������ ���� �� ���� 
void SetOptionData(void)
{
	
	if (gtPIB.Option.u8ProductMode == 0xFF) { 	// �ʱ�ȭ 
		#if (_USE_DIPSWITCH)	
		SetDipSwitch_ChannelPanID();
		#endif
		Init_FlashDefault();
		gtPIB.Option.u8Pid_Chan = 0xFF;
		gtPIB.Option.u8Option3 = 0xFF;		// tx time 
		gtPIB.Option.u8Option4 = 0xFF;		// sleep time 
		
		// ������ �ɼ��� ���� ��� ���� ���� 
	}	  
	else {
		//  option�� default ��(0xff) �̸� ���� ���� ���� �ʵ��� ��ƾ �߰� ???? 
	
		// Porduct_Mode |= gtPIB.Option.u8ProductMode & PM_SAVE_MASK;    
		gtPIB.u16PanID = (gtMIB.u16PanID & 0xFF00) | ((gtPIB.Option.u8Pid_Chan >> 4) & 0x0F); 	// pan id 
		gtPIB.u8Channel = (gtPIB.Option.u8Pid_Chan  & 0x0F) + 11;						// channel 
	}
	
	Pair_PIB_SAVE();
	Pair_Display_table();

}	

	
void SetDestIEEEAddress(UINT8 *address)
{

	rpmemcpy(gtMIB.au8DstIEEEAddr, address, 8);

	// copy short address
	gtMIB.u16DstShortAddr = gtMIB.au8DstIEEEAddr[1];
	gtMIB.u16DstShortAddr = gtMIB.u16DstShortAddr << 8 | gtMIB.au8DstIEEEAddr[0];

}

// ���� ���� ����Ʈ�� �״�� ���
void SetDestShortIEEEAddress(UINT8 *address)
{
	
	rpmemcpy(gtMIB.au8DstIEEEAddr, address, 2);
	
	// copy short address
	gtMIB.u16DstShortAddr = address[1];
	gtMIB.u16DstShortAddr |=(UINT16)address[0] << 8;
	
	
}

#if _USE_DIPSWITCH
void SetDipSwitch_ChannelPanID(void)
{
	UINT8 	u8pen_id = 0;
 	UINT16 	u16switch_in = 0;

	HAL_Gpio0InOutSet(0, GPIO_INPUT, GPIO_PULL_UP );
	AppLib_DelayFor1us();
	HAL_Gpio0InOutSet(1, GPIO_INPUT, GPIO_PULL_UP );
	AppLib_DelayFor1us();
	HAL_Gpio0InOutSet(2, GPIO_INPUT, GPIO_PULL_UP );
	AppLib_DelayFor1us();
	HAL_Gpio0InOutSet(3, GPIO_INPUT, GPIO_PULL_UP );
	AppLib_DelayFor1us();
	HAL_Gpio0InOutSet(4, GPIO_INPUT, GPIO_PULL_UP );
	AppLib_DelayFor1us();
	HAL_Gpio0InOutSet(5, GPIO_INPUT, GPIO_PULL_UP );
	AppLib_DelayFor1us();
	HAL_Gpio0InOutSet(6, GPIO_INPUT, GPIO_PULL_UP );
	AppLib_DelayFor1us();
	HAL_Gpio0InOutSet(7, GPIO_INPUT, GPIO_PULL_UP );
	AppLib_DelayFor1us();
	HAL_Gpio3InOutSet(3, GPIO_INPUT, GPIO_PULL_UP );
	AppLib_DelayFor1us();
 	HAL_Gpio3InOutSet(4, GPIO_INPUT, GPIO_PULL_UP );
	AppLib_DelayFor1us();

	#if (NEW_BOARD_DIP_SW)
	
	if (!GP00)    u16switch_in |= 1<<9;
	if (!GP01)    u16switch_in |= 1<<8;
	if (!GP02)    u16switch_in |= 1<<7;
	if (!GP03)    u16switch_in |= 1<<6;
	if (!GP04)    u16switch_in |= 1<<5;
	if (!GP05)    u16switch_in |= 1<<4;
	if (!GP06)    u16switch_in |= 1<<3;
	if (!GP07)    u16switch_in |= 1<<2;
	if (!GP33)    u16switch_in |= 1<<1;
	if (!GP34)    u16switch_in |= 1<<0;		
	
	#else
	
	if (!GP00)    u16switch_in |= 1<<0;
	if (!GP01)    u16switch_in |= 1<<1;
	if (!GP02)    u16switch_in |= 1<<2;
	if (!GP03)    u16switch_in |= 1<<3;
	if (!GP04)    u16switch_in |= 1<<4;
	if (!GP05)    u16switch_in |= 1<<5;
	if (!GP06)    u16switch_in |= 1<<6;
	if (!GP07)    u16switch_in |= 1<<7;
	if (!GP33)    u16switch_in |= 1<<8;
	if (!GP34)    u16switch_in |= 1<<9;		
	

	#endif
		
	gtPIB.u8Channel = (u16switch_in&0x000F) + 11;	// 0 = 11ch
	gtMIB.u8Channel = gtPIB.u8Channel;
	
	u8pen_id = (u16switch_in&0x03F0)>>4;
	gtMIB.u16PanID = (gtMIB.u16PanID & 0xFF00) | (u8pen_id & 0x0F);	// ���� 1����Ʈ�� ����� ���� �״�� ���
	gtMIB.u16DstPanID=gtMIB.u16PanID;		


	#if (NO_RPT_MODE)	
	// nothing  
	#else
	// pan id MSB(bit5)�� repeater ��� ���θ� ���� �ϴµ� ��� �ϵ��� : pan id �� 5bit(0x1f) �� ���
	if (u8pen_id & 0x20) 
		Porduct_Mode  |= PM_RPT_MODE;
	
	// repeater mode �϶��� ä���� dip switch �� �ٸ��� ���� ��Ŵ
	if (Porduct_Mode  & PM_RPT_MODE) {
		if (gtMIB.u8Channel >= 13) {
			gtMIB.u8Channel -= 2;
			zPrintf(1, "\n Repeater Mode : Current router Channel(DIP switch - 2)  = %02d",(short)gtMIB.u8Channel);	
		}	
		else
			zPrintf(1, "\n Error Channel Configuration (13 ~ 25)  ERROR !!!! : %02d",(short)gtMIB.u8Channel);	
	}	
	#endif
		
}

#endif


// dip switch�� ��� ���� ������
void Set_ChannelPanID(void)
{
	if (gtPIB.uPairCount==0 ) {
		// use default value
		gtPIB.u16PanID = DEFAULT_PANID;
		gtPIB.u8Channel = DEFAULT_CHANNEL;
	}
		
	gtMIB.u16PanID = gtPIB.u16PanID;
	gtMIB.u16DstPanID=gtMIB.u16PanID;	
	gtMIB.u8Channel = gtPIB.u8Channel; 
	
	#if (NO_RPT_MODE)	
	// nothing  
	#else
	// repeater mode �϶��� ä���� dip switch �� �ٸ��� ���� ��Ŵ  
	if (Porduct_Mode  & PM_RPT_MODE) {
		if (gtMIB.u8Channel >= 13) {
			gtMIB.u8Channel -= 2;
			zPrintf(1, "\n Current router Channel(DIP switch - 2)  = %02d",(short)gtMIB.u8Channel);	
		}	
		else
			zPrintf(1, "\n Error Channel Configuration (13 ~ 25)  ERROR !!!! : %02d",(short)gtMIB.u8Channel);	
	}	
	#endif
	
}	


UINT8	au8Key0[16];
UINT8	au8NonceExtAddr[8];
UINT32	u32FrameCounter;
UINT8	u8KeySeqNum;

void Security_Material(void)
{
	UINT8	iw=0;
	
	// Security Material
	for(iw=0 ; iw<16 ; iw++)	au8Key0[iw] = 0xA0 + iw;
	for(iw=0 ; iw<8 ; iw++)	au8NonceExtAddr[iw] = 0xB0 + iw;
	u32FrameCounter = 0x1DEA1B17;
	u8KeySeqNum = 0x1B;
	
	HAL_SecKey0Set(au8Key0);
	HAL_SecTxKeyChoice(SEC_KEY0);
	HAL_SecRxKeyChoice(SEC_KEY0);
	
	HAL_SecTxNonceExtAddrSet(au8NonceExtAddr);
	HAL_SecRxNonceExtAddrSet(au8NonceExtAddr);	

}



UINT8 App_GetRandSeed(UINT8 u8Min, UINT8 u8Offset)
{
	UINT8 u8RetSeed;

        u8RetSeed = rand() & u8Offset;
        u8RetSeed = u8RetSeed + u8Min;
           
        return u8RetSeed; /* Min = u8Min,  Max = u8Min + u16Offset */
}


UINT16  App_Get2ByteRandWait(void)
{

        UINT8   i;
        UINT8   u8RandCnt = 0;
        UINT16 u16RandID = 0;

        for(i = 0; i < 8; i++)
        {
        	u8RandCnt ^= gtMIB.au8IEEEAddr[i];
        }          

        u16RandID = (UINT16)App_GetRandSeed(0x01, 0xfe);		   
        u16RandID ^= (UINT16)u8RandCnt & 0x00ff;
        u16RandID ^= ((UINT16)u8RandCnt << 8) & 0xff00;
        srand(u16RandID);

	u16RandID &= 0x0fff;

        return  u16RandID;

}


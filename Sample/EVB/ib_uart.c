
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2011.
	- Date		: 2016-10-10
	- Version		: V2.6

	- File			: ib_uart.c
*******************************************************************************/

#include <stdlib.h>

#include "ib_main_sen.h"
#include "ib_uart.h"


tSerialMessageFormat		gtHost2RF4CEIn;		//Uart In
//tSerialMessageFormat		gtRF4CE2HostOut;	//Uart Out

float Current_Cal=0;
char strTmp[256];

UINT8 Sensor_Mode=0;

UINT8 CurCal_DataBuff[512]={0};
UINT8 SRCID_DataBuff[512]={0};

UINT32 Calcul_X=267;

//2 --- Interface-------
void ZAPP_CommandInterface(void)		//2----Main call----
{
	UINT8	u8SysCommand;
	UINT16	iw;
	UINT8* 	u8Ptr;
	
	
	
	// ???????
	// 	ZAPP_HostUartParser() 에서 프로토콜이 중간에 깨어지거나 전체가 다 들어오지 못했을때 동작 확인 ???
	// !!!!!!!!!!!
	
	if(ZAPP_HostUartParser())
	{
		u8SysCommand = gtHost2RF4CEIn.u8MessageBody[0];
		switch(u8SysCommand)
		{
			//---------------------------------------------------------------------------
			// test command
			case 0x00	:
				// DATA ---- u8MessageBody[1] ~
				// FF AC 04 00 3E 02 00 EF 0D 0A : 03(length) 00(cmd:body[0] 3E(body[1])
				// FF AC 02 00 01 AE 0D 0A	=> works OK
				//zPrintf(1, "\n cs v1 read temp");
					
				for (iw=1; iw < gtHost2RF4CEIn.u8MessageBodySize; iw++) {
					SYS_Uart0Put(gtHost2RF4CEIn.u8MessageBody[iw]);
					//AppLib_Delay(3);	// ms delay

					SYS_Uart1Put(gtHost2RF4CEIn.u8MessageBody[iw]);
					SYS_Uart1Put('\n');

				}	


				break;				
			//---------------------------------------------------------------------------
			
			case GET_COMMAND_ID_1:
				// DATA ---- u8MessageBody[1] ~
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x01)
				{
					SYS_Uart1Put(0xBB);					
				}
				else if(gtHost2RF4CEIn.u8MessageBody[1]==0x02)
				{
					SYS_Uart1Put(0xBC);					
				}				
				else if(gtHost2RF4CEIn.u8MessageBody[1]==0x03)//FF AC 02 01 03 B1 0D 0A
				{
					zPrintf(1, "\n >> HELP");
					zPrintf(1, "\n #########################################");
					zPrintf(1, "\n End Device : FF AC 02(Len) ~");
					zPrintf(1, "\n #########################################");
					zPrintf(1, "\n 01 03               : HELP\n");
					zPrintf(1, "\n 03 00              : ID Flash Read");			// Flash Read : FF AC 02 03 00 B0 0D 0A
					zPrintf(1, "\n 04 00              : ID Flash Erase");			// Flash Erase : FF AC 02 04 00 B0 0D 0A
					zPrintf(1, "\n 05 00              : Syetem Reset");			// SYSTEM RESET : FF AC 02 05 00 B2 0D 0A
					zPrintf(1, "\n");
					
					zPrintf(1, "\n A2 ID              : Sensor Manual Pairing" );	// pairing : FF AC 02 A2 [01] 50 0D 0A
					
					zPrintf(1, "\n B0 [00,01,02] : SENSOR UART [STOP, ASCII, HEX]");
					zPrintf(1, "\n");
					
					zPrintf(1, "\n C2 [d0 ... d8] : Set Option ");				//FF AC 0A C2 d0 d1 d2 d3 d4 d5 d6 d7 d8 00 0D 0A  (d4 d5 d6 d7 MSB first)
					zPrintf(1, "\n  - d0: option bit [b6:No DIP sw, b7;Use option value]");			
					zPrintf(1, "\n  - d1: PID_CH, d2:tx time, d3:sl time, d4-7:op1,op2, d8:rsv");			
					
					//zPrintf(1, "\n                          d4-5: op1, d6-7: op2,  d8: rsv");		  			
					zPrintf(1, "\n C3 [sec]           : Change TX Time : sec");		// FF AC 02 C3 msg1 00 0D 0A				
					// Thermistor adc offset : u16Option1 에 값을 저장 
					zPrintf(1, "\n C4 d0 d2  	     : Set Offset");				//FF AC 03 C4 d0 d1 00 0D 0A  : MSB first		
					// Zener ADC start and revcovery value 
					zPrintf(1, "\n C5 00  	     : Get current Zener ADC value");	//FF AC 02 C5 00 00 0D 0A		
					// Zener ADC start and revcovery value 
					zPrintf(1, "\n C6 d0 d1  	     : Set Zener ADC Start/Recovery");	//FF AC 03 C6 d0(start ADC) d1(Recovery ADC) 00 0D 0A		
								
					zPrintf(1, "\n");					
					zPrintf(1, "\n #########################################");		
					
					
					u8AppRFSq = SQ_RF_TX_INIT;	// 약간의 delay 를 위해 
				}				
				break;				
			case GET_COMMAND_ID_2: 			// Flash Write(No Pair) : FF AC 02 02 00 AF 0D 0A	//FF AC 02 02 01 B0 0D 0A
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x00)
				{
				}
				else
				{
					zPrintf(1, "\n >> ----- ID Flash Write(No Pair) ----- ");
					zPrintf(1, "\n");
				
					//PairTable_Init();
					//ZUSER_PIB_SAVE();
					//Set_PIB_SRCID(gtHost2RF4CEIn.u8MessageBody[1], 0x00);	// 0x00 : no join mode
					gtPIB.PairInfo.SrcID = gtHost2RF4CEIn.u8MessageBody[1];
					
				#if 0	
					SRCID_DataBuff[0] = gtHost2RF4CEIn.u8MessageBody[1];
					SRCID_DataBuff[1] = 0x00; 	// SRCID Info : No Pair
					HAL_FlashWriteUnprotected();
					HAL_FlashPageWrite(CODE_ADDRESS_USER0, SRCID_DataBuff, 0);
					HAL_FlashWriteProtected();
					AppLib_Delay(10);
					AppLib_Delay(10);
					SYS_WdtReset(0);										
				#endif
					
				}	
				//SYS_Uart1Put(0xDE);
				break;				
			
			case GET_COMMAND_ID_3: // Flash Read : FF AC 02 03 00 B0 0D 0A
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x00)
				{
					zPrintf(1, "\n >> ----- ID Flash Read ----- ");
					zPrintf(1, "\n");
					
				   	 //IR Flash read 1byte
				    	HAL_FlashRead(CODE_ADDRESS_USER0, SRCID_DataBuff, 2); //0x1600
					zPrintf(1, ">> Flash Read SRC ID & INFO = 0x%02x(%02x) ",
					(short)(SRCID_DataBuff[0]),(short)(SRCID_DataBuff[1]));
					zPrintf(1, "\n");
				}	
				
				break;				
			case GET_COMMAND_ID_4: // Flash Erase : FF AC 02 04 00 B1 0D 0A
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x00)
				{
					HAL_FlashWriteUnprotected();
					SRCID_DataBuff[0]=0xFF;
					SRCID_DataBuff[1]=0xFF;
					HAL_FlashPageWrite(CODE_ADDRESS_USER0, SRCID_DataBuff, 0);
					HAL_FlashWriteProtected();
					xRTINT1=0;
					AppLib_Delay(10);
					SYS_WdtReset(0);
				}				
				break;							
			case GET_COMMAND_ID_5: // SYSTEM RESET : FF AC 02 05 00 B2 0D 0A
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x00)
				{
					zPrintf(1, "\n >> ----- SYSTEM RESET ----- ");
					zPrintf(1, "\n");
					AppLib_Delay(10);			
					SYS_WdtReset(0);
				}	
				break;	
			case GET_COMMAND_ID_6: // PER TEST : FF AC 02 06 00 B3 0D 0A
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x00)
				{
					zPrintf(1, "\n >> ----- PER TEST ----- ");
					zPrintf(1, "\n");
					AppLib_Delay(10);			
					
					//PER Test Start Response
					SYS_Uart1Put(0xFF);//STX1
					SYS_Uart1Put(0xCA);//STX2
					SYS_Uart1Put(0x02);//Length
					SYS_Uart1Put(0x02);//CMD1
					SYS_Uart1Put(0x03);//CMD2
					SYS_Uart1Put(0xD0);//CheckSum
					SYS_Uart1Put(0x0D);//ETX1
					SYS_Uart1Put(0x0A);//EXT2				

					u8AppRFSq = SQ_RF_TX_PER; 	//SQ_RF_Status=1; // NoJoin(Destination) Mode								
					u16RfPacketCount=0;
					RED_LED = LED_ON; 	//RED  LED ON					
				}	
				break;	
								
			case GET_COMMAND_ID_A0: // SCALEFACTOR : FF AC 02 A0 00 4D 0D 0A
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x00)//FF AC 02 A0 00 4D 0D 0A
				{				
					zPrintf(1, "\n >> ----- SCALEFACTOR Flash Write ----- ");
					zPrintf(1, "\n");
					AccelerationSensor_ScalBuff[0]=AccelerationSensor_ScalData[1];						
					AccelerationSensor_ScalBuff[1]=AccelerationSensor_ScalData[3];						
					AccelerationSensor_ScalBuff[2]=AccelerationSensor_ScalData[5];					
					HAL_FlashWriteUnprotected();
					HAL_FlashPageWrite(CODE_ADDRESS_USER1, AccelerationSensor_ScalBuff, 0);
					HAL_FlashWriteProtected();
					AppLib_Delay(10);					
					AppLib_Delay(10);			
					SYS_WdtReset(0);
				}
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x01)//FF AC 02 A0 01 4E 0D 0A
				{
					zPrintf(1, "\n >> ----- SCALEFACTOR Flash Read ----- ");
					zPrintf(1, "\n");				
				    	HAL_FlashRead(CODE_ADDRESS_USER1, AccelerationSensor_ScalBuff, 511); //0x1400	
					zPrintf(1, ">> SCALEFACTOR Flash Read X=0x%02x Y=0x%02x Z=0x%02x ",
					(short)(AccelerationSensor_ScalBuff[0]),(short)(AccelerationSensor_ScalBuff[1]),
					(short)(AccelerationSensor_ScalBuff[2]));

				    	AccelerationSensor_getData[1]=AccelerationSensor_ScalBuff[0];
				   	AccelerationSensor_getData[3]=AccelerationSensor_ScalBuff[1];
				    	AccelerationSensor_getData[5]=AccelerationSensor_ScalBuff[2];
				    	Accelero_OUTPUT_DATA();			
				    	OutData(AX_data*18);//X	
				    	OutData(AY_data*18);//Y
				    	OutData(AZ_data*18);//Z
				}
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x02)//FF AC 02 A0 02 4F 0D 0A
				{
					for(iw = 0 ; iw < 511 ; iw++)
					{						
						AccelerationSensor_ScalBuff[iw]=0xFF;
					}
					
					HAL_FlashWriteUnprotected();
					HAL_FlashPageWrite(CODE_ADDRESS_USER1, AccelerationSensor_ScalBuff, 0);
					HAL_FlashWriteProtected();
					xRTINT1=1;
					AppLib_Delay(10);
					zPrintf(1, "\n >> ----- SCALEFACTOR Flash Erase ----- ");
					zPrintf(1, "\n");
					AppLib_Delay(10);					
					SYS_WdtReset(0);
																	
				}									
				break;	

			//---------------------------------------------------------------------------	
			// 센서 ID를 수동 할당 하여 상위와 페어링 진행 명령
			//---------------------------------------------------------------------------	
			case GET_COMMAND_ID_A2:			 // Flash Write(Pair) : FF AC 02 A2 00 4F 0D 0A	//FF AC 02 A2 01 50 0D 0A
				if((gtHost2RF4CEIn.u8MessageBody[1]==0x00)||(gtHost2RF4CEIn.u8MessageBody[1]>MAX_END_DEVICE))
				{
					zPrintf(1, "\n SRCID Fail(1 ~ %02d) = %02d !",(short)MAX_END_DEVICE, (short)gtHost2RF4CEIn.u8MessageBody[1]);		
				}
				else
				{									
					zPrintf(1, "\n >> ----- ID Flash Write(Pair) ----- ");
					// set Sensor ID					 
					gtPIB.PairInfo.SrcID = gtHost2RF4CEIn.u8MessageBody[1];

					Pairing_Init();
					// button pairing 과 구분
					u8AutoPairSrcID = 0;
					//u8Task_pairing = PAIRING_SEND;		
					u8AppRFSq = SQ_PAIR_WAIT_ST; 			
				}
				break;				
			//---------------------------------------------------------------------------	

			case GET_COMMAND_ID_B0: // SENSOR UART MODE
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x00)	//FF AC 02 B0 00 5D 0D 0A
				{
					zPrintf(1, "\n >> ----- SENSOR UART MODE STOP----- ");
					zPrintf(1, "\n");
					u8AppRFSq = 0xAA; GP36 = 1; //BLUE LED OFF
				}
				else if(gtHost2RF4CEIn.u8MessageBody[1]==0x01)	//FF AC 02 B0 01 5E 0D 0A
				{
					zPrintf(1, "\n >> ----- SENSOR UART(ASCII) MODE START----- ");
					zPrintf(1, "\n");
					u8AppRFSq = SQ_RF_TX_INIT; //SQ_RF_Status=1; 		// NoJoin(Destination) Mode
					Sensor_Mode=1;
					//Cycle_Flag=1;
				}					
				else if(gtHost2RF4CEIn.u8MessageBody[1]==0x02)	//FF AC 02 B0 02 5F 0D 0A
				{
					zPrintf(1, "\n >> ----- SENSOR UART(HEX) MODE START----- ");
					zPrintf(1, "\n");
					u8AppRFSq = SQ_RF_TX_INIT; //SQ_RF_Status=1; // NoJoin(Destination) Mode
					Sensor_Mode=2;
					//Cycle_Flag=1;
				}	
								
				break;	

			case GET_COMMAND_ID_C0: // CURRENT CAL MODE 
				//FF AC 12 C0 01 11 00 22 33 44 55 66 77 88 99 AA BB CC DD EE FF 76 0D 0A  
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x01)
				{
				
					HAL_FlashRead(CODE_ADDRESS_USER2, CurCal_DataBuff, 511);
					
					for(iw = 0 ; iw < 16 ; iw++)
					{						
						CurCal_DataBuff[iw]=gtHost2RF4CEIn.u8MessageBody[iw+2];
					}
					////Check sum data 1byte
					CurCal_DataBuff[16]=0; 
					for(iw=0;iw<16;iw++)
					{
						CurCal_DataBuff[16] += CurCal_DataBuff[iw];
					} 
					
					
					HAL_FlashWriteUnprotected();
					HAL_FlashPageWrite(CODE_ADDRESS_USER2, CurCal_DataBuff, 0);
					HAL_FlashWriteProtected();			
					
					zPrintf(1, "\n >> ----- CURRENT CAL DATA Flash Write ----- ");
					zPrintf(1, "\n");	
					AppLib_Delay(10);
					AppLib_Delay(10);
					SYS_WdtReset(0);									
				}	
				break;	
			case GET_COMMAND_ID_C1: // CURRENT CAL READ
				//FF AC 12 C1 00 FF 11 22 33 44 55 66 77 88 99 AA BB CC DD EE 00 76 0D 0A
				if(gtHost2RF4CEIn.u8MessageBody[1]==0x00)//FF AC 02 C1 00 6E 0D 0A
				{				
					HAL_FlashRead(CODE_ADDRESS_USER2, CurCal_DataBuff, 511);	
					zPrintf(1, "\n >> ----- CURRENT CAL DATA Flash Read ----- ");			
					zPrintf(1,"\n");
					
					for(iw = 0 ; iw < 511 ; iw++)
					{	
						zPrintf(1," %02x",(short)CurCal_DataBuff[iw]);
					}
				}
				else if(gtHost2RF4CEIn.u8MessageBody[1]==0x01)//FF AC 02 C1 01 6F 0D 0A 
				{
					for(iw = 0 ; iw < 511 ; iw++)
					{	
						CurCal_DataBuff[iw]=0xFF;
					}				
					HAL_FlashWriteUnprotected();
					HAL_FlashPageWrite(CODE_ADDRESS_USER2, CurCal_DataBuff, 0);
					HAL_FlashWriteProtected();
					zPrintf(1,"\n >> DATA Flash Erase");
					AppLib_Delay(10);			
					SYS_WdtReset(0);					
				}
				else if(gtHost2RF4CEIn.u8MessageBody[1]==0x02)//FF AC 02 C1 02 70 0D 0A 
				{

					Test_int32= 0xc1072928;
					Test_uint32= 0xc1072928;
					zPrintf(1,"\n");
					zPrintf(1," >> %f",*((float*)&Test_int32));
					
					zPrintf(1,"\n");
					zPrintf(1," >> %f",*((float*)&Test_uint32));
					Current_Cal = *((float*)&Test_int32);
					zPrintf(1," >> %f ",(float)Current_Cal);
				}															
				else if(gtHost2RF4CEIn.u8MessageBody[1]==0x03)//FF AC 02 C1 03 71 0D 0A 
				{

					HAL_FlashRead(CODE_ADDRESS_USER2, CurCal_DataBuff, 511);
					zPrintf(1, "\n >> ----- CURRENT CAL DATA Flash Read ----- ");			
					zPrintf(1,"\n");
					
					for(iw = 0 ; iw < 511 ; iw++)
					{	
						zPrintf(1," %02x",(short)CurCal_DataBuff[iw]);
					}
					memcpy(&Test_int32, &CurCal_DataBuff, 4);
					Inver_A = *((float*)&Test_int32);
					
					memcpy(&Test_int32, &CurCal_DataBuff[4], 4);
					Inver_B = *((float*)&Test_int32);
					
					memcpy(&Test_int32, &CurCal_DataBuff[8], 4);
					Inver_C = *((float*)&Test_int32);

					memcpy(&Test_int32, &CurCal_DataBuff[12], 4);
					Inver_D = *((float*)&Test_int32);
		
					
					zPrintf(1,"\n");
					zPrintf(1," >> %f",Inver_A);					
					zPrintf(1,"\n");
					zPrintf(1," >> %f",Inver_B);					
					zPrintf(1,"\n");
					zPrintf(1," >> %f",Inver_C);					
					zPrintf(1,"\n");
					zPrintf(1," >> %f",Inver_D);
					
					Current_Val=(Inver_A*(Calcul_X*Calcul_X*Calcul_X))+
							    (Inver_B*(Calcul_X*Calcul_X))+
							    (Inver_C*Calcul_X)+
							    Inver_D;
					
					Test_uint16=(UINT16)(Current_Val*100);
					
					zPrintf(1,"\n");
					zPrintf(1," >> %f",Current_Val);					
					zPrintf(1,"\n");
					zPrintf(1," >> %d",(short)Test_uint16);					
																		
				}
				break;	
					
			case GET_COMMAND_ID_C2: 	//  set option  : 0 이면 기존값을 그대로 저장, 값이 있는 바이트만 변경 저장 
				//FF AC 0A C2 d0 d1 d2 d3 d4 d5 d6 d7 d8 00 0D 0A   
				// d0 : u8ProductMode,  d1 : u8Pid_Chan , d2 : u8Option3(tx time), d3 : u8Option4(sleep time),   
				// d4-5 : u16Option1, d6-7 :  u16Option2, d8 : reserved (0x00)   -   MSB first   
				u8Ptr = &gtPIB.Option.u8ProductMode;
				for (iw=0; iw < 5; iw++) {
					if(gtHost2RF4CEIn.u8MessageBody[1+ iw] != 0) {	
						*u8Ptr =  gtHost2RF4CEIn.u8MessageBody[1+iw];
					}
					
					u8Ptr++;
				}
				
				iw = (UINT16)gtHost2RF4CEIn.u8MessageBody[5] << 8 | gtHost2RF4CEIn.u8MessageBody[6];
				if(iw != 0) 	gtPIB.Option.u16Option1 = iw;

				iw = (UINT16)gtHost2RF4CEIn.u8MessageBody[7] << 8 | gtHost2RF4CEIn.u8MessageBody[8];
				if(iw != 0) 	gtPIB.Option.u16Option2 = iw;
				
				// d10 이 0xff 이면 flash 영역을 초기화 하는 방법 : 페어링을 새로 해야됨 
				// 해당 변수만 0xff 나 기존 값으로 초기화 하는 방법  : d2, d3 는 기존 값으로 , d4 ; 0xff 로 초기화 하면됨  
				SetOptionData();		// d4 = 0xff  : dip switch 값으로 설정을 저장 

				zPrintf(1, " \n mode=%x   pid(7~4)_chan(3~0)=%x   opt3=%x  opt4=%x ", (short)gtPIB.Option.u8ProductMode,  (short)gtPIB.Option.u8Pid_Chan, (short)gtPIB.Option.u8Option3, (short)gtPIB.Option.u8Option4);	
				zPrintf(1, " \n u16opt1=%x   u16opt2=%x  \n", gtPIB.Option.u16Option1,  gtPIB.Option.u16Option2);	

				u8AppRFSq = SQ_RF_TX_INIT;	// 약간의 delay 를 위해 
										
				break;	
			
			case GET_COMMAND_ID_C3: 	// change TX time period   : FF AC 02 C3 msg1 00 0D 0A
				if (gtHost2RF4CEIn.u8MessageBody[1] !=0) {
					if (Porduct_Mode & PM_SLEEP_MODE) {
						u8_SLEEP_TIME = gtHost2RF4CEIn.u8MessageBody[1];
						zPrintf(1, "\n >> ----- Changed  Sleep time  : %d", (short)u8_SLEEP_TIME);			
					}
					else	{
						TxTimePeriod = gtHost2RF4CEIn.u8MessageBody[1];
						zPrintf(1, "\n >> ----- Changed  TX time  : %d", (short)TxTimePeriod);			
					}	
				}
				else
					zPrintf(1, "\n >> ----- Change  TX time  vlaue Error ");			
					
				break;		

			case GET_COMMAND_ID_C4: 	//  Set offset : 써미스터 adc offset ( 0xC2 option1 의 값만 단독 설정 하는 명령  ) 
				//FF AC 03 C4 d0 d1 00 0D 0A   : MSB first   
				
				u16TempOffset = (UINT16)gtHost2RF4CEIn.u8MessageBody[1] << 8 | gtHost2RF4CEIn.u8MessageBody[2];
				gtPIB.Option.u16Option1 = u16TempOffset;			// 0 입력 가능   

				SetOptionData();		
				zPrintf(1, " \n Set Temp Offset =%x \n", gtPIB.Option.u16Option1);	

				zPrintf(1, " \n Cal Temp Offset =%x \n", (short)(0x1745 + u16TempOffset) );	

				u8AppRFSq = SQ_RF_TX_INIT;	// 약간의 delay 를 위해 
										
				break;	
			case GET_COMMAND_ID_C5: 	//  Get current zener ADC data : 현재 ADC 모니터 value
				//FF AC 02 C5 00 00 0D 0A   : wake up mode 로 동작 하므로 전류 값이 틀려져서 사용 못함 x
				
				zPrintf(1,">> Zener ADC  Value = 0x%x \n",  (short)u16ADC_OverCurrentValue);
										
				break;	
				
			case GET_COMMAND_ID_C6: 	//  Set zener adc start/recovery  data
				//FF AC 03 C6 d0(start ADC) d1(Recovery ADC) 00 0D 0A   : 	start ADC > Recovery ADC

				if ((gtHost2RF4CEIn.u8MessageBody[1] !=0xFF) && (gtHost2RF4CEIn.u8MessageBody[2] !=0xFF)){
					if (gtHost2RF4CEIn.u8MessageBody[1] > gtHost2RF4CEIn.u8MessageBody[2]) {
						u8StartZenorADC = gtHost2RF4CEIn.u8MessageBody[1];
						u8RecoveryZenorADC = gtHost2RF4CEIn.u8MessageBody[2];
						
						CurCal_DataBuff[0] = u8StartZenorADC;
						CurCal_DataBuff[1] = u8RecoveryZenorADC;
						 	
						HAL_FlashWriteUnprotected();
						HAL_FlashPageWrite(CODE_ADDRESS_USER2,  CurCal_DataBuff, 0);
						HAL_FlashWriteProtected();
						AppLib_Delay(10);					

						zPrintf(1," Zener ADC Start / Recovery Value : 0x%02x   0x%02x \n",  (short)u8StartZenorADC, (short)u8RecoveryZenorADC);
					}	
					else {
						zPrintf(1," Data Error  !! :  Zener ADC Start Value> Recovery Value. \n");
					}	
				}
				else {
					CurCal_DataBuff[0] = 0xFF;
					CurCal_DataBuff[1] = 0xFF;
						
					u8StartZenorADC = ADCV_OVER_CURR;
					u8RecoveryZenorADC = ADCV_RECV_CURR;
						
					HAL_FlashWriteUnprotected();
					HAL_FlashPageWrite(CODE_ADDRESS_USER2,  CurCal_DataBuff, 0);
					HAL_FlashWriteProtected();
					
					zPrintf(1," Flash initialized for  ADC Start / Recovery Value !! \n");
				}	
											
				break;	
						
		}
	}
}


UINT8 ZAPP_HostUartParser()	//Host to Front Parser(Uart Parser)
{
	static	UINT8	State = 0;
	static	UINT8	*pDst;
	static	UINT8	BufCnt;
	
	UINT8	u8KeyIn;
	UINT8	ParseOK;

	if(SYS_Uart1Get(&u8KeyIn) == 0)		
		return	0;

	ParseOK = 0;
	
	switch(State)
	{
		case 0 :		//SYNC	
			gtHost2RF4CEIn.u8CheckSum = 0;		
			if(u8KeyIn == SYNC_BYTE)		//0xFF
			{
				State = 1;	//Sync. Byte
				gtHost2RF4CEIn.u8CheckSum += u8KeyIn;
			}
			break;
			
		case 1 :		//HEADER
			if(u8KeyIn == HEADER_PC_TO_ZIGBEE)		//0xAC
			{
				gtHost2RF4CEIn.u8MessageHeader = u8KeyIn;
				gtHost2RF4CEIn.u8CheckSum += u8KeyIn;
				// Header Parser
				State = 2;
			}
			else
			{
				State = 0;
			}	
			break;
			
		case 2 :
			gtHost2RF4CEIn.u8MessageBodySize = u8KeyIn;
			gtHost2RF4CEIn.u8CheckSum += u8KeyIn;

			pDst = (UINT8 *)&gtHost2RF4CEIn.u8MessageBody;

			BufCnt = 0;
			State = 3;
			break;
			
		case 3 :	
			*pDst = u8KeyIn;
			pDst++;
			gtHost2RF4CEIn.u8CheckSum += u8KeyIn;			
			BufCnt++;
			if(BufCnt == gtHost2RF4CEIn.u8MessageBodySize)
			{
				State = 4;
			}
			break;
			
		case 4:
			if( (u8KeyIn==0) || (u8KeyIn== gtHost2RF4CEIn.u8CheckSum) )
			{
				State = 5;
			}
			else	
			{
				State = 0;
			}

			break;

		case 5:
			if(u8KeyIn== ZIGBEE_ETX1)
			{
				State = 6;
			}
			else	
			{
				State = 0;
			}
			break;
			
		case 6:
			if(u8KeyIn== ZIGBEE_ETX2)
			{
				ParseOK = 1;
			}
			else	
			{

			}
			State = 0;
			break;
						
		default	: 
			State = 0;
			break;	
	}

	return	ParseOK;
	
}



//----------------------------------------------------------------------------------------------
// 적외선 센서 CS LT 사용시
#define CSLT_SYNC		0xAA

// 
UINT8 CSLT_State = 0;

// AA AA xx xx xx xx : 온도 센서 데이타가 4바이트가 오도록 설정 해야됨
UINT8 ZAPP_HostUart0Parser(void)	//Host to Front Parser(Uart Parser)
{
	UINT8	u8KeyIn;
	
	
	if(SYS_Uart0Get(&u8KeyIn) == 0)		
		return	0;

	//SYS_Uart1Put(u8KeyIn);
	
	switch (CSLT_State) {
		case 0 : 
			if ( u8KeyIn == CSLT_SYNC)
				CSLT_State = 1;
			break;
		case 1 : 
			if ( u8KeyIn == CSLT_SYNC)
				CSLT_State = 2;			// 연속 2회 AA 가 들어 오면 그다음 데이타 바이트
			else
				CSLT_State = 0;	
			break;
		case 2 : 
			i16ReadTemp = (INT16)u8KeyIn << 8;				// first high byte
			CSLT_State = 3;	
			break;
		case 3 : 
			i16ReadTemp |= u8KeyIn;		// second low byte	
			
			//하동 2회 평균 : AvgReadTempValue();
			// 동해 : 평균 방법 변경 => 항상 누적 평균(20회 : 1초2번 read 하므로 약 10초 정도 소요)
			MovingAverage(i16ReadTemp, &gtTempAvg );
			u8TempSenErrCount = 0;			// ir sensor read error 발생을 체크 하기 위한 카운터
			
			// 4,5 번째 데이타가 AA 가 올수가 있어 마지막 데이타까지 수신 하도록 조정 : AA AA 싱크를 한번 찾으면 그다음은 정상적인 수신이 가능 ??
			//CSLT_State = 0;	
			CSLT_State = 4;	
			break;
		case 4 : 
			CSLT_State = 5;	
			break;
		case 5 : 
			CSLT_State = 0;	
			break;
		default :
			CSLT_State = 0;			
			break;
	}		
	
	return 1;
	
}







/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2011.
	- Date		: 2016-10-10
	- Version		: V2.6

	- File			: ib_packet.c
*******************************************************************************/

#include "ib_main_sen.h"
#include "ib_packet.h"


tPER_PARAMETER		tPER;
tRF_PACKET_FORMAT 	gtRF_MsgBody;

UINT8	u8BoardStatus = 0;						// ���� �߻��� ���� �޼��� ����  : default = 0xFF  


void	Init_PER(tPER_PARAMETER *tpPER)
{

	//Initialize structure variable for PER parameter
	tpPER->u8AckReq = 1;
	tpPER->u16ResultChanFail = 0;
	tpPER->u16ResultNoAck = 0;
	tpPER->u16ResultOK = 0;
	tpPER->u16ResultTxNum = 0;	
	tpPER->u8PacketLen = 20;
	tpPER->u16Interval = 10;
	tpPER->u16MissCount = 0;
	tpPER->u16NumToTx = 1000;
	tpPER->u16Retry1st = 0;
	tpPER->u16Retry2nd = 0;
	tpPER->u16Retry3rd = 0;
	tpPER->u16RxCount = 0;
	tpPER->u8DisplayPktInfo = 0;
	tpPER->u8PacketPattern = 1;
	
}
	

void PER_Packet(UINT8* data_buff)
{
	
	UINT8	RFPC=0;
	UINT16	RFPacketChecksum=0;
	UINT8	iw = 0;
	
	// Frame control //
    	data_buff[iw++] = 0x02;
    	data_buff[iw++] = RF_PER_PACKET_LENGTH;			// PacketLength = 0x10   	 //gtMIB.au8IEEEAddr[1];
    	
    	
    	if(u16RfPacketCount ==(PER_PACKET_COUNT - 1))			// 999, �� ������ ����
    		data_buff[iw++] = 0x01; 						// CMD = 01 : end , 03 : start
    	 else
       		data_buff[iw++] = 0x03; 						// CMD = 01 : end , 03 : start
 	 	   	
 	 
 	data_buff[iw++] =  tPER.u8DisplayPktInfo;
 	 	   	
   	data_buff[iw++] = u16RfPacketCount & 0x00FF;		 //DEST : 1000ȸ ������� ����� �Ǿ� �����Ƿ� ��ü ī��Ʈ�� ���� �ʿ�� ����
   	
    	data_buff[iw++] = (short)(tPER.u16Retry1st&0x00FF);
    	data_buff[iw++] = (short)(tPER.u16Retry1st&0xFF00)>>8;
    	data_buff[iw++] = (short)(tPER.u16Retry2nd&0x00FF);
    	data_buff[iw++] = (short)(tPER.u16Retry2nd&0xFF00)>>8;
    	data_buff[iw++] = (short)(tPER.u16Retry3rd&0x00FF);
    	data_buff[iw++] = (short)(tPER.u16Retry3rd&0xFF00)>>8;

   	 ////Check sum data 1byte 
    	////Check sum data 1byte 
	for(RFPC=2; RFPC<RF_PER_PACKET_LENGTH -2; RFPC++)
	{
		RFPacketChecksum += data_buff[RFPC];
	} 
	
    	data_buff[RF_PER_PACKET_LENGTH-2] = RFPacketChecksum & 0x00FF;
	data_buff[RF_PER_PACKET_LENGTH-1] = END_BYTE;
	
}

   
UINT8 SendDataPacket(UINT8* u8Data)
{
	UINT8 status = 0xAA;
		
	if(Sensor_Mode==0)
	{
		u8Sendstatus = App_Data_Transmit(RF_DATA_PACKET_LENGTH, u8Data);	//App_Data_Transmit(20,u8Data);	
		//if (_IS_PM_DBG_MODE) SensorOut();	
	}
	else if(Sensor_Mode==1)
	{
		u8Sendstatus = SensorOut();										//UART ASCII : RF �� �۽� ���� �ʴ��� ������ ������ ���� �ϵ���
	}
	else if(Sensor_Mode==2)
	{
		u8Sendstatus = DataOut(u8Data);	 								//UART HEX 
	}
	else
	{}
	
	return u8Sendstatus;
	
}




// [0~2] : STX  CMD  LEN  u8Data[0]
//                                      u8Data[0] =>  02     10    FF     34    0D    01    FF     FF       FF 03    00 FC      CC 07 10    03      0D 0A  
//                                                           sync  len   cmd   pan   ch     id     src   dest    temp     current      vib           end 
UINT8 App_Data_Transmit(UINT8 length ,UINT8 *u8Data)
{
	tMAC_DATA_REQ		tMacDataReq;
	UINT8	au8MSDU[127];
	UINT8	iw;	
	UINT8	u8Status;

	UINT8	u8CipherLen;

	gtRF_MsgBody.u8Checksum = 0;
	
	gtRF_MsgBody.u8STX = STX;							// STX(0) : 0xA0
	gtRF_MsgBody.u8CMD = CMD_DATA_REQUEST;			// CMD(1) : 0xA0
	gtRF_MsgBody.u8Checksum += gtRF_MsgBody.u8CMD;
	gtRF_MsgBody.u8Size = length;						// LEN(2) 
	gtRF_MsgBody.u8Checksum += gtRF_MsgBody.u8Size;

	for(iw =0; iw < gtRF_MsgBody.u8Size ; iw++)
	{
		gtRF_MsgBody.u8Data[iw] = u8Data[iw];
		gtRF_MsgBody.u8Checksum += gtRF_MsgBody.u8Data[iw];
	}		

	gtRF_MsgBody.u8ETX = ETX;

	au8MSDU[0]= gtRF_MsgBody.u8STX;	
	au8MSDU[1]= gtRF_MsgBody.u8CMD;	
	au8MSDU[2]= gtRF_MsgBody.u8Size;
	for(iw =0; iw < gtRF_MsgBody.u8Size ; iw++)
	{
		au8MSDU[iw+3]= gtRF_MsgBody.u8Data[iw];
	}
	
	au8MSDU[ gtRF_MsgBody.u8Size+3]= gtRF_MsgBody.u8Checksum;
	au8MSDU[ gtRF_MsgBody.u8Size+4]= gtRF_MsgBody.u8ETX;

	
	tMacDataReq.u16DstPanID = gtMIB.u16DstPanID;

	//-------------------------------------------------------------------------------------------
	#if 0
	tMacDataReq.u8DstMode = maccAddrMode_64bit;	// 8byte addre mode ���
	rpmemcpy(&tMacDataReq.uDstAddr.au8Ext[0], &gtMIB.au8DstIEEEAddr[0], 8);

	tMacDataReq.u8SrcMode = maccAddrMode_64bit;	// 8byte addre mode ���
	#else
	tMacDataReq.uDstAddr.u16Short = gtMIB.u16DstShortAddr;
	tMacDataReq.u8DstMode = maccAddrMode_16bit;		// 2byte addre mode ���

	tMacDataReq.u8SrcMode = maccAddrMode_16bit;		// 2byte addre mode ���, gtMIB.u16ShortAddr�� �״�� ���
	#endif
	
	
	
	//-------------------------------------------------------------------------------------------
	
	tMacDataReq.u8TxOption = 1 & 0x01;
	tMacDataReq.u8MsduLen = gtRF_MsgBody.u8Size + PKT_HEADER_TAIL_LENGTH;
	tMacDataReq.pu8Msdu = au8MSDU;

	// Security Processing
	u8Status = HAL_SecEncryptionCCM32(tMacDataReq.pu8Msdu, tMacDataReq.u8MsduLen, u32FrameCounter, u8KeySeqNum, &u8CipherLen);
	tMacDataReq.u8MsduLen = u8CipherLen;
						
	u8Status = AppLib_MacDataRequest(&tMacDataReq);

	return u8Status;
}



//pairing request packet Transmit (Broadcast)
UINT8 Broadcast_Request(void)
{
	tMAC_DATA_REQ		tMacDataReq;
	tDATA_MESSAGE*		ptMsgFormat;
	
	UINT8	au8MSDU[127];
	UINT8	iw;
	UINT8	u8Status;

	UINT8	u8CipherLen;


	ptMsgFormat = (tDATA_MESSAGE*)gtRF_MsgBody.u8Data;
	
	gtRF_MsgBody.u8STX = STX;							// 0xA0
	gtRF_MsgBody.u8CMD = CMD_PAIRING_REQUEST;			// 0xF0
	gtRF_MsgBody.u8Size = RF_DATA_PACKET_LENGTH;		// 0x10
	
	UserCtrl_Packet((UINT8*)ptMsgFormat);
	
	#if (NO_RPT_MODE)	
	ptMsgFormat->u8DataMsgBody.u8DataMsgChannel = Porduct_Mode;
	#else 
	ptMsgFormat->u8DataMsgBody.u8DataMsgChannel = Porduct_Mode  & PM_RPT_MODE;
	#endif
	ptMsgFormat->u8DataMsgBody.u8DataMsgSrcID = gtPIB.PairInfo.SrcID;				// 0x01;	 coordi ���� ���� ID �� ���
	rpmemcpy(&ptMsgFormat->u8DataMsgBody.u8DataMsgDestID, gtMIB.au8IEEEAddr, 8);	// src IEEE addr 8 byte

	u8Status = CalcCheckSum(&gtRF_MsgBody.u8CMD, RF_DATA_PACKET_LENGTH + 2);	// +cmd + size
	gtRF_MsgBody.u8Checksum = u8Status;					// checksum : u8CMD ~ u8Data[u8Size]
	gtRF_MsgBody.u8ETX = ETX;							//  0xAF(ETX)
	
	au8MSDU[0]= gtRF_MsgBody.u8STX;	
	au8MSDU[1]= gtRF_MsgBody.u8CMD;	
	au8MSDU[2]= gtRF_MsgBody.u8Size;
	for(iw =0; iw < gtRF_MsgBody.u8Size ; iw++)
	{
		au8MSDU[iw+3]= gtRF_MsgBody.u8Data[iw];
	}
	au8MSDU[ gtRF_MsgBody.u8Size+3]= gtRF_MsgBody.u8Checksum;
	au8MSDU[ gtRF_MsgBody.u8Size+4]= gtRF_MsgBody.u8ETX;

	tMacDataReq.u8DstMode = maccAddrMode_16bit;		// maccAddrMode_16bit = 2 => pairing�ô� broadcase addr 0xffff 2byte ����
	tMacDataReq.u16DstPanID = gtMIB.u16DstPanID;	// panid �� ������ġ�� ���� Coodi �� End �� ���� ��ġ �ǰ� ����(pan id �� 0xffff �� ���� ���ʿ����)
	tMacDataReq.uDstAddr.u16Short = maccBroadcast;
	tMacDataReq.u8SrcMode = maccAddrMode_64bit;		// maccAddrMode_64bit = 3
	tMacDataReq.u8TxOption = 0x00;					// Ack Request bit : App_Data_Transmit() �� �ٸ� data ���۽ô� Ack �� ��û
	tMacDataReq.u8MsduLen =  gtRF_MsgBody.u8Size + PKT_HEADER_TAIL_LENGTH;
	tMacDataReq.pu8Msdu = au8MSDU;

	//--- Security Processing...
	u8Status = HAL_SecEncryptionCCM32(tMacDataReq.pu8Msdu, tMacDataReq.u8MsduLen, u32FrameCounter, u8KeySeqNum, &u8CipherLen);
	tMacDataReq.u8MsduLen = u8CipherLen;
	u8Status = AppLib_MacDataRequest(&tMacDataReq);
	
	return u8Status;
}


/*********************************************************************************************************/
// Name : UserCtrl_Packet
// parameter : UINT8* data_buff
// Description :RF Transmission Packet Make
// 02 10 FF    34 0D   01    FF    FF FF    03 00    FC CC 07    10     03   
/*********************************************************************************************************/
UINT8 UserCtrl_Packet(UINT8* data_buff)
{
	
	UINT8	ii = 0;
	UINT16	checksum=0;
	UINT8	status = 0;
	
	
	// Frame control //
    	data_buff[0] = 0x02;
    	data_buff[1] = RF_DATA_PACKET_LENGTH;	// gtMIB.au8IEEEAddr[1];
    	data_buff[2] = CMD_DEFUALT; 			// 0xFF
    	
    	data_buff[3] = gtMIB.u16PanID; 			//0x35,  PANID ���� ����
    	data_buff[4] = gtMIB.u8Channel;			//channel 
    	
    	data_buff[5] = gtPIB.PairInfo.SrcID;			//IEEEID_SRC; //0x01
    	data_buff[6] = 0xFF;					//DEST =>  ���ű⿡�� RSSI�� ��� 

	
	if (Porduct_Mode & PM_TRX_TEST_MODE) {
		//--------------------------------------------------------------------------
		// test code �߰�  : ������ �۽� ����Ÿ����, ack ���� ����, ������ ���� ���� �ڵ𿡼� ���� �ֵ���
		
    		////Temperature data 2byte   	
    		data_buff[7] = (short)(TempData&0x00FF);			// LSB first
		  
		data_buff[9] = (UINT8)(u16RfPacketCountH & 0x00FF);
    		data_buff[8] = (UINT8)(u16RfPacketCountH >> 8);
		data_buff[11] = (UINT8)(u16RfPacketCount & 0x00FF);
    		data_buff[10] = (UINT8)(u16RfPacketCount >> 8);
		
    		data_buff[13] = (UINT8)(u16NoAckCount & 0x00FF);
    		data_buff[12] = (UINT8)(u16NoAckCount >> 8);

		//--------------------------------------------------------------------------
	}
	else {
    		//////////////////////////////////////////		
    		////Temperature data 2byte   	
    	#if 1 // org	
    		data_buff[7] = (short)(TempData&0x00FF);		// LSB first
    		data_buff[8] = (short)(TempData&0xFF00)>>8;		// MSB 
    	#else	
    		data_buff[7] = (short)(u16ADC_OverCurrentValue&0x00FF);		// LSB first
    		data_buff[8] = (short)(u16ADC_OverCurrentValue&0xFF00)>>8;		// MSB 
    	#endif	
    		
   		//////////////////////////////////////////		
   		////Current data 2byte   	
   		#if (PWR_ST_ID == PWR_ST_HADONG)
   		data_buff[9] = (short)(u16RfPacketCount&0x00FF);
		data_buff[10] = (short)(u16RfPacketCount&0xFF00)>>8;
   		#else
   		data_buff[9] = (short)(ADC_Voltage&0x00FF);
		data_buff[10] = (short)(ADC_Voltage&0x3F00)>>8;		// ���� 2��Ʈ�� flag�� ���  
   		
   		data_buff[10] |= u8BoardStatus;						// Flag bit
   		#endif

   		////////////////////////////////////////// 
   		////Acceleration data 6byte   	
   		// 3��° Max ���� ���� ����Ÿ�� ��� : ��Ȥ ���� ����Ÿ�� read  
 		data_buff[11] = AccelerationSensor_ThirdMaxData[1]-AccelerationSensor_Scalefactor[0];
		data_buff[12] = AccelerationSensor_ThirdMaxData[3]-AccelerationSensor_Scalefactor[1];
		data_buff[13] = AccelerationSensor_ThirdMaxData[5]-AccelerationSensor_Scalefactor[2];	
    		
		AX_data = data_buff[11];
   		AY_data = data_buff[12];
		AZ_data = data_buff[13];
 		 		
		#if _ACCEL_USE
 		if ((AX_data == 0) && (AY_data == 0) && (AZ_data == 0)) {
 			status = 1;			// ���� flag  
 			AccelSensor_Init();	// ���� ���� �ʱ�ȭ 
 		}
 		#endif
 		 		
   		// Third max data ���� ���� �ʱ�ȭ   
   		SetThirdMaxValueInit(1);
   		SetThirdMaxValueInit(3);
   		SetThirdMaxValueInit(5);
   		
   		//////////////////////////////////////////		
    		
	}			
       	
    	
    	////Check sum data 1byte 
	for(ii = 2; ii < RF_DATA_PACKET_LENGTH-2; ii++)
	{
		checksum += data_buff[ii];
	} 
	
    	data_buff[14] = checksum & 0x00FF;
	data_buff[15] = END_BYTE;
	
 	return status;
	
}


UINT8 CalcCheckSum(UINT8* data_buff, UINT8 length)
{
	UINT8 ii;
	UINT8 checksum = 0;
	
	for(ii = 0; ii < length; ii++) 
		checksum += (UINT8)data_buff[ii];
		
	return checksum;

}	


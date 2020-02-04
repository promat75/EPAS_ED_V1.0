// Master
#define MG2470_WA 1

void HCI_Init(void)
{
	#ifdef MG2470_WA
	UINT8 i;
   	UINT8 dummy_rx[64];
	UINT8 dummy_tx[64];
	for(i=0;i<64;i++)   dummy_tx[i] = i;
	#endif
	

    HCI_InitQueue();
    HAL_SpiClockOn(1);
	HAL_SpiIntSet(1, 0, 0x18, 1);
	HAL_Gpio3InOutSet(7, 1, 0);
 	
	#ifdef MG2470_WA
	while(1)
	{
	  	DRV_SetWdt(10000);
		HCI_SpiSendData(64, dummy_tx, dummy_rx);
		for(i=0;i<64;i++) zPrintf(1, "%02X ", (short) dummy_rx[i]);
		zPrintf("\n"); 

	}
	//HCI_SpiSendData(SPI_HW_FIFO_SIZE+1, dummy, dummy); //Dummy for MG2470
	#endif
	SPI_CSN = 0;
	SYS_SpiMasterPut(0xAA);	 //  Start for dummy sending.
	SPI_CSN = 1;
}

void HCI_SpiSendData(UINT8 len, UINT8* pBuf, UINT8* pRec)
{
	 while(len--)
	 {
	    SPI_CSN = 0;
		*pRec++ = SYS_SpiMasterPut(*pBuf++);
		SPI_CSN = 1;
 	 }
}


 // SLave

 HAL_SpiIntSet(1, 0, 0x08, 1);
	 for(ib=0; ib<16; ib++) 
	  SYS_SpiSlavePut(0xCC);
	
	  HAL_Gpio3InOutSet(7, 0, 0); 
	  zPrintf(1, "\n SPI Slave test\n");
	  while(1)
	  {
		DRV_SetWdt(10000);
		if(SYS_SpiSlaveGet(&u8SpiOutData))
		{
		   //zPrintf(1, "%02X ", (short) u8SpiOutData);
		   	//xU1_THR = '0' + u8SpiOutData;
	    }
		 
		
	  }



 // ISR 
  
void SPI_ISR(void) interrupt 13
{	
#ifdef __MG2470_SPI_H__
	UINT16	u16NextNum;
	UINT8	u8ReadForClear;

	if(xSPCR & BIT4)		// Master mode
	{
		gu8SpiMaster_RxData = xSPDR;
		gu8SpiMaster_IntFlag = 1;	
	}
	else					// Slave mode
	{
		while(1)
		{

			xSPDR = gu8SpiSlave_NextTxData;
	
			u16NextNum = (gu16SpiSlave_RxWrIdx + 1) & (SPI_RXBUF_SIZE - 1);
			

			if(u16NextNum != gu16SpiSlave_RxRdIdx)
			{
				 gau8SpiSlave_RxBuff[gu16SpiSlave_RxWrIdx] = gu8SpiSlave_NextTxData = xSPDR;
				//gau8SpiSlave_RxBuff[gu16SpiSlave_RxWrIdx] = xSPDR;	   // Bryans
				gu16SpiSlave_RxWrIdx = u16NextNum;	
				xU1_THR = '0' + gu8SpiSlave_NextTxData;			
			}
			else
			{
				u8ReadForClear = xSPDR;
				xU1_THR = 'S';
			}
		
			if(xSPSR & BIT0)
			{
				break;	// Read FIFO Empty
			}
		}
	}
	xSPSR = 0x00;
#endif	
}


/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2013-12-20
	- Version		: V2.5

	[2013-12-20] V2.5
	- SPI_ISR() : corrected & enhanced

	[2013-07-24] V2.4
	- UART0_ISR() : corrected & enhanced
	- UART1_ISR() : corrected & enhanced
	- IRTX_ISR() : corrected & enhanced
	- I2C_ISR() : corrected & enhanced
	- SPI_ISR() : corrected & enhanced
	- PWM_ISR() : enhanced	// redundant code removed.

	[2013-05-27] V2.3
	- I2C() : Function added.

	[2013-01-14] V2.2
	- UART0_ISR() : Enhanced
	- UART1_ISR() : Enhanced

	[2012-08-20] V2.1
	- PHY_ISR() : MCU_INT_FLAG_CLEAR_PHY : Removed

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void EXT0_ISR(void) interrupt 0 
{
#ifdef __MG2470_EXTI0_INCLUDE_
	gu8Ext0IntFlag = 1;
	xU1_THR= '#';
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------	

void TIMER0_ISR(void) interrupt 1 
{	
#ifdef __MG2470_TIMER0_INCLUDE__
	TH0 = gu8Timer0ReloadTH0;
	TL0 = gu8Timer0ReloadTL0;
#endif	

#ifdef __MG2470_UTIL_APP_H__
	if(gu16VirtualTimer0)
	{
		gu16VirtualTimer0--;
	}
	if(gu16VirtualTimer1)
	{
		gu16VirtualTimer1--;
	}
#endif	

#ifdef __MG2470_WDT_H__
	if(gu16WdtResetTick)
	{
		HAL_WdtRestart();
		gu16WdtResetTick--;
		if(gu16WdtResetTick == 0)
		{
			EA = 0;
		}
	}
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void EXT1_ISR(void) interrupt 2 
{
#ifdef __MG2470_EXTI1_INCLUDE_
	gu8Ext1IntFlag = 1;
#endif	
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void TIMER1_ISR(void) interrupt 3 
{
#ifdef __MG2470_TIMER1_INCLUDE__
	TH1 = gu8Timer1ReloadTH1;
	TL1 = gu8Timer1ReloadTL1;
#endif	
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void UART0_ISR (void) interrupt 4
{
#ifdef __MG2470_UART0_INCLUDE__
	UINT8	u8IntID;
	UINT8	u8ReadForClear;
	UINT8	ib;
	UINT16	u16NextNum;
	UINT8	u8ReadIIR;

	while(1)
	{
		EA = 0;
		u8ReadIIR = xU0_IIR;
		DPH = 0;
		EA = 1;
		if(u8ReadIIR & 0x01)
		{
			break;
		}		

		u8IntID = (u8ReadIIR >> 1) & 0x07;		
		if(u8IntID == 0x02)		// Received Data Available
		{
			for(ib=0 ; ib<8 ; ib++)
			{
				u16NextNum = (gu16Uart0Rx_WrIdx+1) & (UART0_RXBUF_SIZE-1);
				if(u16NextNum != gu16Uart0Rx_RdIdx)
				{
					EA = 0;
					gau8Uart0Rx_Buff[gu16Uart0Rx_WrIdx] = xU0_RBR;
					DPH = 0;
					EA = 1;
					gu16Uart0Rx_WrIdx = u16NextNum;
				}
				else
				{
					EA = 0;
					u8ReadForClear = xU0_RBR;
					DPH = 0;
					EA = 1;
				}
			}
		}
		else if(u8IntID == 0x06)	// Character Timeout
		{
			// Timeout Interrupt
			u16NextNum = (gu16Uart0Rx_WrIdx+1) & (UART0_RXBUF_SIZE-1);
			if(u16NextNum != gu16Uart0Rx_RdIdx)
			{
				EA = 0;
				gau8Uart0Rx_Buff[gu16Uart0Rx_WrIdx] = xU0_RBR;
				DPH = 0;
				EA = 1;
				gu16Uart0Rx_WrIdx = u16NextNum;
			}
			else	
			{
				EA = 0;
				u8ReadForClear = xU0_RBR;	
				DPH = 0;
				EA = 1;
			}
		}
		else if(u8IntID == 0x01)	// Tx Holding Empty
		{
			gu8Uart0Tx_Empty = 1;
			if(gu16Uart0Tx_WrIdx != gu16Uart0Tx_RdIdx)
			{
				EA = 0;
				xU0_THR = gau8Uart0Tx_Buff[gu16Uart0Tx_RdIdx];
				DPH = 0;
				EA = 1;
				gu16Uart0Tx_RdIdx++;
				gu16Uart0Tx_RdIdx &= (UART0_TXBUF_SIZE-1);			
				gu8Uart0Tx_Empty = 0;
			}	
		}
		else if(u8IntID == 0x03)	// Receiver Line Status
		{		
			EA = 0;
			u8ReadForClear = xU0_LSR;
			DPH = 0;
			EA = 1;
		}
		else if(u8IntID == 0x00)	// Modem Status
		{

		}
		else						// Unknown ???
		{
		}
	}
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void IRTX_ISR(void) interrupt 5
{
	UINT8	u8ReadCTL;
	
	EA = 0;
	u8ReadCTL = xPPM_CTL; 		// clear Interrt
	DPH = 0;
	EA = 1;

#ifdef __MG2470_IRTX_NEC_H__
	gu8IRTxIntFlag = 1;
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void I2C_ISR(void) interrupt 6
{
#ifdef __MG2470_I2C_H__
	UINT8	u8FifoStatus;
	UINT8	u8IntStatus;

	EA = 0;
	u8IntStatus = xI2C_STR;
	DPH = 0;
	EA = 1;

	if(u8IntStatus & BIT4)
	{
		xI2C_IMSK &= ~BIT4;
		u8FifoStatus = xI2C_FINTVAL;

		if(u8FifoStatus & BIT1)
		{
		//	xI2C_CTR |= BIT2;		// Send NACK
		}
	}

	if(u8IntStatus & BIT2)
	{
		if(gu8I2CByteTransferIntCount)
		{
			if(gu8I2CMasterPreReadLength)
			{
				EA = 0;
				gpu8I2CMasterReadData[gu8I2CByteTransferIntCount - 1] = xI2C_DATA;
				DPH = 0;
				EA = 1;
				gu8I2CMasterPreReadLength--;
			}
		}
		gu8I2CByteTransferIntCount++;
	}

	if(u8IntStatus & BIT6)
	{
		gu8I2CNackedFlag = 1;
	}
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void UART1_ISR (void) interrupt 7
{
#ifdef __MG2470_UART1_INCLUDE__	
	UINT8	u8IntID;
	UINT8	u8ReadForClear;
	UINT8	ib;
	UINT16	u16NextNum;
	UINT8	u8ReadIIR;

	while(1)
	{
		EA = 0;
		u8ReadIIR = xU1_IIR;
		DPH = 0;
		EA = 1;
		if(u8ReadIIR & 0x01)
		{
			break;
		}
	
		u8IntID = (u8ReadIIR >> 1) & 0x07;
		if(u8IntID == 0x02)		// Received Data Available
		{
			for(ib=0 ; ib<8 ; ib++)
			{
				u16NextNum = (gu16Uart1Rx_WrIdx+1) & (UART1_RXBUF_SIZE-1);
				if(u16NextNum != gu16Uart1Rx_RdIdx)
				{
					EA = 0;
					gau8Uart1Rx_Buff[gu16Uart1Rx_WrIdx] = xU1_RBR;
					DPH = 0;
					EA = 1;
					gu16Uart1Rx_WrIdx = u16NextNum;
				}
				else
				{
					EA = 0;
					u8ReadForClear = xU1_RBR;
					DPH = 0;
					EA = 1;
				}
			}
		}
		else if(u8IntID == 0x06)	// Character Timeout
		{
			// Timeout Interrupt
			u16NextNum = (gu16Uart1Rx_WrIdx+1) & (UART1_RXBUF_SIZE-1);
			if(u16NextNum != gu16Uart1Rx_RdIdx)
			{
				EA = 0;
				gau8Uart1Rx_Buff[gu16Uart1Rx_WrIdx] = xU1_RBR;
				DPH = 0;
				EA = 1;
				gu16Uart1Rx_WrIdx = u16NextNum;
			}
			else	
			{
				EA = 0;
				u8ReadForClear = xU1_RBR;
				DPH = 0;
				EA = 1;
			}
		}
		else if(u8IntID == 0x01)	// Tx Holding Empty
		{		
			gu8Uart1Tx_Empty = 1;
			if(gu16Uart1Tx_WrIdx != gu16Uart1Tx_RdIdx)		
			{
				EA = 0;
				xU1_THR = gau8Uart1Tx_Buff[gu16Uart1Tx_RdIdx];
				DPH = 0;
				EA = 1;
				gu16Uart1Tx_RdIdx++;
				gu16Uart1Tx_RdIdx &= (UART1_TXBUF_SIZE-1);			
				gu8Uart1Tx_Empty = 0;
			}
		}
		else if(u8IntID == 0x03)	// Receiver Line Status
		{		
			EA = 0;
			u8ReadForClear = xU1_LSR;
			DPH = 0;
			EA = 1;
		}
		else if(u8IntID == 0x00)	// Modem Status
		{

		}
		else						// Unknown ???
		{
		}
	}
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------	
UINT8 Callback_OobRxFromPhyISR(UINT8 u8RxFifoStartIdx);
	
void PHY_ISR() interrupt 8
{
#ifdef __MG2470_PHY_H__
#ifdef __MG2470_UTIL_SYS_H__
	UINT8	u8IntIdx;
	UINT8	u8PsduLen;
	UINT8	u8FCH;	
	UINT8	u8FCL;
	UINT8	u8DSN;
	INT8		i8dBmRssi;
	UINT8	u8Correlation;
	UINT8	u8Status;
	UINT8	u8ExpectedMRFCWP;
	UINT8	u8ReadMRFCWP;
	UINT8	u8SECMAP;
	volatile	UINT8	vu8ReadMRFCSTA;
	static	UINT8	u8AckDelay;

	UINT8	u8FcsLow;
	UINT8	u8FcsHigh;
	static	UINT8	u8SavedFcsLow = 0;
	static	UINT8	u8SavedFcsHigh = 0;
	static	UINT8	u8SavedDSN = 0;
#if (_OPTION_MULTI_DATA_RATE)	
	UINT8	u8ReadCORCNF6;
#endif	

	UINT16	u16NextReadIdx;
	UINT8	u8RxFifoIdx;
	UINT8	u8FragLen1;
	UINT8	u8FragLen2;
	static	UINT8	u8PtrErrorCount = 0;

	while(1)
	{
		JP_RF_START:
	
		if(xINTCON & BIT6)
		{
			break;
		}

		EA = 0;
		u8IntIdx = xINTIDX & PHY_INT_IDX_MASK;
		EA = 1;

		if(u8IntIdx == PHY_INT_IDX_RX_END)
		{
			u8SECMAP = xSECMAP;
			xSECMAP = 0;
			
			// Read RSSI	
			i8dBmRssi = xAGCSTS3;
			// Read Correlation Value
			if( (xLQICNF0 & 0x88) == 0x88)
			{
				u8Correlation = xLQICNF1;
			}			
			else
			{
				u8Correlation = 255;		// If correlation is invalid, Max value.
			}			

		#if (_OPTION_MULTI_DATA_RATE)
			// Read to specify the data rate mode of the received packet.	
			u8ReadCORCNF6 = xCORCNF6;
		#endif

			u8ReadMRFCWP = xMRFCWP;
			u8RxFifoIdx = xMRFCRP;
			u8PsduLen = xMRxFIFO(u8RxFifoIdx);		u8RxFifoIdx += 1;
			u8FCL = xMRxFIFO(u8RxFifoIdx);			u8RxFifoIdx += 1;
			u8FCH = xMRxFIFO(u8RxFifoIdx);			u8RxFifoIdx += 1;
			u8DSN = xMRxFIFO(u8RxFifoIdx);			//u8RxFifoIdx += 1;
			u8ExpectedMRFCWP = xMRFCRP + u8PsduLen + 1;			
			
			// Error Check : Long length over 0x7F.
			if(u8PsduLen & 0x80)
			{				
				goto JP_RF_ERROR;
			}

			// Error Check : RXFIFO Write Pointer.
			if(u8ExpectedMRFCWP != u8ReadMRFCWP)
			{
				u8PtrErrorCount++;
				if(u8PtrErrorCount == 3)
				{
					goto JP_RF_ERROR;
				}
			}
			else
			{
				u8PtrErrorCount = 0;
			}

			// Error Check : CRC error in MAC Status register.
			if(xMACCTRL & BIT1)
			{
				if( (xMACSTS & BIT0) == 0)
				{
					goto JP_RF_ERROR;
				}
			}

			// Error Check : Address Decode.
			if( (xMACCTRL & 0x06) == 0x06)
			{
				if( (xMACSTS & BIT3) == 0)
				{	
					goto JP_RF_ERROR;
				}
			}
			
			// Error Check : Short length under 5.
			if(u8PsduLen < 5)
			{
				goto JP_RF_ERROR;
			}
			
			// Error Check : Unknown packet type.
			if(u8FCL & 0x04)		
			{
				goto JP_RF_ERROR;
			}

			u16NextReadIdx = xMRFCRP + u8PsduLen + 1;
			if(u16NextReadIdx & 0xFF00)
			{
				u8FragLen1 = 256 - xMRFCRP;
				u8FragLen2 = u8PsduLen + 1 - u8FragLen1;
			}
			else
			{
				u8FragLen1 = u8PsduLen + 1;
				u8FragLen2 = 0;
			}

			if( (u8FCL & 0x07) == 0x02) 		// ACK_FRAME
			{
				gu8PhyIntFlag |= PHY_FLAG_MASK_RX_ACK;
			}
			else
			{
				//------------------------------------------------------------
				// OOB-Packet Processing
				//------------------------------------------------------------
				if(u8FCH & 0x33)		// If OOB code is not equal to 0.
				{
					u8Status = Callback_OobRxFromPhyISR(xMRFCRP);
					if(u8Status)
					{
						goto JP_RF_RX_FINISH;
					}
				}
				//------------------------------------------------------------

	 			if(gu8RxQUsed < RF_RXBUF_SIZE)
				{
					gatRxQPacket[gu8RxQWr].i8RSSI = i8dBmRssi;
					gatRxQPacket[gu8RxQWr].u8Correlation = u8Correlation;

					if(u8FCL & 0x20)	// AckReq=1
					{
						if(gu8McuPeriClockMode == CLOCK_MODE_16_MHz)
						{
							for(u8AckDelay=0; u8AckDelay<125; u8AckDelay++);
						}
						else
						{
							for(u8AckDelay=0; u8AckDelay<40; u8AckDelay++);
						}

					#if (_OPTION_MULTI_DATA_RATE)
						if(u8ReadCORCNF6 == 0x10)
						{
							HAL_DataRateSet_ISR(PHY_DATA_RATE_1000K);
						}
						else	
						{
						}

						HAL_AckTransmit(0, u8DSN);
						HAL_DataRateSet_ISR(PHY_DATA_RATE_250K);
					#else
						HAL_AckTransmit(0, u8DSN);
					#endif
					}

					// Duplication Check
					u8RxFifoIdx = xMRFCRP + u8PsduLen - 1;		
					u8FcsLow = xMRxFIFO(u8RxFifoIdx);
					u8RxFifoIdx += 1;
					u8FcsHigh = xMRxFIFO(u8RxFifoIdx);
					
					if(	(u8DSN == u8SavedDSN) 
					&& 	(u8FcsLow == u8SavedFcsLow)
					&&	(u8FcsHigh == u8SavedFcsHigh) )
					{
						// Duplicated Packet
					}
					else
					{					
						rpmemcpy_isr(gatRxQPacket[gu8RxQWr].au8Packet, &xMRxFIFO(xMRFCRP), u8FragLen1);
						if(u8FragLen2)					
						{
							rpmemcpy_isr(&gatRxQPacket[gu8RxQWr].au8Packet[u8FragLen1], &xMRxFIFO(0), u8FragLen2);						
						}
					
						gu8RxQWr++;
						if(gu8RxQWr == RF_RXBUF_SIZE)	gu8RxQWr = 0;
						gu8RxQUsed++;

						u8SavedDSN = u8DSN;
						u8SavedFcsLow = u8FcsLow;
						u8SavedFcsHigh = u8FcsHigh;
					}
				}
			}

			JP_RF_RX_FINISH:
			if(u16NextReadIdx & 0xFF00)
			{
				xMRFCSTA ^= BIT6;
			}
			else
			{
				vu8ReadMRFCSTA = xMRFCSTA;
				xMRFCSTA = vu8ReadMRFCSTA;		// to write internal register. Don't remove.
			}
			xMRFCRP = u16NextReadIdx;			
			xSECMAP = u8SECMAP;			
		}
		else if(u8IntIdx == PHY_INT_IDX_TX_END)
		{
			gu8PhyIntFlag |= PHY_FLAG_MASK_TX_END;
		}
		else if(u8IntIdx == PHY_INT_IDX_MODEM_ON)
		{
			gu8PhyIntFlag |= PHY_FLAG_MASK_MODEM_ON;
		}
		else if(u8IntIdx == PHY_INT_IDX_RX_START)
		{
			gu8PhyIntFlag |= PHY_FLAG_MASK_RX_START;
		}
		else if(u8IntIdx == PHY_INT_IDX_MODEM_FAIL)
		{
			gu8PhyIntFlag |= PHY_FLAG_MASK_MODEM_FAIL;
		}
		else if(u8IntIdx == PHY_INT_IDX_TX_FAIL)
		{
			gu8PhyIntFlag |= PHY_FLAG_MASK_TX_FAIL;
		}		
		else		// unknown interrupt index
		{
			u8SECMAP = xSECMAP;
			goto JP_RF_ERROR;
		}

		goto JP_RF_START;

		JP_RF_ERROR:
		REGCMD_RX_OFF;
		REGCMD_PHY_INT_RESET_ON;
		REGCMD_PHY_INT_RESET_OFF;
		xMRFCSTA = 0x00;		
		xMRFCRP = 0;
		xMRFCWP = 0;
		REGCMD_RSM_RESET_ON;
		REGCMD_RSM_RESET_OFF;
		REGCMD_RX_ON;
		xSECMAP = u8SECMAP; 		
	}

#endif	// #ifdef __MG2470_UTIL_SYS_H__
#endif	// #ifdef __MG2470_PHY_H__
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void TIMER2_ISR(void) interrupt 9		// Timer2 interrupt
{
#ifdef __MG2470_TIMER2_INCLUDE__
	EXIF1 = BIT5;
#endif
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void AES_ISR(void) interrupt 10		// AES interrupt
{
#ifdef __MG2470_TIMER2_INCLUDE__
	EXIF1 = BIT6;
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void TIMER3_ISR(void) interrupt 11		// Timer3 interrupt
{
#ifdef __MG2470_TIMER3_INCLUDE__
	EXIF1 = BIT7;
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void ST_ISR(void) interrupt 12		// Sleep Timer interrupt
{
	STIF = 1;
#ifdef __MG2470_POWER_H__	
	gu8SleepTimerIntFlag = 1;
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void SPI_ISR(void) interrupt 13
{
#ifdef __MG2470_SPI_H__
	UINT16	u16NextNum;
	UINT8	u8ReadForClear;

	if(xSPCR & BIT4)		// Master mode
	{
		EA = 0;
		gu8SpiMaster_RxData = xSPDR;
		DPH = 0;
		EA = 1;
		gu8SpiMaster_IntFlag++;
	}
	else					// Slave mode
	{
		while(1)
		{
			EA = 0;
			xSPDR = gu8SpiSlave_NextTxData;
			DPH = 0;
			EA = 1;
			u16NextNum = (gu16SpiSlave_RxWrIdx + 1) & (SPI_RXBUF_SIZE - 1);

			if(u16NextNum != gu16SpiSlave_RxRdIdx)		// Buffer Not Full
			{
				EA = 0;
				gau8SpiSlave_RxBuff[gu16SpiSlave_RxWrIdx] = xSPDR;
				DPH = 0;
				EA = 1;
				gu16SpiSlave_RxWrIdx = u16NextNum;	
			}
			else		// Buffer Full
			{
				EA = 0;
				u8ReadForClear = xSPDR;
				DPH = 0;
				EA = 1;
			}

			EA = 0;
			u8ReadForClear = xSPSR;
			DPH = 0;
			EA = 1;
			if(u8ReadForClear & BIT0)
			{
				break;	// Read FIFO Empty
			}
		}
	}
	EA = 0;
	xSPSR = 0x00;
	DPH = 0;
	EA = 1;
#endif	
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void DMA_ISR(void) interrupt 14
{
	
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void WAKEUP_ISR(void) interrupt 15	// Wake-up interrupt
{
	while( !(xPDMON & 0x40) );		// Wait for OSC_OK
	CLKCON2 &= ~0x40;	// bit[6]=0. Clock source change HSRCOSC --> 32MHz crystal
	EXIF2 = 0x03;		// Writing '1' clears interrupt.(STIF, WUIF)
#ifdef __MG2470_POWER_H__	
	gu8WakeupIntFlag = 1;
#endif
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void GPIO_ISR(void) interrupt 16
{
	// P0 Interrupt
	if(GPIO_IRQ_PEND & BIT0)
	{
	#ifdef __MG2470_GPIOINT0_INCLUDE__
		gu8GpioInt0Flag = P0_IRQ_STS;
	#endif

		P0_IRQ_STS = 0xFF;		// Clear interrupt flag
	}

	// P1 Interrupt
	if(GPIO_IRQ_PEND & BIT1)
	{
	#ifdef __MG2470_GPIOINT1_INCLUDE__
		gu8GpioInt1Flag = P1_IRQ_STS;
	#endif

		P1_IRQ_STS = 0xFF;		// Clear interrupt flag
	}

	// P3 Interrupt
	if(GPIO_IRQ_PEND & BIT2)
	{
	#ifdef __MG2470_GPIOINT3_INCLUDE__
		gu8GpioInt3Flag = P3_IRQ_STS;
	#endif

		P3_IRQ_STS = 0xFF;		// Clear interrupt flag
	}
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void PWM_ISR(void) interrupt 17
{
	UINT8	u8ReadINTR;

	EA = 0;
	u8ReadINTR = xPWMX_INTR;
	EA = 1;
	
	if(u8ReadINTR & BIT0)			// PWM0 Interrupt
	{
		if(xPWM0_CTRL & BIT4)
		{
			xPWM0_CTRL = 0;
		}
	}

	if(u8ReadINTR & BIT1)	// PWM1 Interrupt
	{
		if(xPWM1_CTRL & BIT4)
		{
			xPWM1_CTRL = 0;
		}	
	}

	if(u8ReadINTR & BIT2)	// PWM2 Interrupt
	{
		if(xPWM2_CTRL & BIT4)
		{
			xPWM2_CTRL = 0;
		}	
	}

	if(u8ReadINTR & BIT3)	// PWM3 Interrupt
	{
		if(xPWM3_CTRL & BIT4)
		{
			xPWM3_CTRL = 0;
		}	
	}

	if(u8ReadINTR & BIT4)	// PWM4 Interrupt
	{
		if(xPWM4_CTRL & BIT4)
		{
			xPWM4_CTRL = 0;
		}	
	}	
}



#ifndef __MG2470_ISR_H__
#define __MG2470_ISR_H__


void EXT0_ISR(void);
void TIMER0_ISR(void);
void EXT1_ISR(void) interrupt 2 
void TIMER1_ISR(void) interrupt 3 
void UART0_ISR (void) interrupt 4
void IRTX_ISR(void) interrupt 5
void I2C_ISR(void) interrupt 6
void UART1_ISR (void) interrupt 7
void PHY_ISR() interrupt 8

#endif	// #ifndef __MG2470_IB_UART_H__
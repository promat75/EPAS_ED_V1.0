;============================================================
;
;	This STARTUP File is used for code banking of RadioPulse's MG2470B
;
;	NAME	: STARTUP_BANK_RP_MG2470.A51
;	Chip		: MG2470B
;	Vendor	: RadioPulse Inc. 2012.
;	DATE	: 2012-04-25
;	Version	: V2.0
;
;	[2012-04-25] V2.0
;	- Initial Version for MG2470B(ChipID=0x71)
;	
;============================================================



$NOMOD51
;------------------------------------------------------------------------------
;  This file is part of the C51 Compiler package
;  Copyright (c) 1988-2002 Keil Elektronik GmbH and Keil Software, Inc.
;------------------------------------------------------------------------------
;  STARTUP.A51:  This code is executed after processor reset.
;
;  To translate this file use A51 with the following invocation:
;
;     A51 STARTUP.A51
;
;  To link the modified STARTUP.OBJ file to your application use the following
;  BL51 invocation:
;
;     BL51 <your object file list>, STARTUP.OBJ <controls>
;
;------------------------------------------------------------------------------
;
;  User-defined Power-On Initialization of Memory
;
;  With the following EQU statements the initialization of memory
;  at processor reset can be defined:
;
;               ; the absolute start-address of IDATA memory is always 0
IDATALEN		EQU     00H     	; the length of IDATA memory in bytes.
;
XDATASTART		EQU     0H      		; the absolute start-address of XDATA memory
;XDATALEN		EQU     17FFH      	; the length of XDATA memory in bytes.
XDATALEN		EQU     0H   	   	; the length of XDATA memory in bytes.
;
PDATASTART		EQU     0H     		; the absolute start-address of PDATA memory
PDATALEN		EQU     0H      		; the length of PDATA memory in bytes.
;
;  Notes:  The IDATA space overlaps physically the DATA and BIT areas of the
;          8051 CPU. At minimum the memory space occupied from the C51 
;          run-time routines must be set to zero.
;------------------------------------------------------------------------------
;
;  Reentrant Stack Initilization
;
;  The following EQU statements define the stack pointer for reentrant
;  functions and initialized it:
;
;  Stack Space for reentrant functions in the SMALL model.
IBPSTACK        	EQU     0       		; set to 1 if small reentrant is used.
IBPSTACKTOP     	EQU     1  		; set top of stack to highest location+1.
;
;  Stack Space for reentrant functions in the LARGE model.      
XBPSTACK        	EQU     1       		; set to 1 if large reentrant is used.
XBPSTACKTOP     	EQU     17FFH+1	; set top of stack to highest location+1.
;
;  Stack Space for reentrant functions in the COMPACT model.    
PBPSTACK        	EQU     0       		; set to 1 if compact reentrant is used.
PBPSTACKTOP     	EQU     1			; set top of stack to highest location+1.
;
;------------------------------------------------------------------------------
;
;  Page Definition for Using the Compact Model with 64 KByte xdata RAM
;
;  The following EQU statements define the xdata page used for pdata
;  variables. The EQU PPAGE must conform with the PPAGE control used
;  in the linker invocation.
;
PPAGEENABLE     	EQU     0			; set to 1 if pdata object are used.
;
PPAGE           		EQU     0       		; define PPAGE number.
;
PPAGE_SFR       	DATA    0A0H    	; SFR that supplies uppermost address byte
;               (most 8051 variants use P2 as uppermost address byte)
;
;------------------------------------------------------------------------------
PDCON			EQU		22E0H		; xPDCON
RCOSC			EQU		22E1H		; xRCOSC
RTDLY			EQU		22E2H		; xRTDLY
PDMON			EQU		22E6H		; xPDMON
GPIOPS0			EQU		22E7H		; xGPIOPS0
GPIOPS1			EQU		22E8H		; xGPIOPS1
GPIOPS3			EQU		22E9H		; xGPIOPS3
GPIOPE0			EQU		22EAH		; xGPIOPE0
GPIOPE1			EQU		22EBH		; xGPIOPE1
GPIOPE3			EQU		22ECH		; xGPIOPE3
GPIOPOL0		EQU		22EDH		; xGPIOPOL0
GPIOPOL1		EQU		22EEH		; xGPIOPOL1
GPIOPOL3		EQU		22EFH		; xGPIOPOL3
GPIOMSK0		EQU		22F0H		; xGPIOMSK0
GPIOMSK1		EQU		22F1H		; xGPIOMSK1
GPIOMSK3		EQU		22F2H		; xGPIOMSK3

FCN_CMD		EQU		2700H
FCN_ADR1		EQU		2702H
FCN_ADR0		EQU		2703H
FCN_LEN1		EQU		2708H
FCN_LEN0		EQU		2709H
FCN_NOWAIT		EQU		270EH
FCN_TERS1		EQU		2720H
FCN_TERS0		EQU		2721H
FCN_TME1		EQU		2722H
FCN_TME0		EQU		2723H
FCN_TPRG		EQU		2724H
FCN_TRCV		EQU		2725H
FCN_THV1		EQU		2726H
FCN_THV0		EQU		2727H
FCN_TNVS		EQU		2728H
FCN_TNVH		EQU		2729H
FCN_TPGS		EQU		272AH
FCN_TNVH1		EQU		272BH
;------------------------------------------------------------------------------
CRYSTAL_16M		EQU		0H
PLLDIV2			EQU		22B2H		; xPLLDIV2
PLLDFRAC1		EQU		22B5H		; xPLLDFRAC1
MPTOP_CLK_SEL	EQU		2785H		; xPHY_MPTOP_CLK_SEL
PLLWT1			EQU		220FH		; xPLLWT1
;------------------------------------------------------------------------------

; Standard SFR Symbols 
ACC		DATA	0E0H
B		DATA	0F0H
SP		DATA	81H
DPL		DATA	82H
DPH		DATA	83H
REN		BIT		0C0H.1	
CLK_STP0	DATA	98H			; PERI_CLK_STP0
CLK_STP1	DATA	99H			; PERI_CLK_STP1
CLK_CON1	DATA	8EH			; CLKCON1

		NAME	?C_STARTUP

?C_C51STARTUP   SEGMENT   CODE
?STACK          SEGMENT   IDATA

		RSEG	?STACK
		DS		1

		EXTRN CODE (?C_START)
		PUBLIC	?C_STARTUP

		CSEG	AT      0
?C_STARTUP:     
		LJMP		STARTUP1
		
		RSEG	?C_C51STARTUP

STARTUP1:

;
;
;
;
;
		MOV		CLK_STP0, #0x00
		MOV		CLK_STP1, #0x20
		
		MOV		DPTR, #PDCON
		MOV		A, #0xD9
		MOVX	@DPTR, A

;
; 	Initialize FLASH Registers
;

		MOV		DPTR, #FCN_NOWAIT
		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_ADR1
		MOV		A, #0x70
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_ADR0
		MOV		A, #0x00
		MOVX	@DPTR, A

		;
		CLR		A
		MOV		DPTR, #0x0000
		MOVX	@DPTR, A
	DELAYLOOP:
		MOV		DPTR, #0x0000
		MOVX	A, @DPTR
		INC		A
		MOVX	@DPTR, A
		MOVX	A, @DPTR
		CJNE	A, #0xC8, DELAYLOOP	
		;

		MOV		DPTR, #FCN_CMD
		MOV		A, #0x80
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_LEN1
		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_LEN0
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_TERS1
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_TERS0
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_TME1
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_TME0
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_TPRG
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_TRCV
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_THV1
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_THV0
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_TNVS
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_TNVH
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_TPGS
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #FCN_TNVH1
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		CLK_STP1, #0x00

;
;	Initialize 3V Logic Registers
;

		MOV		DPTR, #RCOSC
		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #RTDLY
		MOV		A, #0x90
		MOVX	@DPTR, A

;		MOV		DPTR, #PDMON
;		MOV		A, #0xC3
;		MOVX	@DPTR, A

		MOV		DPTR, #GPIOPS0
		MOV		A, #0xFF
		MOVX	@DPTR, A

		MOV		DPTR, #GPIOPS1
;		MOV		A, #0xFF
		MOVX	@DPTR, A

		MOV		DPTR, #GPIOPS3
;		MOV		A, #0xFF
		MOVX	@DPTR, A

		MOV		DPTR, #GPIOPE0
;		MOV		A, #0xFF
		MOVX	@DPTR, A
		
		MOV		DPTR, #GPIOPE1
;		MOV		A, #0xFF
		MOVX	@DPTR, A

		MOV		DPTR, #GPIOPE3
;		MOV		A, #0xFF
		MOVX	@DPTR, A

		MOV		DPTR, #GPIOPOL0
		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #GPIOPOL1
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #GPIOPOL3
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #GPIOMSK0
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #GPIOMSK1
;		MOV		A, #0x00
		MOVX	@DPTR, A

		MOV		DPTR, #GPIOMSK3
;		MOV		A, #0x00
		MOVX	@DPTR, A

IF CRYSTAL_16M <> 0
		MOV		DPTR, #PLLDIV2
		MOV		A, #0x80
		MOVX	@DPTR, A

		MOV		DPTR, #PLLDFRAC1
		MOV		A, #0x04
		MOVX	@DPTR, A

		MOV		DPTR, #MPTOP_CLK_SEL
		MOV		A, #0x07
		MOVX	@DPTR, A

		MOV		CLK_CON1, #0x08

		MOV		DPTR, #PLLWT1
		MOV		A, #0x16
		MOVX	@DPTR, A
ENDIF

;
;
;
;
;

IF IDATALEN <> 0
		MOV		R0,#IDATALEN - 1
		CLR		A
IDATALOOP:      MOV     @R0,A
		DJNZ	R0,IDATALOOP
ENDIF

IF XDATALEN <> 0
		MOV		DPTR,#XDATASTART
		MOV		R7,#LOW (XDATALEN)
  IF (LOW (XDATALEN)) <> 0
		MOV		R6,#(HIGH (XDATALEN)) +1
  ELSE
		MOV		R6,#HIGH (XDATALEN)
  ENDIF
		CLR		A
XDATALOOP:      
		MOVX	@DPTR,A
		INC		DPTR
		DJNZ	R7,XDATALOOP
		DJNZ	R6,XDATALOOP
ENDIF

IF PPAGEENABLE <> 0
		MOV		PPAGE_SFR,#PPAGE
ENDIF

IF PDATALEN <> 0
		MOV		R0,#LOW (PDATASTART)
		MOV		R7,#LOW (PDATALEN)
		CLR		A
PDATALOOP:      
		MOVX	@R0,A
		INC		R0
		DJNZ	R7,PDATALOOP
ENDIF

IF IBPSTACK <> 0
EXTRN DATA (?C_IBP)
		MOV		?C_IBP,#LOW IBPSTACKTOP
ENDIF

IF XBPSTACK <> 0
EXTRN DATA (?C_XBP)
		MOV		?C_XBP,#HIGH XBPSTACKTOP
		MOV		?C_XBP+1,#LOW XBPSTACKTOP
ENDIF

IF PBPSTACK <> 0
EXTRN DATA (?C_PBP)
		MOV		?C_PBP,#LOW PBPSTACKTOP
ENDIF
		MOV		SP,#?STACK-1
		CLR		REN
		
     EXTRN CODE (?B_SWITCH0)
		CALL    	?B_SWITCH0
		
		LJMP		?C_START
		END

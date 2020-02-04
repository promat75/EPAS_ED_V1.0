

;*********************************************************************************;
;
;	Chiip	: MG2450/2455
;	Vendor	: RadioPulse Inc, 2007.
;	Date		: 2009-03-17
;	Version	: VER 1.0
;	Describe	: memcpy() using dual DPTR(DPH/DPL) of MG245X
;	Usage	: in C source files;
;				#include "rpmemcpy.h"
;				void main()
;				{
;					rpmemcpy(DstBuff, SrcBuff, 128);
;				}
;
;				#include "rpmemcpy.h"
;				void RF_ISR()
;				{
;					rpmemcpy_isr(DstBuff, SrcBuff, 128);
;				};
;
;	Note		: rpmemcpy_isr() only should be called in RF_ISR()
;
;*********************************************************************************

radiopuse_memcpy	SEGMENT		CODE

	PUBLIC	_rpmemcpy
	PUBLIC	_rpmemcpy_isr

	AUXR1	EQU	0xA2

	RSEG	radiopuse_memcpy

	_rpmemcpy:

	MOV		A,R3
	JZ		L001
	INC		R2
L001:
	ORL		A,R2
	JZ		L003	
	MOV		DPH,R4
	MOV		DPL,R5
	XRL		AUXR1,#0x01	
	MOV		DPH,R6
	MOV		DPL,R7
	XRL		AUXR1,#0x01	
	NOP
L002:
	MOVX	A,@DPTR
	INC		DPTR
	XRL		AUXR1,#0x01	
	MOVX	@DPTR,A
	INC		DPTR
	XRL		AUXR1,#0x01
	DJNZ	R3,L002
	DJNZ	R2,L002	
L003:
	RET
	
	_rpmemcpy_isr:	

;	CLR		A
;	MOV		AUXR1, A

	PUSH	AUXR1	
	MOV		AUXR1, #0x01	; AUXR1=0x01
	NOP
	NOP
	PUSH	DPL
	PUSH	DPH
	XRL		AUXR1,#0x01		; AUXR1=0x00
	NOP
	NOP
	PUSH	DPL
	PUSH	DPH

	; Here, AUXR1=0x00
	
	MOV		A,R3
	JZ		L004
	INC		R2
L004:	
	ORL		A,R2
	JZ		L006	
	MOV		DPH,R4
	MOV		DPL,R5
	XRL		AUXR1,#0x01	
	MOV		DPH,R6
	MOV		DPL,R7
	XRL		AUXR1,#0x01
	NOP
L005:	
	MOVX	A,@DPTR
	INC		DPTR
	XRL		AUXR1,#0x01
	MOVX	@DPTR,A
	INC		DPTR
	XRL		AUXR1,#0x01
	DJNZ	R3,L005
	DJNZ	R2,L005	
L006:

	; Here, AUXR1=0x00
	POP		DPH
	POP		DPL
	XRL		AUXR1,#0x01		; AUXR1=0x01
	NOP
	NOP
	POP		DPH
	POP		DPL
	POP		AUXR1

	RET
	END





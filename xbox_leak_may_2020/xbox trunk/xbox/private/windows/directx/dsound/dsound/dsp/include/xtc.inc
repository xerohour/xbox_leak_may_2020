;	File	:	XTC.asm
;	Version	:	1.0
;	Date	:	16/8/2000
;
;	Author	:	RDA
;				Sensaura Ltd
;
;       Modified/optimized by georgioc so single xtalk can do 4in->4out
;
; Description
; ===========
;	Crosstalk canceller include file.
;	This file contains the constant definitions and macros
;	for the crosstalk canceller.
;
; Assembler options : Don't preserve comments in macros
	opt     nocm


; CTC_DELAYBUFFER_SIZE
;	The length of the crosstalk canceller delay line.
;
CTC_DELAYBUFFER_SIZE	set	11


; CTC_COEFF_A1_DEFAULT
;	The IIR A1 coefficient. (The feedback gain)
;
; CTC_COEFF_B0_DEFAULT
;	The IIR B0 coefficient. (The input gain)
;
CTC_COEFF_A1_DEFAULT	set	$7645A1		;0.924*(1<<23)
CTC_COEFF_B0_DEFAULT	set	$F88313		;-0.0585*(1<<23)



; Define the memory layout of the CTC structure

CTC_Memory			set     DATASIZE_DSP_FX_STATE_4IN_4OUT

CTC_FrontDelayBuffer_Left	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+CTC_DELAYBUFFER_SIZE

CTC_IIR_FrontDelayElement_Left	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+1

CTC_FrontDelayBuffer_Right	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+CTC_DELAYBUFFER_SIZE

CTC_IIR_FrontDelayElement_Right	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+1

CTC_FrontDelayBufferIndex	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+1

; rear delay data

CTC_RearDelayBuffer_Left	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+CTC_DELAYBUFFER_SIZE

CTC_IIR_RearDelayElement_Left	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+1

CTC_RearDelayBuffer_Right	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+CTC_DELAYBUFFER_SIZE

CTC_IIR_RearDelayElement_Right	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+1

CTC_RearDelayBufferIndex	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+1

; constants

CTC_Coeff_A1		       	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+1
CTC_Coeff_B0		       	set	CTC_Memory
CTC_Memory		       	set	CTC_Memory+1

CTC_Memory		       	set	CTC_Memory+1	; Add an extra word to make the structure
						        ; a multiple of 4 words long

DATASIZE_XTALK		       	set	CTC_Memory





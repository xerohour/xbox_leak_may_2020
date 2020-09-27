; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			sysmem.asm
;												
;   Contents:		Game Encoder System memory Simulator
;
;	Author:			jmw
;											
;	Revision:		1.00
;											
;	History:
;
;		Rev 1.00	10/9/2000 5:46:11 PM		jmw
;	Created.
;
;
; ******************************************************************

	page	132,66,3,3
	opt		cre,loc,cc,cex,mex,mu,so

	title	'System Memory'

	section	Sysmem

;**** include files ************************************************

	nolist
	include 'ldr_sip.inc'
	include 'se_sip.inc'
	list

;**** equates ******************************************************

SYSMEM_HEAP_SIZE		equ		8192
AC3_OUT_BUFF_SIZE		equ		3072
PCM_LTRT_OUT_BUFF_SIZE	equ		512
PCM_IN_BUFF_SIZE		equ		1536

;**** program code *************************************************

	org		x(100):

SysLdrTable:
	dc		LDR_SYSTABLE_SIZE		; table size
	dc		LID_MAX_PROG			; number of programs
	dc		prog_1_ptr				; pointer to program #1
	dc		prog_1_size				; size of program #1
	dc		prog_2_ptr				; pointer to program #2
	dc		prog_2_size				; size of program #2
	dc		prog_3_ptr				; pointer to program #3
	dc		prog_3_size				; size of program #3
	dc		prog_4_ptr				; pointer to program #4
	dc		prog_4_size				; size of program #4
	dc		pcm_ptr					; pointer to input PCM buffer
	dc		pcm_size				; size of input PCM buffer
	dc		ltrt_ptr				; pointer to output Lt/Rt buffer
	dc		ltrt_size				; size of output Lt/Rt buffer
	dc		ac3_ptr					; pointer to output AC-3 buffer
	dc		ac3_size				; size of output AC-3 buffer
	dc		config_ptr				; configuration data pointer
	dc		config_size				; configuration data size
	dc		heap_ptr				; pointer to heap data buffer
	dc		heap_size				; size of heap data buffer

prog_1_ptr:
	nolist
	include 'encds.asm'				; Dolby Surround Program Code
	list
prog_1_size	equ		(*)-prog_1_ptr

prog_2_ptr:
	nolist
	include 'llef13.asm'			; Game Encoder Fcn 1 Program Code
	list
prog_2_size	equ		(*)-prog_2_ptr

prog_3_ptr:
	nolist
	include 'llef23.asm'			; Game Encoder Fcn 2 Program Code
	list
prog_3_size	equ		(*)-prog_3_ptr

prog_4_ptr:
	nolist
	include 'llef33.asm'			; Game Encoder Fcn 3 Program Code
	list
prog_4_size	equ		(*)-prog_4_ptr

	org		x(101):

pcm_ptr		ds		PCM_IN_BUFF_SIZE		; PCM Input Buffer
pcm_size	equ		(*)-pcm_ptr

	org		x(102):

ltrt_ptr	ds		PCM_LTRT_OUT_BUFF_SIZE	; PCM Lt/Rt Output Buuffer
ltrt_size	equ		(*)-ltrt_ptr

	org		x(103):

ac3_ptr		bsc		AC3_OUT_BUFF_SIZE,0		; AC-3 Output Buffer
ac3_size	equ		(*)-ac3_ptr

	org		x(104):

config_ptr:
	nolist
	include 'lleconfg.asm'					; System Configuration data
	list
config_size	equ		(*)-config_ptr

	org		x(105):

heap_ptr	ds		SYSMEM_HEAP_SIZE		; Heap
heap_size	equ		(*)-heap_ptr

	org		y(301):
	nolist	
	include 'lleyrom.asm'					; Game Encoder ROM
	list	

;**** program code *************************************************

	endsec	; Sysmem
	end

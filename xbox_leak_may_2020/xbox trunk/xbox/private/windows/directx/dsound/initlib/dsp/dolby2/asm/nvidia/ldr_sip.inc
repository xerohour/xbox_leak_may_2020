; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			ldr_sip.inc
;												
;   Contents:		Loader Software Interface Protocol (SIP) for 
;					Dolby Game Encoder
;
;	Author:			jmw
;											
;	Revision:		1.00
;											
;	History:
;
;		Rev 1.00	8/16/2000 12:07:45 PM		jmw
;	Created.
;
;
; ******************************************************************

;	Loader system equates

LDR_HDR_SIZE		equ		4		; program header size

;	Buffer ID equates

LID_MIN				equ		0		; min valid ID
LID_MAX				equ		31		; max valid ID

LID_LOADER_INIT		equ		0		; Loader initialization request

LID_PROG_DSURRE		equ		1		; Dolby Surround Encoder program
LID_PROG_DGAME1		equ		2		; Dolby Game Encoder #1 program
LID_PROG_DGAME2		equ		3		; Dolby Game Encoder #2 program
LID_PROG_DGAME3		equ		4		; Dolby Game Encoder #3 program

LID_MAX_PROG		equ		4		; max valid program ID

LID_BUF_PCM			equ		5		; Input PCM buffer
LID_BUF_LTRT		equ		6		; Output Lt/Rt buffer
LID_BUF_AC3			equ		7		; Output AC-3 buffer

LID_CONFIG_LIST		equ		8		; system configuration data		

LID_PING_PONG		equ		9		; NVIDIA ping pong Loader ID
LID_AC3_ZEROS		equ		10
LID_AC3_PREAMBLE	equ		11
LID_BUF_DATA		equ		12		; Output data buffer base

LDR_NUM_DATA		equ		(LID_MAX-LID_BUF_DATA+1)	; max # data buffers

;	Loader table equates

LDR_TABLE_SIZE		equ		((LID_MAX+1)*2)			; DSP loader table size
LDR_SYSTABLE_SIZE	equ		((LID_BUF_DATA+1)*2)	; SysMem loader table size
LDR_HEAP_ADDR		equ		(LID_BUF_DATA*2)		; heap address offset
LDR_HEAP_SIZE		equ		((LID_BUF_DATA*2)+1)	; heap size offset
LDR_PINGPONG_OFFSET equ     (LID_PING_PONG*2)

;	Action equates

LDR_ACT_MIN			equ		0		; min valid action

LDR_ACT_ALLOC		equ		0		; allocate data buffer
LDR_ACT_TO_DSPX		equ		1		; copy to DSP x:
LDR_ACT_FROM_DSPX	equ		2		; copy from DSP x:
LDR_ACT_TO_DSPY		equ		3		; copy to DSP y:
LDR_ACT_FROM_DSPY	equ		4		; copy from DSP y:
LDR_ACT_TO_DSPP		equ		5		; copy to DSP p:
LDR_ACT_FROM_DSPP	equ		6		; copy from DSP p:

LDR_ACT_MAX			equ		6		; max valid action

;	PCM offset equates

LDR_OFF_PCM_L		equ		0		; left PCM offset
LDR_OFF_PCM_C		equ		1		; center PCM offset
LDR_OFF_PCM_R		equ		2		; right PCM offset
LDR_OFF_PCM_LS		equ		3		; left surround PCM offset
LDR_OFF_PCM_RS		equ		4		; right surround PCM offset
LDR_OFF_PCM_LFE		equ		5		; LFE PCM offset

LDR_OFF_PCM_LT		equ		0		; left total PCM offset
LDR_OFF_PCM_RT		equ		1		; right total PCM offset

;	Error codes

LDR_ERR_NONE		equ		0		; no error
LDR_ERR_ID			equ		1		; invalid buffer ID
LDR_ERR_ACTION		equ		2		; invalid action
LDR_ERR_LENGTH		equ		3		; invalid length/offset combination
LDR_ERR_ALLOC		equ		4		; buffer allocation error

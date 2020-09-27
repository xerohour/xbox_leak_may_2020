; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			se_sip.inc
;												
;   Contents:		Super Exec Configuration Software Interface Protocol (SIP)
;
;	Author:			jmw
;											
;	Revision:		1.00
;											
;	History:
;
;		Rev 1.00	10/14/2000 10:08:08 AM		jmw
;	Created.
;
;
; ******************************************************************

;**** SuperExec input parameter list equates ****

SE_IN_SIZE			equ		0		; input parameter list size
SE_IN_DO_SURROUND	equ		1		; do Dolby Surround Encoder flag
SE_IN_DO_GAME		equ		2		; do Dolby Game Encoder flag
SE_IN_FSCODE		equ		3		; PCM sampling rate code
SE_IN_ACMODE		equ		4		; input channel configuration
SE_IN_LFE			equ		5		; low frequency effects channel present flag
SE_IN_DIALNORM		equ		6		; dialnorm setting
SE_IN_BW			equ		7		; manual channel bandwidth
SE_IN_LPF			equ		8		; channel LPF enabled flag
SE_IN_LFELPF		equ		9		; LFE LPF flag
SE_IN_HPF			equ		10		; DC HPF flag
SE_IN_DYNRNGE		equ		11		; dynrng exists flag
SE_IN_DYNRNG		equ		12		; AC-3 dynamic range code
SE_IN_COMPRE		equ		13		; compr exists flag
SE_IN_COMPR			equ		14		; AC-3 compression code
SE_IN_SURR_GE		equ		15		; Dolby Surround gain enabled flag
SE_IN_SURR_GAIN		equ		16		; Dolby Surround gain
SE_IN_DSURMOD		equ		17		; Dolby Surround Mode.  Only used for 2/0 Mode

SE_IN_MAX			equ		18		; maximum input parameter list size

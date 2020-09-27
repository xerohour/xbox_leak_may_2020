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
SE_IN_DO_DOWNMIX	equ		3		; do Dolby Downmix (1 for Mono, 2 for Stereo)
SE_IN_FSCODE		equ		4		; PCM sampling rate code
SE_IN_ACMODE		equ		5		; input channel configuration
SE_IN_LFE			equ		6		; low frequency effects channel present flag
SE_IN_DIALNORM		equ		7		; dialnorm setting
SE_IN_BW			equ		8		; manual channel bandwidth
SE_IN_LPF			equ		9		; channel LPF enabled flag
SE_IN_LFELPF		equ		10		; LFE LPF flag
SE_IN_HPF			equ		11		; DC HPF flag
SE_IN_DYNRNGE		equ		12		; dynrng exists flag
SE_IN_DYNRNG		equ		13		; AC-3 dynamic range code
SE_IN_COMPRE		equ		14		; compr exists flag
SE_IN_COMPR			equ		15		; AC-3 compression code
SE_IN_SURR_GE		equ		16		; Dolby Surround gain enabled flag
SE_IN_SURR_GAIN		equ		17		; Dolby Surround gain
SE_IN_DSURMOD		equ		18		; Dolby Surround Mode.  Only used for 2/0 Mode
SE_IN_RESERVED_1	equ		19
SE_IN_RESERVED_2	equ		20
SE_IN_RESERVED_3	equ		21
SE_IN_RESERVED_4	equ		22
SE_IN_RESERVED_5	equ		23

SE_IN_MAX			equ		24		; maximum input parameter list size

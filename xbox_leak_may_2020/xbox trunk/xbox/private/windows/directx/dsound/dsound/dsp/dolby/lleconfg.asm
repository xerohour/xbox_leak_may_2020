; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			lleconfg.asm
;												
;   Contents:		System memory simulation of configuration data
;
;	Author:			jmw
;											
;	Revision:		1.00
;											
;	History:
;
;		Rev 1.00	10/14/2000 10:51:57 AM		jmw
;	Created.
;
;
; ******************************************************************

	IF		@DEF('RTSYS')

;	Dolby's Cagent 6 Development hardware

	dc		SE_IN_MAX		; input parameter list size
	dc		0				; do Dolby Surround Encoder flag
	dc		1				; do Dolby Game Encoder flag
	dc		0				; PCM sampling rate code
	dc		3				; input channel configuration
	dc		1				; low frequency effects channel present flag
	dc		16				; dialnorm setting 
	dc		5				; manual channel bandwidth 
	dc		0				; channel LPF enabled flag 
	dc		0				; LFE LPF flag 
	dc		0				; DC HPF flag
	dc		0				; dynrng exists flag
	dc		$0				; AC-3 dynamic range code 
	dc		1				; compr exists flag
	dc		$ef				; AC-3 compression code ($ef = -6dB in RF mode) 
	dc		1				; Surround gain enabled
	dc		0.562341325		; Surround gain: 0.562341325 = -5 dB, 0.177827941 = -15dB
	dc		$02				; Dolby Surround mode in 2/0 Mode (dsurmod)

	ELSE

;	NVIDIA hardware and Motorola Simulator

	dc		SE_IN_MAX		; input parameter list size
	dc		0				; do Dolby Surround Encoder flag
	dc		1				; do Dolby Game Encoder flag
	dc		0				; PCM sampling rate code
	dc		7				; input channel configuration
	dc		1				; low frequency effects channel present flag
	dc		16				; dialnorm setting 
	dc		9				; manual channel bandwidth 
	dc		1				; channel LPF enabled flag 
	dc		1				; LFE LPF flag 
	dc		1				; DC HPF flag 
	dc		0				; dynrng exists flag
	dc		$0				; AC-3 dynamic range code 
	dc		0				; compr exists flag
	dc		$ef				; AC-3 compression code 
	dc		1				; Surround gain enabled
	dc		0.562341325		; Surround gain: 0.562341325 = -5 dB, 0.177827941 = -15dB 
	dc		$02				; Dolby Surround mode in 2/0 Mode (dsurmod)

	ENDIF	; @DEF('RTSYS')	

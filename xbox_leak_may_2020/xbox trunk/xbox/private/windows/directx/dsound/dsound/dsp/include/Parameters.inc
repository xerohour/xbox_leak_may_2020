;	File	:	Parameters.inc
;	Version	:	1.0
;	Date	:	21/11/2000
;
;	Author	:	RDA
;				Sensaura Ltd
;
;
;
; Description
; ===========
; This file defines the default parameters for the reverb engine.
; It includes definitions for the position and size of the delay
; buffers when running on the Motorola DSP56362EVM board.
; Values that are specific to the DSP56362EVM board are prefixed 
; by 'EVM_'.
;
;

;
;------------------------------------------------------
;	Reverb Engine output scaling parameters
;------------------------------------------------------
FixedOutputScaling			equ	7



       
;*********************************************************************
;***  All delays and buffer lengths must be greater than 32        ***
;***  All delays and buffer lengths must be a multiple of 4 words  ***
;*********************************************************************

;------------------------------------------------------
;	Buffer sizes for the DSM56362EVM board
;	(assuming 48 kHz sample rate)
;------------------------------------------------------
EVM_SizeOf_BufferReflection0	equ	1360		;  28.33 ms
EVM_SizeOf_BufferReflection1	equ	1228		;  25.60 ms
EVM_SizeOf_BufferReflection2	equ	1284		;  26.77 ms
EVM_SizeOf_BufferReflection3	equ	1440		;  30.02 ms



	; The short reverb buffers are fixed in length according 
	; to the sample rate.  If the sample rate halves, so does
	; the buffer length.
EVM_SizeOf_BufferShortReverb0	set 2784*4		;  58.00 ms
EVM_SizeOf_BufferShortReverb1	set 4088*4		;  85.25 ms
EVM_SizeOf_BufferShortReverb2	set 3372*4		;  70.25 ms
EVM_SizeOf_BufferShortReverb3	set 4956*4		; 103.17 ms

	IF @DEF('HALFSAMPLERATE')
EVM_SizeOf_BufferShortReverb0	set	(EVM_SizeOf_BufferShortReverb0/2)&$FFFFFC		;Make the buffers half the size, but still a multiple of 4 long
EVM_SizeOf_BufferShortReverb1	set	(EVM_SizeOf_BufferShortReverb1/2)&$FFFFFC
EVM_SizeOf_BufferShortReverb2	set	(EVM_SizeOf_BufferShortReverb2/2)&$FFFFFC
EVM_SizeOf_BufferShortReverb3	set	(EVM_SizeOf_BufferShortReverb3/2)&$FFFFFC
	ENDIF



EVM_SizeOf_BufferLongReverb0	equ 31280		; 0.65 seconds
EVM_SizeOf_BufferLongReverb1	equ 31280		; 0.65 seconds

EVM_SizeOf_MainDelayLine		set 12000		; 0.25 seconds
EVM_SizeOf_MainDelayLine1		set	12000		; 0.25 seconds
EVM_SizeOf_MainDelayLine2		set	12000		; 0.25 seconds
EVM_SizeOf_MainDelayLine3		set	12000		; 0.25 seconds

	IF @DEF('HALFSAMPLERATE')
EVM_SizeOf_MainDelayLine		set (EVM_SizeOf_MainDelayLine/2)&$FFFFFC		; 0.25 seconds
EVM_SizeOf_MainDelayLine1		set	(EVM_SizeOf_MainDelayLine1/2)&$FFFFFC		; 0.25 seconds
EVM_SizeOf_MainDelayLine2		set	(EVM_SizeOf_MainDelayLine2/2)&$FFFFFC		; 0.25 seconds
EVM_SizeOf_MainDelayLine3		set	(EVM_SizeOf_MainDelayLine3/2)&$FFFFFC		; 0.25 seconds
	ENDIF



;------------------------------------------------------
;	Base address of delay line on host 
; (configured for external memory on Motorola DSP56362EVM board)
;------------------------------------------------------
EVM_MainDelayLine				equ $C000
EVM_MainDelayLine1				equ EVM_MainDelayLine+EVM_SizeOf_MainDelayLine
EVM_MainDelayLine2				equ EVM_MainDelayLine1+EVM_SizeOf_MainDelayLine1
EVM_MainDelayLine3				equ EVM_MainDelayLine2+EVM_SizeOf_MainDelayLine2

EVM_BufferReflection0			equ	EVM_MainDelayLine3+EVM_SizeOf_MainDelayLine3
EVM_BufferReflection1			equ	EVM_BufferReflection0+EVM_SizeOf_BufferReflection0
EVM_BufferReflection2			equ	EVM_BufferReflection1+EVM_SizeOf_BufferReflection1
EVM_BufferReflection3			equ	EVM_BufferReflection2+EVM_SizeOf_BufferReflection2

EVM_BufferShortReverb0			equ EVM_BufferReflection3+EVM_SizeOf_BufferReflection3
EVM_BufferShortReverb1			equ EVM_BufferShortReverb0+EVM_SizeOf_BufferShortReverb0
EVM_BufferShortReverb2			equ EVM_BufferShortReverb1+EVM_SizeOf_BufferShortReverb1
EVM_BufferShortReverb3			equ EVM_BufferShortReverb2+EVM_SizeOf_BufferShortReverb2

EVM_BufferLongReverb0			equ EVM_BufferShortReverb3+EVM_SizeOf_BufferShortReverb3
EVM_BufferLongReverb1			equ EVM_BufferLongReverb0+EVM_SizeOf_BufferLongReverb0




;
;------------------------------------------------------
;	Reverb Engine input delay parameters
;------------------------------------------------------


DefaultReflectionInputDelay	set	336		; 0 <= ReflectionDelay < 16384
												; The delay between the source sound and 
												; the first reflection tap

DefaultShortReverbInputDelay	set	524		; 0 <= ReverbInputDelay < 4800
												; The delay between the source sound and
												; the first reverb tap


												; The DefaultReflectionInputDelay# variables
												; are used when creating the default DMA 
												; parameter blocks
												; Values are relative to 'DefaultReflectionInputDelay'
DefaultReflectionInputDelay0	set	0
DefaultReflectionInputDelay1	set	56
DefaultReflectionInputDelay2	set	36
DefaultReflectionInputDelay3	set	92
DefaultReflectionInputDelay4	set	124

												; Values are relative to 'DefaultReflectionInputDelay'
DefaultShortReverbInputDelay0a	set	0
DefaultShortReverbInputDelay0b	set	116
DefaultShortReverbInputDelay1a	set	268
DefaultShortReverbInputDelay1b	set	464
DefaultShortReverbInputDelay2a	set	0
DefaultShortReverbInputDelay2b	set	116
DefaultShortReverbInputDelay3a	set	268
DefaultShortReverbInputDelay3b	set	464

												; Values are absolute. However they
												; are measured from the start of the
												; respective delay buffer
DefaultLongReverbInputDelay0a	set	100
DefaultLongReverbInputDelay1a	set	124
DefaultLongReverbInputDelay0b	set	64
DefaultLongReverbInputDelay1b	set	72
DefaultLongReverbInputDelay0c	set	232
DefaultLongReverbInputDelay1c	set	312
DefaultLongReverbInputDelay0d	set	76
DefaultLongReverbInputDelay1d	set	92


;------------------------------------------------------
;	Reverb engine delay tap positions
;------------------------------------------------------
												; Values are absolute
DefaultReflectionDelay0			set	308
DefaultReflectionDelay1			set	284
DefaultReflectionDelay2			set	296
DefaultReflectionDelay3			set	328

												; Values are absolute
DefaultShortReverbDelay0		set	EVM_SizeOf_BufferShortReverb0
DefaultShortReverbDelay1		set	EVM_SizeOf_BufferShortReverb1
DefaultShortReverbDelay2		set	EVM_SizeOf_BufferShortReverb2
DefaultShortReverbDelay3		set	EVM_SizeOf_BufferShortReverb3

												; Values are absolute
DefaultLongReverbDelay0			set	800
DefaultLongReverbDelay1			set	900


;------------------------------------------------------
;	Long Reverb Gain defaults
;------------------------------------------------------

DefaultLongReverbCrossfeedGain	set	$400000
DefaultLongReverbInputGain		set	$300000


;------------------------------------------------------
;	Default IIR parameters
;------------------------------------------------------
IIR_DefaultCoefficientA1	equ	$0
IIR_DefaultCoefficientB0	equ	$0


IIR2_DefaultInputCoefficientA1	equ $0
IIR2_DefaultInputCoefficientA2	equ $0
IIR2_DefaultInputCoefficientB0	equ $0

IIR2_InterpolationCoefficientA1	equ $0
IIR2_InterpolationCoefficientA2	equ $0
IIR2_InterpolationCoefficientB0	equ $0

                

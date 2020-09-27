;**************************************************************************
;
;  Util.h   
;
;**************************************************************************        
	
	include 'DMAUtil.h'
	include 'Io.h'
;	include 'vectors.asm'

	define rD 	  'r5'		; Standard data structure register
	define rStack 'r6'		; Stack pointer register

kGPMemory_Base_BootROM			equ $FF0000	; X-memory, 128 words
;kGPMemory_Base_PeripheralRegisters	equ $FFFF80	; X-memory, to 0xFFFFFF

INTERRUPT_TABLE_BASE 		equ $FF0000

; VBA at $FF0000 (requires 256 word alignment).  
; The vector table is exactly where the power-up reset starts fetching instructions.
; The first interrupt vector is hardware RESET.
; Only needs up to $24 (36d) .. $44 (68d) interrupts or so, not full 256 word table.

INTERRUPT_REGISTER		equ $FFFFC5	; Interrupt request/clear register
kAbortFrameBit		equ		0			; Bit 0
kStartFrameBit		equ		1			
kPutUpdateBit		equ		2
kMailInUpdateBit	equ		3
kDMAComplete1Bit	equ		4
kDMAComplete2Bit	equ		5
kDMAComplete3Bit	equ		6	
kDMAEndOfListBit	equ		7
kTimer1Bit			equ		8
kTimer2Bit			equ		9
kDMAErrorBit		equ		10
kAddressErrorBit	equ		11

; ==============================================================
;  MailBox Register stuff  
; ==============================================================
MAILBOX_OUT_REGISTER	equ $FFFFC2	; For DSP->Host messages
MAILBOX_IN_REGISTER		equ $FFFFC3	; For DSP<-Host messages

; ==============================================================
;  FIFO stuff  
; ==============================================================
FIFOOUT_CONTROL_REGISTER	equ		$FFFFC8
FIFOIN_CONTROL_REGISTER		equ		$FFFFC9
SAMPLE_CONTROL_REGISTER		equ		$FFFFCA

; Current Value is bits 23:2  (22 bits)	(from dev_apu.ref)
;NV_PAPU_GPOFCUR0  equ	$0000302C 
;NV_PAPU_GPOFCUR1  equ	$0000303C 
;NV_PAPU_GPOFCUR2  equ	$0000304C 
;NV_PAPU_GPOFCUR3  equ	$0000305C 

; End Value is bits 23:8  (16 bits) (from dev_apu.ref)
;NV_PAPU_GPOFEND0  equ	$00003028 
;NV_PAPU_GPOFEND1  equ	$00003038 
;NV_PAPU_GPOFEND2  equ	$00003048 
;NV_PAPU_GPOFEND3  equ	$00003058 

; Base Value is bits 23:8 (16 bits) (from dev_apu.ref)
;NV_PAPU_GPOFBASE0  equ	$00003024 
;NV_PAPU_GPOFBASE1  equ	$00003034 
;NV_PAPU_GPOFBASE2  equ	$00003044 
;NV_PAPU_GPOFBASE3  equ	$00003054 

; ==============================================================
;  Headroom/Valid stuff		Bits 23:4 Zero, Bit 3=Valid, Bits 2:0=Headroom, # right shifts
; ==============================================================
HEADROOM_VALID_REGISTER00	equ		$FFFF90
HEADROOM_VALID_REGISTER01	equ		$FFFF91
HEADROOM_VALID_REGISTER02	equ		$FFFF92
HEADROOM_VALID_REGISTER03	equ		$FFFF93
HEADROOM_VALID_REGISTER04	equ		$FFFF94
HEADROOM_VALID_REGISTER05	equ		$FFFF95
HEADROOM_VALID_REGISTER06	equ		$FFFF96
HEADROOM_VALID_REGISTER07	equ		$FFFF97

HEADROOM_VALID_REGISTER08	equ		$FFFF98
HEADROOM_VALID_REGISTER09	equ		$FFFF99
HEADROOM_VALID_REGISTER10	equ		$FFFF9A
HEADROOM_VALID_REGISTER11	equ		$FFFF9B
HEADROOM_VALID_REGISTER12	equ		$FFFF9C
HEADROOM_VALID_REGISTER13	equ		$FFFF9D
HEADROOM_VALID_REGISTER14	equ		$FFFF9E
HEADROOM_VALID_REGISTER15	equ		$FFFF9F

HEADROOM_VALID_REGISTER16	equ		$FFFFA0
HEADROOM_VALID_REGISTER17	equ		$FFFFA1
HEADROOM_VALID_REGISTER18	equ		$FFFFA2
HEADROOM_VALID_REGISTER19	equ		$FFFFA3
HEADROOM_VALID_REGISTER20	equ		$FFFFA4
HEADROOM_VALID_REGISTER21	equ		$FFFFA5
HEADROOM_VALID_REGISTER22	equ		$FFFFA6
HEADROOM_VALID_REGISTER23	equ		$FFFFA7

HEADROOM_VALID_REGISTER24	equ		$FFFFA8
HEADROOM_VALID_REGISTER25	equ		$FFFFA9
HEADROOM_VALID_REGISTER26	equ		$FFFFAA
HEADROOM_VALID_REGISTER27	equ		$FFFFAB
HEADROOM_VALID_REGISTER28	equ		$FFFFAC
HEADROOM_VALID_REGISTER29	equ		$FFFFAD
HEADROOM_VALID_REGISTER30	equ		$FFFFAE
HEADROOM_VALID_REGISTER31	equ		$FFFFAF

; ==============================================================
;  Timer stuff	
; ==============================================================
TIMER1_CONFIGURATION_REGISTER	equ	$FFFFB0
TIMER1_CONTROL_REGISTER			equ	$FFFFB1
TIMER1_TERMINAL_COUNT_REGISTER	equ	$FFFFB2
TIMER1_COUNT_REGISTER			equ	$FFFFB3
TIMER2_CONFIGURATION_REGISTER	equ	$FFFFB4
TIMER2_CONTROL_REGISTER			equ	$FFFFB5
TIMER2_TERMINAL_COUNT_REGISTER	equ	$FFFFB6
TIMER2_COUNT_REGISTER			equ	$FFFFB7

; ==============================================================
;  Command stuff		Bits 11:2  offset in 4KB circular buffer
; ==============================================================
kGPMemory_Base_CommandCue		equ $FFF000		; Top 4KByte page

COMMAND_GET_REGISTER 		equ $FFFFC0	; Ptr to next read location
COMMAND_PUT_REGISTER 		equ $FFFFC1	; Ptr to 1st empty write location

; ==============================================================
;  Processing control
; ==============================================================
PROCESSING_CONTROL_REGISTER		equ $FFFFC4	
kProcessingControl_SetIdleBit				equ 0		; Bit 0
kProcessingControl_NotifyInterruptBit		equ 1		

; ==============================================================
;  Global Sample Count Register		Bits 23:0
; ==============================================================
GLOBAL_SAMPLE_COUNT_REGISTER		equ $FFFFC6	

k2To7			set		     128.0
k2To8			set		     256.0
k2To15			set		   32768.0
k2To16			set		   65536.0
k2To22			set		 (64.0*k2To16)
k2To23			set		(128.0*k2To16)
k2To24			set		(256.0*k2To16)
k2To31			set		(128.0*k2To24)
k2To32			set		(256.0*k2To24)

k2To15m1		set		 (k2To15-1.0)
k2To16m1		set		 (k2To16-1.0)
k2To23m1		set		 (k2To23-1.0)
k2To24m1		set		 (k2To24-1.0)
k24Normalizer	set		 (k2To23)

Fs			set     48000.0     ; Sampling frequency
SamplingFrequency	set     48000.0              
SamplingPeriod		set     1.0/48000.0              
NormalSamplingFrequency	set     (48000.0/k2To23)              

kFs					equ     48000     
kSamplingFrequency	EQU     kFs              

PI				set     2.0*@asn(1.0)                   
kPi				set     PI                   
kTwoPi			set     2.0*kPi                   
kHalfPi			set     (kPi/2.0)                 
kQuarterPi		set     (kPi/4.0)                   

kDegreeToRadian  set     kPi/180.0   
kDegreeToPi      set	(1.0/180.0)   

kPiOverFs		set		(kPi/Fs)
kTwoPiOverFs	set		(kTwoPi/Fs)

kLog10_2	set 0.3010299956639812
kLog2_10	set (1.0/kLog10_2)	; 1/log2(10) = log10(2)

kLogE_2	    set	0.69314718055994529
kLn_2		set kLogE_2

kLog2_E		set (1.0/kLogE_2)		;  1/logE( 2) = log2(E)

;  FIXXX: go back and get the real numbers !!!
kDetuneRatio_p01CentFraction	set		0.0005777895

kDetuneRatio_m18Cents	set		0.9896566564
kDetuneRatio_m16Cents	set		0.9908006133
kDetuneRatio_m14Cents	set		0.9919458924
kDetuneRatio_m12Cents	set		0.9930924954
kDetuneRatio_m10Cents	set		0.9942404238
kDetuneRatio_m08Cents	set		0.9953896791
kDetuneRatio_m06Cents	set		0.9965402628
kDetuneRatio_m04Cents	set		0.9976921765
kDetuneRatio_m02Cents	set		0.9988454217
kDetuneRatio_m01Cents	set		0.9994225441
kDetuneRatio_m00Cents	set		1.0
kDetuneRatio_p00Cents	set		1.0
kDetuneRatio_p01Cents	set		(1.0+1.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p02Cents	set		(1.0+2.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p03Cents	set		(1.0+3.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p04Cents	set		(1.0+4.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p05Cents	set		(1.0+5.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p06Cents	set		(1.0+6.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p07Cents	set		(1.0+7.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p08Cents	set		(1.0+8.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p09Cents	set		(1.0+9.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p10Cents	set		(1.0+10.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p11Cents	set		(1.0+11.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p12Cents	set		(1.0+12.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p13Cents	set		(1.0+13.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p14Cents	set		(1.0+14.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p15Cents	set		(1.0+15.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p16Cents	set		(1.0+16.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p17Cents	set		(1.0+17.0*kDetuneRatio_p01CentFraction)
kDetuneRatio_p18Cents	set		(1.0+18.0*kDetuneRatio_p01CentFraction)

kTuneRatio_p01SemitoneFraction set 0.0594630944
kTuneRatio_p01Semitones	set 1.0594630944
kTuneRatio_p02Semitones	set 1.1224620483
kTuneRatio_p03Semitones	set	1.1892071150
kTuneRatio_p04Semitones	set	1.2599210499
kTuneRatio_p05Semitones	set	1.3348398542
kTuneRatio_p06Semitones	set	1.4142135624
kTuneRatio_p07Semitones	set	1.4983070769
kTuneRatio_p08Semitones	set	1.5874019520
kTuneRatio_p09Semitones	set	1.6817928305
kTuneRatio_p10Semitones	set	1.7817974363
kTuneRatio_p11Semitones	set	1.8877486254
kTuneRatio_p12Semitones	set	2.0

kTmpBufSize equ		1024
kTmpBufMask set		(kTmpBufSize-1)

kMixBufSize equ		32

;************************************************************
; Bound	
;************************************************************
Bound macro
Bound_IsABigger
	cmp		x0,a		; a - x
	ble		Bound_IsASmaller
	move	x0,a
	bra		Bound_AOk
Bound_IsASmaller
	cmp		y0,a		; a - y
	jge		Bound_AOk
	move	y0,a
Bound_AOk
	move	a,r1
	endm

;************************************************************
; Ceiling	
;************************************************************
Ceiling macro
Ceiling_IsABigger
	cmp		x0,a		; a - x
	ble		Ceiling_Bound_AOk
	move	x0,a
Ceiling_Bound_AOk
	move	a,r1
	endm

;************************************************************
; Wrap	
;************************************************************
Wrap macro
Wrap_IsABigger
	cmp		x0,a		; a - x0
	ble		Wrap_IsASmaller
	move	x0,a
	bra		Wrap_AOk
Wrap_IsASmaller
	cmp		y0,a		; a - y0
	jge		Wrap_AOk
	move	y0,a
Wrap_AOk
	move	a,r1
	endm

;************************************************************
; ClearBufferX_R0X0	Clear buffer at address	x:(r0) of length X0
;************************************************************
ClearBufferX_R0X0	macro
	clr		a
	rep		x0
	move	a,x:(r0)+
					endm

;************************************************************
; ClearBufferY_R0X0	Clear buffer at address	y:(r0) of length X0
;************************************************************
ClearBufferY_R0X0	macro
	clr		a
	rep		x0
	move	a,y:(r0)+
					endm

;************************************************************
; GPAddX	Add frames in XRAM
;			b = a + b	
;
;			r0	a XRAM Buffer address
;                       r1	b XRAM Buffer address
;************************************************************

GPAddX  macro
    dor      #kFrameLength,GPAddX_Loop             
	move	x:(r0)+,x0
	move	x:(r1),a
	add	x0,a
	move	a,x:(r1)+
GPAddX_Loop
 	endm
; ---- end GPAddX ----

;************************************************************
; GPAddY	Add frames in YRAM	
;			c = a + b
;
;			r0	a YRAM Buffer address
;                       r1	b YRAM Buffer address
;                       r2	c YRAM Buffer address
;************************************************************
GPAddY  macro
    dor      #kFrameLength,GPAddY_Loop             
	move	y:(r0)+,x0
	move	y:(r1)+,a
	add	x0,a
	move	a,y:(r2)+
GPAddY_Loop
 	endm
; ---- end GPAddY ----

;************************************************************
; GPScaleXAddtoY	Scale frames in XRAM then add them to
; in Y ram
;			y:(r4) = x:(r0)*x1+y:(r4)
;
;			r0	XRAM Buffer address
;           r4	YRAM Buffer address
;           x1  scaling
;************************************************************
GPScaleXAddToY macro
	move	x:(r0)+,x0
              
    dor     #kFrameLength,GPScaleXAddToY_Loop             
	mpy     x1,x0,a  x:(r0)+,x0  y:(r4),y0
    add     y0,a
	move	a,y:(r4)+
GPScaleXAddToY_Loop
 	endm

;************************************************************
; GPScaleXtoY	Scale frames in XRAM then then put
; in Y ram
;			y:(r4) = x:(r0)*x1
;
;			r0	XRAM Buffer address
;           r1	YRAM Buffer address
;           x1  scaling
;************************************************************
GPScaleXToY macro
	move	x:(r0)+,x0

    dor     #kFrameLength,GPScaleXToY_Loop             
	mpy     x1,x0,a  x:(r0)+,x0 
	move	a,y:(r1)+
GPScaleXToY_Loop
 	endm

;************************************************************
; GPScaleYAddXtoX	Add frames in YRAM	to X ram then put
; in X ram
;			x:(r2) = y:(r0)*x0 + x:(r1)
;
;			r0	a YRAM Buffer address
;           r1	b XRAM Buffer address
;           r2	c XRAM Buffer address
;           x1  scaling
;************************************************************
GPScaleYAddXToX macro
	move	y:(r4)+,y0

    dor     #kFrameLength,GPScaleYAddXToX_Loop             
	mpy     x1,y0,a  x:(r1)+,x0  y:(r4)+,y0
    add     x0,a
	move	a,y:(r2)+
GPScaleYAddXToX_Loop
 	endm
; ---- end ----

;************************************************************
; GPScaleXAddX	Add frames in XRAM	to X ram then put
; in X ram
;			x:(r1) = x:(r0)*x1 + x:(r1)
;
;			r0	a XRAM Buffer address
;           r1	b XRAM Buffer address
;           r2	c XRAM Buffer address
;           x1  scaling
;************************************************************
GPScaleXAddX macro
	move	x:(r0)+,x0

    dor     #kFrameLength,GPScaleXAddX_Loop             
	mpy     x1,x0,a  x:(r0)+,x0 
    move    x:(r1),y0
    add     y0,a
	move	a,x:(r1)+

GPScaleXAddX_Loop
 	endm
; ---- end GPAddY ----



;************************************************************
; GPAverageX	Average frames in XRAM
;			c = (a+b)/2
;
;			r0	a XRAM Buffer address
;                       r1	b XRAM Buffer address
;                       r2	c XRAM Buffer address
;************************************************************
GPAverageX macro
    dor      #kFrameLength,GPAverageX_Loop             
	move	x:(r0)+,a
	move	x:(r1)+,x0
	add	x0,a
	asr	#1,a,a
	move	a,x:(r2)+
GPAverageX_Loop
 	endm
; ---- end GPAverageX ----

;************************************************************
; GPDiffX	Take difference frames in XRAM
;			c = (a-b)/2
;
;			r0	a XRAM Buffer address
;                       r1	b XRAM Buffer address
;                       r2	c XRAM Buffer address
;************************************************************
GPDiffX macro
    dor      #kFrameLength,GPDiffX_Loop             
	move	x:(r0)+,a
	move	x:(r1)+,x0
	sub	x0,a
	asr	#1,a,a
	move	a,x:(r2)+
GPDiffX_Loop
 	endm
; ---- end GPDiffX ----

;************************************************************
; GPAccY	Accumulate frames in YRAM
;			b += a
;
;			r0	a YRAM Buffer address
;                       r1	b YRAM Buffer address
;************************************************************
GPAccY  macro
    dor      #kFrameLength,GPAccY_Loop             
	move	y:(r0)+,x0
	move	y:(r1),a
	add	x0,a
	move	a,y:(r1)+
GPAccY_Loop
 	endm
; ---- end GPAccY ----

;************************************************************
; GPAccXtoY	Accumulate XRAM frame to YRAM
;			ax += by
;
;			r0	a XRAM Buffer address
;                       r1	b YRAM Buffer address
;************************************************************
GPAccXtoY macro
    dor      #kFrameLength,GPAccXtoY_Loop             
	move	x:(r0)+,x0
	move	y:(r1),a
	add	x0,a
	move	a,y:(r1)+
GPAccXtoY_Loop
 	endm
; ---- end GPAccYtoX ----

;************************************************************
; GPAccYtoX	Accumulate YRAM frame to XRAM
;			ax += by
;
;			r0	a YRAM Buffer address
;                       r1	b XRAM Buffer address
;************************************************************
GPAccYtoX macro
    dor      #kFrameLength,GPAccYtoX_Loop             
	move	y:(r0)+,x0
	move	x:(r1),a
	add	x0,a
	move	a,x:(r1)+
GPAccYtoX_Loop
 	endm
; ---- end GPAccYtoX ----

;************************************************************
; GPScaleX	Scale frame in XRAM	
;			b = k*a
;
;			r0	a XRAM Buffer address
;                       r1	b XRAM Buffer address
;			x0	k scale factor
;************************************************************
GPScaleX macro
    dor      #kFrameLength,GPScaleX_Loop             
	move	x:(r0)+,y0
	mpy	x0,y0,a
	move	a,x:(r1)+
GPScaleX_Loop
 	endm
; ---- end GPScaleX ----

;************************************************************
; GPScaleY	Scale frame in YRAM
;			b = k*a
;
;			r0	a YRAM Buffer address
;                       r1	b YRAM Buffer address
;			x0	k scale factor
;************************************************************
GPScaleY macro
    dor      #kFrameLength,GPScaleY_Loop             
	move	y:(r0)+,y0
	mpy	x0,y0,a
	move	a,y:(r1)+
GPScaleY_Loop
 	endm
; ---- end GPScaleY ----

;************************************************************
; GPMacX	Multiply-accumulate frame in XRAM
;			b += k*a
;
;			r0	a XRAM Buffer address
;                       r1	b XRAM Buffer address
;			x0	k scale factor
;************************************************************
GPMacX  macro
    dor      #kFrameLength,GPMacX_Loop             
	move	x:(r0)+,y0
	move	x:(r1),a
	mac	x0,y0,a
	move	a,x:(r1)+
GPMacX_Loop
 	endm
; ---- end GPMacX ----
	
;************************************************************
; GPMacY	Multiply-accumulate frame in YRAM
;			b += k*a
;
;			r0	a YRAM Buffer address
;                       r1	b YRAM Buffer address
;			x0	k scale factor
;************************************************************
GPMacY  macro
    dor      #kFrameLength,GPMacY_Loop             
	move	x:(r0)+,y0
	move	x:(r1),a
	mac	x0,y0,a
	move	a,x:(r1)+
GPMacY_Loop
 	endm
; ---- end GPMacY ----
		  
;************************************************************
; GPMSubX	Multiply-subtract frame in XRAM
;			b -= k*a
;
;			r0	a XRAM Buffer address
;                       r1	b XRAM Buffer address
;			x0	k scale factor
;************************************************************
GPMSubX macro
    dor      #kFrameLength,GPMSubX_Loop             
	move	x:(r0)+,y0
	move	x:(r1),a
	mac	-x0,y0,a
	move	a,x:(r1)+
GPMSubX_Loop
 	endm
; ---- end GPMSubX ----

;************************************************************
; GPSubX	Subtract frame in XRAM
;			c = a - b	
;			r0	a XRAM Buffer address
;                       r1	b XRAM Buffer address
;                       r2	c XRAM Buffer address
;************************************************************
GPSubX  macro
    dor      #kFrameLength,GPSubX_Loop             
	move	x:(r0)+,x0
	move	x:(r1)+,a
	sub	x0,a
	move	a,x:(r2)+
GPSubX_Loop
 	endm
; ---- end GPSubX ----

;************************************************************
; GPClrX	Fill X-RAM frame with zeros
;
;			r0	XRAM Buffer address
;************************************************************
GPClrX  macro
	clr	a
    dor      #kFrameLength,GPClrX_Loop             
	move	a,x:(r0)+
GPClrX_Loop
 	endm
; ---- end GPClrX ----

;************************************************************
; GPClrY	Fill Y-RAM frame with zeros
;
;			r0	YRAM Buffer address
;************************************************************
GPClrY  macro
	clr	a
    dor      #kFrameLength,GPClrY_Loop             
	move	a,y:(r0)+
GPClrY_Loop
 	endm
; ---- end GPClrY ----

;************************************************************
; GPSetX	Fill X-RAM frame with constant
;
;			r0	XRAM Buffer address
;			x0	fill value
;************************************************************
GPSetX  macro
    dor      #kFrameLength,GPSetX_Loop             
	move	x0,x:(r0)+
GPSetX_Loop
 	endm
; ---- end GPSetX ----

;************************************************************
; GPSetY	Fill Y-RAM frame with constant
;
;			r0	YRAM Buffer address
;			x0	fill value
;************************************************************
GPSetY  macro
    dor      #kFrameLength,GPSetY_Loop             
	move	x0,y:(r0)+
GPSetY_Loop
 	endm
; ---- end GPSetY ----

GPCopyXtoXRAM macro
    dor      #kFrameLength,GPCopyXtoXRAM_Loop             
	move	x:(r0)+,x0
	move	x0,x:(r1)+
GPCopyXtoXRAM_Loop
 	endm
			
GPCopyXtoYRAM macro
    dor      #kFrameLength,GPCopyXtoYRAM_Loop             
	move	x:(r0)+,x0     
	move	x0,y:(r1)+
GPCopyXtoYRAM_Loop
 	endm

GPCopyYtoXRAM macro
    dor      #kFrameLength,GPCopyYtoXRAM_Loop             
	move	y:(r0)+,y0     
	move	y0,x:(r1)+
GPCopyYtoXRAM_Loop
 	endm


;************************************************************
; GPClearInterruptRegister	Clear  GP register
;************************************************************
GPClearInterruptRegister macro
      	movep    #$FFF,x:INTERRUPT_REGISTER	 ; "Write-1-To-Clear-One"
	endm
; ---- end GPClearInterruptRegister ----

;************************************************************
; GPClearProcessingControlRegister	Clear  GP register
;************************************************************
GPClearProcessingControlRegister macro
      	movep    #$1,x:PROCESSING_CONTROL_REGISTER	 ; "Write-1-To-Clear-One"	
	endm
; ---- end GPClearProcessingControlRegister ----

;************************************************************
; GPClearSampleControlRegister	Clear  GP register
;************************************************************
GPClearSampleControlRegister macro
      	movep    #$0,x:SAMPLE_CONTROL_REGISTER	 
	endm
; ---- end GPClearSampleControlRegister ----

;************************************************************
; GPSetIdleBit		Assert GP Idle signal (Bit 0)
;************************************************************
GPSetIdleBit macro
	movep    #1,x:PROCESSING_CONTROL_REGISTER	 ; "Write-1-To-Clear-One"
	endm
; ---- end GPSetIdleBit ----

;************************************************************
; GPClearStartFrameBit		Start Frame (Bit 1)
;************************************************************
GPClearStartFrameBit macro
	movep    #2,x:INTERRUPT_REGISTER  ; "Write-1-To-Clear-One"	
	endm
; ---- end GPClearStartFrameBit ----

;**************************************************************
; entry point standard code for avoiding re-init on every frame
;**************************************************************

FXEntryPoint macro
    IF @DEF('STANDALONE')
    move #>$40,rD
    ENDIF        
    move    x:(rD+FX_STATE_FLAGS),x0
    brset   #BIT_FX_STATE_FLAG_INITIALIZED,x0,ComputeFX               
InitFX
    bset    #BIT_FX_STATE_FLAG_INITIALIZED,x0
    move    x0,x:(rD+FX_STATE_FLAGS)
    endm


;**************************************************************
; entry point standard code assuming no init
;**************************************************************

FXEntryPointNoInit macro
    IF @DEF('STANDALONE')
    move #>$40,rD
    ENDIF        
    endm
    
;**************************************************************
; exit point standard code
;**************************************************************

FXExit macro
    IF @DEF('STANDALONE')
    bra     FX_END  
    ELSE
    rts   
    ENDIF
    endm

;**************************************************************
; macro for computing right channel, by invoking compute routine
; a second time
;**************************************************************


FXStereoComputeRightChannel macro

    ;
    ; stereo processing. set a flag telling the FX
    ; to process the right channel, call compute routine again
    ;

    move    x:(rD+FX_STATE_FLAGS),x0
    bset    #BIT_FX_STATE_FLAG_PROCESS_STEREO,x0
    move    x0,x:(rD+FX_STATE_FLAGS)           

    bsr	ComputeFXSubroutine

    ;
    ; restore flags
    ;

    move    x:(rD+FX_STATE_FLAGS),x0
    bclr    #BIT_FX_STATE_FLAG_PROCESS_STEREO,x0
    move    x0,x:(rD+FX_STATE_FLAGS)
    endm


;**************************************************************
;
; set arithmetic saturation, x0 gets clobbered
;
;**************************************************************

FXSetSMMode macro
        movec   sr,x0            ; Set arithmetic saturation 
        bset    #20,x0
        movec   x0,sr
        endm

;**************************************************************
;
; unset arithmetic saturation, x0 gets clobbered
;
;**************************************************************

FXUnsetSMMode macro
        movec   sr,x0            ; Set arithmetic saturation 
        bclr    #20,x0
        movec   x0,sr
        endm




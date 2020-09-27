;***********************************************************************
;
; AmpMod.asm    Amplitude Modulation by Waveform-Selectable Oscillator
;
;***********************************************************************        

	include 'AmpMod.h'

kAmpMod_Default_ModRate		set	1.0	; Hertz
kAmpMod_Default_ModAmount	set	1.0	  ; [0..1]% of delay length

kAmpMod_ModDepth		set	(kAmpMod_Default_ModAmount)
kAmpMod_ModScale		set	(kAmpMod_ModDepth/k2To24)

StartAmpMod 
; Data structure pointer already loaded into rD

        move    x:(rD+FX_STATE_FLAGS),x0
        brset   #BIT_FX_STATE_FLAG_INITIALIZED,x0,ComputeFXAmpMod               
        bset    #BIT_FX_STATE_FLAG_INITIALIZED,x0
        move    x0,x:(rD+FX_STATE_FLAGS)

	move	#>kAmpMod_ModScale,x0
	move	x0,x:(rD+AMPMOD_MOD_SCALE_H)

ComputeFXAmpMod

	bsr	ComputeAmpMod

        rts

;************************************************************
; ComputeAmpMod
;************************************************************
ComputeFXSubroutine
ComputeAmpMod
; rD points to Data Structure 



        ; Add bias to oscillator signal before modulation
        ; r0 = audio input
        ; r1 = audio out
        ; r2 = oscillator output(3 input to this fx..)

        ;
        ; mono case
        ;
	move	x:(rD+FX_STATE_IO2_IN0),r0
	move	x:(rD+FX_STATE_IO2_OUT0),r1
        move	x:(rD+FX_STATE_IO2_IN1),r2        

ComputeAmpMod_StartLoop

        dor      #>kFrameLength,ComputeAmpMod_Loop

        ; Shift oscillator output range from [-1..1] to [0..1]
	move	x:(r2)+,a
	asr	#1,a,a
	add	#>0.5,a
	move	a,x0
        ; Modulate !
	move	x:(r0)+,y0
	mpy	x0,y0,a

        ; check if we need to mix our output
        move    x:(rD+FX_STATE_FLAGS),x1       
        brclr   #BIT_FX_STATE_FLAG_GLOBAL_FX,x1,STOMP_OUTPUT
        move	x:(r1),x0
        add     x0,a            ; mix result to audio Output 
STOMP_OUTPUT
	move	a,x:(r1)+

ComputeAmpMod_Loop
        nop
        rts


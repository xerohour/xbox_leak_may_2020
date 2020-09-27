;***********************************************************************
;
; AmpMod.asm    Amplitude Modulation by Waveform-Selectable Oscillator
;
;***********************************************************************        

        IF @DEF('STANDALONE')
        include "util.h"
        ENDIF

	include 'AmpMod.h'

        ;
        ; worst case cycle usage by this effect (includes one time Init)
        ; stereo: 7580 cycles
        ; mono: 3781 
        ;


StartAmpMod 
; Data structure pointer already loaded into rD

        FXEntryPoint

ComputeFX

	bsr	ComputeAmpMod

        IF @DEF('STEREO')
        FXStereoComputeRightChannel
        ENDIF

        FXExit

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

        IF @DEF('STEREO')

        move    x:(rD+FX_STATE_FLAGS),x0       
        brset   #BIT_FX_STATE_FLAG_PROCESS_STEREO,x0,SETUP_RIGHT_CHANNEL

	move	x:(rD+FX_STATE_IO3_IN0),r0
	move	x:(rD+FX_STATE_IO3_OUT0),r1
        bra     ComputeAmpMod_StartLoop

SETUP_RIGHT_CHANNEL
	move	x:(rD+FX_STATE_IO3_IN1),r0
	move	x:(rD+FX_STATE_IO3_OUT1),r1
        ELSE
        ;
        ; mono case
        ;
	move	x:(rD+FX_STATE_IO2_IN0),r0
	move	x:(rD+FX_STATE_IO2_OUT0),r1
        move	x:(rD+FX_STATE_IO2_IN1),r2        

        ENDIF

ComputeAmpMod_StartLoop

        IF @DEF('STEREO')
	move	x:(rD+FX_STATE_IO3_IN2),r2        
        ENDIF

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
FX_END
        nop

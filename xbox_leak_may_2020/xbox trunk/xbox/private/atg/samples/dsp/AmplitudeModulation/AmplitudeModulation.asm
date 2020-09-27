;***********************************************************************
;
; AmpMod.asm    Amplitude Modulation by Waveform-Selectable Oscillator
;
;***********************************************************************        

        IF @DEF('STANDALONE')
        include "util.h"
        ENDIF

	include 'AmplitudeModulation.h'


StartAmpMod 
        ; register r5 (rD) contains the base offset into x-ram, where the
        ; state variables for this effect reside. The macro below
        ; has a special intruction that must always be the very first one
        ; in the effect. This allows the xgpimage tool to do code fixup
        ; while creating the effect image and replace the r5 load with the actual
        ; xram offset this effect will use

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
        ; r5 is loaded with xram offset 



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

        ; Modulate input signal using oscillator value
	move	x:(r0)+,y0
	mpy	x0,y0,a

        ; check if we need to mix our result with the existing
        ; data on the output. All effect must support this
        ; if the plan to have spekaer mixbins as output targets

        move    x:(rD+FX_STATE_FLAGS),x1       
        brclr   #BIT_FX_STATE_FLAG_GLOBAL_FX,x1,STOMP_OUTPUT
        move	x:(r1),x0
        add     x0,a            ; mix result to audio Output 
STOMP_OUTPUT
	move	a,x:(r1)+

ComputeAmpMod_Loop
        nop
        rts

        ; this is here once again for XGPIMAGE.EXE use. Effect dont use any
        ; branching but instead exectuion just flows from one effect to the next
        ; during each audio frame. the FXExit macro in the first section of this effect
        ; will alwasy branch to FX_END after the compute routine returns
FX_END
        nop

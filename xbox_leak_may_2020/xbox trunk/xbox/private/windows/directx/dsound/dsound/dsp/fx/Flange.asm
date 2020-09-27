;***********************************************************************
;
; Flange.asm    
;
;***********************************************************************        

              IF @DEF('STANDALONE')
              include "util.h"
              ENDIF
              include "flange.h"

        ;
        ; worst case cycle usage by this effect (includes one time Init)
        ; stereo: 13215 cycles (ouch)
        ; mono: 6617 
        ;



StartFlange

        FXEntryPoint

        IF @DEF('STEREO')

        ; 
        ; entire delay line is in Y memory
        ; copy some variables to right channel state
        ; then setup left channel first
        ;

        move    x:(rD+FX_DELAY_BASE),x0        
	move	x0,x:(rD+FX_DELAY_INDEX)

        ; calculate delay end index
        ; delay is length is halfed for each channel, since both channels use
        ; same delay line
	move	x:(rD+FX_DELAY_LENGTH),a
        asr     #1,a,a
        move	a,x:(rD+FX_DELAY_LENGTH)

        add     x0,a
        move    a,x:(rD+FX_DELAY_END)

        ; setup right channel

        move    x:(rD+FX_DELAY_LENGTH),x0
        move	x0,x:(rD+FX_DELAY_LENGTH+FLANGE_RIGHTCHANNEL_OFFSET)

	move	x:(rD+FX_DELAY_END),x0
	move	x0,x:(rD+FX_DELAY_INDEX+FLANGE_RIGHTCHANNEL_OFFSET)
        move	x0,x:(rD+FX_DELAY_BASE+FLANGE_RIGHTCHANNEL_OFFSET)

	move	x:(rD+FX_DELAY_LENGTH),a
        add     x0,a
        move    a,x:(rD+FX_DELAY_END+FLANGE_RIGHTCHANNEL_OFFSET)

	move	x:(rD+FLANGE_FEEDBACK),x0
	move	x0,x:(rD+FLANGE_FEEDBACK+FLANGE_RIGHTCHANNEL_OFFSET)

	move	x:(rD+FLANGE_MOD_SCALE_H),x0
	move	x0,x:(rD+FLANGE_MOD_SCALE_H+FLANGE_RIGHTCHANNEL_OFFSET)

        ELSE

        ;
        ; MONO init
        ; entire delay line is in Y memory
        ;

	move	x:(rD+FX_DELAY_BASE),x0
	move	x0,x:(rD+FX_DELAY_INDEX)

        ; calculate delay end index
	move    x:(rD+FX_DELAY_BASE),a
	move	x:(rD+FX_DELAY_LENGTH),x0
        add     x0,a
        move    a,x:(rD+FX_DELAY_END)

        move	x:(rD+FX_STATE_IO2_IN0),x0
        move    x0,x:(rD+FX_INPUT0)
        move	x:(rD+FX_STATE_IO2_IN1),x0
        move    x0,x:(rD+FX_INPUT1)

        move	x:(rD+FX_STATE_IO2_OUT0),x0
        move    x0,x:(rD+FX_OUTPUT0)

        ENDIF

ComputeFX

        IF @DEF('STEREO')
        
        ; duplicate flags for right channel

	move	x:(rD+FLANGE_FEEDBACK),x0
	move	x0,x:(rD+FLANGE_FEEDBACK+FLANGE_RIGHTCHANNEL_OFFSET)

	move	x:(rD+FLANGE_MOD_SCALE_H),x0
	move	x0,x:(rD+FLANGE_MOD_SCALE_H+FLANGE_RIGHTCHANNEL_OFFSET)

        move    x:(rD+FX_STATE_FLAGS),x0
        move    x0,x:(rD+FX_STATE_FLAGS+FLANGE_RIGHTCHANNEL_OFFSET)

        ; mirror input/output pointers from the base FX_STATE block
        ; into the ptrs for each channel

        move	x:(rD+FX_STATE_IO3_IN0),x0
        move    x0,x:(rD+FX_INPUT0)
        ; oscillator is common input for both channels
        move	x:(rD+FX_STATE_IO3_IN2),x0
        move    x0,x:(rD+FX_INPUT1)
        move	x:(rD+FX_STATE_IO3_OUT0),x0
        move    x0,x:(rD+FX_OUTPUT0)

        ; right channel

        move	x:(rD+FX_STATE_IO3_IN1),x0
        move    x0,x:(rD+FX_INPUT0+FLANGE_RIGHTCHANNEL_OFFSET)
        ; oscillator is common input for both channels
        move	x:(rD+FX_STATE_IO3_IN2),x0
        move    x0,x:(rD+FX_INPUT1+FLANGE_RIGHTCHANNEL_OFFSET)
        move	x:(rD+FX_STATE_IO3_OUT1),x0
        move    x0,x:(rD+FX_OUTPUT0+FLANGE_RIGHTCHANNEL_OFFSET)

        ENDIF

	bsr	ComputeFlange

        IF @DEF('STEREO')
        lua     (rD+FLANGE_RIGHTCHANNEL_OFFSET),rD
        bsr     ComputeFlange
        ENDIF

        FXExit

;************************************************************
; ComputeFlange:	
;************************************************************
ComputeFlange
; Data structure pointer pre-loaded in 'D' register

        ; r0 is oscillator 
        ; r1 is delay line index
        ; r2 is audio input 
        ; r3 is audio output

        
        move    x:(rD+FX_INPUT1),r0
	move	x:(rD+FX_INPUT0),r2	; Get x[n] 
        move    x:(rD+FX_OUTPUT0),r3

        dor     #kFrameLength,ComputeFlange_MonoLoop

        move    x:(r0)+,a

; Bias AC-waveform

	move    a,x0
	move    x:(rD+FLANGE_MOD_SCALE_H),y0
	mpy	x0,y0,a	

	sub	y0,a

	move    x:(rD+FX_DELAY_INDEX),x0        
	add	x0,a		

; Wrap
	move    x:(rD+FX_DELAY_END),x0
	move    x:(rD+FX_DELAY_BASE),y0
	move	x:(rD+FX_DELAY_LENGTH),x1

ComputeFlange_Mono_Wrap_IsABigger
	cmp	x0,a		; a - x0
	ble	ComputeFlange_Mono_Wrap_IsASmaller
	sub	x1,a
	bra	ComputeFlange_Mono_Wrap_AOk
ComputeFlange_Mono_Wrap_IsASmaller
	cmp	y0,a		; a - y0
	bge	ComputeFlange_Mono_Wrap_AOk
	add	x1,a
ComputeFlange_Mono_Wrap_AOk
	move	a,r1


        ; y[n] = x[n] + g*y[n-M]
ComputeFlange_Mono_Output
	move	y:(r1),x0		; Read from delay line
	move	x:(rD+FLANGE_FEEDBACK),y0
	mpy	y0,x0,a
	move	x:(r2)+,x0		; Get x[n] 
	add	x0,a
                     
        ; Update current index
	move    x:(rD+FX_DELAY_INDEX),r4
	move	a,y:(r4)+		       	; Write y[n] to delay line
	move    r4,x:(rD+FX_DELAY_INDEX)	; Store current index

        move    x:(r3),x0                       ; read existing audio output(if any)
                            
        move    x:(rD+FX_STATE_FLAGS),x1        ; read existing audio output(if any)
        brclr   #BIT_FX_STATE_FLAG_GLOBAL_FX,x1,STOMP_OUTPUT
        add     x0,a            ; mix result to audio Output 
STOMP_OUTPUT
	move	a,x:(r3)+		; add to existing output
                
        move    x:(rD+FX_DELAY_INDEX),x0            
        ; if (index >= end) index = base
	move	x:(rD+FX_DELAY_END),a
	cmp	x0,a			        ; a - x
	bge	ComputeFlange_NoUpdate
	move    x:(rD+FX_DELAY_BASE),x0         ; reset delay index  
	move    x0,x:(rD+FX_DELAY_INDEX)
ComputeFlange_NoUpdate
        nop

ComputeFlange_MonoLoop

ComputeFlange_MonoEnd

        rts

FX_END
        nop

;**************************************************************************
;
;       mixer.asm    
;       	
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'mixer.h'


MixerDefault_Gain	equ	1

;************************************************************
; StartMixer	
;************************************************************
StartMixer 
; Data structure pointer already loaded into rD

        FXEntryPoint
        
ComputeFX
        
	bsr	ComputeMixer

        FXExit



;************************************************************
; ComputeMixer
;************************************************************
ComputeMixer

        FXSetSMMode

        ; mix channels 0,1 of input data to output buffer 0
	move	x:(rD+FX_STATE_IO2_IN0),r0
        move	x:(rD+FX_STATE_IO2_IN1),r1
	move	x:(rD+FX_STATE_IO2_OUT0),r2
        clr     a

        dor     #>kFrameLength,MixLoop1
        move    x:(r2),b

        move    x:(r0)+,x0
        move    x:(rD+GAIN_IN0),y0
        mpy     x0,y0,a
        add     a,b

        move    x:(r1)+,x0
        move    x:(rD+GAIN_IN1),y0
        mpy     x0,y0,a

        add     a,b
        move    b,x:(r2)+
MixLoop1
         
        IF      @DEF('IO_4IN_2OUT')
        ; mix channels 2,3 of input data to output buffer 1

	move	x:(rD+FX_STATE_IO2_IN2),r0
        move	x:(rD+FX_STATE_IO2_IN3),r1
	move	x:(rD+FX_STATE_IO2_OUT1),r2
        clr     a

        dor     #>kFrameLength,MixLoop2
        move    x:(r2),a
        move    x:(r0)+,x0
        add     x0,a
        move    x:(r1)+,x0
        add     x0,a
        move    a,x:(r2)+
MixLoop2
        nop

        ENDIF

        FXUnsetSMMode
        nop
        rts

FX_END
        nop

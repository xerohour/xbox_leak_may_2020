;**************************************************************************
;
;       mixer.asm    
;       	
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'spliter.h'



;************************************************************
; StartSpliter	
;************************************************************
StartSpliter
; Data structure pointer already loaded into rD

        FXEntryPoint
        
ComputeFX
        
	bsr	ComputeSpliter

        FXExit



;************************************************************
; ComputeSpliter
; split channel 0 to channels 0,1,2,3,.., N of output data
;************************************************************
ComputeSpliter

        ; load base address of output IO ptr array
        lua	(rD+FX_STATE_IO1_OUT0),r1

        ; load base address of output gains
        lua     (rD+FX_OUTPUT_GAIN0),r2

        move    r2,x:(rD+FX_TEMP)
        move    x:(rD+FX_NUM_OUTPUTS),x0

        dor     x0,OutputLoop

        ; load input io ptr
	move	x:(rD+FX_STATE_IO1_IN),r0

        ; load output IO ptr
        move    x:(r1)+,r4
        ; load gain
        move    x:(r2)+,y0

        dor     #kFrameLength,MixLoop1

        ; multiple input sample by gain for output N
        move    x:(r0)+,x0
        mpy     x0,y0,a

        ; check if we need to mix our output
        move    x:(rD+FX_STATE_FLAGS),x1       
        brclr   #BIT_FX_STATE_FLAG_GLOBAL_FX,x1,STOMP_OUTPUT

        ; load sample from output bin
        move	x:(r4),x0
        ; mix result to audio Output 
        add     x0,a            

STOMP_OUTPUT
	move	a,x:(r4)+

MixLoop1
       nop
OutputLoop

        nop 
        rts

FX_END
        nop

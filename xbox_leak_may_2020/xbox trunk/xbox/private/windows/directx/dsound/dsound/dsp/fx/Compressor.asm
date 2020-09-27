;**************************************************************************
;
;       compressor.asm    
;       	
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'compressor.h'



;************************************************************
; StartSpliter	
;************************************************************
StartSpliter
; Data structure pointer already loaded into rD

        FXEntryPoint

        move    x:(rD+FX_DELAY_BASE),x0        
	move	x0,x:(rD+FX_DELAY_INDEX)
        move	x0,x:(rD+FX_OUTPUT_INDEX)

	move    x:(rD+FX_DELAY_BASE),a
	move	x:(rD+FX_DELAY_LENGTH),x0
        add     x0,a
        move    a,x:(rD+FX_DELAY_END)

        
ComputeFX
        
	bsr	ComputeSpliter

        FXExit



;************************************************************
; ComputeSpliter
; split channel 0 to channels 0,1,2,3,.., N of output data
;************************************************************
ComputeSpliter

        ; load input io ptr
	move	x:(rD+FX_STATE_IO1_IN),r0
        move	x:(rD+FX_STATE_IO1_OUT0),r1
        
	move    x:(rD+FX_DELAY_INDEX),a
	move    x:(rD+FX_DELAY_BASE),x0
        add     x0,a
        move    a,r4
               
        dor     #kFrameLength,DelayLoop
        move    x:(r0)+,x0      
        move    x0,y:(r4)+ 
        move    x0,x:(r1)+ 


DelayLoop
        nop

        ; update index
        move    r4,x:(rD+FX_DELAY_INDEX)
        move    r4,x0
       
        ; if (index >= end) index = base
	move	x:(rD+FX_DELAY_END),a
	cmp	x0,a			        ; a - x
	bge	NoUpdate
	move    x:(rD+FX_DELAY_BASE),x0         ; reset delay index  
	move    x0,x:(rD+FX_DELAY_INDEX)
NoUpdate


        ;
        ; now walk the delay line and use it to produce output
        ;

        move	x:(rD+FX_STATE_IO1_OUT0),r1
	move    x:(rD+FX_OUTPUT_INDEX),a
	move    x:(rD+FX_DELAY_BASE),x0
        add     x0,a
        move    a,r4

        move    #2,n4
        move    #2,n1

        dor     #kFrameLength/2,OutputLoop
        move    y:(r4)+n4,x0      
        move    x0,x:(r1)+ 

OutputLoop
        nop

        ; update index
        move    r4,x:(rD+FX_OUTPUT_INDEX)
        move    r4,x0
       
        ; if (index >= end) index = base
	move	x:(rD+FX_DELAY_END),a
	cmp	x0,a			        ; a - x
	bge	NoUpdateOutputIndex
	move    x:(rD+FX_DELAY_BASE),x0         ; reset delay index  
	move    x0,x:(rD+FX_OUTPUT_INDEX)
NoUpdateOutputIndex



        rts

FX_END
        nop

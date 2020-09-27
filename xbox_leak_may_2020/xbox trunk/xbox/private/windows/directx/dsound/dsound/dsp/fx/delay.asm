;**************************************************************************
;
;       Delay.asm    
;       	
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'Delay.h'


CIRCULAR_DMA            equ     1

;************************************************************
; StartDelay  
;************************************************************
StartDelay
; Data structure pointer already loaded into rD

        FXEntryPoint

        ;
        ; mono version
        ;

        bsr     PrepareDelayLineDMA
        
ComputeFX
        
	bsr	ComputeDelay

        FXExit

; ---- end StartEcho ----


;************************************************************
; ComputeDelay
;************************************************************
ComputeDelay

; dma 32 samples of the delay line in scratch memory
; stick it in temporary buffer in Y memory


; **********************************************************
        ; Read from delay line into input buffer
    	lua     (rD+DMA_NODE_READ0),r0		
        move    r0,a
        IF !@DEF('SIM')
	bsr    DMAStartNode_Wait    
        ENDIF

        ; update delay line with current input
    	lua     (rD+DMA_NODE_WRITE0),r0
        move    r0,a
        IF !@DEF('SIM')
	bsr    DMAStartNode_Wait
        ENDIF

       
        ; Add/overwrite delay data to output 
	move	x:(rD+FX_STATE_IO1_OUT1),r0
	move	x:(rD+FX_STATE_IO1_OUT0),r1

        move    x:(rD+FX_STATE_FLAGS),x1   
        brclr   #BIT_FX_STATE_FLAG_GLOBAL_FX,x1,STOMP_OUTPUT
        GPAddX  ; mix result to audio Output 
        rts

STOMP_OUTPUT
        GPCopyXtoXRAM ; stomp audio Output 	
        rts

; ---- end ComputeEcho_Stereo ----


PrepareDelayLineDMA

    lua	    (rD+DMA_NODE_READ0),r0
    move    r0,a                      
    ori     #kDMANode_EOL,a			
    
    move    a,x:(r0+kDMAWord0_NextCmd)	

    ;
    ; prepare read dma block
    ;

    move    x:(rD+FX_STATE_IO1_OUT1),a

    ; dsp mem base address
    move    #>kGPMemory_Base_XRAM,x0
    add     x0,a
    move    a,x:(r0+kDMAWord3_DSPAddress)		

    ; circular buffer base
    move    x:(rD+FX_STATE_SCRATCH_BASE),x0
    move    x0,x:(r0+kDMAWord5_SMBaseOffset)	

    ; buffer length
    move    x:(rD+FX_STATE_SCRATCH_LENGTH),a
    IF @DEF('STEREO')
    asr	    #1,a,a 
    ENDIF
    sub     #1,a
    move    a,x:(r0+kDMAWord6_SMLength)	

    move    #DMA_CIRCULAR_READ1_ISO,a
    move    a,x:(r0+kDMAWord1_ControlBits)			
    move    #kFrameLength,a1
    move    a1,x:(r0+kDMAWord2_Count)			
        
    ;
    ; prepare write dma block
    ;

    lua	    (rD+DMA_NODE_WRITE0),r0

    ; dsp mem address
    move    x:(rD+FX_STATE_IO1_IN),x0
    move    x0,x:(r0+kDMAWord3_DSPAddress)		
    
    ;
    ; circular buffer base
    ;

    move    x:(rD+FX_STATE_SCRATCH_BASE),x0
    move    x0,x:(r0+kDMAWord5_SMBaseOffset)	

    ;
    ; buffer length
    ;

    move    x:(rD+FX_STATE_SCRATCH_LENGTH),a
    sub     #1,a
    move    a,x:(r0+kDMAWord6_SMLength)	

    move    #DMA_CIRCULAR_WRITE1_ISO,a
    move    a,x:(r0+kDMAWord1_ControlBits)			
    move    #kFrameLength,a1
    move    a1,x:(r0+kDMAWord2_Count)			

    clr     a                      
    ori     #kDMANode_EOL,a			
    move    a,x:(r0+kDMAWord0_NextCmd)	
   
    rts



    IF @DEF('STANDALONE')
    include "fxDMAUtil.asm"
    ENDIF

FX_END
    nop

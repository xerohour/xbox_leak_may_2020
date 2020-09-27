;**************************************************************************
;
;       Echo.asm    
;       	
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'echo.h'


CIRCULAR_DMA            equ     1

;************************************************************
; StartEcho	
;************************************************************
StartEcho 
; Data structure pointer already loaded into rD

        FXEntryPoint

        IF @DEF('STEREO')

        ;
        ; re-create two mono state blocks, one for left channel , one for right
        ;

        ; setup scratch variable for left channel
	move	x:(rD+FX_STATE_SCRATCH_BASE),x0        
	move	x:(rD+FX_STATE_SCRATCH_LENGTH),a        

        ; set the lenght on the second block
        move    a,x:(rD+FX_STATE_SCRATCH_LENGTH+ECHO_RIGHTCHANNEL_OFFSET)

        ;half the length between channels
	asr	#1,a,a 
        add     x0,a
        ; set the scratch base on the second block
        move    a,x:(rD+FX_STATE_SCRATCH_BASE+ECHO_RIGHTCHANNEL_OFFSET)

        ; setup right channel Y-mem base
	move	x:(rD+FX_STATE_YMEM_BASE),a        
        add     #>kFrameLength*2,a
        move	a,x:(rD+FX_STATE_YMEM_BASE+ECHO_RIGHTCHANNEL_OFFSET)        

        ; setup dma control words for both channels
        dor     #>2,EchoStereoInitLoop
        bsr     EchoPrepareDelayLineDMA
        lua     (rD+ECHO_RIGHTCHANNEL_OFFSET),rD
EchoStereoInitLoop

        lua     (rD-ECHO_RIGHTCHANNEL_OFFSET*2),rD

        ELSE

        ;
        ; mono version
        ;

        move	x:(rD+FX_STATE_IO1_IN),x0
        move    x0,x:(rD+FX_INPUT0)

        move	x:(rD+FX_STATE_IO1_OUT0),x0
        move    x0,x:(rD+FX_OUTPUT0)

	move	x:(rD+FX_STATE_SCRATCH_BASE),a

        bsr     EchoPrepareDelayLineDMA

        ENDIF
        
ComputeFX
        
        IF @DEF('STEREO')

        ; copy FX flags to right channel state block

        move    x:(rD+FX_STATE_FLAGS),x0
        move    x0,x:(rD+FX_STATE_FLAGS+ECHO_RIGHTCHANNEL_OFFSET)

        ; copy gain over
        move    x:(rD+ECHO_GAIN),x0
        move    x0,x:(rD+ECHO_GAIN+ECHO_RIGHTCHANNEL_OFFSET)

        ; mirror IO ptrs

        move	x:(rD+FX_STATE_IO2_INL),x0
        move    x0,x:(rD+FX_INPUT0)

        move	x:(rD+FX_STATE_IO2_OUTL),x0
        move    x0,x:(rD+FX_OUTPUT0)

        move	x:(rD+FX_STATE_IO2_INR),x0
        move    x0,x:(rD+FX_INPUT0+ECHO_RIGHTCHANNEL_OFFSET)

        move	x:(rD+FX_STATE_IO2_OUTR),x0
        move    x0,x:(rD+FX_OUTPUT0+ECHO_RIGHTCHANNEL_OFFSET)

        ENDIF

	bsr	ComputeEcho	

        IF @DEF('STEREO')

        ;
        ; update rD to point to state data for right channel
        ; the compute echo for right channel
        ;

        lua (rD+ECHO_RIGHTCHANNEL_OFFSET),rD
        bsr     ComputeEcho

        ENDIF

        FXExit

; ---- end StartEcho ----


;************************************************************
; ComputeEcho
;************************************************************
ComputeEcho	

; dma 32 samples of the delay line in scratch memory
; stick it in temporary buffer in Y memory


; **********************************************************
        ; Read from delay line into input buffer
    	lua     (rD+DMA_NODE_READ0),r0		
        move    r0,a
        IF !@DEF('SIM')
	bsr    DMAStartNode_Wait    
        ENDIF

        ; Scale input by feedback gain
	move	x:(rD+FX_DELAY_BUFFER_IN),r0
        move	x:(rD+FX_DELAY_BUFFER_OUT),r1
	move	x:(rD+ECHO_GAIN),x0
        GPScaleY
 
        ; Add input data to output buffer
	move	x:(rD+FX_INPUT0),r0
	move	x:(rD+FX_DELAY_BUFFER_OUT),r1
        GPAccXtoY 
                  
        ; Write sum into delay line
    	lua     (rD+DMA_NODE_WRITE0),r0
        move    r0,a
        IF !@DEF('SIM')
	bsr    DMAStartNode_Wait
        ENDIF

        ; Add/overwrite delay data to output 
	move	x:(rD+FX_DELAY_BUFFER_IN),r0
	move	x:(rD+FX_OUTPUT0),r1

        move    x:(rD+FX_STATE_FLAGS),x1   
        brclr   #BIT_FX_STATE_FLAG_GLOBAL_FX,x1,STOMP_OUTPUT
        GPAccYtoX    ; mix result to audio Output 
        rts

STOMP_OUTPUT
        GPCopyYtoXRAM ; stomp audio Output 	
        rts

; ---- end ComputeEcho_Stereo ----


EchoPrepareDelayLineDMA

    lua	    (rD+DMA_NODE_READ0),r0
    move    r0,a                      
    ori     #kDMANode_EOL,a			
    
    move    a,x:(r0+kDMAWord0_NextCmd)	

    ;
    ; prepare read dma block
    ;

    move    x:(rD+FX_STATE_YMEM_BASE),a
    move    a,x:(rD+FX_DELAY_BUFFER_IN)

    ; dsp mem base address
    move    #>kGPMemory_Base_YRAM,x0
    add     x0,a
    move    a,x:(r0+kDMAWord3_DSPAddress)		

    ; zero out sm offset
    move    #0,x0
    move    x0,x:(r0+kDMAWord4_SMOffset)	

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
    move    x:(rD+FX_STATE_YMEM_BASE),a
    move    #>kFrameLength,x0
    add     x0,a

    ; save delay buffer
    move    a,x:(rD+FX_DELAY_BUFFER_OUT)

    move    #>kGPMemory_Base_YRAM,x0
    add     x0,a
    move    a,x:(r0+kDMAWord3_DSPAddress)		
    
    ; zero out sm offset
    move    #0,x0
    move    x0,x:(r0+kDMAWord4_SMOffset)	

    ;
    ; circular buffer base
    ;

    move    x:(rD+FX_STATE_SCRATCH_BASE),x0
    move    x0,x:(r0+kDMAWord5_SMBaseOffset)	

    ;
    ; buffer length
    ;

    move    x:(rD+FX_STATE_SCRATCH_LENGTH),a
    IF @DEF('STEREO')
    asr	    #1,a,a 
    ENDIF
    sub     #1,a
    move    a,x:(r0+kDMAWord6_SMLength)	

    move    #DMA_CIRCULAR_WRITE1_ISO,a
    move    a,x:(r0+kDMAWord1_ControlBits)			
    move    #kFrameLength,a1
    move    a1,x:(r0+kDMAWord2_Count)			

    clr     a                      
    ori     #kDMANode_EOL,a			
    move    a,x:(r0+kDMAWord0_NextCmd)	
   
    ;
    ; clear delay lines
    ;

    clr	a
    move    x:(rD+FX_DELAY_BUFFER_IN),r0
    move    x:(rD+FX_DELAY_BUFFER_OUT),r1
    dor     #kFrameLength,ClearDelayLine
    move    a,y:(r0)+
    move    a,y:(r1)+
ClearDelayLine
    rts



       IF @DEF('STANDALONE')
       include "fxDMAUtil.asm"
       ENDIF

FX_END
        nop

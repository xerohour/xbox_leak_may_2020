;**************************************************************************
;
;       rateconv.asm    
;       	
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'rateconv.h'

CIRCULAR_DMA            equ     1


;************************************************************
; StartSRC	
;************************************************************
StartSRC
; Data structure pointer already loaded into rD

    FXEntryPoint
    move    #1,x0
    move    x0,x:(rD+FX_INDEX)
    bsr     PrepareDelayLineDMA
        
ComputeFX
        
    bsr	ComputeSRC

    FXExit



;************************************************************
; ComputeSRC
;************************************************************
ComputeSRC


    ; decimate the current input by using the SRC step parameter
    move    x:(rD+FX_STATE_IO1_IN),r0
    move    x:(rD+FX_TEMP_BUFFER),r1
    move    x:(rD+FX_INDEX),a
    move    x:(rD+FX_SRC_STEP),y0

    ; keep track of the # values we write into the temp buffer
    clr     b

    dor     #kFrameLength,Loop

    ; read input sample
    move    x:(r0)+,x0      

    cmp     y0,a
    blt     Skip
    ; clear index
    clr     a
    ; take input sample and move it into temp buffer
    move    x0,x:(r1)+ 
    add     #1,b

Skip
    add     #1,a

Loop
    ; save current index
    move    a,x:(rD+FX_INDEX)

    ; do write DMA
    lua     (rD+DMA_NODE_WRITE0),r0

    ; update word count for dma write block to reflect how many
    ; words we put in the delay buffer in this iteration
    move    b,x:(r0+kDMAWord2_Count)			

    move    r0,a
    IF !@DEF('SIM')
    bsr    DMAStartNode_Wait
    ENDIF
  
    rts

;************************************************************
; PrepareDelayLineDMA
;************************************************************

PrepareDelayLineDMA
        
    ;
    ; prepare write dma block
    ;

    lua	    (rD+DMA_NODE_WRITE0),r0

    ; dsp mem address
    move    x:(rD+FX_TEMP_BUFFER),a
    move    #>kGPMemory_Base_XRAM,x0
    add     x0,a
    move    a,x:(r0+kDMAWord3_DSPAddress)		
    
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

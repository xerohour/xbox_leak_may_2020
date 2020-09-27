;**************************************************************************
;
;       minirvb.asm    
;       	
;**************************************************************************        
     
        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'minirvb.h'

        org    p:

        IF @DEF('STANDALONE')
CIRCULAR_DMA            equ     1
        ENDIF


;************************************************************
; StartReverb	
;************************************************************
StartReverb 
; Data structure pointer already loaded into rD

        FXEntryPoint

        IF @DEF('FX_TEST')

        ;
        ; fill in variable sin xram to simulate runtime conditions
        ;
            

        ; setup scratch

        move    #$C000,x0
        move    x0,x:(rD+FX_STATE_SCRATCH_BASE)

        move    #>kMixBuf0,x0     
	move	x0,x:(rD+FX_STATE_IO1_IN)                                      ; LEFT INPUT

        move    #>kReadWriteMixBuf00,x0         ; front left
        move	x0,x:(rD+FX_STATE_IO1_OUT0) 
        move    #>kReadWriteMixBuf01,x0         ; front right
        move	x0,x:(rD+FX_STATE_IO1_OUT1) 
        move    #>kReadWriteMixBuf04,x0         ; rl
        move	x0,x:(rD+FX_STATE_IO1_OUT2) 
        move    #>kReadWriteMixBuf05,x0         ; rl
        move	x0,x:(rD+FX_STATE_IO1_OUT3) 

        ; setup temp bins
        move    #>$cb,x0 
	move	x0,x:(rD+FX_STATE_IO1_OUT3+1) 
        move    #>$eb,x0 
	move	x0,x:(rD+FX_STATE_IO1_OUT3+2) 
        move    #>$10b,x0 
	move	x0,x:(rD+FX_STATE_IO1_OUT3+3) 
        move    #>$12b,x0 
	move	x0,x:(rD+FX_STATE_IO1_OUT3+4) 

        ; setup reflection lengths

        move    #>1024,x0 
	move	x0,x:(rD+Reflection0Length-StateBlock) 
        move    #>2048,x0 
	move	x0,x:(rD+Reflection1Length-StateBlock) 
        move    #>3072,x0 
	move	x0,x:(rD+Reflection2Length-StateBlock) 
        move    #>4096,x0 
	move	x0,x:(rD+Reflection3Length-StateBlock) 

        ; setup reflection taps

        move    #>124,x0 
	move	x0,x:(rD+Reflection0Tap-StateBlock) 
        move    #>248,x0 
	move	x0,x:(rD+Reflection1Tap-StateBlock) 
        move    #>372,x0 
	move	x0,x:(rD+Reflection2Tap-StateBlock) 
        move    #>496,x0 
	move	x0,x:(rD+Reflection3Tap-StateBlock) 

        ENDIF

        bsr     PrepareDelayLineDMA

ComputeFX
	bsr	ComputeReverb	

        FXExit

; ---- end StartEcho ----


;************************************************************
; ComputeEcho
;************************************************************
ComputeReverb	

; dma 32 samples of the delay line in scratch memory
; stick it in temporary buffer in Y memory


; **********************************************************
        ; Read all reflections from delay line into input buffer
        move    rD,a
        add     #(DMAReadBlock0-StateBlock),a		
        move    a,r0

        IF !@DEF('SIM')
	bsr    DMAStartNode_Wait    
        ENDIF

        ; Scale the direct path input data
	move	x:(rD+FX_STATE_IO1_IN),r0
	move	r0,r1
        move    #$400000,x0
        bsr     ScaleX 
                  
        ; scale first reflection

	move	x:(rD+pTemp-StateBlock),r0
        move    r0,r1
        move    x:(rD+Reflection0Gain-StateBlock),x0
        bsr     ScaleX

	move	x:(rD+pReflection1-StateBlock),r0
	move	x:(rD+pTemp-StateBlock),r1
        move    x:(rD+Reflection1Gain-StateBlock),x0
        bsr     ScaleXAddX

	move	x:(rD+pReflection2-StateBlock),r0
	move	x:(rD+pTemp-StateBlock),r1
        move    x:(rD+Reflection2Gain-StateBlock),x0
        bsr     ScaleXAddX

	move	x:(rD+pReflection2-StateBlock),r0
	move	x:(rD+pTemp-StateBlock),r1
        move    x:(rD+Reflection2Gain-StateBlock),x0
        bsr     ScaleXAddX

	move	x:(rD+pReflection3-StateBlock),r0
	move	x:(rD+pTemp-StateBlock),r1
        move    x:(rD+Reflection3Gain-StateBlock),x0
        bsr     ScaleXAddX

	move	x:(rD+pReflection4-StateBlock),r0
	move	x:(rD+pTemp-StateBlock),r1
        move    x:(rD+Reflection4Gain-StateBlock),x0
        bsr     ScaleXAddX

	move	x:(rD+pReflection5-StateBlock),r0
	move	x:(rD+pTemp-StateBlock),r1
        move    x:(rD+Reflection5Gain-StateBlock),x0
        bsr     ScaleXAddX

	move	x:(rD+pReflection6-StateBlock),r0
	move	x:(rD+pTemp-StateBlock),r1
        move    x:(rD+Reflection6Gain-StateBlock),x0
        bsr     ScaleXAddX

	move	x:(rD+pReflection7-StateBlock),r0
	move	x:(rD+pTemp-StateBlock),r1
        move    x:(rD+Reflection7Gain-StateBlock),x0
        bsr     ScaleXAddX

        ;
        ; add direct path to temp results
        ;

	move	x:(rD+FX_STATE_IO1_IN),r0
	move	x:(rD+pTemp-StateBlock),r1
        bsr     AddX


        ;
        ; filter reflection sum
        ;

        move    rD,a
        add     #(IIR0-StateBlock),a
        move    a,r1
        move	x:(rD+pTemp-StateBlock),r0
        bsr     IIR_InPlace

        move    rD,a
        add     #(IIR1-StateBlock),a
        move    a,r1
        move	x:(rD+pTemp-StateBlock),r0
        bsr     IIR_InPlace


        ; Write result into delay line
        move    rD,a
    	add     #(DMAWriteBlock0-StateBlock),a
        move    a,r0

        IF !@DEF('SIM')
	bsr    DMAStartNode_Wait
        ENDIF

        move    x:(rD+FX_STATE_FLAGS),x1   
        brclr   #BIT_FX_STATE_FLAG_GLOBAL_FX,x1,STOMP_OUTPUT

	
	lua	(rD+FX_STATE_IO1_OUT0),r2
        dor     #4,MixOutputLoop

	move	x:(rD+pTemp-StateBlock),r0
	move	x:(r2)+,r1
        bsr     AddX    
MixOutputLoop
        nop

        rts

STOMP_OUTPUT
	move	x:(rD+pTemp-StateBlock),r0
	lua	(rD+FX_STATE_IO1_OUT0),r2

        dor     #4,StompOutputLoop
	move	x:(rD+pTemp-StateBlock),r0
	move	x:(r2)+,r1

        bsr     CopyXtoXRAM ; stomp audio Output 	

StompOutputLoop
        nop

        rts

;-------------------------------------------

IIR_InPlace
	move	x:(r1+IIR_CoefficientA1),y0
	move	x:(r1+IIR_CoefficientB0),y1

	dor	#kFrameLength,IIRLoop
		move	x:(r1+IIR_DelayElement),x0
		mpy	x0,y0,a      x:(r0),x1
		mac	x1,y1,a

		move	a,x:(r1+IIR_DelayElement)
		move	a,x:(r0)+
IIRLoop
	rts

;-----------------------------------------------------------


PrepareDelayLineDMA

    ;
    ; create the read dma blocks
    ;

    move    rD,a
    add     #(DMAReadBlock0-StateBlock),a
    move    a,r0

    move    #kDMANode_MaxSize,n0

    move    rD,a
    add     #(DMAWriteBlock0-StateBlock),a
    move    a,r4

    move    #kDMANode_MaxSize,n4

    ; we make the assumption that the pTemp ptr and pReflectionXX
    ; are right next to each to other.Relfection0 gest DMAed in pTemp

    lua     (rD+pTemp-StateBlock),r1
    lua     (rD+Reflection0Length-StateBlock),r2
    lua     (rD+Reflection0Tap-StateBlock),r3

    dor     #NUM_REFLECTIONS,ExternalReadDmaLoop

        ; word3, dsp mem base address
        move    #>kGPMemory_Base_XRAM,a
        move    x:(r1)+,x0
        add     x0,a
        move    a,x:(r0+kDMAWord3_DSPAddress)		

        ; WRITEDMA word3, dsp mem base address
        move	x:(rD+pTemp-StateBlock),x0
        move    x0,x:(r4+kDMAWord3_DSPAddress)		

        move    x:(rD+FX_STATE_SCRATCH_BASE),a
        ;add delay tap to delay base
        move    x:(r3)+,x0
        add     x0,a        
        ;word5, circular buffer base
        move    a,x:(r0+kDMAWord5_SMBaseOffset)	

        ; writedma, word5
        move    a,x:(r4+kDMAWord5_SMBaseOffset)	


        ; read length
        move    x:(r2)+,x0
        move    x0,x:(r0+kDMAWord6_SMLength)	

        move    x0,x:(r4+kDMAWord6_SMLength)	

        ; word1, control bits
        move    #DMA_CIRCULAR_READ1_ISO,a
        move    a,x:(r0+kDMAWord1_ControlBits)			
        ; write dma
        move    #DMA_CIRCULAR_WRITE1_ISO,a
        move    a,x:(r4+kDMAWord1_ControlBits)			

        ; word2,  count
        move    #kFrameLength,a1
        move    a1,x:(r0+kDMAWord2_Count)			
        ; write dma
        move    a1,x:(r4+kDMAWord2_Count)			

        ; word0, nextCmd
        move    r0,a
        add     #kDMANode_MaxSize,a
        move    a,x:(r0)+n0	

        move    r4,a
        add     #kDMANode_MaxSize,a
        move    a,x:(r4)+n4	


ExternalReadDmaLoop

    ;
    ; on the last read block, set EOL bit
    ;

    clr     a
    ori     #kDMANode_EOL,a			    
    move    a,x:(r0-kDMANode_MaxSize)	
        
    clr     a
    ori     #kDMANode_EOL,a			    
    move    a,x:(r4-kDMANode_MaxSize)	

   
    rts

;-----------------------------------------------------------
CopyXtoXRAM
    dor      #kFrameLength,CopyXtoXRAM_Loop             
	move	x:(r0)+,x0
	move	x0,x:(r1)+
CopyXtoXRAM_Loop
 	

;-----------------------------------------------------------

AddX  
    dor      #kFrameLength,AddX_Loop             
	move	x:(r0)+,x0
	move	x:(r1),a
	add	x0,a
	move	a,x:(r1)+
AddX_Loop 	
    rts

;-----------------------------------------------------------

ScaleX 
    dor      #kFrameLength,ScaleX_Loop             
	move	x:(r0)+,y0
	mpy	x0,y0,a
	move	a,x:(r1)+
ScaleX_Loop
    rts
;-----------------------------------------------------------

;-----------------------------------------------------------

ScaleXAddX 
    dor      #kFrameLength,ScaleXAddX_Loop             
	move	x:(r0)+,y0
	mpy	x0,y0,a    x:(r1),x1
        add     x1,a
	move	a,x:(r1)+
ScaleXAddX_Loop
    rts
;-----------------------------------------------------------

    IF @DEF('STANDALONE')
    include "fxDMAUtil.asm"
    ENDIF

FX_END
    nop

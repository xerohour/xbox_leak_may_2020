;*******************************************************************
;  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
; 
;  Author: George Chrysanthakopoulos (georgioc)
;  Creation Date: 4/1/2001
;  Comments: default execution engine for downloading FX code and doing basic
;  passthrough DMA from VP-> gp FIFOs (system ram)
;******************************************************************        
        include 'Util.h'

CIRCULAR_DMA            equ     0

        org     x:kGPMemory_XRAM_Base

commandBlock                    ds      DATASIZE_COMMAND_BLOCK  

dmaCommandBlock                 ds      kDMANode_Size_DSPnSM
dmaStateXRAMNode                ds      kDMANode_Size_DSPnSM
dmaCodeNode                     ds      kDMANode_Size_DSPnSM
dmaWriteCommandBlock            ds      kDMANode_Size_DSPnSM
dmaMultipassWriteCommandBlock   ds      kDMANode_Size_DSPnSM


AC3_DMA_TEMP                    dc      0
DMA_BLOCK_BASE           	ds      kDMANode_Size_FIFO+(6*kDMANode_MaxSize)

MAX_GP_CYCLES                   equ 106000      ; 160Mhz

DMA_CONTROL_WORD8		equ DMA_BLOCK_BASE+$7
DMA_CONTROL_WORD9		equ $0059D2

DMA_CONTROL_WORD10		equ kFrameLength
DMA_CONTROL_WORD12		equ $000000			;System memory offset
DMA_CONTROL_WORD14		equ $0007ff			;System memory size minus one

        org     x:DEBUG_VARIABLES_OFFSET

        IF @DEF('DEBUG')
FX_ELAPSED_TIME                 ds      1
FX_PREV_TIME                    ds      1
ELAPSED_TIME                    ds      1
PREV_TIME                       ds      1
        ENDIF

        org     x:GLOBAL_FX_STATE_XRAM_BASE_OFFSET
dspStateBlockXRAM               ds      DATASIZE_GLOBAL_FX_STATE

;***********************************************************
        org     p:$0      ; Place just before START

START
main
        IF !@DEF('SIM')
        jsr     GPInit          
        ENDIF


        ;
        ; Set up DMA to read GP command block
        ; create a an array of DMA words that read the command block
        ; 

        move    #dmaCommandBlock,r0
        move    #(kGPMemory_XRAM_Base+commandBlock),r1          
        move    #SM_OFFSET_COMMAND_BLOCK,r2                     
        move    #DATASIZE_COMMAND_BLOCK,r3                      

        ;
        ; prepare the dma words
        ;

        IF !@DEF('SIM')
        jsr     DMANewNode_Read_Eol     
        ENDIF

        ;
        ; set up DMA to write back to system memory an updated
        ; command block...
        ;

        move    #dmaWriteCommandBlock,r0
        move    #commandBlock,r1          
        move    #SM_OFFSET_COMMAND_BLOCK,r2                     
        move    #DATASIZE_COMMAND_BLOCK,r3                      

        ;
        ; prepare the dma words
        ;

        IF !@DEF('SIM')
        jsr     DMANewNode_Write_Eol     
        jsr     InitFIFOAndEPDma
        ENDIF

        ;
        ; set up DMA to write back to system memory all the FX send
        ; mixbins in one concatenated buffer...
        ;

        move    #dmaMultipassWriteCommandBlock,r0
        move    #kFxSendMixBuf00,r1          
        move    #SM_OFFSET_MULTIPASS_BUFFER,r2                     
        move    #DATASIZE_MULTIPASS_BUFFER,r3                      

        ;
        ; prepare the dma words
        ;

        IF !@DEF('SIM')
        jsr     DMANewNode_Write_Eol     
        ENDIF

        ; modify the control bits for multipass write dma
        move    #>DMA_MULTIPASS_WRITE1,x0
        move    x0,x:(r0+1)

        IF @DEF('DEBUG')

        ;
        ; init timer variables
        ;

        move    #0,x0
        move    x0,x:PREV_TIME
        move    x0,x:ELAPSED_TIME

        ; setup timer registers

        movep   #$FFFFFF,x:TIMER1_TERMINAL_COUNT
        movep   #1,x:TIMER1_CONFIG

        ; start the timer from 0
        movep   #1,x:TIMER1_CONTROL

        ENDIF


;*********************************************************
; Main loop
;*********************************************************
MainLoop

        GPSetIdleBit    

        IF !@DEF('SIM')
        jsr             GPWaitForStartFrame  
        ENDIF

        IF @DEF('DEBUG')

        ;
        ; read the time counter now
        ;

        move    x:TIMER1_COUNT,x0
        move    x0,x:PREV_TIME

        ENDIF

        jsr             GPProcessCommandBlock

        IF @DEF('DEBUG')

        ;
        ; read the time counter now
        ;

        move    x:TIMER1_COUNT,x0
        move    x0,x:FX_PREV_TIME

        ENDIF

        ;
        ; the code block should be have already been moved into P-ram
        ; jump to the first line of the first FX in the code block
        ;

        IF @DEF('FX_TEST')

        jsr     PrepareAndExecuteFX        

        ELSE

        jsr     FX0_BASE    

        ENDIF

        IF @DEF('DEBUG')

        ; see how much the FX took 

        clr     a
        move    x:TIMER1_COUNT,a1
        move    x:FX_PREV_TIME,x0
        cmpu    x0,a
        blt     <MainLoop_StartDMA     

        move    a1,a
        sub     x0,a        
        move    a1,x:FX_ELAPSED_TIME       	

        ENDIF

        ;
        ; after all the FX execute, an rts instruction should bring execution
        ; back into the following line
        ; Using the dma fifo words we setup at start of this program,
        ; we DMA X-RAM MixBufs 0&1 -> Out Fifo #0 and mixbufs0->5 to AC3 output
        ;

        IF !@DEF('SIM') 

MainLoop_StartDMA

        move	#>DMA_BLOCK_BASE,a
	jsr	DMAStartNode_Wait

        ;
        ; do the multipass DMA
        ;

        move    #>dmaMultipassWriteCommandBlock,a
        jsr	DMAStartNode_Wait

        ENDIF

        IF @DEF('DEBUG')

        ;
        ; see how much this iteration took
        ; 

        clr     a
        move    x:TIMER1_COUNT,a1
        move    x:PREV_TIME,x0
        cmpu    x0,a
        blt     <MainLoop     

        move    a1,a
        sub     x0,a        
        move    a1,x:ELAPSED_TIME       	

        ; check if we exceed max cycles

        move    #>MAX_GP_CYCLES,x0
        cmpu    x0,a
        blt     <MainLoop

        ; we did, halt the DSP

        ;bra     <*

        ENDIF

        bra     <MainLoop                    
; ---- end MainLoop ----


;************************************************************
; GPProcessCommandBlock         Load and inspect command block
;
;************************************************************
GPProcessCommandBlock

LoadCommandNode    
            
        ;
        ; perform the actual dma that brings in the command block data struct
        ;

        move    #>dmaCommandBlock,a         
        IF !@DEF('SIM')
        jsr     DMAStartNode_Wait 
        ENDIF

        ;
        ; Check if a command has been submitted from the host
        ;
          
        move    x:(commandBlock+COMMAND_BLOCK_FIELD_COMMAND_FLAGS),x0
        move    #>0,a            
        cmp     x0,a
        beq     GPProcessCommandBlock_End

        ;
        ; check if they want us to download new code
        ;

        jclr    #BIT_COMMAND_BLOCK_COMMAND_LOAD_CODE,x:(commandBlock+COMMAND_BLOCK_FIELD_COMMAND_FLAGS),LoadBlock_DSPStateXRAM

LoadBlock_DSPCode

        ;
        ; always X and Y ram before loading new code
        ;

	move    #>GLOBAL_FX_STATE_XRAM_BASE_OFFSET,r0
	move    #>(kGPMemory_Size_XRAM-GLOBAL_FX_STATE_XRAM_BASE_OFFSET),x0
        jsr     GPClearXRAM_All
        jsr     GPClearYRAM_All	

        ;
        ; set linear addressing mode
        ;

	move	#$ffffff,m0
        move    m0,m1
	move	m0,m2	   
	move	m0,m3
	move	m0,m4
	move	m0,m5

        ;
        ; DMA in the concatenated blocks of DSP FX code
        ;

        move    #>dmaCodeNode,r0
        ; specify P-RAM offset to place DSP code into
        move    #>(kGPMemory_Base_PRAM+FX0_BASE),r1         

        ; specify SM offset to read code words from.
        move    #>SM_OFFSET_CODE_BLOCK,r2

        ; specify the transfer length using command block dwCodeLength field
        move    x:(commandBlock+COMMAND_BLOCK_FIELD_CODE_LENGTH),r3                      

        ;
        ; create the DMA words
        ;

        jsr     DMANewNode_Read_Eol     
        move    #>dmaCodeNode,a               

        ;
        ; execute dma program
        ;

        IF !@DEF('SIM')
        jsr     DMAStartNode_Wait
        ENDIF

LoadBlock_DSPStateXRAM    

        ;
        ; see if they want us to update state block
        ;
                
        jclr    #BIT_COMMAND_BLOCK_COMMAND_LOAD_XRAM_STATE,x:(commandBlock+COMMAND_BLOCK_FIELD_COMMAND_FLAGS),GPProcessCommandBlock_End
        
        ;
        ; DMA in the concatenated blocks of state variables for all FX
        ; the state block starts after the command block and the dsp code block
        ;

        move    #>dmaStateXRAMNode,r0

        ; create Xram offset
        move    #>(kGPMemory_Base_XRAM+dspStateBlockXRAM),a         
        move    x:(commandBlock+COMMAND_BLOCK_FIELD_OFFSET),x0
        add     x0,a
        move    a,r1

        ; specify SM offset
        move    x:(commandBlock+COMMAND_BLOCK_FIELD_STATE_OFFSET),x0
        move    x0,r2

        ; specify the transfer length using command block dwStateLength field
        move    x:(commandBlock+COMMAND_BLOCK_FIELD_STATE_LENGTH),r3                     

        ;
        ; create the DMA words
        ;

        jsr     DMANewNode_Read_Eol     
        move    #>dmaStateXRAMNode,a                

        ;
        ; execute dma program
        ;

        IF !@DEF('SIM')
        jsr     DMAStartNode_Wait
        ENDIF

        ;
        ; after reading an active command node, update the system memory command node
        ; zero-ing out the command flags. this way we will not be doing DMA on every cycle..
        ;

        move #>0,x0
        move x0,x:(commandBlock+COMMAND_BLOCK_FIELD_COMMAND_FLAGS)
        move #>dmaWriteCommandBlock,a                

        ;
        ; execute dma program
        ;

        IF !@DEF('SIM')
        jsr     DMAStartNode_Wait   
        ENDIF

GPProcessCommandBlock_End

        rts

; ---- end GPProcessCommandNode ----
        

InitFIFOAndEPDma

        ;
        ; Create the 5 word DMA node used to spit interleaved stereo
        ; out to system memory. Only used for debug and DVT with no REVB
        ;

	move 	#>DMA_BLOCK_BASE,r4      	  

        ; 
        ; the scratch offset were we deliver the 6 buffer 512 sample interleave
        ; is communicated by the driver
        ;

        move    #>SM_OFFSET_AC3_DMA_OUTPUT,x0  
	move	#>AC3_DMA_TEMP,r1
        move    x0,x:(r1)

	move	#>DMA_CONTROL_WORD8,a		;Next DMA Control Structure
	move	#>DMA_MIXBUFFER_MAP,r2

        ;
        ; Create 6 AC3 DMA control structures
        ; They will dma out 512 samples for each mixbin (0->5)
        ; The base system memory offset is set by the mcpx driver(it pokes it in our XRAM)
        ;

	dor	#6,AC3_DMA_LOOP

	move    a,x:(r4)+
	add	#>kDMANode_MaxSize,a   				;Increment next DMA control struct pointer	

	move    #>DMA_CONTROL_WORD9,x0			
	move    x0,x:(r4)+	
	move    #>DMA_CONTROL_WORD10,x0			
	move    x0,x:(r4)+	

	move	p:(r2)+,x0					; Get Mixbuffer address from table
	move    x0,x:(r4)+

	move    #>DMA_CONTROL_WORD12,x0			
	move    x0,x:(r4)+
		
	move	x:(r1),b
	move	b,x:(r4)+
	add	#>AC3_BUFFER_SIZE,b			;Increment system memory base 
	move    b,x:(r1)
		
	move    #>DMA_CONTROL_WORD14,x0			
	move    x0,x:(r4)+	
AC3_DMA_LOOP

        ; put an EOL on the last nodes nextCmd word

        move	#>DMA_BLOCK_BASE,a		        ;base DMA Control Structure
	jsr	DMAInit

        add	#>kDMANode_MaxSize*5,a
        move    a,r4

	move	#>$4000,x0 ; EOL bit in word0
        move    x0,x:(r4)+	
        

        rts

DMA_MIXBUFFER_MAP:
	dc	kMixBuf00                               ; front left
	dc	kMixBuf02                               ; center
	dc	kMixBuf01                               ; front right
	dc	kMixBuf04                               ; rear left
	dc	kMixBuf05                               ; rear right
	dc	kMixBuf03                               ; LFE


        IF @DEF('FX_TEST')
PrepareAndExecuteFX

        ; *******************************************************************
        ; for testing FX: we jump directly to a linked FX setting up I/O ptrs
        ; *******************************************************************
        move    #>dspStateBlockXRAM,rD

        jsr     FX_START_ROUTINE
        rts

        ENDIF ; FX_TEST

        ;
        ; library code..
        ;

	include 'DMAUtil.asm' 
	include 'Util.asm' 

        IF @DEF('FX_TEST')

        include "FX_FILENAME"

        ENDIF

FX0_BASE
        nop
        rts

                  
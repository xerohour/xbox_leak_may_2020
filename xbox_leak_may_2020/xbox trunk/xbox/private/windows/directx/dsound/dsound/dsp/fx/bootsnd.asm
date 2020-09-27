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
SIZE_REVERB                 equ     512
SIZE_BOOTSND                equ     512

        org     x:kGPMemory_XRAM_Base
dmaOutFIFONode			ds	kDMANode_Size_FIFO       
ReverbFxData                    ds      SIZE_REVERB
ReverbFxTempData                ds      (kFrameLength*8)

; brian put your private data in this block
BootSndFxData                   ds      SIZE_BOOTSND

;***********************************************************
        org     p:$0      ; Place just before START

START
main
        IF !@DEF('SIM')
        jsr     GPInit          
        ENDIF

	move	#dmaOutFIFONode,r0
	move	#>(kDMANode_EOLBit+dmaOutFIFONode),x0
	move	x0,x:(r0)+
	move	#>kDMA_FIFOOut0_Word1,x0
	move	x0,x:(r0)+
	move	#>kDMA_FIFOOut0_Word2,x0
	move	x0,x:(r0)+
	move	#>kReadWriteMixBuf00,x0
	move	x0,x:(r0)+
	
	move	#dmaOutFIFONode,a
	jsr	DMAInit

        ; initialize boot snd FX

        jsr     FXInit

;*********************************************************
; Main loop
;*********************************************************
MainLoop

        GPSetIdleBit    

        IF !@DEF('SIM')
        jsr             GPWaitForStartFrame  
        ENDIF

        ;
        ; call bootsnd code
        ;

        jsr     FXCompute    

        ; call reverb code
        jsr     InvokeReverb

        IF !@DEF('SIM') 

MainLoop_StartDMA

        move	#dmaOutFIFONode,a
	jsr	DMAStartNode_Wait

        ENDIF


        bra     <MainLoop                    
; ---- end MainLoop ----


;
; bootsnd code init
;

FXInit
        rts

;
; bootsnd process audio frame routine
;

FXCompute

        rts



;
; reverb
;
InvokeReverb

        move    #>ReverbFxData,rD
        move    #>ReverbFxTempData,r6
        jsr     StartReverb


        rts





        ;
        ; library code..
        ;

	include 'DMAUtil.asm' 
	include 'Util.asm' 

        ;
        ; inplace FX code

        include "reverb_wrapper.asm"


                  
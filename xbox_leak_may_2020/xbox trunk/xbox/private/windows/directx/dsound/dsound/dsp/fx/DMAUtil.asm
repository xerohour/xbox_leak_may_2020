;*******************************************************************
;
;       DMAUtil.asm    DSP DMA utility routines  
;
;******************************************************************        
		
;************************************************************
; DMAInit	Configure DMA engine for first program use
;
;
;	Parameters	a	dmaStart node
;
;		Stops DMA Engine, configures DMA node list
;		and clears DMA and Interrupt registers
;************************************************************
DMAInit
	bsr 	DMAStopAndFreeze	
	ClearDMAConfigRegister

	andi	#>kDMANode_EOLMask,a	; Clear reserved EOL and upper bits
	movep    a,x:DMA_START_BLOCK_CONTROL_REGISTER
	movep    a,x:DMA_NEXT_BLOCK_CONTROL_REGISTER	

 	rts
; ---- end DMAInit ----


;************************************************************
; DMANewNode_Read_Eol	
;
;	This DMA Read node is configured for the System
;	Memory -> DSP transfer of one buffer of specified length
;	This node is also set up to be the last node in
;	the list, so it is self-linked with the EndOfList
;	bit set
;
;	Parameters:  	r0	node DSP XRAM address
;			r1	DSP    Memory address
;			r2	System Memory address
;			r3	buffer length
;************************************************************
DMANewNode_Read_Eol	
	move	r0,a
	andi	#>kDMANode_EOLMask,a	; Clear reserved upper bits
	ori		#>kDMANode_EOL,a			
        move    a,x:(r0+kDMAWord0_NextCmd) ; Link to itself			

        move    #>DMA_BASE_READ1,a		
	move	a,x:(r0+kDMAWord1_ControlBits)			
        move    r3,x:(r0+kDMAWord2_Count)			
	move    r1,x:(r0+kDMAWord3_DSPAddress)						
	move    r2,x:(r0+kDMAWord4_SMOffset)	

 	rts
; ---- end DMANewNode_Read_Eol ----

;************************************************************
; DMANewNode_Write_Eol	
;
;	This DMA write node is configured for the System
;	Memory <- DSP transfer of one buffer of specified length.
;	This node is also set up to be the last node in
;	the list, so it is self-linked with the EndOfList
;	bit set
;
;	Parameters:  	r0	node DSP XRAM address
;			r1	DSP    Memory address
;			r2	System Memory address
;			r3	Buffer length
;************************************************************
DMANewNode_Write_Eol	
	move	r0,a
	andi	#>kDMANode_EOLMask,a	; Clear reserved upper bits
	ori	#>kDMANode_EOL,a			
        move    a,x:(r0+kDMAWord0_NextCmd)	; Link to itself			

        move    #>DMA_BASE_WRITE1,a		
	move	a,x:(r0+kDMAWord1_ControlBits)			
        move    r3,x:(r0+kDMAWord2_Count)			
	move    r1,x:(r0+kDMAWord3_DSPAddress)		
	move    r2,x:(r0+kDMAWord4_SMOffset)	
 	rts
; ---- end DMANewNode_Write_Eol ----

        include 'fxdmautil.asm'


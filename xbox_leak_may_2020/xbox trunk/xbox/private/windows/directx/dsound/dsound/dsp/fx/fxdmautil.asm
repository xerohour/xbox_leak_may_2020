;************************************************************
; DMAStart	Write word to start DMA engine and
;			wait until DMA has entered Running state
;
;		Alters DMA Running state (Well, duh !!)
;************************************************************
DMAStart
; Writes to status Bits 5:3 have no effect
    	movep    #>kDMA_ActionRequest_Start,x:DMA_CONTROL_REGISTER	

; Wait for DMA engine to enter Running state: 0 = Idle, 1 = Running
DMAStart_Wait
	brclr 	#kDMA_RunningBit,x:DMA_CONTROL_REGISTER,DMAStart_Wait

 	rts
; ---- end DMAStart ----

;************************************************************
; DMAStop	Write word to stop DMA engine and
;			wait until DMA has entered Idle state
;
;		Alters DMA Running state (Well, duh !!)
;************************************************************
DMAStop 
; Writes to status Bits 5:3 have no effect
      	movep    #>kDMA_ActionRequest_Stop,x:DMA_CONTROL_REGISTER	

; Wait for DMA engine to enter Idle state: 0 = Idle, 1 = Running
DMAStop_Wait
	brset 	#kDMA_RunningBit,x:DMA_CONTROL_REGISTER,DMAStop_Wait

 	rts
; ---- end DMAStop ----

;************************************************************
; DMAFreeze	Write word to freeze DMA engine and
;			wait until DMA has entered Freeze state
;
;		Does not alter DMA Running state 
;************************************************************
DMAFreeze 
; Writes to status Bits 5:3 have no effect
      	movep    #>kDMA_ActionRequest_Freeze,x:DMA_CONTROL_REGISTER	

; Wait for DMA engine to enter Frozen state :  0 = Not Frozen, 1 = Frozen
DMAFreeze_Wait
	brclr 	#kDMA_FrozenBit,x:DMA_CONTROL_REGISTER,DMAFreeze_Wait

 	rts
; ---- end DMAFreeze ----

;************************************************************
; DMAStopAndFreeze	Same as calling DMAStop() followed by
;			DMA Freeze
;
;		Alters DMA Running state (Well, duh !!)
;************************************************************
DMAStopAndFreeze 
; Writes to status Bits 5:3 have no effect
      	movep    #>kDMA_ActionRequest_Stop,x:DMA_CONTROL_REGISTER
	
; Wait for DMA engine to enter Idle state: 0 = Idle, 1 = Running
DMAStopAndFreeze_WaitForStop
	brset 	#kDMA_RunningBit,x:DMA_CONTROL_REGISTER,DMAStopAndFreeze_WaitForStop

; Wait for DMA engine to enter Frozen state :  0 = Not Frozen, 1 = Frozen
      	movep    #>kDMA_ActionRequest_Freeze,x:DMA_CONTROL_REGISTER	
DMAStopAndFreeze_WaitForFreeze
	brclr 	#kDMA_FrozenBit,x:DMA_CONTROL_REGISTER,DMAStopAndFreeze_WaitForFreeze

 	rts
; ---- end DMAStopAndFreeze ----

;************************************************************
; DMAUnFreeze	Write word to unfreeze DMA engine and
;			wait until DMA has entered Freeze state
;
;		Does not alter DMA Running state 
;************************************************************
DMAUnFreeze 
; Writes to status Bits 5:3 have no effect
      	movep    #>kDMA_ActionRequest_UnFreeze,x:DMA_CONTROL_REGISTER	

; Wait for DMA engine to exit Frozen state :  0 = Not Frozen, 1 = Frozen
DMAUnFreeze_Wait
	brset 	#kDMA_FrozenBit,x:DMA_CONTROL_REGISTER,DMAUnFreeze_Wait

 	rts
; ---- end DMAUnFreeze ----

;************************************************************
; DMAUnFreezeAndStart	Same as calling DMAUnFreeze() followed by
;				DMAStart
;
;		Alters DMA Running state (Well, duh !!)
;************************************************************
DMAUnFreezeAndStart 
; Writes to status Bits 5:3 have no effect
      	movep    #>kDMA_ActionRequest_UnFreeze,x:DMA_CONTROL_REGISTER	

; Wait for DMA engine to exitFrozen state :  0 = Not Frozen, 1 = Frozen
DMAUnFreezeAndStart_WaitForUnfreeze
	brset 	#kDMA_FrozenBit,x:DMA_CONTROL_REGISTER,DMAUnFreezeAndStart_WaitForUnfreeze


; Wait for DMA engine to enter Running state: 0 = Idle, 1 = Running
    	movep    #>kDMA_ActionRequest_Start,x:DMA_CONTROL_REGISTER	
DMAUnFreezeAndStart_WaitForStart
	brclr 	#kDMA_RunningBit,x:DMA_CONTROL_REGISTER,DMAUnFreezeAndStart_WaitForStart

 	rts
; ---- end DMAUnFreezeAndStart ----

;************************************************************
; DMAWaitForEOL		Wait for DMA EOL interrupt and clear register bit
;************************************************************
DMAWaitForEOL 
; Poll for DMA EOL interrupt
DMAWaitForEOL_Poll
	brclr 	#kDMAEndOfListBit,x:INTERRUPT_REGISTER,DMAWaitForEOL_Poll

; Clear DMA EndofList, Bit 7
	movep    #$80,x:INTERRUPT_REGISTER	; "Write-1-To-Clear-One" 
 	rts
; ---- end DMAWaitForEOL ----

;************************************************************
; DMAStartNode_Wait	
;
;	Replace DMA next block ptr, initiate transfer and 
;		Wait for completion
;
;	This function assumes a single DMA node that is
;	setup as the EOL block.
;
;	Parameters:  	 a	next node DSP RAM address 
;************************************************************
DMAStartNode_Wait	
	bsr 	DMAStopAndFreeze

	andi	#>$FFDFFF,a	; Mask out EOL bit	
	movep    a,x:DMA_NEXT_BLOCK_CONTROL_REGISTER

	bsr 	DMAUnFreezeAndStart
	bsr 	DMAWaitForEOL

 	rts
; ---- end DMAStartNode_Wait ----

;************************************************************
; DMAStartNode	
;
;	Replace DMA next block ptr, initiate transfer and 
;		Wait for completion
;
;	This function assumes a single DMA node that is
;	setup as the EOL block.
;
;	Parameters:  	 a	next node DSP RAM address 
;************************************************************
DMAStartNode	
	bsr 	DMAStopAndFreeze

	andi	#>$FFDFFF,a	; Mask out EOL bit	
	movep    a,x:DMA_NEXT_BLOCK_CONTROL_REGISTER

	bsr 	DMAUnFreezeAndStart

 	rts
; ---- end DMAStartNode_Wait ----


        IF      !CIRCULAR_DMA

;************************************************************
; DMANewNode_WriteFrame_Eol	
;
;	This DMA Read node is configured for the System
;	Memory -> DSP transfer of one sample frame buffer.
;	This node is also set up to be the last node in
;	the list, so it is self-linked with the EndOfList
;	bit set
;
;	Parameters:  	r0	node DSP XRAM address
;			r1	DSP    Memory address
;			r2	System Memory address
;************************************************************
DMANewNode_WriteFrame_Eol	
	move	r0,a
	andi	#>kDMANode_EOLMask,a	; Clear reserved upper bits
	ori		#>kDMANode_EOL,a			
        move    a,x:(r0+kDMAWord0_NextCmd)		; Link to itself			

        move    #>DMA_BASE_WRITE1_ISO,a		
	move	a,x:(r0+kDMAWord1_ControlBits)			
        move    #>kFrameLength,a		
	move	a,x:(r0+kDMAWord2_Count)			
	move    r1,x:(r0+kDMAWord3_DSPAddress)		
	move    r2,x:(r0+kDMAWord4_SMOffset)	

 	rts
; ---- end DMANewNode_WriteFrame_Eol ----

;************************************************************
; DMANewNode_ReadFrame_Eol	
;
;	This DMA Read node is configured for the System
;	Memory -> DSP transfer of one sample frame buffer.
;	This node is also set up to be the last node in
;	the list, so it is self-linked with the EndOfList
;	bit set
;
;	Parameters:  	r0	node DSP XRAM address
;			r1	DSP    Memory address
;			r2	System Memory address
;************************************************************
DMANewNode_ReadFrame_Eol	
	move	r0,a
	andi	#>kDMANode_EOLMask,a	; Clear reserved upper bits
	ori		#>kDMANode_EOL,a			
        move    a,x:(r0+kDMAWord0_NextCmd)	; Link to itself			

        move    #>DMA_BASE_READ1_ISO,a		
	move	a,x:(r0+kDMAWord1_ControlBits)			
        move    #>kFrameLength,a		
	move	a,x:(r0+kDMAWord2_Count)			
	move    r1,x:(r0+kDMAWord3_DSPAddress)	 	
	move    r2,x:(r0+kDMAWord4_SMOffset)	

 	rts

        ENDIF
; ---- end DMANewNode_ReadFrame_Eol ----



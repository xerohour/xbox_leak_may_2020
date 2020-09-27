; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			loader.asm
;												
;   Contents:		Loader for Dolby Game Encoder
;
;	Author:			jmw
;											
;	Revision:		1.00b
;											
;	History:
;
;		Rev 1.00	10/11/2000 7:23:25 AM		jmw
;	          Created.
;               Rev 1.00b       12/11/2000 4:11:12 PM           Ethan Brodsky
;        	  brought DMA code over from Dolby/v1.20 package
;
;
; ******************************************************************

	page	132,66,3,3
	opt		cre,loc,cc,cex,mex,mu,so

	title	'Dolby Loader'

	section	Loader

;**** include files ************************************************

	nolist
	include 'ldr_sip.inc'
	include 'eputil.h'    ; EP constants
	include 'dmautil.h'   ; DMA constants
	list

;**** equates ******************************************************



	DEFINE	PREAMBLE	'1'		; Enable AC3 SPDIF Preamble
	DEFINE	ZEROFILL	'1'		; Enable AC3 SPDIF Zero Fill

ROM_BASE        equ             $800
ROM_SIZE        equ             $800
XRAM_BUFFER     equ             $400



;**** program code *************************************************

	xref	LdrTable


	org		p(100):				; Loader Runtime Counter is 100

;******************************************************************************
;*										  
;*   Subroutine Name:	Loader
;*								  
;*   Description:		Loader
;*										      
;*   Input:				a	- Buffer ID word
;*						b	- Loader action
;*						r0	- DSP memory address
;*						n0	- number of words to transfer
;*						n1	- System memory offset
;*
;*   Output:			a	- error return code: nonzero if error
;*
;*   Modifies:			
;*
;*	 Locals:			
;*
;*	 Stack Space Used:	
;*										      
;******************************************************************************

loader:

	cmp		#LID_LOADER_INIT,a		; loader init request?
	beq		<loader_init		    ; if so, go init the loader

	cmp		#LID_PING_PONG,a		; should we toggle the PCM input ping pong
	beq		<toggle_ping_pong

        bra             <done

;***********************************************************************
;	Data storage for loader variables

temp                    dc          0
dmaNode	                dc	    0
dmaNode1                dc	    0
dmaNode2	            dc	    0
dmaNode3	            dc	    0
dmaNode4	            dc	    0
dmaNode5	            dc	    0
dmaNode6	            dc	    0
prev_time                   dc      0
elapsed_time                dc      0


;***********************************************************************
;	Toggle ping pong buffer on PCM input

toggle_ping_pong:

        ; first read the EP boot rom into xram offset

        move    #>XRAM_BUFFER,r0
        move    #>ROM_BASE,r4

        dor     #ROM_SIZE,CopyRomLoop

        move    y:(r4)+,x0
        move    x0,x:(r0)+

CopyRomLoop

        ; now that the rom contents are in ram, DMA them into FIFO0
        move    #>XRAM_BUFFER,r0     
        move    #>ROM_SIZE,n0
        move    #>0,r1
        jsr     CopyDSPXToSys


	bra		<done					; exit


;***********************************************************************
;	Exit points

done:

	clr		a						; no error status
	rts
												  


;***********************************************************************
;	Loader initialization code
;		Copies loader table from system memory to DSP memory
;		Clears dynamic table entries
;		Initializes local heap variables

loader_init:
	clr	a #LdrTable+LDR_HEAP_ADDR,r0		; set up pointer
	dor	#LDR_NUM_DATA*2,clrlp
	move	a,p:(r0)+						; clear table value
clrlp:


	bsr	DMAInit					; one-time DMA initialization
	
	bra	<done					; exit

;******************************************************************************
;*										  
;*   Subroutine Name:	CopySysToDSPX
;*								  
;*   Description:	Copy from System Memory to DSP x: space
;*										      
;*   Input:		r0 	- DSP address (x:)
;*			n0	- number of words to copy
;*			r1	- System Memory address
;*										      
;******************************************************************************

CopySysToDSPX:
	IF		@DEF('RTSYS')
	jsr		set_timing
	ENDIF

; Note: Dolby-only testing code
;	dor		n0,systoxlp
;	move	x:(r1)+,x0
;	move	x0,x:(r0)+
;systoxlp:

	move	#dmaNode,r4			; r4 <- address of dma node
	
	move	r0,a1
	add 	#kEPMemory_Base_XRAM,a 		; adjust DSP address for X base
	move	a1,r0
	
	jsr	DMANewNode_Read			; create DMA node
	
	jsr	DMAStartNodeP_Wait		; initiate DMA 
						; (and wait for completion)

	IF		@DEF('RTSYS')
	jsr		clr_timing
	ENDIF
	rts

;******************************************************************************
;*										  
;*   Subroutine Name:	CopyDSPXToSys
;*								  
;*   Description:	Copy from DSP x: space to System Memory
;*										      
;*   Input:		r0 	- DSP address (x:)
;*			n0	- number of words to copy
;*			r1	- System Memory address
;*										      
;******************************************************************************

CopyDSPXToSys:
	IF		@DEF('RTSYS')
	jsr		set_timing
	ENDIF
	
; Note: Dolby-only testing code
;	dor		n0,xtosyslp
;	move	x:(r0)+,x0
;	move	x0,x:(r1)+
;xtosyslp:

	move	#dmaNode,r4			; r4 <- address of dma node
	
	move	r0,a1
	add 	#kEPMemory_Base_XRAM,a 		; adjust DSP address for X base
	move	a1,r0
	
	jsr	DMANewNode_Write		; create DMA node
	
	jsr	DMAStartNodeP_Wait		; initiate DMA 
						; (and wait for completion)
        
	IF		@DEF('RTSYS')
	jsr		clr_timing
	ENDIF
	rts

;******************************************************************************
;*										  
;*   Subroutine Name:	CopyDSPXToFIFO
;*								  
;*   Description:	Copy from DSP x: space to FIFO
;*									      
;*   Input:		r0 	- DSP address (x:)
;*			n0	- number of words to copy
;*			r1	- FIFO index
;*										      
;******************************************************************************

CopyDSPXToFIFO:

	move	#dmaNode,r4			; r4 <- address of dma node
	
	move	r0,a1
	add 	#kEPMemory_Base_XRAM,a 		; adjust DSP address for X base
	move	a1,r0
	
	jsr	DMANewNode_WriteFIFO		; create DMA node

CopyDSPXToFIFOStart:
	jsr	DMAStartNodeP_Wait		; initiate DMA 
						; (and wait for completion)
        
	rts

;******************************************************************************
;*										  
;*   Subroutine Name:	CopySysToDSPY
;*								  
;*   Description:	Copy from System Memory to DSP y: space
;*		       							      
;*   Input:	 	r0 	- DSP address (y:)
;*			n0	- number of words to copy
;*			r1	- System Memory address
;*										      
;******************************************************************************

CopySysToDSPY:

; Note: Dolby-only testing code
;	dor		n0,systoylp
;	move	x:(r1)+,x0
;	move	x0,y:(r0)+
;systoylp:

	move	#dmaNode,r4			; r4 <- address of dma node
	
	move	r0,a1
	add 	#kEPMemory_Base_YRAM,a 		; adjust DSP address for Y base
	move	a1,r0
	
	jsr	DMANewNode_Read			; create DMA node
	
	jsr	DMAStartNodeP_Wait		; initiate DMA 
						; (and wait for completion)

	rts

;******************************************************************************
;*										  
;*   Subroutine Name:	CopyDSPYToSys
;*								  
;*   Description:	Copy from DSP y: space to System Memory
;*										      
;*   Input:		r0 	- DSP address (y:)
;*	   		n0	- number of words to copy
;*			r1	- System Memory address
;*										      
;******************************************************************************

CopyDSPYToSys:

; Note: Dolby-only testing code
;	dor		n0,ytosyslp
;	move	y:(r0)+,x0
;	move	x0,x:(r1)+
;ytosyslp:


	move	#dmaNode,r4			; r4 <- address of dma node
	
	move	r0,a1
	add 	#kEPMemory_Base_YRAM,a 		; adjust DSP address for Y base
	move	a1,r0
	
	jsr	DMANewNode_Write		; create DMA node
	
	jsr	DMAStartNodeP_Wait		; initiate DMA 
						; (and wait for completion)

	rts

;******************************************************************************
;*										  
;*   Subroutine Name:	CopySysToDSPP
;*								  
;*   Description:	Copy from System Memory to DSP p: space
;*										      
;*   Input:	 	r0 	- DSP address (p:)
;*			n0	- number of words to copy
;*			r1	- System Memory address
;*										      
;******************************************************************************

CopySysToDSPP:

; Note: Dolby-only testing code
;	dor		n0,systoplp
;	move	x:(r1)+,x0
;	move	x0,p:(r0)+
;	nop
;systoplp:

	move	#dmaNode,r4			; r4 <- address of dma node
	
	move	r0,a1
	add 	#kEPMemory_Base_PRAM,a 		; adjust DSP address for Y base
	move	a1,r0
	
	jsr	DMANewNode_Read			; create DMA node
	
	jsr	DMAStartNodeP_Wait		; initiate DMA 
						; (and wait for completion)

	rts

;******************************************************************************
;*										  
;*   Subroutine Name:	CopyDSPPToSys
;*								  
;*   Description:	Copy from DSP p: space to System Memory
;*										      
;*   Input:	       	r0 	- DSP address (p:)
;*		  	n0	- number of words to copy
;*		       	r1	- System Memory address
;*										      
;******************************************************************************

CopyDSPPToSys:
	
; Note: Dolby-only testing code
;	dor		n0,ptosyslp
;	move	p:(r0)+,x0
;	move	x0,x:(r1)+
;ptosyslp:

	move	#dmaNode,r4			; r4 <- address of dma node
	
	move	r0,a1
	add 	#kEPMemory_Base_PRAM,a 		; adjust DSP address for Y base
	move	a1,r0
	
	jsr	DMANewNode_Write		; create DMA node
	
	jsr	DMAStartNodeP_Wait		; initiate DMA 
						; (and wait for completion)
	rts
	
	
;************************************************************
; DMANewNode_Read
;
;	This DMA Read node is configured for the System
;	Memory -> DSP transfer
;	This node is also set up to be the last node in
;	the list, so it is self-linked with the EndOfList
;	bit set
;
;	Parameters:  	r4	node DSP PRAM address
;			r0	DSP Memory address
;			r1	System Memory address
;           n0      Length
;************************************************************
DMANewNode_Read

	jsr 	DMAWaitForIdle			    ; Wait fro DMA to be idle

; Word0
	move	r4,r5			; r5 <- copy of node base 
	
	move	r4,a1
	and		#>kDMANode_EOLMask,a	; Clear reserved upper bits
	or		#>kDMANode_EOL,a 	; set EOL bit
	
    movem   a1,p:(r5)+		; Link to itself			
; Word1
    move    #>DMA_BASE_READ1_ISO,a1
    ;move    #>DMA_BASE_READ1,a1
	movem	a1,p:(r5)+
; Word2
	movem	n0,p:(r5)+		; Length       	
; Word3
	movem	r0,p:(r5)+		; DSP RAM address (System memory space)	
; Word4						
	move	r1,a
	lsl		#2,a					
	movem	a1,p:(r5)+		; System memory address


 	rts
; ---- end DMANewNode_Read


;************************************************************
; DMANewNode_Write     	
;
;	This DMA Write node is configured for the DSP -> SM
;	transfer 
;	This node is also set up to be the last node in
;	the list, so it is self-linked with the EndOfList
;	bit set
;
;	Parameters:  	r4	node DSP PRAM address
;			r0	DSP Memory address 
;			r1	System Memory address
;			n0	Length
;************************************************************
DMANewNode_Write	

	jsr 	DMAWaitForIdle			    ; Wait fro DMA to be idle

; Word0
	move	r4,r5			; r5 <- copy of node base 
	
	move	r4,a1
	and		#>kDMANode_EOLMask,a	; Clear reserved upper bits
	or		#>kDMANode_EOL,a	; set EOL bit
    	
; Word0
	move    a1,p:(r5)+		; Link to itself			

; Word1
    	move    #>DMA_BASE_WRITE1_ISO,a1
        ;move    #>DMA_BASE_WRITE1,a1
	movem	a1,p:(r5)+			
; Word2
	movem	n0,p:(r5)+		; Length
; Word3
	movem	r0,p:(r5)+		; DSP RAM address (System memory space)	
; Word4	
	move	r1,a
	lsl		#2,a					
	movem	a1,p:(r5)+		; System memory address

 	rts
; ---- end DMANewNode_Write ----

;************************************************************
; DMANewNode_WriteFIFO
;
;	This DMA Write node is configured for the DSP -> FIFO
;	transfer 
;	This node is also set up to be the last node in
;	the list, so it is self-linked with the EndOfList
;	bit set
;
;	Parameters:  	r4	node DSP PRAM address
;			r0	DSP Memory address 
;			r1	FIFO number
;			n0	Length
;			n4  NULL output flag
;************************************************************
DMANewNode_WriteFIFO

	jsr 	DMAWaitForIdle			    ; Wait fro DMA to be idle

; Word0
	move	r4,r5			; r5 <- copy of node base 
	
	move	r4,a1
	and		#>kDMANode_EOLMask,a	; Clear reserved upper bits
	or		#>kDMANode_EOL,a	; Set EOL bit
    	
	move    a1,p:(r5)+		; Link to itself			

; Word1
        ;move    #>DMA_BASE_WRITEFIFO1_ISO,a1 	; start with "write to FIFO/16-bit" control word
        move #>DMA_BASE_WRITEFIFO_DATA_ISO,a1
        move	r1,b			; FIFO index from r1
        lsl	#5,b			; shift FIFO index smBufId bits
        move	b,x1
	or	x1,a			; put FIFO index into control word
	movem	a1,p:(r5)+		; write control word
; Word2
	movem	n0,p:(r5)+			
; Word3
	move	n4,a			; Null Flag
        lsl	#14,a
	move	r0,x0
	add	x0,a			; Add DSP Address
	movem	a1,p:(r5)+		; DSP RAM address (System memory space)	
; Word4 is no longer used

 	rts
; ---- end DMANewNode_WriteFIFO ----


;************************************************************
; DMANewNode_WriteFIFOInterleaved
;
;	This DMA Write node is configured for the DSP -> FIFO
;	transfer 
;	This node is also set up to be the last node in
;	the list, so it is self-linked with the EndOfList
;	bit set
;
;	Parameters:  	r4	node DSP PRAM address
;			r0	DSP Memory address 
;			r1	FIFO number
;			n0	Length
;			n2  Channels per block
;			n4  NULL output flag
;************************************************************
DMANewNode_WriteFIFOInterleaved

	jsr 	DMAWaitForIdle			    ; Wait fro DMA to be idle

; Word0
	move	r4,r5			; r5 <- copy of node base 
	
	move	r4,a1
	and 	#>kDMANode_EOLMask,a	; Clear reserved upper bits
	or		#>kDMANode_EOL,a	; Set EOL bit
	move    a1,p:(r5)+		; Link to itself			

; Word1
    move    #>DMA_BASE_WRITEFIFO2_ISO,a1 	; start with "write to FIFO/16-bit interleaved" control word
    ;move    #>DMA_BASE_WRITEFIFO2,a1
	move	r1,b			; FIFO index from r1
    lsl	    #5,b			; shift FIFO index smBufId bits
	move	b,x1
	or	    x1,a			; put FIFO index into control word

	move	n0,b			; Set Incr value
    lsl	    #14,b			; shift incr value bits (Increment by size of a single channel buffer)
	move	b,x1
	or	    x1,a			; put FIFO index into control word
	movem	a1,p:(r5)+		; write control word
; Word2
	move	n0,b			; Set Length value
    lsl	    #4,b			; shift Length bits (Increment by size of a single channel buffer)
	move	n2,x1
	add		x1,b			; Add samples per block
	movem	b1,p:(r5)+		; write control word
; Word3
	move	n4,a			; Null Flag
    lsl	    #14,a
	move	r0,x0
	add		x0,a			; Add DSP Address
	movem	a1,p:(r5)+		; DSP RAM address (System memory space)	
; Word4 is no longer used

 	rts
; ---- end DMANewNode_WriteFIFO ----





;************************************************************
; DMAWaitForStop	
;
;		Alters DMA Running state 
;************************************************************
DMAWaitForIdle 


; Wait for DMA engine to enter Idle state: 0 = Idle, 1 = Running
	jset 	#kDMA_RunningBit,x:DMA_CONTROL_REGISTER,DMAWaitForIdle

; Clear DMA EndofList, Bit 7
	movep    #$80,x:INTERRUPT_REGISTER	; "Write-1-To-Clear-One" 

 	rts
; ---- end DMAStopAndFreeze ----

;************************************************************
; DMAStart
;
;		Alters DMA Running state 
;************************************************************
DMAStart 


; Write "run" command
    movep    #>kDMA_ActionRequest_Start,x:DMA_CONTROL_REGISTER	

; Wait for DMA engine to enter Running state: 0 = Idle, 1 = Running
DMAUnFreezeAndStart_WaitForStart
;	jset	#kDMAErrorBit,x:INTERRUPT_REGISTER,DMAError
	jclr 	#kDMA_RunningBit,x:DMA_CONTROL_REGISTER,DMAUnFreezeAndStart_WaitForStart

 	rts
; ---- end DMAUnFreezeAndStart ----

;************************************************************
; DMAWaitForEOL		Wait for DMA EOL interrupt and clear register bit
;************************************************************
DMAWaitForEOL 

; Poll for DMA EOL interrupt
DMAWaitForEOL_Poll
	jset	#kDMAErrorBit,x:INTERRUPT_REGISTER,DMAError
	jclr 	#kDMAEndOfListBit,x:INTERRUPT_REGISTER,DMAWaitForEOL_Poll

; Clear DMA EndofList, Bit 7
	movep    #$80,x:INTERRUPT_REGISTER	; "Write-1-To-Clear-One" 

    ;debug_wait


 	rts
; ---- end DMAWaitForEOL ----

;************************************************************
; DMAStartNodeP_Wait	
;
;	Replace start block, initiate transfer and Wait for
;				completion
;
;	This function assumes a single DMA node that is
;	setup as the EOL block.
;
;	Parameters:  	 r4	next node DSP PRAM address
;************************************************************
DMAStartNodeP_Wait 
	
	move	r4,a
	add		#kEPMemory_Base_PRAM,a			; block is in PRAM 
	movep	a,x:DMA_NEXT_BLOCK_CONTROL_REGISTER
	
	jsr 	DMAStart			        ; Start transfer
	
    clr     b
    move    n5,b
    cmp     #0,b
    beq     DMAStartNodeP_NoWait

	jsr 	DMAWaitForEOL				; Wait for completion

DMAStartNodeP_NoWait
 	rts
; ---- end DMAStartNodeX_Wait ----


;******************************************************************************
;*
;*   Subroutine Name:	DMAInit             
;*
;*   Description:		Inits the EP for DMA (based on Gints' GP code)
;*
;******************************************************************************
;
;

DMAInit:
	;GPClearInterruptRegister
	movep    #$FFF,x:INTERRUPT_REGISTER	 ; "Write-1-To-Clear-One"
	
	;GPClearProcessingControlRegister
	movep    #$1,x:PROCESSING_CONTROL_REGISTER	 ; "Set Idle bit"	

	;GPClearSampleControlRegister
	movep    #$0,x:SAMPLE_CONTROL_REGISTER	 
	
	;abort any DMA transfers
;    movep    #>kDMA_ActionRequest_Abort,x:DMA_CONTROL_REGISTER

    move    #$4000,x0	
	move    x0,x:DMA_START_BLOCK_CONTROL_REGISTER
	move    x0,x:DMA_NEXT_BLOCK_CONTROL_REGISTER

	;GPClearDMAConfigRegister 
	movep    #>0,x:DMA_CONFIGURATION_REGISTER

	rts


DMAError
	bra	DMAError




;** Loader Status

;status_active	dc	0
;status_ID		dc	-1
;status_action	dc	-1
;status_dspaddr	dc	-1
;status_length	dc	-1
;status_sysaddr	dc	-1
;status_wait		dc	0

	endsec
	end

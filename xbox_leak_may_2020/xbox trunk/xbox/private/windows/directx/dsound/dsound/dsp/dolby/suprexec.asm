; ******************************************************************
;
; 	Unpublished work.  Copyright 2000 Dolby Laboratories Inc.
; 	All Rights Reserved.
;												
;	File:			Suprexec.asm
;												
;   Contents:		Superexec for Dolby Game Encoder
;
;	Author:			jmw
;											
;	Revision:		1.00
;											
;	History:
;
;		Rev 1.00	8/16/2000 3:51:37 PM		jmw
;	Created.
;
;
; ******************************************************************

	page	132,66,3,3
	opt		cre,loc,cc,cex,mex,mu,so

	title	'Dolby Super Exec'

	section	SuperExec

;**** include files ************************************************

	nolist
	include 'ldr_sip.inc'
	include	'se_sip.inc'
	include 'eputil.h'        
	list

;**** Development Switches *****************************************

	IF			@DEF('SIM_XBOX_SYS_MEM') 	
 		DEFINE		TRASH_DSP_MEMORY	   	'1'		; Overwrite DSP memory each block
	ENDIF

;*** Switches (leave defined for X-Box) ****************************

	DEFINE		PERF_COUNTERS                       '1'
	DEFINE		INFINITE_LOOP_IF_TIME_OVERRUN		'1'
	;DEFINE		ERRHALT								'1'		; Halt on loader error	

;**** Development External References ******************************

	IF		@DEF('RTSYS')						; Dolby development HW only
		xref	rt_init,rt_input,rt_output
	ENDIF

	IF		@DEF('SIM_XBOX_SYS_MEM')			; Motorola simulator only
		xref	CopyInFilesToSysMem
		xref	CopySysMemToOutputFiles
		xref	Copy6ChInFileToSysMem
		xref	CopySysMemToAC3OutputFile
	ENDIF

;**** equates ******************************************************


;MAX_EP_CYCLES		  		equ		1060000 ;200mhz
;MAX_EP_CYCLES		  		equ		850000  ;160mhz
MAX_EP_CYCLES		  		equ     708000  ;133mhz

STACK_EXTSZ					equ		32		; stack extension size
CONFIG_SIZE					equ		32		; config data size
SE_NUM_BLOCKS				equ		6		; number of blocks per frame (6)
PMEMSIZE					equ		$001000	; p: is 4k words
YMEMSIZE					equ		$000100	; y: is 256 words
TRASHWORD					equ		$ffffff	; overwrite memory with this word

MONO_DNMIX_CODE				equ		$000001	; 
STEREO_DNMIX_CODE			equ		$000002	;

;	IF	@DEF('PERF_COUNTERS')
;MAX_TIMES_IGNORE_TIMER_AFTER_REBOOT		equ		0
;	ENDIF	; 	IF	@DEF('PERF_COUNTERS')

;*******************************************************************
;	Super Exec sets up stack extension at top of x: RAM.
	
	mode	rel
	org		x(201):					; place stack at top of RAM.

stack_ext	ds	STACK_EXTSZ			; stack extension

;*******************************************************************
; 	Config List from System. Place immediately below stack extension.

	mode	rel
	org		x(204):

config_list		ds	CONFIG_SIZE

;;;;; Note: Interrupt vectors are in ROM on nVidia system

;**** code *********************************************************
;	Start Program Code Here.

	mode	rel
	org		p(202):

SuperExec:

	bra		<SEStart				; skip over local variables

;	Local Variables

supexec_blknum	    dc		0 			; to sync all functions to same block.
surr_reset	    	dc		1 			; surround reset flag - init to one (!)
game_reset	    	dc		1 			; game encoder reset flag - init to one (!)

	IF @DEF('PERF_COUNTERS')
elapsed_time        dc      0
prev_time           dc      0
suprexec_temp	    dc	    $cccccc		; $cccccc is no time overflow, $bbbbbb if exceed 5.33 ms.
count               dc      0			; allows algorithm to exceed 5.33 ms during reboot only.
	ENDIF	; IF @DEF('PERF_COUNTERS')

;*******************************************************************
;	Start SuperExec Program Code.  

SEStart:

	IF @DEF('PERF_COUNTERS')
        ; init timer variables

        move    #0,x0
        move    x0,p:elapsed_time
        move    x0,p:prev_time

        ; setup timer registers

        movep   #$FFFFFF,x:TIMER1_TERMINAL_COUNT
        movep   #1,x:TIMER1_CONFIG

        ; start the timer from 0
        movep   #1,x:TIMER1_CONTROL
	ENDIF	; IF @DEF('PERF_COUNTERS')


;	Enable Stack extension.

	movec	#0,sp					; reset the stack pointer
	movec	#stack_ext,ep			; init extension pointer register
	movec	#STACK_EXTSZ,sz			; init stack size register
	bset	#20,omr					; enable stack extension

;	Enable any counters, etc. at reset.
	
	clr		a					   	; a0 = 0
	move	a0,p:supexec_blknum		; start with block zero at reset
	inc		a					   	; a0 = 1
	move	a0,p:surr_reset			; explicitly set reset flag
	move	a0,p:game_reset			; explicitly set reset flag

;	Initialize the loader

	move	#>LID_LOADER_INIT,a		; Packet ID
	jsr		LOADERBASE				; Call the Loader
	IF	@DEF('ERRHALT')
		tst	a						; check error status
		bne	<*
	ELSE
		tst     a
		bne     handle_error 
	ENDIF

;	Dolby development hardware only (RTSYS)

	IF	@DEF('RTSYS')
		bsr	rt_init					; init real-time hardware
	ENDIF

;	Clear the start frame bit
	EPClearStartFrameBit

	
;*******************************************************************
;	Enter Endless Loop to process each of the four possible Functions
;	in series.	Note that we might not process the function; it depends upon
;	the corresponding flag in the config_list.

;	1. Dolby Surround OR Downmix Function if Surround Not Enabled
;	2. Dolby Game Encoder Function 1
;	3. Dolby Game Encoder Function 2
;	4. Dolby Game Encoder Function 3

SELoop:

	IF @DEF('PERF_COUNTERS')
        move    p:count,a
        add     #1,a
        move    a,p:count
	ENDIF	; 	IF @DEF('PERF_COUNTERS')

; Write the idle bit
	EPSetIdleBit

;	Wait for Start Frame bit.
	clr     a
waitframe:
	add     #1,a
	jclr 	#kStartFrameBit,x:INTERRUPT_REGISTER,waitframe

	IF @DEF('PERF_COUNTERS')
        ;***********************************************************************
        move    x:TIMER1_COUNT,x0
        movem   x0,p:prev_time
        ;***********************************************************************
    ENDIF	; IF @DEF('PERF_COUNTERS')


ClearStartFrameBit:
; Clear GP Start Block (bit 1)
	EPClearStartFrameBit	

;	Simulate fact that memory is in unknown state

	IF	@DEF('TRASH_DSP_MEMORY')
		bsr	<trashmem
	ENDIF

;	following section is for Dolby development hardware only
;	gui can override config_list parameters 
;	call to rt_input returns a = audio/non-audio flag

	IF	@DEF('RTSYS')
		bsr	rt_input			; get input from real-time hardware
	ENDIF	; @DEF('RTSYS')

;	if block zero, get configuration data from system memory

	move	p:supexec_blknum,a			; get block number
	tst		a #config_list,r0			; is it zero? preload config_list address
	bne		<config_list_loaded			; if not, don't load list

	move	#>LID_CONFIG_LIST,a			; Packet ID
	move	#CONFIG_SIZE,n0				; tell Loader max number of words allowable
	jsr		LOADERBASE					; Call the Loader
	IF	@DEF('ERRHALT')
		tst	a							; check error status
		bne	<*							; stop if loader error
	ELSE
		tst     a
		bne     handle_error
	ENDIF	
	
config_list_loaded:
	
;-------------------------------------------------------------------
;	1A. Check if we're supposed to do Dolby Surround. If not, skip it.
;-------------------------------------------------------------------

SEDS:

	move	x:config_list+SE_IN_DO_SURROUND,a	; get DO_SURROUND flag
	tst		a #DOLBYEXECBASE,r0					; is zero? preload DSP memory address
	beq		<SEDownMix							; if so, bypass Surround

;	Load Program Code for Dolby Surround

SEDSLdP:

	move	#>LID_PROG_DSURRE,a						; Packet ID
	jsr	LOADERBASE								; Call the Loader
	IF	@DEF('ERRHALT')
		tst	a										; check error status
		bne	<*										; stop if loader error
	ELSE
		tst     a
		bne     handle_error
	ENDIF	

	IF	@DEF('SIM_XBOX_SYS_MEM')

;	Copy inputs to System memory (Simulator Only - NVIDIA: Remove this call!)

SECopyIn:

	jsr	<CopyInFilesToSysMem

	ENDIF	; @DEF('SIM_XBOX_SYS_MEM')


	;	Run Program Code for Dolby Surround

SEDSjsr:
	
	move	p:surr_reset,b							; get surround reset word
	move	#config_list,r0							; pass pointer to configuration data (in x:)
	jsr		DOLBYEXECBASE+LDR_HDR_SIZE				; jsr to Entry Point of Dolby Surround Exec (skip header)
	   
;	clear surround reset word

	move	#0,x0
	move	x0,p:surr_reset


	IF	@DEF('SIM_XBOX_SYS_MEM')

;	copy System Memory to output files (Simulator Only - NVIDIA: Remove this call!)

SECopyOut:

	jsr	<CopySysMemToOutputFiles

	ENDIF	; @DEF('SIM_XBOX_SYS_MEM')

; All Done with Dolby Surround.
	bra		<SEF1

;-------------------------------------------------------------------
;	1B. Check if we're supposed to do DownMix, since we did not do Surround..
;-------------------------------------------------------------------

SEDownMix:

;	Get Info from System Configuration table.

	move	x:config_list+SE_IN_DO_DOWNMIX,b	; get SE_IN_DO_DOWNMIX flag
	tst		b #DOLBYEXECBASE,r0					; is zero? preload DSP memory address
	beq		<SEF1 								; if so, bypass Downmix

SEDMMono:
	clr		a #>LID_PROG_DMMONO,x0				; clear a, preload mono dnmix LID
	cmp		#>MONO_DNMIX_CODE,b					; is SE_IN_DO_DOWNMIX == MONO?
	tfr		x0,a ifeq							; if so, load mono LID into a

SEDMStereo:
	move	#>LID_PROG_DMSTEREO,x0				; preload stereo dnmix LID
	cmp		#>STEREO_DNMIX_CODE,b				; is SE_IN_DO_DOWNMIX == STEREO?
	tfr		x0,a ifeq							; if so, load stereo LID into a

SEDMInvalid:
	tst		a									; if neither was loaded, invalid SE_IN_DO_DOWNMIX
	beq		<SEF1								; so bypass to be safe.

;	Load code.

	jsr	LOADERBASE								; Call the Loader
	IF	@DEF('ERRHALT')
		tst	a								 	; check error status
		bne	<*								 	; stop if loader error
	ELSE
		tst     a
		bne     handle_error
	ENDIF	

	;	Run Program Code for Dolby Downmix.

SEDMJSR:
	
	move	#config_list,r0							; pass pointer to configuration data (in x:)
	jsr		DOLBYEXECBASE+LDR_HDR_SIZE				; jsr to Entry Point of Dolby Surround Exec (skip header)

;	All done, go to next function.

	bra		<SEF1

;-------------------------------------------------------------------
;	2. Check if we're supposed to do Game Encoder. If not, skip it.
;-------------------------------------------------------------------

SEF1:

	move	x:config_list+SE_IN_DO_GAME,a	; get DO_GAME flag
	tst		a #DOLBYEXECBASE,r0				; is it zero? preload DSP memory address
	beq		<SEF3Done						; if so, bypass Game Encoder
	
;	Load Program Code for Function 1

SEF1LdP:

	move	#>LID_PROG_DGAME1,a						; Packet ID
	jsr		LOADERBASE								; Call the Loader
	IF	@DEF('ERRHALT')
		tst	a										; check error status
		bne	<*										; stop if loader error	
	ELSE
		tst     a
		bne     handle_error
	ENDIF


	IF	@DEF('SIM_XBOX_SYS_MEM')

;	Copy inputs to System memory (Simulator Only - NVIDIA: Remove this call!)

	jsr	Copy6ChInFileToSysMem

	ENDIF	; @DEF('SIM_XBOX_SYS_MEM')


;	Run Program Code for Function 1

SEF1jsr:
	
	move	#config_list,r0							; set up config list pointer
	move	p:game_reset,b							; get Game Encoder reset word
	move	p:supexec_blknum,x0						; set up x0 with current block number
	jsr		DOLBYEXECBASE+LDR_HDR_SIZE				; jsr to Entry Point of GE Function 1 (skip past header)

; 	All Done with Function 1.

SEF1Done:


;-------------------------------------------------------------------
;	Game Encoder Fcn 2.
;-------------------------------------------------------------------

SEF2:

;	Load Program Code for Function 2

SEF2LdP:

	move	#DOLBYEXECBASE,r0						; r0 is DSP memory address
	move	#>LID_PROG_DGAME2,a						; Packet ID
	jsr		LOADERBASE								; Call the Loader
	IF	@DEF('ERRHALT')
		tst	a										; check error status
		bne	<*										; stop if loader error
	ELSE
		tst             a
		bne             handle_error
	ENDIF

;	Run Program Code for Function 2

SEF2jsr:
	
	move	p:game_reset,b							; get Game Encoder reset word
	jsr		DOLBYEXECBASE+LDR_HDR_SIZE				; jsr to Entry Point of GE Function 2 (skip past header)

; 	All Done with Function 2.

SEF2Done:

;-------------------------------------------------------------------
;	Game Encoder Fcn 3.
;-------------------------------------------------------------------

SEF3:
	
;	Load Program Code for Function 3 Executive

SEF3LdP:

	move	#DOLBYEXECBASE,r0						; r0 is DSP memory address
	move	#>LID_PROG_DGAME3,a						; Packet ID
	jsr		LOADERBASE								; Call the Loader
	IF	@DEF('ERRHALT')
		tst	a										; check error status
		bne	<*										; stop if loader error
	ELSE
		tst     a
		bne     handle_error
	ENDIF	


;	Run Program Code for Function 3

SEF3jsr:
	
	move	p:game_reset,b							; get Game Encoder reset word
	move	p:supexec_blknum,x0						; set up x0 with current block number (need for output)
	jsr		DOLBYEXECBASE+LDR_HDR_SIZE				; jsr to Entry Point of GE Function 3 (skip past header)

;	clear Game Encoder reset word

	move	#0,x0
	move	x0,p:game_reset

	IF	@DEF('SIM_XBOX_SYS_MEM')

;	Write Outputs (for Motorola Simulator only) at the end of block 5
;	NVIDIA - remove this call!

SEWriteAC3:

	move	p:supexec_blknum,a
	cmp	#SE_NUM_BLOCKS-1,a
	bseq	<CopySysMemToAC3OutputFile

	ENDIF	; @DEF('SIM_XBOX_SYS_MEM') 

; 	All Done with Function 3.

SEF3Done:

	IF	@DEF('RTSYS')

;	Dolby development hardware only (RTSYS)

	move	x:config_list+SE_IN_DO_GAME,a
	move	p:supexec_blknum,x0	; set up x0 with current block number
	bsr		rt_output			; send output to real-time hardware

	ENDIF	; @DEF('RTSYS')

;-------------------------------------------------------------------
; 	Increment Block Number, and wrap if necessary.

SEBlkAdj:

	move	p:supexec_blknum,a
	add		#1,a
	cmp		#SE_NUM_BLOCKS,a
	clr		a ifge
	move	a1,p:supexec_blknum

;	Increment the ping-pong for pcm input

	move	#>LID_PING_PONG,a					; Packet ID
	jsr		LOADERBASE							; Call the Loader


	IF @DEF('PERF_COUNTERS')
        ;***********************************************************************
		;
		; This section checks to see if we exceed time limit,
		; and if we did, it enters INFINITE LOOP (a VERY HARD CRASH).
		;
        move    x:TIMER1_COUNT,a	; calculate time algorithm took to run
        movem   p:prev_time,x0
        sub     x0,a        
        movem   a1,p:elapsed_time	; save it in elapsed time
        
        move    #MAX_EP_CYCLES,x0	; compare elapsed time to max allowed
        cmpu    x0,a
        blt     <SELoop				; if algoithm did NOT exceed, keep processing.

;        move    p:count,a			; if algorithm DID exceed, check if this happened
;									; immediately after chip rebooted. 
;        cmp     #>MAX_TIMES_IGNORE_TIMER_AFTER_REBOOT,a
;        blt     <SELoop				; If it did, ignore.

        move    #$bbbbbb,x0			; MAJOR FAILURE!  Algorithm exceed 5.33 ms allowed time.
        move    x0,p:suprexec_temp	; write tag to indicate failure.
		
		IF	@DEF('INFINITE_LOOP_IF_TIME_OVERRUN')
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		bra		<*					; HARD CRASH - INFINITE LOOP to isolate problem during debugging.
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		ENDIF	; IF @DEF('INFINITE_LOOP_IF_TIME_OVERRUN')

        IF      @DEF('ERRHALT')
        bra     <*
        ENDIF
        ;***********************************************************************
	ENDIF	; 	IF @DEF('PERF_COUNTERS')


	bra	<SELoop
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;*******************************************************************
;*
;*	Subroutine Name:	handle_error
;*																								
;*  Description:		it will attempt to gracefully handle
;*                              an error condition
;*******************************************************************

handle_error:

	bra    <SELoop
         
	
	IF		@DEF('TRASH_DSP_MEMORY')
;*******************************************************************
;*
;*	Subroutine Name:	trashmem
;*																								
;*  Description:		trashes DSP memory as an acid test
;*
;*	Input:
;*											
;*	Output:
;*											
;*	Modifies:
;*
;*	Locals:
;*
;*	Stack Space Used:
;*
;*******************************************************************

trashmem:

;	Overwrite p: DSP memory from DOLBYEXECBASE to top of p: space.

trashp:

	move	#>DOLBYEXECBASE,x0		; get base of Dolby functions
	move	x0,r0					; save as start address
	move	#>PMEMSIZE,a			; get size of p:
	sub		x0,a					; compute size of Dolby Fcn space
	move	a,n0					; and trash this many locations
	move	#>TRASHWORD,a			; overwrite word

	dor		n0,trashplp
		move a,p:(r0)+
trashplp:

;	Overwrite y: DSP memory from $0 to top of y: space

trashy:

	move	#YMEMSIZE,n0				; trash all of y:
	move	#0,r0						; start at zero
	
	dor		n0,trashylp
		move	a,y:(r0)+			
trashylp:		

;	Overwrite x: DSP memory from $0 to
;	to bottom of stack extension
;	(stack extension is located at top of x:)

trashx:

	move	#CONFIGBASE,n0		; trash everything in x: below config	
	move	#0,r0				; start at zero
	
	dor		n0,trashxlp
		move	a,x:(r0)+	
trashxlp:		

	rts

;*******************************************************************
	ENDIF	; @DEF('TRASH_DSP_MEMORY')


;*******************************************************************
	endsec		; SuperExec
	end			; end of file

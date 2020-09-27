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
	jsr	LOADERBASE				; Call the Loader



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


;	Increment the ping-pong for pcm input

	move	#>LID_PING_PONG,a					; Packet ID
	jsr    	LOADERBASE							; Call the Loader


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


        ; stop here
	;bra	<SELoop
        bra     <*
		
         
	
;*******************************************************************
	endsec		; SuperExec
	end			; end of file

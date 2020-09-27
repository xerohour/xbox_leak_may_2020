;	File	:	XTalk.asm
;	Version	:	1.01
;	Date	:	8/9/2000
;
;	Author	:	RDA
;       Modified/optimized by georgioc so single xtalk can do 4in->4out
;
;
; Description
; ===========
;	This file implements cross-talk cancellation functionality on
;	a Motorola 56362 processor.
;
;	This source file requires definitions from source "XTC.inc" to work 
;	correctly.  The file should usually be included the calling function
;	that controls the cross talk operation.
;
;	No data is allocated in the cross-talk canceller. On entry to
;	each function R5 contains a pointer to the approproate data block.
;	No registers are explicitely stacked in the code, however
;	subroutine calls and loop instructions will use the hardware 
;	stack.
;
;	Since no registers are preserved in the functions in this module,
;	the exit conditions for each function lists the registers that
;	are used (i.e. corrupted).  It is up to the caller to preserve
;	registers as necessary.
;
; Version History
; ===============
;	Version 1.0
;		Functional, but not optimised.
;
;	Version 1.01
;		Modified to use R5 instead of R0 as the workspace pointer

	org	p:

;###########################################################
;	The file "xtalk.inc" must either be included here or
;	before this point
;###########################################################
;	include 'xtalk.inc'


        IF @DEF('STANDALONE')
        include "util.h"
        ENDIF

	include 'xtc.inc'

;===================================
; 	CTC_Initialise
;===================================
;
; On Entry
;	r5=Pointer to crosstalk canceller data structure
;
; On Exit
;	x0	Corrupted
;	r0	Corrupted	n0	Corrupted	m0	-1
;
;	Initialise the system.
;		- Set up the default parameters
;		- Clear the delay buffers and IIR buffer
;

StartCTC

        FXEntryPoint

        IF @DEF('FX_TEST')

        ; setup I/O ptrs

        move    #>kMixBuf00,x0     
        move    #>kMixBuf01,y0
        move    x0,x:(rD+FX_STATE_IO4_IN0)
        move    y0,x:(rD+FX_STATE_IO4_IN1)

        move    #>kMixBuf04,x0     
        move    #>kMixBuf05,y0
        move    x0,x:(rD+FX_STATE_IO4_IN2)
        move    y0,x:(rD+FX_STATE_IO4_IN3)

        move    #>kReadWriteMixBuf00,x0     
        move    #>kReadWriteMixBuf01,y0
        move    #>kReadWriteMixBuf04,x1     
        move    #>kReadWriteMixBuf05,y1

        move    x0,x:(rD+FX_STATE_IO4_OUT0)
        move    y0,x:(rD+FX_STATE_IO4_OUT1)
        move    x1,x:(rD+FX_STATE_IO4_OUT2)
        move    y1,x:(rD+FX_STATE_IO4_OUT3)

        ENDIF

CTC_Initialise
	move	#CTC_COEFF_A1_DEFAULT,x0
	move	x0,x:(r5+CTC_Coeff_A1)

	move	#CTC_COEFF_B0_DEFAULT,x0
	move	x0,x:(r5+CTC_Coeff_B0)

ComputeFX

	move	#$ffffff,m1				      		; Set the linear addressing modes
	move	m1,m2					      		; for all the address registers
        move    m1,m0
	move	m1,m3
	move	m1,m4
	move	m1,m5

        bset    #20,sr
        ; process front left,right

        bsr     CTC_ProcessFrameFront

        ; Store the updated index to the delay buffers
	move	n3,x:(r5+CTC_FrontDelayBufferIndex)	     

        ; process rear left,right
        
        bsr     CTC_ProcessFrameRear
        
        bclr    #20,sr
        ; Store the updated index to the delay buffers
	move	n3,x:(r5+CTC_RearDelayBufferIndex)	     

        FXExit	

;===================================
; 	CTC_ProcessFrame
;===================================
;
; On entry
;	r5=Pointer to crosstalk canceller data structure
;
;	Data in the input frames should be aligned in the LSB of the words.
;
; On exit
;	Data in buffers input pointed to by r1 and r2 on entry are updated 
;	to contain processed data.
;
;	a	Corrupted
;	b	Corrupted
;	x	Corrupted
;	y	Corrupted
;

;	r1	Corrupted	n1	Corrupted	m1	-1
;	r2	Corrupted	n2	Corrupted	m2	-1
;	r3	Corrupted	n3	Corrupted	m3	-1
;	r4	Corrupted	n4	Corrupted	m4	-1
;									m5  -1


CTC_ProcessFrameFront

        ; r1=Pointer to 32 word left channel input sample frame
        ; r2=Pointer to 32 word right channel input sample frame
        ; r6=Pointer to 32 word left channel output sample frame
        ; r7=Pointer to 32 word right channel output sample frame

        move    x:(r5+FX_STATE_IO4_IN0),r1
        move    x:(r5+FX_STATE_IO4_IN1),r2
        move    x:(r5+FX_STATE_IO4_OUT0),r6
        move    x:(r5+FX_STATE_IO4_OUT1),r7

	move	r5,a
	add	#CTC_FrontDelayBuffer_Left,a
	move	a,r3
													; Initialise the pointer to the left delay buffer
	move	r5,a
	add	#CTC_FrontDelayBuffer_Right,a
	move	a,r4
													; Initialise the indices into the delay buffers
	move	x:(r5+CTC_FrontDelayBufferIndex),n3
	move	n3,n4

        lua     (r5+CTC_IIR_FrontDelayElement_Left),r0

        bra     CTC_Compute

CTC_ProcessFrameRear
        move    x:(r5+FX_STATE_IO4_IN2),r1
        move    x:(r5+FX_STATE_IO4_IN3),r2
        move    x:(r5+FX_STATE_IO4_OUT2),r6
        move    x:(r5+FX_STATE_IO4_OUT3),r7

	move	r5,a
	add	#CTC_RearDelayBuffer_Left,a
	move	a,r3
													; Initialise the pointer to the left delay buffer
	move	r5,a
	add	#CTC_RearDelayBuffer_Right,a
	move	a,r4
													; Initialise the indices into the delay buffers
	move	x:(r5+CTC_RearDelayBufferIndex),n3
	move	n3,n4

        lua     (r5+CTC_IIR_RearDelayElement_Left),r0

CTC_Compute

	; Initialise the pointer to the left delay buffer
       	; r5 = Pointer to base of crosstalk data structure
       	; r1 = Input sample buffer Left
       	; r2 = Input sample buffer Right
       	; r3 = Pointer to base of left delay buffer structure
       	; r4 = Pointer to base of right delay buffer structure

	move	x:(r5+CTC_Coeff_B0),y1			       	; y1 = Coefficient B0

	dor	#kFrameLength,CTC_LoopEnd		       	; Process the 32 samples in the two input frames

          	move	x:(r5+CTC_Coeff_A1),y0			       	; y0 = Coefficient A1

        	;-------------------------------
        	;	Implement the IIR filters
        	;-------------------------------								

		move 	x:(r0),x0	                        ; x0 = IIR delay element left

		mpy	x0,y0,b					; b  = A1*IIRleft
	
		move	x:(r3+n3),x1				; x1 = Output of left delay buffer
		mac	x1,y1,b					; b  = DelayedSampleLeft*B0 + A1*IIRleft

		move	b,x:(r0)	                        ; Store output of IIR filter in its delay element
	
		
		move 	x:(r0+CTC_DELAYBUFFER_SIZE+1),x0        ; x0 = IIR delay element right
		mpy	x0,y0,a			  		; a  = A1*IIRleft
	
		move	x:(r4+n4),x1		  		; x1 = Output of right delay buffer
		mac	x1,y1,a			  		; a  = DelayedSampleRight*B0 + A1*IIRright

		move	a,x:(r0+CTC_DELAYBUFFER_SIZE+1)	        ; Store output of IIR filter in its delay element


        	;-------------------------------
        	;	Combine output of IIR 
        	;	filters with input data
        	;-------------------------------
			
		move	x:(r1)+,x0				; Get input sample from left channel
		move	x:(r2)+,x1				; Get input sample from right channel

		add	x0,a					; Add to the output from the right IIR filter
		add	x1,b					; Add to the output of the left IIR filter

		move	a,x:(r3+n3)				; Store result in left delay buffer
		move	b,x:(r4+n4)				; Store result in right delay buffer

                ;
                ; check if we are supposed to be enabled. if not, just copy input->output
                ;

                move    x:(rD+FX_STATE_FLAGS),y0   
                brset   #BIT_FX_STATE_FLAG_GLOBAL_FX,y0,CTC_UpdateOutput

                ; xtalk is disabled, replace xtalk data with input samples

                clr     a
                clr     b
                move    x0,a
                move    x1,b

CTC_UpdateOutput

                ;
                ; combine crosstalk output with existing output mixbin data
                ;

                move    x:(r6),x0
                move    x:(r7),x1

                add     x0,a
                add     x1,b


		move	a,x:(r6)+				; Store result in left output buffer
		move	b,x:(r7)+				; Store result in right output buffer


        	;-------------------------------
        	;	Increment cyclic sample 
        	;	delay buffer index
        	;-------------------------------

		move	n3,a
		add	#1,a
		cmp	#CTC_DELAYBUFFER_SIZE,a
		blt	CTC_IndexUpdated
		clr	a
CTC_IndexUpdated
		move	a,n3
		move	a,n4

CTC_LoopEnd	
        nop
	rts

FX_END
        nop

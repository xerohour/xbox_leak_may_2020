;	File	:	Reverb.asm
;	Version	:	2.11
;	Date	:	17/07/2001
;
;	Author	:	RDA
;				Sensaura Ltd
;
;
; Description
; ===========
; Sensaura's reverb engine
;
; Version history
; ===============
;	0.1		First implementation. Untested
;	0.2		Separated support functions into a separate file called 
;			'RvbFuncs.asm'
;			Implemented a cunning version of the crossfeed matrix 
;			(saved about 6 instructions)
;	0.3		Implemented the crossfeed matrix with a tripple nexted 
;			loop (saved 16 instructions). Tested thoroughly. 
;	0.4		The stereo input frames are now mixed into the main delay
;			buffer. The input and output frames are passed to the 
;			process function with R1 and R2.
;	0.41	Realised that the index into the delay line for the read 
;			taps need a lower value	than the write taps for correct 
;			operation. Modified the code accordingly
;	0.5		Functional reverb engine with compile time reverb constants.
;			The early reflections works correctly. The reverberation 
;			sounds like it works correctly although it need to be
;			validated.
;	0.51	Modified so that the workspace is accessed indirectly
;			through r5.
;			Initialisation of the DMA command blocks works correctly.
;			Removed the function 'RVB_SetnOutputChannels'. (Only called
;			once and it is just one instruction long).
;	0.52	An error was found in the structure of reverb engine. It
;			was corrected and recoded. The reverb now feeds back into 
;			the start of the crossfeed matrix.
;	0.53	Incorporated the correct scaling into the output mixing stage.
;
;	2.00	Begun the re-write for the version 2 reverb engine
;	2.01	Changed entry conditions : Pointers to the input and output 
;			frames are now located in the memory block 'pIO_Frames' 
;			(which can be found in the reverb engine's private workspace).
;			Replaced the variable 'TwoChannels' with the variable 
;			'nOutputChannels'. 'nOutputChannels' should be set equal to the
;			number of output channels
;	2.02	Added support for fading the output gains to their new values
;			and muting buffers while their delay taps are moved. The fader 
;			is enable/disabled by setting the variable FADER_ENABLED.
;	2.03	Modified to enable the internals of the reverb engine to 
;			operate at half the sample rate.
;			Run 'reverb2h" to initialise the debugger for half rate
;			processing. Presets and macros are stored in directories
;			"eax_2" and "macros_2".
;	2.04	The reverb engine is now operating correctly at half the sample
;			rate.
;			Added support for the debugger to reset the reverb engine.
;	2.10	Modified for XBox.
;				- r5 replaced by rD
;				- Made the code relocateable
;				- Removed RVB_Initialise
;				- Removed the guts of RVB_UpdateParameters
;				- Changed the location of the input buffers for RVB_ProcessFrame
;				- Added an option to skip first processing stage to mix the stereo 
;				inputs into a mono buffer & simply accept one mono input instead.
;				- Changed output mixing stage so we don't use the output buffers
;				as temporary storage. Now we accumulate into them.
;	2.11	Reinstated RVB_UpdateParameters
;			Bug fixes as follows:
;				- Fixed artefact in half-rate version by regenerating the correct
;				  pointers to the temp bins each time a new set of parameters is downloaded.
;				- Fixed bug in reverb delay tap fader so that it produces identical
;				  results to when not fading. (The bug was due to the difference 
;				  between bytes and words.)
;				- Fixed a potential overflow problem in RVB_UpdateDmaCommandBlockOffset 
;				  when updating command block offsets.
;
; Entry points into the reverb module are as follows:
;	RVB_Initialise
;	RVB_Reset
;	RVB_UpdateParameters
;	RVB_ProcessFrame
;
;
;
;
; 'RVB_ProcessFrame' performs the body of the processing within the reverb engine.  
; The function accepts audio data from 2 sources.  First there is a stereo pair 
; of frame buffers representing the primary input for the reverb engine.  Second
; are the 31 input buffers that provide taps on the various delay lines within 
; the system.
;
; The input buffers are labelled 0 to 30 and filled as follows via the DMA
; controller.
;
;	Read Buffer	Data on entry
;	----------------------
;		0		First reflection tap 0
;		1		Channel 0 - Reflection input tap 1 on main delay line
;		2		Channel 1 - Reflection input tap 2 on main delay line
;		3		Channel 2 - Reflection input tap 3 on main delay line
;		4		Channel 3 - Reflection input tap 4 on main delay line
;		5		Channel 0 - Short reverb input tap A on main delay line
;		6		Channel 0 - Short reverb input tap B on main delay line
;		7		Channel 1 - Short reverb input tap A on main delay line
;		8		Channel 1 - Short reverb input tap B on main delay line
;		9		Channel 2 - Short reverb input tap A on main delay line
;		10		Channel 2 - Short reverb input tap B on main delay line
;		11		Channel 3 - Short reverb input tap A on main delay line
;		12		Channel 3 - Short reverb input tap B on main delay line
;		13		Channel 0 - Long reverb input tap a on main delay line
;		14		Channel 1 - Long reverb input tap a on main delay line
;		15		Channel 0 - Long reverb input tap b on extension 1 of main delay line
;		16		Channel 1 - Long reverb input tap b on extension 1 of main delay line
;		17		Channel 0 - Long reverb input tap c on extension 2 of main delay line
;		18		Channel 1 - Long reverb input tap c on extension 2 of main delay line
;		19		Channel 0 - Long reverb input tap d on extension 3 of on main delay line
;		20		Channel 1 - Long reverb input tap d on extension 3 of on main delay line
;		21		Channel 0 - Reflection feedback delay buffer
;		22		Channel 1 - Reflection feedback delay buffer
;		23		Channel 2 - Reflection feedback delay buffer
;		24		Channel 3 - Reflection feedback delay buffer
;		25		Channel 0 - Reverb feedback delay buffer
;		26		Channel 1 - Reverb feedback delay buffer
;		27		Channel 2 - Reverb feedback delay buffer
;		28		Channel 3 - Reverb feedback delay buffer
;		29		Channel 0 - Long reveb feedback delay buffer
;		30		Channel 1 - Long reveb feedback delay buffer
;
;
; When the system has finished processing the reverb engine produces two sets
; of output frames. The primary output is either two or four audio
; data buffers corresponding to either stereo or quad output mode.
; The secondary output is 14 frames of audio data corresponding to inputs
; for the various delay lines within the system.  These buffers are stored
; in the memory allocated for the input buffers and have the following
; purpose.
;
;	Write buffer	Data on exit
;		0			Filtered input - writing to main delay line
;		1			Main delay line first IIR
;		2			Main delay line second IIR
;		3			Main delay line third IIR
;		4			Channel 0 - Reflection delay line write
;		5			Channel 1 - Reflection delay line write
;		6			Channel 2 - Reflection delay line write
;		7			Channel 3 - Reflection delay line write
;		8			Channel 0 - Short reverb delay line write
;		9			Channel 1 - Short reverb delay line write
;		10			Channel 2 - Short reverb delay line write
;		11			Channel 3 - Short reverb delay line write
;		12			Channel 0 - Long reverb delay line write
;		13			Channel 1 - Long reverb delay line write





;	include	'DMA_Emulator.asm'			; Emulates the DMA controller by transferring
;										; data to the external memory.

RVB_StartOfMemory
	include 'RvbFuncs.asm'				; Various support functions.
	include 'Parameters.inc'			; Defines various default parameters
	include 'ReverbData.asm'			; Defines the private and scratch workspace

	IF FADER_ENABLED
	include 'RVBFader.asm'				; Defines functions that fade between parameters sets
	ENDIF

	org	p:



;==================================
;		RVB_Reset
;==================================
;	On Entry
;		rD	Pointer to the private workspace of the reverb engine 
;			(rD+0) - a pointer to the DirectX parameters (Unused)
;			(rD+1) - a pointer to the scratch memory
;		The data block 'HostBufferInfo' contains information describing
;		the layout of the reverb buffers.
;
;	On Exit
;		rD	Preserved
;		Assume all other registers Undefined
;
;	Resetting the reverb module performs the following actions
;		1. Reinitialise all the variables to their default values
;		2. Reset the DMA command blocks
;		3. Back to 2 channels
;	Note that resetting the reverb engine is the only way to resize
;	reverb buffers on the host.

RVB_Reset
	bsr RVB_GenerateBufferList
	bsr RVB_CopyParameters

	IF	FADER_ENABLED

	;--------------------------------------
	; Initialise the fader if it is enabled
	;--------------------------------------

	bsr		RVB_Fader_Initialise

	ENDIF

	bsr		RVB_InitialiseDmaCommandBlocks
	rts
					
;==================================
;		RVB_UpdateParameters
;==================================
;	On Entry
;		r5=Pointer to private workspace
;	On Exit
;		

RVB_UpdateParameters
	bsr		RVB_GenerateBufferList

	IF FADER_ENABLED
	bsr		RVB_Fader_StartFading					; Trigger a gradual fading from one set of 
										; parameters to another.
	ENDIF																

	IF (FADER_ENABLED&FADER_ENABLE_DELAYTAPFADE)
		   								; The operation updates
		   								; DMA command blocks corresponding to 
		   								; buffers on the main delay line.
	  
		bsr RVB_UpdateDmaCommandBlocksNotProcessedByFader		; Update the remaining DMA command blocks

	ELSE

		bsr RVB_CopyParameters
        
		bsr RVB_UpdateAllDmaCommandBlocks

	ENDIF

	rts

;-----------------------------------
;		RVB_CopyParameters
;-----------------------------------
;	On Entry
;
;	rD=pointer to base of private data
;
;	On Exit
;		a  Undefined
;		r0 Undefined
;		r1 undefined


RVB_CopyParameters
        ;
        ; copy output gains from host block to internal block
        ;

	move	r5,a
	add    	#ReflectionOutputGain_Channel0-StaticWorkspaceBase,a
	move	a,r0

	add    	#NewReflectionOutputGain_Channel0-ReflectionOutputGain_Channel0,a
	move	a,r1

	dor    	#6,RVB_CopyTransformedOutputGainParametersLoop
		move	x:(r1)+,x0
		move	x0,x:(r0)+
RVB_CopyTransformedOutputGainParametersLoop

        rts




;-----------------------------------
;		RVB_GenerateBufferList
;-----------------------------------
;	On Entry
;
;	rD=pointer to base of private data
;		The entry 'pScratchMemory' should have been filled with
;		the address of the static workspace base address.
;
;	On Exit
;		a  Undefined
;		r0 Undefined
;		rD preserved
RVB_GenerateBufferList
	
	; georgioc - not used anymore the tool creates the pointers
	IF 0
;	move	rD,a
;	add	#(pBuffer-StaticWorkspaceBase),a	; Calculate a pointer to the base of 
;											; the private pointer array that is
;											; to be filled with pointers to the
;											; scratch memory.
;	move	a,r0	 						; R0 points to the base of the buffer pointer block
;
;			 								; Get a pointer to the scratch memory
;	move	x:(rD+pScratchXMemory-StaticWorkspaceBase),a
;	add	#(Buffer0-BaseScratchData),a		; 'a' points to the first frame buffer
;	
;	dor	#31,RVB_GenerateBufferListLoop
;		move	a,x:(r0)+
;		add		#SizeOfFrame,a
;RVB_GenerateBufferListLoop

	ENDIF

	IF @DEF('HALFSAMPLERATE')
	move	rD,a
	add	#(pBuffer0-StaticWorkspaceBase),a	; Calculate a pointer to the base of 
				      			; the private pointer array that is
				      			; to be filled with pointers to the
				      			; scratch memory.
	move	a,r0	 	      			; R0 points to the base of the buffer pointer block
	
	move	x:(r0),a
	
	dor	#31,RVB_GenerateBufferListLoop
		move	a,x:(r0)+
		add		#SizeOfFrame,a
RVB_GenerateBufferListLoop
	ENDIF

	rts




;==================================
;		RVB_ProcessFrame
;==================================
;	On Entry
;		rD=Pointer to reverb data structure
;
;	On Exit
;		a	Undefined
;		x	Undefined
;		y	Undefined
;		r0-r4	Undefined
;		n0,n2
;
;
; Process a frame of audio data to calculate its reverb contribution.
; The processing stages are as follows
;	0.	Transfer data from the Host memory to the DSP
;	1.	Apply the IIR filter to the input stage
;	2.	Wait until DMA transfer from host is complete
;	3.	Perform calculations associated with the reflection buffers
;	4.	Calculate the short reverb feedback and short reverb input 
;		in preparation for the crossfeed matrix
;	5.	Perform long reverb calculations
;	6.	Evaluate the crossfeed matrix
;	7.	Combine the crossfeed from the long reverb
;	8.	Apply short reverb IIRs
;	9.	Mix the output channels.
;
;	During the processing the output frame "IO_LeftFrontOut" is used as an 
;	intermediate buffer. Therefore if data representing the direct path is
;	requried
RVB_ProcessFrame
	
        ;====================================
        ; Initialise DMA transfer to read input frames from delay line scratch
        ;====================================
	
	bsr	RVB_ReadInputFrames
	

	move	#-1,m0
	move	m0,m1
	move	m0,m2
	move	m0,m3
	move	m0,m4
	move	m0,m5
	

	;====================================
	; Combine the stereo input frames into one mono frame
	;====================================
	; Input frames		: Source frame Left,Right
	; Output frames		: Source frame Right
	; Workspace frames	: Destination left front output buffer

	; Add the left and right input frames
	; Use one of the output buffers as a temporary buffer

	IF USE_TWO_INPUTS
                 
	move	x:(rD+FX_STATE_IO2_IN0),r0			; Pointer to the left input frame buffer
	move	x:(rD+FX_STATE_IO2_IN1),r1			; Pointer to the right input frame buffer
	move	x:(rD+FX_STATE_IO2_IN0),r2			;(ignore) Pointer to the front left output frame buffer
	bsr	AccumulateFrame_OutOfPlace
	IF @DEF('HALFSAMPLERATE')

        							; If we are running at half the sample
        							; rate, the input frames are twice the size
        							; of the internal frames, so we need to do
	bsr	AccumulateFrame_OutOfPlace			; another call to the accumulate function

	ENDIF

	ENDIF ; USE_TWO_INPUTS

	move	#$400000,x0		  			; Multiply the result by 1/2
	move	x:(rD+FX_STATE_IO2_IN0),r0                     ; left front out
	bsr	ScaleFrame_InPlace

	IF @DEF('HALFSAMPLERATE')

        ; If we are running at half the sample
        ; rate, the input frames are twice the size
        ; of the internal frames, so we need to do
        ; another call to the scale function

	bsr	ScaleFrame_InPlace
	ENDIF

        ;====================================
        ; Apply IIR filter to input
        ;====================================
        ; Input frames		: Source frame Left
        ; Output frames		: Source frame Left
        ; Workspace frames	: None

        ;------------------------------------
        ; Filter the input data stream at
        ; the full data rate
        ;------------------------------------

	move	x:(rD+FX_STATE_IO2_IN0),r0                     ; left front out
	move	rD,a
	add		#(IIR_Input-StaticWorkspaceBase),a
	move	a,r1						; Apply the input IIR filter
	bsr		RVB_IIR_InPlace
	
	IF @DEF('HALFSAMPLERATE')

        ; If we are running at half the sample
        ; rate, the input frames are twice the size
        ; of the internal frames, so we need to do
        ; another call to the filter function
	bsr		RVB_IIR_InPlace

	ENDIF



	IF @DEF('HALFSAMPLERATE')
        ;----------------------------------
        ;decimate the input data stream to
        ;half the input data rate
        ;----------------------------------
        ;apply the anti-aliasing IIR filter
        ;and the input filter

	move	x:(rD+FX_STATE_IO2_IN0),r0                     ; left front output
	move	rD,a
	add		#(IIR2_AntiAlias_Input-StaticWorkspaceBase),a
	move	a,r1
	bsr		RVB_AntiAlias_InPlace

        ; Decimate the filtered input by
        ; copying every other sample
	move	x:(rD+FX_STATE_IO2_IN0),r0                     ; left front output
	move	r0,r1
	move	#2,n0
	dor	#SizeOfFrame,RVB_DecimateInputLoop
		move	x:(r0)+n0,x0
		move	x0,x:(r1)+
RVB_DecimateInputLoop

	
	ENDIF							; endif HALFSAMPLERATE

	IF      !BLOCKING_DMA
        bsr	DMAWaitForEOL    
	ENDIF

        ;======================================
        ; Apply any fading to the input buffers
        ;======================================
	IF FADER_ENABLED
	
	bsr	RVB_Fader_Process

	ENDIF							; endif FADER_ENABLED

        ;====================================
        ; Calculate early reflections
        ;====================================
        ; Input frames		: 0 , 1 to 4 , 21 to 24
        ; Output frames		: 1 - 4
        ; Workspace frames	: None


        ; Buffer 0,1,21 -> Buffer 1
        ; Buffer 0,2,22 -> Buffer 2
        ; Buffer 0,3,23 -> Buffer 3
        ; Buffer 0,4,24 -> Buffer 4

	move	x:(rD+pBuffer1-StaticWorkspaceBase),r1		;r1=Pointer to the first of the 4 reflection taps
	dor	#4,RVB_EarlyReflectionsLoop1
		move	x:(rD+pBuffer0-StaticWorkspaceBase),r0	; r0=pointer to the common reflection tap
		bsr	AccumulateFrame_InPlace
		
		;nop							; Can't have a 'bsr' at the end of a loop
		move	rD,a					;( 'a' used after the loop )
RVB_EarlyReflectionsLoop1



	move	x:(rD+pBuffer21-StaticWorkspaceBase),r0	
	move	x:(rD+pBuffer1-StaticWorkspaceBase),r1	;r1=Pointer to the first of the 4 reflection taps
	
	move	rD,a
	add		#(ReflectionsFeedbackGain-StaticWorkspaceBase),a
	move	a,r2
	dor	#4,RVB_EarlyReflectionsLoop2
		move	x:(r2)+,x0
		bsr	MultiplyAccumulateFrame_InPlace		; frame1[n]=frame1[n]+x0*frame0[n]

		;nop		 ; Can't have a bsr at the end of a do loop
		move	rD,a	 ; This instruction is needed after the loop
RVB_EarlyReflectionsLoop2

        ; Buffer 0 is free
        ; Buffers 1-4 contain data that should be 
        ; written to reflection input delay buffer



        IF SKIP_SHORT_REVERB

        bra     RVB_SkipShortReverb  

        ELSE
                     
        ;====================================
        ; Add the short reverb feedback to 
        ; short reverb inputs
        ;====================================
        ; Input frames		: 25 to 28 , 5 to 12
        ; Output frames		: 25 to 28
        ; Workspace frames	: None


        ; Get a pointer to the short reverb input gains.
	add	#(ShortReverbInputGain_Channel0a-StaticWorkspaceBase),a
	move	a,r3
	
	move	x:(rD+pBuffer5-StaticWorkspaceBase),r0
	move	x:(rD+pBuffer25-StaticWorkspaceBase),r2
	
	move	#SizeOfFrame,n2

	dor	#4,RVB_ShortReverbLoop1
		dor	#2,RVB_ShortReverbLoop2
			move	r2,r1
			move	x:(r3)+,x0
			bsr	MultiplyAccumulateFrame_InPlace	; frame1[n]=frame1[n]+x0*frame0[n]
			nop
RVB_ShortReverbLoop2
		move	x:(r2)+n2,x0			       	; Increment r2 (Ignore result)
RVB_ShortReverbLoop1


        ; Buffers 25-28 contain input data for 
        ; the crossfeed matrix
        ; Buffers 5-12 are free

        ENDIF ; skip short reverb

RVB_SkipShortReverb

        ;====================================
        ; Long Reverb
        ;====================================
        ;   Input frames		: Buffers 13 - 20 , 29,30
        ;	Output frames		: Buffers 9,10,11,29,30
        ;	Workspace frames	: None


        ; Accumulate the long reverb input taps

	move	x:(rD+pBuffer14-StaticWorkspaceBase),r0
	move	x:(rD+pBuffer15-StaticWorkspaceBase),r1
	move	x:(rD+pBuffer9-StaticWorkspaceBase),r2
	bsr		AccumulateFrame_OutOfPlace

	move	x:(rD+pBuffer13-StaticWorkspaceBase),r0
	;move	x:(rD+pBuffer16-StaticWorkspaceBase),r1		; (Set up by exit conditions of previous subroutine)
	;move	x:(rD+pBuffer10-StaticWorkspaceBase),r2		; (Set up by exit conditions of previous subroutine)
	bsr		AccumulateFrame_OutOfPlace
	

	move	x:(rD+pBuffer17-StaticWorkspaceBase),r0
	move	x:(rD+pBuffer9-StaticWorkspaceBase),r1
	bsr		AccumulateFrame_InPlace

	;move	x:(rD+pBuffer18-StaticWorkspaceBase),r0		; (Set up by exit conditions of previous subroutine)
	;move	x:(rD+pBuffer10-StaticWorkspaceBase),r1		; (Set up by exit conditions of previous subroutine)
	bsr		AccumulateFrame_InPlace

	move	x:(rD+pBuffer20-StaticWorkspaceBase),r0
	move	x:(rD+pBuffer9-StaticWorkspaceBase),r1
	bsr		AccumulateFrame_InPlace

	move	x:(rD+pBuffer19-StaticWorkspaceBase),r0
	;move	x:(rD+pBuffer10-StaticWorkspaceBase),r1		; (Set up by exit conditions of previous subroutine)
	bsr		AccumulateFrame_InPlace


       	;-------------------------------------------
       	; Mix and scale the accumulated outputs into
       	; a separate buffer in preparation 
       	; for mixing with the outputs of the 
       	; crossfeed matrix
       	;-------------------------------------------

	move	x:(rD+pBuffer9-StaticWorkspaceBase),r0
	move	x:(rD+pBuffer10-StaticWorkspaceBase),r1
	move	x:(rD+pBuffer11-StaticWorkspaceBase),r2
	bsr    	AccumulateFrame_OutOfPlace
	
	move	x:(rD+pBuffer11-StaticWorkspaceBase),r0
	move	x:(rD+LongReverbCrossfeedGain-StaticWorkspaceBase),x0
	bsr    	ScaleFrame_InPlace

	

        ; Scale outputs of the large adder by the
        ; long reverb input gain

	move	x:(rD+pBuffer9-StaticWorkspaceBase),r0
	move	x:(rD+LongReverbInputGain-StaticWorkspaceBase),x0
	bsr    	ScaleFrame_InPlace			        ; Buffer9 =Buffer9 *LongReverbInputGain
	bsr    	ScaleFrame_InPlace			        ; Buffer10=Buffer10*LongReverbInputGain
	
	
	move	x:(rD+pBuffer29-StaticWorkspaceBase),r0
	move	x:(rD+pBuffer9-StaticWorkspaceBase),r1
	bsr    	AccumulateFrame_InPlace			        ; Buffer9 =Buffer9 +Buffer29
	bsr    	AccumulateFrame_InPlace			        ; Buffer10=Buffer10+Buffer30

	move	x:(rD+pBuffer9-StaticWorkspaceBase),r0	        ; Copy the buffers for 
	move	x:(rD+pBuffer29-StaticWorkspaceBase),r1	        ; the long reverb outputs
	bsr	CopyFrame
	bsr	CopyFrame

	; Apply the IIR within the long reverb feedback
	move	x:(rD+pBuffer9-StaticWorkspaceBase),r0
	move	rD,a
	add	#(IIR_LongReverb_Channel0-StaticWorkspaceBase),a
	move	a,r1
	bsr	RVB_IIR_InPlace

	move	rD,a
	add	#(IIR_LongReverb_Channel1-StaticWorkspaceBase),a
	move	a,r1
	bsr	RVB_IIR_InPlace


        ; Long reverb outputs in frames 29 and 30
        ; Long reverb delay buffers input in frames 9 and 10
        ; Long reverb crossfeed in frame 11


        ;================================
        ; Long reverb main delay line
        ;================================
        ; Calculate IIRs on the main delay line
        ; Process read buffers 14,16 and 18 in place.

	move	x:(rD+pBuffer14-StaticWorkspaceBase),r0
	move	rD,a
	add	#(IIR_LongReverb_MainDelay0-StaticWorkspaceBase),a
	move	a,r1
	move	#SizeOfFrame,n0
	move	#SizeOfIIR_Data,n1
	dor	#3,RVB_LongReverbLoop3
		bsr	RVB_IIR_InPlace

		move	x:(r0)+n0,x0				; Increment r0 so it points to the next frame

        	; The IIR operation increments r0 by SizeOfFrame,
        	; so after this instruction, r0 has been 
        	; incremented by a total of 2*SizeOfFrame.

		move	x:(r1)+n1,x0				; Increment r1 so it points to the next IIR block
RVB_LongReverbLoop3
				    				; Outputs in frames 22,24 and 26.

        ;====================================
        ; Crossfeed matrix
        ;====================================
        ; Input frames      : 25 to 28
        ; Output frames     : 5 to 8
        ; Workspace frames  : None

        IF      SKIP_CROSSFEED

        bra     SkipCrossfeed

        ELSE
                 
	move	x:(rD+pBuffer5-StaticWorkspaceBase),r4	        ; r4=Pointer to output frames
							 	; Output frames are in buffers 5 to 8
	
	move	rD,a
	add	#(CrossfeedMatrixChannel0-StaticWorkspaceBase),a
	move	a,r3					 	; r3=Pointer to coeffs
	
	move	#SizeOfFrame,n0

	dor	#4,RVB_Crossfeed_Loop2			        ; k loop
		move	x:(rD+pBuffer25-StaticWorkspaceBase),r1	; r1=pointer to input frames
								; Input frames are in buffers 25 to 28

		dor	#SizeOfFrame,RVB_Crossfeed_Loop1	; i loop
			move	r1,r0				; Get a pointer to the i'th element of the first input frame
			
			move	r3,r2				; Reset the pointer to the base of the coefficients
	
			clr	a

			dor	#4,RVB_Crossfeed_Loop	        ; j loop
				move	x:(r0)+n0,x0		; Read data from BufferIn[j][i]
				move	x:(r2)+,x1		; Read coefficient j
				macr	x0,x1,a
RVB_Crossfeed_Loop
			move	a,x:(r4)+			; Write FrameOut[i]
			
			move	x:(r1)+,x0			; Increment pointer to input frame
								; (Ignore result of load)
RVB_Crossfeed_Loop1
		move	x:(r3)-,x0				; Select the next set of coefficients
								; (Ignore result of load)
RVB_Crossfeed_Loop2


								; Results in frames 5-8
        ENDIF ; Crossfeed

SkipCrossfeed

        ;====================================
        ; Add long reverb to crossfeed output
        ;====================================
        ; Input frames		: 5-8,11
        ; Output frames		: 5-8
        ; Workspace frames	: None

	move	x:(rD+pBuffer5-StaticWorkspaceBase),r1
	dor	#4,RVB_AddLongReverbLoop
		move	x:(rD+pBuffer11-StaticWorkspaceBase),r0
		bsr	AccumulateFrame_InPlace

		;nop						; (Can't have a 'bsr' at the end of a do loop)
SkipCrossfeedAndLongReverb
		move	rD,a					; (This instruction is needed after the loop)
RVB_AddLongReverbLoop


        ;====================================
        ; Apply short Reverb IIRs
        ;====================================
        ; Input frames		: 5-8
        ; Output frames		: 5-8
        ; Workspace frames	: None

	;move	#SizeOfFrame,n2
	move	#SizeOfIIR_Data,n3

	move	x:(rD+pBuffer5-StaticWorkspaceBase),r0	        ; r0 points to the first of the 4 consecutive 
								; buffers to process
	
	;move	rD,a						;( This instruction is include in place of the 
								; 'nop' at the end of the previous 'do' loop.
	add	#(IIR_ShortReverbChannel0-StaticWorkspaceBase),a
	move	a,r3						; r3 points to the first of the 4 consecutive 
								; IIR filter structures

	dor	#4,RVB_ReverbIIRLoop
		move	r3,r1					; A pointer to the filter

;*********************************************************************		
; DEBUGGING CODE - to replace the IIR filter with a divide by 2
;*********************************************************************
;		msg		"Replaced short reverb IIR filters with a /2"
;		dor	#SizeOfFrame,RVB_TestIIR_Loop
;		move	x:(r0),a
;		asr		a
;		move	a,x:(r0)+
;RVB_TestIIR_Loop
;*********************************************************************
;	End of debugging code
;**	*******************************************************************
		bsr    	RVB_IIR_InPlace


        	; Filter the buffer.
        	; Increment r0 by 'SizeOfFrame'


		move	x:(r3)+n3,x0				; Increment r3 by the size of a IIR block
RVB_ReverbIIRLoop



		      						; Results in frames 5-8 (ready to be written to feedback buffers)



RVB_FinishedCrossfeed


		; The following buffers now contain data
		;	Buffer 0	Unused (Will be filled with data to send to BL40)
		;	Buffer 1	Short reflection 0 (Send to BL00 & output mixer)
		;	Buffer 2	Short reflection 1 (Send to BL01 & output mixer)
		;	Buffer 3	Short reflection 2 (Send to BL02 & output mixer)
		;	Buffer 4	Short reflection 3 (Send to BL03 & output mixer)
		;	Buffer 5	Short reverb 0 (Send to BL30)
		;	Buffer 6	Short reverb 1 (Send to BL31)
		;	Buffer 7	Short reverb 2 (Send to BL32)
		;	Buffer 8	Short reverb 3 (Send to BL33)
		;	Buffer 9	Long reverb 0 (Send to BL50)
		;	Buffer 10	Long reverb 1 (Send to BL51)
		;	Buffer 11	Unused
		;	Buffer 12	Unused
		;	Buffer 13	Unused
		;	Buffer 14	Filtered main buffer 0 (Send to BL41)
		;	Buffer 15	Unused
		;	Buffer 16	Filtered main buffer 1 (Send to BL42)
		;	Buffer 17	Unused
		;	Buffer 18	Filtered main buffer 2 (Send to BL43)
		;	Buffer 19	Unused
		;	Buffer 20	Unused
		;	Buffer 21	Unused
		;	Buffer 22	Unused
		;	Buffer 23	Unused
		;	Buffer 24	Unused
		;	Buffer 25	Short reverb 0 (Send to output mixer)
		;	Buffer 26	Short reverb 1 (Send to output mixer)
		;	Buffer 27	Short reverb 2 (Send to output mixer)
		;	Buffer 28	Short reverb 3 (Send to output mixer)
		;	Buffer 29	Long reverb 0 (Send to output mixer)
		;	Buffer 30	Long reverb 1 (Send to output mixer)


        ;====================================
        ; Copy the filered input frame into the buffer that
        ; will be copied into the main delay buffer
        ;====================================

	move	x:(rD+FX_STATE_IO2_IN0),r0	                ; Copy the (decimated) processed input frame to buffer 0
	move	x:(rD+pBuffer0-StaticWorkspaceBase),r1
	bsr		CopyFrame


        ;====================================
        ; Mix the output channels
        ;====================================
        ; Two channel mode
        ;   Input frames		: Buffers 1,2, 25,26, 29
        ;   Input frames		: Buffers 3,4, 27,28, 30
        ;	Output frames		: Left-Out, Right-Out
        ;	Workspace frames	: Buffers 21,22,23,24

        ;
        ; Four channel mode
        ;   Input frames		: Buffers 1,25,29
        ;   Input frames		: Buffers 2,26,29
        ;   Input frames		: Buffers 3,27,30
        ;   Input frames		: Buffers 4,28,30
        ;	Output frames		: Left-Front
        ;						: Left-Rear
        ;						: Right-Front
        ;						: Right-Rear
        ;	Workspace frames	: Buffers 21,22,23,24


        ; Reflections data in buffers 21-24
        ; Short reverb data in buffer 25-28
        ; Long reverb data in buffers 29-30


	
	move	x:(rD+pBuffer1-StaticWorkspaceBase),r1		; A pointer to reflections data buffers 0 & 1
	move	x:(rD+pBuffer25-StaticWorkspaceBase),r2		; A pointer to short reverb data buffers 0 & 1
	move	x:(rD+pBuffer29-StaticWorkspaceBase),r3		; A pointer to long reverb data buffer 0
	move	x:(rD+pBuffer21-StaticWorkspaceBase),r4		; A pointer to the output data buffer 0 (or 0&1 in 4 channel mode)
														;


	move	rD,a
	add		#(ReflectionOutputGain_Channel0-StaticWorkspaceBase),a
	move	a,r0							      							; A pointer to reflections
																			; output gain 0 and 1

	move	x:(rD+ShortReverbOutputGain_Channel-StaticWorkspaceBase),y0    	; Short reverb
	move	x:(rD+LongReverbOutputGain_Channel-StaticWorkspaceBase),y1    	; Long reverb gain

	; Are we in four channel mode?
	move	x:(rD+nOutputChannels-StaticWorkspaceBase),a
	cmp	#4,a
	beq	RVB_FourChannelMixFront

RVB_TwoChannelMixFront
														; Two channel mix
	bsr	RVB_TwoChannelOutputMix							; Process the left channel
														; Buffers 1,2, 25,26, 29 => 21

														; Initialise a pointer to the right output channel
	move	x:(rD+pBuffer22-StaticWorkspaceBase),r4		; A pointer to the output data buffer 1
														; (r4 set up by previous call to RVB_TwoChannelOutputMix)
	bra	RVB_MixedFront

RVB_FourChannelMixFront
														; Four channel mix
	bsr	RVB_FourChannelOutputMix						; Process the front channels
														; Buffers 1,25,29 => 21
														; Buffers 2,26,29 => 22

														; Initialise a pointer to the rear output channels
	move	x:(rD+pBuffer23-StaticWorkspaceBase),r4		; A pointer to the output data buffers 2&3
RVB_MixedFront


	move	x:(rD+pBuffer3-StaticWorkspaceBase),r1		; A pointer to reflections data buffer 2 & 3
	move	x:(rD+pBuffer27-StaticWorkspaceBase),r2		; A pointer to short reverb data buffer 2 & 3
	move	x:(rD+pBuffer30-StaticWorkspaceBase),r3		; A pointer to long reverb data buffer 1
														; (R3 set up by previous call to RVB_#OutputMix)
														; Pointer to output channel already initialised

	move	rD,a
	add	#(ReflectionOutputGain_Channel2-StaticWorkspaceBase),a
	move	a,r0															; A pointer to reflections
																			; output gain 2 and 3
	
	move	x:(rD+ShortReverbOutputGain_Channel-StaticWorkspaceBase),y0    	; Short reverb gain
	move	x:(rD+LongReverbOutputGain_Channel-StaticWorkspaceBase),y1     	; Long reverb gain

	       							; Are we in four channel mode?
	move	x:(rD+nOutputChannels-StaticWorkspaceBase),a
	cmp    	#4,a
	beq    	RVB_FourChannelMixRear

RVB_TwoChannelMixRear
	bsr	RVB_TwoChannelOutputMix			       			; Two channel mix
														; Mix the right channel
														; Buffers 3,4, 27,28, 30 => 22
	
	bra	RVB_MixedRear

RVB_FourChannelMixRear
	bsr	RVB_FourChannelOutputMix						; Four channel mix
														; Mix the rear channels
														; Buffers 3,27,30 => 23
														; Buffers 4,28,30 => 24
	
	move	x:(rD+pBuffer23-StaticWorkspaceBase),r0
	IF @DEF('HALFSAMPLERATE')
		move	x:(rD+pBuffer27-StaticWorkspaceBase),r1                     ; Use Buffer1 & 2 as temp buffer
		move	rD,a
		add		#(IIR2_AntiAlias_LeftRear-StaticWorkspaceBase),a
		move	a,r2
		bsr		RVB_InterpolateFrame

		move	x:(rD+pBuffer27-StaticWorkspaceBase),r0                     ; Buffer1 & 2 contains interpolated output data
	ENDIF							; HALFSAMPLERATE
	
	move	x:(rD+FX_STATE_IO2_OUT2),r1                     ; left rear out
	;bsr CopyFrame
	bsr	AccumulateFrame_InPlace
	IF @DEF('HALFSAMPLERATE')
		bsr	AccumulateFrame_InPlace
	ENDIF
	
	
	move	x:(rD+pBuffer24-StaticWorkspaceBase),r0
	IF @DEF('HALFSAMPLERATE')
		move	x:(rD+pBuffer27-StaticWorkspaceBase),r1                     ; Use Buffer1 & 2 as temp buffer
		move	rD,a
		add		#(IIR2_AntiAlias_RightRear-StaticWorkspaceBase),a
		move	a,r2
		bsr		RVB_InterpolateFrame

		move	x:(rD+pBuffer27-StaticWorkspaceBase),r0                     ; Buffer1 & 2 contains interpolated output data
	ENDIF							; HALFSAMPLERATE
	
	move	x:(rD+FX_STATE_IO2_OUT3),r1                     ; right rear out
	;bsr CopyFrame
	bsr	AccumulateFrame_InPlace
	IF @DEF('HALFSAMPLERATE')
		bsr	AccumulateFrame_InPlace
	ENDIF

RVB_MixedRear

	; All output channels mixed


	move	x:(rD+pBuffer21-StaticWorkspaceBase),r0
	IF @DEF('HALFSAMPLERATE')
		move	x:(rD+pBuffer27-StaticWorkspaceBase),r1                     ; Use Buffer1 & 2 as temp buffer
		move	rD,a
		add		#(IIR2_AntiAlias_LeftFront-StaticWorkspaceBase),a
		move	a,r2
		bsr		RVB_InterpolateFrame
		move	x:(rD+pBuffer27-StaticWorkspaceBase),r0                     ; Buffer1 & 2 contains interpolated output data
	ENDIF							; HALFSAMPLERATE

	move	x:(rD+FX_STATE_IO2_OUT0),r1                     ; left front out
	;bsr CopyFrame
	bsr	AccumulateFrame_InPlace
	IF @DEF('HALFSAMPLERATE')
		bsr	AccumulateFrame_InPlace
	ENDIF

	move	x:(rD+pBuffer22-StaticWorkspaceBase),r0
	IF @DEF('HALFSAMPLERATE')
		move	x:(rD+pBuffer27-StaticWorkspaceBase),r1                     ; Use Buffer1 & 2 as temp buffer
		move	rD,a
		add		#(IIR2_AntiAlias_RightFront-StaticWorkspaceBase),a
		move	a,r2
		bsr		RVB_InterpolateFrame
		move	x:(rD+pBuffer27-StaticWorkspaceBase),r0                     ; Buffer1 & 2 contains interpolated output data
	ENDIF							; HALFSAMPLERATE

	move	x:(rD+FX_STATE_IO2_OUT1),r1                     ; right front out
	;bsr CopyFrame
	bsr	AccumulateFrame_InPlace
	IF @DEF('HALFSAMPLERATE')
		bsr	AccumulateFrame_InPlace
	ENDIF

        ;====================================
        ; Finished processing all the data.
        ; Write the result to the host
        ;====================================
FinishedProcessing




	bsr		RVB_WriteOutputFrames
	rts





;----------------------------------
;	RVB_TwoChannelOutputMix
;----------------------------------
;	On entry
;		r0	points to Reflection 0 (or 1) gain
;		r1  points to reflection 0 (or 1) buffer
;		r2  points to ShortReverb 0 (or 1) buffer
;		r3  points to LongReverb 0 (or 1) buffer
;		r4	points to output buffer
;		y0	Short reverb 0 (or 1) gain
;		y1	Long reverb 0 (or 1) gain
;
;	On exit
;		a	Undefined
;		b	Undefined
;		x0	Undefined
;		x1	Undefined
;		r0	Undefined
;		r1	Incremented by SizeOfFrame
;		r2	Incremented by SizeOfFrame
;		r3	Incremented by SizeOfFrame
;		r4	Incremented by SizeOfFrame
;		The input channels have mixed into the buffer pointed to by r4 on entry
;		

RVB_TwoChannelOutputMix
	move	#SizeOfFrame,n1
	move	#SizeOfFrame,n2
	move	x:(r0)+,x0					; Increment r0 (Ignore the result)

	dor	#SizeOfFrame,RVB_TwoChannelOutputMixLoop
		move	x:(r1+n1),x1
		move	x:(r0)-,x0
		mpy	x1,x0,a					; Scaled reflection 1

		move	x:(r1)+,x1
		move	x:(r0)+,x0
		macr	x1,x0,a					; + Scaled reflection 0

		move	x:(r2+n2),x1
		macr	x1,y0,a					; + Scaled short reverb 1

		move	x:(r2)+,x1
		macr	x1,y0,a					; + Scaled short reverb 0

		move	x:(r3)+,x1
		macr	x1,y1,a					; + Scaled long reverb 0

		asl	#FixedOutputScaling,a,a

		move	a,x:(r4)+				; Store result
RVB_TwoChannelOutputMixLoop


	rts



;----------------------------------
;	RVB_FourChannelOutputMix
;----------------------------------
;	On entry
;		r0	points to Reflection 0 (or 1) gain
;		r1  points to reflection 0 (or 1) buffer
;		r2  points to ShortReverb 0 (or 1) buffer
;		r3  points to LongReverb 0 (or 1) buffer
;		r4	points to output buffer
;		y0	Short reverb gain
;		y1	Long reverb gain
;
;	On exit
;		a	Undefined
;		b	Undefined
;		x0	Undefined
;		x1	Undefined
;		r0	Undefined
;		r1	Incremented by SizeOfFrame
;		r2	Incremented by SizeOfFrame
;		r3	Incremented by SizeOfFrame
;		r4	Incremented by SizeOfFrame
;		The input channels have mixed into the buffer r4 and r4+SizeOfFrame
;				r4[0: n-1] = ( r0[0] * r1[0: n-1]  +  y0 * r2[0: n-1] + y1/2 * r3[0:n-1] ) << FixedOutputScaling
;				r4[n:2n-1] = ( r0[1] * r1[n:2n-2]  +  y0 * r2[n:2n-1] + y1/2 * r3[0:n-1] ) << FixedOutputScaling

RVB_FourChannelOutputMix
	
	move	#SizeOfFrame,n4
	move	#SizeOfFrame,n2
	move	#SizeOfFrame,n1

	move	x:(r0)+,x0					; Increment r0 (Ignore the result)

	move	y1,a						; Divide the long reverb 
	asr	a						; gain by 2
	move	a,y1

	dor		#SizeOfFrame,RVB_FourChannelOutputMixLoop
		move	x:(r1+n1),x1
		move	x:(r0)-,x0
		mpy    	x1,x0,b					; Scaled reflection 1

		move	x:(r1)+,x1
		move	x:(r0)+,x0
		mpy    	x1,x0,a					; + Scaled reflection 0

		move	x:(r2+n2),x1
		macr	x1,y0,b					; + Scaled short reverb 1

		move	x:(r2)+,x1
		macr	x1,y0,a					; + Scaled short reverb 0



		move	x:(r3)+,x1
		macr	x1,y1,b					; + Scaled long reverb 1
		macr	x1,y1,a					; + Scaled long reverb 0

		asl    	#FixedOutputScaling,b,b
		asl    	#FixedOutputScaling,a,a

		move	b,x:(r4+n4)				; Store result 1
		move	a,x:(r4)+				; Store result 0

RVB_FourChannelOutputMixLoop
	rts




;----------------------------------
;	RVB_ReadInputFrames
;----------------------------------
;	On Entry
;		rD	Pointer to Reverb engine workspace

;	Configure DMA controller to transfer data from the host into the GP memory
;	Wait until the transfer is complete before returning
RVB_ReadInputFrames

	;DEBUGGERLOG	#0
	;bsr		DMA_WaitUntilReady	; During debugging we don't return from the DMA 
		  				; transfer until the DMA transfer has completed,
		  				; so this call is unnecessary
	

	move	rD,a
	add	#(DMA_ControlBlock_Rd0-StaticWorkspaceBase),a
        IF !@DEF('SIM')

        IF      BLOCKING_DMA
	bsr     DMAStartNode_Wait   
        ELSE
	bsr 	DMAStartNode   
        ENDIF

        ENDIF

	rts



;----------------------------------
;	RVB_WriteOutputFrames
;----------------------------------
;
;	Configure the DMA controller to transfer results frames from the GP to the host
;	Wait until the transfer is complete before returning
RVB_WriteOutputFrames


	move	rD,a
	add	#(DMA_ControlBlock_Wr0-StaticWorkspaceBase),a

        IF !@DEF('SIM')

	bsr     DMAStartNode_Wait  

        ENDIF

	
	rts






;----------------------------------
;	 RVB_IIR_InPlace
;----------------------------------
;	On Entry
;		r0	Pointer to buffer to process
;		r1	Pointer to IIR data block
;
;	On Exit
;		a	Undefined
;		x	Undefined
;		y	Undefined
;		r0	Points to word after the end of the input buffer
;		r1	Preserved
;
;		Buffer pointed to by r0 on entry has been passed through the IIR filter
;
;	Apply an IIR filter to the specified frame.
;
RVB_IIR_InPlace
	move	x:(r1+IIR_CoefficientA1),y0
	move	x:(r1+IIR_CoefficientB0),y1

	dor		#SizeOfFrame,RVB_IIRLoop
		move	x:(r1+IIR_DelayElement),x0
		mpy		x0,y0,a

		move	x:(r0),x1
		mac		x1,y1,a

		move	a,x:(r1+IIR_DelayElement)
		move	a,x:(r0)+
RVB_IIRLoop
	rts




	IF @DEF('HALFSAMPLERATE')


;----------------------------------
;	RVB_InterpolateFrame					**** This function is only used when processing at half the sample rate ****
;----------------------------------
;	On Entry
;		r0	Pointer to input data frame (Length= SizeOfFrame)
;		r1	Pointer to output data (Length=SizeOfIoFrame)
;		r2	Pointer to IIR output filter data
;		rD	Pointer to reverb engine workspace
;
;	On Exit
;		a	Undefined
;		r0	Undefined
;		r1	Undefined
;		r2	Undefined
;		r3	Undefined
;	
; Interpolate the samples by a factor of 2 by
;	1. Repeating each sample twice.
;	2. Applying an antialiasing filter
; It is NOT acceptable for the input and output frames to be the same.(i.e. Ok for r0!=r1).

RVB_InterpolateFrame
	move	r1,r3						; Make a copy of the output pointer

        ;---------------------------------------
        ; Expand the input frame to the output
        ; frame by repeating input samples twice
        ;---------------------------------------
	dor	#SizeOfFrame,RVB_InterpolateFrameLoop
		move	x:(r0)+,x0
		move	x0,x:(r1)+
		move	x0,x:(r1)+
RVB_InterpolateFrameLoop

        ;--------------------------
        ; Filter the output block
        ;--------------------------

	move	r3,r0						; Pointer to output block
	move	r2,r1						; Pointer to filter
	bsr	RVB_AntiAlias_InPlace
													
	rts



;----------------------------------
;	 RVB_AntiAlias_InPlace					**** This function is only used when processing at half the sample rate ****
;----------------------------------
;	On Entry
;		r0	Pointer to buffer to process
;		r1	Pointer to IIR2 data block
;		rD	Pointer to reverb engine workspace
;
;	On Exit
;		a	Undefined
;		x	Undefined
;		y	Undefined
;		r0	Points to word after the end of the input buffer
;		r1	Preserved
;		r3	Undefined
;
;		Buffer pointed to by r0 on entry has been passed through the IIR filter
;
;	Apply a second order section IIR filter, with coefficients suitable 
;	for a 1 to 2 interpolation, to the specified frame.
;
RVB_AntiAlias_InPlace
	msg		"Turned off antialiasing"
	rts

	move	r0,r3							; Copy the buffer pointer
	move	rD,a
	add	#IIR2_AntiAliasing_Coefficients1-StaticWorkspaceBase,a
	move	a,r2
	bsr	RVB_IIR2_InPlace

	
	move	r3,r0
	move	x:(r1)+,x0						; Increment IIR data 
	move	x:(r1)+,x0						; pointer by 2
	
	move	rD,a
	add	#IIR2_AntiAliasing_Coefficients2-StaticWorkspaceBase,a
	move	a,r2
	bsr	RVB_IIR2_InPlace

	rts



;----------------------------------
;	 RVB_IIR2_InPlace					**** This function is only used when processing at half the sample rate ****
;----------------------------------
;	On Entry
;		r0	Pointer to buffer to process. (Length=SizeOfIoFrame)
;		r1	Pointer to IIR2 data block
;		r2	Pointer to IIR2 coefficient block
;
;	On Exit
;		a	Undefined
;		b	Undefined
;		x	Undefined
;		y	Undefined
;		r0	Points incremented by SizeOfIoFrame
;		r1	Preserved
;		r2	Preserved
;
;		Buffer pointed to by r0 on entry has been passed through the IIR filter
;
;	Apply a second order section IIR filter to the specified frame.
;
;       BUGBUG(georgioc) Optimize IIRs so they dont suck. make them use Y-mem like standalone IIR2

RVB_IIR2_InPlace


	move	x:(r1+IIR2_CoefficientY0),y0			; Load y0
	move	x:(r1+IIR2_CoefficientY1),y1			; Load y1
	
	dor	#SizeOfIoFrame,RVB_IIR2Loop
		move	x:(r2+IIR2_CoefficientA2),x0		; a2
		move	x:(r2+IIR2_CoefficientB2),x1		; b2
		
		mpy	x0,y1,a					; a2*y1
		mpy	x1,y1,b					; b2*y1
				
		move	x:(r2+IIR2_CoefficientA1),x0		; a1
		move	x:(r2+IIR2_CoefficientB1),x1		; b1
		move	y0,y1

		mac	x0,y0,a				        ; a1*y0 + a2*y1
		mac	x1,y0,b				        ; b1*y0 + b2*y1
		
		move	x:(r0),x0				; Sample
		add	x0,b				        ; Sample + b1*y0 + b2*y1

		move	x:(r2+IIR2_CoefficientA0),x1		; a0
		move	b,y0

		mac	y0,x1,a				        ; (Sample + b1*y0 + b2*y1) *a0 + (a1*y0 + a2*y1)
		
		move	a,x:(r0)+				; Store output
RVB_IIR2Loop
	
	move	y0,x:(r1+IIR2_CoefficientY0)			; Store y0
	move	y1,x:(r1+IIR2_CoefficientY1)			; Store y1

	rts

	ENDIF							;HALFSAMPLERATE





;----------------------------------
;	RVB_InitialiseDmaCommandBlocks
;----------------------------------
;	On Entry
;		rD	Pointer to Reverb engine workspace
;
;	On Exit
;		r0		Undefined
;		n0		Undefined
;		m0		-1
;		x0		Undefined
;		a		Undefined
;

;
;	This function initialises the linked list of DMA command blocks. The
;	following operations are performed:
;		1. Create the linked list for the read and write blocks
;		2. Sets up the pointers to the DSP memory.
;
;	The read linked list is 16 frames in length
;	The write linked list is 9 frames in length
;
RVB_InitialiseDmaCommandBlocks

        ;==================================================
        ; Fill the following elements of the DMA command block
        ;	'controlBits'
        ;	'Count' 
        ;	'Offset
        ;	'Base Address'
        ;	'SizeMinusOne'
        ;==================================================

	move	rD,a
	add	#DMA_ControlBlock_Wr0-StaticWorkspaceBase,a
	move	a,r0						; r0 points to the start of the first write control block


	move	#DMAcbs_SizeofDMAcbs,n0				; n0 = Size of DMA command block
	move	#0,a0
	move	#SizeOfFrame,a1					; a0=0  a1=SizeOfFrame

	move	#(DMAcbs_DspToHost+DMAcbs_InterleaveOFF+DMAcbs_IOCOFF+DMAcbs_smOffWrBkON+DMAcbs_smBufIdCircular+DMAcbs_isoON+DMAcbs_smDataFmt2+DMAcbs_inc1),x0
	move	#>14,y0						; Fill 15 DMA write blocks
	bsr	RVB_FillDmaCommandBlocks
	
	move	#(DMAcbs_HostToDsp+DMAcbs_InterleaveOFF+DMAcbs_IOCOFF+DMAcbs_smOffWrBkON+DMAcbs_smBufIdCircular+DMAcbs_isoON+DMAcbs_smDataFmt2+DMAcbs_inc1),x0
	move	#>31,y0						; Fill 31 DMA read blocks
	bsr	RVB_FillDmaCommandBlocks



        ;======================================
        ; Fill the DSP start address entries
        ; in the Write DMA command blocks
        ;======================================
        ; There are 14 write blocks

	move	#-1,m0
	move	#DMAcbs_SizeofDMAcbs,n0

	move	rD,a
	add	#(DMA_ControlBlock_Wr0-StaticWorkspaceBase+DMAcbs_pDspStart),a
	move	a,r0
	
                                                                ; The first DMA command block contains
	move	x:(rD+pBuffer0-StaticWorkspaceBase),a	        ; a pointer to buffer 0
	move	a,x:(r0)+n0

        ; The next 3 DMA write command blocks refer to
        ; the separate components of the main delay
        ; buffer used in the long reverb. Data for
        ; these buffer is contained in buffers 14,16 and 18.

	move	x:(rD+pBuffer14-StaticWorkspaceBase),a
	dor	#3,RVB_udcb_Wr_MainDelayBuffersLoop
		move	a,x:(r0)+n0
		add	#2*SizeOfFrame,a
RVB_udcb_Wr_MainDelayBuffersLoop

        ; The next 4 DMA write command blocks are the
        ; reflection delay buffers.  Data is contained
        ; in buffer 1,2,3 and 4
        ; The next 4 DMA write command blocks are the
        ; short reverb delay buffers.  Data is contained
        ; in buffer 5,6,7 and 8

	move	x:(rD+pBuffer1-StaticWorkspaceBase),a
	dor	#8,RVB_udcb_Wr_ReflectionsBuffersLoop
		move	a,x:(r0)+n0
		add	#SizeOfFrame,a
RVB_udcb_Wr_ReflectionsBuffersLoop
													
        ; The next 2 DMA write command blocks are the
        ; long reverb delay buffers.  Data is contained
        ; in buffer 9 and 10

	move	x:(rD+pBuffer9-StaticWorkspaceBase),a
	dor	#2,RVB_udcb_Wr_LongReverbBuffersLoop
		move	a,x:(r0)+n0
		add	#SizeOfFrame,a
RVB_udcb_Wr_LongReverbBuffersLoop


	;======================================
	; Fill the DSP start address entries
	; in the Read DMA command blocks
	;======================================
	; There are 16 read blocks

	move	rD,a
	add	#(DMA_ControlBlock_Rd0-StaticWorkspaceBase+DMAcbs_pDspStart),a
	move	a,r0
	move	#DMAcbs_SizeofDMAcbs,n0
	
	; DMA blocks 1 through to 16 map to 
	; directly to buffers 1 through 16

	move	x:(rD+pBuffer0-StaticWorkspaceBase),a
	dor	#31,RVB_udcb_Rd_BuffersLoop
		move	a,x:(r0)+n0
		add	#SizeOfFrame,a
RVB_udcb_Rd_BuffersLoop


	;================================================
	; Fill in the information about the host memory 
	; pointers & delay buffers
	;================================================
	;----------------------------
	; Fill the Read DMA blocks
	;----------------------------

	
	move	rD,a						; Get a pointer to the start of the
	add	#(DMA_ControlBlock_Rd0-StaticWorkspaceBase),a	; block of DMA read blocks
	move	a,r0

	move	rD,a	                                        ; Get a pointer to the start of the new
	add	#(HostBufferInfo-StaticWorkspaceBase),a		; parameters stored in the scratch which have
	move	a,r1						; been updated by the host.
								; r1 points to start of Host buffer information block


	; Read blocks 0-12

       	; The first 13 read blocks are the reflection taps
       	; and short reverb taps. These all access the main 
       	; delay line. Therefore they all contain the same
       	; host pointer and length

	move	x:(r1),x0					; Get the host address of the main delay buffer
	move	x:(r1+1),x1					; Get the length of the main delay -1
													;
	dor	#13,RVB_udcb_HostBaseAddressLoop
		move	x0,x:(r0+DMAcbs_smBaseAddress)		; Store host base address of main delay buffer
		move	x1,x:(r0+DMAcbs_smSizeMinus1)		; Store length of main delay buffer

		move	x:(r0)+n0,a1				; Point to the next DMA block
RVB_udcb_HostBaseAddressLoop


        ; Read blocks 13-20
        ; The next 8 read blocks are the long reverb delay taps
        ; Delay taps are found in pairs with the first on the
        ; main delay line, and the next pairs successive
        ; buffers described in the host transfer block.

        ;
	dor	#4,RVB_udcb_HostBaseAddressLoop1
		move	x:(r1)+,x0				; Get the host address of the main delay buffer
		move	x:(r1)+,x1				; Get the length of the main delay -1
		dor	#2,RVB_udcb_HostBaseAddressLoop2
			move	x0,x:(r0+DMAcbs_smBaseAddress)	; Store host base address of main delay buffer
			move	x1,x:(r0+DMAcbs_smSizeMinus1)	; Store length of main delay buffer
			move	x:(r0)+n0,a1			; Point to the next DMA block
RVB_udcb_HostBaseAddressLoop2
		nop
RVB_udcb_HostBaseAddressLoop1



        ; Read blocks 21-30
        ; Information for the next 12 DMA blocks are 
        ; copied directly from the host information
        ; block which is configured by the host

	dor	#10,RVB_udcb_HostBaseAddressLoop3
		move	x:(r1)+,x0
		move	x0,x:(r0+DMAcbs_smBaseAddress)		; into the DMA command block
		move	x:(r1)+,x1
		move	x1,x:(r0+DMAcbs_smSizeMinus1)		; into the DMA command block
		move	x:(r0)+n0,a1				; Point to the next DMA block
RVB_udcb_HostBaseAddressLoop3


        ;----------------------------
        ; Fill the Write DMA blocks
        ;----------------------------
        ; There are 14 write DMA blocks and these map
        ; to each of the 14 buffer information blocks
        ; in the host information block.

	move	rD,a
	add	#(DMA_ControlBlock_Wr0-StaticWorkspaceBase),a
	move	a,r0

        move	rD,a
	add	#(HostBufferInfo-StaticWorkspaceBase),a
	move	a,r1						; r1 points to start of Host buffer information block

	dor	#14,RVB_udcb_HostBaseAddresLoop2
		move	x:(r1)+,x0
		move	x0,x:(r0+DMAcbs_smBaseAddress)		; Write the buffer base address
		move	x:(r1)+,x1
		move	x1,x:(r0+DMAcbs_smSizeMinus1)		; Write the buffer size
		move	x:(r0)+n0,a1				; Point to the next DMA block
RVB_udcb_HostBaseAddresLoop2


        ;--------------------------------------------
        ; Set up the correct read delay taps
        ;--------------------------------------------

	bsr	RVB_UpdateAllDmaCommandBlocks

	rts




;---------------------------------
;	RVB_FillDmaCommandBlocks
;---------------------------------
;	On entry
;		x0	The control bits
;		a0	Zero
;		a1	The size of the frame
;		y0	The number of DMA blocks to be written
;		rD	Pointer to reverb engine workspace
;
;	On exit
;		x0,a0,a1,y0 preserved
;		x1 Undefined
;
;	Fill the specified number of DMA command blocks with data.
;		1. Create the linked list structure
;		2. Fill the control bits
;		3. Fill the count value
;		4. Set the offset into the buffer to zero
;
RVB_FillDmaCommandBlocks
	move r0,r1					        ; Within the loop r1 takes the 
								; value of the previous DMA block.

	dor	y0,RVB_udcb_FillLoop
		move	r0,x:(r1+DMAcbs_pNxtCmd)		; Store the address of the current DMA block
								; in the 'next' pointer of the previous DMA block
		move	x0,x:(r0+DMAcbs_controlBits)
		move	a1,x:(r0+DMAcbs_count)
		move	a0,x:(r0+DMAcbs_smOffset)
		move	r0,r1					; Record the address of the current DMA block
		move	x:(r0)+n0,x1
RVB_udcb_FillLoop

	move	#DMAcbs_eol,x0					; Terminate the linked list with the 
	move	x0,x:(r1)					; end of list identifier
	rts
	

;----------------------------------
;		RVB_UpdateAllDmaCommandBlocks
;----------------------------------
;	On Entry
;		rD	Pointer to reverb private workspace
;
;	On Exit
;		a	Undefined
;		x	Undefined
;		y	Undefined
;		r0-r4	Undefined
;		n0,n2

;		Private data updated to reflect the new delay constants
;
;
; This function updates the parameters in the linked list of DMA command blocks.
;
; It changes the tap position on a delay line by repositioning the read taps 
; with respect to a corresponsing write tap. This ensures that the mininum 
; audible discontinuitiy occurs.
; 
; The position of the new read tap is calculated as follows:
;	1.	Find the position of the specified host write pointer
;	2.	Get the new delay tap position
;	3.	Calculate the delay tap position relative to the current write position.
;		(Take in to account that the buffer is a cyclic buffer).
;	4.	Write the new read tap position for the specified buffer.
; This procedure ensures that the data going in to the delay buffer is always continuous.
;

RVB_UpdateAllDmaCommandBlocks

        ; Wait until the current DMA transfer has
        ; completed. This avoids potential problems
        ; with the DMA controller updating the buffer
        ; indices at the same time as us.


	bsr	RVB_UpdateDmaCommandBlocksProcessedByFader	; Update the DMA blocks that are processed
						    		; by the 'delay tap' fader.
															

	bsr    	RVB_UpdateDmaCommandBlocksNotProcessedByFader	; Update the other DMA blocks in the system
	rts



;=================================
;	RVB_UpdateDmaCommandBlocksProcessedByFader
;=================================
;	On Entry
;		rD	Pointer to reverb private workspace
;
;	On Exit
;		r0	Undefined
;		r1	Undefined
;		r2	Undefined
;		a	Undefined
;		b	Undefined
;		x0	Undefined
;		x1	Undefined
;		y0	Undefined
;		y1	Undefined
;		n0	Undefined
;		n1	Undefined
;		DMA blocks are updated
;
;	Update the delays on the main delay line
;
RVB_UpdateDmaCommandBlocksProcessedByFader

								; Update the positions of the 5 read taps
								; corresponding to the reflections
	
	move	rD,a						; The positions of the read taps
	add 	#ReflectionsInputDelayList-StaticWorkspaceBase,a ; are stored in 
	move	a,r2						; 'ReflectionsInputDelayList'


	;move	rD,a						; Delays are relative to the
	;add 	#DMA_ControlBlock_Wr0-StaticWorkspaceBase,a	; main delay buffer write block
	add 	#DMA_ControlBlock_Wr0-ReflectionsInputDelayList,a
	move	a,r0						 

	;move	rD,a						; We are updating the 5 read
	;add	#DMA_ControlBlock_Rd0-StaticWorkspaceBase,a	; blocks beginning with
	add    	#DMA_ControlBlock_Rd0-DMA_ControlBlock_Wr0,a
	move	a,r1						; 'DMA_ControlBlock_Rd0'
	
	move	#0,y0						; Reflection delays are absolute

	move	#>5,y1						; 5 reflections to update
	bsr    	RVB_UpdateDelaysOnMainDelayLine



								; Update the positions of the 8 read taps
								; corresponding to the short reverbs

	move	rD,a							; The positions of the read taps
	add    	#ShortReverbInputDelayList-StaticWorkspaceBase,a	; are spaced according to the
	move	a,r2							; delays in 'ShortReverbInputDelayList'


	move	rD,a							; The delays are relative to the
	add 	#DMA_ControlBlock_Wr0-StaticWorkspaceBase,a		; main delay buffer write block
	;add 	#DMA_ControlBlock_Wr0-ShortReverbInputDelayList,a
	move	a,r0								 

	;move	rD,a							; We are updating the 8 read
	;add		#DMA_ControlBlock_Rd5-StaticWorkspaceBase,a	; blocks beginning with
	add	#DMA_ControlBlock_Rd5-DMA_ControlBlock_Wr0,a
	move	a,r1							; 'DMA_ControlBlock_Rd5'

	
	move	x:(rD+ShortReverbInputDelay-StaticWorkspaceBase),y0	; Short reverb delays are relative
									; to 'ShortReverbInputDelay'

	move	#>8,y1							; 8 short reverb delays to update
	bsr	RVB_UpdateDelaysOnMainDelayLine



									;=================================
									;	Update the reflections feedback delays
									;=================================

	move	rD,a							; The tap positions for the reflections
	add	#ReflectionsDelayList-StaticWorkspaceBase,a		; delay buffers are stored in the block 
	move	a,r2							; starting at 'ReflectionsDelayList'.


	;move	rD,a							; The reflection delays are relative
	;add		#DMA_ControlBlock_Wr4-StaticWorkspaceBase,a	; to DMA_ControlBlock_Wr4,5,6 and 7
	add	#DMA_ControlBlock_Wr4-ReflectionsDelayList,a
	move	a,r0

	;move	rD,a							; Update the read DMA blocks
	;add		#DMA_ControlBlock_Rd21-StaticWorkspaceBase,a	; DMA_ControlBlock_Rd21,22,23 and 24
	add	#DMA_ControlBlock_Rd21-DMA_ControlBlock_Wr4,a
	move	a,r1
	
	
	move	#DMAcbs_SizeofDMAcbs,n0
	move	#DMAcbs_SizeofDMAcbs,n1
	dor	#4,RVB_FillReflectionDelaysLoop
		move	x:(r2)+,x1
		bsr	RVB_UpdateDmaCommandBlockOffset
		move	x:(r0)+n0,x0					; Increment r0
		move	x:(r1)+n1,x0					; Increment r1
RVB_FillReflectionDelaysLoop




									;=================================
									;	Update the long reverb feedback delays
									;=================================

	move	rD,a							; The long reverb delays are relative 
	add	#DMA_ControlBlock_Wr12-StaticWorkspaceBase,a		; to DMA_ControlBlock_Wr12 and 13
	move	a,r0

	;move	rD,a							; Update the read blocks 29 and 30
	;add		#DMA_ControlBlock_Rd29-StaticWorkspaceBase,a
	add	#DMA_ControlBlock_Rd29-DMA_ControlBlock_Wr12,a
	move	a,r1
	
	move	#DMAcbs_SizeofDMAcbs,n0
	move	#DMAcbs_SizeofDMAcbs,n1
	dor	#2,RVB_FillLongReverbDelaysLoop
		move	x:(rD+LongReverbDelay-StaticWorkspaceBase),x1	; Get LongReverb[i] delay (The same for both channels)
		bsr	RVB_UpdateDmaCommandBlockOffset
		move	x:(r0)+n0,x0					; Increment r0 (Ignore result of move)
		move	x:(r1)+n1,x0					; Increment r1 (Ignore result of move)
RVB_FillLongReverbDelaysLoop


	rts




;---------------------------------------------
;	RVB_UpdateDmaCommandBlocksNotProcessedByFader
;---------------------------------------------
;	Update the DMA command blocks that are not processed by the delay tap fader
;
RVB_UpdateDmaCommandBlocksNotProcessedByFader

									;=================================
									;	Update the long reverb delays
									;=================================

									; Update the positions of the 8 read taps
									; corresponding to the long reverbs

	move	rD,a							; The positions of the read taps
	add	#LongReverbInputDelay0a-StaticWorkspaceBase,a		; are defined by the variable block
	move	a,r2							; beginning at 'LongReverbInputDelay0a'


	;move	rD,a							; The delays are relative to the
	;add 	#DMA_ControlBlock_Wr0-StaticWorkspaceBase,a		; main delay buffer write block
	add 	#DMA_ControlBlock_Wr0-LongReverbInputDelay0a,a
	move	a,r0

	;move	rD,a							; We are updating the 8 read
	;add		#DMA_ControlBlock_Rd13-StaticWorkspaceBase,a	; blocks beginning with
	add	#DMA_ControlBlock_Rd13-DMA_ControlBlock_Wr0,a
	move	a,r1							; 'DMA_ControlBlock_Rd13'
	
	move	#DMAcbs_SizeofDMAcbs,n0
	dor	#4,RVB_UpdateLongDelaysLoop
		move	#0,y0						; Delays for the long reverb are absoute

		move	#>2,y1						; 4 lots of 2 long reverb delays to update
		bsr	RVB_UpdateDelaysOnMainDelayLine

		move	x:(r0)+n0,x0					; Point to the next DMA write block
																		; (Ignore result of load)
RVB_UpdateLongDelaysLoop




        ;=================================
        ;	Update the short reverb feedback delays
        ;=================================

	move	rD,a						     	; The short reverb delays are relative
	add	#DMA_ControlBlock_Wr8-StaticWorkspaceBase,a		; to DMA_ControlBlock_Wr8,9,10,11
	move	a,r0

	;move	rD,a						     	; Update the read DMA blocks
	;add		#DMA_ControlBlock_Rd25-StaticWorkspaceBase,a	; DMA_ControlBlock_Rd25,26,27 and 28
	add	#DMA_ControlBlock_Rd25-DMA_ControlBlock_Wr8,a
	move	a,r1
	
	move	rD,a						     	; The delay positions are stored in
	add	#ShortReverbDelayList-StaticWorkspaceBase,a    	        ; the constant array starting at
	;add	#ShortReverbDelayList-DMA_ControlBlock_Rd25,a
	move	a,r2						     	; 'ShortReverbDelayList'

	move	#DMAcbs_SizeofDMAcbs,n0
	move	#DMAcbs_SizeofDMAcbs,n1
	dor	#4,RVB_FillShortReverbDelaysLoop
		move	x:(r2)+,x1				     	; Get Reverb[i] delay
		bsr    	RVB_UpdateDmaCommandBlockOffset
		move	x:(r0)+n0,x0				     	; Increment r0 (Ignore result of move)
		move	x:(r1)+n1,x0				     	; Increment r1 (Ignore result of move)
RVB_FillShortReverbDelaysLoop




	rts




;------------------------------------------------------------
;		RVB_UpdateDelaysOnMainDelayLine
;------------------------------------------------------------
;	On entry
;		r0	Points to the reference write control block
;		r1	Points to first read control block to update
;		r2	Points to a list of relative delays
;		y0	Added to (r2) to give the total relative delay
;		y1	The number of continuous DMA blocks to update
;	On exit
;		a	Undefined
;		b	Undefined
;		n1	Undefined
;		x0	Undefined
;		x1	Undefined
;		r0	Preserved
;		r1	Incremented by x0*DMAcbs_SizeofDMAcbs
;		r2	Incremented by x0
;
;	Given a pointer to the DMA write blocks and an array of
;	delays, update the specified DMA read blocks 
;
RVB_UpdateDelaysOnMainDelayLine

	move	#DMAcbs_SizeofDMAcbs,n1

	dor	y1,RVB_UpdateMainBufferDelaysLoop
		move	x:(r2)+,a				; Get the delay offset corresponding to this buffer.
								; The delay offset is relative to y0

								; Add the relative offset to y0
		add    	y0,a
		move	a,x1					; x1=offset of read pointer relative to the write pointer
								; r0=pointer to the (reference) DMA write block
								; r1=pointer to the DMA read block to be updated
		bsr    	RVB_UpdateDmaCommandBlockOffset

		move	x:(r1)+n1,x0			        ; Increment r1 to point to the 
								; next read pointer
RVB_UpdateMainBufferDelaysLoop
	rts




;---------------------------------------------
;		RVB_UpdateDmaCommandBlockOffset
;---------------------------------------------
;	On entry
;		r0 	Points to base of a DMA block write pointer
;		r1 	Points to base of the DMA block read pointer to be modified
;		x1 	The delay of the new cyclic buffer (measured in samples).
;			This is actually the position of the read pointer relative 
;			to the appropriate write pointer.
;			Delays should all be positive and smaller than the size of
;			the buffer.
;		rD	Pointer to reverb engine workspace
;
;	On exit
;		r0	Preserved
;		r1	Preserved
;		x0	Undefined
;		b	Undefined
;		The DMA command block pointed to by R1 has been updated.
;
RVB_UpdateDmaCommandBlockOffset

	move	x:(r0+DMAcbs_smOffset),b			; Get the reverb write index
	
	sub	x1,b						; Calculate position of read index
	and	#$FFFFF0,b					; Make sure that the index is a multiple of 4*4=16
RVB_CheckAddressInRange
	cmp	#0,b						; If the new read index is -ve then
	bge	RVB_AddressInRange				; add the length of the buffer
	move	x:(r0+DMAcbs_smSizeMinus1),x0			; to the index.  This will ensure
	add	x0,b						; that is within the correct range
	add	#1,b
	bra RVB_CheckAddressInRange
RVB_AddressInRange
	move	b,x:(r1+DMAcbs_smOffset)			; Store the new read offset
	rts




RVB_EndOfFile



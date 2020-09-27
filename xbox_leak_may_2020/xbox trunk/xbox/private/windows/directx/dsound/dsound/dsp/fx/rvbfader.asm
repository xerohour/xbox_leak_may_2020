;	File	:	Reverb.asm
;	Version	:	1.01
;	Date	:	18/09/2000
;
;	Author	:	RDA
;				Sensaura (c)
;
;
; Description
; ===========
; The fader for the reverb engine.
;
; The fader for the reverb engine provides a gradual change in reverb 
; parameters settings to avoid audible clicks.  It does this by changing
; the positions of the delay taps one-by-one.  To further smooth the 
; transition, the buffer corresponding the the delay tap that is being
; repositioned is faded to zero then back to one over a total of 512
; samples.  When the buffer is faded to zero it is moved.  Fading
;
; The amplitudes of the output buffers are also faded from current values
; to target values
;
; Buffers that are
; Version history
; ===============
;	0.00		Development version
;	1.00		First working version
;	1.01		Guarantees that the maximum amplitude swing can be
;				achieved exactly.		
;


;
;	This file implements addition functionality to the reverb engine
;	giving it the ability to fade between new parameters sets.
;	
;	The variable 'FADER_ENABLED' (which can be found in 'reverb.asm')
;	can be set to zero to prevent the fader from being compiled into 
;	the project.  'FADER_ENABLED' should be set to 1 to enable the
;	fader.
;
;
;	The following buffers are faded:
;		The five reflections buffers reading from the main delay line
;		The eight short reveb buffer reading from the main delay line
;		The four reflection feedback buffers
;		The two long reveb buffer feedback buffers



	msg		'Reverb fader enabled'



;=========================================
;	Buffer Muting Constants
;=========================================

;	There are a total of 21 taps that are repositioned with the
;	fader. At the full sample rate each one takes a total of 
;	(256+256)=512 samples.  This means that it takes a total of 
;	512*21/48000=0.224 seconds to change between parameters sets.
;
;
;	'FADER_BUFFERMUTEDURATION' controls the number of samples over which
;	buffers are faded in and out while their delay taps are being 
;	repositioned
;	'FADER_STEPSIZE_FADEOUT' controls the size of the amplitude steps
;	for these buffers are they are being muted

	IF @DEF('HALFSAMPLERATE')
FADER_BUFFERMUTEDURATION	equ	128		;	Amplitudes are faded out over 128 samples
;FADER_BUFFERMUTEDURATION	equ	512		;	Amplitudes are faded out over 512 samples
	ELSE
FADER_BUFFERMUTEDURATION	equ	256		;	Amplitudes are faded out over 256 samples
;FADER_BUFFERMUTEDURATION	equ	1024	;	Amplitudes are faded out over 1024 samples
	ENDIF

FADER_STEPSIZE_FADEIN		equ 1.0/FADER_BUFFERMUTEDURATION
FADER_STEPSIZE_FADEOUT		equ -1.0/FADER_BUFFERMUTEDURATION



;=========================================
;	Output Amplitude Control Constants
;=========================================


; Amplitudes are faded by applying what is in essence an IIR filter to the amplitude
; value.  It causes the current amplitude to fade towards the targe amplitude
; exponentially.

; The system fades from one parameter set to another in 21*512 samples.
; The amplitude fader alters the amplitudes on a frame-by-frame basis. The
; amplitude fader therefore has
;		(21*512)/32 = 336 frames @ full sample rate and
;		(21*256)/16 = 336 frames @ half sample rate
; to fade the amplitude.
; 
; After this number of frames have been processed the output amplitude
; is set equal to the target amplitude in a final step.  In order to not
; perceive this final step, the output amplitude must be able to decay
; from 2^23 down to 512 over 336 frames.  This criterion defined the
; value of the decay rate for the output amplitude fader
; 
;		FaderAmplitudeDecayRate=1 - 1/#Frames * ln( 512/2^23 )
;		                       =1 - 1/336*ln(2^-14)

FaderAmplitudeDecayRate		equ 0.971
;FaderAmplitudeDecayRate		equ 0.993






	org	p:




;==============================
;		RVB_Fader_Initialise
;==============================
;	On entry
;		r5	Pointer to reverb engine workspace
;
;	On exit
;
;	Variables associated with the fader have been initialise
;
RVB_Fader_Initialise
	bsr		RVB_Fader_StopFading
	rts



;==============================
;		RVB_Fader_StartFading
;==============================
;	On entry
;		r5	Pointer to reverb engine workspace
;
;	On exit
;		a	Undefined
;
;	Begin the processing of fading from one set of delay taps to another
;
RVB_Fader_StartFading
	move	#0,a												; Initialise the amplitude fader
	move	a,x:(r5+RVB_FaderAmplitudeState-StaticWorkspaceBase)

																; Start the fading process at the 
	move	a,x:(r5+RVB_FaderStep-StaticWorkspaceBase)			; beginning of a buffer

	move	#>1,a												; Enable fading and begin fading
	move	a,x:(r5+RVB_FaderState-StaticWorkspaceBase)			; buffer 0


	rts



;==============================
;		RVB_Fader_StopFading
;==============================
;	On entry
;		r5	Pointer to reverb engine workspace
;
;	On exit
;		a	Undefined
;
;	Stop fading.
; This will stop the fader immediately.  Any buffer that is in the process
; of being faded will revert to full volume with whatever tap position is
; currently defined. (This may be the original or the final depending on when
; the function is called.)

RVB_Fader_StopFading
	move	#-1,a												; Disable the amplitude fader
	move	a,x:(r5+RVB_FaderAmplitudeState-StaticWorkspaceBase)

	move	#0,a												; Start the fading process at the 
	move	a,x:(r5+RVB_FaderStep-StaticWorkspaceBase)			; beginning of a buffer

	move	a,x:(r5+RVB_FaderState-StaticWorkspaceBase)			; Disable fading

	rts





;==============================
;		RVB_Fader_Process
;==============================
;	On entry
;		r5	Pointer to reverb engine workspace
;
;	On exit
;		a	Undefined
;		b	Undefined
;		x	Undefined
;		y	Undefined
;		r0	Undefined
;		r1	Undefined
;		r5	Preserved
;
;	This function should be called every frame immediately after the DMA controller
;	has filled the reverb's frame buffers.  If the fader not active, it exits
;	immediately.
;
RVB_Fader_Process

	IF FADER_ENABLE_OUTPUTAMPITUDEFADE

	msg		"Performing output amplitude fading"
	bsr		RVB_Fader_FadeOutputAmplitudes						; Fade the output amplitude between
																; the current values and the new
																; values
	ENDIF

	IF FADER_ENABLE_DELAYTAPFADE

	msg		"Performing delay tap fading"
	bsr		RVB_Fader_FadeDelayTaps								; Shuffle the delay tap positions
																; after ramping down the corresponsing
																; amplitude buffer
	ENDIF

	rts



RVB_Fader_FadeDelayTaps
																;===============================
																; Check to see if the fader
																; is active
																;===============================
	move	x:(r5+RVB_FaderState-StaticWorkspaceBase),a
	cmp		#0,a
	beq		RVB_Fader_Exit										; If inactive then exit


	


																;===============================
																; Check to see if the fader is
																; in a 'special' state
																;===============================
	move	x:(r5+RVB_FaderStep-StaticWorkspaceBase),a
	cmp		#0,a												; Check to see if we are starting
	beq		RVB_Fader_FadeNewBuffer								; to fade a new buffer
	
	cmp		#FADER_BUFFERMUTEDURATION,a												; Check to see if we are ready
	beq		RVB_Fader_MoveDelayTap								; to move the delay tap

	cmp		#2*FADER_BUFFERMUTEDURATION,a						; Check to see if we have finished
	beq		RVB_Fader_FinishedFading							; fading this buffer





RVB_Fader_FadeFrame
																;=========================================
																; Apply linear fade to the current buffer
																;=========================================
	move	x:(r5+RVB_FaderpDMAread-StaticWorkspaceBase),r0		; Get a pointer to the 
	move	x:(r0+DMAcbs_pDspStart),r0							; buffer we are fading

	move	x:(r5+RVB_FaderGain-StaticWorkspaceBase),b			; Get the current gain value
	move	x:(r5+RVB_FaderStepSize-StaticWorkspaceBase),y0		; Get the size of each step in the gain

	dor		#SizeOfFrame,RVB_Fader_LinearScaleFrameLoop
		move	b,x1											; 
		move	x:(r0),x0										; Get a data sample
		mpy		x0,x1,a											; Scale the sample by the gain
		move	a,x:(r0)+										; Store the result
		
		add		y0,b											; Update the gain
RVB_Fader_LinearScaleFrameLoop

	move	b,x:(r5+RVB_FaderGain-StaticWorkspaceBase)			; Store the last gain value

	move	x:(r5+RVB_FaderStep-StaticWorkspaceBase),a
	add		#SizeOfFrame,a
	move	a,x:(r5+RVB_FaderStep-StaticWorkspaceBase)
	
	

RVB_Fader_Exit
	rts





;===============================
; RVB_Fader_FadeNewBuffer
;===============================
;	We are about to begin fading a new buffer. Configure the fader
;	parameters accordingly
RVB_Fader_FadeNewBuffer

	move	x:(r5+RVB_FaderState-StaticWorkspaceBase),x0
	brset	#0,x0,RVB_Fader_ConfigureReflections				; Process the first reflections buffer
																; (There are 5 reflection buffers)

	brset	#5,x0,RVB_Fader_ConfigureShortReverb				; Process the first short reveb buffer
																; (There are 8 short reverb buffers)

	brset	#13,x0,RVB_Fader_ConfigureReflectionDelayLines		; Process the first reflection feedback buffers
																; (There are 4 reflection feedback buffers)

	brset	#17,x0,RVB_Fader_ConfigureLongReverbfeedback		; Process the first long reveb buffer
																; (There are 2 long reverb feedback buffers)

	brset	#19,x0,RVB_Fader_DoneAllBuffers						; The fader has repositioned all the delay taps


																; Update the pointer to the
																; DMA read control block
	move	x:(r5+RVB_FaderpDMAread-StaticWorkspaceBase),a
	add		#DMAcbs_SizeofDMAcbs,a
	move	a,x:(r5+RVB_FaderpDMAread-StaticWorkspaceBase)

																; Update the pointer to the DMA
																; write block
	move	x:(r5+RVB_FaderpDMAwrite-StaticWorkspaceBase),a
	move	x:(r5+RVB_FaderDMAwriteIncrement-StaticWorkspaceBase),x0
	add		x0,a
	move	a,x:(r5+RVB_FaderpDMAwrite-StaticWorkspaceBase)
	
	bra		RVB_Fader_Configure




;--------------------------------------------
;	Configure the fader for reflections taps
;--------------------------------------------
;	Initialise the following
;		1. A pointer to the read DMA block
;		2. A pointer to the write DMA block
;		3. The list of new delay tap positions measured 
;			relative to the write DMA block
RVB_Fader_ConfigureReflections
	move	r5,a												; Set the fader tap list pointer
	add		#ReflectionsInputDelayList-StaticWorkspaceBase,a	; equal to the updated list
																; of reflection delay tap positions
	move	a,x:(r5+RVB_FaderpNewTapList-StaticWorkspaceBase)	;


	;move	r5,a												; The DMA write block from which delays
	;add 	#DMA_ControlBlock_Wr0-StaticWorkspaceBase,a			; are measured is 'DMA_ControlBlock_Wr0'
	add		#DMA_ControlBlock_Wr0-ReflectionsInputDelayList,a
	move	a,x:(r5+RVB_FaderpDMAwrite-StaticWorkspaceBase)		; 

	;move	r5,a												; The first DMA read block to process
	;add 	#DMA_ControlBlock_Rd0-StaticWorkspaceBase,a			; for the first reflection is 'DMA_ControlBlock_Rd0'
	add		#DMA_ControlBlock_Rd0-DMA_ControlBlock_Wr0,a
	move	a,x:(r5+RVB_FaderpDMAread-StaticWorkspaceBase)		; 


	bra		RVB_Fader_Configure_SetDMAWriteIncrementZero



;--------------------------------------------
;	Configure the fader for short reverb taps
;--------------------------------------------

;	Initialise the following
;		1. A pointer to the read DMA block
;		2. A pointer to the write DMA block
;		3. The list of new delay tap positions measured 
;			relative to the write DMA block
RVB_Fader_ConfigureShortReverb
	move	r5,a												; The DMA write block from which 
	add		#DMA_ControlBlock_Wr0-StaticWorkspaceBase,a			; delay are measured
	move	a,x:(r5+RVB_FaderpDMAwrite-StaticWorkspaceBase)		; is 'DMA_ControlBlock_Wr0'


	;move	r5,a												; The first DMA read block to process
	;add	#DMA_ControlBlock_Rd5-StaticWorkspaceBase,a			; for the first short reverb is 'DMA_ControlBlock_Rd5'
	add		#DMA_ControlBlock_Rd5-DMA_ControlBlock_Wr0,a
	move	a,x:(r5+RVB_FaderpDMAread-StaticWorkspaceBase)		; 


	
	
	;move	r5,a												; Get the address of the list of
	;add	#ShortReverbInputDelayList-StaticWorkspaceBase,a	; input reverb tap offsets
	add		#ShortReverbInputDelayList-DMA_ControlBlock_Rd5,a
	move	a,r0												; (all measured wrt 'ShortReverbInputDelay')

	move	x:(r5+ShortReverbInputDelay-StaticWorkspaceBase),x0	; Read the 'ShortReverbInputDelay'


	;move	r5,a												; Get a pointer to the start of a
	;add	#RVB_FaderNewTapList-StaticWorkspaceBase,a			; block that we will fill with the
																; positions of the new delay taps
	add		#RVB_FaderNewTapList-ShortReverbInputDelayList,a
	move	a,r1												; 
	move	a,x:(r5+RVB_FaderpNewTapList-StaticWorkspaceBase)	; Store a pointer to that block.


	dor		#8,RVB_Fader_ConfigureShortReverbLoop				; Fill the block with the new tap positions
		move	x:(r0)+,a
		add		x0,a
		move	a,x:(r1)+
RVB_Fader_ConfigureShortReverbLoop

																; Perform the common configuration
																; functions for a reverb tap.
	bra		RVB_Fader_Configure_SetDMAWriteIncrementZero


;--------------------------------------------
;	Configure the fader for long reverb feedback taps
;--------------------------------------------
RVB_Fader_ConfigureLongReverbfeedback
	move	r5,a												; The DMA write block from which 
	add		#DMA_ControlBlock_Wr12-StaticWorkspaceBase,a			; delay are measured
	move	a,x:(r5+RVB_FaderpDMAwrite-StaticWorkspaceBase)		; is 'DMA_ControlBlock_Wr12'


	;move	r5,a												; The first DMA read block to process 
	;add 	#DMA_ControlBlock_Rd29-StaticWorkspaceBase,a		; for the reflection is 'DMA_ControlBlock_Rd29'
	add 	#DMA_ControlBlock_Rd29-DMA_ControlBlock_Wr12,a
	move	a,x:(r5+RVB_FaderpDMAread-StaticWorkspaceBase)


	;move	r5,a												; Get the address of the list of
	;add 	#RVB_FaderNewTapList-StaticWorkspaceBase,a			; input reverb tap offsets
	add 	#RVB_FaderNewTapList-DMA_ControlBlock_Rd29,a
	move	a,r0

	move	a,x:(r5+RVB_FaderpNewTapList-StaticWorkspaceBase)	; Store a pointer to that block.

	move	x:(r5+LongReverbDelay-StaticWorkspaceBase),a		; All the delay times are the same,
	move	a,x:(r0)+											; so fill the buffer with the 
	move	a,x:(r0) 											; value of 'LongReverbDelay'

																; The delays for the late reverb
																; refer to DMA_ControlBlock_Wr5,6,7,8
	move	#>DMAcbs_SizeofDMAcbs,a
																; Perform the common configuration
	bra		RVB_Fader_Configure_SetDMAWriteIncrement			; functions for a reverb tap.	
	



;--------------------------------------------
;	Configure the fader for reflection delay taps
;--------------------------------------------

;	Initialise the following
;		1. A pointer to the read DMA block
;		2. A pointer to the write DMA block
;		3. The list of new delay tap positions measured 
;			relative to the write DMA block
RVB_Fader_ConfigureReflectionDelayLines
	move	r5,a												; Get the address of the list of
	add 	#ReflectionsDelayList-StaticWorkspaceBase,a			; reflection tap offsets
	move	a,x:(r5+RVB_FaderpNewTapList-StaticWorkspaceBase)	; Store a pointer to that block.


	;move	r5,a												; The DMA write block from which delay
	;add 	#DMA_ControlBlock_Wr4-StaticWorkspaceBase,a			; are measured is 'DMA_ControlBlock_Wr4'
	add		#DMA_ControlBlock_Wr4-ReflectionsDelayList,a
	move	a,x:(r5+RVB_FaderpDMAwrite-StaticWorkspaceBase)		; 


	;move	r5,a												; The first DMA read block to process
	;add 	#DMA_ControlBlock_Rd21-StaticWorkspaceBase,a		; for the reflection is 'DMA_ControlBlock_Rd21'
	add 	#DMA_ControlBlock_Rd21-DMA_ControlBlock_Wr4,a
	move	a,x:(r5+RVB_FaderpDMAread-StaticWorkspaceBase)		; 
	
	

																; The delays for the late reverb
																; refer to DMA_ControlBlock_Wr5,6,7,8
	move	#>DMAcbs_SizeofDMAcbs,a
																; Perform the common configuration
	bra		RVB_Fader_Configure_SetDMAWriteIncrement			; functions for a reverb tap.
	
	
	
	



;--------------------------------------------
;	Common configuration for all taps
;--------------------------------------------
;----------------------------
;	RVB_Fader_Configure_SetDMAWriteIncrementZero
;----------------------------	
; On entry
;	No entry parameters
;
RVB_Fader_Configure_SetDMAWriteIncrementZero
	clr		a													; Set the increment for the 
																; pointer to the DMA write
																; control block to zero. 
;----------------------------
;	RVB_Fader_Configure_SetDMAWriteIncrement
;----------------------------	
; On entry
;	'a' contains the increment that should be applied 
;	to the pointer to the DMA write control block
RVB_Fader_Configure_SetDMAWriteIncrement
																; Store the increment that is
																; applied to the pointer to the
																; DMA write control block. This
																; is applied after each buffer
																; has finished fading.
	move	a,x:(r5+RVB_FaderDMAwriteIncrement-StaticWorkspaceBase)

;----------------------------
;	RVB_Fader_Configure
;----------------------------
; On entry
;	No entry parameters
;
RVB_Fader_Configure
	move	#>FADER_STEPSIZE_FADEOUT,a							; Begin by fading the amplitude
	move	a,x:(r5+RVB_FaderStepSize-StaticWorkspaceBase)		; downwards
	
	move	#1.0-FADER_STEPSIZE_FADEIN,a						; Set the initial amplitude
	move	a,x:(r5+RVB_FaderGain-StaticWorkspaceBase)			; to 1 - 1/256

	bra		RVB_Fader_FadeFrame






;	Move the tap position
;	Set the gain step so that we fade the buffer back in.
RVB_Fader_MoveDelayTap
	
	move	x:(r5+RVB_FaderpNewTapList-StaticWorkspaceBase),r0	; Get a pointer to the tap position
	move	x:(r0)+,a											; Get the new tap position
	move	r0,x:(r5+RVB_FaderpNewTapList-StaticWorkspaceBase)	; Point to the next tap position

	sub		#4*SizeOfFrame,a									; Subtract the size of a frame from the 
																; delay because the reference point (in 
																; the linked list of write blocks) has 
																; already been incremented by the DMA
																; controller.

	cmp		#0,a												; Make sure that the delay is always
	clr		a	Iflt											; positive. (Delays should always
	move	a,x1												; be greater than 32, but there could
																; be problems on startup if we don't
																; check)


	move	x:(r5+RVB_FaderpDMAwrite-StaticWorkspaceBase),r0	; A pointer to the write DMA block
	move	x:(r5+RVB_FaderpDMAread-StaticWorkspaceBase),r1		; A pointer to the read DMA block
	
	bsr		RVB_UpdateDmaCommandBlockOffset


	move	#>FADER_STEPSIZE_FADEIN,a							; Begin fading the amplitude
	move	a,x:(r5+RVB_FaderStepSize-StaticWorkspaceBase)		; upwards


	bra		RVB_Fader_FadeFrame






RVB_Fader_DoneAllBuffers
	bsr		RVB_Fader_StopFading
	;bra		RVB_Fader_FinishedFading                        ;Optimized-- Uncomment if code added before RVB_Fader_FinishedFading



; Finished fading a frame
;	- Prepare for the next frame...
RVB_Fader_FinishedFading
	move	#0,a												; Reset the fader to begin at the
	move	a,x:(r5+RVB_FaderStep-StaticWorkspaceBase)			; start of a fade-out sequence

	
	move	x:(r5+RVB_FaderState-StaticWorkspaceBase),a			; Update the state of the fader to
	asl		a													; process the next buffer
	move	a,x:(r5+RVB_FaderState-StaticWorkspaceBase)

	bra		RVB_Fader_Exit















        IF FADER_ENABLE_OUTPUTAMPITUDEFADE


;=========================================
;		RVB_Fader_FadeOutputAmplitudes
;=========================================
; The amplitudes are faded from their current values to the target value.
;	The size of the amplitude step for each channel is
;		0.02 * (TargetAmplitude - CurrentAmplitude)
;
; After the amplitude fader has processed the prescribed number of frames
; the output amplitudes are set equal to the 'TargetAmplitude' in a single 
; step.
; 
RVB_Fader_FadeOutputAmplitudes

													; Check to see if the amplitude fader is active
	move	x:(r5+RVB_FaderAmplitudeState-StaticWorkspaceBase),a
	cmp		#0,a
	blt		RVB_Fader_FadeOutputAmplitudeExit		; If not then exit
													

													; The amplitude fader is active

	move	r5,b									; Get a pointer to the target output amplitudes
	add		#(NewReflectionOutputGain_Channel0-StaticWorkspaceBase),b
	move	b,r0

	;move	r5,b									; Get a pointer to the current output amplitudes
	;add		#(ReflectionOutputGain_Channel0-StaticWorkspaceBase),b
	add		#(ReflectionOutputGain_Channel0-NewReflectionOutputGain_Channel0),b
	move	b,r1


	cmp		#336,a									; If this is the last frame to process
	beq		RVB_Fader_CopyTargetAmplitudes			; set all the current output amplitudes equal 
													; to the target amplitudes.
	

	add		#1,a									; Increment the state of the amplitude fader
	move	a,x:(r5+RVB_FaderAmplitudeState-StaticWorkspaceBase)

; DEBUGGING
;	move x:(r1+3),y0
;	DEBUGGERLOG y0
; ENDOF DEBUGGING

	dor		#6,RVB_Fader_FadeOutputAmplitudesLoop
		move	x:(r1),a							; Read the current output gain
		move	x:(r0)+,b							; Read the target output gain

		sub		b,a									; Calculate the difference 
		
		move	a,y0								; y0=Difference
		mpyi	#FaderAmplitudeDecayRate,y0,a		; a=Difference*0.98
		
		add		a,b									; Update the current amplitude

		move	b,x:(r1)+							; Write the current output gain

RVB_Fader_FadeOutputAmplitudesLoop


RVB_Fader_FadeOutputAmplitudeExit
	rts



RVB_Fader_CopyTargetAmplitudes

	dor		#6,RVB_Fader_CopyTargetAmplitudesLoop
		move	x:(r0)+,x0
		move	x0,x:(r1)+
RVB_Fader_CopyTargetAmplitudesLoop

													; Disable the amplitude fader
	move	#-1,a
	move	a,x:(r5+RVB_FaderAmplitudeState-StaticWorkspaceBase)
	bra		RVB_Fader_FadeOutputAmplitudeExit


        ENDIF ;FADER_ENABLE_OUTPUTAMPITUDEFADE

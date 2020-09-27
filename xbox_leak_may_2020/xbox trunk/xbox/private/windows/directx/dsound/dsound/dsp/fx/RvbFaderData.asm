;	File	:	RvbFaderData.asm
;	Version	:	1.0
;	Date	:	29/09/2000
;
;	Author	:	RDA
;				Sensaura Ltd
;
;
;
; Description
; ===========
; This file defines the data space for the reverb fader.
;
; ***************************************
; ** THIS FILE MUST BE INCLUDED AT THE **
; ** END OF THE FILE "REVERBDATA.ASM"  **
; ***************************************
;	The reason is because this file allocates data space using the 'org:x'
;	instruction and this data must be located with specific bounds for
;	the data space to be initialise correctly.
;
; Version history
; ===============
;	0.x		Development versions
;	1.0		First working version


	org	x:




;	RVB_FaderState determines the state of the fader and indicates which 
;	buffer is being faded at the moment.
;	Bit n of 'RVB_FaderState' is set to indicate that buffer 'n' is being
;	faded. If 'RVB_FaderState'=0 the fader is disabled. Only one buffer
;	can be faded at once.
;
;	RVB_FaderState
;		0	Not fading
;		2^n	Fading buffer n

RVB_FaderState
	dc	0



;	RVB_FaderStep denotes the stage of the current fading operation.  In 
;	practice RVB_FaderStep takes the value equal to the number of samples processed
;	during the current buffer.
;
;	A value between 0 to 255 indicates that the fader is ramping the amplitued down
;	A value equal to 256 indicates that the buffer should have faded
;		to zero so we can change the tap position
;	A value between 257 to 511 indicates that the fader is ramping the amplitued up
;	A value equal to 512 indicates that the fader has finished fading the current
;		buffer.
;
;		0	<= RVB_FaderStep <= 512
RVB_FaderStep
	dc	0



;	RVB_FaderStepSize is the size of the steps applied to RVB_FaderGain
;	on consecutive samples when fading.
;
;		RVB_FaderStepSize += 1/256
RVB_FaderStepSize
	dc	0



;	RVB_FaderGain is the gain of the fader.
;
;		1.0-(1/256) <= RVB_FaderGain < 0
RVB_FaderGain
	dc	0



;	RVB_FaderpDMAread is a pointer to the DMA read block corresponding to
;	the current buffer that is being faded
RVB_FaderpDMAread
	dc	0



;	RVB_FaderpDMAwrite is a pointer to the DMA write block corresponding to
;	the current buffer that is being faded.  This is required when the 
;	tap position is being updated because the DMA read block offset is
;	measured relative to this.
RVB_FaderpDMAwrite
	dc	0


;	After each buffer has been faded, the pointer to the DMA write 
;	control block is incremented by RVB_FaderDMAwriteIncrement.
;	By setting RVB_FaderDMAwriteIncrement to zero, consecutive
;	buffers can be processed that all refer to the same DMA write block.
RVB_FaderDMAwriteIncrement
	dc	0


;	RVB_FaderpNewTapList is a pointer into the block 'RVB_FaderNewTapList'
RVB_FaderpNewTapList
	dc	0



;	RVB_FaderNewTapList contains a list of new tap positions for the 
; current block
RVB_FaderNewTapList
	bsc	8,0


;	RVB_FaderAmplitudeState contins the state of the amplitude fader
;		A -ve value indicates that the amplitude fader is disabled
;		The amplitude fader is enabled by setting the variable to 0.
;		The code will then increment the variable to reflect the current
;		state of the fade process. Once the fade has finished, the variable
;		is set to -1.
RVB_FaderAmplitudeState
	dc	0

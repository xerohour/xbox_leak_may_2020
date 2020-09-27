;	File	:	ReverbData.asm
;	Version	:	2.01
;	Date	:	21/11/2000
;
;	Author	:	RDA
;				Sensaura Ltd
;
;
;
; Description
; ===========
; This file defines the data space for the reverb engine.
;
; Version history
; ===============
;	0.1		First implementation.
;	1.0		Completed and working implementation for version 1.0 of the reverb engine
;	2.0		Implementation for version 2.0 of the reverb engine
;	2.01	Added support for half rate processing.

        IF @DEF('SIM')
	org	x:$80
        ELSE
        org     x:
        ENDIF


;==============================================================
;	Static data that should be preserved across frames
;==============================================================
StaticWorkspaceBase

FX_STATE_BLOCK          ds     DATASIZE_DSP_FX_STATE_2IN_4OUT

;
; state info in X-ram
;

pBuffer									; An array of pointers to the 31 frame buffers.
										; If this block is within the first 32 words of the start of
										; 'pStaticWorkspaceBase' then instructions of the form
										;	move x:(r5+pBuffer5-pStaticWorkspaceBase),a
										; are encoded into one word.
										; The array of buffer pointer are initialise by the function
										; 'RVB_GenerateBufferList'
pBuffer0
	dc	0
pBuffer1
	dc	0
pBuffer2
	dc	0
pBuffer3
	dc	0
pBuffer4
	dc	0
pBuffer5
	dc	0
pBuffer6
	dc	0
pBuffer7
	dc	0
pBuffer8
	dc	0
pBuffer9
	dc	0
pBuffer10
	dc	0
pBuffer11
	dc	0
pBuffer12
	dc	0
pBuffer13
	dc	0
pBuffer14
	dc	0
pBuffer15
	dc	0
pBuffer16
	dc	0
pBuffer17
	dc	0
pBuffer18
	dc	0
pBuffer19
	dc	0
pBuffer20
	dc	0
pBuffer21
	dc	0
pBuffer22
	dc	0
pBuffer23
	dc	0
pBuffer24
	dc	0
pBuffer25
	dc	0
pBuffer26
	dc	0
pBuffer27
	dc	0
pBuffer28
	dc	0
pBuffer29
	dc	0
pBuffer30
	dc	0


;----------------------------------------------------------------
;	Host data transfer blocks
;----------------------------------------------------------------

										; The host buffer information block is used to communicate
										; with the host. Data is transferred into this block defines
										; the positions and size of the delay buffers in host memory
										;
HostBufferInfo
MainDelayLine
	dc	EVM_MainDelayLine
SizeOf_MainDelayLine
	dc	EVM_SizeOf_MainDelayLine-1

MainDelayLine1
	dc	EVM_MainDelayLine1
SizeOf_MainDelayLine1
	dc	EVM_SizeOf_MainDelayLine1-1

MainDelayLine2
	dc	EVM_MainDelayLine2
SizeOf_MainDelayLine2
	dc	EVM_SizeOf_MainDelayLine2-1

MainDelayLine3
	dc	EVM_MainDelayLine3
SizeOf_MainDelayLine3
	dc	EVM_SizeOf_MainDelayLine3-1

BufferReflection0
	dc	EVM_BufferReflection0
SizeOf_BufferReflection0
	dc	EVM_SizeOf_BufferReflection0-1

BufferReflection1
	dc	EVM_BufferReflection1
SizeOf_BufferReflection1
	dc	EVM_SizeOf_BufferReflection1-1

BufferReflection2
	dc	EVM_BufferReflection2
SizeOf_BufferReflection2
	dc	EVM_SizeOf_BufferReflection2-1

BufferReflection3
	dc	EVM_BufferReflection3
SizeOf_BufferReflection3
	dc	EVM_SizeOf_BufferReflection3-1
	
BufferShortReverb0
	dc	EVM_BufferShortReverb0
SizeOf_BufferShortReverb0
	dc	EVM_SizeOf_BufferShortReverb0-1

BufferShortReverb1
	dc	EVM_BufferShortReverb1
SizeOf_BufferShortReverb1
	dc	EVM_SizeOf_BufferShortReverb1-1

BufferShortReverb2
	dc	EVM_BufferShortReverb2
SizeOf_BufferShortReverb2
	dc	EVM_SizeOf_BufferShortReverb2-1

BufferShortReverb3
	dc	EVM_BufferShortReverb3
SizeOf_BufferShortReverb3
	dc	EVM_SizeOf_BufferShortReverb3-1

BufferLongReverb0
	dc	EVM_BufferLongReverb0
SizeOf_BufferLongReverb0
	dc	EVM_SizeOf_BufferLongReverb0-1

BufferLongReverb1
	dc	EVM_BufferLongReverb1
SizeOf_BufferLongReverb1
	dc	EVM_SizeOf_BufferLongReverb1-1


StartOfDspParameterBlock

											;====================================
											; Reflections input tap positions
											;====================================
											; ReflectionsDelay :
											;	The delay between the input and the reflection block
ReflectionsInputDelayList
ReflectionsInputDelay0
	dc	DefaultReflectionInputDelay0
ReflectionsInputDelay1
	dc	DefaultReflectionInputDelay1
ReflectionsInputDelay2
	dc	DefaultReflectionInputDelay2
ReflectionsInputDelay3
	dc	DefaultReflectionInputDelay3
ReflectionsInputDelay4
	dc	DefaultReflectionInputDelay4


											;====================================
											; Short reverb input delay
											;====================================

											; ShortReverbInputDelay :
											;   The delay measured from the start of the main delay line 
											; to the first short reverb tap.
ShortReverbInputDelay
	dc	DefaultShortReverbInputDelay


											;====================================
											; Long reverb input tap positions
											;====================================

LongReverbInputDelay0a
	dc	DefaultLongReverbInputDelay0a
LongReverbInputDelay0b
	dc	DefaultLongReverbInputDelay1a
LongReverbInputDelay1a
	dc	DefaultLongReverbInputDelay0b
LongReverbInputDelay1b
	dc	DefaultLongReverbInputDelay1b
LongReverbInputDelay2a
	dc	DefaultLongReverbInputDelay0c
LongReverbInputDelay2b
	dc	DefaultLongReverbInputDelay1c
LongReverbInputDelay3a
	dc	DefaultLongReverbInputDelay0d
LongReverbInputDelay3b
	dc	DefaultLongReverbInputDelay1d


											;====================================
											; Reflection feedback delay tap positions
											;====================================
											; The following 4 values are the delay tap positions for 
											; the delay buffers in the 4 reflection channels
ReflectionsDelayList
ReflectionDelay0
	dc	DefaultReflectionDelay0						; Channel 0 reflection
ReflectionDelay1
	dc	DefaultReflectionDelay1						; Channel 1 reflection
ReflectionDelay2
	dc	DefaultReflectionDelay2						; Channel 2 reflection
ReflectionDelay3
	dc	DefaultReflectionDelay3						; Channel 3 reflection


											;====================================
											; Long reverb feedback delay tap positions
											;====================================
LongReverbDelay
	dc	DefaultLongReverbDelay0



											;====================================
											; Short reverb gains
											;====================================
											; Input gains
ShortReverbInputGain_Channel0a
	dc	$0
ShortReverbInputGain_Channel0b
	dc	$0
ShortReverbInputGain_Channel1a
	dc	$0
ShortReverbInputGain_Channel1b
	dc	$0
ShortReverbInputGain_Channel2a
	dc	$0
ShortReverbInputGain_Channel2b
	dc	$0
ShortReverbInputGain_Channel3a
	dc	$0
ShortReverbInputGain_Channel3b
	dc	$0


											;====================================
											; Long reverb gains
											;====================================
											; Input gain
LongReverbInputGain
	dc	DefaultLongReverbInputGain

											; Crossfeed gain
LongReverbCrossfeedGain
	dc	DefaultLongReverbCrossfeedGain



											;====================================
											; Output gains
											;====================================
											;
											;	The reflections output gain
NewReflectionOutputGain_Channel0
	dc	$000000
NewReflectionOutputGain_Channel1
	dc	$000000
NewReflectionOutputGain_Channel2
	dc	$000000
NewReflectionOutputGain_Channel3
	dc	$000000

											;	The short reverb output gain
NewShortReverbOutputGain_Channel
	dc	$010000

											;	The long reverb output gain
NewLongReverbOutputGain_Channel
	dc	$000000


;------------------------------------------------------------
;			Number of output channels
;------------------------------------------------------------
; The variable 'nOutputChannels' determines the number of
; output channels. At present the system can operate only
; in 2 or 4 channel mode.  If an invalid value is specified
; the system will use 2 channel mode.
;
; Values
;	2	Two channel mode (Default)
;	4	Four channel mode
;
nOutputChannels
	dc	0



													; This label marks the end of the block where
													; parameters are copied from the host
EndOfSimpleDspParameterBlock

													; The following block contains IIR coefficients.
													; These value are inserted into an IIR data
													; structure.
;---------------------------------------------------------
;			Allocate space for the IIR filters
;---------------------------------------------------------
;	Note that the default values for the IIR filters quoted
;	in the following section are overwritten when the
;	reverb engine initialises itself.
;	The values below are therefore not important.



IIR_Input
	NewIIR_Coefficients 0,0					; First order input filter


IIR_LongReverb_MainDelay0
	NewIIR_Coefficients 0,0

IIR_LongReverb_MainDelay1
	NewIIR_Coefficients 0,0

IIR_LongReverb_MainDelay2
	NewIIR_Coefficients 0,0

IIR_ShortReverbChannel0
	NewIIR_Coefficients 0,0
 
IIR_ShortReverbChannel1
	NewIIR_Coefficients 0,0

IIR_ShortReverbChannel2
	NewIIR_Coefficients 0,0

IIR_ShortReverbChannel3
	NewIIR_Coefficients 0,0

IIR_LongReverb_Channel0
	NewIIR_Coefficients 0,0

IIR_LongReverb_Channel1
	NewIIR_Coefficients 0,0


;------------------------------------------------------------
;	Allocate space for constant data
;------------------------------------------------------------
; The constant data can be initialised by the host at start-up.
;
;			Crossfeed coefficients
;
; The following are the fixed crossfeed matrix coefficients
;	Note that the coefficients are interleaved to reduce storage space
;	from 16 coefficients to 7

CrossfeedMatrixChannel3
	dc	$C00000					;-0.5
CrossfeedMatrixChannel2
	dc	$C00000					;-0.5
CrossfeedMatrixChannel1
	dc	$C00000					;-0.5
CrossfeedMatrixChannel0
	dc	$400000					;0.5
	dc	$C00000					;-0.5
	dc	$C00000					;-0.5
	dc	$C00000					;-0.5


;	The following block defines the gains in the reflection feedback loop
ReflectionsFeedbackGain
	dc	-3355443
	dc	3355443
	dc	-3355443
	dc	-3355443
;	msg		"Set reverb feedback gain to zero"
;	dc	0
;	dc	0
;	dc	0
;	dc	0

; The following array specifies the position of the short reverb input taps
; relative to 'ShortReverbInputDelay'.

ShortReverbInputDelayList
	dc	DefaultShortReverbInputDelay0a						; Channel 0a reflection
	dc	DefaultShortReverbInputDelay0b						; Channel 0b reflection
	dc	DefaultShortReverbInputDelay1a						; Channel 1a reflection
	dc	DefaultShortReverbInputDelay1b						; Channel 1b reflection
	dc	DefaultShortReverbInputDelay2a						; Channel 2a reflection
	dc	DefaultShortReverbInputDelay2b						; Channel 2b reflection
	dc	DefaultShortReverbInputDelay3a						; Channel 3a reflection
	dc	DefaultShortReverbInputDelay3b						; Channel 3b reflection


; The following values are the constant delays (measured in 
; samples) for the 4 short reverb channels.
ShortReverbDelayList
	dc	DefaultShortReverbDelay0					; Channel 0 reflection
	dc	DefaultShortReverbDelay1					; Channel 1 reflection
	dc	DefaultShortReverbDelay2					; Channel 2 reflection
	dc	DefaultShortReverbDelay3					; Channel 3 reflection
  
	
	IF @DEF('HALFSAMPLERATE')
											;  Filter data for the antialiasing filters											; (Coefficients are stored elsewhere)
IIR2_AntiAlias_Input
	IIR2_Data
	IIR2_Data

IIR2_AntiAlias_LeftFront
	IIR2_Data
	IIR2_Data

IIR2_AntiAlias_RightFront
	IIR2_Data
	IIR2_Data

IIR2_AntiAlias_LeftRear
	IIR2_Data
	IIR2_Data

IIR2_AntiAlias_RightRear
	IIR2_Data
	IIR2_Data
	ENDIF

EndOfDspParameterBlock

	IF @DEF('HALFSAMPLERATE')        
IIR2_AntiAliasing_Coefficients1
  
	IIR2_Coefficients 0.517806,0.587918,0.517806,0.280613,-0.275552		; Second order antialiasing filter part 1
IIR2_AntiAliasing_Coefficients2
	IIR2_Coefficients 0.517806,0.034041,0.517806,0.136654,-0.882101		; Second order antialiasing filter part 2
	ENDIF


;---------------------------------------------------------------
;			DMA control blocks to write results back to host
;---------------------------------------------------------------

;																Filtered input - writing to main delay line
DMA_ControlBlock_Wr0
	NewDMAcbs DMA_ControlBlock_Wr1,DMAcbs_DspToHost,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,0				; Stop the transfere at the end of the writes

;																Main delay line first IIR
DMA_ControlBlock_Wr1
	NewDMAcbs DMA_ControlBlock_Wr2,DMAcbs_DspToHost,SizeOfFrame,14,EVM_MainDelayLine1,EVM_SizeOf_MainDelayLine1,0

;																Main delay line second IIR
DMA_ControlBlock_Wr2
	NewDMAcbs DMA_ControlBlock_Wr3,DMAcbs_DspToHost,SizeOfFrame,16,EVM_MainDelayLine2,EVM_SizeOf_MainDelayLine2,0

;																Main delay line third IIR
DMA_ControlBlock_Wr3
	NewDMAcbs DMA_ControlBlock_Wr4,DMAcbs_DspToHost,SizeOfFrame,18,EVM_MainDelayLine3,EVM_SizeOf_MainDelayLine3,0

;																Channel 0 - Reflection delay line write
DMA_ControlBlock_Wr4
	NewDMAcbs DMA_ControlBlock_Wr5,DMAcbs_DspToHost,SizeOfFrame,1,EVM_BufferReflection0,EVM_SizeOf_BufferReflection0,0

;																Channel 1 - Reflection delay line write
DMA_ControlBlock_Wr5
	NewDMAcbs DMA_ControlBlock_Wr6,DMAcbs_DspToHost,SizeOfFrame,2,EVM_BufferReflection1,EVM_SizeOf_BufferReflection1,0

;																Channel 2 - Reflection delay line write
DMA_ControlBlock_Wr6
	NewDMAcbs DMA_ControlBlock_Wr7,DMAcbs_DspToHost,SizeOfFrame,3,EVM_BufferReflection2,EVM_SizeOf_BufferReflection2,0

;																Channel 3 - Reflection delay line write
DMA_ControlBlock_Wr7
	NewDMAcbs DMA_ControlBlock_Wr8,DMAcbs_DspToHost,SizeOfFrame,4,EVM_BufferReflection3,EVM_SizeOf_BufferReflection3,0

;																Channel 0 - Short reverb delay line write
DMA_ControlBlock_Wr8
	NewDMAcbs DMA_ControlBlock_Wr9,DMAcbs_DspToHost,SizeOfFrame,5,EVM_BufferShortReverb0,EVM_SizeOf_BufferShortReverb0,0

;																Channel 1 - Short reverb delay line write
DMA_ControlBlock_Wr9
	NewDMAcbs DMA_ControlBlock_Wr10,DMAcbs_DspToHost,SizeOfFrame,6,EVM_BufferShortReverb1,EVM_SizeOf_BufferShortReverb1,0

;																Channel 2 - Short reverb delay line write
DMA_ControlBlock_Wr10
	NewDMAcbs DMA_ControlBlock_Wr11,DMAcbs_DspToHost,SizeOfFrame,7,EVM_BufferShortReverb2,EVM_SizeOf_BufferShortReverb2,0

;																Channel 3 - Short reverb delay line write
DMA_ControlBlock_Wr11
	NewDMAcbs DMA_ControlBlock_Wr12,DMAcbs_DspToHost,SizeOfFrame,8,EVM_BufferShortReverb3,EVM_SizeOf_BufferShortReverb3,0


;																Channel 0 - Long reverb delay line write
DMA_ControlBlock_Wr12
	NewDMAcbs DMA_ControlBlock_Wr13,DMAcbs_DspToHost,SizeOfFrame,9,EVM_BufferLongReverb0,EVM_SizeOf_BufferLongReverb0,0

;																Channel 1 - Long reverb delay line write
DMA_ControlBlock_Wr13
	NewDMAcbs DMAcbs_eol,DMAcbs_DspToHost,SizeOfFrame,10,EVM_BufferLongReverb1,EVM_SizeOf_BufferLongReverb1,0

	



;---------------------------------------------------------
;		DMA control blocks to read data from host
;---------------------------------------------------------


;																First reflection tap 0
DMA_ControlBlock_Rd0
	NewDMAcbs DMA_ControlBlock_Rd1,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultReflectionInputDelay+DefaultReflectionInputDelay0)

;																Channel 0 - Reflection tap 1
DMA_ControlBlock_Rd1
	NewDMAcbs DMA_ControlBlock_Rd2,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultReflectionInputDelay+DefaultReflectionInputDelay1)

;																Channel 1 - Reflection tap 2
DMA_ControlBlock_Rd2
	NewDMAcbs DMA_ControlBlock_Rd3,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultReflectionInputDelay+DefaultReflectionInputDelay2)

;																Channel 2 - Reflection tap 3
DMA_ControlBlock_Rd3
	NewDMAcbs DMA_ControlBlock_Rd4,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultReflectionInputDelay+DefaultReflectionInputDelay3)

;																Channel 3 - Reflection tap 1
DMA_ControlBlock_Rd4
	NewDMAcbs DMA_ControlBlock_Rd5,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultReflectionInputDelay+DefaultReflectionInputDelay4)







;																Channel 0 - Short reverb tap A
DMA_ControlBlock_Rd5
	NewDMAcbs DMA_ControlBlock_Rd6,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultShortReverbInputDelay+DefaultShortReverbInputDelay0a)

;																Channel 0 - Short reverb tap B
DMA_ControlBlock_Rd6
	NewDMAcbs DMA_ControlBlock_Rd7,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultShortReverbInputDelay+DefaultShortReverbInputDelay0b)

;																Channel 1 - Short reverb tap A
DMA_ControlBlock_Rd7
	NewDMAcbs DMA_ControlBlock_Rd8,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultShortReverbInputDelay+DefaultShortReverbInputDelay1a)

;																Channel 1 - Short reverb tap B
DMA_ControlBlock_Rd8
	NewDMAcbs DMA_ControlBlock_Rd9,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultShortReverbInputDelay+DefaultShortReverbInputDelay1b)

;																Channel 2 - Short reverb tap A
DMA_ControlBlock_Rd9
	NewDMAcbs DMA_ControlBlock_Rd10,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultShortReverbInputDelay+DefaultShortReverbInputDelay2a)

;																Channel 2 - Short reverb tap B
DMA_ControlBlock_Rd10
	NewDMAcbs DMA_ControlBlock_Rd11,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultShortReverbInputDelay+DefaultShortReverbInputDelay2b)

;																Channel 3 - Short reverb tap A
DMA_ControlBlock_Rd11
	NewDMAcbs DMA_ControlBlock_Rd12,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultShortReverbInputDelay+DefaultShortReverbInputDelay3a)

;																Channel 3 - Short reverb tap B
DMA_ControlBlock_Rd12
	NewDMAcbs DMA_ControlBlock_Rd13,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-(DefaultShortReverbInputDelay+DefaultShortReverbInputDelay3b)





;																Channel 0 - Long reverb tap a
DMA_ControlBlock_Rd13
	NewDMAcbs DMA_ControlBlock_Rd14,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-DefaultLongReverbInputDelay0a

;																Channel 1 - Long reverb tap a
DMA_ControlBlock_Rd14
	NewDMAcbs DMA_ControlBlock_Rd15,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine,EVM_SizeOf_MainDelayLine,EVM_SizeOf_MainDelayLine-DefaultLongReverbInputDelay1a

;																Channel 0 - Long reverb tap b
DMA_ControlBlock_Rd15
	NewDMAcbs DMA_ControlBlock_Rd16,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine1,EVM_SizeOf_MainDelayLine1,EVM_SizeOf_MainDelayLine1-DefaultLongReverbInputDelay0b

;																Channel 1 - Long reverb tap b
DMA_ControlBlock_Rd16
	NewDMAcbs DMA_ControlBlock_Rd17,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine1,EVM_SizeOf_MainDelayLine1,EVM_SizeOf_MainDelayLine1-DefaultLongReverbInputDelay1b


;																Channel 0 - Long reverb tap c
DMA_ControlBlock_Rd17
	NewDMAcbs DMA_ControlBlock_Rd18,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine2,EVM_SizeOf_MainDelayLine2,EVM_SizeOf_MainDelayLine2-DefaultLongReverbInputDelay0c

;																Channel 1 - Long reverb tap c
DMA_ControlBlock_Rd18
	NewDMAcbs DMA_ControlBlock_Rd19,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine2,EVM_SizeOf_MainDelayLine2,EVM_SizeOf_MainDelayLine2-DefaultLongReverbInputDelay1c

;																Channel 0 - Long reverb tap d
DMA_ControlBlock_Rd19
	NewDMAcbs DMA_ControlBlock_Rd20,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine3,EVM_SizeOf_MainDelayLine3,EVM_SizeOf_MainDelayLine3-DefaultLongReverbInputDelay0d

;																Channel 1 - Long reverb tap d
DMA_ControlBlock_Rd20
	NewDMAcbs DMA_ControlBlock_Rd21,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_MainDelayLine3,EVM_SizeOf_MainDelayLine3,EVM_SizeOf_MainDelayLine3-DefaultLongReverbInputDelay1d









;																Channel 0 - Reflection feedback delay buffer
DMA_ControlBlock_Rd21
	NewDMAcbs DMA_ControlBlock_Rd22,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_BufferReflection0,EVM_SizeOf_BufferReflection0,EVM_SizeOf_BufferReflection0-DefaultReflectionDelay0

;																Channel 1 - Reflection feedback delay buffer
DMA_ControlBlock_Rd22
	NewDMAcbs DMA_ControlBlock_Rd23,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_BufferReflection1,EVM_SizeOf_BufferReflection1,EVM_SizeOf_BufferReflection1-DefaultReflectionDelay1

;																Channel 2 - Reflection feedback delay buffer
DMA_ControlBlock_Rd23
	NewDMAcbs DMA_ControlBlock_Rd24,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_BufferReflection2,EVM_SizeOf_BufferReflection2,EVM_SizeOf_BufferReflection2-DefaultReflectionDelay2

;																Channel 3 - Reflection feedback delay buffer
DMA_ControlBlock_Rd24
	NewDMAcbs DMA_ControlBlock_Rd25,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_BufferReflection3,EVM_SizeOf_BufferReflection3,EVM_SizeOf_BufferReflection3-DefaultReflectionDelay3



;																Channel 0 - Reverb feedback delay buffer
DMA_ControlBlock_Rd25
	NewDMAcbs DMA_ControlBlock_Rd26,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_BufferShortReverb0,EVM_SizeOf_BufferShortReverb0,EVM_SizeOf_BufferShortReverb0-DefaultShortReverbDelay0

;																Channel 1 - Reverb feedback delay buffer
DMA_ControlBlock_Rd26
	NewDMAcbs DMA_ControlBlock_Rd27,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_BufferShortReverb1,EVM_SizeOf_BufferShortReverb1,EVM_SizeOf_BufferShortReverb1-DefaultShortReverbDelay1

;																Channel 2 - Reverb feedback delay buffer
DMA_ControlBlock_Rd27
	NewDMAcbs DMA_ControlBlock_Rd28,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_BufferShortReverb2,EVM_SizeOf_BufferShortReverb2,EVM_SizeOf_BufferShortReverb2-DefaultShortReverbDelay2

;																Channel 3 - Reverb feedback delay buffer
DMA_ControlBlock_Rd28
	NewDMAcbs DMA_ControlBlock_Rd29,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_BufferShortReverb3,EVM_SizeOf_BufferShortReverb3,EVM_SizeOf_BufferShortReverb3-DefaultShortReverbDelay3




;																Channel 0 - Long reveb feedback delay buffer
DMA_ControlBlock_Rd29
	NewDMAcbs DMA_ControlBlock_Rd30,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_BufferLongReverb0,EVM_SizeOf_BufferLongReverb0,EVM_SizeOf_BufferLongReverb0-DefaultLongReverbDelay0

;																Channel 1 - Long reveb feedback delay buffer
DMA_ControlBlock_Rd30
	NewDMAcbs DMAcbs_eol,DMAcbs_HostToDsp,SizeOfFrame,0,EVM_BufferLongReverb1,EVM_SizeOf_BufferLongReverb1,EVM_SizeOf_BufferLongReverb1-DefaultLongReverbDelay1

;---------------------------------------------------------------
; Copy of output gains used by the fader
;
;---------------------------------------------------------------

ReflectionOutputGain_Channel0
	dc	$000000
ReflectionOutputGain_Channel1
	dc	$000000
ReflectionOutputGain_Channel2
	dc	$000000
ReflectionOutputGain_Channel3
	dc	$000000

											;	The short reverb output gain
ShortReverbOutputGain_Channel
	dc	$010000

											;	The long reverb output gain
LongReverbOutputGain_Channel
	dc	$000000


	IF FADER_ENABLED
													; If the fader has been enabled, allocated
													; memory for its data space
	include 'RvbFaderData.asm'
	ENDIF


EndOfPrivateData

; This file was written to enable the reverb engine to run
; in stand-alone mode on the FPGA version of the xbox.
;
; The code in this file does the following
;	1.	Initialises all the constant data for the reverb engine
;	2.	Copies a set of low-level reverb parameters to the
;		reverb engine.

ReverbBase equ $C000

;
; debug only BASE XRAM offset is $80
;

XRAM_BASE               equ $0

; The following constants determine which reverb coefficients are used.
; A set is enabled by setting the appropriate constant equal to 1.
; Make sure that only one constant in the list is ever 1.
BATHROOM		set 1
HANGAR			set 0
TESTREVERB		set 0

;============================================================
;		Initialise the constant data in the reverb engine
;============================================================
InitialiseConstants
        move    #>CrossfeedMatrixChannel3-StaticWorkspaceBase,x0
        move    rD,a
        add     x0,a
        move    a,r0

	move	#>$C00000,x0					;-0.5
	move	x0,x:(r0)+
	move	#>$C00000,x0					;-0.5
	move	x0,x:(r0)+
	move	#>$C00000,x0					;-0.5
	move	x0,x:(r0)+
	move	#>$400000,x0					;0.5
	move	x0,x:(r0)+
	move	#>$C00000,x0					;-0.5
	move	x0,x:(r0)+
	move	#>$C00000,x0					;-0.5
	move	x0,x:(r0)+
	move	#>$C00000,x0					;-0.5
	move	x0,x:(r0)+


;	The following block defines the gains in the reflection feedback loop

        move    #>ReflectionsFeedbackGain-StaticWorkspaceBase,x0
        move    rD,a
        add     x0,a
        move    a,r0

	move	#>-3355443,x0
	move	x0,x:(r0)+
	move	#>3355443,x0
	move	x0,x:(r0)+
	move	#>-3355443,x0
	move	x0,x:(r0)+
	move	#>-3355443,x0
	move	x0,x:(r0)+



; The following array specifies the position of the short reverb input taps
; relative to 'ShortReverbInputDelay'.

        move    #>ShortReverbInputDelayList-StaticWorkspaceBase,x0
        move    rD,a
        add     x0,a
        move    a,r0

	move	#>DefaultShortReverbInputDelay0a*4,x0						; Channel 0a reflection
	move	x0,x:(r0)+
	move	#>DefaultShortReverbInputDelay0b*4,x0						; Channel 0b reflection
	move	x0,x:(r0)+
	move	#>DefaultShortReverbInputDelay1a*4,x0						; Channel 1a reflection
	move	x0,x:(r0)+
	move	#>DefaultShortReverbInputDelay1b*4,x0						; Channel 1b reflection
	move	x0,x:(r0)+
	move	#>DefaultShortReverbInputDelay2a*4,x0						; Channel 2a reflection
	move	x0,x:(r0)+
	move	#>DefaultShortReverbInputDelay2b*4,x0						; Channel 2b reflection
	move	x0,x:(r0)+
	move	#>DefaultShortReverbInputDelay3a*4,x0						; Channel 3a reflection
	move	x0,x:(r0)+
	move	#>DefaultShortReverbInputDelay3b*4,x0						; Channel 3b reflection
	move	x0,x:(r0)+




; The following values are the constant delays (measured in 
; samples) for the 4 short reverb channels.

        move    #>ShortReverbDelayList-StaticWorkspaceBase,x0
        move    rD,a
        add     x0,a
        move    a,r0

	move	#>DefaultShortReverbDelay0*4,x0				;,x0 Channel 0 reflection
	move	x0,x:(r0)+
	move	#>DefaultShortReverbDelay1*4,x0				;,x0 Channel 1 reflection
	move	x0,x:(r0)+
	move	#>DefaultShortReverbDelay2*4,x0				;,x0 Channel 2 reflection
	move	x0,x:(r0)+
	move	#>DefaultShortReverbDelay3*4,x0				;,x0 Channel 3 reflection
	move	x0,x:(r0)+


	IF @DEF('HALFSAMPLERATE')
			; Second order antialiasing filter part 1

        move    #>IIR2_AntiAliasing_Coefficients1-XRAM_BASE,r0
	move	#0.517806,x0
	move	x0,x:(r0)+
	move	#0.587918,x0
	move	x0,x:(r0)+
	move	#0.517806,x0
	move	x0,x:(r0)+
	move	#0.280613,x0
	move	x0,x:(r0)+
	move	#-0.275552,x0
	move	x0,x:(r0)+

		; Second order antialiasing filter part 2
        move    #>IIR2_AntiAliasing_Coefficients2-XRAM_BASE,r0
	move	#0.517806,x0
	move	x0,x:(r0)+
	move	#0.034041,x0
	move	x0,x:(r0)+
	move	#0.517806,x0
	move	x0,x:(r0)+
	move	#0.136654,x0
	move	x0,x:(r0)+
	move	#-0.882101,x0
	move	x0,x:(r0)+
	ENDIF		; end HALFSAMPLERATE

	rts




	IF (BATHROOM)
	msg	"Bathroom reverb coefficients"
;============================================================
;	Download a set of coefficients to the reverb engine.
;============================================================
;	This code performs the task of transferring a set of
;	reverb parameters to the reverb engine.  These parameters
;	correspond to the I3DL2 'bathroom' environment.
;
DownloadCoefficients
        clr     a
	; Initialise the pointer to system memory
        move    #>HostBufferInfo-StaticWorkspaceBase,x0
        move    rD,a
        add     x0,a
        move    a,r0

	move	#>(ReverbBase+(131072-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((11999+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(143072-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((11999+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(155072-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((11999+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(167072-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((11999+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(179072-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((1359+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(180432-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((1227+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(181660-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((1283+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(182944-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((1439+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(184384-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((2783+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(187168-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((3371+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(190540-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((4087+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(194628-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((4955+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(199584-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((31279+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(230864-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((31279+1)*4-1),x0
	move	x0,x:(r0)+
        
	; Initialise the reverb data parameters
	move	#>336*4,x0			; Reflections delays
	move	x0,x:(r0)+
	move	#>392*4,x0
	move	x0,x:(r0)+
	move	#>372*4,x0
	move	x0,x:(r0)+
	move	#>428*4,x0
	move	x0,x:(r0)+
	move	#>460*4,x0
	move	x0,x:(r0)+

	move	#>524*4,x0			; Short reverb input delay
	move	x0,x:(r0)+
	
	
	move	#>1920*4,x0			; Long reverb input delays
	move	x0,x:(r0)+
	move	#>1016*4,x0
	move	x0,x:(r0)+
	move	#>9044*4,x0
	move	x0,x:(r0)+
	move	#>12000*4,x0
	move	x0,x:(r0)+
	move	#>10960*4,x0
	move	x0,x:(r0)+
	move	#>11908*4,x0
	move	x0,x:(r0)+
	move	#>10960*4,x0
	move	x0,x:(r0)+
	move	#>6512*4,x0
	move	x0,x:(r0)+

	move	#>304*4,x0			; Reflections delays
	move	x0,x:(r0)+
	move	#>284*4,x0
	move	x0,x:(r0)+
	move	#>296*4,x0
	move	x0,x:(r0)+
	move	#>328*4,x0
	move	x0,x:(r0)+

	move	#>31280*4,x0			; Long reverb delay
	move	x0,x:(r0)+

	move	#>8388607,x0		; Short reverb input gains
	move	x0,x:(r0)+
	move	#>-8388607,x0
	move	x0,x:(r0)+
	move	#>-8388607,x0
	move	x0,x:(r0)+
	move	#>8388607,x0
	move	x0,x:(r0)+
	move	#>8388607,x0
	move	x0,x:(r0)+
	move	#>8388607,x0
	move	x0,x:(r0)+
	move	#>-8388607,x0
	move	x0,x:(r0)+
	move	#>-8388607,x0
	move	x0,x:(r0)+

	move	#>0,x0				; Long reverb input gain
	move	x0,x:(r0)+

	move	#>1677721,x0		; Long reverb crossfeed gain
	move	x0,x:(r0)+

	move	#>7715,x0			; Reflections output gains
	move	x0,x:(r0)+
	move	#>7038,x0
	move	x0,x:(r0)+
	move	#>7309,x0
	move	x0,x:(r0)+
	move	#>8121,x0
	move	x0,x:(r0)+

	move	#>67839,x0			; Short reverb output gain
	move	x0,x:(r0)+

	move	#>67839,x0			; Long reverb output gain
	move	x0,x:(r0)+

	move	#>2,x0				; Number of channels
	move	x0,x:(r0)+

        move    #>0,x0
        move    x0,x:(r0)+
	move	#>7100993,x0		; Input IIR
	move	x0,x:(r0)+
	move	#>1287615,x0
	move	x0,x:(r0)+

        move    #>0,x0
        move    x0,x:(r0)+
	move	#>6485885,x0		; Main delay line IIRs
	move	x0,x:(r0)+
	move	#>597551,x0       
	move	x0,x:(r0)+

        move    #>0,x0
        move    x0,x:(r0)+
	move	#>6485885,x0
	move	x0,x:(r0)+
	move	#>597551,x0
	move	x0,x:(r0)+

        move    #>0,x0
        move    x0,x:(r0)+
	move	#>6485885,x0
	move	x0,x:(r0)+
	move	#>597551,x0
	move	x0,x:(r0)+

        move    #>0,x0
        move    x0,x:(r0)+        
	move	#>3694380,x0		; Short reverb IIRs
	move	x0,x:(r0)+
	move	#>3589749,x0
	move	x0,x:(r0)+

        move    #>0,x0
        move    x0,x:(r0)+        
	move	#>4042153,x0
	move	x0,x:(r0)+
	move	#>3141486,x0
	move	x0,x:(r0)+

        move    #>0,x0
        move    x0,x:(r0)+
	move	#>4406414,x0
	move	x0,x:(r0)+
	move	#>2686101,x0
	move	x0,x:(r0)+

        move    #>0,x0
        move    x0,x:(r0)+
	move	#>4766351,x0
	move	x0,x:(r0)+
	move	#>2246359,x0
	move	x0,x:(r0)+

        move    #>0,x0
        move    x0,x:(r0)+
	move	#>8285400,x0		; Long reverb IIRs
	move	x0,x:(r0)+
	move	#>1001,x0
	move	x0,x:(r0)+

        move    #>0,x0
        move    x0,x:(r0)+
	move	#>8285400,x0
	move	x0,x:(r0)+
	move	#>1001,x0
	move	x0,x:(r0)+

	rts

	ENDIF


	IF (HANGAR)
	msg	"Hangar reverb coefficients"
;============================================================
;	Download a set of coefficients to the reverb engine.
;============================================================
;	This code performs the task of transferring a set of
;	reverb parameters to the reverb engine.  These parameters
;	correspond to the I3DL2 hangar environment.
;
DownloadCoefficients
	; Initialise the pointer to system memory
	move	#>HostBufferInfo-XRAM_BASE,r0

;								Host addresses and sizes (converted to bytes)

	move	#>(ReverbBase+(131072-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((11999+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(143072-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((11999+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(155072-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((11999+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(167072-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((11999+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(179072-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((1359+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(180432-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((1227+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(181660-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((1283+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(182944-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((1439+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(184384-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((2783+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(187168-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((3371+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(190540-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((4087+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(194628-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((4955+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(199584-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((31279+1)*4-1),x0
	move	x0,x:(r0)+
	move	#>(ReverbBase+(230864-131072)*4),x0
	move	x0,x:(r0)+
	move	#>((31279+1)*4-1),x0
	move	x0,x:(r0)+

;									Delay buffer sizes (converted into bytes)
	move	#>956*4,x0
	move	x0,x:(r0)+
	move	#>1108*4,x0
	move	x0,x:(r0)+
	move	#>1052*4,x0
	move	x0,x:(r0)+
	move	#>1204*4,x0
	move	x0,x:(r0)+
	move	#>1300*4,x0
	move	x0,x:(r0)+
	move	#>1436*4,x0
	move	x0,x:(r0)+
	move	#>1920*4,x0
	move	x0,x:(r0)+
	move	#>1016*4,x0
	move	x0,x:(r0)+
	move	#>9044*4,x0
	move	x0,x:(r0)+
	move	#>12000*4,x0
	move	x0,x:(r0)+
	move	#>10960*4,x0
	move	x0,x:(r0)+
	move	#>11908*4,x0
	move	x0,x:(r0)+
	move	#>10960*4,x0
	move	x0,x:(r0)+
	move	#>6512*4,x0
	move	x0,x:(r0)+
	move	#>528*4,x0
	move	x0,x:(r0)+
	move	#>484*4,x0
	move	x0,x:(r0)+
	move	#>508*4,x0
	move	x0,x:(r0)+
	move	#>564*4,x0
	move	x0,x:(r0)+
	move	#>31280*4,x0
	move	x0,x:(r0)+

;									Gains
	move	#>8388607,x0
	move	x0,x:(r0)+
	move	#>-8388607,x0
	move	x0,x:(r0)+
	move	#>-8388607,x0
	move	x0,x:(r0)+
	move	#>8388607,x0
	move	x0,x:(r0)+
	move	#>8388607,x0
	move	x0,x:(r0)+
	move	#>8388607,x0
	move	x0,x:(r0)+
	move	#>-8388607,x0
	move	x0,x:(r0)+
	move	#>-8388607,x0
	move	x0,x:(r0)+

	move	#>0,x0
	move	x0,x:(r0)+
	move	#>1677721,x0
	move	x0,x:(r0)+
	move	#>5906,x0
	move	x0,x:(r0)+
	move	#>5388,x0
	move	x0,x:(r0)+
	move	#>5595,x0
	move	x0,x:(r0)+
	move	#>6217,x0
	move	x0,x:(r0)+
	move	#>26030,x0
	move	x0,x:(r0)+
	move	#>26030,x0
	move	x0,x:(r0)+

;									Number of channels
	move	#>2,x0


;									IIRs
	move	x0,x:(r0)+
	move	#>6773307,x0
	move	x0,x:(r0)+
	move	#>1615300,x0
	move	x0,x:(r0)+
	move	#>5432341,x0
	move	x0,x:(r0)+
	move	#>2490253,x0
	move	x0,x:(r0)+
	move	#>5432341,x0
	move	x0,x:(r0)+
	move	#>2490253,x0
	move	x0,x:(r0)+
	move	#>5432341,x0
	move	x0,x:(r0)+
	move	#>2490253,x0
	move	x0,x:(r0)+
	move	#>2778974,x0
	move	x0,x:(r0)+
	move	#>5394292,x0
	move	x0,x:(r0)+
	move	#>3097415,x0
	move	x0,x:(r0)+
	move	#>5047235,x0
	move	x0,x:(r0)+
	move	#>3426655,x0
	move	x0,x:(r0)+
	move	#>4684385,x0
	move	x0,x:(r0)+
	move	#>3765125,x0
	move	x0,x:(r0)+
	move	#>4309920,x0
	move	x0,x:(r0)+
	move	#>7862650,x0
	move	x0,x:(r0)+
	move	#>264515,x0
	move	x0,x:(r0)+
	move	#>7862650,x0
	move	x0,x:(r0)+
	move	#>264515,x0
	move	x0,x:(r0)+
	rts

	ENDIF

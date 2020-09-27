;*******************************************************************
;
;		Rvb.asm
;               wrapper around sensaura ID3l2 reverb
;
;******************************************************************        

	IF @DEF('HALFSAMPLERATE')
	msg		'Half sample rate version compiling'
	ELSE
	msg		'Full sample rate version compiling'
	ENDIF

        IF @DEF('STANDALONE')
 	include 'Util.h'

        ;
        ; the following define will disable unused DMA routins in fxdmautil.asm
        ;
CIRCULAR_DMA            equ     1

        ENDIF

        org p:
StartReverb

        FXEntryPoint

        IF !@DEF('STANDALONE')

        ; ----------------------------------------------
        ; Custom initialisation for hardwired to the engine version

	bsr InitialiseConstants				; Initialise the constant data in the reverb engine.
							; These constants would normally be part of the 
							; binary file.
        
        
        ENDIF

;---------------------------------------------------------------------------------
;											Prepare input/output buffers
;---------------------------------------------------------------------------------

        IF @DEF('FX_TEST')
        
        ; setup scratch

        move    #$C000,x0
        move    x0,x:(rD+FX_STATE_SCRATCH_BASE)

        move    #>kFxSendMixBuf00,x0     
        move    #>kFxSendMixBuf00,y0

	move	x0,x:(r5+FX_STATE_IO2_IN0)                                      ; LEFT INPUT
	move	y0,x:(r5+FX_STATE_IO2_IN1)

        move    #>kReadWriteMixBuf00,x0         ; front left
        move    #>kReadWriteMixBuf01,x1         ; front right
        move    #>kReadWriteMixBuf04,y0         ; rear left
        move    #>kReadWriteMixBuf05,y1         ; rear right

	; Configure the output for the (front) left channel
	move	x0,x:(r5+FX_STATE_IO2_OUT0) 
	; Configure the output for the (front) right channel
	move	x1,x:(r5+FX_STATE_IO2_OUT1) 
	; Configure the output for the (rear) left channel
	move	y0,x:(r5+FX_STATE_IO2_OUT2) 
	; Configure the output for the (rear) right channel
	move	y1,x:(r5+FX_STATE_IO2_OUT3) 

        ;
        ; fill in the tempbixbins
        ;

        move    r6,a
        lua     (rD+FX_STATE_IO2_OUT3+1),r0 

        dor     #31,FillTempBinLoop

        move    a,x:(r0)+
        add     #kFrameLength,a

FillTempBinLoop

        ENDIF

;---------------------------------------------------------------------------------


;---------------------------------------------------------------------------------
;				Download a set of reverb parameters
;---------------------------------------------------------------------------------

        IF !@DEF('STANDALONE')
	bsr	DownloadCoefficients			; Configure the reverb engine with a specific set 
							; of parameters.  (This call takes the place of
							; the DMA transfer that would download the low level
							; reverb parameters).
;---------------------------------------------------------------------------------
        ELSE

        ;
        ; setup r0 with the base of HostInfo block so we can init the
        ; host scratch offsets for all delay taps
        ; the lengths are already initialized by dsound/xgpimage
        ;
             
	move	#>HostBufferInfo-StaticWorkspaceBase,x0
        move    rD,a
        add     x0,a
        move    a,r0
        add     #1,a
        move    a,r1
        move    #2,n0
        move    #2,n1

        move    x:(rD+FX_STATE_SCRATCH_BASE),x0
        move    x0,x:(r0)+n0

        dor     #((StartOfDspParameterBlock-HostBufferInfo)/2-1),InitHostBufferInfoLoop

        move    x:(r0-2),a
        move    x:(r1)+n1,x0
        add     x0,a
        add     #1,a
        move    a,x:(r0)+n0       
         
InitHostBufferInfoLoop

	IF @DEF('HALFSAMPLERATE')
	; Second order antialiasing filter part 1

        move    #>IIR2_AntiAliasing_Coefficients1-StaticWorkspaceBase,x0
        move    rD,a
        add     x0,a
        move    a,r0

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
        move    #>IIR2_AntiAliasing_Coefficients2-StaticWorkspaceBase,x0
        move    rD,a
        add     x0,a
        move    a,r0

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


        ENDIF



;---------------------------------------------------------------------------------
;				Reset the reverb engine
;---------------------------------------------------------------------------------
;	Note that a set of parameters must have
;	been downloadedbefore the reverb engine
;       is reset.

	bsr	RVB_Reset
;---------------------------------------------------------------------------------

	
ComputeFX
    
; -------------------- DSP -------------------------------
        
        ;
        ; check if an update occured. If it did, invoke the fader
        ;

        move    x:(rD+FX_STATE_FLAGS),x0
        brclr   #BIT_FX_STATE_FLAG_UPDATE,x0,ProcessFrame

        ;
        ; clear update flag, call update routine
        ;

        bclr    #BIT_FX_STATE_FLAG_UPDATE,x0
        move    x0,x:(rD+FX_STATE_FLAGS)

        bsr	RVB_UpdateParameters

ProcessFrame
	bsr	RVB_ProcessFrame

	;The output of the reverb engine contains the reverb
	; component corresponding to its input signal.  For normal
	; listening the direct path needs to be added to this
	; result.


        FXExit
     
	include 'options.inc'
	include 'reverb.inc'
	include 'reverb.asm'

        IF !@DEF('STANDALONE')
	include 'Init_Params.asm'
        ELSE
        include 'fxdmautil.asm'
        ENDIF

FX_END
        nop

; ---- end MainLoop ----
			  
; **************************************************************************
;
; Chorus.h    
;
; **************************************************************************        

;
; delay line in Ymem
;

FX_DELAY_LENGTH         equ FX_STATE_YMEM_LENGTH
FX_DELAY_BASE           equ FX_STATE_YMEM_BASE


    IF @DEF('STEREO')
FX_OFFSET 	        set	(DATASIZE_DSP_FX_STATE_3IN_2OUT)
    ELSE
FX_OFFSET 	        set	(DATASIZE_DSP_FX_STATE_2IN_1OUT)
    ENDIF

CHORUS_GAIN			set FX_OFFSET
FX_OFFSET           set FX_OFFSET+1

CHORUS_MOD_SCALE_H	set FX_OFFSET
FX_OFFSET           set FX_OFFSET+1

FX_DELAY_INDEX      set FX_OFFSET
FX_OFFSET           set FX_OFFSET+1

FX_DELAY_END		set FX_OFFSET
FX_OFFSET           set FX_OFFSET+1

FX_INPUT0           set FX_OFFSET
FX_OFFSET           set FX_OFFSET+1

FX_INPUT1           set FX_OFFSET
FX_OFFSET           set FX_OFFSET+1

FX_OUTPUT0          set FX_OFFSET
FX_OFFSET           set FX_OFFSET+1

DATASIZE_CHORUS	    set FX_OFFSET

    IF @DEF('STEREO')
CHORUS_RIGHTCHANNEL_OFFSET equ DATASIZE_CHORUS+1
    ENDIF


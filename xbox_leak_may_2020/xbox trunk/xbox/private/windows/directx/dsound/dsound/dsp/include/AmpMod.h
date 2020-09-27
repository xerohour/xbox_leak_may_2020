; **************************************************************************
;
; AmpMod.h    
;
; **************************************************************************        

; Low Level Data

    IF @DEF('STEREO')

DATASIZE_AMPMOD	equ	(DATASIZE_DSP_FX_STATE_3IN_2OUT)	

    ELSE

DATASIZE_AMPMOD	equ	(DATASIZE_DSP_FX_STATE_2IN_1OUT)	

    ENDIF

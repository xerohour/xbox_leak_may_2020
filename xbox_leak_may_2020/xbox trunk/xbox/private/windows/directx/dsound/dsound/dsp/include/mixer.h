;**************************************************************************
;
;       mixer.h    
;
;**************************************************************************        



    IF @DEF('IO_2IN_1OUT')
FX_OFFSET   set DATASIZE_DSP_FX_STATE_STEREO
    ENDIF

    IF @DEF('IO_4IN_2OUT')
FX_OFFSET   set DATASIZE_DSP_FX_STATE_4IN_2OUT 
    ENDIF

GAIN_IN0    set FX_OFFSET
FXOFFSET    set FX_OFFSET+1

GAIN_IN1    set FX_OFFSET
FXOFFSET    set FX_OFFSET+1


DATASIZE_MIXER	equ	(FX_OFFSET)




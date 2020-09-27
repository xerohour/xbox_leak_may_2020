;**************************************************************************
;
;       oscillator.h    
;
;**************************************************************************        

; we support max of 8 outputs

FX_OFFSET          set (DATASIZE_DSP_FX_STATE_MONO-1)

FX_OSCILLATOR_H    set FX_OFFSET
FX_OFFSET          set FX_OFFSET+1

FX_OSCILLATOR_Y1   set FX_OFFSET
FX_OFFSET          set FX_OFFSET+1

FX_OSCILLATOR_Y2   set FX_OFFSET
FX_OFFSET          set FX_OFFSET+1

DATASIZE_OSCILLATOR  equ	(FX_OFFSET)




;**************************************************************************
;
;       spliter.h    
;
;**************************************************************************        

; we support max of 4 outputs

FX_OFFSET          set (DATASIZE_DSP_FX_STATE_1IN_4OUT-1)

FX_NUM_OUTPUTS     set FX_OFFSET
FX_OFFSET          set FX_OFFSET+1

FX_FREQ0           set FX_OFFSET
FX_OFFSET          set FX_OFFSET+4

FX_TABLE_INDEX0    set FX_OFFSET
FX_OFFSET          set FX_OFFSET+(2*4)

FX_SIN_TABLE       set FX_OFFSET
FX_OFFSET          set FX_OFFSET+65

DATASIZE_LFO       equ	(FX_OFFSET)






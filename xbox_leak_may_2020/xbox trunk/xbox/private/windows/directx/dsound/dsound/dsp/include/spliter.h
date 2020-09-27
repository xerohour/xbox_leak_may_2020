;**************************************************************************
;
;       spliter.h    
;
;**************************************************************************        

; we support max of 8 outputs

FX_OFFSET          set DATASIZE_DSP_FX_STATE_1IN_8OUT 

FX_NUM_OUTPUTS     set FX_OFFSET
FX_OFFSET          set FX_OFFSET+1


FX_OUTPUT_GAIN0    set FX_OFFSET
FX_OFFSET          set FX_OFFSET+9

FX_TEMP            set FX_OFFSET
FX_OFFSET          set FX_OFFSET+1
FX_TEMP1           set FX_OFFSET
FX_OFFSET          set FX_OFFSET+1



DATASIZE_SPLITER	equ	(FX_OFFSET)




;**************************************************************************
;
;       COMPRESSOR.h    
;
;**************************************************************************        

FX_DELAY_LENGTH         equ FX_STATE_YMEM_LENGTH
FX_DELAY_BASE           equ FX_STATE_YMEM_BASE

FX_OFFSET          set DATASIZE_DSP_FX_STATE_MONO 

FX_DELAY_INDEX     set FX_OFFSET
FX_OFFSET          set FX_OFFSET+1


FX_DELAY_END       set FX_OFFSET
FX_OFFSET          set FX_OFFSET+1

FX_OUTPUT_INDEX    set FX_OFFSET
FX_OFFSET          set FX_OFFSET+1


DATASIZE_COMPRESSOR	equ	(FX_OFFSET)




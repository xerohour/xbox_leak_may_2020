; **************************************************************************
;
; rms.h    
;
; **************************************************************************        


; memory locations and pointers

FX_OFFSET       set     (DATASIZE_DSP_FX_STATE_6IN_0OUT)    

FX_RMS_NUM_MIXBINS  set  FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1

FX_RMS_AVG_OUTPUT  set  FX_OFFSET
FX_OFFSET          set  FX_OFFSET+6

FX_RMS_MAX_OUTPUT  set  FX_OFFSET
FX_OFFSET          set  FX_OFFSET+6


DATASIZE_RMS	    equ  FX_OFFSET	

; constants

RMS_NUM_MIXBINS     equ  6

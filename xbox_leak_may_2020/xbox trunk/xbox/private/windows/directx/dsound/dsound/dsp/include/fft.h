; **************************************************************************
;
; rms.h    
;
; **************************************************************************        


; memory locations and pointers

FX_OFFSET       set     (DATASIZE_DSP_FX_STATE_1IN_1OUT)    

FX_COEFF_TABLE  set  FX_OFFSET
FX_OFFSET       set  FX_OFFSET+1

FX_




DMA_NODE_WRITE0 set FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1
DMA_NODE_WRITE1 set FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1
DMA_NODE_WRITE2 set FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1
DMA_NODE_WRITE3 set FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1
DMA_NODE_WRITE4 set FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1
DMA_NODE_WRITE5 set FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1
DMA_NODE_WRITE6 set FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1


DATASIZE_RMS	    equ  FX_OFFSET	

; constants

RMS_NUM_MIXBINS     equ  6

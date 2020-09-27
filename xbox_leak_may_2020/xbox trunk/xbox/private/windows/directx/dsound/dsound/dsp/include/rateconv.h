;**************************************************************************
;
;       RATECONV.h    
;
;**************************************************************************        

FXMEM               set DATASIZE_DSP_FX_STATE_MONO
FX_TEMP_BUFFER      equ FX_STATE_IO1_OUT0

FX_SRC_STEP         set FXMEM
FXMEM               set FXMEM+1

DMA_NODE_WRITE0     set FXMEM
FXMEM               set FXMEM+1

DMA_NODE_WRITE1     set FXMEM
FXMEM               set FXMEM+1

DMA_NODE_WRITE2     set FXMEM
FXMEM               set FXMEM+1

DMA_NODE_WRITE3     set FXMEM
FXMEM               set FXMEM+1

DMA_NODE_WRITE4     set FXMEM
FXMEM               set FXMEM+1

DMA_NODE_WRITE5     set FXMEM
FXMEM               set FXMEM+1

DMA_NODE_WRITE6     set FXMEM
FXMEM               set FXMEM+1

FX_INDEX            set FXMEM
FXMEM               set FXMEM+1




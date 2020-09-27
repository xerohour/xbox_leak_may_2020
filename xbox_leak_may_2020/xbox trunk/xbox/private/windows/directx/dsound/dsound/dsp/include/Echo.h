;**************************************************************************
;
;       Echo.h    
;
;**************************************************************************        


    IF @DEF('STEREO')
ECHO_GAIN   equ DATASIZE_DSP_FX_STATE_STEREO
    ELSE
ECHO_GAIN	equ	DATASIZE_DSP_FX_STATE_MONO 
    ENDIF

FX_DELAY_BUFFER_IN      equ (ECHO_GAIN+1)
FX_DELAY_BUFFER_OUT     equ (ECHO_GAIN+2)

DMA_NODE_READ0 equ (ECHO_GAIN+3)
DMA_NODE_READ1 equ (ECHO_GAIN+4)
DMA_NODE_READ2 equ (ECHO_GAIN+5)
DMA_NODE_READ3 equ (ECHO_GAIN+6)
DMA_NODE_READ4 equ (ECHO_GAIN+7)
DMA_NODE_READ5 equ (ECHO_GAIN+8)
DMA_NODE_READ6 equ (ECHO_GAIN+9)


DMA_NODE_WRITE0 equ (ECHO_GAIN+10)
DMA_NODE_WRITE1 equ (ECHO_GAIN+11)
DMA_NODE_WRITE2 equ (ECHO_GAIN+12)
DMA_NODE_WRITE3 equ (ECHO_GAIN+13)
DMA_NODE_WRITE4 equ (ECHO_GAIN+14)
DMA_NODE_WRITE5 equ (ECHO_GAIN+15)
DMA_NODE_WRITE6 equ (ECHO_GAIN+16)

FX_INPUT0       equ (ECHO_GAIN+17)
FX_OUTPUT0      equ (ECHO_GAIN+18)

DATASIZE_ECHO	equ	(1+FX_OUTPUT0)

    IF @DEF('STEREO')
ECHO_RIGHTCHANNEL_OFFSET equ DATASIZE_ECHO+1
    ENDIF



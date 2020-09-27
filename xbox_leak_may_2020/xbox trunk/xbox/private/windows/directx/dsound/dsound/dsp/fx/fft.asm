;**************************************************************************
;
;       fft.asm    
;       FFT and iFFT fx. Produces (using dma)
;
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF


        ; disabled dma for now since the host can get the results direct from xram
DMA_ENABLED             equ     0
CIRCULAR_DMA            equ     0
NORM_SCALE              set     (1.0/32.0)

        include 'fft.h'

        org     x:
        ;
        ; create a sin and cos table
        ;


;************************************************************
; StartFFT
;************************************************************
StartFFT
; Data structure pointer already loaded into rD

        FXEntryPoint
        IF DMA_ENABLED
        bsr     FFTPrepareDelayLineDMA
        ENDIF        

        ;
        ; the coefficient tables will be placed directly in X and Y
        ; memory from the host
        ;


ComputeFX
        
	bsr	ComputeFFT
        FXExit




;************************************************************
; ComputeFFT
;************************************************************
ComputeFFT	

        ;
        ; the computation routine process only 32 samples of data at a time
        ; the intermediate results are stored in memory.
        ; a current stage variable tells us where to pick up on the next frame
        ;

        move    #FFT_SIZE/2,n0  ; butterfiles per group
        move    #1,n2           ; groups per pass
        move    #FFT_SIZE/4,n6  ; coefficient increment
        move    -1,m0           ; linear addressing for inputs
        move    m0,m1
        move    m0,m4
        move    m0,m5
        move    #0,m6           ; bit reverse addressing for sin/cos tables

        ;
        ; triple nested DO loop for entire FFT pass
        ;

NUM_PASSES     equ     (@cvi(@log(FFT_SIZE)/(@log(2)+0.5))

        dor #NUM_PASSES, FFT_PassLoop

        move    #data,r0
        move    r0,r4                           ; initialize A input pointer
        lua     (r0)+n0,r1                      ; initialize A output pointer
        move    x:(rD+FX_COEFF_TABLE),r6        ; load table offset (init C pointer)
        move    rD,x:(rD+FX_STATE_PTR)          ; save state pointer r5
        lua     (r1)-1,r5                        ; initialize B output pointer
        move    n0,n1                           ; initialize pointer offsets
        move    n0,n4
        move    n0,n5

        dor n2,FFT_GroupLoop

        move    x:(r1),x1       y:(r6),y0       ; lookup -sine and -cos values
        move    x:(r5),a        y:(r0),b        ; preload data
        move    x:(r6)+n6,x0                    ; update C pointer

        do  n0,FFT_BfyLoop

        mac     x1,y0,b         y:(r1)+,y1
        macr    -x0,y1,b        a,x:(r5)+       y:(r0),a
        subl    b,a             b,x:(r4)+       y:(r0),b

FFT_BfyLoop

        move    a,x:(r5)+n5     y:(r1)+n1,y1    ; update A and B pointers
        move    x:(r0)+n0,x1    y:(r4)+n4,y1

FFT_GroupLoop

        move    n0,b1
        lsr     b               n2,a1
        lsl     a               b1,n0
        move    a1,n2

FFT_PassLoop

        rts





FFT_BfyLoop

FFT_GroupLoop

FFT_PassLoop



        IF DMA_ENABLED
        ; Write output values into delay line
    	lua     (rD+DMA_NODE_WRITE0),r0
        move    r0,a
        IF !@DEF('SIM')
	bsr    DMAStartNode_Wait
        ENDIF

        ENDIF


        rts

    IF DMA_ENABLED
RMSPrepareDelayLineDMA

    ;
    ; prepare write dma block
    ;

    ; dma block location
    lua	    (rD+DMA_NODE_WRITE0),r0
    ; dsp mem address to read RMS results from
    lua     (rD+FX_RMS_TEMP_OUTPUT),r1
    ; system memory address to stick it in
    move    x:(rD+FX_STATE_SCRATCH_BASE),r2
    ; size of dma in words
    move    x:(rD+FX_RMS_NUM_MIXBINS),r3

    ;
    ; prepare the dma words
    ;

    bsr     DMANewNode_Write_Eol     
   
    rts
    ENDIF

    IF @DEF('STANDALONE')
    include "DMAUtil.asm"
    ENDIF

FX_END
        nop

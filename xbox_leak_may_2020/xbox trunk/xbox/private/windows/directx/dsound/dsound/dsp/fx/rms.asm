;**************************************************************************
;
;       rms.asm    
;       simple 'fx' that calculates the average value of eac 32 sample frame	
;       and outputs it using DMA to system memory (one dword for each mixbin)
;
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF


        ; disabled dma for now since the host can get the results direct from xram
NORM_SCALE              set     (1.0/32.0)

        include 'rms.h'

;************************************************************
; StartRMS
;************************************************************
StartRMS
; Data structure pointer already loaded into rD

        FXEntryPoint
        
ComputeFX
        
	bsr	ComputeRMS	
        FXExit


;************************************************************
; ComputeRMS
;************************************************************
ComputeRMS	

	lua	(rD+FX_STATE_IO6_IN0),r0
        lua     (rD+FX_RMS_AVG_OUTPUT),r1
        lua     (rD+FX_RMS_MAX_OUTPUT),r2
        move    x:(rD+FX_RMS_NUM_MIXBINS),x0

        ; use a circular buffer to store the last N values


        dor     x0,RMS_MIXBIN_LOOP

        move    x:(r0)+,r4

        ;
        ; create an RMS output out of the current 32 samples in the 6 speaker mixbins
        ; the RMS output will be DMAed back in system memory taking up 6 consecutive DWORDs
        ; of scratch space
        ;

        bsr     AccX

        ; save result, increment pointers

        move    a,x:(r1)+    

        ; increment r2
        move    x:(r2)+,a

RMS_MIXBIN_LOOP

        rts



AccX 
    ; zero out prev. frame cummulative value
    clr     a
    dor     #kFrameLength,AccX_Loop             

    move    x:(r4)+,b
    abs     b

    ; save A and B real quick.
    move    b,x0
    move    a,y0

    ; put current sample into A
    move    b,a

    ; compare this value with the maximum
    move    x:(r2),b
    max     a,b

    ;update maximum
    move    b,x:(r2)

    ; restore a
    move    y0,a

    move    #NORM_SCALE,y0
    mac     x0,y0,a

AccX_Loop


    rts

FX_END
        nop

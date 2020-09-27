;**************************************************************************
;
;       LFO.asm    
;       	
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'osci_table.h'

FREQ_CONVERSION set     1.0/4500.0
GAIN            set     0.5

;************************************************************
; StartLFO	
;************************************************************
StartLFO
; Data structure pointer already loaded into rD

        FXEntryPoint

ComputeFX
        
	bsr	ComputeLFO
        FXExit



;************************************************************
; ComputeOscillator
; split channel 0 to channels 0,1,2,3,.., N of output data
;************************************************************
ComputeLFO

        ; load base of table
        lua     (rD+FX_SIN_TABLE),r2


        ; load base address of output IO ptr array
        lua	(rD+FX_STATE_IO1_OUT0),r1


        ; load output IO ptr
        move    x:(r1)+,r4

        ; load current index       
        move    x:(rD+FX_TABLE_INDEX),a0
        move    x:(rD+FX_TABLE_INDEX+1),a1

        dor     #kFrameLength,FrameLoop

        ; retrieve base ptr of table
        move    r2,b
        add     a,b

        move    b1,r3

        move    x:(r3),x0
        move    x0,x:(r4)+

        move    x:(rD+FX_FREQ0),x0
        move    #FREQ_CONVERSION,y0
        mpy     x0,y0,b

        add     b,a
        and     #$3f,a


FrameLoop
        

        ; save integer part (high order bits of a) as the index
        move    a0,x:(rD+FX_TABLE_INDEX)
        move    a1,x:(rD+FX_TABLE_INDEX+1)

        nop 
        rts

FX_END
        nop

;**************************************************************************
;
;       LFO.asm    
;       	
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'osci_table.h'

; since our table is 64 elements, one full pass through it generates a 48000/64=750 hz sine wave
; to allow the caller to specify small increments, dive their FX_FREQ0 param by 7500 so the units
; are in tenths of a Hz

FREQ_CONVERSION set     1.0/7500.0
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
        lua	(rD+FX_STATE_IO0_OUT0),r1
        ; load index base
        lua	(rD+FX_TABLE_INDEX0),r6
        ; load freq factor
        lua	(rD+FX_FREQ0),r7

        move    x:(rD+FX_NUM_OUTPUTS),x0

        dor     x0,OutputsLoop

        ; load output IO ptr
        move    x:(r1)+,r4

        ; load current index       
        move    x:(r6),a0
        move    x:(r6+1),a1

        dor     #kFrameLength,FrameLoop

        ; retrieve base ptr of table
        move    r2,b
        add     a,b

        move    b1,r3

        move    x:(r3),x0
        move    x0,x:(r4)+

        ; load frequency
        move    x:(r7),x0
        move    #FREQ_CONVERSION,y0
        mpy     x0,y0,b

        add     b,a
        and     #$3f,a


FrameLoop        

        ; save integer part (high order bits of a) as the index
        move    a0,x:(r6)+
        move    a1,x:(r6)+
        ;increment freq base
        move    x:(r7)+,x0

OutputsLoop

        nop 
        rts

FX_END
        nop

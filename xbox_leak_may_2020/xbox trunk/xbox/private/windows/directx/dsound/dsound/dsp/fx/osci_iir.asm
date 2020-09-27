;**************************************************************************
;
;       osci.asm    
;       Generate sin from second order IIR
;       y[n] = h*y[n-1] - y[n-2] 
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'osci.h'


;************************************************************
; StartLFO	
;************************************************************
StartOscillator
; Data structure pointer already loaded into rD

        FXEntryPoint
        
ComputeFX
        
	bsr	ComputeOscillator

        FXExit



;************************************************************
; ComputeOscillator
;************************************************************
ComputeOscillator

	move	x:(rD+FX_STATE_IO0_OUT0),r3

        move    x:(rD+FX_OSCILLATOR_H),x0
        move    x:(rD+FX_OSCILLATOR_Y1),y0  
        move    x:(rD+FX_OSCILLATOR_Y2),b  

	dor	#kFrameLength,LoopEnd

        ;  y[n] = h*y[n-1] - y[n-2] 
	mpy     x0,y0,a                 ; Get  h*y1 in 2:14 format
        subl    b,a		        ; Get (h*y1 - y2)= y in 
                                        ; fractional format
        move    a,x:(rD+FX_OSCILLATOR_Y1)  
        move    y0,x:(rD+FX_OSCILLATOR_Y2)             

        move	a,x:(r3)+

LoopEnd

        rts


FX_END
        nop

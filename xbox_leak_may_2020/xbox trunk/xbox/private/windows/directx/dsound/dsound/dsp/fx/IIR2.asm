;**************************************************************************
;
; IIR2.asm    2nd Order, IIR filter module
;
;**************************************************************************        

        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'iir2.h'

        ;
        ; worst case cycle usage by this effect (includes one time Init)
        ; stereo: 
        ; mono: 
        ;

IIR2_Default_Frequency	set 4500.0	; hertz
IIR2_Default_Q		set 1.0
IIR2_Default_GainDB	set 15.0
IIR2_Default_Mode	set 0		

Fc			set (IIR2_Default_Frequency/Fs)
Wc			set (IIR2_Default_Frequency/Fs)*kTwoPi
d			set 2.0*@tan(Wc/(2.0*IIR2_Default_Q))/@sin(Wc)

t			set 1.0/2.0*d*@sin(Wc)
beta		        set 0.5*((1.0-t)/(1.0+t))
gamma		        set (0.5+beta)*@cos(Wc)
alpha		        set (0.5+beta-gamma)/4.0
scale24		        set 1.0

iir2_H_LowPass1_b0      set (alpha*scale24)
iir2_H_LowPass1_b1      set (2.0*alpha*scale24)
iir2_H_LowPass1_b2      set (alpha*scale24)
iir2_H_LowPass1_a1      set (gamma*scale24)
iir2_H_LowPass1_a2      set (-beta*scale24)

StartIIR2 

        FXEntryPoint

ComputeFX
	bsr	ComputeIIR2	

        ;
        ; FX exit point
        ;

        FXExit


;************************************************************
; ComputeIIR2	5 MAC, Direct Form IIR, Order 2
;					Ok for in-place operation
;************************************************************
ComputeIIR2	
; rD points to Data Structure 

	move	x:(rD+FX_STATE_IO1_IN),r2
	move	x:(rD+FX_STATE_IO1_OUT0),r3
        move    #>kFrameLength,x0
	dor	x0,ComputeIIR2_Loop

	ori     #$08,mr         ; Set scaling mode to scale up
		    		; when data is moved from accumulator

        ; Do 5 multiply accumulates
        ; y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] + a1*y[n-1] + a2*y[n-2]

	lua	(rD+IIR2_H_B0),r0
	move	x:(rD+FX_DELAY_BASE),r4                 ; load y-mem offset of our delay elements
       
	move	x:(r0)+,x0				
	move	x:(r2)+,y1				
	mpy	x0,y1,a	x:(r0)+,x0 y:(r4)+,y0		; b0*x[n]
        rep     #3
	mac    	x0,y0,a x:(r0)+,x0 y:(r4)+,y0		; b1*x[n-1], b2*x[n-2], a1*y[n-1]
	mac    	x0,y0,a					; a2*y[n-2]        

        move	x:(rD+FX_DELAY_BASE),r4
        ; Shift delay elements
	move	y:(r4+IIR2_Z_X1),x0
	move	x0,y:(r4+IIR2_Z_X2)
	move	y1,x0
	move	x0,y:(r4+IIR2_Z_X1)

	move	y:(r4+IIR2_Z_Y1),x0
	move	x0,y:(r4+IIR2_Z_Y2)
	move	a,x0
	move	x0,y:(r4+IIR2_Z_Y1)

        andi    #$f7,mr         ; Unset scaling mode

        ; check if we need to mix our output
        move    x:(rD+FX_STATE_FLAGS),x1       
        brclr   #BIT_FX_STATE_FLAG_GLOBAL_FX,x1,STOMP_OUTPUT
        move	x:(r3),x0
        add     x0,a            ; mix result to audio Output 
STOMP_OUTPUT

	move	a,x:(r3)+

ComputeIIR2_Loop

        rts
; ---- end ComputeIIR2	----

FX_END
        nop


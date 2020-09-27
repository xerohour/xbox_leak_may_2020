;**************************************************************************
;
; Distortion.asm    Distortion module
;       
;**************************************************************************        
        IF @DEF('STANDALONE')
        include 'util.h'
        ENDIF

        include 'distortion.h'

        ;
        ; worst case cycle usage by this effect (includes one time Init)
        ; stereo: 
        ; mono: 11510 
        ;


Distortion_Default_Gain			set	(110.0/256.0)	
Distortion_Default_Edge			set	1.0
Distortion_Default_PostFilter_Frequency	set	1000.0	; Hertz
Distortion_Default_PostFilter_Q		set	10.0	
Distortion_Default_PreFilter_Frequency	set	2000.0	; Hertz	

eqFc		set (Distortion_Default_PostFilter_Frequency/Fs)
eqWc		set (Distortion_Default_PostFilter_Frequency/Fs)*kTwoPi
lpFc		set (Distortion_Default_PreFilter_Frequency/Fs)
lpWc		set (Distortion_Default_PreFilter_Frequency/Fs)*kTwoPi
EQScale24	set 1.0
g24		set 1.0

; IIR2: mode=Parametric fc=1000 Q=10 gainDB=10 h=< 0.50430103 -0.98592954 0.49172073 0.98592954 -0.49602176 >,< 408CEF 81CD10 3EF0B4 7E32F0 C0825C >
; Fc/Fs=0.0226757 Wc/Fs=0.142476 (123CA6) coefficientScale=0.5
postFilter_H_Parametric_f01000_q10_b0    set  0.50430103       ; 408CEF
postFilter_H_Parametric_f01000_q10_b1    set -0.98592954       ; 81CD10
postFilter_H_Parametric_f01000_q10_b2    set  0.49172073       ; 3EF0B4
postFilter_H_Parametric_f01000_q10_a1    set  0.98592954       ; 7E32F0
postFilter_H_Parametric_f01000_q10_a2    set -0.49602176       ; C0825C

; IIR2: mode=LowPass fc=2000 Q=1 gainDB=0 h=< 0.01758625 0.03517250 0.01758625 0.83705819 -0.37223069 >,< 024044 048088 024044 6B24B9 D05ABF >
; Fc/Fs=0.0453515 Wc/Fs=0.284952 (24794C)
preFilter_H_LowPass_f02000_q01_b0    set  0.01758625       ; 024044
preFilter_H_LowPass_f02000_q01_b1    set  0.03517250       ; 048088
preFilter_H_LowPass_f02000_q01_b2    set  0.01758625       ; 024044
preFilter_H_LowPass_f02000_q01_a1    set  0.83705819       ; 6B24B9
preFilter_H_LowPass_f02000_q01_a2    set -0.37223069       ; D05ABF

StartDistortion 

        FXEntryPoint

; Data structure pointer already loaded into rD

	move	#>Distortion_Default_Gain,x0
	move	x0,x:(rD+DISTORTION_GAIN)	

        ; delay elements are initialized to zero already(state always is 0 at init)

ComputeFX
	bsr	ComputeDistortion	

        FXExit

ComputeDistortion	
; rD points to Data Structure 

	move	x:(rD+FX_STATE_IO1_IN),r2
	move	x:(rD+FX_STATE_IO1_OUT0),r3

	dor	#>kFrameLength,ComputeDistortion_Loop
ComputeDistortion_PreFilter
	ori     #$08,mr         ; Set scaling mode to scale up
				; when data is moved from accumulator

        ; y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] + a1*y[n-1] + a2*y[n-2]
	lua	(rD+DISTORTION_PREFILTER_H_B0),r0

        ; load y-mem offset of our delay elements
        ; prefilter is at the base of our yram allocation
	move	x:(rD+FX_DELAY_BASE),r4 

	move	x:(r0)+,x0				
	move	x:(r2)+,y1				
	mpy	x0,y1,a	x:(r0)+,x0 y:(r4)+,y0		; b0*x[n]
        rep #3
	mac	x0,y0,a x:(r0)+,x0 y:(r4)+,y0     	; b1*x[n-1]+b2*x[n-2]+a1*y[n-1]+
	mac	x0,y0,a					; a2*y[n-2]

        ;
        ; save existing output for next stage
        ;

        move    x:(r3),x0
        move    x0,x:(rD+DISTORTION_TEMP)
	move	a,x:(r3)			

        ; Shift data
        move	x:(rD+FX_DELAY_BASE),r4 

	move	y:(r4+DISTORTION_PREFILTER_Z_X1),x0
	move	x0,y:(r4+DISTORTION_PREFILTER_Z_X2)
	move	y1,x0
	move	x0,y:(r4+DISTORTION_PREFILTER_Z_X1)

	move	y:(r4+DISTORTION_PREFILTER_Z_Y1),x0
	move	x0,y:(r4+DISTORTION_PREFILTER_Z_Y2)
	move	a,x0
	move	x0,y:(r4+DISTORTION_PREFILTER_Z_Y1)
        andi    #$f7,mr         ; Unset scaling mode

ComputeDistortion_Clipping
	
	move	x:(r3),a         ; read from output, to get results of prefilter
	asl	#4,a,a
	move	a,x1

	move	x:(rD+DISTORTION_GAIN),y0
	mpy	x1,y0,a			; Scale down by gain
	move	a,x:(r3)

ComputeDistortion_PostFilter

        ; y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] + a1*y[n-1] + a2*y[n-2]
	lua	(rD+DISTORTION_POSTFILTER_H_B0),r0

        move	x:(rD+FX_DELAY_BASE),a
        move    #>DISTORTION_POSTFILTER_Z_X1,x0	
        add     x0,a
        move    a,r4               

	ori     #$08,mr         ; Set scaling mode to scale up
				; when data is moved from accumulator

	move	x:(r0)+,x0				
	move	x:(r3),y1		; read previous stage results		
	mpy	x0,y1,a	x:(r0)+,x0 y:(r4)+,y0    	; b0*x[n]

        rep     #3
	mac	x0,y0,a x:(r0)+,x0 y:(r4)+,y0     	
	mac	x0,y0,a					

        ; Shift delay elements
        move	x:(rD+FX_DELAY_BASE),r4 
	move	y:(r4+DISTORTION_POSTFILTER_Z_X1),x0
	move	x0,y:(r4+DISTORTION_POSTFILTER_Z_X2)
	move	y1,x0
	move	x0,y:(r4+DISTORTION_POSTFILTER_Z_X1)

	move	y:(r4+DISTORTION_POSTFILTER_Z_Y1),x0
	move	x0,y:(r4+DISTORTION_POSTFILTER_Z_Y2)
	move	a,x0
	move	x0,y:(r4+DISTORTION_POSTFILTER_Z_Y1)

        andi    #$f7,mr				; Unset scaling mode

        move    x:(rD+FX_STATE_FLAGS),x1       
        brclr   #BIT_FX_STATE_FLAG_GLOBAL_FX,x1,STOMP_OUTPUT
        move	x:(rD+DISTORTION_TEMP),x0
        add     x0,a            ; mix result to audio Output 
STOMP_OUTPUT
        move    a,x:(r3)+

	nop		; DSP restriction:  can't change scaling mode too close to loop end
	nop
	nop
ComputeDistortion_Loop

ComputeDistortionEnd
        rts

FX_END
        nop


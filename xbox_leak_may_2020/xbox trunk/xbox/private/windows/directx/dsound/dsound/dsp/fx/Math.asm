;**************************************************************************
;
; Math.asm    
;       
; 
;		Written by Gints Klimanis
;
; Assembler provides:
; ACS - Arc cosine
; ASN - Arc sine
; AT2 - Arc tangent
; ATN - Arc tangent
; COH - Hyperbolic cosine
; COS - Cosine
; SIN - Sine
; SNH - Hyperbolic sine
; TAN - Tangent
; TNH - Hyperbolic tangent

; L10 - Log base 10
; LOG - Natural logarithm
; POW - Raise to a power
; XPN - Exponential function
; SQT - Square root

; ABS - Absolute value
; CEL - Ceiling function
; FLR - Floor function
; MAX - Maximum value
; MIN - Minimum value
; RND - Random value
; SGN - Return sign

; CVF - Convert integer to floating point
; CVI - Convert floating point to integer
; CVS - Convert memory space
; FLD - Shift and mask operation
; FRC - Convert floating point to fractional
; LFR - Convert floating point to long fractional
; LNG - Concatenate to double word
; LUN - Convert long fractional to floating point
; RVB - Reverse bits in field
; UNF - Convert fractional to floating point 

;**************************************************************************        
	list  

;*******************************************************************
;DivideA
;  Input:  y1  and  a
; Output:  a=a/y1
;************************************************************
DivideA
	and     #$fe,ccr
	rep     #$18
	div     y1,a
	move    a0,a1
	move    #0,a2

	rts
; ---- end DivideA ----

;*******************************************************************
;DivideB
;  Input:  y1  and  b
; Output:  b=b/y1
;************************************************************
DivideB
	and     #$fe,ccr
	rep     #$18
	div     y1,b
	move    b0,b1
	move    #0,b2

	rts
; ---- end DivideB ----

;***********************************************************************
;  Sqrt							       *	
;	Full 23 bit precision square root routine using                *
;       a successive approximation technique.		               * 		
; SQRT Macro
; y  = double precision (48 bit) positive input number
; b  = 24 bit output root
;
; a  = temporary storage
; x0 = guess
; x1 = bit being tested
; y1:y0 = input number
;***********************************************************************
;sqrt3   macro
;sqrt3   ident   1,0
; 
;sqrt3
;        clr     b    #<$40,x0           ; init root and guess
;        move          x0,x1             ; init bit to tes
;  do    #23,_endl                       ;START OF LOOP
;        mpy     -x0,x0,a                ; square and negate the guess
;        add     y,a                     ; compare to double precision input
;        tge     x0,b                    ; update root if input >= guess
;        mpy    x1,#1,a       
;        nop
;        add     b,a     a,x1            ; form new guess
;        nop
;        move                    a,x0            ; save new guess
;        
;_endl                                    ;END OF LOOP
;        endm
; ---- end Sqrt ----

;************************************************************
; Exp        x       (log2ofE * x)
;           e  =   2
;************************************************************
Exp 
; Operand in x0

; x         *= (float) kLog2_E;
; exp2Approx = 0.1713425f*x*x + 0.6674432f*x + 0.9979554f;
;                 a2            a1           a0


; 0.1713425*(kLog2_E * kLog2_E) = 0.3566269646
; 0.6674432*(kLog2_E)           = 0.9629169947
; expApprox = 0.3566269646*x*x + 0.9629169947*x + 0.9979554f;

        mpyr    x0,x0,a			;x**2
		move   #>.3566269646,y0
		move	a,x1
		mpyr    x1,y0,a		; Have a2*x*x
		move	a,x1

		move   #>.9629169947,y0
        mpyr    x0,y0,a			; Have a1*x
        add     x1,a            	; Add a2*x^2

		move   #>.9979554,y0
        add     y0,a           		; Add a0

 rts
; ---- end Exp ----

;************************************************************
; Exp2        x       
;           2  = .1713425 x*x + .6674432 x + .9979554
;
; Polynomial approximation, 8 bit accuracy.
;      for -1 < x <= 0
;
;       Input in x0, output in A
;************************************************************
Exp2 

;       2^x =.1713425*x^2 + .6674432*x + .9979554
;                 a2            a1           a0

        mpyr    x0,x0,a			;x**2
		move   #>.1713425,y0
		move	a,x1
		mpyr    x1,y0,a			; Have a2*x*x
		move	a,x1

		move   #>.6674432,y0
        mpyr    x0,y0,a			; Have a1*x
        add     x1,a            ; Add a2*x^2

		move   #>.9979554,y0
        add     y0,a           ; Add a0

 rts
; ---- end Exp2 ----

;************************************************************
; Sig24div		a/x0, where a >= x0
;
;		Quotient in x1, remainder in b1
;************************************************************
Sig24div
	abs 	a	a,b
	eor	x0,b b,x:$0
	and 	#$fe,ccr
	rep	#$18
	div	x0,a
	tfr	a,b
	jpl	Sig24div_savequo
	neg	b
Sig24div_savequo
	tfr	x0,b	b0,x1
	abs	b
	add	a,b
	jclr 	#23,x:$0,Sig24div_done
	move	#$0,b0
	neg	b
Sig24div_done

	rts
; ---- end Sig24div ----

;************************************************************
; nToSintSfrac		Convert negative 56-bit number in a
;			to signed integer in a1 and
;			signed fraction in b1
;
;************************************************************
nToSintSfrac
	abs 	a	
	lsl	a
	asr	a
	move	a0,b
	neg	b #0,a0
	neg	a

	rts
; ---- end nToSintSfrac ----

;************************************************************
; pToSintSfrac		Convert positive 56-bit number in a
;			to signed integer in a1 and
;			signed fraction in b1
;
;************************************************************
pToSintSfrac
	lsl	a
	asr	a
	move	a0,b
	move	#0,a0

	rts
; ---- end pToSintSfrac ----

;************************************************************
; SinX        Sine polynomial approximation
;                                   2           3             4            5
; sin(x) = 3.140625*x + 0.02026367*x -5.325196*x + 0.5446778*x - 1.800293*x
;
;  5.325196*(0.589767f,  0.003805f  , -1.000000f,  0.102283f , -0.338071f)
;
;      for 0 <= x <= 1.0
;
;       Input in x0, output in A
;************************************************************
SinX  

SinX_Quad1or2
	move	#>0.5,a
	cmp	x0,a
	bge	Sinx_ComputePoly
	move	#>1.0,a
	sub	x0,a
	move	a,x0

Sinx_ComputePoly
	move	#>0.589767,y0
	move	x0,x1		; x
	mpy	x0,y0,a		

	mpy	x0,x1,b
	move	b,x1		; x*x
	move	#>0.003805,y1
	mac	x1,y1,a

	mpy	x0,x1,b
	move	b,x1		; x*x*x
	move	#>-1.000000,y1
	mac	x1,y1,a

	mpy	x0,x1,b
	move	b,x1		; x*x*x*x
	move	#>0.102283,y1
	mac	x1,y1,a

	mpy	x0,x1,b
	move	b,x1		; x*x*x*x*x
	move	#>-0.338071,y1
	mac	x1,y1,a

;  Now, scale to output range with k=5.325196 = (4.0 + 1.0 + 0.325196)
SineX_ReScale
	move	a,x0
	asl	#2,a,a
	add	x0,a
	move	#>0.325196,y1
	mac	x0,y1,a

	rts	
; ---- end SinX() ----


;************************************************************
; CosX        Cosine polynomial approximation
;
;      for 0 <= x <= 1.0
;
;       Input in x0, output in A
;************************************************************
CosX  
;if (x > 0.5f)
;	y = -Sin(x-0.5f);
;else
;	y = Sin(x+0.5f);
	move	#>0.5,a
	cmp	x0,a
	bge	CosX_Quad1

CosX_Quad2
	neg	a
	add	x0,a	; x-0.5
	move	a,x0
	jsr	SinX
	neg	a
	bra	CosX_End
CosX_Quad1
	add	x0,a	; x+0.5
	move	a,x0
	jsr	SinX

CosX_End
	rts	
; ---- end CosX() ----



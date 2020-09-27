;**************************************************************************
;
;       IIR1.h    1st Order, IIR filter module
;       
;		Written by Gints Klimanis
;
;**************************************************************************        
    

; High Level parameters
;DATASIZE_IO1 = 2
IIR1_LENGTH		equ	2

IIR1_FREQUENCY		equ	3
IIR1_MODE			equ	4

; Low Level Data
IIR1_H_B0	equ		5
IIR1_H_B1	equ		6
IIR1_H_A1	equ		7

IIR1_Z_X1	equ		8
IIR1_Z_Y1	equ		9

DATA_SIZE_IIR1	equ	(1+IIR1_Z_Y1)


iir1_H_LowPass_PassThru_b0    set 1.0       
iir1_H_LowPass_PassThru_b1    set 0.000000       
iir1_H_LowPass_PassThru_a1    set 0.0

;IIR1: fc=125 LowPass h=< 0.0176518 0 -0.982348 >,< 02426A 000000 82426B >
; Fc/Fs=0.00283447 Wc/Fs=0.0178095 (024794) radius=0.982348 (7DBD95)
iir1_H_LowPass_0125_b0    set 0.017652       ; 02426A
iir1_H_LowPass_0125_b1    set 0.000000       ; 000000
iir1_H_LowPass_0125_a1    set 0.982348       ; 82426B

;IIR1: fc=250 LowPass h=< 0.0349921 0 -0.965008 >,< 047A9E 000000 847A9F >
; Fc/Fs=0.00566893 Wc/Fs=0.035619 (048F29) radius=0.965008 (7B8561)
iir1_H_LowPass_0250_b0    set 0.034992       ; 047A9E
iir1_H_LowPass_0250_b1    set 0.000000       ; 000000
iir1_H_LowPass_0250_a1    set 0.965008       ; 847A9F

;IIR1: fc=500 LowPass h=< 0.0687597 0 -0.93124 >,< 08CD1E 000000 88CD1E >
; Fc/Fs=0.0113379 Wc/Fs=0.0712379 (091E53) radius=0.93124 (7732E2)
iir1_H_LowPass_0500_b0    set 0.068760       ; 08CD1E
iir1_H_LowPass_0500_b1    set 0.000000       ; 000000
iir1_H_LowPass_0500_a1    set 0.931240       ; 88CD1E

;IIR1: fc=1000 LowPass h=< 0.132792 0 -0.867208 >,< 10FF4F 000000 90FF50 >
; Fc/Fs=0.0226757 Wc/Fs=0.142476 (123CA6) radius=0.867208 (6F00B0)
iir1_H_LowPass_1000_b0    set 0.132792       ; 10FF4F
iir1_H_LowPass_1000_b1    set 0.000000       ; 000000
iir1_H_LowPass_1000_a1    set 0.867208       ; 90FF50

;IIR1: fc=2000 LowPass h=< 0.247949 0 -0.752051 >,< 1FBCCE 000000 9FBCCF >
; Fc/Fs=0.0453515 Wc/Fs=0.284952 (24794C) radius=0.752051 (604331)
iir1_H_LowPass_2000_b0    set 0.247949       ; 1FBCCE
iir1_H_LowPass_2000_b1    set 0.000000       ; 000000
iir1_H_LowPass_2000_a1    set 0.752051       ; 9FBCCF

;IIR1: fc=125 HighPass h=< 0.982348 0 0.0176518 >,< 7DBD95 000000 02426A >
; Fc/Fs=0.00283447 Wc/Fs=0.0178095 (024794) radius=0.982348 (7DBD95)
iir1_H_HighPass_0125_b0    set 0.982348       ; 7DBD95
iir1_H_HighPass_0125_b1    set 0.000000       ; 000000
iir1_H_HighPass_0125_a1    set -0.017652      ; 02426A

;IIR1: fc=250 HighPass h=< 0.965008 0 0.0349921 >,< 7B8561 000000 047A9E >
; Fc/Fs=0.00566893 Wc/Fs=0.035619 (048F29) radius=0.965008 (7B8561)
iir1_H_HighPass_0250_b0    set 0.965008       ; 7B8561
iir1_H_HighPass_0250_b1    set 0.000000       ; 000000
iir1_H_HighPass_0250_a1    set -0.034992      ; 047A9E

;IIR1: fc=500 HighPass h=< 0.93124 0 0.0687597 >,< 7732E2 000000 08CD1E >
; Fc/Fs=0.0113379 Wc/Fs=0.0712379 (091E53) radius=0.93124 (7732E2)
iir1_H_HighPass_0500_b0    set 0.931240       ; 7732E2
iir1_H_HighPass_0500_b1    set 0.000000       ; 000000
iir1_H_HighPass_0500_a1    set -0.068760      ; 08CD1E

;IIR1: fc=1000 HighPass h=< 0.867208 0 0.132792 >,< 6F00B0 000000 10FF4F >
; Fc/Fs=0.0226757 Wc/Fs=0.142476 (123CA6) radius=0.867208 (6F00B0)
iir1_H_HighPass_1000_b0    set 0.867208       ; 6F00B0
iir1_H_HighPass_1000_b1    set 0.000000       ; 000000
iir1_H_HighPass_1000_a1    set -0.132792      ; 10FF4F

;IIR1: fc=2000 HighPass h=< 0.752051 0 0.247949 >,< 604331 000000 1FBCCE >
; Fc/Fs=0.0453515 Wc/Fs=0.284952 (24794C) radius=0.752051 (604331)
iir1_H_HighPass_2000_b0    set 0.752051       ; 604331
iir1_H_HighPass_2000_b1    set 0.000000       ; 000000
iir1_H_HighPass_2000_a1    set -0.247949      ; 1FBCCE




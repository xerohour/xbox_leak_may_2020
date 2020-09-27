;**************************************************************************
;
; Equalizer.h    IIR Tri-Parameter Equalizer module
;       
;		Written by Gints Klimanis
;
;**************************************************************************        

; typedef struct _DSFXParamEq
; {
;     FLOAT   fCenter;
;     FLOAT   fQ;		// was fBandwidth
;     FLOAT   fGain;
; } DSFXParamEq, *LPDSFXParamEq;


; High Level parameters
;DATASIZE_IO1 = 2
EQUALIZER_LENGTH	equ 2

EQUALIZER_FREQUENCY	equ	3
EQUALIZER_Q			equ	4
EQUALIZER_GAINDB	equ	5
EQUALIZER_MODE		equ	6

; Low Level parameters
EQUALIZER_H_B0	equ	7
EQUALIZER_H_B1	equ	8
EQUALIZER_H_B2	equ	9
EQUALIZER_H_A1	equ	10
EQUALIZER_H_A2	equ	11

EQUALIZER_Z_X1	equ	12
EQUALIZER_Z_X2	equ	13
EQUALIZER_Z_Y1	equ	14
EQUALIZER_Z_Y2	equ	15

DATA_SIZE_EQUALIZER	equ	(1+EQUALIZER_Z_Y2)

;**************************************************************************
;
;       IIIR.h    Interpolated IIR module
;       
;		Written by Gints Klimanis
;
;**************************************************************************        


; High Level Data
;DATASIZE_IO1 = 2
IIIR_LENGTH			equ 2

IIIR_DELAY_LENGTH	equ	3
IIIR_GAIN			equ	4
IIIR_TYPE			equ	5

; Low Level Data
IIIR_DELAY_BASE		equ	6
IIIR_DELAY_END		equ	7
IIIR_DELAY_INDEX	equ	8
IIIR_DELAY_LENGTHM1	equ	9
IIIR_DELAY_INDEX2	equ	10

DATA_SIZE_IIIR	equ	(1+IIIR_DELAY_INDEX2)

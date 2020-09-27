;**************************************************************************
;
; IIR2.h    2nd Order, IIR filter module
;       
;**************************************************************************        
    
FX_DELAY_BASE           equ FX_STATE_YMEM_BASE


IIR2_H_B0	equ	(DATASIZE_DSP_FX_STATE_MONO)
IIR2_H_B1	equ	(DATASIZE_DSP_FX_STATE_MONO+1)
IIR2_H_B2	equ	(DATASIZE_DSP_FX_STATE_MONO+2)
IIR2_H_A1	equ	(DATASIZE_DSP_FX_STATE_MONO+3)
IIR2_H_A2	equ	(DATASIZE_DSP_FX_STATE_MONO+4)

DATASIZE_IIR2	equ	(1+IIR2_H_A2)

;
; the following delay variables are kept in Y RAM
;

IIR2_Z_X1	equ	0
IIR2_Z_X2	equ	1
IIR2_Z_Y1	equ	2
IIR2_Z_Y2	equ	3



	include 'IIR2_LowPass.h'
	include 'IIR2_BandPass.h'
	include 'IIR2_BandStop.h'
	include 'IIR2_Equalizer.h'


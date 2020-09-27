;**************************************************************************
;
; Distortion.h    Distortion module
;       
;
;
;**************************************************************************        

; DX8 struct 
; typedef struct _DSFXDistortion
; {
;     FLOAT   fGain;			
;     FLOAT   fOverDrive;
;     FLOAT   fPostEQCenterFrequency;
;     FLOAT   fPostEQQ;
;     FLOAT   fPostEQGain;
;    FLOAT   fPreLowpassCutoff;
; } DSFXDistortion, *LPDSFXDistortion;


FX_DELAY_BASE                   equ FX_STATE_YMEM_BASE

DISTORTION_GAIN					equ	DATASIZE_DSP_FX_STATE_MONO

; Pre filter (DX8 is low pass)
DISTORTION_PREFILTER_H_B0	equ	(DATASIZE_DSP_FX_STATE_MONO+1)
DISTORTION_PREFILTER_H_B1	equ	(DATASIZE_DSP_FX_STATE_MONO+2)
DISTORTION_PREFILTER_H_B2	equ	(DATASIZE_DSP_FX_STATE_MONO+3)
DISTORTION_PREFILTER_H_A1	equ	(DATASIZE_DSP_FX_STATE_MONO+4)
DISTORTION_PREFILTER_H_A2	equ	(DATASIZE_DSP_FX_STATE_MONO+5)

; Post filter (DX8 is parametric equalizer)
DISTORTION_POSTFILTER_H_B0	equ	(DATASIZE_DSP_FX_STATE_MONO+6)
DISTORTION_POSTFILTER_H_B1	equ	(DATASIZE_DSP_FX_STATE_MONO+7)
DISTORTION_POSTFILTER_H_B2	equ	(DATASIZE_DSP_FX_STATE_MONO+8)
DISTORTION_POSTFILTER_H_A1	equ	(DATASIZE_DSP_FX_STATE_MONO+9)
DISTORTION_POSTFILTER_H_A2	equ	(DATASIZE_DSP_FX_STATE_MONO+10)

DISTORTION_TEMP             equ (DATASIZE_DSP_FX_STATE_MONO+11)

DATASIZE_DISTORTION			equ	(1+DISTORTION_TEMP)

;
; the following delay variables are kept in Y RAM
;

DISTORTION_PREFILTER_Z_X1	equ	0
DISTORTION_PREFILTER_Z_X2	equ	1
DISTORTION_PREFILTER_Z_Y1	equ	2
DISTORTION_PREFILTER_Z_Y2	equ	3

DISTORTION_POSTFILTER_Z_X1	equ	4
DISTORTION_POSTFILTER_Z_X2	equ	5
DISTORTION_POSTFILTER_Z_Y1	equ	6
DISTORTION_POSTFILTER_Z_Y2	equ	7

	include 'IIR2_LowPass.h'
	include 'IIR2_BandPass.h'
	include 'IIR2_BandStop.h'
	include 'IIR2_Equalizer.h'


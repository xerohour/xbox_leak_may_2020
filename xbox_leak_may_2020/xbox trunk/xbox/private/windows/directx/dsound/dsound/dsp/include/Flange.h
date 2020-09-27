; **************************************************************************
;
; Flange.h    
;
; **************************************************************************        

; typedef struct _DSFXFlanger
; {
;     FLOAT       fWetDryMix;
;     FLOAT       fDepth;
;     FLOAT       fDecayTime;	// replaced fFeedback
;     FLOAT       fFrequency;
;     LONG        lWaveform;
;     FLOAT       fDelay;
;     LONG        lPhase;
; } DSFXFlanger, *LPDSFXFlanger;


;
; delay line in Y mem
;

FX_DELAY_LENGTH         equ FX_STATE_YMEM_LENGTH
FX_DELAY_BASE           equ FX_STATE_YMEM_BASE


    IF @DEF('STEREO')
FX_OFFSET       set     (DATASIZE_DSP_FX_STATE_3IN_2OUT)
    ELSE
FX_OFFSET       set     (DATASIZE_DSP_FX_STATE_2IN_1OUT)
    ENDIF

; Low Level Data
FLANGE_FEEDBACK     set  FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1

FLANGE_MOD_SCALE_H	set  FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1

FX_DELAY_INDEX	    set  FX_OFFSET 
FX_OFFSET           set  FX_OFFSET+1

FX_DELAY_END		set  FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1

FX_INPUT0           set  FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1

FX_INPUT1           set  FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1

FX_OUTPUT0          set  FX_OFFSET
FX_OFFSET           set  FX_OFFSET+1


DATASIZE_FLANGE	    equ  FX_OFFSET	

    IF @DEF('STEREO')
FLANGE_RIGHTCHANNEL_OFFSET equ DATASIZE_FLANGE+1
    ENDIF


;======================================================================
;======================================================================
;	The following variables enable and control the reverb fader.
;
; To enable the fader set 'FADER_ENABLED' equal to 1
; To disble the fader set 'FADER_ENABLED' equal to 0
;
;
FADER_ENABLED		set	1
;
;
; The following variables control the type of fading that is performed
; when the reverb parameters are changed.  Either, both or neither
; can be enabled.
;
;
;	'FADER_ENABLE_OUTPUTAMPITUDEFADE'	controls the fading of 
;	the output amplitudes.
;		FADER_ENABLE_OUTPUTAMPITUDEFADE=0	Disable output amplitude fading
;		FADER_ENABLE_OUTPUTAMPITUDEFADE=1	Enable output amplitude fading
;
;										
FADER_ENABLE_OUTPUTAMPITUDEFADE	set	1



;	'FADER_ENABLE_DELAYTAPFADE'	controls the fading of the delay buffers
;	on the main delay line.
;		FADER_ENABLE_DELAYTAPFADE=0	Disable fading
;		FADER_ENABLE_DELAYTAPFADE=1	Enable fading
;
;
FADER_ENABLE_DELAYTAPFADE		set 1

;======================================================================
;======================================================================


;	'HALFSAMPLERATE' controls whether the reverb engine is running at
;	the full sample rate or half the input sample rate.
;		HALFSAMPLERATE = 0	Full sample rate processing
;		HALFSAMPLERATE = 1	Half sample rate processing
;
;

;======================================================================
;======================================================================



;	'SizeOfFrame'   is the size of the internal audio frames measured in samples (=words)
;	'SizeOfIoFrame' is the size of the input/output audio frames measured in samples (=words)
; These settings are identical unless the reverb engine is operating at half the external
; sample rate.

	IF @DEF('HALFSAMPLERATE')
	msg				"Reverb engine operating a half external sample rate"
SizeOfFrame			set	16
SizeOfIoFrame		set	32

	ELSE

SizeOfFrame			set	32
SizeOfIoFrame		set	32

	ENDIF


;======================================================================
;======================================================================

;       set if reverb is supposed to be 2in->4out fx

USE_TWO_INPUTS    set     0

;======================================================================
;======================================================================

;       if BLOCKING_DMA is set, dma transfers will be syncronous
;       if not set, DMA will not block until the next dma transfer starts

BLOCKING_DMA    set     0

;======================================================================
;======================================================================


        ;
        ; set to skip crossfeed
        ;

SKIP_CROSSFEED    set     0

;======================================================================
;======================================================================

        ;
        ; set to skip short reverb
        ;

SKIP_SHORT_REVERB set     0

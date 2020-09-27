// FILE:			library\hardware\tuner\generic\FR1236.h
// AUTHOR:		Stefan Herr
// COPYRIGHT:	(c) 1996 Viona Development.  All Rights Reserved.
// CREATED:		31.07.96
//
// PURPOSE:		Definition of class for Philips FR1236 tuner (TV and FM radio support)
//
// HISTORY:

#ifndef FR1236_H
#define FR1236_H

#include "library\hardware\audio\generic\stereodc.h"
#include "library\hardware\videodma\generic\pip.h"
#include "library\hardware\video\generic\videnc.h"
#include "fi1236x.h"					// The FR1236 is an extended FI1236

// Minimum and maximum frequency for the FM band in 10^2 Hz
#define FM_LOW_LIMIT 	875000
#define FM_HIGH_LIMIT	1080000

#ifndef ONLY_EXTERNAL_VISIBLE

#define I2C_FR1236_UNIT0	0xc0	// The 1236's I2C base addresses. Up to 4 units possible.
#define I2C_FR1236_UNIT1	0xc2
#define I2C_FR1236_UNIT2	0xc4									
#define I2C_FR1236_UNIT3	0xc6									

class VirtualFR1236Tuner;

/////////////////////////////////////////////////////////////////////////////////////////////
// Class FR1236Tuner definition (physical unit)
/////////////////////////////////////////////////////////////////////////////////////////////
class FR1236Tuner : public FI1236XTuner {
	friend class VirtualFR1236Tuner;
	private:
		BOOL				fmEnable;
		BOOL				autoStep;
		SignalMode		mode;

		VirtualVideoEncoder		*fmVideoEnc;
		VirtualPIP					*fmPIP;

		VirtualStereoDecoder		*fmDecoder;
		VirtualStereoDecoder		*tvDecoder;
		
		WordInputPort	*fmSignalPort;
		BitOutputPort	*fmSelectPort;
	protected:                 
		void InitControlWordBytes();

		Error	ProgramFrequency(DWORD freq, DWORD __far & lastFreq);	// Program desired frequency into the FI1236
		Error	GetBand(DWORD freq, TVTunerBands __far & band);			// Get band for current frequency.

		Error	SetPLLDisable(BOOL pllDisable);								// *** Debugging purposes only

		virtual Error ActivateStereoDecoders();
		virtual Error PassivateStereoDecoders();

		virtual Error SetStepSize(StepRatioSelect stepSize);			// Set step size for scanning
		virtual Error SetFMEnable(BOOL fmEnable);							// Switch between TV/FM reception
		virtual Error GetMode(SignalMode __far & mode);
		virtual Error SetMode(SignalMode mode);

		Error	StartStepScan(Direction dir);
		Error ScanStep(ScanResult	__far & result,
							DWORD 		__far & curFreq,
							WORD			__far & curLevel,
						   ScanState 	__far & curState);

		Error Update(void);
		Error CheckScanInputLevel(WORD __far & level);
		Error GetSignal(BIT __far & signal);

 		Error SetFrequency(DWORD freq);
	public:
		FR1236Tuner(ByteInOutPort				*	i2cPort,					// I2C port to FR1236
						BitInputPort				*	signalPort,				// Port to video signal detection
	  				  	VirtualUnit					*	audioBus,				// Audio bus for multiplexing between MPEG and video
						VirtualStereoDecoder		*	tvDecoder,				// Access to TV stereo decoder
						VirtualStereoDecoder		*	fmDecoder,				// Access to FM stereo decoder
						WordInputPort				*	fmSignalPort,			// Port to FM signal/level detection
						BitOutputPort				*	fmSelectPort,			// Port to switch between FM/TV for external periphery,
						VirtualPIP					*	fmPIP,					// Will be activated when switching to FM
						VirtualVideoEncoder		*	fmVideoEnc				//   "        "             "           "
					 );

		VirtualUnit * CreateVirtual(void);

	};


/////////////////////////////////////////////////////////////////////////////////////////////
// Class VirtualFR1236Tuner definition (virtual unit)
/////////////////////////////////////////////////////////////////////////////////////////////

class VirtualFR1236Tuner : public VirtualFI1236XTuner {

	private:
		FR1236Tuner	*tuner;

		BOOL					fmEnable;
		StepRatioSelect	stepSize;
		SignalMode			mode;
		
	protected:
		Error DoPreempt(VirtualUnit * previous);

	public:
		VirtualFR1236Tuner(FR1236Tuner * physical, BOOL idleUnit);

		Error StartStepScan(Direction dir);
		
	 	Error	Configure (TAG __far *tags);
	};

#endif // of ONLY_EXTERNAL_VISIBLE

		
#endif


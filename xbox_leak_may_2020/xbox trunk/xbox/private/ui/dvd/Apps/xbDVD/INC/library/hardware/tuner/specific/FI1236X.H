// FILE:			library\hardware\tuner\generic\FI1236X.h
// AUTHOR:		Stefan Herr
// COPYRIGHT:	(c) 1995 Viona Development.  All Rights Reserved.
// CREATED:		19.07.95
//
// PURPOSE:		Definition of class for Philips FI1236 tuner with sound extension as available
//					on the H20 Debug board.
//
// HISTORY:

#ifndef FI1236X_H
#define FI1236X_H

#include "..\generic\tvtuner.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#define I2C_FI1236_UNIT0	0xc0	// The 1236's I2C base addresses. Up to 4 units possible.
#define I2C_FI1236_UNIT1	0xc2	// This is the default.
#define I2C_FI1236_UNIT2	0xc4									
#define I2C_FI1236_UNIT3	0xc6									

#define	STATIONGOODTHRESHOLD	2*(1000/5)	// if the result of CheckLevel() is >= this value,
														// the reception of a station on the current freq.
														// is OK.
														// Philips docu says it's a 5 level A/D converter.
														// 2 is 0 kHz, whatever this means...

#define LOCKPLLTIMEOUT		3000 				// timeout is 3000ms = 3s, after that the PLL should have
														// locked. Remark: Locking takes quite a long time when
														// switching between bands (> 1000 ms).

							
class VirtualFI1236XTuner;

/////////////////////////////////////////////////////////////////////////////////////////////
// Class FI1236XTuner definition (physical unit)
/////////////////////////////////////////////////////////////////////////////////////////////
class FI1236XTuner : public TVTuner {
	friend class VirtualFI1236XTuner;
	protected:                 
		ByteInOutPort*	i2cPort;
		BitOutputPort*	stereoPort;
		BitOutputPort* mutePort;
		BitInputPort*	signalPort;

		VirtualUnit	*	audioBus;			

		BOOL				pllDisable, mute, chargePump, stereo;
		DWORD				lastFreq,
							visionIF;

		DWORD				startFreq;

		BYTE				ctrlWordByte1, ctrlWordByte2;

		DWORD				stepSize;
		DWORD				bigJump;
		DWORD				minStepsValid,	maxStepsValid;
		DWORD				upperLimit, lowerLimit;

		Error Lock(VirtualUnit * unit);
		Error Unlock(VirtualUnit * unit);		

		Error	ScanStation(Direction dir, DWORD __far & freq);						// Scan for next station.
		Error	StartStepScan(Direction dir) {GNRAISE_OK;}
		Error ScanStep(ScanResult __far & 	result,
							DWORD __far & 			curFreq,
							WORD	__far & 			curLevel,
						   ScanState __far & 	curState);
		
		Error	CheckInputLevel(WORD __far & level);									// Check signal level of station on current frequency.
		virtual Error CheckScanInputLevel(WORD __far & level);

		Error Update(void);																	// Central function for updating the tuner settings.

		virtual Error	ProgramFrequency(DWORD freq, DWORD __far & lastFreq);	// Program desired frequency into the FI1236
		virtual Error	WaitLockPLL(void);												// Helper function for waiting for the PLL to lock
															
		virtual Error	SetChargePump(BOOL chargePump);								// *** Debugging purposes only
		virtual Error	SetPLLDisable(BOOL pllDisable);								// *** Debugging purposes only

		virtual Error	SetMute(BOOL mute);
		virtual Error	SetStereo(BOOL stereo);											// Switch between mono and stereo       

		virtual Error	GetBand(DWORD freq, TVTunerBands __far & band);			// Get band for current frequency.

		virtual void InitControlWordBytes();
		virtual Error GetSignal(BIT __far & signal);
	public:
		FI1236XTuner(ByteInOutPort *	i2cPort,			// I2C port to FI1236
	  				  	 BitOutputPort *	stereoPort,		// Port for stereo/mono switch
						 BitOutputPort	* 	mutePort,		// Port for mute on/off
						 BitInputPort	*	signalPort,		// Port for detecting the video signal level
	  				  	 VirtualUnit	*	audioBus			// Audio bus for multiplexing between MPEG and video
					 );

		VirtualUnit * CreateVirtual(void);

	};


/////////////////////////////////////////////////////////////////////////////////////////////
// Class VirtualFI1236XTuner definition (virtual unit)
/////////////////////////////////////////////////////////////////////////////////////////////

class VirtualFI1236XTuner : public VirtualTVTuner {

	private:
		FI1236XTuner* tuner;

	protected:		
		BOOL				pllDisable,
							chargePump,
							mute,
							stereo;
							
		BOOL				idleUnit;
		
	protected:
		virtual Error DoPreempt(VirtualUnit * previous);

	public:
		VirtualFI1236XTuner(FI1236XTuner * physical, BOOL idleUnit);
		
	 	Error	Configure (TAG __far *tags);
		Error Preempt(VirtualUnit * previous);
		
		Error ScanStation(Direction dir);							// Used for automatic station scanning.
		Error ScanStep(ScanResult __far & 	result,					// Scan stepwise
						   DWORD	__far & 			curFreq,
						   WORD __far & 			curLevel);
	};

#endif // of ONLY_EXTERNAL_VISIBLE

#endif


// FILE:			library\hardware\tuner\generic\AlpsTSBH.h
// AUTHOR:		Stefan Herr
// COPYRIGHT:	(c) 1995 Viona Development.  All Rights Reserved.
// CREATED:		19.07.95
//
// PURPOSE:		Definition of class for Alps TSBH1 tuner with sound extension as available
//					on the Buster board.
//
// HISTORY:

#ifndef ALPSTSBH_H
#define ALPSTSBH_H

#include "..\generic\tvtuner.h"


#ifndef ONLY_EXTERNAL_VISIBLE

//#define	STATIONGOODTHRESHOLD	2*(1000/5)	
//#define LOCKPLLTIME			300 

#define I2C_ADDR_ALPSTSBH1	0xc2				// I2C address

/////////////////////////////////////////////////////////////////////////////////////////////
// Class AlpsTSBH1Tuner definition (physical unit)
/////////////////////////////////////////////////////////////////////////////////////////////
class AlpsTSBH1Tuner : public TVTuner {
	friend class VirtualAlpsTSBH1Tuner;
	private:
		ByteInOutPort*	i2cPort;
		BitOutputPort*	stereoPort;
		BitInputPort*	signalPort;
  	 	BitInputPort*	aftPort;
  	 	BitInputPort*	rfaPort;
  	 	BitInputPort*	ifaPort;

		BYTE				ctrlWordByte1, ctrlWordByte2;
		BOOL				pllDisable, chargePump, stereo;
		DWORD				lastFreq,
							visionIF;

		VirtualUnit	*	audioBus;			

	protected:                 
		Error Lock(VirtualUnit * unit);
		Error Unlock(VirtualUnit * unit);		
	
		Error Update(void);														// Central function for updating the tuner settings.

		Error	ProgramFrequency(DWORD freq, DWORD __far & lastFreq);	// Program desired frequency into the FI1236
		Error	WaitLockPLL(void);												// Helper function for waiting for the PLL to lock
															
		Error	SetChargePump(BOOL chargePump);								// *** Debugging purposes only
		Error	SetPLLDisable(BOOL pllDisable);								// *** Debugging purposes only

		Error	SetStereo(BOOL stereo);											// Switch between mono and stereo       
		
		Error ScanStation(Direction dir, DWORD __far & freq);			// Scan for next station.
		Error	StartStepScan(Direction dir) {GNRAISE_OK;}
		Error ScanStep(ScanResult __far & result,
							DWORD __far & curFreq,
							WORD	__far & curLevel,
						   ScanState __far & curState);

		Error CheckInputLevel(WORD __far & level);						// Check signal level of station on current frequency.

		Error	GetBand(DWORD freq, TVTunerBands __far & band);			// Get band for current frequency.

	public:
		AlpsTSBH1Tuner(ByteInOutPort	*	i2cPort,			// I2C port to FI1236
	  				  	 	BitOutputPort	*	stereoPort,		// Port for stereo/mono switch
						 	BitInputPort	*	signalPort,		// Port for detecting the video signal level
	  				  	 	VirtualUnit		*	audioBus,		// Audio bus for multiplexing between MPEG and video
	  				  	 	BitInputPort	*	aftPort,			// Port for detecting AFT level.
	  				  	 	BitInputPort	*	rfaPort,			// Port for detecting RFA level.
	  				  	 	BitInputPort	*	ifaPort			// Port for detecting IFA level.
					 	  );
					 	  
		VirtualUnit * CreateVirtual(void);					 	 
	};


/////////////////////////////////////////////////////////////////////////////////////////////
// Class VirtualAlpsTSBH1Tuner definition (virtual unit)
/////////////////////////////////////////////////////////////////////////////////////////////

class VirtualAlpsTSBH1Tuner : public VirtualTVTuner {

	private:
		AlpsTSBH1Tuner* tuner;
		
		BOOL				pllDisable, 
							chargePump,
							stereo;
							
		BOOL				idleUnit;
				
	public:
		VirtualAlpsTSBH1Tuner(AlpsTSBH1Tuner * physical, BOOL idleUnit);
		
	 	Error	Configure (TAG __far *tags);
		Error Preempt(VirtualUnit * previous);
		
		Error ScanStation	(Direction dir);								// Used for automatic station scanning.
		Error ScanStep(ScanResult __far & 	result,					// Scan stepwise
						   DWORD	__far & 			curFreq,
						   WORD __far & 			curLevel);
	};

#endif // of ONLY_EXTERNAL_VISIBLE

		
#endif


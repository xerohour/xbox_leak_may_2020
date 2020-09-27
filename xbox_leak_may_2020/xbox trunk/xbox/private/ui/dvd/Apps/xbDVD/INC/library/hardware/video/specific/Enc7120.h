
// FILE:      library\hardware\video\specific\enc7120.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1997 Viona Development.  All Rights Reserved.
// CREATED:   03.11.97
//
// PURPOSE: The class for the SAA 7120 video encoder.
//
// HISTORY:

#ifndef ENC7120_H
#define ENC7120_H

#include "..\generic\videnc.h"
#include "library\lowlevel\hardwrio.h"



#define I2C_SAA7120   0x88  // the 7120's I2C bus address


class VirtualSAA7120;


class SAA7120 : public VideoEncoder 
	{
	friend class VirtualSAA7120;
	protected:
		ByteIndexedInOutPort	*	port;		// Comunication port

		VideoStandard		standard;		// current video standard
		VideoMode			mode;				// current video mode
		BOOL					extSync;			// current state of extSync

		BOOL					ntscSetup;		// NTSC 7.5 IRE setup

		DWORD					changed;			// state changed ??

		BYTE					ctrlState;
		//
		// Change parameters of physical unit
		//		
		virtual Error SetVideoStandard(VideoStandard std);
		virtual Error SetMode(VideoMode mode);
		virtual Error SetExtSync(BOOL extsync);
		virtual Error SetSampleMode(VideoSampleMode mode);
		virtual Error SetHOffset (int offset) {GNRAISE_OK;}
		virtual Error SetVOffset (int offset) {GNRAISE_OK;}
		virtual Error SetIdleScreen (int idleScreen);

      //
      // Perform the changes
      //
		Error ProcessChanges(void);

		Error SetIdleScreen (void);

		//
		// Program specific encoder mode
		//
		Error SetInit(void);
		Error SetNTSC(void);
		Error SetPAL(void);
		Error SetNTSCPattern(void);
		Error SetPALPattern(void);
		Error SetNTSCPlayback(void);
		Error SetPALPlayback(void);
		Error SetNTSCCapture(void);
		Error SetPALCapture(void);

		Error DisableEncoder(void);
		Error EnableEncoder(void);

		Error Configure(TAG __far * tags);

	public:
		SAA7120 (ByteIndexedInOutPort *port, Profile *profile = NULL);

		VirtualUnit * CreateVirtual(void);
	};


class VirtualSAA7120 : public VirtualVideoEncoder
	{
	friend class SAA7120;
	private:
		SAA7120	*	saa7120;
	public:
		VirtualSAA7120(SAA7120 * physical) 
			: VirtualVideoEncoder(physical) {saa7120 = physical;}

		Error Configure (TAG __far * tags);
	protected:
		Error Preempt (VirtualUnit * previous);
	};


#endif

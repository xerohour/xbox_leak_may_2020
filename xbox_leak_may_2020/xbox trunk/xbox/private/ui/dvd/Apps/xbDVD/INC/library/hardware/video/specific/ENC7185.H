
// FILE:      library\hardware\video\specific\enc7185.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   27.03.95
//
// PURPOSE: The class for the SAA 7185 video encoder.
//
// HISTORY:

#ifndef ENC7185_H
#define ENC7185_H

#include "..\generic\videnc.h"
#include "library\lowlevel\hardwrio.h"



#define I2C_SAA7185   0x88  // the 7185's I2C bus address


class VirtualSAA7185;


class SAA7185 : public VideoEncoder 
	{
	friend class VirtualSAA7185;
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
		SAA7185 (ByteIndexedInOutPort *port, Profile *profile = NULL);

		VirtualUnit * CreateVirtual(void);
	};


class VirtualSAA7185 : public VirtualVideoEncoder
	{
	friend class SAA7185;
	private:
		SAA7185	*	saa7185;
	public:
		VirtualSAA7185(SAA7185 * physical) 
			: VirtualVideoEncoder(physical) {saa7185 = physical;}

		Error Configure (TAG __far * tags);
	protected:
		Error Preempt (VirtualUnit * previous);
	};


#endif

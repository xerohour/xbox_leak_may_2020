
// FILE:      library\hardware\video\specific\dac624.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1998 Viona Development.  All Rights Reserved.
// CREATED:   22.03.98
//
// PURPOSE: The class for the IBM RGB624 RAM DAC used as a video encoder.
//
// HISTORY:

#ifndef DAC624_H
#define DAC624_H

#include "library\hardware\video\generic\videnc.h"
#include "library\lowlevel\hardwrio.h"



class VirtualRGB624;



class RGB624 : public VideoEncoder 
	{
	friend class VirtualRGB624;

	protected:
		ByteIndexedInOutPort	*	port;		// communication port

		VideoStandard		standard;		// current video standard
		VideoMode			mode;				// current video mode
		BOOL					extSync;			// current state of extSync

		BOOL					ntscSetup;		// NTSC 7.5 IRE setup

		DWORD					changed;			// state changed ?

		BYTE					ctrlState;

		// Change parameters of physical unit.
		virtual Error SetVideoStandard (VideoStandard std);
		virtual Error SetMode (VideoMode mode);
		virtual Error SetExtSync (BOOL extsync) {GNRAISE_OK;}
		virtual Error SetSampleMode (VideoSampleMode mode) {GNRAISE_OK;}
		virtual Error SetHOffset (int offset) {GNRAISE_OK;}
		virtual Error SetVOffset (int offset) {GNRAISE_OK;}
		virtual Error SetIdleScreen (int idleScreen);

		Error ProcessChanges (void);

		Error SetIdleScreen (void);

		// Program specific encoder mode.
		Error SetInit (void);
		Error SetPattern (void);
		Error SetPlayback (void);
		Error SetCapture (void);

		Error DisableEncoder (void);
		Error EnableEncoder (void);

		Error Configure (TAG __far *tags);

	public:
		RGB624 (ByteIndexedInOutPort *port, Profile *profile = NULL);

		VirtualUnit * CreateVirtual (void);
	};



class VirtualRGB624 : public VirtualVideoEncoder
	{
	friend class RGB624;

	private:
		RGB624	*	rgb624;

	public:
		VirtualRGB624 (RGB624 *physical) 
			: VirtualVideoEncoder (physical) {rgb624 = physical;}

		Error Configure (TAG __far *tags);

	protected:
		Error Preempt (VirtualUnit *previous);
	};



#endif

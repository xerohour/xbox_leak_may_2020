
// FILE:      library\hardware\video\specific\encBT856.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   27.03.95
//
// PURPOSE: The class for the  BT856b video encoder.
//
// HISTORY:

#ifndef ENCBT856_H
#define ENCBT856_H

#include "..\generic\videnc.h"
#include "..\..\..\lowlevel\hardwrio.h"



#define I2C_BT856   0x88  // the BT856's I2C bus address



// Sync polarities for the operation modes. Don't alter; chip specific!


class VirtualBT856;

class BT856 : public VideoEncoder 
	{
	friend class VirtualBT856;

	protected:
		ByteIndexedInOutPort	*port;

		VideoStandard		standard;
		VideoMode 			mode;
		BOOL 					extSync;
		int 					hOffset, vOffset;
		VideoSampleMode   sampleMode;

		BOOL changed;

		virtual Error SetVideoStandard (VideoStandard std);
		virtual Error SetMode (VideoMode mode);
		virtual Error SetExtSync (BOOL extsync);
		virtual Error SetSampleMode(VideoSampleMode mode);
		virtual Error SetHOffset (int offset);
		virtual Error SetVOffset (int offset);

		Error ProcessChanges (void);

		Error ProgramAll (void);

		Error Set_NTSC_TestPattern (void);
		Error Set_PAL_TestPattern (void);
		Error Set_NTSC_Capture (void);
		Error Set_PAL_Capture (void);
		Error Set_NTSC_Playback (void);
		Error Set_PAL_Playback (void);
		Error Set_NTSC_Playback_Ext (void);
		Error Set_PAL_Playback_Ext (void);

	public:
		BT856 (ByteIndexedInOutPort *port) : VideoEncoder ()
			{
			this->port = port;
			changed = TRUE;
			standard = VSTD_PAL;
			mode = VMOD_RESET;
			extSync = FALSE;
			hOffset = 0; vOffset = 0; 
			sampleMode = VSAMOD_CCIR;
			}

		VirtualUnit *CreateVirtual (void);
	};



class VirtualBT856 : public VirtualVideoEncoder
	{
	friend class BT856;

	private:
		BT856 *	bt856;

	public:
		VirtualBT856 (BT856 *physical) : VirtualVideoEncoder (physical) {bt856 = physical; }

		Error Configure (TAG __far * tags);

	protected:
		Error Preempt (VirtualUnit * previous);
	};



#endif

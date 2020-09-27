
// FILE:      library\hardware\video\specific\enc7199.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   27.03.95
//
// PURPOSE: The class for the SAA 7199b video encoder.
//
// HISTORY:

#ifndef ENC7199_H
#define ENC7199_H

#include "library\hardware\video\generic\videnc.h"
#include "library\lowlevel\hardwrio.h"



#define I2C_SAA7199   0xb0  // the 7199's I2C bus address



// Sync polarities for the operation modes. Don't alter; chip specific!

enum SAA7199_Sync
	{
	SAA7199_CSYN_LOW          = 0,
	SAA7199_HS_VS_LOW_ACTIVE  = 1,
	SAA7199_CSYN_HIGH         = 2,
	SAA7199_HS_VS_HIGH_ACTIVE = 3
	};



class VirtualSAA7199;



class SAA7199 : public VideoEncoder 
	{
	friend class VirtualSAA7199;

	protected:
		ByteIndexedInOutPort	*port;
		SAA7199_Sync patternSync, captureSync, playbackSync;

		VideoStandard standard;
		VideoMode mode;
		BOOL extSync;
		int hOffset, vOffset;

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
		Error Set_SECAM_Capture (void);
		Error Set_NTSC_Playback (void);
		Error Set_PAL_Playback (void);
		Error Set_NTSC_Playback_Ext (void);
		Error Set_PAL_Playback_Ext (void);

	public:
		SAA7199 (ByteIndexedInOutPort *port, SAA7199_Sync patternSync,
		         SAA7199_Sync captureSync, SAA7199_Sync playbackSync)
			: VideoEncoder ()
			{
			this->port = port;
			this->patternSync = patternSync;
			this->captureSync = captureSync;
			this->playbackSync = playbackSync;
			changed = TRUE;
			standard = VSTD_PAL;
			mode = VMOD_RESET;
			extSync = FALSE;
			}

		VirtualUnit *CreateVirtual (void);
	};



class VirtualSAA7199 : public VirtualVideoEncoder
	{
	friend class SAA7199;

	private:
		SAA7199 *saa7199;

	public:
		VirtualSAA7199 (SAA7199 *physical) : VirtualVideoEncoder (physical)
			{saa7199 = physical; sampleMode = VSAMOD_SQP;}

		Error Configure (TAG __far * tags);

	protected:
		Error Preempt (VirtualUnit * previous);
	};



#endif

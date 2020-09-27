
// FILE:      library\hardware\video\specific\enc7187.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   27.03.95
//
// PURPOSE: The class for the SAA 7187 video encoder.
//
// HISTORY:

#ifndef ENC7187_H
#define ENC7187_H

#include "library\hardware\video\generic\videnc.h"
#include "library\lowlevel\hardwrio.h"



#define I2C_SAA7187   0x88  // the 7187's I2C bus address



// Sync polarities for the operation modes. Don't alter; chip specific!

enum SAA7187_Sync
	{
	SAA7187_HS_VS_LOW_ACTIVE  = 0x09,
	SAA7187_HS_VS_HIGH_ACTIVE = 0x00,
	SAA7187_HS_LOW_VS_HIGH    = 0x01,
	SAA7187_HS_HIGH_VS_LOW    = 0x08
	};



class VirtualSAA7187;



class SAA7187 : public VideoEncoder 
	{
	friend class VirtualSAA7187;

	protected:
		ByteIndexedInOutPort *port;
		SAA7187_Sync patternSync, captureSync, playbackSync;

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
		virtual Error SetIdleScreen (int idleScreen);

		Error ProcessChanges (void);

		Error ProgramAll (void);

		Error SetIdleScreen (void);

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
		SAA7187 (Profile *profile,
		         ByteIndexedInOutPort *port, SAA7187_Sync patternSync,
		         SAA7187_Sync captureSync, SAA7187_Sync playbackSync);

		VirtualUnit *CreateVirtual (void);
	};



class VirtualSAA7187 : public VirtualVideoEncoder
	{
	friend class SAA7187;

	private:
		SAA7187 *saa7187;

	public:
		VirtualSAA7187(SAA7187 *physical) : VirtualVideoEncoder (physical)
			{saa7187 = physical; sampleMode = VSAMOD_SQP;}

		Error Configure (TAG __far *tags);

	protected:
		Error Preempt (VirtualUnit *previous);
	};



#endif

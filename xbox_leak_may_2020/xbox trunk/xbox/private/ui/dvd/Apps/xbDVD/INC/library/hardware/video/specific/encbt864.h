
// FILE:      library\hardware\video\specific\encBT864.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   27.03.95
//
// PURPOSE: The class for the  BT864b video encoder.
//
// HISTORY:

#ifndef ENCBT864_H
#define ENCBT864_H

#include "..\generic\ccapvenc.h"
#include "library\lowlevel\hardwrio.h"



#define I2C_BT864   0x88  // the BT864's I2C bus address



// Sync polarities for the operation modes. Don't alter; chip specific!


class VirtualBT864;

class BT864 : public CloseCaptionVideoEncoder 
	{
	friend class VirtualBT864;

	protected:
		ByteIndexedInOutPort	*port;

		VideoStandard			standard;
		PALVideoSubStandard	palSubStandard;
		NTSCVideoSubStandard	ntscSubStandard;

		VideoMode 			mode;
		BOOL 					extSync;
		BOOL					modeOverride;
		int 					hOffset, vOffset;
		VideoSampleMode   sampleMode;
		int					copyProtection;		
		VideoBusFormat		videoBusFormat;
		
		BYTE					shadow0xce, shadow0xbc, vidForm;
		BOOL					palNBit;
		BOOL					setupDis;

		DWORD changed;

		virtual Error SetVideoStandard (VideoStandard std);
		virtual Error SetPALVideoSubStandard(PALVideoSubStandard palSubStandard);
		virtual Error SetNTSCVideoSubStandard(NTSCVideoSubStandard ntscSubStandard);
		virtual Error SetMode (VideoMode mode);
		virtual Error SetExtSync (BOOL extsync);
		virtual Error SetSampleMode(VideoSampleMode mode);
		virtual Error SetHOffset (int offset);
		virtual Error SetVOffset (int offset);
		virtual Error SetCopyProtection (int copyProtection);
		virtual Error SetIdleScreen (int idleScreen);

		Error GetCCStatus(BOOL __far & first, BOOL __far & second);
		Error PutCCData(BOOL field, BYTE d1, BYTE d2);
		Error EnableCCTransfer(void);
		Error DisableCCTransfer(void);		

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
		Error Set_CopyProtection (void);
		
		Error Configure(TAG __far * tags);
	public:
		BT864 (ByteIndexedInOutPort *port, Profile * profile = NULL);

		Error	ReInitialize(void);

		VirtualUnit *CreateVirtual (void);
	};



class VirtualBT864 : public VirtualCloseCaptionVideoEncoder
	{
	friend class BT864;

	private:
		BT864 *	bt864;

	public:
		VirtualBT864 (BT864 * physical) : VirtualCloseCaptionVideoEncoder (physical) {bt864 = physical; }

		Error Configure (TAG __far * tags);

	protected:
		Error Preempt (VirtualUnit * previous);
	};



#endif

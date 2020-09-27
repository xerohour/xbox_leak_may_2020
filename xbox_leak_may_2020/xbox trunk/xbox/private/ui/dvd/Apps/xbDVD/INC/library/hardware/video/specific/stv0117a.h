
// FILE:      library\hardware\video\specific\encBT856.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   27.03.95
//
// PURPOSE: The class for the STV0117A video encoder
//
// HISTORY:

#ifndef STV0117A_H
#define STV0117A_H

#include "..\generic\videnc.h"
#include "..\..\..\lowlevel\hardwrio.h"



#define I2C_STV0117A   0x40 



class VirtualVideoEncoderSTV0117A;

class VideoEncoderSTV0117A : public VideoEncoder 
	{
	friend class VirtualVideoEncoderSTV0117A;

	protected:
		ByteIndexedInOutPort	*port;

		VideoStandard		standard;
		VideoMode 			mode;
		int 					hOffset, vOffset;
		int					copyProtection;        
		VideoSampleMode   sampleMode;

		DWORD changed;

		virtual Error SetVideoStandard (VideoStandard std);
		virtual Error SetMode (VideoMode mode);
		virtual Error SetExtSync (BOOL sync);
		virtual Error SetSampleMode(VideoSampleMode mode);
		virtual Error SetHOffset (int offset);
		virtual Error SetVOffset (int offset);
		virtual Error SetCopyProtection (int copyProtection);

		Error ProcessChanges (void);

		Error ProgramAll (void);

		Error Set_NTSC_TestPattern (void);
		Error Set_PAL_TestPattern (void);
		Error Set_NTSC_Capture (void);
		Error Set_PAL_Capture (void);
		Error Set_NTSC_Playback (void);
		Error Set_PAL_Playback (void);
		
		Error Set_CopyProtection (void);
	public:
		VideoEncoderSTV0117A (ByteIndexedInOutPort *port) : VideoEncoder ()
			{
			this->port = port;
			changed = 0xffffffff;
			standard = VSTD_PAL;
			mode = VMOD_RESET;
			hOffset = 0; vOffset = 0; 
			sampleMode = VSAMOD_CCIR;
			copyProtection = 0;
			}

		VirtualUnit *CreateVirtual (void);
	};



class VirtualVideoEncoderSTV0117A : public VirtualVideoEncoder
	{
	friend class VideoEncoderSTV0117A;

	private:
		VideoEncoderSTV0117A * device;

	public:
		VirtualVideoEncoderSTV0117A (VideoEncoderSTV0117A *physical) : VirtualVideoEncoder (physical) 
			{device = physical; }

		Error Configure (TAG __far * tags);

	protected:
		Error Preempt (VirtualUnit * previous);
	};



#endif


// FILE:      library\hardware\video\specific\dec7110.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   20.03.95
//
// PURPOSE: The class for the SAA 7110 video decoder.
//
// HISTORY:

#ifndef DEC7110_H
#define DEC7110_H

#include "library\common\gnerrors.h"
#include "library\hardware\video\generic\viddec.h"


#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\hardwrio.h"

#define I2C_SAA7110  0x9c   // the 7110's I2C bus address



class VirtualSAA7110;   // forward declaration



// For mapping input lines to 7110 input chip modes.

#define SAA7110_MAX_INPUT_LINES  6

enum SAA7110_InputMode
	{
	SAA7110_INPUTMODE_CVBS1, SAA7110_INPUTMODE_CVBS2, SAA7110_INPUTMODE_CVBS3,
	SAA7110_INPUTMODE_CVBS4, SAA7110_INPUTMODE_NONE,  SAA7110_INPUTMODE_CVBS6,
	SAA7110_INPUTMODE_YC1, SAA7110_INPUTMODE_YC2, SAA7110_INPUTMODE_YC3,
	SAA7110_INPUTMODE_YC4,   // for H22
	SAA7110_INPUTMODE_YC5
	};



class SAA7110 : public VideoDecoder
	{
	friend class VirtualSAA7110;
	friend class SAA7110GPSWPort;

	protected:
		Error OutGPSWBit (BIT data);

		ByteIndexedInOutPort *port;
		SAA7110_InputMode lines[SAA7110_MAX_INPUT_LINES];
		BYTE gpsw;

		BYTE brightness;
		BYTE contrast;
		BYTE saturation;
		BYTE hue;

		WORD input;
		VideoStandard standard;
		VideoMode mode;
		BOOL extSync;

		DWORD changed;

		Error SetVideoStandard (VideoStandard std);
		Error SetMode (VideoMode mode);
		Error SetExtSync (BOOL extsync);
		Error SetInput (WORD input);
		Error SetBrightness (WORD brightness);
		Error SetContrast (WORD contrast);
		Error SetSaturation (WORD saturation);
		Error SetHue (WORD hue);
		Error SetSampleMode (VideoSampleMode mode);

		BOOL VideoStandardWasChanged (void);

		virtual Error ProcessChanges (void);

		Error ProgramAll (void);
		Error ProgramInput (void);
		Error ProgramBrightness (void);
		Error ProgramContrast (void);
		Error ProgramSaturation (void);
		Error ProgramHue (void);

		Error ProgramInputNone (void);

		Error Set_RESET (void);

		Error Set_NTSC_TestPattern (void);
		Error Set_PAL_TestPattern (void);
		Error Set_NTSC_Capture (void);
		Error Set_PAL_Capture (void);
		Error Set_SECAM_Capture (void);
		Error Set_NTSC_Playback (void);
		Error Set_PAL_Playback (void);
		Error Set_NTSC_Playback_Ext (void);
		Error Set_PAL_Playback_Ext (void);

		virtual Error DetectVideoStandard (VideoStandard __far &std);

	public:
		// Select the input mode for the six input lines.
		SAA7110 (ByteIndexedInOutPort *port, VirtualUnit *videoBus,
		         WORD numInputs, VideoDecoderInputEntry __far *inputNames,
		         SAA7110_InputMode lines[SAA7110_MAX_INPUT_LINES]);
		~SAA7110 (void);

		VirtualUnit *CreateVirtual (void);

		BitOutputPort *outPort;
	};



class VirtualSAA7110 : public VirtualVideoDecoder
	{
	private:
		SAA7110 *saa7110;

	public:
		VirtualSAA7110 (SAA7110 *physical) : VirtualVideoDecoder (physical)
			{ saa7110 = physical; sampleMode = VSAMOD_SQP;}

		Error Configure (TAG __far *tags);

	protected:
		Error Preempt (VirtualUnit *previous);
	};



#endif

#endif

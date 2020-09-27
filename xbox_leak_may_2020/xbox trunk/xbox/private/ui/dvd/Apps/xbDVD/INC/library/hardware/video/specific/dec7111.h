
// FILE:			library\hardware\video\specific\dec7111.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 Viona Development.  All Rights Reserved.
// CREATED:		23.08.95
//
// PURPOSE: 	Class for SAA 7111 video decoder.
//
// HISTORY:

#ifndef DEC7111_H
#define DEC7111_H

#include "library\common\gnerrors.h"         
#include "..\generic\viddec.h"

#include "library\common\hooks.h"
MKHOOK (DecVBlank, WORD)
MKTAG (DEC_CLOSEDCAPTION, VIDEO_DECODER_UNIT, 0x3000, DecVBlankHook*)

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\intrctrl.h"
#include "library\lowlevel\hardwrio.h"

#define I2C_SAA7111  0x48   // the 7111's I2C bus address when IICSA pin = LOW
									 // If HIGH, use value 0x4a
                                                         

class VirtualSAA7111;   // forward declaration



// For mapping input lines to 7111 input chip modes.

#define SAA7111_MAX_INPUT_LINES  8

enum SAA7111_InputMode
	{
	SAA7111_INPUTMODE_CVBS0, SAA7111_INPUTMODE_CVBS1, SAA7111_INPUTMODE_CVBS2, SAA7111_INPUTMODE_CVBS3,
	SAA7111_INPUTMODE_YC0, SAA7111_INPUTMODE_YC1, SAA7111_INPUTMODE_YC2, SAA7111_INPUTMODE_YC3,
	SAA7111_INPUTMODE_NONE
	};



enum SAA7111_OutputMode   // don't change: chip specific !
	{
	SAA7111_OUTPUTMODE_RGB565    = 0,
	SAA7111_OUTPUTMODE_YUV_16bit = 1,
	SAA7111_OUTPUTMODE_YUV_12bit = 2,
	SAA7111_OUTPUTMODE_YUV_8bit  = 3
	};



class SAA7111 : public VideoDecoder
	{
	friend class VirtualSAA7111;
	friend class SAA7111GPSWPort;
	friend class SAA7111SignalPort;
	friend class SAA7111VBlankIntHandler;
	
	private:
		//
		// Communication port (typically I2C)
		//		
		ByteIndexedInOutPort *port;
      
		SAA7111_InputMode lines[SAA7111_MAX_INPUT_LINES];

		//
		// State of the "gpsw" bit
		//
		BYTE				gpsw;
		
		//
		// Current image settings in internal format
		//
		WORD				brightness;
		WORD				contrast;
		WORD				saturation;
		BYTE				hue;
      
		BOOL				teletext;
      
		DecVBlankHookHandle		closedHookHandle;
		InterruptServer*	vBlankIntServer;
		SAA7111VBlankIntHandler*	vBlankIntHandler;
		BOOL				closedCaptionRunning;
      //
		// Current input, standard, mode etc.
		//
		WORD				input;
		VideoStandard	standard;
		VideoMode		mode;
		BOOL				extSync;

		SAA7111_OutputMode outputMode;

		void VBlankInterrupt(void);
		//
		// Parameter setting functions		
		//
		Error SetVideoStandard(VideoStandard std);
		Error SetMode(VideoMode mode);
		Error SetExtSync(BOOL extsync);
		Error SetInput(WORD input);
		Error SetBrightness(WORD brightness);
		Error SetContrast(WORD contrast);
		Error SetSaturation(WORD saturation);
		Error SetHue(WORD hue);         
		Error SetSampleMode(VideoSampleMode mode);
		Error SetTeletextMode(BOOL mode);
		Error SetClosedCaptionHook(DecVBlankHook* closedHook);

		//
		// Reprogram chip values
		//		
#if 0
		Error ProgramAll(void);		
#endif		
		Error ProgramInput(void);
		Error ProgramBrightness(void);
		Error ProgramContrast(void);
		Error ProgramSaturation(void);
		Error ProgramHue(void);
		Error ProgramClosedCaption(void);
      
		//
		// Program specific encoder mode
		//
		Error SetInit(void);
		Error SetNTSCPlayback(void);
		Error SetPALPlayback(void);
		Error SetNTSCCapture(void);
		Error SetPALCapture(void);
		Error SetReset(void);
		Error SetNTSCPatternSync (void);
		Error SetPALPatternSync (void);
		
#if 0
		//
		// Set mode and standard
		//
		Error Set_NTSC_TestPattern (void);
		Error Set_PAL_TestPattern (void);
		Error Set_NTSC_Capture (void);
		Error Set_PAL_Capture (void);
		Error Set_SECAM_Capture (void);
		Error Set_NTSC_Playback (void);
		Error Set_PAL_Playback (void);
		Error Set_NTSC_Playback_Ext (void);
		Error Set_PAL_Playback_Ext (void);
		Error	Set_RESET (void);
#endif

		Error StartClosedCaption();
		Error StopClosedCaption();

		BYTE CalcBrightness (void);
		BYTE CalcContrast (void);
		BYTE CalcSaturation (void);

		//
		// Fix the NTSC bug in 7111 first silicon
		//
		Error NTSCSwitchBugFix(void);

	protected:
		virtual Error SetNTSC(void);
		virtual Error SetPAL(void);

		//                         
		// Helper functions for the two included ports
		//
		Error OutGPSWBit(BIT data);
		Error CheckInputSignal(BOOL __far & signal);

		// Check the current video standard
		Error DetectVideoStandard(VideoStandard __far &std);

		// Checks changes in setting, and triggers changes in registers
		virtual Error ProcessChanges(void);

		// Flags, which part of the programming have been changed
		DWORD				changed;

		BOOL VideoStandardWasChanged (void);

		VideoStandard GetVideoStandard(void) {return standard;}

	public:
		SAA7111 (ByteIndexedInOutPort *port, VirtualUnit *videoBus, InterruptServer *vBlankIntServer = NULL,
		         WORD numInputs = 0, VideoDecoderInputEntry __far *inputNames = NULL,
		         SAA7111_InputMode lines[SAA7111_MAX_INPUT_LINES] = NULL);
		~SAA7111 (void);

		VirtualUnit * CreateVirtual(void);

		Error SetOutput (SAA7111_OutputMode output);   // (change to private scope later)

		BitOutputPort	*	outPort;
	};



class VirtualSAA7111 : public VirtualVideoDecoder
	{      
	protected:
		BOOL						teletext;
		DecVBlankHook* closedHookPtr;
	private:
		SAA7111		*	saa7111;
	public:
		VirtualSAA7111 (SAA7111 *physical);

		Error Configure (TAG __far *tags);

	protected:
		Error Preempt (VirtualUnit *previous);
	};


#endif	// of #define ONLY_EXTERNAL_VISIBLE

#endif

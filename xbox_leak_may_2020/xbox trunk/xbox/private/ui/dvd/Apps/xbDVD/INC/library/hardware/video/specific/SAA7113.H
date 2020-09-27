//
// FILE:       library\hardware\video\specific\saa7113.h
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 Viona Development GmbH.  All Rights Reserved.
// CREATED:		29.11.1999
//
// PURPOSE: 	9-bit video input processor SAA 7113 H - interface
//
// HISTORY:
//

#ifndef SAA7113_H
#define SAA7113_H


#include "library\common\gnerrors.h"         
#include "..\generic\viddec.h"
#include "library\lowlevel\intrctrl.h"


class VirtualSAA7113;   // forward declaration


#define SAA7113_MAX_INPUT_LINES  6

//
// SAA 7113 input mode
// four analog inputs, internal analog source selectors, e.g. 4 * CVBS or 2 * Y/C
enum SAA7113_InputMode
	{
	SAA7113_INPUTMODE_CVBS0, SAA7113_INPUTMODE_CVBS1, SAA7113_INPUTMODE_CVBS2, SAA7113_INPUTMODE_CVBS3,
	SAA7113_INPUTMODE_YC0, SAA7113_INPUTMODE_YC1, SAA7113_INPUTMODE_YC2, SAA7113_INPUTMODE_YC3,
	SAA7113_INPUTMODE_NONE
	};

//
// SAA 7113 output mode
// standard ITU 656 YUV 4:2:2 format (8 bit)
enum SAA7113_OutputMode		
	{
	SAA7113_OUTPUTMODE_YUV_8bit    = 0,
	};


class SAA7113 : public VideoDecoder
	{
	friend class VirtualSAA7113;
	friend class SAA7113VBlankIntHandler;
	
	private:
		void VBlankInterrupt(void);
		//
		// Communication port (typically I2C)
		//		
		ByteIndexedInOutPort		*port;

		SAA7113_InputMode lines[SAA7113_MAX_INPUT_LINES];

		WORD				brightness;
		WORD				contrast;
		WORD				saturation;
		BYTE				hue;
      
		//
		// Current input, standard, mode etc.
		//
		WORD				input;
		VideoStandard	standard;
		VideoMode		mode;

		InterruptServer			*vBlankIntServer;
		SAA7113VBlankIntHandler	*vBlankIntHandler;

		SAA7113_OutputMode		outputMode;

		//
		// Parameter setting functions		
		//
		Error SetVideoStandard(VideoStandard std);
		Error SetMode(VideoMode mode);
		Error SetInput(WORD input);
		Error SetBrightness(WORD brightness);
		Error SetContrast(WORD contrast);
		Error SetSaturation(WORD saturation);
		Error SetHue(WORD hue);         
		Error SetSampleMode(VideoSampleMode mode);		// mst: don't know yet what a video sample mode is!

		//
		// Reprogram chip values
		//		
		Error ProgramInput(void);
		Error ProgramBrightness(void);
		Error ProgramContrast(void);
		Error ProgramSaturation(void);
		Error ProgramHue(void);   
		
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
		
		BYTE CalcBrightness (void);
		BYTE CalcContrast (void);
		BYTE CalcSaturation (void);

	public:
		SAA7113(ByteIndexedInOutPort *port, VirtualUnit *videoBus, InterruptServer *vBlankIntServer = NULL,
		         WORD numInputs = 0, VideoDecoderInputEntry __far *inputNames = NULL,
		         SAA7113_InputMode lines[SAA7113_MAX_INPUT_LINES] = NULL);
		~SAA7113(void);

		VirtualUnit * CreateVirtual(void);

		Error SetOutput (SAA7113_OutputMode output);   // (change to private scope later)

	protected:
		virtual Error SetNTSC(void);
		virtual Error SetPAL(void);

		// Check the current video standard
		Error DetectVideoStandard(VideoStandard __far &std);

		BOOL VideoStandardWasChanged (void);

		VideoStandard GetVideoStandard(void) {return standard;}

		// Checks changes in setting, and triggers changes in registers
		virtual Error ProcessChanges(void);

		// Flags, which part of the programming have been changed
		DWORD			changed;

	};

class VirtualSAA7113 : public VirtualVideoDecoder
	{      
	private:
		SAA7113		*	saa7113;
	public:
		VirtualSAA7113 (SAA7113 *physical);

		Error Configure (TAG __far *tags);

	protected:
		Error Preempt (VirtualUnit *previous);
	};


#endif // endif SAA7113_H

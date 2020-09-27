
// FILE:		   library\hardware\video\specific\vidde848.h
// AUTHOR:		P.C.Straasheijm
// COPYRIGHT:	(c) 1995 Viona Development.  All Rights Reserved.
// CREATED:		10.10.96
//
// PURPOSE: 	Class for BT848 video decoder.
//
// HISTORY:

#ifndef VIDDE848_H
#define VIDDE848_H

#include "library\common\gnerrors.h"         
#include "library\hardware\video\generic\viddec.h" 
#include "library\hardware\video\generic\vidtypes.h"
#include "library\common\hooks.h"

#ifndef ONLY_EXTERNAL_VISIBLE
#include "library\hardware\pcibrdge\specific\bt848\bt848reg.h"
#endif

MKHOOK (DecVBlank, WORD)
MKTAG (DEC_CLOSEDCAPTION, VIDEO_DECODER_UNIT, 0x3000, DecVBlankHook*)

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\intrctrl.h"
#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\memmpdio.h"

class VirtualVideoDecoder848;   // forward declaration

class VideoDecoder848 : public VideoDecoder
	{
	friend class VirtualVideoDecoder848;
	friend class VideoDecoder848VBlankIntHandler;
	friend class BT848SignalPort;
	
	private:
		
		//
		// Current image settings in internal format
		//
		BYTE				brightness;
		WORD				contrast;
		WORD				saturation;
		BYTE				hue;
		BOOL				teletext;
		BOOL				autoselect;
      
		MemoryMappedIO		*	io;
		
//		DecVBlankHookHandle		closedHookHandle;
		InterruptServer						*vBlankIntServer;
		VideoDecoder848VBlankIntHandler	*vBlankIntHandler;
//		BOOL					closedCaptionRunning;
      //
		// Current input, standard, mode etc.
		//
		WORD					input;
		VideoStandard		standard;
		VideoMode			mode;
		BOOL					extSync;
      BOOL					autoformat;
		//
		// Flags, which part of the programming have been changed
		//		
		DWORD				changed;

		void VBlankInterrupt(void);
		//
		// Parameter setting functions		
		//                                   +#
		
		Error SetVideoStandard(VideoStandard std);
		Error SetMode(VideoMode mode);
		Error SetExtSync(BOOL extsync);
		Error SetInput(WORD input);
		Error SetBrightness(WORD brightness);
		Error SetContrast(WORD contrast);
		Error SetSaturation(WORD saturation);
		Error SetHue(WORD hue);         
		Error SetSampleMode(VideoSampleMode mode);
//		Error SetTeletextMode(BOOL mode);
		Error SetClosedCaptionHook(DecVBlankHook* closedHook);
		
		//
		// Checks changes in setting, and triggers changes in registers
		//
		Error ProcessChanges(void);

		//
		// Reprogram chip values
		//		
		Error ProgramAll(void);		
		Error ProgramInput(void);
		Error ProgramBrightness(void);
		Error ProgramContrast(void);
		Error ProgramSaturation(void);
		Error ProgramHue(void);
		Error ProgramClosedCaption(void);
      Error CheckInputSignal(BOOL __far & signal);
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
		Error Set_RESET (void);

//		Error StartClosedCaption();
//		Error StopClosedCaption();
		//
		// Check the current video standard
		//		
		Error DetectVideoStandard(VideoStandard __far &std);

	public:
		VideoDecoder848 (MemoryMappedIO *io, InterruptServer * vBlankIntServer = NULL, VirtualUnit * videoBus = NULL);
		~VideoDecoder848 (void);

		VirtualUnit 		* 	CreateVirtual(void);

	};



class VirtualVideoDecoder848 : public VirtualVideoDecoder
	{      
	protected:
		BOOL						teletext;
		DecVBlankHook		*	closedHookPtr;
	private:
		VideoDecoder848	*	videodecoder;
	public:
		VirtualVideoDecoder848 (VideoDecoder848 *physical);

		Error Configure (TAG __far *tags);

	protected:
		Error Preempt (VirtualUnit *previous);
	};


#endif	// of #define ONLY_EXTERNAL_VISIBLE

#endif

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


// FILE:      library\hardware\videodma\generic\pip.h
// AUTHOR:    Ulrich Sigmund, S. Herr
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   08.04.95
//
// PURPOSE: The PIP-related classes.
//
// HISTORY:

#ifndef PIP_H
#define PIP_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"
#include "library\common\hooks.h"
#include "library\hardware\video\generic\vidtypes.h"

#ifndef ONLY_EXTERNAL_VISIBLE
#include "library\common\virtunit.h"
#include "library\general\queues.h"
#include "library\lowlevel\kernlmem.h"
#endif

MKTAG(PIP_VIDEOSTANDARD,	PIP_UNIT,	0x00000001, VideoStandard)
#if !NT_KERNEL && !VXD_VERSION
MKTAG(PIP_WINDOW,				PIP_UNIT,	0x00000002, HWND)
#endif

	//
	// Source region, relative to active display
	//
MKTAG(PIP_SOURCE_LEFT,		PIP_UNIT,	0x00000003, int)
MKTAG(PIP_SOURCE_TOP,		PIP_UNIT,	0x00000004, int)
MKTAG(PIP_SOURCE_WIDTH,		PIP_UNIT,   0x00000005, int)
MKTAG(PIP_SOURCE_HEIGHT,	PIP_UNIT,   0x00000006, int)

	//
	// Destination region, relative to current window
	//
MKTAG(PIP_DEST_LEFT,			PIP_UNIT,	0x00000007, int)
MKTAG(PIP_DEST_TOP,			PIP_UNIT,	0x00000008, int)
MKTAG(PIP_DEST_WIDTH,		PIP_UNIT,   0x00000009, int)
MKTAG(PIP_DEST_HEIGHT,		PIP_UNIT,   0x0000000A, int)

MKTAG(PIP_ADAPT_CLIENT_SIZE, PIP_UNIT, 0x0000000B, BOOL)
MKTAG(PIP_ADAPT_SOURCE_SIZE, PIP_UNIT, 0x0000000C, BOOL)
MKTAG(PIP_SCREEN_DEST,     PIP_UNIT,   0x0000000D, BOOL)
MKTAG(PIP_SAMPLEMODE,		PIP_UNIT,	0x0000000E, VideoSampleMode)
MKTAG(PIP_HOFFSET,			PIP_UNIT,	0x0000000F, int)
MKTAG(PIP_VOFFSET,			PIP_UNIT,	0x00000010, int)
MKTAG(PIP_ALWAYS_OVERLAY,	PIP_UNIT,	0x00000011, BOOL)
MKTAG(PIP_COLORKEY_COLOR,	PIP_UNIT,	0x00000012, DWORD)
MKTAG(PIP_COLORKEYED,		PIP_UNIT,	0x00000013, BOOL)

	//
	// Display region, relative to the destination rectangle
	//
MKTAG(PIP_DISPLAY_LEFT,		PIP_UNIT,	0x00000014, short)
MKTAG(PIP_DISPLAY_TOP,		PIP_UNIT,	0x00000015, short)
MKTAG(PIP_DISPLAY_WIDTH,	PIP_UNIT,	0x00000016, short)
MKTAG(PIP_DISPLAY_HEIGHT,	PIP_UNIT,	0x00000017, short)

MKTAG(PIP_COLORKEY_INDEX,	PIP_UNIT,	0x00000018, DWORD)

// Make sure to set physical, bytesPerRow and pixelFormat at the same time when enabling this:
MKTAG (PIP_OFFSCREEN_OVERRIDE,		PIP_UNIT, 0x00000019, BOOL)
MKTAG (PIP_OFFSCREEN_BASE,				PIP_UNIT, 0x0000001a, void __far *)
MKTAG (PIP_OFFSCREEN_BASE_LINEAR,	PIP_UNIT, 0x0000001b, DWORD)		// Use a 32bit flat pointer
MKTAG (PIP_OFFSCREEN_BYTES_PER_ROW,	PIP_UNIT, 0x0000001c, WORD)
MKTAG (PIP_OFFSCREEN_PIXEL_FORMAT,	PIP_UNIT, 0x0000001d, PIPFormat)

struct FieldDisplayMsg
	{
	DWORD	fieldCnt;
	};

MKHOOK(FieldDisplay, FieldDisplayMsg);

MKTAG (PIP_FIELD_DISPLAY_HOOK,		PIP_UNIT, 0x0000001e, FieldDisplayHook *)

MKTAG (PIP_IS_POSSIBLE,					PIP_UNIT, 0x0000001f, BOOL)


#include "library\hardware\videodma\generic\colctrlr.h"

MKTAG (PIP_COLORCONTROL,				PIP_UNIT, 0x00000020, WORD)			// Get only tag to query color control support
MKTAG (PIP_BRIGHTNESS,					PIP_UNIT, 0x00000021, WORD)
MKTAG (PIP_CONTRAST,						PIP_UNIT, 0x00000022, WORD)
MKTAG (PIP_SATURATION,					PIP_UNIT, 0x00000023, WORD)
MKTAG (PIP_HUE,							PIP_UNIT, 0x00000024, WORD)
MKTAG (PIP_GAMMA,							PIP_UNIT, 0x00000025, WORD)			// Gamma correction
MKTAG (PIP_SHARPNESS,					PIP_UNIT, 0x00000026, WORD)
MKTAG (PIP_COLORENABLE,					PIP_UNIT, 0x00000027, BOOL)			// Used to toggle between B/W and color

// Use these to adjust brightness individually for R/G/B (only works with special VGAs)
MKTAG (PIP_RED_BRIGHTNESS,				PIP_UNIT, 0x00000028, WORD)
MKTAG (PIP_BLUE_BRIGHTNESS,			PIP_UNIT, 0x00000029, WORD)
MKTAG (PIP_GREEN_BRIGHTNESS,			PIP_UNIT, 0x0000002a, WORD)

// Used to crop the source rectangle when ADAPT_SOURCE_SIZE is TRUE
MKTAG (PIP_SOURCE_CROP_LEFT,			PIP_UNIT, 0x0000002b, WORD)
MKTAG (PIP_SOURCE_CROP_TOP,			PIP_UNIT, 0x0000002c, WORD)
MKTAG (PIP_SOURCE_CROP_RIGHT,			PIP_UNIT, 0x0000002d, WORD)
MKTAG (PIP_SOURCE_CROP_BOTTOM,		PIP_UNIT, 0x0000002e, WORD)



// Error definitions.
#define GNR_DMA_PIP_NOT_POSSIBLE			MKERR(ERROR, PIP, GENERAL, 0x01)		// Current screen mode does not allow DMA PIP
#define GNR_VIDEO_STD_NOT_SUPPORTED		MKERR(ERROR, PIP, GENERAL, 0x02)		// Desired video standard (e.g. SECAM) not supported.
#define GNR_PIP_NOT_RUNNING				MKERR(ERROR, PIP, GENERAL, 0x03)		// PIP is not running when trying to execute OptimizeBuffer()
#define GNR_NO_OPTIMAL_MINPIX				MKERR(ERROR, PIP, GENERAL, 0x04)		// No optimal minPix value could be found by OptimizeBuffer()
#define GNR_DEST_RECT_SIZE					MKERR(ERROR, PIP, GENERAL, 0x05)		// Destination rectangle too small or too big.
#define GNR_WRONG_GRAB_SIZE				MKERR(ERROR, PIP, GENERAL, 0x06)		// Dimensions of frame to grab are too big.
#define GNR_NO_VBLANK_IRQ					MKERR(ERROR, PIP, GENERAL, 0x07)		// An expected VBlank IRQ did not occur.
#define GNR_GRAB_TIMEOUT					MKERR(ERROR, PIP, GENERAL, 0x08)		// A timeout occurred during grabbing a frame.
#define GNR_WRONG_GRAB_FORMAT				MKERR(ERROR, PIP, GENERAL, 0x09)		// An unsupported capture format was requested
#define GNR_SAMPLEMODE_NOT_SUPPORTED	MKERR(ERROR, PIP, GENERAL, 0x0a)		// Desired video sample mode not supported
#define GNR_VP_ERROR							MKERR(ERROR, PIP, GENERAL, 0x0b)		// General error with Video Port
#define GNR_VP_NO_PORT_MATCH				MKERR(ERROR, PIP, GENERAL, 0x0c)		// Video source does not match Video Port
#define GNR_VP_HARDWARE_IN_USE			MKERR(ERROR, PIP, GENERAL, 0x0d)		// Video Port Hardware in use
#define GNR_VPM_ERROR						MKERR(ERROR, PIP, GENERAL, 0x0e)		// General error with VPM services
#define GNR_VPE_ERROR						MKERR(ERROR, PIP, GENERAL, 0x0f)		// General error with DirectX5/VPE services
#define GNR_NOT_ENOUGH_VIDEO_MEMORY		MKERR(ERROR, PIP, MEMORY,  0x10)
#define GNR_NO_OVERLAY_HARDWARE			MKERR(ERROR, PIP, GENERAL, 0x11)
#define GNR_OVERLAY_BUSY					MKERR(ERROR, PIP, BUSY,    0x12)
#define GNR_PRIMARY_NOT_AVAILABLE		MKERR(ERROR, PIP, BUSY,		0x13)



#if NT_KERNEL || VXD_VERSION || DRVDLL

struct WinDescRect
	{
	BOOL	include;
	RECT	pos;
	};

typedef struct WinDesc
	{
	RECT				frame;
	int				clips;
	WinDescRect	*	clip;
	} * WinDescPtr;

MKTAG(PIP_WINDOW_DESC,		PIP_UNIT,	0x00000102, WinDescPtr)

#endif



typedef struct StreamCaptureMsgStruct
	{
	FPTR bufferInfo;
	FPTR userData;
	DWORD captureTime;
	} StreamCaptureMsg;

MKHOOK(StreamCapture, StreamCaptureMsg);



typedef struct YStreamCaptureMsgStruct
	{
	FPTR bufferInfo;
	FPTR userData;
	WORD yCaptureCounter;
	} YStreamCaptureMsg;

MKHOOK(YStreamCapture, YStreamCaptureMsg);



#ifndef ONLY_EXTERNAL_VISIBLE



	class PrepInfoPIP : public LinkedQueueElement
		{
		// Extend this class to add more preparation info.
		public:
			KernelMemory kernelMem;
			FPTR bufferPtr;   // Kernel space
			FPTR userData;

			PrepInfoPIP (void) {}
			virtual ~PrepInfoPIP (void) {Unprepare ();}

			virtual Error Prepare (FPTR buffer, DWORD size);
			virtual Error Unprepare (void);

			FPTR KernelSpace (void) {return kernelMem.KernelSpace();}
		};



class PIP : public PhysicalUnit
	{
	friend class VirtualPIP;

	protected:
		BOOL pipIsPossible;

		virtual Error SetVideoStandard(VideoStandard std) = 0;
		virtual Error SetSampleMode(VideoSampleMode mode) = 0;
		virtual Error SetHOffset(int offset) = 0;
		virtual Error SetVOffset(int offset) = 0;
#if !NT_KERNEL && !VXD_VERSION
		virtual Error SetWindow(HWND win) = 0;
#endif
		virtual Error SetSourceRect(int left, int top, int width, int height) = 0;
		virtual Error SetDestRect(int left, int top, int width, int height) = 0;

		virtual int GetDisplayLeft(void) = 0;
		virtual int GetDisplayTop(void) = 0;
		virtual int GetDisplayWidth(void) = 0;
		virtual int GetDisplayHeight(void) = 0;

		virtual Error EnablePIP(BOOL enable) = 0;
#if WDM_VERSION
		virtual Error VirtuallyEnablePIP(BOOL enable) = 0;
		virtual Error VideoStreamOn() = 0;
		virtual Error VideoStreamOff() = 0;
#endif
		virtual Error GrabFrame(FPTR base, WORD width, WORD height,
		                        WORD stride, GrabFormat fmt) = 0;
		virtual Error SetAdaptClientSize(BOOL adaptClientSize) = 0;
		virtual Error SetAdaptSourceSize(BOOL adaptSourceSize) = 0;
		virtual Error SetScreenDest(BOOL screenDest) = 0;

#if NT_KERNEL || VXD_VERSION
		virtual Error SetWinDesc(WinDesc * wd) = 0;
#endif

#if !NT_KERNEL && !VXD_VERSION
		virtual Error SetColorkeyColor(DWORD color) {GNRAISE_OK;}
		virtual Error SetColorkeyIndex(DWORD index) {GNRAISE_OK;}
		virtual Error GetColorkeyed(BOOL __far & keyed) {keyed = FALSE; GNRAISE_OK;}
		virtual Error SetAlwaysOverlay(BOOL alwaysOverlay) {GNRAISE_OK;}
#endif
		virtual Error SetOffscreenOverride(BOOL offscreen) = 0;
		virtual Error SetOffscreenBase(FPTR base) = 0;
		virtual Error SetOffscreenBytesPerRow(WORD bpr) = 0;
		virtual Error SetOffscreenPixelFormat(PIPFormat format) = 0;
		virtual Error SetFieldDisplayHook(FieldDisplayHookHandle hook) {if (hook) GNRAISE(GNR_UNIMPLEMENTED); else GNRAISE_OK;}

		virtual Error SetSourceCrop(int left, int top, int right, int bottom) = 0;

		virtual Error Update(void);


		// Streaming capture mechanism.
		virtual Error InitGrabStream (StreamCaptureHookHandle hook, DWORD milliSecPerFrame,
		                              WORD width, WORD height, WORD stride, GrabFormat format);
		virtual Error FinishGrabStream (void);
		virtual Error PrepareGrabBuffer (FPTR buffer, DWORD size, FPTR __far &info);
		virtual Error UnprepareGrabBuffer (FPTR info);
		virtual Error AddStreamGrabBuffer (FPTR info, FPTR userData);
		virtual Error StartGrabStream (void);
		virtual Error StopGrabStream (void);
		virtual PrepInfoPIP *CreatePrepInfo (void);

		typedef struct CaptureParameters
			{
			StreamCaptureHookHandle hook;
			DWORD milliSecPerFrame;
			WORD width, height, stride;
			GrabFormat format;
			LinkedQueue capturePipe;
			int nBuffers;
			BOOL grabRunning;
			BOOL firstFrameIsHere;
			DWORD nextFrameNum;
			DWORD nextFrameTime;
			};
		CaptureParameters capture;


		// Y capture streaming.
		virtual Error StartGrabYStream (YStreamCaptureHookHandle hook, int left, int top, WORD width, WORD height) = 0;
		virtual Error StopGrabYStream (void) = 0;
		Error YDequeue (LinkedQueueElement __far * __far &element);


		virtual Error UpdateDisplay(void);
	public:
		PIP (void) {pipIsPossible = TRUE;}

		virtual Error Initialize(void);
		virtual Error ReInitialize(void) {GNRAISE_OK;}

		virtual Error SuspendDMA(void) {GNRAISE_OK;}
		virtual Error ResumeDMA(void) {GNRAISE_OK;}
	};



class VirtualPIP : public VirtualUnit
	{
	friend class PIP;

	protected:
		VideoStandard		standard;

#if NT_KERNEL || VXD_VERSION
		WinDesc			*	windesc;
#else
		HWND					window;
#endif

		int					sourceLeft, sourceTop, sourceWidth, sourceHeight;
		int					destLeft, destTop, destWidth, destHeight;
		WORD					sourceCropLeft, sourceCropTop, sourceCropRight, sourceCropBottom;
		BOOL					pipEnabled;
		BOOL					adaptClientSize;
		BOOL					adaptSourceSize;
		BOOL					screenDest;

#if !NT_KERNEL && !VXD_VERSION
		HANDLE				saveImage;
#else
		FPTR					saveImage;
#endif

		VideoSampleMode   sampleMode;
		int hOffset, vOffset;

		LinkedQueue yCapturePipe;
		YStreamCaptureHookHandle grabYHook;
		int 					grabYLeft;
		int 					grabYTop;
		WORD 					grabYWidth;
		WORD					grabYHeight;

		BOOL					alwaysOverlay;
		DWORD					keyColor;
		DWORD					keyIndex;

#if NT_KERNEL
		BOOL					virtualEnable;
#endif

		BOOL					offscreenOverride;
		FPTR					offscreenBase;
		WORD					offscreenBPR;
		PIPFormat			offscreenFormat;

		FieldDisplayHookHandle	fieldDisplayHook;

		virtual Error PreserveDisplay(void);
	private:
		PIP*				unit;
	public:
		VirtualPIP(PIP* physical);
		~VirtualPIP(void);

		Error Configure(TAG __far * tags);
		Error Preempt(VirtualUnit* previous);
		Error EnablePIP(BOOL enable);

#if NT_KERNEL
		Error VirtuallyEnablePIP(BOOL enable);
		Error VideoStreamOn();
		Error VideoStreamOff();
#endif

		Error GrabFrame(FPTR base, WORD width, WORD height,
		                WORD stride, GrabFormat fmt);

		Error AdaptClientSize(int destLeft, int destTop, int destWidth, int destHeight);
		Error AdaptSourceSize(int sourceLeft, int sourceTop, int sourceWidth, int sourceHeight);


		Error InitGrabStream (StreamCaptureHook *hook, DWORD milliSecPerFrame,
		                      WORD width, WORD height, WORD stride, GrabFormat format);
		Error FinishGrabStream (void);
		Error PrepareGrabBuffer (FPTR buffer, DWORD size, FPTR __far &info)
			{return unit->PrepareGrabBuffer (buffer, size, info);}
		Error UnprepareGrabBuffer (FPTR info)
			{return unit->UnprepareGrabBuffer (info);}
		Error AddStreamGrabBuffer (FPTR info, FPTR userData);
		Error StartGrabStream (void);
		Error StopGrabStream (void);


		virtual Error InitGrabYStream (void);
		virtual Error FinishGrabYStream (void);
		Error StartGrabYStream (YStreamCaptureHook *hook, int left, int top, WORD width, WORD height);
		Error StopGrabYStream(void);
		Error SupplyYStreamGrabBuffer (FPTR info, FPTR userData);


#if NT_KERNEL || VXD_VERSION
		virtual Error GetDMASizes (int &dmaLeft, int &dmaTop, int &dmaWidth, int &dmaHeight) = 0;
		virtual Error UpdateDisplay(FPTR & rDIB, BOOL & rAdaptClientSize, int & rDestLeft, int & rDestTop, int & rDestWidth, int & rDestHeight);
#if WDM_VERSION
		virtual Error PassivateDone() = 0;
#endif
#else
		virtual Error UpdateDisplay(void);
#endif
	};



inline Error VirtualPIP::AdaptClientSize(int destLeft, int destTop, int destWidth, int destHeight)
	{
	if (destLeft   != this->destLeft  ||
	    destTop    != this->destTop   ||
	    destWidth  != this->destWidth ||
	    destHeight != this->destHeight)
		{
		this->destLeft = destLeft;
		this->destTop = destTop;
		this->destWidth = destWidth;
		this->destHeight = destHeight;

		if (GetState() == current)
			GNREASSERT(unit->SetDestRect(destLeft, destTop, destWidth, destHeight));
		}

	GNRAISE_OK;
	}



inline Error VirtualPIP::AdaptSourceSize(int sourceLeft, int sourceTop, int sourceWidth, int sourceHeight)
	{
	if (sourceLeft   != this->sourceLeft  ||
	    sourceTop    != this->sourceTop   ||
	    sourceWidth  != this->sourceWidth ||
	    sourceHeight != this->sourceHeight)
		{
		this->sourceLeft = sourceLeft;
		this->sourceTop = sourceTop;
		this->sourceWidth = sourceWidth;
		this->sourceHeight = sourceHeight;

		if (GetState() == current)
			GNREASSERT(unit->SetSourceRect(sourceLeft, sourceTop, sourceWidth, sourceHeight));
		}

	GNRAISE_OK;
	}



#endif

#endif

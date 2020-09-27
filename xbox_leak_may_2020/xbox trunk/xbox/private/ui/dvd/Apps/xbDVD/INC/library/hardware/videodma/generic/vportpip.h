// FILE:      library\hardware\videodma\generic\vportpip.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1997 Viona Development GmbH. All Rights Reserved.
// CREATED:   23.04.97
//
// PURPOSE:   Base PIP class for Video Port based live video display.
//
// HISTORY:

#ifndef VPORTPIP_H
#define VPORTPIP_H

#include "pip.h"
#include "library\hardware\video\generic\videvice.h"
#include "library\lowlevel\intrctrl.h"

#if !NT_KERNEL && !VXD_VERSION
#include "library\support\VDSup32\VDSup32.h"
#include "library\common\winports.h"
#include "..\generic\clrkeymg.h"
#endif

#define VP_VBLANK_TIMEOUT 100				// Time in [ms] until a VSYNC IRQ must have occurred.
#define VP_GRAB_TIMEOUT	  1000			// Time in [ms] until grabbing of one frame must be finished.

class VideoPortController
	{
	public:
		virtual Error EnableVideoPort(const VideoPortCaps __far & caps) = 0;
		virtual Error DisableVideoPort(void) = 0;		
	};
	
class VideoPortPIP : public PIP, public DDColorkeyManager
	{
	friend class VirtualVideoPortPIP;
	friend class VPPIPVBlankIntHandler;

	protected:
		friend DWORD _export FAR PASCAL VPSizeHookProc(WORD code, WPARAM wParam, LPARAM lParam);

		VideoPortController	*	videoPortController;
		
		BOOL	changed, displayChanged;

		BOOL	pipEnabled;
		BOOL	transferRunning;

		BOOL	adaptSourceSize;
		BOOL	adaptClientSize;
		
		int	srcLeft, srcTop;												// Source rectangle dimensions
		WORD  srcWidth, srcHeight;
	   int	dstLeft, dstTop, dstWidth, dstHeight;					// Destination rectangle dimensions

		int	srcCropLeft, srcCropTop, srcCropRight, srcCropBottom;

		VideoSampleMode			sampleMode;
		VideoStandard				curVideoStd;

		int							hOffset, vOffset;
		WORD							activeLeft, activeTop, activeWidth, activeHeight;	// Active (i.e. usable) part of input field
		int							hTrigger;
		BIT							hsyncPol, vsyncPol;
		VideoFieldIndication		fieldIndication;
		BOOL							pixelClockEdge;
		BOOL							stillSequence;
		DWORD							contentType;

		const HardVideoParams *	curVideoParams;

#if NT_KERNEL || VXD_VERSION
		WinDesc	*	wd;
#else
		HWND			win;
		HHOOK			hook;
		HINSTANCE	hinst;
#endif

		FieldDisplayHookHandle	fieldDisplayHook;

		Error SetVideoStandard(VideoStandard std);
		Error SetSampleMode(VideoSampleMode mode);
		Error SetHOffset(int offset);
		Error SetVOffset(int offset);

		Error SetAdaptClientSize(BOOL adaptClientSize);
		Error SetAdaptSourceSize(BOOL adaptSourceSize);

		Error SetSourceRect(int left, int top, int width, int height);
		Error SetDestRect(int left, int top, int width, int height);

#if !NT_KERNEL && !VXD_VERSION
  		BOOL	colorKeyed;

		Error SetColorkeyColor(DWORD color);
		Error SetColorkeyIndex(DWORD index);
		Error GetColorkeyed(BOOL __far & keyed);

		Error SetAlwaysOverlay(BOOL alwaysOverlay) {GNRAISE_OK;}

		Error DrawColorkey(void) {GNRAISE(DDColorkeyManager::DrawColorkey(win));}
		Error DrawBlack(void) {GNRAISE(DDColorkeyManager::DrawBlack(win));}
		virtual Error RedrawBackground(void) {return DrawColorkey();}
#endif

		Error SetScreenDest(BOOL screenDest)					{GNRAISE_OK;}
		Error SetOffscreenOverride(BOOL offscreen)			{GNRAISE_OK;}
		Error SetOffscreenBase(FPTR base)						{GNRAISE_OK;}
		Error SetOffscreenBytesPerRow(WORD bpr)				{GNRAISE_OK;}
		Error SetOffscreenPixelFormat(PIPFormat format)		{GNRAISE_OK;}

		Error SetFieldDisplayHook(FieldDisplayHookHandle hook);

#if !NT_KERNEL && !VXD_VERSION
		Error SetWindow(HWND win);
#endif
#if NT_KERNEL || VXD_VERSION
		Error SetWinDesc(WinDesc * wd);
#endif

		Error SetSourceCrop(int left, int top, int right, int bottom);

		Error EnablePIP(BOOL enable);

		Error Update(void);
		Error UpdateDisplay(void);

   	// IRQ Handling
		VPPIPVBlankIntHandler	* vbiHandler;			// VBlank handler.
		InterruptServer 			* vbiServer;
		DWORD volatile 			vBlankCount;			// Tells us, how many vertical blank IRQs happened.

		virtual void VBlankInterrupt(void);				// VBlank (VSYNC) IRQ routine.
  		Error WaitVBlankIRQ(WORD count);					// Wait for the number of "count" VSYNC IRQs to occur.


		//
		// Functions to be defined in successors:
		//
		virtual Error StartVideoDisplay(void) = 0;
		virtual Error StopVideoDisplay(void) = 0; 
		virtual Error MovePIP(void) = 0;
		virtual Error SizePIP(void) = 0;

		virtual WORD  GetColorControl() = 0;
		virtual Error SetColorControlValues(ColorControlValues __far & clrCtrl) = 0;
		
		virtual Error StillSequenceChanged(void) {GNRAISE_OK;}
		virtual Error ContentTypeChanged(void)	{GNRAISE_OK;}

/*
		virtual Error SetCCBrightness(WORD ccBrightness) = 0;
		virtual Error SetCCContrast(WORD ccContrast) = 0;
		virtual Error SetCCSaturation(WORD ccSaturation) = 0;
		virtual Error SetCCHue(WORD ccHue) = 0;
		virtual Error SetCCGamma(WORD ccGamma) = 0;
		virtual Error SetCCColorEnable(BOOL ccColorEnable) = 0;
*/		
		virtual Error NotifyUnitCreate(void) = 0;
		virtual Error NotifyUnitDelete(void) = 0;
	public:
		BOOL	canSendDisplayChange;
	
		VideoPortPIP(HINSTANCE hinst, InterruptServer * vBlankIntServer,
						 VideoPortController	*	videoPortController = 0);
		~VideoPortPIP();
		VirtualUnit * CreateVirtual(void);

		Error Configure(TAG __far * tags);

		virtual Error Reconfigure(void) = 0;
		virtual Error RegainSurface(void) {GNRAISE_OK;}

		// Use this to get a valid connection to the port. If error, then the port PIP cannot be used.
		virtual Error MatchPortCaps(const VideoPortCaps __far & inCaps, VideoPortCaps __far & usedCaps) = 0;

		Error ReInitialize(void);

		HardVideoParams 			ntscSQP, palSQP, ntscCCIR, palCCIR;
	};


	
class VirtualVideoPortPIP : public VirtualPIP, public WinPort
	{
	friend class VideoPortPIP;
	
	private:
		VideoPortPIP	*	unit;
	
	protected:
		void Message(WPARAM wParam, LPARAM dParam);
		
		ColorControlValues clrCtrl;
		
	public:
		VirtualVideoPortPIP(VideoPortPIP	* unit);
		~VirtualVideoPortPIP(void) {unit->NotifyUnitDelete();}

		Error Configure(TAG __far * tags);

      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);
	};


#endif



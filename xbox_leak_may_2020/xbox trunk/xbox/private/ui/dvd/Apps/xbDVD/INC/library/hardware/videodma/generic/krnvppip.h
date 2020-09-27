// FILE:      library\hardware\videodma\generic\krnvppip.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1997 Viona Development GmbH. All Rights Reserved.
// CREATED:   05.11.1997
//
// PURPOSE:   Kernel Mode Video Port PIP class
//
// HISTORY:

#ifndef KRNVPPIP_H
#define KRNVPPIP_H

#include "library\common\prelude.h"
#include "library\common\hooks.h"
#include "pip.h"
#include "library\hardware\video\generic\videvice.h"


//
//	Definition of tags
// 

// Defined VP events

#define VP_EVENT_ACTIVATE_PORT				MKFLAG(0)
#define VP_EVENT_PASSIVATE_PORT				MKFLAG(1)
#define VP_EVENT_CHANGED_PORT_PARAMS		MKFLAG(2)
#define VP_EVENT_CHANGED_STILL_SEQUENCE	MKFLAG(3)
#define VP_EVENT_CHANGED_CONTENT_TYPE		MKFLAG(4)

struct VPEventMsg
	{
	DWORD		vpEvent;
	DWORD		data0, data1, data2, data3;
	DWORD		signalEvent;
	};
	
MKHOOK(VPEvent, VPEventMsg);

MKTAG (PIP_VP_EVENT_HOOK,		PIP_UNIT, 0x0060, VPEventHook *)

// See vidtypes.h for an explanation of types
MKTAG (PIP_VP_AVAIL_PROTOCOLS,		PIP_UNIT, 0x0061, DWORD)
MKTAG (PIP_VP_PROTOCOL,					PIP_UNIT, 0x0062, DWORD)
MKTAG (PIP_VP_AVAIL_CONNFEATURES,	PIP_UNIT, 0x0063, DWORD)
MKTAG (PIP_VP_CONNFEATURES,			PIP_UNIT, 0x0064, DWORD)

#define UMDLL_VERSION_1		0
#define UMDLL_VERSION_2		1

MKTAG	(PIP_VP_USERMODEDLL_VERSION,	PIP_UNIT, 0x0065, DWORD)

#ifndef ONLY_EXTERNAL_VISIBLE

class KernelVideoPortPIP : public PIP
	{
	friend class VirtualKernelVideoPortPIP;

	protected:
		KEVENT	signalEvent;

		BOOL	changed;

		BOOL	userDLLVersion;

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

		VPEventHookHandle			vpEventHook;

		ColorControlValues		clrCtrl;

		DWORD	availConnFeatures;
		DWORD availProtocols;

		DWORD	connFeatures;
		DWORD	protocol;

		BOOL	vpAlwaysEnabled;

		ColorController * colorController;

		Error SetVideoStandard(VideoStandard std);
		Error SetSampleMode(VideoSampleMode mode);

		Error SetHOffset(int offset);
		Error SetVOffset(int offset);

		Error SetAdaptClientSize(BOOL adaptClientSize);
		Error SetAdaptSourceSize(BOOL adaptSourceSize);

		Error SetSourceRect(int left, int top, int width, int height);
		Error SetDestRect(int left, int top, int width, int height);

		int	GetDisplayLeft(void) {return (int) dstLeft;}
		int	GetDisplayTop(void)  {return (int) dstTop;}
		int	GetDisplayWidth(void) {return (int) dstWidth;}
		int	GetDisplayHeight(void) {return (int) dstHeight;}
		
		Error EnablePIP(BOOL enable);

		Error VirtuallyEnablePIP(BOOL enable);
		Error VideoStreamOn();
		Error VideoStreamOff();

		Error Update(void);
		Error UpdateDisplay(void);

		Error GrabFrame(FPTR base, WORD width, WORD height, 
		                WORD stride, GrabFormat fmt) {GNRAISE_OK;}
		
		Error SetScreenDest(BOOL screenDest) {GNRAISE_OK;}

		Error SetWinDesc(WinDesc * wd) {GNRAISE_OK;}

		Error SetOffscreenOverride(BOOL offscreen)			{GNRAISE_OK;}
		Error SetOffscreenBase(FPTR base)						{GNRAISE_OK;}
		Error SetOffscreenBytesPerRow(WORD bpr)				{GNRAISE_OK;}
		Error SetOffscreenPixelFormat(PIPFormat format)		{GNRAISE_OK;}

		Error SetSourceCrop(int left, int top, int right, int bottom);

		Error StartGrabYStream (YStreamCaptureHookHandle hook, int left, int top, WORD width, WORD height) {GNRAISE_OK;}
		Error StopGrabYStream (void) {GNRAISE_OK;}

		Error NotifyUnitCreate(void);
		Error NotifyUnitDelete(void);

		virtual Error StartVideoDisplay(void) = 0;
		virtual Error StopVideoDisplay(void) = 0;

		virtual DWORD GetAvailableProtocols() {return availProtocols;}
		virtual DWORD GetAvailableConnFeatures() {return availConnFeatures;}

		virtual Error SetUserDLLVersion(DWORD userDLLVersion) {this->userDLLVersion = userDLLVersion; GNRAISE_OK;}

		virtual Error SetProtocol(DWORD protocol);
		virtual Error SetConnFeatures(DWORD connFeatures);

		virtual Error SetVPEventHook(VPEventHookHandle vpEventHook) {this->vpEventHook = vpEventHook; changed = TRUE; GNRAISE_OK;}

		virtual WORD  GetColorControl();
		virtual Error SetColorControlValues(ColorControlValues __far & clrCtrl);

		virtual Error StillSequenceChanged(void);
		virtual Error ContentTypeChanged(void);
	public:
		KernelVideoPortPIP(BOOL vpAlwaysEnabled,
								 DWORD availConnFeatures,
								 DWORD availProtocols,
								 ColorController * colorController = NULL);

		~KernelVideoPortPIP();
		VirtualUnit * CreateVirtual(void);

		Error Configure(TAG __far * tags);

		virtual Error Reconfigure(void) {GNRAISE_OK;}

		Error ReInitialize(void);

		HardVideoParams 			ntscSQP, palSQP, ntscCCIR, palCCIR;
	};


	
class VirtualKernelVideoPortPIP : public VirtualPIP
	{
	friend class KernelVideoPortPIP;
	
	private:
		KernelVideoPortPIP	*	unit;

		BOOL	userDLLVersion;
		
		DWORD protocol;
		DWORD connFeatures;

		ColorControlValues clrCtrl;

	public:
		VPEventHookHandle vpEventHook;
		KEVENT activateDoneEvent;

		VirtualKernelVideoPortPIP(KernelVideoPortPIP	* unit);
		~VirtualKernelVideoPortPIP(void) {unit->NotifyUnitDelete();}

		Error Configure(TAG __far * tags);

      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);

		Error GetDMASizes (int &dmaLeft, int &dmaTop, int &dmaWidth, int &dmaHeight) {GNRAISE_OK;}

		Error PassivateDone();
	};

#endif

#endif



// FILE:      library\hardware\videodma\specific\vpepip.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1997 Viona Development GmbH. All Rights Reserved.
// CREATED:   12.08.97
//
// PURPOSE:   PIP class for DirectX 5/VPE controlled video ports. Implementation.
//
// HISTORY:

#ifndef VPEPIP_H
#define VPEPIP_H

#if !NT_KERNEL && !VXD_VERSION
#include <windows.h>
#include "library\lowlevel\memmpdio.h"
#endif

#include "..\generic\vportpip.h"
#include "..\generic\clrkeymg.h"
#include "library\common\krnlsync.h"

#include "library\common\profiles.h"

class VPEPIP : public VideoPortPIP
	{
	friend class VirtualVPEPIP;

	private:
#if !NT_KERNEL && !VXD_VERSION
		DDSURFACEDESC	desc;
		DDAccess			ddAccess;

		BOOL				colorKeyed;
		BOOL				alwaysOverlay;

		VideoPortCaps	decoderCaps;
		
		char				messageTitle[32];
#endif
		BOOL				embeddedSyncs;

		int 				transferSuspended;
                     
		PrepInfoPIP		*captureBuffer;

		DWORD				yCaptureDMADWordsPerRow;
//		DWORD				yCaptureDWordsPerRow;
		WORD				yCaptureHeight;
		FPTR				yCaptureBuffer;

		YStreamCaptureHookHandle	yCaptureHook;
		
		StreamCaptureHookHandle		captureHook;

		long				dispLeft, dispTop, dispWidth, dispHeight;

		long				dWidth, dHeight;
		long				xOffset, yOffset;

		WORD				screenWidth, screenHeight;
		
		Profile			*profile;
		
		BOOL				vpeDisplayRunning;
		BOOL				restartVPE;
		BOOL				vpeDisplayInUse;
		
		MemoryMappedIO	nvidiaRegs;
		BOOL				nvidiaColorPatch;
		WORD				nvidiaBrightness;
		WORD				nvidiaRedBrightness;
		WORD				nvidiaBlueBrightness;
		WORD				nvidiaGreenBrightness;
		int				nvidiaBaseR;
		int				nvidiaBaseG;
		int				nvidiaBaseB;
		BOOL				nvidiaTable;
		BOOL				nvidiaDelayTime;
		BOOL				permedia2Patch;
		
		BOOL				needPaintMsg;
		
		BOOL				nvidiaFixBob;

		int				numVirtualUnits;
		
		ColorController	* colorController;
	protected:
		int GetDisplayLeft(void);
		int GetDisplayTop(void);
		int GetDisplayWidth(void);
		int GetDisplayHeight(void);
		
		Error GrabFrame(FPTR base, WORD width, WORD height, WORD stride, GrabFormat fmt);

		// Y capture streaming.
		Error StartGrabYStream (YStreamCaptureHookHandle hook, int left, int top, WORD width, WORD height);
		Error StopGrabYStream (void);

		Error Update(void);

		Error DeferedStartVPEDisplay(void);
		Error StartVPEDisplay(void);
		Error StopVPEDisplay(void);   
		
		Error RegainSurface(void);
		
		Error StartVideoDisplay(void);
		Error StopVideoDisplay(void);
		Error MovePIP(void);
		Error SizePIP(void);

		WORD  GetColorControl();
		Error SetColorControlValues(ColorControlValues __far & clrCtrl);
		
		Error StillSequenceChanged(void);
		Error ContentTypeChanged(void);
		
		Error NotifyUnitCreate(void);
		Error NotifyUnitDelete(void);

		Error RedrawBackground(void);
		Error PaintNoOverlayMsg(void);
	public:
		VPEPIP(HINSTANCE hinst, InterruptServer * vblankIntServer, Profile * profile,
				 VideoPortController	*	videoPortController = 0,
				 ColorController * colorController = 0);
		~VPEPIP(void);

		VirtualUnit * CreateVirtual(void);

		virtual Error Reconfigure(void);

		// Use this to get a valid connection to the port. If error, then the port PIP cannot be used.
		virtual Error MatchPortCaps(const VideoPortCaps __far & inCaps, VideoPortCaps __far & usedCaps);

		Error Initialize(void);
		
		Error SuspendDMA(void);
		Error ResumeDMA(void);
	};


	
class VirtualVPEPIP : public VirtualVideoPortPIP
	{
	friend class VPEPIP;
	
	private:
		VPEPIP	*	unit;
	
		void Message(WPARAM wParam, LPARAM dParam);
	public:
		VirtualVPEPIP(VPEPIP	* unit) : VirtualVideoPortPIP(unit) {this->unit = unit;}
	};


#endif


// FILE:      library\hardware\videodma\generic\vpmpip.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1997 Viona Development GmbH. All Rights Reserved.
// CREATED:   25.04.97
//
// PURPOSE:   PIP class for VPM controlled video ports.
//
// HISTORY:

#ifndef VPMPIP_H
#define VPMPIP_H

#if !NT_KERNEL && !VXD_VERSION
#include <windows.h>
#endif

#include "..\generic\vportpip.h"

//#include "vpm.h"
#include "y:\vpmsdk\vpm.h"
#include "library\common\profiles.h"

class VPMPIP : public VideoPortPIP
	{
	friend class VirtualVPMPIP;

	private:
#if !NT_KERNEL && !VXD_VERSION
		HINSTANCE		vpmHandle;
		LPVPMDRIVERPROC vpmProc;

		DDSURFACEDESC	desc;

		BOOL				alwaysOverlay;

		VideoPortCaps	decoderCaps;

		LPVPMPROVIDER	vpmProvider;
		LPVPMSTREAM		stream;			// The VPM stream
		VPMCONFIG		streamConfig;	// The valid VPM configuration for our stream

		DWORD				globalCapFlags;
		DWORD				usedPort;
		
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
		
		BOOL				vpmDisplayRunning;
		
		BOOL				verticalOffset;
		BOOL				tsengSpecials;
		BOOL				s3Specials;
		BOOL				enableColorControl;
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

		Error StartVPMDisplay(void);
		Error StopVPMDisplay(void);
		
		Error StartVideoDisplay(void);
		Error StopVideoDisplay(void);
		Error MovePIP(void);
		Error SizePIP(void);

		// VPM Helper functions
		Error	CalculateVPMSettings(void);
		void	CalculateVPMSrcRect(void);

		Error AdaptNSetVPMSettings(void);

		BOOL	KillAField(void);

		long 	HalveWidth();
		long 	HalveHeight();
		long	ReduceMemoryConsumption();

		Error FindPortMatch(VPMCONFIG __far & inConfig,
								  VPMCONFIG __far & outConfig,
								  const VideoPortCaps __far & inCaps,
								  VideoPortCaps __far & usedCaps);
		
		BOOL	CheckPort(DWORD type, VPMCONFIG __far & inConfig, VPMCONFIG __far & outConfig);

		WORD  GetColorControl();
		Error SetColorControlValues(ColorControlValues __far & clrCtrl);

		int	numVirtualUnits;

		Error NotifyUnitCreate(void);
		Error NotifyUnitDelete(void);
	public:
		VPMPIP(HINSTANCE hinst, InterruptServer * vblankIntServer, Profile * profile);
		~VPMPIP(void);

		VirtualUnit * CreateVirtual(void);

		virtual Error Reconfigure(void);

		// Use this to get a valid connection to the port. If error, then the port PIP cannot be used.
		virtual Error MatchPortCaps(const VideoPortCaps __far & inCaps, VideoPortCaps __far & usedCaps);

		Error Initialize(void);
		
		Error SuspendDMA(void);
		Error ResumeDMA(void);
	};


	
class VirtualVPMPIP : public VirtualVideoPortPIP
	{
	friend class VPMPIP;
	
	private:
		VPMPIP	*	unit;
	
	public:
		VirtualVPMPIP(VPMPIP	* unit) : VirtualVideoPortPIP(unit) {this->unit = unit;}
	};


#endif


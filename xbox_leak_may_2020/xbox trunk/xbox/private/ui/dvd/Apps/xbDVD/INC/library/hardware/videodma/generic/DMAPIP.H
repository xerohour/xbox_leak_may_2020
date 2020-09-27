
// FILE:      library\hardware\videodma\generic\dmapip.h
// AUTHOR:    Ulrich Sigmund, S. Herr
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   08.04.95
//
// PURPOSE: The PIP-related classes.
//
// HISTORY:

#ifndef DMAPIP_H
#define DMAPIP_H

#include "pip.h"

#if !NT_KERNEL && !VXD_VERSION
#include "..\generic\clrkeymg.h"
#endif


MKTAG (PIP_OVERLAY_IN_USE,				PIP_UNIT, 0x00000020, BOOL)   // overlay instead of inlay in use right now ?



#ifndef ONLY_EXTERNAL_VISIBLE

#include "rectmap.h"

#if !NT_KERNEL && !VXD_VERSION
#include "library\common\winports.h"
#if VIONA_VERSION
#include "library\common\profiles.h"
#include "library\support\VDSup32\VDSup32.h"
#include "library\lowlevel\dmabuffr.h"
#endif

struct HookData {
        LPARAM lParam;
        WPARAM wParam;
        unsigned message;               
        HWND hwnd;      
};
#endif



#if !NT_KERNEL && !VXD_VERSION
class DMAPIP : public PIP, public DDColorkeyManager
#else
class DMAPIP : public PIP
#endif
	{
	friend class VirtualDMAPIP;
	protected:
		enum DMAPIPState
			{
			DPS_IDLE,
			DPS_INLAY,
			DPS_OVERLAY,
			DPS_COPYOVERLAY,
			DPS_CLIPPEROVERLAY,
			DPS_OFFSCREEN
			} state;
		
		BOOL			dmaRunning;
		BOOL			maskRebuild;

#if NT_KERNEL || VXD_VERSION
		WinDesc	*	wd;

		BOOL			colorKeyed;
		DWORD			keyIndex;
		BOOL			alwaysOverlay;
#else
#if VIONA_VERSION
		DDAccess		ddAccess;
		BOOL			colorKeyed;
		BOOL			copyOverlayEnabled;
		BOOL			dmaOverlayEnabled;

		BOOL			noDDClipper;				// Prevents using DirectDraw Clipper Objects
		
		RECT			keyPaintRect;
		
		DWORD			keyColor;
		DWORD			keyIndex;
		DWORD			keyColorRGB;
		DWORD			keyColorIndex;

		Profile		*profile;
		
		BOOL			alwaysOverlay;
		BOOL			overlayPossible, forceOverlay;
		
		WORD			screenWidth, screenHeight;

		DDSURFACEDESC	desc;
#endif
		HWND			win;
		HHOOK 		hook;
		HINSTANCE	hinst;
#endif
	
	   int			dstLeft, dstTop, dstWidth, dstHeight;	   
	   BOOL		 	pipEnabled;           
	   BOOL			adaptClientSize;
		BOOL			screenDest;
		BOOL			destAspectRatio;
					   
		BOOL			changed;
		
		BOOL			offscreenOverride;
		FPTR			offscreenBase;
		DWORD			offscreenPhysicalBase;
		WORD			offscreenBPR;
		PIPFormat	offscreenFormat;
	
		// DMA sizes are the destination sizes aligned to what the DMA needs (e.g. dmaLeft <= dstLeft).
		int			dmaLeft, dmaTop, dmaWidth, dmaHeight;
		int			dispLeft, dispTop, dispWidth, dispHeight;
				
		virtual Error StartDMA(void) = 0;
		virtual Error StopDMA(void) = 0;
        
		Error DrawBlack(void) 
			{
#if !NT_KERNEL && !VXD_VERSION
			GNRAISE(DDColorkeyManager::DrawBlack(win));
#else
			GNRAISE_OK;
#endif
			}
        
		// Functions to build a clip mask.
	   virtual Error BuildDMARegion (void);
		virtual Error InitDMARegion(int left, int top, int width, int height) = 0;
		virtual Error AddDMAClipRect(int left, int top, int width, int height) = 0;	// make rect invisible
		virtual Error RemDMAClipRect(int left, int top, int width, int height) = 0;	// make rect visible
		virtual Error CompleteDMARegion(void) = 0;
#if NT_KERNEL || VXD_VERSION
	public:
		virtual Error SetOffscreenDMARegion (DWORD physical, DWORD stride, PIPFormat format,
		                                     int left, int top, int width, int height)
			{GNRAISE(GNR_UNIMPLEMENTED);}
		virtual Error SetOffscreenDMA (DWORD physical);
		virtual Error CheckForOverlay (WORD targetWidth, WORD targetHeight, WORD &maxDMAWidth, WORD &maxDMAHeight);
#else
		virtual Error SetOffscreenDMARegion(DWORD physical, DWORD stride, PIPFormat format,
		                                    int left, int top, int width, int height)
			{GNRAISE(GNR_UNIMPLEMENTED);}
#endif

	protected:

#if VIONA_VERSION
		virtual int MaxOverlayDestWidth(void) {return MaxDestWidth();}
		virtual int MaxInlayDestWidth(void) {return MaxDestWidth();}

#if !NT_KERNEL && ! VXD_VERSION
		DWORD ConvertKeyColor(DWORD colorRGB, LPDDSURFACEDESC desc);
		DWORD ConvertKeyIndex(DWORD __far & colorIndex);
#endif

#endif		
		virtual int MaxDestWidth(void) = 0;
		virtual int MaxDestHeight(void) = 0;

		virtual int HorizontalPixelsPerMaskBit (void) = 0;
		virtual int VerticalPixelsPerMaskBit (void) = 0;

#if NT_KERNEL || VXD_VERSION
		Error SetWinDesc(WinDesc * wd);
#else
		Error SetWindow(HWND win);
#endif
		Error SetScreenDest(BOOL screenDest);
		Error SetDestRect(int left, int top, int width, int height);
		Error SetAdaptClientSize(BOOL adaptClientSize);

		int GetDisplayLeft(void);
		int GetDisplayTop(void);
		int GetDisplayWidth(void);
		int GetDisplayHeight(void);

#if VIONA_VERSION
#if !NT_KERNEL && ! VXD_VERSION
		Error GetColorkeyed(BOOL __far & keyed);
		Error SetColorkeyColor(DWORD color);
		Error SetColorkeyIndex(DWORD index);
		Error SetAlwaysOverlay(BOOL alwaysOverlay);
#endif
#endif

		Error SetOffscreenOverride(BOOL offscreen);
		Error SetOffscreenBase(FPTR base);
		Error SetOffscreenBytesPerRow(WORD bpr);
		Error SetOffscreenPixelFormat(PIPFormat format);
		
		Error EnablePIP(BOOL enable);
		
		virtual Error Update(void);
		
		Error UpdateDisplay(void);

		int	numVirtualUnits;
		
		Error NotifyUnitCreate(void);
		Error NotifyUnitDelete(void);
	public:

#if NT_KERNEL || VXD_VERSION
		DMAPIP(void);
#else
#if VIONA_VERSION
		DMAPIP(HINSTANCE	hinst, Profile * profile = NULL);
#else
		DMAPIP(HINSTANCE	hinst);
#endif
#endif
		~DMAPIP(void);

		Error	Initialize(void);
		Error ReInitialize(void);

#if !NT_KERNEL	&& !VXD_VERSION

#if VIONA_VERSION
		virtual Error Reconfigure(void);
		virtual BOOL ScreenModeChanged(void);
#endif

		friend int AffectsMask(HookData __far * hd);
		friend DWORD FAR PASCAL SizeHookProc(WORD code, WPARAM wParam, LPARAM lParam);
		friend void DMARegionChanged();

		int   	maskMsgReceived;
#endif
	};



#if NT_KERNEL || VXD_VERSION

class VirtualDMAPIP : public VirtualPIP
	{
	private:
		DMAPIP	*	unit;
	public:
		VirtualDMAPIP(DMAPIP * physical) : VirtualPIP(physical) {unit = physical; unit->NotifyUnitCreate();}
		~VirtualDMAPIP(void) {unit->NotifyUnitDelete();}

		Error Configure (TAG __far *tags);

		virtual Error GetDMASizes (int &dmaLeft, int &dmaTop, int &dmaWidth, int &dmaHeight)
			{
			dmaLeft   = unit->dmaLeft;
			dmaTop    = unit->dmaTop;
			dmaWidth  = unit->dmaWidth;
			dmaHeight = unit->dmaHeight;
			GNRAISE_OK;
			}
	};

#else

class VirtualDMAPIP : public VirtualPIP, public WinPort {
	private:
		DMAPIP	*	unit;

	   void Message(WPARAM wParam, LPARAM dParam);
	public:
		VirtualDMAPIP(DMAPIP * physical) : VirtualPIP(physical), WinPort(physical->hinst) {unit = physical; unit->NotifyUnitCreate();}		
		~VirtualDMAPIP(void) {unit->NotifyUnitDelete();}

		Error Configure (TAG __far *tags);
	};

#endif



class MaskDMAPIP : public DMAPIP {
	friend class VirtualMaskDMAPIP;
	private:
		RectBitMap	*	dmaMap;		
	protected:               
		virtual Error SetMask(RectBitMap * map) = 0;
		
		virtual int HorizontalAlign(void) {return 1;}
		virtual int VerticalAlign(void) {return 1;}
		
		virtual int HorizontalPixelsPerMaskBit(void) {return 1;}
		virtual int VerticalPixelsPerMaskBit(void) {return 1;}

		virtual RectBitMap* CreateDMAMap(int left, int top, int width, int height)
			{return new RectBitMap(left, top, width, height);}									// Default is a non-inverse bitmap!
				
		Error InitDMARegion(int left, int top, int width, int height);
		Error AddDMAClipRect(int left, int top, int width, int height);
		Error RemDMAClipRect(int left, int top, int width, int height);
		Error CompleteDMARegion(void);
		
	public:
#if NT_KERNEL || VXD_VERSION
		MaskDMAPIP(void) : DMAPIP() {dmaMap = NULL; dmaLeft = 0; dmaTop = 0; dmaHeight = 0; dmaWidth = 0;}
#else
#if VIONA_VERSION
		MaskDMAPIP(HINSTANCE hinst, Profile * profile) : DMAPIP(hinst, profile) {dmaMap = NULL; hook = NULL; dmaLeft = 0; dmaTop = 0; dmaHeight = 0; dmaWidth = 0;}
#else
		MaskDMAPIP(HINSTANCE hinst) : DMAPIP(hinst) {dmaMap = NULL; hook = NULL; dmaLeft = 0; dmaTop = 0; dmaHeight = 0; dmaWidth = 0;}
#endif
#endif
		~MaskDMAPIP(void);
	};
	
class VirtualMaskDMAPIP : public VirtualDMAPIP {
	public:
		VirtualMaskDMAPIP(MaskDMAPIP * physical) : VirtualDMAPIP(physical) {}		
	};
	
#endif // ONLY_EXTERNAL_VISIBLE

#endif

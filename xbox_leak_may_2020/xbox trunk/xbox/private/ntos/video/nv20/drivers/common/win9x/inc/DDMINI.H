/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1995,1999 NVidia Corporation. All Rights Reserved.
 *
 *  File:       ddmini.h
 *  Content:    header for Windows95 Direct Draw driver
 *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef NVARCH
ERROR! NVARCH must be defined. Definition of NV3, NV4, NV10, etc. is obsolete.
#endif

#if !defined(_WIN32) && !defined(WINNT)
#define NOUSER
#define NOGDI
#define NOGDIOBJ
#define NOGDICAPMASKS
#include <windows.h>
#define NOPTRC
#define PTTYPE POINT
#include <nvgdi.inc>
#define NVFAR FAR
#include <nvdib.inc>
typedef struct tagPALETTEENTRY FAR* LPPALETTEENTRY;
typedef struct tagRGNDATA      FAR* LPRGNDATA;
typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    };
} ULARGE_INTEGER;
#include <ddrawi.h>
#endif

#include "nvFourCC.h"
#include "nvVPP.h"
#include "nvcm.h"
#include "nvdd.h"

#include "nvUniversal.h"

/***************************************************************************
 *
 * DriverData
 *
 * this structure contains all the globals of the driver, it is shared
 * between the 16 and 32bit side of the driver.
 *
 ***************************************************************************/

#ifndef HDRVEVENT
#include "nvProcMan.h"
#endif

#ifndef NVTWINVIEWDATA
#include "nvMultiMon.h"
#endif

#define DDMINI_VERSION  0x0400

#if (NVARCH < 0x4)
#define NV_MAX_OVERLAY_SURFACES  10
#define NV_MAX_EXTRA_SURFACES    6
#define NV_MAX_EXTRA_FS_SURFACES 4
#endif

//---------------------------------------------------------------------------

typedef struct tagNVMCSURFACEFLAGS {
  DWORD dwMCSurfaceBase;
  DWORD dwMCSurface422Offset;
  DWORD dwMCMotionCompReferenceCount;
  DWORD dwMCFormatCnvReferenceCount;
  DWORD dwMCCompositeReferenceCount;
  DWORD dwMCTemporalFilterReferenceCount;
  BYTE  bMCPictureStructure;
  BYTE  bMCPictureCodingType;
  BYTE  bMCTopFieldFirst;
  BYTE  bMCSecondField;
  BYTE  bMCFrameType;
  BYTE  bMCFrameIsFiltered;
  BYTE  bMCFrameIsComposited;
  BYTE  bMCFrameAlreadyFormatConverted;
} NVMCSURFACEFLAGS, *LPNVMCSURFACEFLAGS;

//---------------------------------------------------------------------------
#ifdef ENABLE_VPP_DISPATCH_CODE

typedef struct {
    DWORD               dwState;    // dispatcher state
    DWORD               dwCommand;  // dispatcher command
    DWORD               dwThreadPriority;
    DWORD               dwProcessPriority;
    DWORD               dwOriginalThreadPriority;
    DWORD               dwOriginalProcessPriority;
    PROCESS_INFORMATION procInfo;
    HDRVEVENT           hSignalGo0;     // ring0 event to dispatch
    HDRVEVENT           hSignalGo3;     // ring3 event to dispatch
    HDRVEVENT           hSignalDone0;   // ring0 event signaling completion
    HDRVEVENT           hSignalDone3;   // ring3 event signaling completion

    // parameters for vppVPPandFlip
    DWORD               dwOffset;
    DWORD               dwPitch;
    DWORD               dwWidth;
    DWORD               dwHeight;
    DWORD               dwFourCC;
    DWORD               dwFlags;
    DWORD               dwReturnCode;
    DWORD               dwReserved1;
} VPPDISPATCH;
#else
// NB: The following # of DWORDS must exactly match the size of the dispatch code
// above.  Someday this can all go away...
typedef DWORD VPPDISPATCH[22];
#endif

//---------------------------------------------------------------------------
#if (NVARCH < 0x4)
typedef struct {
    DWORD               dwLastFlags;
    DWORD               dwLastExec;
    DWORD               dwMarker1;
    DWORD               dwReserved1;

    LONGLONG            qwCount;

    LONGLONG            qwInterFrameTime;
    LONGLONG            qwInterFrameAcc;
    LONGLONG            qwInterFramePeak;
    LONGLONG            qwInterFrameDelta;
    LONGLONG            qwInterFrameJitterAcc;
    LONGLONG            qwInterFrameJitterPeak;

    LONGLONG            qwIntraFrameAcc;
    LONGLONG            qwIntraFramePeak;
    LONGLONG            qwIntraFrameDelta;
    LONGLONG            qwIntraFrameJitterAcc;
    LONGLONG            qwIntraFrameJitterPeak;

    LONGLONG            qwMarker2;
    LONGLONG            qwReserved2;
} VPPSNOOP;

//---------------------------------------------------------------------------

typedef struct {
    DWORD               dwOpCount;
    HDRVEVENT           hLastStage;
    BOOL                doLateFlipSync;
    DWORD               dwIndex;
    DWORD               dwSrcOffset;
    DWORD               dwSrcPitch;
    DWORD               dwWidth;
    DWORD               dwHeight;
    DWORD               dwFourCC;
    DWORD               dwFlags;
    DWORD               dwWorkSurfaces;
    DWORD               vppExec;
    LONGLONG            qwTime1;
    LONGLONG            qwTime2;
    LONGLONG            qwDelta;
    LONGLONG            qwJitter;
} VPPSTATE;
#endif
#define NVPE_EVENT_MAX 3

//---------------------------------------------------------------------------
/// NVPESTATE

typedef struct {
    BOOL                bVPEInitialized;
    BOOL                bFsMirrorEnabled;
    DWORD               dwVPEState;
    HANDLE              hVPEKernel;
    HANDLE              hNVPSyncEvent[NVPE_EVENT_MAX];
    HANDLE              hNVPSyncEvent0[NVPE_EVENT_MAX];
} NVPESTATE;

//---------------------------------------------------------------------------
// blit data, including cached values and other control parameters

typedef struct {
    DWORD               dwStatusReferenceCount;
    DWORD               dwSystemSurfaceContextDMAIndex;
    DWORD               dwMTMFIndex;     // alternating odd or even, does not need to be initialized
    DWORD               dwLastRop;
    DWORD               dwLastColourKey;
    DWORD               dwLastColourFormat;
    DWORD               dwLastCombinedPitch;
    DWORD               dwLastSrcOffset;
    DWORD               dwLastDstOffset;
    DWORD               dwSystemBltFallback;
    DWORD               dwLetItThrash;
    DWORD               dwQueuedPrimaryBlitCount;
} BLTDATA;

//---------------------------------------------------------------------------
// blt work surface info

typedef struct {
    DWORD               dwLocale;
    DWORD               dwContextDma;
    DWORD               dwWSSize;
    FLATPTR             fpVidMem;
    DWORD               dwOffset;
    DWORD               dwReserved;
} BLTWSINFO;

//---------------------------------------------------------------------------
// sync record

typedef struct {
    DWORD               bSyncFlag;
    DWORD               dwReserved1;
} SYNCRECORD;

//---------------------------------------------------------------------------
// flip record
//---------------------------------------------------------------------------
typedef struct {
    DWORD               fpFlipFrom;
    DWORD               dwFlipDuration;
    DWORD               bFlipFlag;
    DWORD               bIsD3DSurface;
    DWORD               fpFlipTo;
    DWORD               fpPreviousFlipFrom;
    DWORD               dwReserved1;
    DWORD               dwReserved2;
    LONGLONG            llFlipTime;
    LONGLONG            llPreviousFlipTime;
    NV_CFGEX_CRTC_TIMING_PARAMS nvTimingInfo[2];
    LONGLONG            llFlipDurationNs[2];
} FLIPRECORD;

typedef DWORD  (FAR PASCAL *LPDDHALSURFCB_GETSURFACEINFO)(LPDDRAWI_DDRAWSURFACE_LCL);

#ifdef WINNT
typedef struct _PDEV PDEV;      // Defined in NT display driver's DRIVER.H file
#endif

//---------------------------------------------------------------------------

/*
 * nvdd32 driver context
 *
 * this is only used when nvdd32.dll source files include this file
 */
#if (NVARCH < 0x4)
#ifdef __cplusplus
class CDriverContext;
#define NVDD32_CONTEXT      DWORD  dwNVDD32Context;
#else
#define NVDD32_CONTEXT      DWORD  ContextSpaceFiller;
#endif
#endif
/*
 * forward definitions
 */
#ifdef __cplusplus
class CNvObject;
class CInterProcessHeap;
struct _NV_OBJECT_LIST;
#endif

#ifdef __cplusplus
class CReferenceCount;
#define REFCOUNT_t CReferenceCount
#else
struct CReferenceCount;
#define REFCOUNT_t struct CReferenceCount
#endif

// ---------------------------------------------------------
// GLOBALDATA struct
// ---------------------------------------------------------

#define FSDOSSTATUS_RECOVERYNEEDED_DDRAW  0x00000001
#define FSDOSSTATUS_RECOVERYNEEDED_D3D    0x00000002
#define FSDOSSTATUS_TWINVIEW_STATE_CHANGE 0x40000000
#define FSDOSSTATUS_COMPLETE              0x80000000

typedef struct _GLOBALDATA {
    // ---------------------------------------------------------
    // Stuff whose offset in this structure is critical.
    // ---------------------------------------------------------

#if (NVARCH >= 0x04)

    DWORD                       dwVersion;
    DWORD                       dwGlobalStructSize;          // current size of this structure
    DWORD                       DDrawVideoSurfaceCount;
    DWORD                       dwFullScreenDOSStatus;

#else  // NV3

    // on nv3, we still have dependencies with the display driver, so offsets are critical
#ifdef _WIN32
    DWORD                       lpPDevice;                   // *MUST* be first (DIBLINK.ASM) uses it.
#else
    DIBENGINE FAR *             lpPDevice;
#endif
    DWORD                       dwVersion;
    DWORD                       dwGlobalStructSize;          // current size of this structure
    DWORD                       DDrawVideoSurfaceCount;      // *MUST* start at 0x0C (cursor code and realizeObject checks it)
    WORD                        fDmaPusherActive;            // *MUST* start at 0x10 (both display driver and direct draw check it)
    BYTE                        fFullScreenDosOccurred;      // *MUST* start at 0x12 (both display driver and D3D check it)
    BYTE                        fDDrawFullScreenDosOccurred; // *MUST* start at 0x13 (both display driver and DDraw check it)
    DWORD                       dwModeSwitchCount;           // *MUST* start at 0x16 offset (display driver uses it)

#endif  // NV3

    // ---------------------------------------------------------
    // General Info -
    // stuff not specific to NV internals, D3D, DirectDraw,
    // or MultiMon alone. If specific, please file below.
    // (sooner or later, i _will_ organize the world.
    // ---------------------------------------------------------
#if (NVARCH >= 0x4)
    CPushBuffer                 nvPusher;
#endif
    DWORD                       dwDDDmaPusherChannelIndex;
    DWORD                       dwDDDmaPusherChannelMask;
    DWORD                       NvAGPDmaPusherBufferBase;
    DWORD                       dwSharedClipChangeCount;
    DWORD                       dwDDMostRecentClipChangeCount;
#ifdef  _WIN32
    volatile long               *NvDmaPusherBufferEnd;
#else   // _WIN32
#endif  // _WIN32
    long                        NvDmaPusherBufferSize;
    DWORD                       dwVidMemCtxDmaSize;
    #if (NVARCH < 0x4)
    DWORD                       dwDmaPusherCtxDmaSize;
    #endif
    DWORD                       dwMacrovisionNavigatorID;
    SYNCRECORD                  syncPioFifoRecord;
    SYNCRECORD                  syncDmaRecord;
    DWORD                       blitCalled;
    WORD                        fVidMemLocked;
    WORD                        fActiveExternalUser; // NVLIB, NVDDX, etc
    WORD                        fNvActiveFloatingContexts;
#ifdef WINNT
    // stored in pDXShare under win9x
    WORD                        primaryBufferIndex;
#else
    WORD                        unused_01;  // Keep everything dword aligned
#endif

    // info about the current mode
    DWORD                       TotalVRAM;          // total VRAM on card
#if (NVARCH < 0x4)
    DWORD                       ModeNumber;
    DWORD                       ScreenSel;          // selector to screen
    DWORD                       maxOffScreenY;      // maximum screen pitch width Y at current resolution
#endif
    DWORD                       BaseAddress;        // base addess of accessable video memory.
    long                        VideoHeapBase;      // start of video heap
    long                        VideoHeapEnd;       // end of video heap
    long                        VideoHeapTotal;     // total available heap
    long                        VideoHeapOverhead;  // GDI and RM overhead, not related to DX surfaces
    long                        VideoHeapFree;      // free heap
#ifdef WINNT
    DWORD_PTR                   GARTLinearBase;     // linear base address of AGP GART memory
    DWORD_PTR                   GARTPhysicalBase;   // physical base address of AGP GART memory
    DWORD_PTR                   GARTLinearHeapOffset; // WINNT - offset from GARTLinearBase to beginning of AGP heap
#else
    DWORD                       GARTLinearBase;     // linear base address of AGP GART memory
    DWORD                       GARTPhysicalBase;   // physical base address of AGP GART memory
    VOID                        *notUsedGARTLinearHeapOffset; // not used - placeholder for Win9x VxD builds
#endif // !WINNT

    DWORD                       regPersistAGPSize;  // # of bytes reserved for persistent AGP allocs
#if (NVARCH < 0x4)
    // bitmapinfo for current mode
    BITMAPINFOHEADER            bi;
    DWORD                       color_table[256];

    DWORD                       dwDXRuntimeVersion; // version of DirectX runtime installed (inferred from GUIDs)
    #endif
    DWORD                       dwDDGlobal;         // LPDDRAWI_DIRECTDRAW_GBL for this context
    DWORD                       __pad_xxx;

    DWORD                       dwGenInfoPad0;      // unused
    DWORD                       dwGenInfoPad1;      // unused

    // ---------------------------------------------------------
    // NV stuff
    // ---------------------------------------------------------
    DWORD                       lpNvDmaBuffer; // obsolete
    DWORD                       NvCommonDmaBufferFlat;
    DWORD                       NvScanlineBufferFlat;
#if (NVARCH < 0x04)
    DWORD                       NvDevFlatPio;
#endif
    DWORD                       NvDevVideoFlatPio;  // Video channel
    DWORD                       NvDevFlatVPE;       // Vpe channel
#if (NVARCH >= 0x04)
    NvNotification *            pDmaBufferNotifierFlat;
    NvNotification *            pDmaFlipPrimaryNotifierFlat;
    NvNotification *            pDmaDmaToMemNotifierFlat;
    NvNotification *            pDmaSyncNotifierFlat;
    NvNotification *            pGenericScaledImageNotifier;
    NvNotification *            pPioColourControlEventNotifierFlat;
    NvNotification *            pPioCombinerEventNotifierFlat;
#else
    DWORD                       NvPioFlipPrimaryNotifierFlat;
    DWORD                       NvPioSyncNotifierFlat;
    DWORD                       NvPioDmaFromMemNotifierFlat;
#endif
    DWORD                       NvPioDmaToMemNotifierFlat;
    DWORD                       NvPioBufferNotifierFlat;
    NvNotification *            pDmaPusherSyncNotifierFlat;
    NvNotification *            pPioFlipOverlayNotifierFlat;
    DWORD                       NvDmaFromMemBaseAddress;
    DWORD                       NvDmaFromMemSurfaceMem;
    DWORD                       NvFloating0UYVYSurfaceMem;                  // only used for WINNT now, delete when done
    DWORD                       NvFloating1UYVYSurfaceMem;                  // only used for WINNT now, delete when done
    DWORD                       NvFloating0YUYVSurfaceMem;                  // only used for WINNT now, delete when done
    DWORD                       NvFloating1YUYVSurfaceMem;                  // only used for WINNT now, delete when done
    DWORD                       NvFloatingMem2MemFormatMemoryHandle;        // only used for WINNT now, delete when done
    DWORD                       NvFloatingMem2MemFormatSurfaceAddress[4];   // only used for NV3 now, delete when done
    DWORD                       NvYUY2Surface0Mem;                          // only used for WINNT now, delete when done
    DWORD                       NvYUY2Surface1Mem;                          // only used for WINNT now, delete when done
    DWORD                       NvYUY2Surface2Mem;                          // only used for WINNT now, delete when done
    DWORD                       NvYUY2Surface3Mem;                          // only used for WINNT now, delete when done
    DWORD                       NvWinDmaPusherNotifierFlat;
    DWORD                       dwVideoPortsAvailable;
    DWORD                       NvFloatingTexture;
    DWORD                       CurrentVisibleSurfaceAddress;
    DWORD                       dDrawSpareSubchannelObject; // subchannel 7
    BOOL                        bDirtyRenderTarget;                         // nv4 only - RT/ZB has/have changed, need HW update
    DWORD                       NvBaseFlat;
    NvU32                       dwDesktopState;

    // --------------------------------------------------------
    // Overlay related stuff
    // --------------------------------------------------------
    Vpp_t                       vpp;

    NvU32                       regSuper7Compat;
#if (NVARCH < 0x4)
    DWORD                       dwOverlaySurfaces;
    DWORD                       dwOverlayOwner;
    DWORD                       dwOverlaySurfaceLCL[NV_MAX_OVERLAY_SURFACES];
    DWORD                       dwOverlaySrcWidth;
    DWORD                       dwOverlaySrcHeight;
    DWORD                       dwOverlayDstWidth;
    DWORD                       dwOverlayDstHeight;
    DWORD                       dwOverlayDeltaX;
    DWORD                       dwOverlayDeltaY;
    DWORD                       dwOverlayLastVisibleSurfaceLCL;
    DWORD                       dwOverlayLastVisibleFpVidMem;
    DWORD                       dwOverlayLastVisiblePitch;
    DWORD                       dwOverlayLastVisiblePixelFormat;
    DWORD                       dwOverlayFlipCount;
    WORD                        OverlaySrcX;        // overlay starting points
    WORD                        OverlaySrcY;
    WORD                        OverlayDstX;
    WORD                        OverlayDstY;
    WORD                        OverlayBufferIndex;
    WORD                        OverlayRelaxOwnerCheck;
    DWORD                       OverlaySrcOffset;
    DWORD                       OverlayFormat;      // current overlay format - used so VDD knows what's up!
    DWORD                       OverlaySrcPitch;    // current overlay pitch   - used so VDD knows what's up!
    DWORD                       OverlaySrcSize;
    DWORD                       OverlayColorKey;
    DDCOLORCONTROL              OverlayColourControl;
    DWORD                       OverlayCachedFlags;
    DWORD                       extra422OverlayOffset[NV_MAX_EXTRA_SURFACES];   // can be up to n "chunks" longer original surface
    DWORD                       extra422Pitch;
    DWORD                       extra422NumSurfaces;
    DWORD                       extra422Index;
    DWORD                       dwOverlayFSOffset[NV_MAX_EXTRA_FS_SURFACES];
    DWORD                       dwOverlayFSWidth;
    DWORD                       dwOverlayFSHeight;
    DWORD                       dwOverlayFSPitch;
    DWORD                       dwOverlayFSFormat;
    DWORD                       dwOverlayFSNumSurfaces;
    DWORD                       dwOverlayFSIndex;
    DWORD                       dwOverlayFSDeltaX;
    DWORD                       dwOverlayFSDeltaY;
    DWORD                       dwOverlayFSHead;
    DWORD                       dwOverlayFSOvlHead;
    DWORD                       dwOverlayFSSrcWidth;
    DWORD                       dwOverlayFSSrcHeight;
    DWORD                       dwOverlayFSSrcMinX;
    DWORD                       dwOverlayFSSrcMinY;
    NVTWINVIEWDATA              dwOverlayFSTwinData;
    DWORD                       dwOverlayFSOvlHeadSaved;
    DWORD                       dwOverlayFSOvlLost;
    DWORD                       fpOverlayShadow;
    DWORD                       dwOverlayByteAlignmentPad;
    DWORD                       dwOverlayMaxDownScale;
    DWORD                       dwOverlayMaxDownScaleX;
    DWORD                       dwOverlayMaxDownScaleY;
    DWORD                       regOverlayColourControlEnable;
    DWORD                       regRing0ColourCtlInterlockFlags;    // bit0 = 1 -> ring0 should run; bit2 = 1 ring0 in progress
    DWORD                       regOverlayMode;
    DWORD                       regOverlayMode2;
    DWORD                       regOverlayMode3;
    DWORD                       OverlayMode;            // used to tell if we're doing interleaved or what...
    DWORD                       regVPPInvMask;
    DWORD                       regVPPMaxSurfaces;
    VPPDISPATCH                 vppDispatch;
    VPPSNOOP                    vppSnoop;
    VPPSTATE                    vppState;
    DWORD                       dwVPPSubPictureOffset;
    DWORD                       dwVPPSubPicturePitch;
    DWORD                       dwVPPSubPictureFormat;
    DWORD                       dwVPPSubPictureX;
    DWORD                       dwVPPSubPictureY;
    DWORD                       dwVPPSubPictureWidth;
    DWORD                       dwVPPSubPictureHeight;
    #endif
    NvU32                       regPBLocale;
    LPPROCESSINFO               lpProcessInfoHead;
    PROCESSINFO                 procInfo;           // statically created for NT
    DWORD                       dwUseRefreshRate;
    WORD                        ddClipUpdate;
    WORD                        fDeviceHasDitheringControl;
    DWORD                       physicalColorMask;
    BYTE                        fDmaPushBufferHasBeenFlushed;
    BYTE                        fIsBltStyleFlip;
    WORD                        NvDeviceVersion;
    WORD                        bEnableVPE;     // used to allow registry disabling of VPE port
    WORD                        ddPad0point5;   // the other word which is unused
    DWORD                       CloseNvDevVideoFlatPio;
    DWORD                       CloseNvDevFlatVPE;
    DWORD                       northBridgeID;
    BLTDATA                     bltData;
    BLTWSINFO                   bltSysMemInfo;
    BLTWSINFO                   bltAGPMemInfo;
    BLTWSINFO                   bltVidMemInfo;
    DWORD                       dwTVTunerFlipCount;
    DWORD                       dwTVTunerTimer;
    NvU32                       regMaxAGPLimit;
    NvU32                       regTVTunerHack;         // unused under WINNT
    NvU32                       regEnumDDRefresh;

    // --------------------------------------------------------
    // VPE related stuff
    // --------------------------------------------------------

    NVPESTATE                   nvpeState;

    // --------------------------------------------------------
    // Motion Comp related stuff
    // --------------------------------------------------------
    DWORD                       dwMCTex0Control0;
    DWORD                       dwMCTex1Control0;
    BYTE                        bMCNewFrame;
    BYTE                        bMCPictureStructure;
    BYTE                        bMCPictureCodingType;
    BYTE                        bMCTopFieldFirst;
    BYTE                        bMCSecondField;
    BYTE                        bMCFrameType;
    BYTE                        bMCForwardPictureStructure;
    BYTE                        bMCBackwardPictureStructure;
    DWORD                       dwMCDestinationSurface;
    DWORD                       dwMCForwardSurface;
    DWORD                       dwMCBackwardSurface;
    DWORD                       dwMCDualPrimeSurface;
    DWORD                       dwMCIDCTSurface;
    DWORD                       dwMCMostRecentlyDecodedSurfaceBase;
    DWORD                       dwMCNVDSSurfaceBase;
    DWORD                       dwMCFirstAlignedIDCTIndex;
    DWORD                       dwMCIDCTAGPSurfaceBase[4];
    DWORD                       dwMCIDCTAGPCtxDmaSize[4];
    DWORD                       dwMCCurrentCelsiusIDCTCtxDma;
    DWORD                       dwMCDestinationSurfaceOffset;
    DWORD                       dwMCForwardSurfaceOffset;
    DWORD                       dwMCBackwardSurfaceOffset;
    DWORD                       dwMCNVMCSurface;
    DWORD                       dwMCNV12Surface[8];
    DWORD                       dwMCNVIDSurface[4];
    DWORD                       dwMCNVSPSurface;
    NVMCSURFACEFLAGS            nvMCSurfaceFlags[9];
    BYTE                        bMCTemporalFilterDisabled;
    BYTE                        bMCPercentCurrentField;
    BYTE                        bMCMostRecentlyDecodedPictureStructure;
    BYTE                        bMCOverlaySrcIsSingleField1080i;
    BYTE                        bMCCurrentExecuteFunction;
    BYTE                        bMCHorizontallyDownscale1080i;
    BYTE                        bMCPad0; // unused
    BYTE                        bMCPad1; // unused
    DWORD                       dwMCNVIDSurfaceReferenceCount[4];
    // --------------------------------------------------------
    DWORD                       dwMostRecentHWUser;   // most recent user of shared HW objects (celsius, kelvin, etc.)
    DWORD                       NvVideoNotifierBuffer;
    DWORD                       lockCount;
    DWORD                       thisDeviceID;
    DWORD                       dwRingZeroMutex; // when true prevents ring 0 from stepping on ring3 spare channel state
    WORD                        flatSelector;
    WORD                        NvFreeCount;
    DWORD                       dwBusType;
    DWORD                       pCurrentChannelID;
    DWORD                       NvVideoFreeCount;
    #if (NVARCH < 0x4)
    BYTE                        NvFirstCall;
    BYTE                        NvPrevDepth;
    #else
    WORD                        wNv4Pad;
    #endif
    BYTE                        TwoDRenderingOccurred;
    BYTE                        ThreeDRenderingOccurred;
    DWORD                       fourCC[32];

    // Used by VPP, MoComp & D3D
    FLIPRECORD               flipPrimaryRecord;
    FLIPRECORD               flipOverlayRecord;
    REFCOUNT_t              *pRefCount;

    // ---------------------------------------------------------
    // D3D stuff
    // ---------------------------------------------------------

    // for nv4 and beyond, most stuff has been moved into CDriverContext
#if (NVARCH >= 0x04)
    NV_SystemInfo_t             nvD3DPerfData;
    DWORD                       dwCurrentContextHandle;     // probably don't need
#else  // NV3
    DWORD                       dwCurrentContextHandle;
    DWORD                       dwD3DContextCount;
    DWORD                       dwZBufferContextAddr;
    DWORD                       dwTextureContextValid;
    DWORD                       dwD3DTextureCount;
    DWORD                       dwDMAPushContext;
    DWORD                       dwDmaPushBufferSize;
    DWORD                       hContexts_gbl;
    DWORD                       hTextures_gbl;
    DWORD                       hAGPList_gbl;
    DWORD                       hTextureHeap;
    DWORD                       dwContextListHead;
    DWORD                       dwDmaMaxPushCount;
    DWORD                       dwDmaFifoAddr;
    DWORD                       dwDmaFifoOffset;
    DWORD                       dwDmaFifoCurrentBuffer;
    DWORD                       dwDmaCount;
    DWORD                       dwDmaFifoPhysOffset;
    DWORD                       dwDmaFifoCurrent;
    DWORD                       dwDmaFifoOffsetCurrent;
    DWORD                       dwTextureListHead;
#ifdef __cplusplus
    CNvObject                  *pNvObjectHead;
#else
    void                       *dwD3DPad3;
#endif
    DWORD                       dwRMChID;
    DWORD                       dwRMPtBase;
    DWORD                       dwRMAddressSpace;
    DWORD                       dwProcessorSpeed;
    DWORD                       dwProcessorFlags;
    // Texture heap data.
    DWORD                       pRealHeapBase;
    DWORD                       pTextureHeapBase;
    DWORD                       pTextureHeapAllocHead;
    DWORD                       pTextureHeapAllocTail;
    DWORD                       pTextureHeapFreeHead;
    DWORD                       pTextureHeapFreeTail;
    DWORD                       dwTextureHeapSize;
    DWORD                       dwTextureHeapSizeMax;
    DWORD                       dwTextureHeapLimit;
    DWORD                       dwTextureHeapLimitMax;
    DWORD                       dwAllocSize;
    DWORD                       dwFreeSize;
    // D3D registry settings.
    DWORD                       regD3DEnableBits1;
    DWORD                       regDmaPushBufferSizeMax;
    DWORD                       regMipMapLevels;
    DWORD                       regZOHMode;
    DWORD                       regTexHeap;
    DWORD                       regMinVideoTextureSize;
    DWORD                       regFilterControl;
    DWORD                       regD3DContextMax;
    DWORD                       regD3DTextureMax;
    DWORD                       regNonSquareExpand;
    DWORD                       regFOHMode;
    DWORD                       regLODMethod;
    DWORD                       regAAReadCompatibilityFile;
    DWORD                       regAAMethod;
    DWORD                       regTurboMode;
    DWORD                       regD3DAlphaBlendMethod;
    DWORD                       regDmaMinPushCount;
    DWORD                       regNVPEventBits;   // locutus enable bits
    DWORD                       regNVPTimeBits;    // locutus enable bits
    DWORD                       regNVPCountBits;   // locutus enable bits
    DWORD                       regGapFill;
    DWORD                       regTexAddrAdjust;
    // Extra D3D data space
    DWORD                       dwD3DPad0;         // unused
    DWORD                       dwD3DPad1;         // unused
    DWORD                       fReset;
    DWORD                       fVidMemVarsReset;
#endif  // NV3

    // ---------------------------------------------------------
    // DirectDraw stuff
    // ---------------------------------------------------------

    DWORD                       dwFourCCSelector;
    DWORD                       dwPvmListSelector;
    DWORD                       dwDDCallbacksSelector;
    DWORD                       dwDDSurfaceCallbacksSelector;
    DWORD                       dwDDPaletteCallbacksSelector;
    DWORD                       dwDDExecuteBufferCallbackSelector;
    DWORD                       dwModeListSelector;
#ifndef WINNT
#define                         MAX_MODES   MAX_MODEENTRYS
#ifdef  _WIN32  // This is because we use this file with both 16 and 32 bit compilers
    DDHALMODEINFO NEAR          *ModeList;
#else
    DWORD                       ModeList;
#endif  // _WIN32
    DDHALDDRAWFNS               HALCallbacks;
#endif // !WINNT

#ifdef WINNT
    DDHALINFO                   HALInfo;                // allocated dynamically under Win9x
#endif // WINNT
    DDHAL_DDCALLBACKS           DDCallbacks;
    DDHAL_DDSURFACECALLBACKS    DDSurfaceCallbacks;
    DDHAL_DDPALETTECALLBACKS    DDPaletteCallbacks;

#ifndef WINNT   // WINNT BUG we will need to remove when DX7 port is complete
    DDHAL_DDEXEBUFCALLBACKS     DDExecuteBufferCallbacks;
#else
    DDHAL_D3DBUFCALLBACKS       DDExecuteBufferCallbacks;
#endif // WINNT0

#ifndef WINNT
    VIDMEM                      vidMem[2];
#endif // #ifdef WINNT

    DWORD                       dwMySurfacesPtr;        // unused
    DWORD                       bRing0FlippingFlag;        // TRUE when KMVT based overlay flipping is happening
    DWORD                       dwDDrawPad3;        // unused

    DWORD                       dwSurfaceAlignPad;
    DWORD                       dwSurfaceAlign;     // hw required surface alignment
    DWORD                       dwSurfaceBitAlign; // hw required surface alignment in term of bits

    // ---------------------------------------------------------
    // Multi-Monitor stuff - only use this data in a MM driver
    // ---------------------------------------------------------
    DWORD                       dwDrvRefCount;      // driver reference count, can get greater than 1 in multihead
    DWORD                       dwRootHandle;       // NV connection Root Handle
    DWORD                       dwDeviceHandle;     // ID associated with a particular driver for a display by VDD
    DWORD                       dwDeviceIDNum;      // ID for a display assigned by the RM
    DWORD                       dwDeviceClass;
    DWORD                       dwHeads;            // number of heads (DACs) on this device
#ifdef WINNT
    //*****************************************************************************
    // Semaphore related fields.
    //*****************************************************************************
    BOOL    NvSemRefCnt;        // Does this GLOBALDATA own the semaphore?
#else
    DWORD   NvSemPlaceHolder;   // Dummy placeholder for Win9x
#endif // !WINNT

    DWORD                       pStereoData;        // pointer to a structure containing stereo stuff.

    // ---------------------------------------------------------
    // DirectX-VA Stuff
    // ---------------------------------------------------------
    BYTE                        bDXVAIndexToNVIndex[8];
    DWORD                       dwDXVAConvertBuffer;
    WORD                        wDXVAReserved1;
    WORD                        wDXVAReserved2;
    DWORD                       dwDXVAFlags;
    DWORD                       dwDXVAIndexToVidMem[8];
    DWORD                       dwDXVAReserved3;

    DWORD                       dwNVOverlayFlags;

} GLOBALDATA;

/***************************************************************************
 ***************************************************************************/

#ifdef _WIN32

#ifdef __cplusplus
    class CDriverContext;
    extern CDriverContext *pDriverData;   // in ddDrv.cpp
#else
    extern GLOBALDATA     *pDriverData;   // in ddDrv.cpp
#endif

/* Ternary raster operation indexes */

#define BLACKNESSINDEX      0x00000000L
#define NOTSRCERASEINDEX    0x00000011L
#define NOTSRCCOPYINDEX     0x00000033L
#define SRCERASEINDEX       0x00000044L
#define PATINVERTINDEX      0x0000005AL
#define SRCINVERTINDEX      0x00000066L
#define SRCANDINDEX         0x00000088L
#define MERGEPAINTINDEX     0x000000BBL
#define SRCCOPYINDEX        0x000000CCL
#define SRCPAINTINDEX       0x000000EEL
#define PATCOPYINDEX        0x000000F0L
#define WHITENESSINDEX      0x000000FFL

/* Supported ROP table bits */

/* 0x00 */
#define BLACKNESSBIT        0x00000001L
/* 0x10 */
#define NOTSRCERASEBIT      0x00020000L

/* 0x20 */
/* 0x30 */
#define NOTSRCCOPYBIT       0x00080000L

/* 0x40 */
#define SRCERASEBIT         0x00000010L
/* 0x50 */
#define PATINVERTBIT        0x04000000L

/* 0x60 */
#define SRCINVERTBIT        0x00000040L
/* 0x70 */

/* 0x80 */
#define SRCANDBIT           0x00000100L
/* 0x90 */

/* 0xA0 */
/* 0xB0 */
#define MERGEPAINTBIT       0x08000000L

/* 0xC0 */
#define MERGECOPYBIT        0x00000001L
#define SRCCOPYBIT          0x00001000L
/* 0xD0 */

/* 0xE0 */
#define SRCPAINTBIT         0x00004000L
/* 0xF0 */
#define PATCOPYBIT          0x00010000L
#define WHITENESSBIT        0x80000000L


#else  // _WIN32

/* Ternary raster operations */
#define BLACKNESS           0x00000042L
#define SRCCOPY             0x00CC0020L
#define PATCOPY             0x00F00021L
#define WHITENESS           0x00FF0062L

/***************************************************************************
 *
 * global driver data
 *
 ***************************************************************************/

#if !defined(_WIN32) && !defined(WINNT)
extern GLOBALDATA           DriverData;     // in ddmini16.c
#endif /* !defined(_WIN32) && !defined(WINNT) */

/***************************************************************************
 *
 * functions in ddmini.c
 *
 ***************************************************************************/

void FAR PASCAL _loadds BeginAccess(DIBENGINE FAR *pde, int left, int top, int right, int bottom, UINT flags);
void FAR PASCAL _loadds EndAccess(DIBENGINE FAR *pde, UINT flags);

/***************************************************************************
 *
 * functions in setmode.c
 *
 ***************************************************************************/

BOOL HWSetMode();
BOOL HWTestMode(int ModeNumber);
void HWEndAccess(void);
BOOL HWSetPalette(int start, int count, DWORD FAR *colors);


/***************************************************************************
 *
 * functions in DIBENG
 *
 ***************************************************************************/

extern DWORD FAR PASCAL CreateDIBPDevice    (LPBITMAPINFOHEADER lpbi, DIBENGINE FAR *lpDevice, LPVOID lpBits, WORD dwFlags);
extern UINT  FAR PASCAL DIB_Enable          (LPVOID, UINT, LPSTR, LPSTR, LPVOID);
extern UINT  FAR PASCAL DIB_Disable         (DIBENGINE FAR *);
extern LONG  FAR PASCAL DIB_Control         (DIBENGINE FAR *, UINT, LPVOID, LPVOID);
extern UINT  FAR PASCAL DIB_BeginAccess     (DIBENGINE FAR *, int left, int top, int right, int bottom, WORD flags);
extern UINT  FAR PASCAL DIB_EndAccess       (DIBENGINE FAR *, WORD flags);
extern UINT  FAR PASCAL DIB_SetPaletteExt   (UINT start, UINT count, DWORD FAR *lpPalette, DIBENGINE FAR * pde);
extern UINT  FAR PASCAL DIB_BitBlt          (DIBENGINE FAR *, int, int, DIBENGINE FAR *, int, int, int, int, DWORD, DIB_Brush8 FAR *, DRAWMODE FAR *);
extern UINT  FAR PASCAL DIB_StretchBlt      (DIBENGINE FAR *, int, int, int, int, DIBENGINE FAR *, int, int, int, int, DWORD, DIB_Brush8 FAR *, DRAWMODE FAR *,RECT FAR *);
extern UINT  FAR PASCAL DIB_StretchDIBits   (DIBENGINE FAR *, int, int, int, int, int, int, int, int, LPVOID, BITMAPINFO FAR *, LPVOID, DWORD, DIB_Brush8 FAR *, DRAWMODE FAR *,RECT FAR *);
extern UINT  FAR PASCAL DIB_DibToDevice     (DIBENGINE FAR *, int, int, int, int, RECT FAR *, DRAWMODE FAR *, LPVOID, BITMAPINFO FAR *, LPVOID);
extern UINT  FAR PASCAL DIB_Output          (DIBENGINE FAR *, int, int, POINT FAR *, DIB_Pen FAR *, DIB_Brush8 FAR *, DRAWMODE FAR *, RECT FAR *);
extern UINT  FAR PASCAL DIB_ExtTextOutExt   (DIBENGINE FAR *pde, int x, int y, RECT FAR *Clip, LPSTR sz, UINT cb, LPVOID lpFont, DRAWMODE FAR *pdm, LPVOID xform, int FAR *pdx, RECT FAR *lpORect, UINT f, LPVOID DrawBitmap, LPVOID DrawRect);
// extern UINT  FAR PASCAL DIB_SetCursorExt    (CURSORSHAPE FAR *lpCursor, DIBENGINE FAR *pde);
extern UINT  FAR PASCAL DIB_MoveCursorExt   (int x, int y, DIBENGINE FAR * pde);

#endif // _WIN32

/* NV Device Version Numbers */
#define NV_DEVICE_VERSION_4     0x0004
#define NV_DEVICE_VERSION_5     0x0005
#define NV_DEVICE_VERSION_10    0x0010
#define NV_DEVICE_VERSION_20    0x0020

/* AGP stuff */
#define NV_MAX_AGP_MEMORY_LIMIT 0x1FFFFFF

/* Dma flags */
#define NV_WIN_DMA_PUSHER_IS_ACTIVE  1
#define NV_DD_DMA_PUSHER_IS_ACTIVE   2

/* Floating system memory context in use flags */
#define NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY        1
#define NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY        2
#define NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY        4
#define NV_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY        8
#define NV_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY       16

/* Other NV specific defines */
#define NV_OVERLAY_BYTE_ALIGNMENT_PAD   63L
#define NV_OVERLAY_BYTE_ALIGNMENT       64L

/* Surface Alignment */
#if (NVARCH >= 0x010)
#define NV_BYTE_ALIGNMENT_PAD           63L
#define NV_BYTE_ALIGNMENT               64L
#define NV_SCAN_OUT_BYTE_ALIGNMENT      256
#define NV_BIT_ALIGNMENT                256
#elif (NVARCH >= 0x04)
#define NV_BYTE_ALIGNMENT_PAD           31L
#define NV_BYTE_ALIGNMENT               32L
#define NV_SCAN_OUT_BYTE_ALIGNMENT      256
#define NV_BIT_ALIGNMENT                256
#else /* NVARCH < 0x04 */
#define NV_BYTE_ALIGNMENT_PAD           15L
#define NV_BYTE_ALIGNMENT               16L
#define NV_SCAN_OUT_BYTE_ALIGNMENT      128
#define NV_BIT_ALIGNMENT                128
#endif  /* !(NV10 || NV4) */

/* Define driver defined surface bits for the driver reserved field in the local surface structure */
#define NV_D3D_HAS_FLIPPED              0x00000001

/* Detect the Via VP3 and MVP3 Northbridge IDs for workarounds */
#define VIA_VP3_NB_ID   0x5971106
#define VIA_MVP3_NB_ID  0x5981106
#define IS_VIA_MVP3_NB_ID(nb)   (((nb) == VIA_VP3_NB_ID) || ((nb) == VIA_MVP3_NB_ID))

// dwDXVAFlags in pDriverData
#define DXVA_MO_COMP_IN_PROGRESS     0x00000001
#define DXVA_PICTURE_IN_PROGRESS     0x00000002
#define DXVA_ENABLED_IN_REGISTRY     0x00000004
#define DXVA_INTRA_IDCT_UNSIGNED     0x00000008
#define DXVA_OVERLAY_WAS_BOBBED      0x00000010
#define DXVA_BACK_END_ALPHA_SELECTED 0x00000020
#define DXVA_BACK_END_ALPHA_ENABLED  0x00000040

// dwNVOverlayFlags in pDriverData
#define OVERLAY_FLAG_ON_PANEL        0x00000001


/***************************************************************************
 *
 * DEBUG stuff
 *
 ***************************************************************************/
#ifdef IS_32
    extern void __cdecl PF (LPSTR szFormat, ...);
    #ifdef DEBUG
        extern void __cdecl DPF(LPSTR szFormat, ...);
        #define BREAK()     DebugBreak();
    #else
        #define DPF         1 ? (void)0 : (void)
        #define BREAK()
    #endif
#else  // !IS_32
    #define PF          1 ? (void)0 : (void)
    #define DPF         1 ? (void)0 : (void)
    #define BREAK()
#endif  // !IS_32

#ifdef WINNT
//
// WINNT Only:
// Need to declare these with "C" linkage so we can call them from the display
// driver.
//
// Function prototypes. One time DX6 Init/Destroy code. This is the 32 bit ddraw init code.
// Found in $(BUILDDIR)\main\drivers\ddraw\common\src\ddDrv.cpp
//
BOOL  __stdcall buildDDHALInfo32(GLOBALDATA *pDriverData);
DWORD __stdcall DriverInit(DWORD dwDriverData);
HRESULT __stdcall DestroyDriver32(GLOBALDATA *pGlobalData);
void  __stdcall init_globals(void);
void  __stdcall NvWin2KInitIPM();
void  __stdcall NvWin2KInitDXGlobals();
void  __stdcall NvWin2KInitDXState(PDEV *ppdev);
BOOL  __stdcall NvWin2KD3DHALCreateDriver(DD_HALINFO *pHalInfo, PDEV *ppdev);
BOOL  __stdcall NvWin2KAllocDriverData(PDEV* ppdev);
void  __stdcall NvWin2KpvmConfig(PDEV *ppdev, VIDEOMEMORY *pvmList);
DWORD __stdcall NvWin2KMapAgpHeap(PDEV *ppdev);
DWORD __stdcall NvWin2KGetAgpLimit(PDEV *ppdev, DWORD dwRootHandle, DWORD dwDevID);
DWORD __stdcall NvWin2KUnmapAgpHeap(PDEV *ppdev);

extern GLOBALDATA  DefaultGlobalData;
extern ENGSAFESEMAPHORE NvSem;
#define NvAcquireSemaphore(pGblData)                \
{                                                   \
    if ((pGblData) && !((pGblData)->NvSemRefCnt))   \
    {                                               \
        EngAcquireSemaphore(NvSem.hsem);            \
    }                                               \
    (pGblData)->NvSemRefCnt++;                      \
}
#define NvReleaseSemaphore(pGblData)            \
{                                               \
    if ((pGblData) && (pGblData)->NvSemRefCnt)  \
    {                                           \
        (pGblData)->NvSemRefCnt--;              \
        if (!((pGblData)->NvSemRefCnt))         \
        {                                       \
            EngReleaseSemaphore(NvSem.hsem);    \
        }                                       \
    }                                           \
}
#else   // Win9x
#define NvAcquireSemaphore(pGblData)
#define NvReleaseSemaphore(pGblData)
#endif

/*
 * This is the error return value from the DX heap manager allocation functions.
 * It is defined differently under Win2K since the DX heap manager returns an
 * offset rather than an address, which makes 0 a valid offset rather than an
 * error code.
 */
#ifndef WINNT
#define ERR_DXALLOC_FAILED 0
#else
#define ERR_DXALLOC_FAILED 0xffffffff
#endif

#ifdef __cplusplus
}
#endif // __cplusplus


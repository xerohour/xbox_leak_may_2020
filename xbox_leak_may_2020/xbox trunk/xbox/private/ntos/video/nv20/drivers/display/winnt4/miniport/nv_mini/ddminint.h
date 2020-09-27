/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1995,1997 NVidia Corporation. All Rights Reserved.
 *
 *  File:       ddmini.h
 *  Content:    header for Windows95 Direct Draw driver
 *
 *  Files:
 *           ddmini16.c
 *           ddddrv16.c
 *           ddmode.c
 *
 *           dddrv32.c
 *           blt832.c
 *           blt1632.c
 *           blt3232.c
 *
 ***************************************************************************/
#ifndef _DDMININT_H_
#define _DDMININT_H_

// Turn this off to disable the use of the VPP code for the winnt4
// display driver. @mjl@
#define MJL_USE_VPP


// Note:

// From NT4 DDK (Graphics System Overview)
//    The display driver is in the same privileged kernel-mode address space as the 
//    rest of the Windows NT Executive. Faults in the display driver will cause GDI 
//    and the rest of the system to fault.

#include "nvPusher.h"
#include "nvVPP.h"
#include "nvUtil.h"
#include "CompileControl.h"

#ifndef NVD3D
#include "nvFourCC.h"
#undef NV_MAX_FOURCC
#define NV_MAX_FOURCC 7
#endif

#define NV_MAX_EXTRA_FS_SURFACES 4
#define NV_MAX_EXTRA_SURFACES    6

//  Remove double-defintion warning
#ifdef  DIRECTDRAW_DLLNAME
#undef  DIRECTDRAW_DLLNAME
#endif
#define DIRECTDRAW_DLLNAME "NV3DD32.DLL"

#ifndef _WIN32
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
#include <ddrawi.h>
#endif

#define FAST register

//  Remove double-definition warning
#ifdef  asmMergeCoords
#undef  asmMergeCoords
#endif
#define asmMergeCoords(LOWWORD, HIWORD) \
        (long)(((long)HIWORD << 16) | (unsigned short)LOWWORD)

// Probably should move these up to a more general level -@mjl@

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


//************************************************************************************
// Entry/exit macros used by the ddraw driver to acquire the rendering semaphore from the
// This notifies the OGL ICD that we need to touch the fifo
//************************************************************************************
#define START_DMA_PUSH_SYNC()           \
{ \
    ppdev->pfnAcquireOglMutex(ppdev); \
    ppdev->NVFreeCount = 0; \
    releaseOpenglSemaphore = TRUE; \
}

#define ENTER_DMA_PUSH_SYNC()           \
    if (OglIsEnabled(ppdev))       \
        {                               \
        START_DMA_PUSH_SYNC(); \
        }

#define END_DMA_PUSH_SYNC() \
    if (releaseOpenglSemaphore == TRUE) \
        {                               \
        ppdev->pfnReleaseOglMutex(ppdev); \
        }

#define EXIT_DMA_PUSH_SYNC(exitcode)    \
{ \
    END_DMA_PUSH_SYNC();            \
    return(exitcode); \
}


/***************************************************************************
 *
 * DriverData
 *
 * this structure contains all the globals of the driver, it is shared
 * between the 16 and 32bit side of the driver.
 *
 ***************************************************************************/

//  Remove double-definition warning
#ifdef  DDMINI_VERSION
#undef  DDMINI_VERSION
#endif
#define DDMINI_VERSION  0x0300



#ifndef NVD3D
typedef struct {
    short   x;
    short   y;
} NVXY;

typedef struct {
    short   y;
    short   x;
} NVYX;

typedef struct {
    unsigned short w;
    unsigned short h;
} NVWH;

typedef struct {
    unsigned short h;
    unsigned short w;
} NVHW;

typedef union {
    long xy;
    long yx;
    unsigned long wh;
    unsigned long hw;
    NVXY xy16;
    NVYX yx16;
    NVWH wh16;
    NVHW hw16;
} NVCOORD;

// Not sure this is the best approach to accessing this object methods. -@mjl@
#define BETA4_SET_BETA_FACTOR_OFFSET                            NV072_SET_BETA_FACTOR
#define SURFACES_2D_PITCH_OFFSET                                NV042_SET_PITCH
#define BLIT_POINT_IN_OFFSET                                    NV05F_CONTROL_POINT_IN
#define MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET                      NV039_OFFSET_IN
#define SET_ROP_OFFSET                                          NV043_SET_ROP5
#define SET_TRANSCOLOR_OFFSET                                   NV057_SET_COLOR
#define SURFACES_2D_SET_COLOR_FORMAT_OFFSET                     NV042_SET_COLOR_FORMAT
#define SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET                   NV077_SET_CONTEXT_DMA_IMAGE
#define SCALED_IMAGE_CONTEXT_SURFACE_OFFSET                     NV077_SET_CONTEXT_SURFACE
#define SCALED_IMAGE_SET_COLOR_FORMAT                           NV077_SET_COLOR_FORMAT
#define SCALED_IMAGE_CLIPPOINT_OFFSET                           NV077_CLIP_POINT
#define SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET                       NV077_IMAGE_IN_SIZE
#define SCALED_IMAGE_NOTIFY_OFFSET                              NV077_NOTIFY
#define IMAGE_BLACK_RECTANGLE_POINT_OFFSET                      NV019_SET_POINT
#define RECT_AND_TEXT_COLOR1A_OFFSET                            NV04A_COLOR1_A

//---------------------------------------------------------------------------
// blit data, including cached values and other control parameters
// 
// Used to allow sharing of several objects (ROP, 2D SURFACE, ...?)
//  by multiple users within the same context (may not need in NT4) -@mjl@

typedef struct BltData_s {
    NvU32               dwStatusReferenceCount;
    NvU32               dwSystemSurfaceContextDMAIndex;
    NvU32               dwMTMFIndex;     // alternating odd or even, does not need to be initialized
    NvU32               dwLastRop;
    NvU32               dwLastColourKey;
    NvU32               dwLastColourFormat;
    NvU32               dwLastCombinedPitch;
    NvU32               dwLastSrcOffset;
    NvU32               dwLastDstOffset;
    NvU32               dwSystemBltFallback;
    NvU32               dwLetItThrash;
    NvU32               dwQueuedPrimaryBlitCount;
} BltData_t;

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
} FLIPRECORD;


// I think these are mocomp... why are they used in VPP? -@mjl@
#define NVS2VF_SP_ODD           0x00000004
#define NVS2VF_SP_EVEN          0x00000008
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


typedef struct _PDEV PDEV;      // Handy forward declaration

typedef struct _GLOBALDATA
{
    // lpPDevice *must* be first (DIBLINK.ASM) uses it.
#ifdef _WIN32
    DWORD                       lpPDevice;
#else
    DIBENGINE FAR *             lpPDevice;
#endif


    DWORD                       dwVersion;
    DWORD                       fIsDisplay;

    // flags
    DWORD                       DDrawVideoSurfaceCount; // *MUST* start at 0xC offset from beginning of struct (cursor code and realizeObject checks it)
    WORD                        fDmaPusherActive;   // *MUST* start at 0x10 offset from beginning of struct (both display driver and direct draw check it)
    WORD                        fFullScreenDosOccurred; // *MUST* start at 0x12 offset from beginning of struct (both display driver and direct draw check it)
    WORD                        fVidMemLocked;
    WORD                        fActiveExternalUser; // NVLIB, NVDDX, etc
    WORD                        fNvActiveFloatingContexts;
    WORD                        OverlayBufferIndex;

    // info about the current mode
    DWORD                       ModeNumber;
    DWORD                       TotalVRAM;          // total VRAM on card
    DWORD                       ScreenSel;          // selector to screen
    DWORD                       maxOffScreenY;      // maximum screen pitch width Y at current resolution
    DWORD                       BaseAddress;        // base addess of accessable video memory.(set to zero on NT)
    long                        VideoHeapBase;      // start of video heap
    long                        VideoHeapEnd;       // end of video heap
    DWORD                       GARTLinearBase;     // linear base address of AGP GART memory
    DWORD                       GARTPhysicalBase;   // physical base address of AGP GART memory
    WORD                        OverlaySrcX;        // overlay source starting point
    WORD                        OverlaySrcY;

    // bitmapinfo for current mode
    BITMAPINFOHEADER            bi;
    DWORD                       color_table[256];

    PDEV *                      ppdev;  // A pointer back to the physical device
    // NV stuff
//    DWORD                       lpNvDev;
//    DWORD                       lpNvDmaBuffer;
//    DWORD                       NvDevFlat;
    DWORD                       NvDevFlatVPE; // Vpe channel (used in NV3 -- maybe)
//    DWORD                       NvDmaBufferFlat;
//    DWORD                       NvDummyNotifierFlat;
//    DWORD                       NvFlipPrimaryNotifierFlat;
//    DWORD                       NvFlipOverlayNotifierFlat;
//    DWORD                       NvFlipVportNotifierFlat;
//    DWORD                       NvDmaToMemNotifierFlat;
//    DWORD                       NvDmaFromMemNotifierFlat;
//    DWORD                       NvDmaFromMemBaseAddress;
//    DWORD                       NvDmaFromMemSurfaceMem;
//    DWORD                       NvPusherSyncNotifierFlat;
//    DWORD                       NvScanlineBufferFlat;
//    DWORD                       NvFloating0UYVYSurfaceMem;
//    DWORD                       NvFloating1UYVYSurfaceMem;
//    DWORD                       NvFloating0YUYVSurfaceMem;
//    DWORD                       NvFloating1YUYVSurfaceMem;
//    DWORD                       NvFloatingMem2MemFormatBaseAddress;
//    DWORD                       NvYUY2Surface0Mem;
//    DWORD                       NvYUY2Surface1Mem;
//    DWORD                       NvFloatingMem2MemFormatMemoryHandle;
//    DWORD                       NvWinDmaPusherNotifierFlat;
//    DWORD                       dwVideoPortsAvailable;
//    DWORD                       NvFloatingTexture;
//    DWORD                       CurrentVisibleSurfaceAddress;
//    DWORD                       lpLast3DSurfaceRendered;
    NvU32                       NvBaseFlat;                        // Base Address of NV IoSpace
//    DWORD                       dwOverlaySurfaces;
//    DWORD                       dwOverlayOwner;

//    DWORD                       dwOverlaySrcHeight;
//    DWORD                       dwOverlayDstHeight;
//    DWORD                       dwModeIndex;
//    DWORD                       dwUseRefreshRate;
//    DWORD                       NvFloatingMem2MemFormatNextAddress;
//    DWORD                       NvYUY2Surface2Mem;
    DWORD                       physicalColorMask; // (accessed in NV3 -- but used?)
    WORD                        flatSelector; // NV3 only -- used?
    WORD                        NvFreeCount;
    BYTE                        NvFirstCall;
    BYTE                        NvPrevDepth;
    BYTE                        ThreeDRenderingOccurred;// NV3 only -- used?
    BYTE                        unused_01;

//    DWORD                       fourCC[16];    // leave some empty locations so we can add more without rereleasing 16 bit portion of driver

    //*********************************************************************************
    // Stuff added for compatibility with NT5/Win9x Build
    //*********************************************************************************
    // TBD: make sure ALL of these are initialized properly -@mjl@
    LPPROCESSINFO               lpProcessInfoHead;
    PROCESSINFO                 procInfo;           // statically created for WinNT4/Win2k
    NvU16                       NvDeviceVersion;
    NvU8                        TwoDRenderingOccurred;       // Used trivially by VPP, but not otherwise
    NvU8                        unused_02;
    NvU32                       dwRootHandle;       
    NvU32                       dwDeviceHandle;
    NvU32                       dwSurfaceAlignPad;
    NvU32                       dwMostRecent3dUser;
    NvU32                      *pCurrentChannelID;
    NvU32                       regPBLocale;
    NvU32                       blitCalled;

    NvU32                       dDrawSpareSubchannelObject;  // This is referenced DIRECTLY by the VPP code.  Fix @mjl@
    NvU32                       ddClipUpdate;                // Used trivially by VPP, but not otherwise
    NvU32                       dwDXVAFlags;                 // Used trivially by VPP, but not otherwise
    NvU32                       dwSharedClipChangeCount;     // TBD: vpp makes global ref to these! -@mjl@
    NvU32                       dwDDMostRecentClipChangeCount;//Ditto

    CPushBuffer                 nvPusher;   // Used exclusively by the ported VPP code

    NV_SystemInfo_t             sysInfo;
    BltData_t                   bltData;
    FLIPRECORD                  flipPrimaryRecord;
    FLIPRECORD                  flipOverlayRecord;

    NvU32                       dwTVTunerFlipCount;
    NvU32                       dwOverlayFlipCount;
    NvU32                       dwHeads;            // number of heads (DACs) on this device
    NvU32                       dwDesktopState;
    NvV32                      *pCommonNotifierBuffer;
    NvNotification             *pPioFlipOverlayNotifierFlat; // To be changed! @mjl@

    //*********************************************************************************
    // Video & Overlay Stuff
    //*********************************************************************************
    Vpp_t                       vpp;

#if 0
    // D3D stuff
    DWORD                       dwCurrentContextHandle;
    DWORD                       dwCurrentContextPtr;
    DWORD                       dwCurrentTextureHandle;
    DWORD                       dwCurrentTexturePtr;
    DWORD                       dwD3DContextCount;
    DWORD                       dwZBufferContextAddr;
    DWORD                       dwTriangleContextHandle;
    DWORD                       dwTriangleContextAddr;
    DWORD                       dwD3DTextureCount;
    DWORD                       dwDMAPushContext;
    DWORD                       dwDmaPushBufferSize;
    DWORD                       hContexts_gbl;
    DWORD                       hTextures_gbl;
    DWORD                       hAGPList_gbl;
    DWORD                       hTextureHeap;
    DWORD                       hPipelines_gbl;
    DWORD                       hMaterials_gbl;
    DWORD                       dwDmaMaxPushCount;
    DWORD                       regD3DEnableBits1;
    DWORD                       regDmaPushBufferSizeMax;
    DWORD                       regMipMapLevels;
    DWORD                       regZOHMode;
    DWORD                       regTexHeap;
    DWORD                       regMinVideoTextureSize;
    DWORD                       regFilterControl;
    DWORD                       regDmaMinPushCount;
    DWORD                       regD3DContextMax;
    DWORD                       regD3DTextureMax;
    DWORD                       dwContextListHead;
    DWORD                       dwDmaFifoAddr;
    DWORD                       dwDmaFifoOffset;
    DWORD                       dwDmaFifoCurrentBuffer;
    DWORD                       dwDmaCount;

    // DirectDraw stuff
    DWORD                       fReset;
    DD_HALINFO                  HALInfo;    // This is unreferenced... check to see if we need it @mjl@
    DD_CALLBACKS                DDCallbacks;
    DD_SURFACECALLBACKS         DDSurfaceCallbacks;
    DD_PALETTECALLBACKS         DDPaletteCallbacks;
#endif
} GLOBALDATA;
#endif // #ifndef NVD3D

/***************************************************************************
 ***************************************************************************/
#ifdef _WIN32

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

extern GLOBALDATA           DriverData;     // in ddmini16.c

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
void FAR PASCAL HWBeginAccess(DIBENGINE FAR *,short,short,DWORD,DWORD);
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

/* Escape functions */
#define MOUSETRAILS         39
#define DCICOMMAND          3075
#define GETSURFINFO         0x6979
#define GETNVCHANNELPTR     0x6980
#define RECONFIGNVOFFSCREEN 0x6981
#define NVSETDDRAWMODE      0x6982


#ifndef DX7   // For DX7, this is defined in ddmini.h.
/* AGP stuff */
#define NV_MAX_AGP_MEMORY_LIMIT 0x1FFFFFF
#endif

/* Dma flags */
#define NV_WIN_DMA_PUSHER_IS_ACTIVE  1
#define NV_DD_DMA_PUSHER_IS_ACTIVE   2

/* Floating system memory context in use flags */
#define NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY        1
#define NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY        2
#define NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY        4
#define NV_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY        8
#define NV_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY       16

/* Other NV specific defines. NV3 and NV4 surface alignments have
   been changed since NV10 video scaler only supports 64 byte aligned
   surfaces. Note that changing these #defines causes ALL vidmem surfaces
   to be 64 byte aligned.
 */
#define NV3_BYTE_ALIGNMENT_PAD   15L
#define NV3_BIT_ALIGNMENT        128
#define NV4_BYTE_ALIGNMENT_PAD   31L
#define NV4_BIT_ALIGNMENT        256
#define NV10_BYTE_ALIGNMENT_PAD  63L
#define NV10_BIT_ALIGNMENT       512

#define NV10_INITIAL_PITCH    64
#define NV4_INITIAL_PITCH     32

/* NV Device Version Numbers */
#define NV_DEVICE_VERSION_4     0x0004
#define NV_DEVICE_VERSION_5     0x0005
#define NV_DEVICE_VERSION_10    0x0010

#ifndef NVD3D

#if _WIN32_WINNT < 0x0500
//*************************************************************************
// Misc stuff which changed between dx3 and dx5, Win95 and WinNT
//*************************************************************************
#define DDSCAPS_LOCALVIDMEM DDSCAPS_VIDEOMEMORY
#ifndef NV_AGP
// BUGBUG need to undefine this when AGP support arrives
#define DDSCAPS_NONLOCALVIDMEM DDSCAPS_SYSTEMMEMORY
#endif
#endif // #if _WIN32_WINNT < 0x0500
#endif  // #ifndef NVD3D

/***************************************************************************
 *
 * DEBUG stuff
 *
 ***************************************************************************/
#ifdef IS_32
#ifdef DEBUG
#if defined(_WIN32_WINNT)
//#define DPF(a) DISPDBG((0, (a))
#define BREAK() EngDebugBreak();
#else
    extern void __cdecl DPF(LPSTR szFormat, ...);
    #define BREAK() DebugBreak();
#endif
#else
    #define DPF         1 ? (void)0 : (void)
    #define BREAK()
#endif
#else
    #define DPF         1 ? (void)0 : (void)
    #define BREAK()
#endif

#define DDRAW_SET_PRIMARY(ppdev, Offset, Stride)        \
       if (Stride < 32)                                 \
           ppdev->DdCurrentDestPitch = 32;              \
       else                                             \
           {                                            \
           ppdev->DdCurrentDestPitch = ((Stride + ppdev->ulSurfaceAlign ) & ~ppdev->ulSurfaceAlign);          \
           }                                            \
                                                        \
       ppdev->DdCurrentDestOffset = Offset;             \
       while (freeCount < 3*4)                          \
           freeCount = NvGetFreeCount(npDev, NV_DD_PRIMARY);    \
       freeCount -= 3*4;                                \
                                                        \
       npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetPitch = ((ppdev->DdCurrentDestPitch << 16) | (ppdev->DdCurrentSourcePitch));  \
       npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetOffsetSource = ppdev->DdCurrentSourceOffset;  \
       npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetOffsetDestin = Offset

#define DDRAW_SET_SOURCE(ppdev, Offset, Stride)         \
    if (Stride < 32)                                    \
        ppdev->DdCurrentSourcePitch = 32;               \
    else                                                \
        {                                               \
        ppdev->DdCurrentSourcePitch = ((Stride + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign);          \
        }                                               \
    ppdev->DdCurrentSourceOffset = Offset;              \
    while (freeCount < 3*4)                             \
        freeCount = NvGetFreeCount(npDev, NV_DD_PRIMARY);    \
    freeCount -= 3*4;                                   \
    npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetPitch = ((ppdev->DdCurrentDestPitch <<16) | (ppdev->DdCurrentSourcePitch));    \
    npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetOffsetSource = Offset;  \
    npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetOffsetDestin = ppdev->DdCurrentDestOffset

#if (_WIN32_WINNT >= 0x0500) && defined(NV3) && !defined(DISPDRV)
#ifdef RM_HEAPMGR
#define HEAP_ALLOC_DEPTH_WIDTH_HEIGHT        1
#define HEAP_ALLOC_SIZE                      2
#define HEAP_FREE                            3
#define HEAP_PURGE                           4
#define HEAP_INFO                            5
#define TYPE_IMAGE                           0
#define TYPE_DEPTH                           1
#define TYPE_TEXTURE                         2
#define TYPE_OVERLAY                         3
#define TYPE_FONT                            4
#define TYPE_CURSOR                          5
#define TYPE_DMA                             6
#define TYPE_INSTANCE                        7
#define MEM_TYPE_PRIMARY                     8
#define MEM_TYPE_IMAGE_TILED                 9

extern BOOL __fastcall bMoveAllDfbsFromOffscreenToDibs(PDEV *);

// TBD: create a better way of allocating and freeing memory - huge macro!  Baaaadddd. -@mjl@
#define NVHEAP_ALLOC(sts,pvm,sz,tp)             \
{                                           \
    NVOS11_PARAMETERS   HeapParams;         \
    PVOID pParms = (PVOID) &HeapParams;     \
    DWORD cbReturned;                       \
                                            \
    if (!PDEV_PTR()->DDrawVideoSurfaceCount)     \
    {                                       \
        PDEV_PTR()->cbGdiHeap = PDEV_PTR()->VideoHeapTotal -   \
        PDEV_PTR()->VideoHeapFree;                      \
                                                                    \
    }                                       \
                                                    \
    HeapParams.hRoot = PDEV_PTR()->hClient;  \
    HeapParams.hObjectParent = PDEV_PTR()->hDevice;                  \
    HeapParams.function = HEAP_ALLOC_SIZE;                      \
    HeapParams.owner    = 'NVDD';                               \
    HeapParams.type     = (tp);                                 \
    HeapParams.size     = (sz);                                 \
    EngDeviceIoControl(                         \
        PDEV_PTR()->hDriver,            \
        (DWORD)IOCTL_NV01_ARCH_HEAP,            \
        (&pParms),                              \
        sizeof(PVOID),                          \
        pParms,                                 \
        sizeof(NVOS11_PARAMETERS),              \
        &cbReturned                             \
    );                                          \
    (pvm) = HeapParams.status ? 0 : HeapParams.offset;          \
    (sts) = HeapParams.status; \ 
    if (!(pvm))                                                 \
    {                                                           \
        bMoveAllDfbsFromOffscreenToDibs(PDEV_PTR());    \
        HeapParams.hRoot = PDEV_PTR()->hClient;  \
        HeapParams.hObjectParent = PDEV_PTR()->hDevice;                  \
        HeapParams.function = HEAP_ALLOC_SIZE;                      \
        HeapParams.owner    = 'NVDD';                               \
        HeapParams.type     = (tp);                                 \
        HeapParams.size     = (sz);                                 \
        EngDeviceIoControl(                         \
            PDEV_PTR()->hDriver,            \
            (DWORD)IOCTL_NV01_ARCH_HEAP,            \
            (&pParms),                              \
            sizeof(PVOID),                          \
            pParms,                                 \
            sizeof(NVOS11_PARAMETERS),              \
            &cbReturned                             \
        );                                          \
        (pvm) = HeapParams.status ? 0 : HeapParams.offset;          \
    }                                               \
    if (pvm)                                        \
        PDEV_PTR()->VideoHeapFree = HeapParams.free;    \
}

#define NVHEAP_FREE(pvm)                    \
{                                           \
    NVOS11_PARAMETERS HeapParams;           \
    PVOID pParms = (PVOID) &HeapParams;     \
    DWORD cbReturned;                       \
    HeapParams.hRoot = PDEV_PTR()->hClient;      \
    HeapParams.hObjectParent = PDEV_PTR()->hDevice; \
    HeapParams.function = HEAP_FREE;        \
    HeapParams.owner    = 'NVDD';         \
    HeapParams.offset   = (U032)(pvm);      \
    EngDeviceIoControl(                         \
        PDEV_PTR()->hDriver,                                \
        (DWORD)IOCTL_NV01_ARCH_HEAP,            \
        (&pParms),                              \
        sizeof(PVOID),                          \
        pParms,                                 \
        sizeof(NVOS11_PARAMETERS),              \
        &cbReturned                             \
    );                                          \
    PDEV_PTR()->VideoHeapFree = HeapParams.free;              \
}
#else // !RM_HEAPMGR
#define NVHEAP_INIT(strt, end, heap)  \
    heap_init((strt), (end), heap)
#define NVHEAP_INFO()
#define NVHEAP_CLEAR(heap)  \
    heap_clear(heap)
#define NVHEAP_ALLOC(pvm,sz,tp)             \
    (pvm) = heap_alloc(sz, (memory_t *) (ppdev)->heap.heap_2d_ptr);
// TBD: where do we get sts in this case?
#define NVHEAP_FREE(pvm)             \
    heap_free((pvm), (memory_t *) (ppdev)->heap.heap_2d_ptr);
#endif // !RM_HEAPMGR
#endif // NV3

//**************************************************************************************
// Common ddraw fct prototypes.
//**************************************************************************************
EXTERN_C BOOL __stdcall nvDeterminePerformanceStrategy_NT4(GLOBALDATA *pDriverData, NV_SystemInfo_t *pSysInfo);
EXTERN_C DWORD __stdcall DdCanCreateSurface(PDD_CANCREATESURFACEDATA lpCanCreateSurface );
EXTERN_C DWORD __stdcall DdSetColorKey(PDD_SETCOLORKEYDATA lpSetColorKey);
EXTERN_C DWORD DdCreateSurface(PDD_CREATESURFACEDATA lpCreateSurface);
EXTERN_C DWORD DdGetFlipStatus(PDD_GETFLIPSTATUSDATA lpGetFlipStatus);
EXTERN_C DWORD DdMapMemory(PDD_MAPMEMORYDATA lpMapMemory);
EXTERN_C DWORD DdWaitForVerticalBlank(PDD_WAITFORVERTICALBLANKDATA lpWaitForVerticalBlank);
EXTERN_C DWORD GetScanLine32(PDD_GETSCANLINEDATA lpGetScanLine );
DWORD Nv3Blt(PDD_BLTDATA pbd);
DWORD Nv4Blt(PDD_BLTDATA pbd);
DWORD __stdcall Nv3DestroySurface( PDD_DESTROYSURFACEDATA lpDestroySurface );
DWORD __stdcall Nv4DestroySurface( PDD_DESTROYSURFACEDATA lpDestroySurface );
DWORD __stdcall Nv3Flip(PDD_FLIPDATA pfd);
DWORD __stdcall Nv4Flip(PDD_FLIPDATA pfd);
DWORD Nv3GetBltStatus(PDD_GETBLTSTATUSDATA lpGetBltStatus);
DWORD Nv4GetBltStatus(PDD_GETBLTSTATUSDATA lpGetBltStatus);
DWORD WINAPI DdGetDriverInfo(struct _DD_GETDRIVERINFODATA *lpData);
DWORD Nv3Lock(PDD_LOCKDATA lpLockData);
DWORD Nv4Lock(PDD_LOCKDATA lpLockData);
DWORD __stdcall DdSetOverlayPosition( PDD_SETOVERLAYPOSITIONDATA lpSOPData );
DWORD __stdcall Nv3Unlock( PDD_UNLOCKDATA lpUnlockData );
DWORD __stdcall Nv4Unlock( PDD_UNLOCKDATA lpUnlockData );
DWORD __stdcall Nv3UpdateOverlay( PDD_UPDATEOVERLAYDATA lpUOData );
DWORD __stdcall Nv4UpdateOverlay( PDD_UPDATEOVERLAYDATA lpUOData );
#ifdef NVD3D
DWORD WINAPI GetAvailDriverMemory (PDD_GETAVAILDRIVERMEMORYDATA  pDmd);
#endif
HRESULT Nv3UpdateFlipStatus( PDEV *ppdev, FLATPTR fpVidMem );
unsigned long Nv4UpdateFlipStatus( PDEV *ppdev, FLATPTR fpVidMem );
DWORD __stdcall Nv3UnlockOverlay( PDD_UNLOCKDATA lpUnlockData );
DWORD __stdcall Nv4UnlockOverlay( PDD_UNLOCKDATA lpUnlockData );
BOOLEAN bCreateNV4DDPatchPio(PDEV *ppdev);
BOOLEAN bDestroyNV4DDPatchPio(PDEV *ppdev);
DWORD Nv3GetFlipStatus(PDD_GETFLIPSTATUSDATA lpGetFlipStatus);
DWORD Nv4GetFlipStatus(PDD_GETFLIPSTATUSDATA lpGetFlipStatus);
DWORD __stdcall Nv3SetOverlayPosition( PDD_SETOVERLAYPOSITIONDATA lpSOPData );
DWORD __stdcall Nv4SetOverlayPosition( PDD_SETOVERLAYPOSITIONDATA lpSOPData );
extern DWORD __stdcall Nv3Blt8( PDD_BLTDATA pbd );
extern DWORD __stdcall Nv3Blt16( PDD_BLTDATA pbd );
extern DWORD __stdcall Nv3Blt32( PDD_BLTDATA pbd );
extern DWORD __stdcall Nv4Blt8( PDD_BLTDATA pbd );
extern DWORD __stdcall Nv4Blt16( PDD_BLTDATA pbd );
extern DWORD __stdcall Nv4Blt32( PDD_BLTDATA pbd );

//------------------------------------------------------------------------------
// The minimum depth of the NV chip's FIFO -- never wait for the free count
// to reach a value greater than NV_GUARANTEED_FIFO_SIZE (124 bytes)!
//------------------------------------------------------------------------------

#define NV_GUARANTEED_FIFO_SIZE                 (0x007C)

#endif










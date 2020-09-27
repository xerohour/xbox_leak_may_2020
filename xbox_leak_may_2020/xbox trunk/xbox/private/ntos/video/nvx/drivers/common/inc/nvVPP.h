 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

/*==========================================================================;
 *
 *  File:       nvVPP.h
 *  Content:    Video Post Processing
 *
 ***************************************************************************/

#ifndef _NVVPP_H_
#define _NVVPP_H_

#include "CompileControl.h"

#include "nvTypes.h"
#include "ddProcMan.h"
#include "nvPusher.h"
#include "nvUniversal.h"
#include "nvMultiMon.h" // Need twinview definition

// {BFD51181-16C4-11d4-8E67-00104B9D3738}
static const GUID GUID_NV_OVERLAY_INFO = { 0xbfd51181, 0x16c4, 0x11d4, { 0x8e, 0x67, 0x0, 0x10, 0x4b, 0x9d, 0x37, 0x38 } };

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Objects created and destroyed by the VPP code
//  Note: IID = instance ID
// TBD: lots of the code in ddraw common assumes these objects exist and interact with them
//   directly.  They should be changed to access them only through VPP routines. -mlavoie
#define NV_VPP_SWIZZLE_BLIT_IID                     0xDD00B000
#define NV_VPP_OVERLAY_IID                          0xDD00B010
#define NV_VPP_V2V_FORMAT_Y_IID                     0xDD00B020
#define NV_VPP_ALPHA_BLIT_IID                       0xDD00B030
#define NV_VPP_TIMER_IID                            0xDD00B040


#define NV_VPP_VIDEO_MEMORY_TO_OVERLAY_SHADOW_FORMAT_IID   0xDD003FE4
#define NV_VPP_OVERLAY_SHADOW_TO_VIDEO_MEMORY_FORMAT_IID   0xDD003FE5

#define NV_VPP_MAX_OVERLAY_SURFACES  10
#define NV_VPP_MAX_EXTRA_SURFACES    6
#define NV_VPP_MAX_EXTRA_FS_SURFACES 4


// vppVPPandFlip flags
#define VPP_ODD             NV4_REG_VPP_INV_MASK_ODD
#define VPP_EVEN            NV4_REG_VPP_INV_MASK_EVEN
#define VPP_BOB             NV4_REG_VPP_INV_MASK_BOB
#define VPP_INTERLEAVED     NV4_REG_VPP_INV_MASK_INTERLEAVED
#define VPP_VIDEOPORT       NV4_REG_VPP_INV_MASK_VIDEOPORT
#define VPP_WAIT            NV4_REG_VPP_INV_MASK_WAIT
#define VPP_SAVE_STATE      NV4_REG_VPP_SAVE_STATE_DISABLE
#define VPP_RESTORE_STATE   NV4_REG_VPP_RESTORE_STATE_DISABLE
#define VPP_CONVERT         NV4_REG_VPP_CONVERT_DISABLE
#define VPP_SUBPICTURE      NV4_REG_VPP_SUBPICTURE_DISABLE
#define VPP_PRESCALE        NV4_REG_VPP_PRESCALE_DISABLE
#define VPP_COLOURCONTROL   NV4_REG_VPP_COLOURCONTROL_DISABLE
#define VPP_TEMPORAL        NV4_REG_VPP_TEMPORAL_DISABLE
#define VPP_OPTIMIZEFLIP    NV4_REG_VPP_OPTIMIZEFLIP_DISABLE
#define VPP_DEINTERLACE     NV4_REG_VPP_DEINTERLACE_DISABLE
#define VPP_FSMIRROR        NV4_REG_VPP_FSMIRROR_DISABLE
#define VPP_DMABLIT_DISABLE NV4_REG_VPP_DMABLIT_DISABLE
#define VPP_MASTER_DISABLE  NV4_REG_VPP_MASTER_DISABLE
// TBD: why redefine these?  Note that master disable WAS being used directly anyway. - mlavoie

#define VPP_ALL             (VPP_CONVERT | VPP_SUBPICTURE | VPP_PRESCALE | VPP_COLOURCONTROL | VPP_DEINTERLACE | VPP_TEMPORAL | VPP_FSMIRROR)
#define VPP_STATIC          (VPP_CONVERT | VPP_SUBPICTURE | VPP_PRESCALE | VPP_COLOURCONTROL | VPP_FSMIRROR)
#define VPP_MOCOMP          (VPP_PRESCALE | VPP_FSMIRROR)

#define IS_OVERLAY(caps,fourcc) ((caps & DDSCAPS_OVERLAY) && (fourcc != FOURCC_NVDS) && (fourcc != FOURCC_NVMC))
#define IS_FIELD(f) ((f & (VPP_INTERLEAVED | VPP_BOB)) && (f & (VPP_ODD | VPP_EVEN)))

#ifdef ENABLE_VPP_DISPATCH_CODE
// This code is no longer used but may someday be useful.

// dispatcher commands
#define VPPDISPATCH_NOOP            0x0
#define VPPDISPATCH_DISPATCH        0x1
#define VPPDISPATCH_SETPRIORITY     0x2
#define VPPDISPATCH_RESTOREPRIORITY 0x4
#define VPPDISPATCH_EXIT            0xFFFFFFFF

// dispatcher state
#define VPPDSTATE_WAITING           0x1
#define VPPDSTATE_BUSY              0x2
#define VPPDSTATE_INITERROR         0x10000
#define VPPDSTATE_CMDERROR          0x20000
#define VPPDSTATE_TERMINATED        0x80000000

#endif

#ifdef DEBUG
    // so we know when something is drastically wrong
    #define VPP_TIMEOUT_TIME    250
    // snooping automatically enabled unless we're on NT where it doesn't work
    #ifndef WINNT
    #define VPP_SNOOP           1
    #endif
#else  // !DEBUG
    // maximum time we can spend on a frame
    #define VPP_TIMEOUT_TIME    42
    #ifdef DEVELOP
        #ifndef WINNT
        #define VPP_SNOOP       1
        #endif
    #endif  // DEVELOP
#endif  // !DEBUG

typedef struct {
  NvF32 sx;
  NvF32 sy;
  NvF32 sz;
  NvF32 rhw;
  NvV32 color;
  NvV32 specular;
  NvF32 tu0;
  NvF32 tv0;
  NvF32 tu1;
  NvF32 tv1;
} Tlmtvertex;

typedef struct Vpp_s Vpp_t;

void VppConstructor(GLOBALDATA *pDriverData,Vpp_t *pVpp);
void VppDestructor(Vpp_t *pVpp);

BOOL VppReadRegistry(Vpp_t *pVpp);


// Exported Functions
NvU8 VppPrepareResources(
    Vpp_t           *pVpp,
    CPushBuffer     *pPusher,
    NV_SystemInfo_t *pSysInfo,
    NvU32            hChannel,
    NvU32            hThreeDClass,
    NvU32            threeDClassSubCh,
    NvU8             threeDClassType,
    NvU32            hInVideoMemContextDma,
    NvU32            hFromVideoMemContextDma,
    NvU32            hToVideoMemContextDma,
    NvU32            hLutCursorDac,
    NvU32            hOverlayDmaToMemNotifier,
    NvU32            hOverlayFlipNotifier,
    NvU32            hMiscEventNotifier,
    NvU32            hContextSurfaceSwizzled,
    NvU32            hContextBeta4,
    NvU32            hSurfaces2D,
    NvU32            surfaces2DSubCh,
    NvU32            hVideoMemUtoVideoMemFormat,    // U-conversion object
    NvU32            hVideoMemVtoVideoMemFormat,    // V-conversion object
    NvU32            hVideoMemUVtoVideoMemFormat,    // UV-conversion object
    NvU32            hDvdSubpicture,
    NvU32            hContextPattern,
    NvU32            hContextColorKey,
    NvU32            hGenericScaledImage,
    NvU32            hFloatingContextDmaInOverlayShadow,
    NvNotification  *pFlipOverlayNotifier,  // The above hOverlayFlipNotifier MUST be really wrong! -mlavoie
    NvNotification  *pFlipPrimaryNotifier,  
    NvNotification  *pPusherSyncNotifier,
    NvNotification  *pColorControlNotifier,

    NvU32            hContextSurfacesARGB_ZS,
    NvU32            hDX6MultiTextureTriangle,
    NvU32            hTimer,

    NvU32            ropRectTextSubCh,
    NvU32            ropSubCh,
    NvU32            spareSubCh);


NvU8 VppSetOverlayColourControl(Vpp_t *pVpp);

BOOL VppEnable(Vpp_t *pVpp);
void VppDisable(Vpp_t *pVpp);
BOOL VppEnableFSMirror(Vpp_t *pVpp, DWORD dwWidth, DWORD dwHeight);
void VppDisableFSMirror(Vpp_t *pVpp);
BOOL VppDoFlip(Vpp_t *pVpp,DWORD dwOffset, DWORD dwPitch, DWORD dwWidth,
                   DWORD dwHeight, DWORD dwFourCC, DWORD dwFlags);

#if (IS_WINNT5 | IS_WIN9X)
#define NV_GET_VIDEO_HEAP_FREE() (DWORD)vpp.pDriverData->VideoHeapFree

#elif (IS_WINNT4)
#define NV_GET_VIDEO_HEAP_FREE() (DWORD)vpp.pDriverData->ppdev->VideoHeapFree

#else
#error Unsupported OS.
#endif

/*
 * NV_ColorControl_t
 *    'Derived' from DDCOLORCONTROL for multi-OS compatibility
 */
#define NV_VPP_COLOR_BRIGHTNESS		0x00000001l // Field validity flags
#define NV_VPP_COLOR_CONTRAST		0x00000002l
#define NV_VPP_COLOR_HUE			0x00000004l
#define NV_VPP_COLOR_SATURATION		0x00000008l
#define NV_VPP_COLOR_SHARPNESS		0x00000010l
#define NV_VPP_COLOR_GAMMA			0x00000020l
#define NV_VPP_COLOR_COLORENABLE	0x00000040l

typedef struct {
    NvU32               dwLastFlags;
    NvU32               dwLastExec;
    NvU32               dwMarker1;
    NvU32               dwReserved1;

    LONGLONG            qwCount;    // TBD: what to do with LONGLONG? -mlavoie
    
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
} Vpp_snoop_t;


typedef struct NV_ColorControl_s
{
    NvU32 		dwSize;
    NvU32		dwFlags;
    NvS32		lBrightness;
    NvS32		lContrast;
    NvS32		lHue;
    NvS32 		lSaturation;
    NvS32		lSharpness;
    NvS32		lGamma;
    NvS32		lColorEnable;
    NvU32		dwReserved1;
} Nv_ColorControl_t;

// Experimental struct (may replace overlay corresponding vars with this as well)
typedef struct Nv_Surface_s
{
    NvU32       format; // TBD: enumerate - mlavoie
    NvU32       originX;
    NvU32       originY;
    NvU32       pitch;
    NvU32       width;
    NvU32       height;
    NvU32       offset; // General offset?  Unsure -mlavoie
} Nv_Surface_t;


#define NV_MAX_EXTRA_FS_SURFACES 4

// Copied from DDMINI.H - mlavoie

/* Shared Celsius class usage flags */
#define CELSIUSUSER_NONE                0  // celsius object is untouched and (almost) fully uninitialized
#define CELSIUSUSER_D3D                 1
#define CELSIUSUSER_DDRAW               2
#define CELSIUSUSER_DDRAW_VPP           3

#define VPP_3D_CLASS_IS_UNDEFINED 0
#define VPP_3D_CLASS_IS_CELSIUS   1
#define VPP_3D_CLASS_IS_KELVIN    2

#define VPP_FLAG_CONSTRUCTED    0x00000001 // VPP class has been initialized
#define VPP_FLAG_READY          0x00000002 // Resources have been assigned, and VPP is ready to be enabled
#define VPP_FLAG_ENABLED        0x00000004 // VPP has been enabled and is running
#define VPP_FLAG_CELSIUS_3D     0x00000008 // VPP is using a celsius class as it's 3D object
#define VPP_FLAG_KELVIN_3D      0x00000010 // VPP is using a kelvin class as it's 3D object

typedef struct {
    NvU32               dwOpCount;
    HDRVEVENT           hLastStage;
    NvU8                doLateFlipSync;
    NvU32               dwIndex;
    NvU32               dwSrcOffset;
    NvU32               dwSrcPitch;
    NvU32               dwWidth;
    NvU32               dwHeight;
    NvU32               dwFourCC;
    NvU32               dwFlags;
    NvU32               dwWorkSurfaces;
    NvU32               vppExec;
    LONGLONG            qwTime1;
    LONGLONG            qwTime2;
    LONGLONG            qwDelta;
    LONGLONG            qwJitter;
} Vpp_pipeline_state_t;

// Note: The 3D class object is used for "advanced" blts that are not possible
//  using the normal bltter.  Surfaces are treated as textures and texture ops
//  are applied to get the desired results.
//
// TBD: turn this into a proper class (if we can get away from C enough) - mlavoie
//
typedef struct Vpp_s
{
    NvU32             dwFlags; // General flags detailing state of the VPP object (see VPP_FLAG_*)

    DWORD             dwOverlayEventsAllocated;        // used to keep track of overlay event allocation

    GLOBALDATA      *pDriverData;          // Hope to eliminate this some day. -mlavoie

    CPushBuffer      *pPusher;             // A cmd pusher class attached to a DMA push buffer
    NV_SystemInfo_t  *pSysInfo;  // System Information
    
    NvU32            hChannel;
    NvU32            hThreeDClass;        // Unique object ID for the 3D object to be used
    NvU32            threeDClassSubCh;    // Subchannel that _always_ holds the 3D class object
    NvU32            hInVideoMemContextDma;      // DMA context for V2V 
    NvU32            hFromVideoMemContextDma;    // DMA context for V2S
    NvU32            hToVideoMemContextDma;      // DMA context for S2V
    NvU32            hOverlayDmaToMemNotifier; // must be context DMA to memory type
    NvU32            hOverlayFlipNotifier;       // This appears to be a dma notifier too?!?! -mlavoie

    NvU32            hLutCursorDacBase;   // Base object ID for DACs (1 per head)
    NvU32            hMiscEventNotifier;   // Used as the notifier in many cases
    NvU32            hContextSurfaceSwizzled;
    NvU32            hContextBeta4;
    NvU32            hSurfaces2D;
    NvU32            surfaces2DSubCh;       // May be able to just use spare? -mlavoie

    NvU32            hVideoMemUtoVideoMemFormat;    // U-conversion object
    NvU32            hVideoMemVtoVideoMemFormat;    // V-conversion object
    NvU32            hVideoMemUVtoVideoMemFormat;    // UV-conversion object

    NvU32            hContextPattern;
    NvU32            hContextColorKey;


    NvU32            hDvdSubpicture;
    NvU32            hGenericScaledImage;
    NvU32            hFloatingContextDmaInOverlayShadow;

    NvNotification  *pFlipOverlayNotifier;  // The above hOverlayFlipNotifier MUST be really wrong! -mlavoie
    NvNotification  *pFlipPrimaryNotifier;
    NvNotification  *pPusherSyncNotifier;
    NvNotification  *pColorControlNotifier; // This isn't really for color control! -mlavoie rename

    NvU32            hContextSurfacesARGB_ZS;
    NvU32            hDX6MultiTextureTriangle;
    NvU32            hTimer;

    NvU32            ropRectTextSubCh;
    NvU32            ropSubCh;
    NvU32            spareSubCh;          // General use subchannel (SetObject always done first)

    // TBD:hMiscEventNotifier was using NV_DD_DMA_COLOUR_CONTROL_EVENT_NOTIFIER which IS NV_DD_PIO_COLOUR_CONTROL_EVENT_NOTIFIER -mlavoie

    NvU32            dwOverlayFSNumSurfaces;
    NvU32            dwOverlayFSHead;    //what head is overlay on over is head on
    NvU32            dwOverlayFSOvlHead; // where mirror is
    NvU32            dwOverlayFSOvlHeadSaved;
    NvU32            dwOverlayFSOvlLost;

    NvU32            dwOverlayFSOldOffset;
    NvU32            dwOverlayFSWidth;
    NvU32            dwOverlayFSHeight;
    NvU32            dwOverlayFSPitch;
    NvU32            dwOverlayFSFormat;
    NvU32            dwOverlayFSOffset[NV_VPP_MAX_EXTRA_FS_SURFACES];

    NvU32   dwOverlayFSIndex;
    NvU32   dwOverlayFSDeltaX;
    NvU32   dwOverlayFSDeltaY;

    // Size & offset of each of the extra surfaces used to run the VPP engine
    NvU32   extraOverlayOffset[NV_VPP_MAX_EXTRA_SURFACES];   // can be up to n "chunks" longer original surface
    NvU32   extraPitch;
    NvU32   extraNumSurfaces;
    NvU32   extraIndex; // Next 'extra' surface to be used

    NvU32   dwOverlaySurfaces;
    NvU32   dwOverlayOwner;
    NvU32   dwOverlaySurfaceLCL[NV_VPP_MAX_OVERLAY_SURFACES];
    NvU32   dwOverlaySrcWidth;
    NvU32   dwOverlaySrcHeight;
    NvU32   dwOverlayDstWidth;
    NvU32   dwOverlayDstHeight;
    NvU32   dwOverlayDeltaX;
    NvU32   dwOverlayDeltaY;

    NvU32   dwOverlaySrcX;          // overlay source starting point
    NvU32   dwOverlaySrcY;
    NvU32   dwOverlayDstX;        // overlay Dst starting point
    NvU32   dwOverlayDstY;
    NvU32   dwOverlaySrcOffset;
    NvU32   dwOverlayFormat;      // current overlay format - used so VDD knows what's up!
    NvU32   dwOverlaySrcPitch;    // current overlay pitch   - used so VDD knows what's up!
///
    NvU32   dwOverlayLastVisibleSurfaceLCL;
    NvU32   dwOverlayLastVisibleFpVidMem;
    NvU32   dwOverlayLastVisiblePitch;
    NvU32   dwOverlayLastVisiblePixelFormat;
    NvU32   dwOverlayFlipCount;
    NvU16   overlayBufferIndex;
    NvU16   overlayRelaxOwnerCheck;
    NvU32   dwOverlaySrcSize;
    NvU32   dwOverlayColorKey;

    NvU32                       dwOverlayCachedFlags;
    NvU32                       dwOverlayFSSrcWidth;
    NvU32                       dwOverlayFSSrcHeight;
    NvU32                       dwOverlayFSSrcMinX;
    NvU32                       dwOverlayFSSrcMinY;
    NVTWINVIEWDATA              dwOverlayFSTwinData;
    NvU32                       fpOverlayShadow;
    NvU32                       dwOverlayMaxDownScale;
    NvU32                       dwOverlayMaxDownScaleX;
    NvU32                       dwOverlayMaxDownScaleY;
    NvU32                       regOverlayColourControlEnable;
    NvU32                       dwOverlayByteAlignmentPad;
    NvU32                       regOverlayMode;
    NvU32                       regOverlayMode2;
    NvU32                       regOverlayMode3;
    NvU32                       regVPPInvMask;
    NvU32                       regVPPMaxSurfaces;

    NvU32                       regRing0ColourCtlInterlockFlags;    // bit0 = 1 -> ring0 should run; bit2 = 1 ring0 in progress
   


    NvU32                       dwOverlayMode;            // used to tell if we're doing interleaved or what...
//    VPPDISPATCH                 vppDispatch;          No longer used.
///

    NvU32                       dwPrevFrameOffset;

    __int64                     llDeliveryPeriod;    // in ns
    __int64                     llDeliveryTime;      // in ns
    __int64                     llDeliveryStart;     // in ns

    Nv_ColorControl_t    colorCtrl;

    Nv_Surface_t         subPicture; // Dvd Subpicture

    Vpp_pipeline_state_t pipeState;

    Vpp_snoop_t          snoop;

} Vpp_t;


#define DEFAULT_FINAL_CW0   (NV056_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE_FALSE << 29) |   \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA_FALSE << 28) |     \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_0 << 24) |    \
                                                                                            \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE_FALSE << 21) |   \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA_FALSE << 20) |     \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_0 << 16) |    \
                                                                                            \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE_FALSE << 13) |   \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA_FALSE << 12) |     \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_0 << 8) |     \
                                                                                            \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE_FALSE << 5) |    \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA_FALSE << 4) |      \
                             NV056_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_C

#define DEFAULT_KELVIN_FINAL_CW0   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE_FALSE << 29) |   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA_FALSE << 28) |     \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_0 << 24) |    \
                                                                                                   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE_FALSE << 21) |   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA_FALSE << 20) |     \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_0 << 16) |    \
                                                                                                   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE_FALSE << 13) |   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA_FALSE << 12) |     \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_0 << 8) |     \
                                                                                                   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE_FALSE << 5) |    \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA_FALSE << 4) |      \
                                    NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_C

// TBD: add CELSIUS to the following - mlavoie
#define DEFAULT_FINAL_CW1   (NV056_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE_FALSE << 29) |   \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA_FALSE << 28) |     \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_0 << 24) |    \
                                                                                            \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE_FALSE << 21) |   \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA_FALSE << 20) |     \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_0 << 16) |    \
                                                                                            \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE_FALSE << 13) |   \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA_TRUE << 12) |      \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_C << 8) |     \
                                                                                            \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP_TRUE << 7) | \
                            (NV056_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5_FALSE << 6) | \
                             NV056_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12_FALSE

#define DEFAULT_KELVIN_FINAL_CW1   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE_FALSE << 29) |   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA_FALSE << 28) |     \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_0 << 24) |    \
                                                                                                   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE_FALSE << 21) |   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA_FALSE << 20) |     \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_0 << 16) |    \
                                                                                                   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE_FALSE << 13) |   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA_TRUE << 12) |      \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_C << 8) |     \
                                                                                                   \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP_TRUE << 7) | \
                                   (NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5_FALSE << 6) | \
                                    NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12_FALSE

#ifdef __cplusplus
}
#endif // __cplusplus

#endif _NVVPP_H_

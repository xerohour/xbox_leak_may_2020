 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2001 NVIDIA, Corporation.  All rights reserved.      *|
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
#include "nvProcMan.h"
#include "nvPusher.h"
#include "nvRegTool.h"
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
//   directly.  They should be changed to access them only through VPP routines. -@mjl@
#define NV_VPP_SWIZZLE_BLIT_IID     0xBB00B000
#define NV_VPP_OVERLAY_IID          0xBB00B010
#define NV_VPP_V2V_FORMAT_Y_IID     0xBB00B020
#define NV_VPP_ALPHA_BLIT_IID       0xBB00B030
#define NV_VPP_TIMER_IID            0xBB00B040
#define NV_VPP_V2OSH_FORMAT_IID     0xBB00B050  // Video Mem -> Overlay SHadow (OSH)
#define NV_VPP_OSH2V_FORMAT_IID     0xBB00B060
#define NV_VPP_SCALED_IMAGE1_IID    0xBB00B070
#define NV_VPP_SCALED_IMAGE2_IID    0xBB00B080
#define NV_VPP_DMABLT_TO_VID_IID    0xBB00B090
#define NV_VPP_DVD_SUBPICTURE_IID   0xBB00B0A0

#define NV_VPP_SWIZZLE_BLIT_CONTEXT_IID             0xBB00C000
#define NV_VPP_OVERLAY_CONTEXT_IID                  0xBB00C010
#define NV_VPP_V2V_FORMAT_Y_CONTEXT_IID             0xBB00C020
#define NV_VPP_ALPHA_BLIT_CONTEXT_IID               0xBB00C030
#define NV_VPP_TIMER_CONTEXT_IID                    0xBB00C040
#define NV_VPP_V2OSH_FORMAT_CONTEXT_IID             0xBB00C050
#define NV_VPP_OSH2V_FORMAT_CONTEXT_IID             0xBB00C060
#define NV_VPP_SCALED_IMAGE1_CONTEXT_IID            0xBB00C070
#define NV_VPP_SCALED_IMAGE2_CONTEXT_IID            0xBB00C080
#define NV_VPP_DMABLT_TO_VID_CONTEXT_IID            0xBB00C090
#define NV_VPP_DVD_SUBPICTURE_CONTEXT_IID           0xBB00C0A0

#define NV_VPP_CONTEXT_DMA_MEMORY_IID               0xBB00C990  // Used only on Win9x systems


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
// TBD: why redefine these?  Note that master disable WAS being used directly anyway. - @mjl@

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

//
// Exported functions
//
#ifdef WINNT
extern NvU8 VppConstructor(PDEV *ppdev, Vpp_t *pVpp, NV_SystemInfo_t *pSysInfo);
#else
extern NvU8 VppConstructor(GLOBALDATA *pDriverData, Vpp_t *pVpp, NV_SystemInfo_t *pSysInfo);
#endif
extern void VppDestructor(Vpp_t *pVpp);
extern NvU8 VppReadRegistry(Vpp_t *pVpp, HANDLE hDriver, NvU32 logicalHeadID);
extern void VppBltWriteDword(Vpp_t *pVpp, NvU32 dwOffset, NvU32 dwIndex, NvU32 dwData);
extern NvU8 VppIsOverlayActive(Vpp_t *pVpp);
extern NvU8 VppIsConstructed(Vpp_t *pVpp);
extern NvU8 VppGetVideoScalerBandwidthStatus(Vpp_t *pVpp, NvU16 wWidth);

extern int  VppWaitForNotification(NvNotification* pNotify, HDRVEVENT hEvent, unsigned long timeOut);
extern void VppResetNotification(NvNotification* pNotify, HDRVEVENT hEvent);

// Exported Functions
NvU8 VppEnable(
    Vpp_t           *pVpp,
    PDEV            *ppdev,
    CPushBuffer     *pPusher,
    NvU32            hChannel, 
    NvU32            hThreeDClass,
    NvU32            threeDClassSubCh,
    NvU32           *pThreeDClassLastUser,
    NvU32            hInVideoMemContextDma,
    NvU32            hFromVideoMemContextDma,
    NvU32            hToVideoMemContextDma,
    NvU32            hLutCursorDac,
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
    NvU32            hContextRop,
    NvU32            hContextColorKey,
    NvU32            hFloatingContextDmaInOverlayShadow,

    NvNotification  *pFlipPrimaryNotifier,  
    NvNotification  *pPusherSyncNotifier,
    NvNotification  *pUnused,

    NvU32            hContextSurfacesARGB_ZS,
    NvU32            hDX6MultiTextureTriangle,
    NvU32            hImageBlackRect,

    NvU32            ropRectTextSubCh,
    NvU32            ropSubCh,
    NvU32            spareSubCh);


NvU8 VppSetOverlayColourControl(Vpp_t *pVpp);

BOOL VppCreateOverlay(Vpp_t *pVpp);
void VppDestroyOverlay(Vpp_t *pVpp);
void VppDisable(Vpp_t *pVpp);
BOOL VppCreateFSMirror(Vpp_t *pVpp, DWORD dwWidth, DWORD dwHeight);
void VppDestroyFSMirror(Vpp_t *pVpp);
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

    LONGLONG            qwCount;    // TBD: what to do with LONGLONG? -@mjl@
    
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
    NvU32       format; // TBD: enumerate - @mjl@
    NvU32       originX;
    NvU32       originY;
    NvU32       pitch;
    NvU32       width;
    NvU32       height;
    NvU32       offset; // General offset?  Unsure -@mjl@
} Nv_Surface_t;


typedef struct {
    NvU32               dwOpCount;
    HDRVEVENT           hLastStage;
    NvNotification     *pLastStageNotifier;
    HDRVEVENT           hLastFSStage;
    NvNotification     *pLastFSStageNotifier;
    NvU8                doLateFlipSync;
    NvU8                doFSMirror;
    NvU16               unused_01;
    NvU32               dwIndex;
    NvU32               dwSrcOffset;
    NvU32               dwSrcPitch;
    NvU32               dwWidth;
    NvU32               dwHeight;
    NvU32               dwFourCC;
    NvU32               dwFlags;
    NvU32               dwWorkSurfaces;
    NvU32               vppExec;
    NvU32               dwPrescaleFactorX;
    NvU32               dwPrescaleFactorY;
#ifdef VPP_SNOOP
    LONGLONG            qwTime1;
    LONGLONG            qwTime2;
    LONGLONG            qwDelta;
    LONGLONG            qwJitter;
#endif
} Vpp_pipeline_state_t;

// *********************************************************************
// Notesf on m_obj_a array:
//
// Note that the 2nd field is sometimes overwritten depending on the hardware.  Usually
// to conditionally 'upgrade' the class.
//
// The entries MUST remain in the order that corresponds to the indexes (see nvVPP.h)
//
// Any entries with NV_DD_ names are allocated by the main driver and NOT the vpp.  They are in
// this array to provide a consistent access point to a notifier pointer.  Since the status
// is set to VPP_OBJ_NA, the alloc/dealloc routines will ignore the object.
//  [NB: there are none like this right now - @mjl@]
//
// *********************************************************************

typedef struct {
	NvU16 status;	            // Allocated by VPP, FREED, NA or ERROR
	NvU16 classNdx;             // Index that identifies the class
	NvU32 classIID;	         	// Instance of this class
	NvU32 contextIID;	        // Instance of a DMA context used by this class
	int notifierCount;          // # of notifier slots used by <classNdx>
    NvNotification* notifier_a; // Length of array depends on the object
} VppObjectRecord_t;

#define  VPP_OBJECT_COUNT 11

// This list MUST match the one in nvVPP.cpp -- need a better way to do this...@mjl@
#define OVERLAY_NDX         0
#define SWIZZLE_NDX         1 
#define TIMER_NDX           2 
#define ALPHA_BLT_NDX       3 
#define V2V_FORMAT_NDX      4  
#define V2OSH_FORMAT_NDX    5 
#define OSH2V_FORMAT_NDX    6
#define SCALED_IMAGE1_NDX   7
#define SCALED_IMAGE2_NDX   8
#define DMABLT_TO_VID_NDX   9
#define DVDPICT_NDX        10


// Note: The 3D class object is used for "advanced" blts that are not possible
//  using the normal bltter.  Surfaces are treated as textures and texture ops
//  are applied to get the desired results.
//
// TBD: turn this into a proper class (if we can get away from C enough) - @mjl@
//
typedef struct Vpp_s
{
    NvU32            dwFlags; // General flags detailing state of the VPP object (see VPP_FLAG_*)

	NvU32            m_pContextDMABase;	// Address of a context DMA memory for VPP use

    DWORD            dwOverlayEventsAllocated;        // used to keep track of overlay event allocation

    GLOBALDATA      *pDriverData;           // Hope to eliminate this some day. -@mjl@

    PDEV            *m_ppdev;              // Hope to eliminate this some day. -@mjl@

    CPushBuffer     *pPusher;              // A cmd pusher class attached to a DMA push buffer
    NV_SystemInfo_t *pSysInfo;             // System Information
    NvU32           *pThreeDClassLastUser; // Last user of the 3d superclass (kelvin,celsius,...)

    CRegTool         regTool;              // A tool to access the system registry

    // Handles to objects created outside of the VPP
    NvU32            hImageBlackRect;
    NvU32            hContextPattern;
    NvU32            hContextRop;
    NvU32            hContextColorKey;
    NvU32            hFloatingContextDmaInOverlayShadow;
    NvU32            hVideoMemUtoVideoMemFormat;        // U-conversion object
    NvU32            hVideoMemVtoVideoMemFormat;        // V-conversion object
    NvU32            hVideoMemUVtoVideoMemFormat;       // UV-conversion object
    NvU32            hLutCursorDacBase;                 // Base object ID for DACs (1 per head)
    NvU32            hMiscEventNotifier;                // Used as the notifier in many cases
    NvU32            hContextSurfaceSwizzled;
    NvU32            hContextBeta4;
    NvU32            hSurfaces2D;
    NvU32            hChannel;
    NvU32            hThreeDClass;        // Unique object ID for the 3D object to be used
    NvU32            hInVideoMemContextDma;      // DMA context for V2V 
    NvU32            hFromVideoMemContextDma;    // DMA context for V2S
    NvU32            hToVideoMemContextDma;      // DMA context for S2V
    NvU32            hContextSurfacesARGB_ZS;
    NvU32            hDX6MultiTextureTriangle;

    // Notifiers created outside of the VPP

    NvNotification  *pFlipPrimaryNotifier;
    NvNotification  *pPusherSyncNotifier;

    // Sub-channel definitions

    NvU32            ropRectTextSubCh;
    NvU32            ropSubCh;
    NvU32            spareSubCh;          // General use subchannel (SetObject always done first)
    NvU32            surfaces2DSubCh;       // May be able to just use spare? -@mjl@
    NvU32            threeDClassSubCh;    // Subchannel that _always_ holds the 3D class object

    // TBD:hMiscEventNotifier was using NV_DD_DMA_COLOUR_CONTROL_EVENT_NOTIFIER which IS NV_DD_PIO_COLOUR_CONTROL_EVENT_NOTIFIER -@mjl@

    NvU32            dwOverlayFSNumSurfaces;
    NvU32            dwOverlayFSHead;    //what head is overlay on over is head on
    NvU32            dwOverlayFSOvlHead; // where mirror is
    NvU32            dwOverlayFSOvlHeadSaved;
    NvU32            dwOverlayFSOvlLost;

    NvU32            dwOverlayFSWidth;
    NvU32            dwOverlayFSHeight;
    NvU32            dwOverlayFSPitch;
    NvU32            dwOverlayFSFormat;
    NvU32            dwOverlayFSOffset[NV_VPP_MAX_EXTRA_FS_SURFACES];

    NvU32            dwOverlayFSIndex;
    NvU32            dwOverlayFSDeltaX;
    NvU32            dwOverlayFSDeltaY;

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

    NvU32                       dwDecoderIdentity;
    NvU32                       dwDecoderCaps;

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

    NvU32                dwNVOverlayFlags;

    VppObjectRecord_t    m_obj_a[VPP_OBJECT_COUNT];
} Vpp_t;

#define KELVIN_Z_SCALE24                16777215.0f  // 2^24 - 1

#define DEFAULT_KELVIN_ALPHA_OCW   (NV097_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT << 15) |        \
                                   (NV097_SET_COMBINER_ALPHA_OCW_MUX_ENABLE_FALSE << 14) |         \
                                   (NV097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_C << 8) |             \
                                   (NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0 << 4) |              \
                                    NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0

#define DEFAULT_KELVIN_COLOR_OCW (NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_DISABLE << 19) |  \
                                 (NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_DISABLE << 18) |  \
                                 (NV097_SET_COMBINER_COLOR_OCW_OP_NOSHIFT << 15) |              \
                                 (NV097_SET_COMBINER_COLOR_OCW_MUX_ENABLE_FALSE << 14) |        \
                                 (NV097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |     \
                                 (NV097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |     \
                                 (NV097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_C << 8) |            \
                                 (NV097_SET_COMBINER_COLOR_OCW_AB_DST_REG_0 << 4) |             \
                                  NV097_SET_COMBINER_COLOR_OCW_CD_DST_REG_0

#define DEFAULT_KELVIN_ALPHA_ICW   (NV097_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |  \
                                   (NV097_SET_COMBINER_ALPHA_ICW_A_ALPHA_TRUE << 28) |             \
                                   (NV097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4 << 24) |           \
                                                                                                   \
                                   (NV097_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_INVERT << 21) |    \
                                   (NV097_SET_COMBINER_ALPHA_ICW_B_ALPHA_TRUE << 20) |             \
                                   (NV097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_0 << 16) |           \
                                                                                                   \
                                   (NV097_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |  \
                                   (NV097_SET_COMBINER_ALPHA_ICW_C_ALPHA_TRUE << 12) |             \
                                   (NV097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_0 << 8) |            \
                                                                                                   \
                                   (NV097_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |   \
                                   (NV097_SET_COMBINER_ALPHA_ICW_D_ALPHA_TRUE << 4) |              \
                                    NV097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_0

#define DEFAULT_KELVIN_COLOR_ICW   (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |  \
                                   (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |            \
                                   (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4 << 24) |           \
                                                                                                   \
                                   (NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_INVERT << 21) |    \
                                   (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |            \
                                   (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_0 << 16) |           \
                                                                                                   \
                                   (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |  \
                                   (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |            \
                                   (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0 << 8) |            \
                                                                                                   \
                                   (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |   \
                                   (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |             \
                                    NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0

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

// TBD: add CELSIUS to the following - @mjl@
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

#define NV_MAX_EXTRA_FS_SURFACES 4

// Flags for vpp.dwFlags field
#define VPP_FLAG_CONSTRUCTED    0x00000001 // VPP class has been initialized
#define VPP_FLAG_ENABLED        0x00000002 // Resources have been assigned and VPP is ready to start
#define VPP_FLAG_OVERLAY_READY  0x00000004 // VPP is ready to run the overlay
#define VPP_FLAG_MIRROR_READY   0x00000008 // VPP is ready to run the full screen mirror
#define VPP_FLAG_CELSIUS_3D     0x00000010 // VPP is using a celsius class as it's 3D object
#define VPP_FLAG_KELVIN_3D      0x00000020 // VPP is using a kelvin class as it's 3D object


// Values for the status field of the m_obj_a[] array
//
#define VPP_OBJ_NA        0	// Ignore this entry.  Object is not applicable.
#define VPP_OBJ_FREED     1 // Needs to be allocated before next use
#define VPP_OBJ_ALLOCATED 2 // Has been allocated
#define VPP_OBJ_ERROR     3 // An error occured during allocation

// dwNVOverlayFlags values
#define OVERLAY_FLAG_ON_PANEL        0x00000001

// Using polling notifiers ALWAYS on an IKOS or NT4 build
//    events do not work properly on IKOS and are not available on NT4
//
//#if 1
#if defined(IKOS) || IS_WINNT4
#undef VPP_USE_EVENT_NOTIFIERS   // use event notifiers if this is defined, else use polling
#else
#define VPP_USE_EVENT_NOTIFIERS  // Win2k, Win9x...
#endif


// Note: The following assumes that all the WRITE_ONLY and WRITE_THEN_AWAKEN values are the
//    same for all objects. (NV039, NV056...) this IS the case.
//
#ifdef VPP_USE_EVENT_NOTIFIERS
#define VPP_NOTIFY_TYPE (NV039_BUFFER_NOTIFY_WRITE_THEN_AWAKEN)
#else
#define VPP_NOTIFY_TYPE (NV039_BUFFER_NOTIFY_WRITE_ONLY)
#pragma message ("VPP will not use events.")
#endif

#endif _NVVPP_H_

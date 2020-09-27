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
 *
 *  File:       nvVPP.cpp
 *  Content:    Shared Video Post Processing Code
 *
 *
 *  VPP pipe for NV4/5:
 *
 *                                           /-----\   /-------\
 *  YUV12           YUV422                   |Scale|   |Colour |
 *    |  /---------\  |                  +-->|Down |-->|Control|--+
 *    +->|Convert  |  |   /----------\   |   \-----/   \-------/  |   /--------\   /----\   /-------\
 *    +->|to YUV422|--+-->|Subpicture|-->+                        +-->|Temporal|-->|Flip|-->|Overlay|
 *    |  \---------/      \----------/   |   /-------\   /-----\  |   |Filter  |   \----/   |Scale  |
 *  YUV9                                 +-->|Colour |-->|Scale|--+   \--------/            \-------/
 *                                           |Control|   |Up   |
 *                                           \-------/   \-----/
 *
 *
 *  VPP pipe for NV10/NV20:
 *
 *  YUV12            YUV422
 *    |  /---------\   |                                                                                     /-------\
 *    +->|Convert  |   |    /----------\   /-----------\   /--------\      /--------\   /----\   /-------\   |Overlay|
 *    +->|to YUV422|---+--->|Subpicture|-->|DeInterlace|-->|Temporal|--+-->|Prescale|-->|Flip|-->|Overlay|-->|Colour |
 *    |  \---------/        \----------/   |Filter     |   |Filter  |  |   \--------/   \----/   |Scale  |   |Control|
 *  YUV9                                   \-----------/   \--------/  |                         \-------/   \-------/
 *                                                                     |
 *                                                                     v
 *                                                                /---------\   /-------\
 *                                                                |FS Mirror|-->|FS Flip|
 *                                                                \---------/   \-------/
 *
 * Initialization/Use Sequence:
 *
 *     VppConstructor()          - Done @ DriverInit.  A few driver escapes to the VPP are safe (but no 4cc blts etc)
 *                                 Allocates DMA contexts.
 *        VppEnable()            - Done @ DirectDraw Enable.  4CC blts are okay using the driver escapes.
 *                                 Allocates object, sets up pointers for access to pusher etc.
 *           VppCreateOverlay()  - Done @ Overlay creation time.  Actually creates the overlay object
 * 
 *           VppDestroyOverlay() - Done when overlay surface is destroyed
 *
 *           [VppCreateFSMirror VPPDestroyFSMirror] - These are at this level as well
 *
 *        VppDisable()           - Done when DirectDraw is disabled (objects are freed, channel ptrs are cleared)
 *
 *     VppDestructor()           - Done @ Driver Exit.  DMA contexts are freed.
 *
 *  Extra surfaces:
 *      In addition to the overlay surfaces that the application creates, 4 extra
 *      (or 3 optimized) work surfaces are created for VPP usage.  The original
 *      data in the overlay surface is never modified.  Each run through the pipe
 *      uses 2 of the extra surfaces (the other two hold video data which is
 *      being displayed or pre-swizzled) and each stage alternates working on
 *      one surface to the other, i.e. the output of one stage is the input for
 *      the next.  On each flip, the 2 working surfaces are swapped with the 2
 *      holding surfaces.
 *
 *  Event protocol
 *      Each stage must generate a unique event (vppSubPicture and vppPrescale are
 *      excepted).  VppDoFlip waits for last event to complete before flipping.
 *      Note that because we can only have one DVD_SUBPICTURE per channel, we must
 *      wait until the previous instance completes before using it again.
 *
 *  Decision making:
 *      Gross level stage enable decisions are made in VppDoFlip.  More complex
 *      decision making which may require computation is made within the stage.  Each
 *      stage returns TRUE if it succeeds, FALSE if not.  If the stage returns FALSE,
 *      it must do nothing so that video can still proceed though the pipe.
 *
 *  State changes:
 *      Each stage is responsible for changing the surface parameters if a stage
 *      modifies them.  This can either be done within the stage, or in VppDoFlip
 *      upon successful completion of the stage.  E.g. the convert stage changes
 *      the fourcc code and the prescaler changes the width and height.
 *
 *  Stage enables and disables:
 *      There can be up to 3 levels of stage enables:
 *      1) Most (but not all) stages have registry enables
 *      2) Each stage is selectively enabled by the caller of VppDoFlip
 *      3) The enable flags passed by the caller are masked by an inverse mask set in the registry
 *      The final enable is determined by ((2 & ~3) && 1)
 *
 ***************************************************************************/

// For 9x and NT5, nvprecomp.h comes from the ddraw common area,
// for NT4, it comes from drivers/display/winnt4/ddraw/nv4/nvprecomp.h
#include "nvprecomp.h"
#include "wincommon.h"

#include "nvVPP.h"

// We define this as a null label to use instead of 'static' to indicate to the reader that a function
// is intended for use only within this file.  If we define the functions as static, we get reports off
// faults from the last exported function instead of the function we should be looking at.
#define LOCAL

#if (IS_WINNT4 || IS_WINNT5)
#undef GET_MODE_BPP
#define GET_MODE_BPP()       (vpp.m_ppdev->cBitsPerPel)
#define LOGICAL_HEAD_ID()    (vpp.m_ppdev->ulDeviceDisplay[0])
#define DRIVER_HANDLE()      (vpp.m_ppdev->hDriver)
#else
#define LOGICAL_HEAD_ID()    (pDXShare->dwHeadNumber)
#define DRIVER_HANDLE()      (0)
#endif

#if (IS_WINNT5)
extern "C" BOOL __cdecl NvSetDac(PDEV *ppdev, ULONG ulHead, RESOLUTION_INFO * pResolution);
extern "C" void __cdecl DisableHeadCursor(PDEV *ppdev, ULONG ulHead, ULONG ulDiable);
extern "C" void __cdecl NVMoveDisplayWindow(PDEV* ppdev, ULONG ulHead, RECTL * prclCurrentDisplay);
extern "C" void __cdecl NvSetDacImageOffset(PDEV* ppdev, ULONG ulDac, ULONG ulOffset);
#endif

// *********************************************************************
// Local Prototypes
// *********************************************************************
LOCAL void vppFreeOverlay(Vpp_t *pVpp);
LOCAL NvU8 vppAllocOverlay(Vpp_t *pVpp);
LOCAL void vppChooseOverlayHead(Vpp_t *pVpp);
LOCAL BOOL vppAllocateObjects(Vpp_t *pVpp);
LOCAL void vppFreeObjects(Vpp_t *pVpp);
LOCAL void vppInitObjects(Vpp_t *pVpp);
LOCAL BOOL vppAllocateContextDMAs(Vpp_t *pVpp);
LOCAL void vppFreeContextDMAs(Vpp_t *pVpp);
LOCAL BOOL vppInitCelsius(Vpp_t *pVpp);
LOCAL BOOL vppInitKelvin(Vpp_t *pVpp);
LOCAL BOOL vppValidateMode(Vpp_t *pVpp, DWORD dwWidth, DWORD dwHeight, DWORD dwBytesPerPel,
                            DWORD dwNumSurfaces, DWORD dwHead, DWORD dwRefreshRate, DWORD dwDCHead, DWORD dwDeviceType);
LOCAL BOOL vppPreCopy(Vpp_t *pVpp, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwDstOffset,
                       DWORD dwDstPitch, DWORD dwWidth, DWORD dwHeight);
LOCAL BOOL vppConvert(Vpp_t *pVpp, LPPROCESSINFO lpProcInfo, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwDstOffset,
                       DWORD dwDstPitch, DWORD dwWidth, DWORD dwHeight, DWORD dwFourCC);
LOCAL BOOL vppSubPicture(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwDstOffset,
                          DWORD dwDstPitch, DWORD dwWidth, DWORD dwHeight, DWORD dwFourCC);
LOCAL BOOL vppPreScale(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwDstOffset, DWORD dwDstPitch,
                        LPDWORD pWidth, LPDWORD pHeight, DWORD dwFourCC, LPDWORD dwField);
LOCAL BOOL vppFSMirror(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwWidth, DWORD dwHeight,
                        DWORD dwFourCC, DWORD dwFlags, DWORD dwPrescaleFactorX, DWORD dwPrescaleFactorY);
LOCAL BOOL vppColourControl(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwDstOffset, DWORD dwDstPitch,
                             DWORD dwWidth, DWORD dwHeight, DWORD dwFourCC, DWORD dwField);
LOCAL BOOL vppNv20Deinterlace(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwDstOffset,
                               DWORD dwDstPitch, DWORD dwWidth, DWORD dwHeight, DWORD dwFlags);
LOCAL BOOL vppNv20Temporal(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwDstOffset,
                            DWORD dwDstPitch, DWORD dwWidth, DWORD dwHeight, DWORD dwFlags, LPDWORD pPrevFrameOffset);
LOCAL BOOL vppNv10Deinterlace(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwDstOffset,
                               DWORD dwDstPitch, DWORD dwWidth, DWORD dwHeight, DWORD dwFlags);
LOCAL BOOL vppNv10Temporal(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwDstOffset,
                            DWORD dwDstPitch, DWORD dwWidth, DWORD dwHeight, DWORD dwFlags, LPDWORD pPrevFrameOffset);
LOCAL BOOL vppNv5Temporal(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwSrcOffset, DWORD dwSrcPitch, DWORD dwDstOffset,
                           DWORD dwDstPitch, DWORD dwWidth, DWORD dwHeight, LPDWORD pPrevFrameOffset);
LOCAL BOOL vppFSFlipSync(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwIndex, BOOL bWait);
LOCAL BOOL vppFSFlip(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwIndex);
LOCAL BOOL vppFlipSync(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwIndex, BOOL bWait);
LOCAL BOOL vppFlip(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo,
                    DWORD dwOffset, DWORD dwPitch, DWORD dwWidth, DWORD dwHeight,
                    DWORD dwIndex, DWORD dwFourCC, DWORD dwFlags,
                    DWORD dwPrescaleFactorX, DWORD dwPrescaleFactorY);
LOCAL void vppBltFillBlock(Vpp_t *pVpp, NvU32 dwColor, NvU32 dwOffset, NvU32 dwPitch, NvU32 dwWidth, NvU32 dwHeight, NvU32 dwBytesPerPel);
LOCAL void vppBltWriteDword(Vpp_t *pVpp, NvU32 dwOffset, NvU32 dwIndex, NvU32 dwData);
LOCAL void vppBltUpdateClip(Vpp_t *pVpp);

extern NvU8 VppIsConstructed(Vpp_t *pVpp)
{
    nvAssert(pVpp);
    return ((pVpp->dwFlags & VPP_FLAG_CONSTRUCTED) != 0 );
}
extern NvU8 VppIsOverlayActive(Vpp_t *pVpp)
{
    nvAssert(pVpp);
    return ((pVpp->dwFlags & VPP_FLAG_OVERLAY_READY) != 0 );
}

// These substitute for the real constructor/destructor that we'd have in C++
static VppObjectRecord_t VppDefaultObjects[VPP_OBJECT_COUNT] =
{
    {VPP_OBJ_FREED, NV10_VIDEO_OVERLAY,            NV_VPP_OVERLAY_IID,         NV_VPP_OVERLAY_CONTEXT_IID,       3, NULL}, // May be NV4_OVERLAY
    {VPP_OBJ_NA,    NV04_SCALED_IMAGE_FROM_MEMORY, NV_VPP_SWIZZLE_BLIT_IID,    NV_VPP_SWIZZLE_BLIT_CONTEXT_IID,  1, NULL}, // Used on NV4/NV5 only
    {VPP_OBJ_NA,    NV01_TIMER,                    NV_VPP_TIMER_IID,           NV_VPP_TIMER_CONTEXT_IID,         2, NULL},
    {VPP_OBJ_FREED, NV04_IMAGE_BLIT,               NV_VPP_ALPHA_BLIT_IID,      NV_VPP_ALPHA_BLIT_CONTEXT_IID,    1, NULL},
    {VPP_OBJ_FREED, NV03_MEMORY_TO_MEMORY_FORMAT,  NV_VPP_V2V_FORMAT_Y_IID,    NV_VPP_V2V_FORMAT_Y_CONTEXT_IID,  2, NULL},
    {VPP_OBJ_FREED, NV03_MEMORY_TO_MEMORY_FORMAT,  NV_VPP_V2OSH_FORMAT_IID,    NV_VPP_V2OSH_FORMAT_CONTEXT_IID,  2, NULL},
    {VPP_OBJ_FREED, NV03_MEMORY_TO_MEMORY_FORMAT,  NV_VPP_OSH2V_FORMAT_IID,    NV_VPP_OSH2V_FORMAT_CONTEXT_IID,  2, NULL},
    {VPP_OBJ_FREED, NV10_SCALED_IMAGE_FROM_MEMORY, NV_VPP_SCALED_IMAGE1_IID,   NV_VPP_SCALED_IMAGE1_CONTEXT_IID, 1, NULL}, // will be NV4, NV5 or NV10
    {VPP_OBJ_FREED, NV10_SCALED_IMAGE_FROM_MEMORY, NV_VPP_SCALED_IMAGE2_IID,   NV_VPP_SCALED_IMAGE2_CONTEXT_IID, 1, NULL}, // will be NV4, NV5 or NV10
    {VPP_OBJ_FREED, NV03_MEMORY_TO_MEMORY_FORMAT,  NV_VPP_DMABLT_TO_VID_IID,   NV_VPP_DMABLT_TO_VID_CONTEXT_IID, 2, NULL},
    {VPP_OBJ_FREED, NV04_DVD_SUBPICTURE,           NV_VPP_DVD_SUBPICTURE_IID,  NV_VPP_DVD_SUBPICTURE_CONTEXT_IID,1, NULL}, // may upgrade to NV10

};

#ifdef ENABLE_VPP_DISPATCH_CODE
// This code is no longer used but may someday be useful.

//---------------------------------------------------------------------------
// vppDispatcher
//      Remote dispatcher for VppDoFlip
#define PDRIVERDATA     ((GLOBALDATA *)pData)
#define TIME_INFINITY   0x7FFFFFFF
LOCAL DWORD WINAPI vppDispatcher(Vpp_t *pVpp,LPVOID pData)
{
    // create signal
#if IS_WINNT5
    EngCreateEvent(&(vppDispatch.hSignalGo0));
    vppDispatch.hSignalGo3 = vppDispatch.hSignalGo0;
    EngCreateEvent(&(vppDispatch.hSignalDone0));
    vppDispatch.hSignalDone3 = vppDispatch.hSignalDone0;
#elif IS_WINNT4
    // Do something for NT4
#elif IS_WIN9X
    vppDispatch.hSignalGo3 = CreateEvent(NULL, TRUE, FALSE, EVENTNAME_DISPATCH_GO);
    vppDispatch.hSignalGo0 = ConvertRing3EventToRing0(vppDispatch.hSignalGo3);
    vppDispatch.hSignalDone3 = CreateEvent(NULL, FALSE, FALSE, EVENTNAME_DISPATCH_DONE);
    vppDispatch.hSignalDone0 = ConvertRing3EventToRing0(vppDispatch.hSignalDone3);
#endif
    if (vppDispatch.hSignalGo3 == NULL ||
        vppDispatch.hSignalGo0 == NULL ||
        vppDispatch.hSignalDone3 == NULL ||
        vppDispatch.hSignalDone0 == NULL) {

        vppDispatch.dwState |= VPPDSTATE_INITERROR | VPPDSTATE_TERMINATED;
        CLOSE_HANDLE(vppDispatch.hSignalGo3);
        CLOSE_HANDLE(vppDispatch.hSignalDone3);
        vppDispatch.hSignalGo3 = NULL;
        vppDispatch.hSignalGo0 = NULL;
        vppDispatch.hSignalDone3 = NULL;
        vppDispatch.hSignalDone0 = NULL;
        return -1;
    }

    vppDispatch.procInfo.hProcess = GetCurrentProcess();
    vppDispatch.procInfo.dwProcessId = nvGetCurrentProcessId();
    vppDispatch.dwOriginalThreadPriority = GetThreadPriority(GetCurrentThread());
    vppDispatch.dwOriginalProcessPriority = GetPriorityClass(GetCurrentProcess());

    vppDispatch.dwState &= ~VPPDSTATE_CMDERROR;
    vppDispatch.dwState &= ~VPPDSTATE_INITERROR;
    vppDispatch.dwState &= ~VPPDSTATE_TERMINATED;

    // explicitly reset these events, windows does not seem to set initial state properly
    VppResetNotification(NULL,vppDispatch.hSignalGo3);
    VppResetNotification(NULL,vppDispatch.hSignalDone3);

    while (vppDispatch.dwCommand != VPPDISPATCH_EXIT) {
        vppDispatch.dwState &= ~VPPDSTATE_BUSY;
        vppDispatch.dwState |= VPPDSTATE_WAITING;

        nvWaitEvent(vppDispatch.hSignalGo3, TIME_INFINITY);

        vppDispatch.dwState &= ~VPPDSTATE_WAITING;
        vppDispatch.dwState |= VPPDSTATE_BUSY;

        switch (vppDispatch.dwCommand) {
        case VPPDISPATCH_NOOP:
        case VPPDISPATCH_EXIT:
            break;

        case VPPDISPATCH_DISPATCH:
            vppDispatch.dwReturnCode = VppDoFlip(vppDispatch.dwOffset,
                                                                  vppDispatch.dwPitch,
                                                                  vppDispatch.dwWidth,
                                                                  vppDispatch.dwHeight,
                                                                  vppDispatch.dwFourCC,
                                                                  vppDispatch.dwFlags);
            vppDispatch.dwCommand = VPPDISPATCH_NOOP;
            break;

        case VPPDISPATCH_SETPRIORITY:
            SetThreadPriority(GetCurrentThread(), vppDispatch.dwThreadPriority);
            SetPriorityClass(GetCurrentProcess(), vppDispatch.dwProcessPriority);
            vppDispatch.dwCommand = VPPDISPATCH_NOOP;
            break;

        case VPPDISPATCH_RESTOREPRIORITY:
            SetThreadPriority(GetCurrentThread(), vppDispatch.dwOriginalThreadPriority);
            SetPriorityClass(GetCurrentProcess(), vppDispatch.dwOriginalProcessPriority);
            vppDispatch.dwCommand = VPPDISPATCH_NOOP;
            break;

        default:
            vppDispatch.dwState |= VPPDSTATE_CMDERROR;
            vppDispatch.dwCommand = VPPDISPATCH_NOOP;
            break;
        }

        VppResetNotification(NULL,vppDispatch.hSignalGo3);
        setEvent(NULL,vppDispatch.hSignalDone3);
    }

    SetThreadPriority(GetCurrentThread(), vppDispatch.dwOriginalThreadPriority);
    SetPriorityClass(GetCurrentProcess(), vppDispatch.dwOriginalProcessPriority);

    vppDispatch.dwState |= VPPDSTATE_TERMINATED;

    // destroy signal
    CLOSE_HANDLE((HANDLE)vppDispatch.hSignalGo3);
    vppDispatch.hSignalGo3 = NULL;
    vppDispatch.hSignalGo0 = NULL;
    CLOSE_HANDLE((HANDLE)vppDispatch.hSignalDone3);
    vppDispatch.hSignalDone3 = NULL;
    vppDispatch.hSignalDone0 = NULL;
    return 0;
}

//---------------------------------------------------------------------------
// vppInitDispatcher
//      Initializes vpp dispatcher, should be called from DriverInit
LOCAL BOOL vppInitDispatcher(Vpp_t *pVpp)
{
    vppDestroyDispatcher();

    vppDispatch.dwState = 0;
    vppDispatch.dwCommand = VPPDISPATCH_NOOP;

    vppDispatch.procInfo.hThread = CreateThread(NULL, 4096, vppDispatcher, pDriverData, 0, &vppDispatch.procInfo.dwThreadId);
    if (vppDispatch.procInfo.hThread == NULL) {
        vppDestroyDispatcher();
        return FALSE;
    }

    // may as well init some subpicture vars while we are here
    vpp.subPicture.originX = 0xFFFFFFFF;
    vpp.subPicture.originY = 0xFFFFFFFF;
    vpp.subPicture.width = 0xFFFFFFFF;
    vpp.subPicture.height = 0xFFFFFFFF;
    return TRUE;
}


//---------------------------------------------------------------------------
// vppDestroyDispatcher
//      Destroys vpp dispatcher, should be called from DestroyDriver
LOCAL void vppDestroyDispatcher(Vpp_t *pVpp)
{
    if (vppDispatch.procInfo.hThread) {
        HDRVEVENT vppSignalGo, vppSignalDone;

#if IS_WINNT5
        vppSignalGo   = vppDispatch.hSignalGo0;
        vppSignalDone = vppDispatch.hSignalDone0;
#elif IS_WINNT4
        // Do something here
#elif IS_WIN9X
        vppSignalGo   = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_DISPATCH_GO);
        vppSignalDone = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_DISPATCH_DONE);
#endif
        vppDispatch.dwCommand = VPPDISPATCH_EXIT;
        NvSetEvent(vppSignalGo);
        VppWaitForNotification(NULL, vppSignalDone, 1000);
    }

    vppDispatch.dwState = VPPDSTATE_TERMINATED;
    vppDispatch.dwCommand = VPPDISPATCH_EXIT;
    vppDispatch.dwThreadPriority = THREAD_PRIORITY_IDLE;
    vppDispatch.dwProcessPriority = IDLE_PRIORITY_CLASS;
    vppDispatch.dwOriginalThreadPriority = 0;
    vppDispatch.dwOriginalProcessPriority = 0;
    vppDispatch.procInfo.hProcess = 0;
    vppDispatch.procInfo.hThread = 0;
    vppDispatch.procInfo.dwProcessId = 0;
    vppDispatch.procInfo.dwThreadId = 0;
    vppDispatch.hSignalGo0 = NULL;
    vppDispatch.hSignalGo3 = NULL;
    vppDispatch.hSignalDone0 = NULL;
    vppDispatch.hSignalDone3 = NULL;
    vppDispatch.dwOffset = 0;
    vppDispatch.dwPitch = 0;
    vppDispatch.dwWidth = 0;
    vppDispatch.dwHeight = 0;
    vppDispatch.dwFourCC = 0;
    vppDispatch.dwFlags = 0;
}

#endif

//
// VppGetVideoScalerBandwidthStatus
// TBD: this probably needs a better interface @mjl@.
//
extern NvU8 VppGetVideoScalerBandwidthStatus(Vpp_t *pVpp, NvU16 wWidth)
{
    Vpp_t &vpp = *pVpp;
    NvU32 dwGetDownScaleRatioX;
    NvU32 dwAllowed;

    nvAssert(vpp.dwFlags & VPP_FLAG_CONSTRUCTED);

    if (wWidth <= 768) {
        dwGetDownScaleRatioX = NV_CFG_VIDEO_DOWNSCALE_RATIO_768;
    } else if (wWidth <= 1280) {
        dwGetDownScaleRatioX = NV_CFG_VIDEO_DOWNSCALE_RATIO_1280;
    } else {
        dwGetDownScaleRatioX = NV_CFG_VIDEO_DOWNSCALE_RATIO_1920;
    }

    // Get max overlay downscale
    NvRmConfigGet(vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwDeviceHandle, 
        dwGetDownScaleRatioX, &(vpp.dwOverlayMaxDownScale));

    if (vpp.dwOverlayMaxDownScale == 0)
    {
        vpp.dwOverlayMaxDownScale = 1;  // this can be reported as zero on NV4
    }
    vpp.dwOverlayMaxDownScaleX = vpp.dwOverlayMaxDownScale;
    vpp.dwOverlayMaxDownScaleY = vpp.dwOverlayMaxDownScale;

    // Ask the resource manager if a video scaler is allowed within the current
    // resolution, given the current clocks and bandwidth limitations
    NvRmConfigGet(vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwDeviceHandle,
                   NV_CFG_VIDEO_OVERLAY_ALLOWED, &(dwAllowed));

    return (dwAllowed ? TRUE : FALSE);
}

#ifdef WINNT
extern NvU8 VppConstructor(PDEV *ppdev, Vpp_t *pVpp, NV_SystemInfo_t *pSysInfo)
#else
extern NvU8 VppConstructor(GLOBALDATA *pDriverData, Vpp_t *pVpp, NV_SystemInfo_t *pSysInfo)
#endif
{
    Vpp_t &vpp = *pVpp;
    DWORD dwArch;
#ifdef WINNT
    GLOBALDATA *pDriverData = ppdev->pDriverData;
#endif

    nvAssert(pVpp);
    nvAssert(pSysInfo);

    if (VppIsConstructed(pVpp))
    {
        dbgError("VPP: VppConstructor being called again -- before destructor\n");
        return TRUE;
    }

    // Much up everything so that we can easily see uninitialized data
    memset(pVpp, 0,sizeof(*pVpp));

    vpp.pDriverData = pDriverData;  // Hopefully this will someday go away

#ifdef WINNT
    vpp.m_ppdev = ppdev;            // Hopefully this will someday go away
#else
    vpp.m_ppdev = NULL;             // Hopefully this will someday go away
#endif

    vpp.pSysInfo = pSysInfo;    // This is used early in nvpriv routines (by control panel) so it MUST
                                // be set early in VppConstructor (@ Driver Init time)
    vpp.dwDecoderIdentity = 0;
    vpp.dwDecoderCaps = 0;
    
    vpp.dwFlags = 0;
    vpp.m_pContextDMABase = 0;
    vpp.dwOverlayEventsAllocated = FALSE;
    vpp.pPusher = NULL;
    vpp.pThreeDClassLastUser = NULL;

    vpp.hImageBlackRect = NULL;
    vpp.hContextPattern = NULL;
    vpp.hContextRop = NULL;
    vpp.hContextColorKey = NULL;
    vpp.hFloatingContextDmaInOverlayShadow = NULL;
    vpp.hVideoMemUtoVideoMemFormat = NULL;
    vpp.hVideoMemVtoVideoMemFormat = NULL;
    vpp.hVideoMemUVtoVideoMemFormat = NULL;
    vpp.hLutCursorDacBase = NULL;
    vpp.hMiscEventNotifier = NULL;
    vpp.hContextSurfaceSwizzled = NULL;
    vpp.hContextBeta4 = NULL;
    vpp.hSurfaces2D = NULL;
    vpp.hChannel = NULL;
    vpp.hThreeDClass = NULL;
    vpp.hInVideoMemContextDma = NULL;
    vpp.hFromVideoMemContextDma = NULL;
    vpp.hToVideoMemContextDma = NULL;
    vpp.hThreeDClass = NULL;
    vpp.hContextSurfacesARGB_ZS = NULL;
    vpp.hDX6MultiTextureTriangle = NULL;

    vpp.pFlipPrimaryNotifier = NULL;
    vpp.pPusherSyncNotifier = NULL;

    vpp.ropRectTextSubCh = 0xffffffff;
    vpp.ropSubCh = 0xffffffff;
    vpp.spareSubCh = 0xffffffff;
    vpp.surfaces2DSubCh = 0xffffffff;
    vpp.threeDClassSubCh = 0xffffffff;

    vpp.dwOverlayFSNumSurfaces = 0;
    vpp.dwOverlayFSOvlLost = FALSE;

    vpp.dwOverlayFSWidth     = 0;
    vpp.dwOverlayFSHeight    = 0;
    vpp.dwOverlayFSPitch     = 0;
    vpp.dwOverlayFSFormat    = 0;
    vpp.dwOverlayFSIndex     = 0;
    vpp.dwOverlayFSDeltaX    = 0;
    vpp.dwOverlayFSDeltaY    = 0;

    memset(&(vpp.dwOverlayFSOffset),0,sizeof(vpp.dwOverlayFSOffset));  // Init arrays to zero
    memset(&(vpp.extraOverlayOffset),0,sizeof(vpp.extraOverlayOffset));
    memset(&(vpp.dwOverlaySurfaceLCL),0,sizeof(vpp.dwOverlaySurfaceLCL));

    vpp.extraPitch       = 0;
    vpp.extraNumSurfaces = 0;
    vpp.extraIndex       = 0;

    vpp.dwOverlaySrcWidth  = 0;
    vpp.dwOverlaySrcHeight = 0;
    vpp.dwOverlayDstWidth  = 0;
    vpp.dwOverlayDstHeight = 0;
    vpp.dwOverlayDeltaX    = 0;
    vpp.dwOverlayDeltaY    = 0;

    vpp.dwOverlaySrcX      = 0;
    vpp.dwOverlaySrcY      = 0;
    vpp.dwOverlayDstX      = 0;
    vpp.dwOverlayDstY      = 0;
    vpp.dwOverlaySrcOffset = 0;
    vpp.dwOverlayFormat    = 0;
    vpp.dwOverlaySrcPitch  = 0;

    vpp.dwOverlayLastVisibleFpVidMem    = 0xffffffff;
    vpp.dwOverlayLastVisiblePitch       = 0xffffffff;
    vpp.dwOverlayLastVisiblePixelFormat = 0xffffffff;
    vpp.dwOverlayFlipCount              = 0;
    vpp.overlayBufferIndex              = 0;
    vpp.overlayRelaxOwnerCheck          = FALSE;
    vpp.dwOverlaySrcSize                = 0;
    vpp.dwOverlayColorKey               = 0;
    vpp.dwOverlaySrcPitch               = 0;
    vpp.dwOverlayCachedFlags            = 0;
    vpp.dwOverlayFSSrcWidth             = 0;
    vpp.dwOverlayFSSrcHeight            = 0;
    vpp.dwOverlayFSSrcMinX              = 0;
    vpp.dwOverlayFSSrcMinY              = 0;
    vpp.dwOverlayMaxDownScale           = 0;
    vpp.dwOverlayMaxDownScaleX          = 0;
    vpp.dwOverlayMaxDownScaleY          = 0;
    vpp.regRing0ColourCtlInterlockFlags = 0;

    memset(&(vpp.dwOverlayFSTwinData), 0,sizeof(vpp.dwOverlayFSTwinData));

    vpp.dwOverlayMode     = 0;
    vpp.dwPrevFrameOffset = 0;
    vpp.llDeliveryPeriod  = 0;
    vpp.llDeliveryTime    = 0;
    vpp.llDeliveryStart   = 0;

    memset(&(vpp.subPicture),0x0,sizeof(vpp.subPicture));
    memset(&(vpp.pipeState),0x0,sizeof(vpp.pipeState));
    memset(&(vpp.snoop),0x0,sizeof(vpp.snoop));

    // Initialize overlay alignment.

    if (NvRmConfigGet(vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwDeviceHandle, NV_CFG_ARCHITECTURE, &dwArch) != NVOS_CGE_STATUS_SUCCESS)
    {
        dbgError("Failed to query arch from RM in VPP constructor!");
        return FALSE;
	}

    if (dwArch >= 5) { // NV10
        vpp.dwOverlayByteAlignmentPad = NV_OVERLAY_BYTE_ALIGNMENT_PAD;
    }
    else {
        vpp.dwOverlayByteAlignmentPad = vpp.pDriverData->dwSurfaceAlignPad;
    }

    vpp.dwOverlaySurfaces              = 0;
    vpp.dwOverlayLastVisibleSurfaceLCL = 0;
    vpp.dwOverlayFSOvlHead             = 0xFFFFFFFF;
    vpp.dwOverlayFSOvlHeadSaved        = 0xFFFFFFFF;

    vpp.colorCtrl.dwSize = sizeof(vpp.colorCtrl);
    vpp.colorCtrl.dwFlags = NV_VPP_COLOR_BRIGHTNESS |
                            NV_VPP_COLOR_CONTRAST   |
                            NV_VPP_COLOR_SATURATION |
                            NV_VPP_COLOR_HUE;
    vpp.colorCtrl.lSharpness  = 0;
    vpp.colorCtrl.lHue        = 0;
    vpp.colorCtrl.dwReserved1 = 0;

    // Initialize overlay color control data to default Microsoft values
    if (vpp.pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10)
    {
        vpp.colorCtrl.lBrightness  =    0;
        vpp.colorCtrl.lContrast    =  255;
        vpp.colorCtrl.lHue         =    0;
        vpp.colorCtrl.lSaturation  =  256;
        vpp.colorCtrl.lGamma       =  255;
        vpp.colorCtrl.lColorEnable =    1;
        vpp.subPicture.format      = NV038_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT;
    }
    else
    {
        vpp.colorCtrl.lBrightness  =   750;
        vpp.colorCtrl.lContrast    = 10000;
        vpp.colorCtrl.lSaturation  = 10000;
        vpp.colorCtrl.lGamma       =     0;
        vpp.colorCtrl.lColorEnable =     0;
        vpp.subPicture.format      = NV088_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT;
    }

    vpp.fpOverlayShadow = 0;
    vpp.dwOverlayFSHead = 0xFFFFFFFF;

    // Overlay colour control stuff
    vpp.regOverlayColourControlEnable = 0;
    vpp.regOverlayMode    = 0;
    vpp.regOverlayMode2   = NV4_REG_OVL_MODE2_FSASPECT | NV4_REG_OVL_MODE2_FSTRACKOVLASPECT | NV4_REG_OVL_MODE2_FSSETMODE;
    vpp.regOverlayMode3   = 0;
    vpp.regVPPInvMask     = 0;
    vpp.regVPPMaxSurfaces = NV_VPP_MAX_EXTRA_SURFACES;

    memcpy(vpp.m_obj_a,&VppDefaultObjects,sizeof(VppDefaultObjects));

    if (vpp.pSysInfo->dwNVClasses & NVCLASS_0088_DVDPICT)
    {
        vpp.m_obj_a[DVDPICT_NDX].classNdx = NV10_DVD_SUBPICTURE;
    }
    else
    {
        nvAssert(vpp.pSysInfo->dwNVClasses & NVCLASS_0038_DVDPICT);
    }

    // Select the SCALED_IMAGE class based on what we have (in order of preference)
    if      (vpp.pSysInfo->dwNVClasses & NVCLASS_0089_SCALEDIMG) vpp.m_obj_a[SCALED_IMAGE1_NDX].classNdx = NV10_SCALED_IMAGE_FROM_MEMORY;
    else if (vpp.pSysInfo->dwNVClasses & NVCLASS_0063_SCALEDIMG) vpp.m_obj_a[SCALED_IMAGE1_NDX].classNdx = NV05_SCALED_IMAGE_FROM_MEMORY;
    else if (vpp.pSysInfo->dwNVClasses & NVCLASS_0077_SCALEDIMG) vpp.m_obj_a[SCALED_IMAGE1_NDX].classNdx = NV04_SCALED_IMAGE_FROM_MEMORY;
    vpp.m_obj_a[SCALED_IMAGE2_NDX].classNdx = vpp.m_obj_a[SCALED_IMAGE1_NDX].classNdx;

    // For NV4 & NV5:
    //
    // Can't have more than one surfaces type for each channel, so share NV_DD_SURFACES_2D,
    //  D3D_CONTEXT_SURFACE_SWIZZLED, and vpp.hContextSurfacesARGB_ZS instead of using
    //      objects comment out below
    //   For NV_DD_SURFACES_2D_A8R8G8B8, we only change the pixel depth so be sure to restore that
    //   For NV_DD_SURFACES_3D, call nvSetD3DSurfaceState(pContext) to restore
    //   For NV_DD_SURFACES_SWIZZLED, D3D restores its state on each usage, ok to dirty it
    //
    // @mjl@ the above are probably out of date... TBD

    if (vpp.pSysInfo->dwNVClasses & NVCLASS_FAMILY_KELVIN)
    {
        vpp.dwFlags |= VPP_FLAG_KELVIN_3D;
    }
    else if (vpp.pSysInfo->dwNVClasses & NVCLASS_FAMILY_CELSIUS)
    {
        vpp.dwFlags |= VPP_FLAG_CELSIUS_3D;
    }
    else
    {
        vpp.m_obj_a[OVERLAY_NDX].classNdx = NV04_VIDEO_OVERLAY; // Used NV04 overlay instead of NV10
        vpp.m_obj_a[SWIZZLE_NDX].status   = VPP_OBJ_FREED;      // 'Enables' this object
    }

    // If we have the timer class, turn it on
    if (vpp.pSysInfo->dwNVClasses & NVCLASS_0004_TIMER)
    {
        vpp.m_obj_a[TIMER_NDX].status = VPP_OBJ_FREED;      // 'Enables' this object
    }
    
    if (!vppAllocateContextDMAs(pVpp))
    {
        dbgError("Failed to allocate a context DMA in VPP constructor!");
        return FALSE;
    }

    vpp.dwFlags |= VPP_FLAG_CONSTRUCTED; // Construction is done

	return TRUE;
}

void VppDestructor(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;

    if (!VppIsConstructed(pVpp)) return;    // This can occur under special circumstance

    vppFreeContextDMAs(pVpp);

    if (vpp.fpOverlayShadow) {
        FreeIPM((void*)vpp.fpOverlayShadow);
    }

    // Wreck the data in VPP to make sure we don't make use of it somehow
    //  NB: This implicitly clears VPP_FLAG_CONSTRUCTED
    memset(pVpp, 0,sizeof(*pVpp));
}

//------------------------------------------------------------------------------
// some utility functions to manage switching between using notifiers and events
//
// These are not meant to be called by the general driver, but only the nvPriv
//

extern int VppWaitForNotification(NvNotification* pNotify, HDRVEVENT hEvent, unsigned long timeOut)
{
#ifdef VPP_USE_EVENT_NOTIFIERS
    nvAssert(hEvent);

    if (IS_EVENT_ERROR(NvWaitEvent(hEvent, timeOut)))
    {
        return -1;
    }
#else
    unsigned long count = timeOut;
    nvAssert(pNotify);

#ifdef IKOS
    while(pNotify->status == NV_IN_PROGRESS) // on IKOS we'll ignore timeOut; could be a very long time
#else
    while( (count>0) && (pNotify->status == NV_IN_PROGRESS) )
#endif
    {
        // TBD: find out how VPP_TIMEOUT_TIME was derived.  Use pSysInfo->dwSpinLoopCount instead perhaps @mjl@
        nvSpin(timeOut);
        count--;
    }
#endif
    return 0;
}

extern void VppResetNotification(NvNotification* pNotify, HDRVEVENT hEvent)
{
#ifdef VPP_USE_EVENT_NOTIFIERS
    nvAssert(hEvent);
    NvResetEvent(hEvent);
#else
    nvAssert(pNotify);
    pNotify->status = NV_IN_PROGRESS;
#endif
}

#if (NVARCH >= 0x10)
//---------------------------------------------------------------------------
// vppInitCelsius
//      Inits celsius object for temporal filter usage.  Celsius is a shared,
//      cached object, so these methods only get set when we are a new consumer
LOCAL BOOL vppInitCelsius(Vpp_t *pVpp)
{
    DWORD tmpVal;
    float fBuffer[16];
    unsigned long *ftol = (unsigned long *)&fBuffer[0];
    float xTrans, yTrans, zTrans;
    Vpp_t &vpp = *pVpp;

#if (NVARCH >= 0x20)
    nvAssert (!(vpp.dwFlags & VPP_FLAG_KELVIN_3D));
#endif
    nvAssert(vpp.dwFlags & VPP_FLAG_CELSIUS_3D);

    nvAssert(vpp.pThreeDClassLastUser);
    *vpp.pThreeDClassLastUser = MODULE_ID_DDRAW_VPP;

    // Let D3D code know that we have touched NV
    vpp.pDriverData->TwoDRenderingOccurred = 1;

    // Set object
    vpp.pPusher->setObject(vpp.threeDClassSubCh,vpp.hThreeDClass);

    if (vpp.dwFlags & VPP_FLAG_CELSIUS_3D)
    {
        NvU32 subchannel = SUB_CHANNEL(vpp.threeDClassSubCh);

        // Reset the celsius context Dmas to use the default video context Dma
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_CONTEXT_DMA_A | 0x40000);
        vpp.pPusher->push(1, vpp.hInVideoMemContextDma);

        vpp.pPusher->push(2, subchannel +
                      NV056_SET_CONTEXT_DMA_B | 0x40000);
        vpp.pPusher->push(3, vpp.hInVideoMemContextDma);

        vpp.pPusher->push(4, subchannel +
                      NV056_SET_CONTEXT_DMA_VERTEX | 0x40000);
        vpp.pPusher->push(5, vpp.hInVideoMemContextDma);

        vpp.pPusher->push(6, subchannel +
                      NV056_SET_CONTEXT_DMA_STATE | 0x40000);
        vpp.pPusher->push(7, vpp.hInVideoMemContextDma);

        vpp.pPusher->push(8, subchannel +
                      NV056_SET_CONTEXT_DMA_COLOR | 0x40000);
        vpp.pPusher->push(9, vpp.hInVideoMemContextDma);

        vpp.pPusher->push(10, subchannel +
                       NV056_SET_CONTEXT_DMA_ZETA | 0x40000);
        vpp.pPusher->push(11, vpp.hInVideoMemContextDma);

        vpp.pPusher->adjust(12);

        // Set up one window clip rectangle to be the rendered area
        // All the other rectangles are deactivated
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_WINDOW_CLIP_HORIZONTAL(0) | 0x200000);
        vpp.pPusher->push(1, ((2047 << 16) | (-2048 & 0xfff)));
        vpp.pPusher->push(2, 0);
        vpp.pPusher->push(3, 0);
        vpp.pPusher->push(4, 0);
        vpp.pPusher->push(5, 0);
        vpp.pPusher->push(6, 0);
        vpp.pPusher->push(7, 0);
        vpp.pPusher->push(8, 0);

        vpp.pPusher->push(9, subchannel +
                      NV056_SET_WINDOW_CLIP_VERTICAL(0) | 0x200000);
        vpp.pPusher->push(10, ((2047 << 16) | (-2048 & 0xfff)));
        vpp.pPusher->push(11, 0);
        vpp.pPusher->push(12, 0);
        vpp.pPusher->push(13, 0);
        vpp.pPusher->push(14, 0);
        vpp.pPusher->push(15, 0);
        vpp.pPusher->push(16, 0);
        vpp.pPusher->push(17, 0);

        vpp.pPusher->push(18, subchannel +
                       NV056_SET_WINDOW_CLIP_TYPE | 0x40000);
        vpp.pPusher->push(19, NV056_SET_WINDOW_CLIP_TYPE_V_INCLUSIVE);
        vpp.pPusher->adjust(20);

        // Enable transforms
        tmpVal = (NV056_SET_TLMODE_W_DIVIDE_1_DISABLE << 2) |
                 (NV056_SET_TLMODE_W_DIVIDE_0_DISABLE << 1) |
                  NV056_SET_TLMODE_PASSTHROUGH_FALSE;

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_TLMODE | 0x40000);
        vpp.pPusher->push(1, tmpVal);
        vpp.pPusher->adjust(2);

        // Set Control0 defaults
        tmpVal = (NV056_SET_CONTROL0_PREMULTIPLIEDALPHA_FALSE << 24) |
                 (NV056_SET_CONTROL0_TEXTUREPERSPECTIVE_FALSE << 20) |
                 (NV056_SET_CONTROL0_Z_PERSPECTIVE_ENABLE_FALSE << 16) |
                 (NV056_SET_CONTROL0_Z_FORMAT_FIXED << 12) |
                 (NV056_SET_CONTROL0_WBUFFER_SELECT_0 << 8) |
                 (NV056_SET_CONTROL0_STENCIL_WRITE_ENABLE_TRUE);

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_CONTROL0 | 0x40000);
        vpp.pPusher->push(1, tmpVal);

        vpp.pPusher->adjust(2);

        // Load ModelView matrix and inverse ModelView matrix with identity
        fBuffer[0]  = 1.0; fBuffer[1]  = 0.0; fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
        fBuffer[4]  = 0.0; fBuffer[5]  = 1.0; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
        fBuffer[8]  = 0.0; fBuffer[9]  = 0.0; fBuffer[10] = 1.0; fBuffer[11] = 0.0;
        fBuffer[12] = 0.0; fBuffer[13] = 0.0; fBuffer[14] = 0.0; fBuffer[15] = 1.0;

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_MODEL_VIEW_MATRIX0(0) | 0x400000);
        vpp.pPusher->push(1, ftol[0]);
        vpp.pPusher->push(2, ftol[4]);
        vpp.pPusher->push(3, ftol[8]);
        vpp.pPusher->push(4, ftol[12]);
        vpp.pPusher->push(5, ftol[1]);
        vpp.pPusher->push(6, ftol[5]);
        vpp.pPusher->push(7, ftol[9]);
        vpp.pPusher->push(8, ftol[13]);
        vpp.pPusher->push(9, ftol[2]);
        vpp.pPusher->push(10, ftol[6]);
        vpp.pPusher->push(11, ftol[10]);
        vpp.pPusher->push(12, ftol[14]);
        vpp.pPusher->push(13, ftol[3]);
        vpp.pPusher->push(14, ftol[7]);
        vpp.pPusher->push(15, ftol[11]);
        vpp.pPusher->push(16, ftol[15]);

        vpp.pPusher->push(17, subchannel +
                       NV056_SET_INVERSE_MODEL_VIEW_MATRIX0(0) | 0x400000);
        vpp.pPusher->push(18, ftol[0]);
        vpp.pPusher->push(19, ftol[4]);
        vpp.pPusher->push(20, ftol[8]);
        vpp.pPusher->push(21, ftol[12]);
        vpp.pPusher->push(22, ftol[1]);
        vpp.pPusher->push(23, ftol[5]);
        vpp.pPusher->push(24, ftol[9]);
        vpp.pPusher->push(25, ftol[13]);
        vpp.pPusher->push(26, ftol[2]);
        vpp.pPusher->push(27, ftol[6]);
        vpp.pPusher->push(28, ftol[10]);
        vpp.pPusher->push(29, ftol[14]);
        vpp.pPusher->push(30, ftol[3]);
        vpp.pPusher->push(31, ftol[7]);
        vpp.pPusher->push(32, ftol[11]);
        vpp.pPusher->push(33, ftol[15]);

        vpp.pPusher->adjust(34);

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_SURFACE_FORMAT | 0x40000);
        vpp.pPusher->push(1, (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
                       NV056_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8);
        vpp.pPusher->adjust(2);

        // Set up viewport
        xTrans = (float)(-2048.0 - (1.0 / 32.0)); // hardware epsilon
        yTrans = (float)(-2048.0 - (1.0 / 32.0));
        zTrans = 0.0;

        // Load composite matrix assuming full pel scaling of surface coordinates
        fBuffer[0]  = 0.25; // quarter pel xScale
        fBuffer[1]  = 0.0;
        fBuffer[2]  = 0.0;
        fBuffer[3]  = 0.0;
        fBuffer[4]  = 0.0;
        fBuffer[5]  = 0.25; // quarter pel pel yScale
        fBuffer[6]  = 0.0;
        fBuffer[7]  = 0.0;
        fBuffer[8]  = 0.0;
        fBuffer[9]  = 0.0;
        fBuffer[10] = 1.0;
        fBuffer[11] = 0.0;
        fBuffer[12] = 0.0;
        fBuffer[13] = 0.0;
        fBuffer[14] = 0.0;
        fBuffer[15] = 1.0;

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_COMPOSITE_MATRIX(0) | 0x400000);
        vpp.pPusher->push(1, ftol[0]);
        vpp.pPusher->push(2, ftol[4]);
        vpp.pPusher->push(3, ftol[8]);
        vpp.pPusher->push(4, ftol[12]);
        vpp.pPusher->push(5, ftol[1]);
        vpp.pPusher->push(6, ftol[5]);
        vpp.pPusher->push(7, ftol[9]);
        vpp.pPusher->push(8, ftol[13]);
        vpp.pPusher->push(9, ftol[2]);
        vpp.pPusher->push(10, ftol[6]);
        vpp.pPusher->push(11, ftol[10]);
        vpp.pPusher->push(12, ftol[14]);
        vpp.pPusher->push(13, ftol[3]);
        vpp.pPusher->push(14, ftol[7]);
        vpp.pPusher->push(15, ftol[11]);
        vpp.pPusher->push(16, ftol[15]);

        vpp.pPusher->adjust(17);

        // Set viewport offset separately
        fBuffer[0] = xTrans;
        fBuffer[1] = yTrans;
        fBuffer[2] = zTrans;
        fBuffer[3] = 0.0;

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_VIEWPORT_OFFSET(0) | 0x100000);
        vpp.pPusher->push(1, ftol[0]);
        vpp.pPusher->push(2, ftol[1]);
        vpp.pPusher->push(3, ftol[2]);
        vpp.pPusher->push(4, ftol[3]);

        vpp.pPusher->adjust(5);

        // Set front and back clipping
        fBuffer[0] = 0.0;
        fBuffer[1] = 1.0;

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_CLIP_MIN | 0x80000);
        vpp.pPusher->push(1, ftol[0]);
        vpp.pPusher->push(2, ftol[1]);

        vpp.pPusher->adjust(3);

        // Disable almost everything

        // Disable alpha test
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_ALPHA_TEST_ENABLE | 0x40000);
        vpp.pPusher->push(1, NV056_SET_ALPHA_TEST_ENABLE_V_FALSE);

        // Disable blending
        vpp.pPusher->push(2, subchannel +
                      NV056_SET_BLEND_ENABLE | 0x40000);
        vpp.pPusher->push(3, NV056_SET_BLEND_ENABLE_V_FALSE);

        // Disable culling
        vpp.pPusher->push(4, subchannel +
                      NV056_SET_CULL_FACE_ENABLE | 0x40000);
        vpp.pPusher->push(5, NV056_SET_CULL_FACE_ENABLE_V_FALSE);

        // Disable depth test
        vpp.pPusher->push(6, subchannel +
                      NV056_SET_DEPTH_TEST_ENABLE | 0x40000);
        vpp.pPusher->push(7, NV056_SET_DEPTH_TEST_ENABLE_V_FALSE);

        // Disable dither
        vpp.pPusher->push(8, subchannel +
                      NV056_SET_DITHER_ENABLE | 0x40000);
        vpp.pPusher->push(9, NV056_SET_DITHER_ENABLE_V_FALSE);

        // Disable lighting
        vpp.pPusher->push(10, subchannel +
                       NV056_SET_LIGHTING_ENABLE | 0x40000);
        vpp.pPusher->push(11, NV056_SET_LIGHTING_ENABLE_V_FALSE);

        // Disable point parameters computation
        vpp.pPusher->push(12, subchannel +
                       NV056_SET_POINT_PARAMS_ENABLE | 0x40000);
        vpp.pPusher->push(13, NV056_SET_POINT_PARAMS_ENABLE_V_FALSE);

        // Disable point smoothing
        vpp.pPusher->push(14, subchannel +
                       NV056_SET_POINT_SMOOTH_ENABLE | 0x40000);
        vpp.pPusher->push(15, NV056_SET_LINE_SMOOTH_ENABLE_V_FALSE);

        // Disable line smoothing
        vpp.pPusher->push(16, subchannel +
                       NV056_SET_LINE_SMOOTH_ENABLE | 0x40000);
        vpp.pPusher->push(17, NV056_SET_LINE_SMOOTH_ENABLE_V_FALSE);

        // Disable polygon smoothing
        vpp.pPusher->push(18, subchannel +
                       NV056_SET_POLY_SMOOTH_ENABLE | 0x40000);
        vpp.pPusher->push(19, NV056_SET_POLY_SMOOTH_ENABLE_V_FALSE);

        // Disable skinning
        vpp.pPusher->push(20, subchannel +
                       NV056_SET_SKIN_ENABLE | 0x40000);
        vpp.pPusher->push(21, NV056_SET_SKIN_ENABLE_V_FALSE);

        // Disable stencil test
        vpp.pPusher->push(22, subchannel +
                       NV056_SET_STENCIL_TEST_ENABLE | 0x40000);
        vpp.pPusher->push(23, NV056_SET_STENCIL_TEST_ENABLE_V_FALSE);

        // Disable polygon offset point
        vpp.pPusher->push(24, subchannel +
                       NV056_SET_POLY_OFFSET_POINT_ENABLE | 0x40000);
        vpp.pPusher->push(25, NV056_SET_POLY_OFFSET_POINT_ENABLE_V_FALSE);

        // Disable polygon offset line
        vpp.pPusher->push(26, subchannel +
                       NV056_SET_POLY_OFFSET_LINE_ENABLE | 0x40000);
        vpp.pPusher->push(27, NV056_SET_POLY_OFFSET_LINE_ENABLE_V_FALSE);

        // Disable polygon offset fill
        vpp.pPusher->push(28, subchannel +
                       NV056_SET_POLY_OFFSET_FILL_ENABLE | 0x40000);
        vpp.pPusher->push(29, NV056_SET_POLY_OFFSET_FILL_ENABLE_V_FALSE);

        vpp.pPusher->adjust(30);

        // Set light control
        tmpVal = (NV056_SET_LIGHT_CONTROL_SEPARATE_SPECULAR_EN_FALSE << 1) |
                  NV056_SET_LIGHT_CONTROL_SECONDARY_COLOR_EN_FALSE;

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_LIGHT_CONTROL | 0x40000);
        vpp.pPusher->push(1, tmpVal);

        // Disable color material
        vpp.pPusher->push(2, subchannel +
                      NV056_SET_COLOR_MATERIAL | 0x40000);
        vpp.pPusher->push(3, NV056_SET_COLOR_MATERIAL_V_DISABLED);

        // Set material emission
        fBuffer[0] = 0.0;

        vpp.pPusher->push(4, subchannel +
                      NV056_SET_MATERIAL_EMISSION(0) | 0xC0000);
        vpp.pPusher->push(5, ftol[0]);
        vpp.pPusher->push(6, ftol[0]);
        vpp.pPusher->push(7, ftol[0]);

        // Disable specular
        vpp.pPusher->push(8, subchannel +
                      NV056_SET_SPECULAR_ENABLE | 0x40000);
        vpp.pPusher->push(9, NV056_SET_SPECULAR_ENABLE_V_FALSE);

        // Disable all lights
        vpp.pPusher->push(10, subchannel +
                       NV056_SET_LIGHT_ENABLE_MASK | 0x40000);
        vpp.pPusher->push(11, 0);

        vpp.pPusher->adjust(12);

        // Disable texgen modes for now (OpenGL default is EYE_LINEAR)

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_TEXGEN_S(0) | 0x40000);
        vpp.pPusher->push(1, NV056_SET_TEXGEN_S_V_DISABLE);

        vpp.pPusher->push(2, subchannel +
                      NV056_SET_TEXGEN_T(0) | 0x40000);
        vpp.pPusher->push(3, NV056_SET_TEXGEN_T_V_DISABLE);

        vpp.pPusher->push(4, subchannel +
                      NV056_SET_TEXGEN_R(0) | 0x40000);
        vpp.pPusher->push(5, NV056_SET_TEXGEN_R_V_DISABLE);

        vpp.pPusher->push(6, subchannel +
                      NV056_SET_TEXGEN_Q(0) | 0x40000);
        vpp.pPusher->push(7, NV056_SET_TEXGEN_Q_V_DISABLE);

        vpp.pPusher->push(8, subchannel +
                      NV056_SET_TEXGEN_S(1) | 0x40000);
        vpp.pPusher->push(9, NV056_SET_TEXGEN_S_V_DISABLE);

        vpp.pPusher->push(10, subchannel +
                       NV056_SET_TEXGEN_T(1) | 0x40000);
        vpp.pPusher->push(11, NV056_SET_TEXGEN_T_V_DISABLE);

        vpp.pPusher->push(12, subchannel +
                       NV056_SET_TEXGEN_R(1) | 0x40000);
        vpp.pPusher->push(13, NV056_SET_TEXGEN_R_V_DISABLE);

        vpp.pPusher->push(14, subchannel +
                       NV056_SET_TEXGEN_Q(1) | 0x40000);
        vpp.pPusher->push(15, NV056_SET_TEXGEN_Q_V_DISABLE);

        vpp.pPusher->adjust(16);

        // Set default texgen planes
        fBuffer[0] = 0.0;
        fBuffer[1] = 1.0;

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_TEXGEN_SPLANE0(0) | 0x100000);
        vpp.pPusher->push(1, ftol[1]);
        vpp.pPusher->push(2, ftol[0]);
        vpp.pPusher->push(3, ftol[0]);
        vpp.pPusher->push(4, ftol[0]);

        vpp.pPusher->push(5, subchannel +
                      NV056_SET_TEXGEN_TPLANE0(0) | 0x100000);
        vpp.pPusher->push(6, ftol[0]);
        vpp.pPusher->push(7, ftol[1]);
        vpp.pPusher->push(8, ftol[0]);
        vpp.pPusher->push(9, ftol[0]);

        vpp.pPusher->push(10, subchannel +
                       NV056_SET_TEXGEN_RPLANE0(0) | 0x100000);
        vpp.pPusher->push(11, ftol[0]);
        vpp.pPusher->push(12, ftol[0]);
        vpp.pPusher->push(13, ftol[0]);
        vpp.pPusher->push(14, ftol[0]);

        vpp.pPusher->push(15, subchannel +
                       NV056_SET_TEXGEN_QPLANE0(0) | 0x100000);
        vpp.pPusher->push(16, ftol[0]);
        vpp.pPusher->push(17, ftol[0]);
        vpp.pPusher->push(18, ftol[0]);
        vpp.pPusher->push(19, ftol[0]);

        vpp.pPusher->push(20, subchannel +
                       NV056_SET_TEXGEN_SPLANE1(0) | 0x100000);
        vpp.pPusher->push(21, ftol[1]);
        vpp.pPusher->push(22, ftol[0]);
        vpp.pPusher->push(23, ftol[0]);
        vpp.pPusher->push(24, ftol[0]);

        vpp.pPusher->push(25, subchannel +
                       NV056_SET_TEXGEN_TPLANE1(0) | 0x100000);
        vpp.pPusher->push(26, ftol[0]);
        vpp.pPusher->push(27, ftol[1]);
        vpp.pPusher->push(28, ftol[0]);
        vpp.pPusher->push(29, ftol[0]);

        vpp.pPusher->push(30, subchannel +
                       NV056_SET_TEXGEN_RPLANE1(0) | 0x100000);
        vpp.pPusher->push(31, ftol[0]);
        vpp.pPusher->push(32, ftol[0]);
        vpp.pPusher->push(33, ftol[0]);
        vpp.pPusher->push(34, ftol[0]);

        vpp.pPusher->push(35, subchannel +
                       NV056_SET_TEXGEN_QPLANE1(0) | 0x100000);
        vpp.pPusher->push(36, ftol[0]);
        vpp.pPusher->push(37, ftol[0]);
        vpp.pPusher->push(38, ftol[0]);
        vpp.pPusher->push(39, ftol[0]);

        vpp.pPusher->adjust(40);

        // Set default fog mode
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_FOG_MODE | 0x40000);
        vpp.pPusher->push(1, NV056_SET_FOG_MODE_FOG_MODE_LINEAR);

        vpp.pPusher->push(2, subchannel +
                      NV056_SET_FOG_GEN_MODE | 0x40000);
        vpp.pPusher->push(3, NV056_SET_FOG_GEN_MODE_FOG_GEN_MODE_USE_INPUT);

        // Disable fog
        vpp.pPusher->push(4, subchannel +
                      NV056_SET_FOG_ENABLE | 0x40000);
        vpp.pPusher->push(5, NV056_SET_FOG_ENABLE_V_FALSE);

        // Set default fog parameters
        fBuffer[0] = 1.0;                           // K0
        fBuffer[1] = 1.0;                           // K1
        fBuffer[2] = 0.0;                           // K2

        vpp.pPusher->push(6, subchannel +
                      NV056_SET_FOG_PARAMS(0) | 0xC0000);
        vpp.pPusher->push(7, ftol[0]);
        vpp.pPusher->push(8, ftol[1]);
        vpp.pPusher->push(9, ftol[2]);

        // Set default fog color
        vpp.pPusher->push(10, subchannel +
                       NV056_SET_FOG_COLOR | 0x40000);
        vpp.pPusher->push(11, 0xFFFFFFFF);

        vpp.pPusher->adjust(12);

        // Set default alpha function
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_ALPHA_FUNC | 0x40000);
        vpp.pPusher->push(1, NV056_SET_ALPHA_FUNC_V_ALWAYS);

        vpp.pPusher->push(2, subchannel +
                      NV056_SET_ALPHA_REF | 0x40000);
        vpp.pPusher->push(3, 0);

        // Set default blend equation
        vpp.pPusher->push(4, subchannel +
                      NV056_SET_BLEND_EQUATION | 0x40000);
        vpp.pPusher->push(5, NV056_SET_BLEND_EQUATION_V_FUNC_ADD);

        // Set default blend color
        vpp.pPusher->push(6, subchannel +
                      NV056_SET_BLEND_COLOR | 0x40000);
        vpp.pPusher->push(7, 0);

        // Set default blend func source factor
        vpp.pPusher->push(8, subchannel +
                      NV056_SET_BLEND_FUNC_SFACTOR | 0x40000);
        vpp.pPusher->push(9, NV056_SET_BLEND_FUNC_SFACTOR_V_ONE);

        // Set default blend func destination factor
        vpp.pPusher->push(10, subchannel +
                       NV056_SET_BLEND_FUNC_DFACTOR | 0x40000);
        vpp.pPusher->push(11, NV056_SET_BLEND_FUNC_DFACTOR_V_ZERO);

        vpp.pPusher->adjust(12);

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_FRONT_FACE | 0x40000);
        vpp.pPusher->push(1, NV056_SET_FRONT_FACE_V_CCW);

        vpp.pPusher->push(2, subchannel +
                      NV056_SET_CULL_FACE | 0x40000);
        vpp.pPusher->push(3, NV056_SET_CULL_FACE_V_BACK);

        vpp.pPusher->push(4, subchannel +
                      NV056_SET_SHADE_MODE | 0x40000);
        vpp.pPusher->push(5, NV056_SET_SHADE_MODE_V_SMOOTH);

        vpp.pPusher->push(6, subchannel +
                      NV056_SET_FRONT_POLYGON_MODE | 0x40000);
        vpp.pPusher->push(7, NV056_SET_FRONT_POLYGON_MODE_V_FILL);

        vpp.pPusher->push(8, subchannel +
                      NV056_SET_BACK_POLYGON_MODE | 0x40000);
        vpp.pPusher->push(9, NV056_SET_BACK_POLYGON_MODE_V_FILL);

        fBuffer[0] = 0.0;

        vpp.pPusher->push(10, subchannel +
                       NV056_SET_POLYGON_OFFSET_SCALE_FACTOR | 0x40000);
        vpp.pPusher->push(11, ftol[0]);

        vpp.pPusher->push(12, subchannel +
                       NV056_SET_POLYGON_OFFSET_BIAS | 0x40000);
        vpp.pPusher->push(13, ftol[0]);

        vpp.pPusher->push(14, subchannel +
                       NV056_SET_EDGE_FLAG | 0x40000);
        vpp.pPusher->push(15, 1);

        vpp.pPusher->adjust(16);

        // Initialize some vertex attributes
        fBuffer[0] = 0.0;
        fBuffer[1] = 1.0;

        // Set color for MODULATE blend mode
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_DIFFUSE_COLOR4F(0) | 0x100000);
        vpp.pPusher->push(1, ftol[1]);
        vpp.pPusher->push(2, ftol[1]);
        vpp.pPusher->push(3, ftol[1]);
        vpp.pPusher->push(4, ftol[1]);

        vpp.pPusher->push(5, subchannel +
                      NV056_SET_SPECULAR_COLOR3F(0) | 0xC0000);
        vpp.pPusher->push(6, ftol[0]);
        vpp.pPusher->push(7, ftol[0]);
        vpp.pPusher->push(8, ftol[0]);

        vpp.pPusher->push(9, subchannel +
                      NV056_SET_TEXCOORD0_4F(0) | 0x100000);
        vpp.pPusher->push(10, ftol[0]);
        vpp.pPusher->push(11, ftol[0]);
        vpp.pPusher->push(12, ftol[0]);
        vpp.pPusher->push(13, ftol[1]);

        vpp.pPusher->push(14, subchannel +
                       NV056_SET_TEXCOORD1_4F(0) | 0x100000);
        vpp.pPusher->push(15, ftol[0]);
        vpp.pPusher->push(16, ftol[0]);
        vpp.pPusher->push(17, ftol[0]);
        vpp.pPusher->push(18, ftol[1]);

        vpp.pPusher->push(19, subchannel +
                       NV056_SET_NORMAL3F(0) | 0xC0000);
        vpp.pPusher->push(20, ftol[0]);
        vpp.pPusher->push(21, ftol[0]);
        vpp.pPusher->push(22, ftol[1]);

        vpp.pPusher->push(23, subchannel +
                       NV056_SET_FOG1F | 0x40000);
        vpp.pPusher->push(24, ftol[0]);

        vpp.pPusher->push(25, subchannel +
                       NV056_SET_WEIGHT1F | 0x40000);
        vpp.pPusher->push(26, ftol[1]);

        vpp.pPusher->adjust(27);

        // Set depth function
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_DEPTH_FUNC | 0x40000);
        vpp.pPusher->push(1, NV056_SET_DEPTH_FUNC_V_ALWAYS);

        // Set color mask
        tmpVal = (NV056_SET_COLOR_MASK_ALPHA_WRITE_ENABLE_TRUE << 24) |
                 (NV056_SET_COLOR_MASK_RED_WRITE_ENABLE_TRUE << 16) |
                 (NV056_SET_COLOR_MASK_GREEN_WRITE_ENABLE_TRUE << 8) |
                  NV056_SET_COLOR_MASK_BLUE_WRITE_ENABLE_TRUE;

        vpp.pPusher->push(2, subchannel +
                      NV056_SET_COLOR_MASK | 0x40000);
        vpp.pPusher->push(3, tmpVal);

        // Set depth mask
        vpp.pPusher->push(4, subchannel +
                      NV056_SET_DEPTH_MASK | 0x40000);
        vpp.pPusher->push(5, NV056_SET_DEPTH_MASK_V_FALSE);

        vpp.pPusher->adjust(6);

        // Initialize stencil state (test already disabled above)
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_STENCIL_FUNC | 0x40000);
        vpp.pPusher->push(1, NV056_SET_STENCIL_FUNC_V_ALWAYS);

        vpp.pPusher->push(2, subchannel +
                      NV056_SET_STENCIL_FUNC_REF | 0x40000);
        vpp.pPusher->push(3, 0);

        vpp.pPusher->push(4, subchannel +
                      NV056_SET_STENCIL_FUNC_MASK | 0x40000);
        vpp.pPusher->push(5, 0x000000FF);

        vpp.pPusher->push(6, subchannel +
                      NV056_SET_STENCIL_OP_FAIL | 0x40000);
        vpp.pPusher->push(7, NV056_SET_STENCIL_OP_FAIL_V_KEEP);

        vpp.pPusher->push(8, subchannel +
                      NV056_SET_STENCIL_OP_ZFAIL | 0x40000);
        vpp.pPusher->push(9, NV056_SET_STENCIL_OP_ZFAIL_V_KEEP);

        vpp.pPusher->push(10, subchannel +
                       NV056_SET_STENCIL_OP_ZPASS | 0x40000);
        vpp.pPusher->push(11, NV056_SET_STENCIL_OP_ZPASS_V_KEEP);

        vpp.pPusher->push(12, subchannel +
                       NV056_SET_STENCIL_MASK | 0x40000);
        vpp.pPusher->push(13, 0x000000FF);

        vpp.pPusher->adjust(14);

        // This is set once and forgotten. It puts the class in OGL flat shade mode.
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_FLAT_SHADE_OP | 0x40000);
        vpp.pPusher->push(1, NV056_SET_FLAT_SHADE_OP_V_LAST_VTX);

        vpp.pPusher->adjust(2);

        // Load texture matrices assuming quarter pel scaling of texture coordinates
        fBuffer[0]  = 0.25; fBuffer[1]  = 0.0;  fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
        fBuffer[4]  = 0.0;  fBuffer[5]  = 0.25; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
        fBuffer[8]  = 0.0;  fBuffer[9]  = 0.0;  fBuffer[10] = 1.0; fBuffer[11] = 0.0;
        fBuffer[12] = 0.0;  fBuffer[13] = 0.0;  fBuffer[14] = 0.0; fBuffer[15] = 1.0;

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_TEXTURE_MATRIX0(0) | 0x400000);
        vpp.pPusher->push(1, ftol[0]);
        vpp.pPusher->push(2, ftol[4]);
        vpp.pPusher->push(3, ftol[8]);
        vpp.pPusher->push(4, ftol[12]);
        vpp.pPusher->push(5, ftol[1]);
        vpp.pPusher->push(6, ftol[5]);
        vpp.pPusher->push(7, ftol[9]);
        vpp.pPusher->push(8, ftol[13]);
        vpp.pPusher->push(9, ftol[2]);
        vpp.pPusher->push(10, ftol[6]);
        vpp.pPusher->push(11, ftol[10]);
        vpp.pPusher->push(12, ftol[14]);
        vpp.pPusher->push(13, ftol[3]);
        vpp.pPusher->push(14, ftol[7]);
        vpp.pPusher->push(15, ftol[11]);
        vpp.pPusher->push(16, ftol[15]);

        vpp.pPusher->push(17, subchannel +
            NV056_SET_TEXTURE_MATRIX1(0) | 0x400000);
        vpp.pPusher->push(18, ftol[0]);
        vpp.pPusher->push(19, ftol[4]);
        vpp.pPusher->push(20, ftol[8]);
        vpp.pPusher->push(21, ftol[12]);
        vpp.pPusher->push(22, ftol[1]);
        vpp.pPusher->push(23, ftol[5]);
        vpp.pPusher->push(24, ftol[9]);
        vpp.pPusher->push(25, ftol[13]);
        vpp.pPusher->push(26, ftol[2]);
        vpp.pPusher->push(27, ftol[6]);
        vpp.pPusher->push(28, ftol[10]);
        vpp.pPusher->push(29, ftol[14]);
        vpp.pPusher->push(30, ftol[3]);
        vpp.pPusher->push(31, ftol[7]);
        vpp.pPusher->push(32, ftol[11]);
        vpp.pPusher->push(33, ftol[15]);

        vpp.pPusher->adjust(34);

        vpp.pPusher->start(TRUE);

        // Enable both texture matrices since these texture coordinates are in quarter pel
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_TEXTURE_MATRIX0_ENABLE | 0x40000);
        vpp.pPusher->push(1, NV056_SET_TEXTURE_MATRIX0_ENABLE_V_TRUE);
        vpp.pPusher->push(2, subchannel +
                      NV056_SET_TEXTURE_MATRIX1_ENABLE | 0x40000);
        vpp.pPusher->push(3, NV056_SET_TEXTURE_MATRIX1_ENABLE_V_TRUE);
        vpp.pPusher->adjust(4);

        tmpVal = (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) |
                 (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
                 (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) |
                 (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
                 (1 << 12) | // 1 MIPMAP level
                 (NV096_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_A8R8G8B8 << 7) |
                 (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) |
                 (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) |
                 (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
                  NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

        vpp.pPusher->push(0, subchannel +
                      NV056_SET_TEXTURE_FORMAT(0) | 0x80000);
        vpp.pPusher->push(1, tmpVal);
        vpp.pPusher->push(2, tmpVal);
        vpp.pPusher->push(3, subchannel +
                      NV056_SET_TEXTURE_CONTROL0(0) | 0x80000);
        vpp.pPusher->push(4, (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                      (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                      (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_1 << 4) |
                      (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                      (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                       NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE);
        vpp.pPusher->push(5, (NV056_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
                      (15 << 8) | // huh ?  default MAX_LOD_CLAMP from OGL (no clamping)
                      (NV056_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
                      (NV056_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
                      (NV056_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
                       NV056_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE);
        vpp.pPusher->push(6, subchannel +
                      NV056_SET_TEXTURE_CONTROL2(0) | 0x80000);
        vpp.pPusher->push(7, 0);
        vpp.pPusher->push(8, 0);

        tmpVal = (NV056_SET_TEXTURE_FILTER_TEXTUREMAG_LINEAR << 28) |
                 (NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR << 24);

        vpp.pPusher->push(9, subchannel +
                       NV056_SET_TEXTURE_FILTER(0) | 0x80000);
        vpp.pPusher->push(10, tmpVal);
        vpp.pPusher->push(11, tmpVal);

        vpp.pPusher->adjust(12);

        // Disable fog
        vpp.pPusher->push(0, subchannel +
                      NV056_SET_FOG_ENABLE | 0x40000);
        vpp.pPusher->push(1, NV056_SET_FOG_ENABLE_V_FALSE);

        vpp.pPusher->adjust(2);

        // Now setup combiners
        // Default register combiner settings are:
        // A = primary color
        // B = one
        // C = zero
        // D = zero

        // Our temporal filter moComp values
        // combiner0, RGB portion:   A = constantColor0
        // combiner0, RGB portion:   B = texture0
        // combiner0, RGB portion:   C = constantColor1
        // combiner0, RGB portion:   D = texture1

        // Initialize combiner ICW methods
        vpp.pPusher->push(0, subchannel | NV056_SET_COMBINER_ALPHA_ICW(0) | 0x80000);
        vpp.pPusher->push(1, (NV056_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                      (NV056_SET_COMBINER_ALPHA_ICW_A_ALPHA_TRUE << 28) |
                      (NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1 << 24) |

                      (NV056_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                      (NV056_SET_COMBINER_ALPHA_ICW_B_ALPHA_TRUE << 20) |
                      (NV056_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_8 << 16) |

                      (NV056_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_INVERT << 13) |
                      (NV056_SET_COMBINER_ALPHA_ICW_C_ALPHA_TRUE << 12) |
                      (NV056_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_1 << 8) |

                      (NV056_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                      (NV056_SET_COMBINER_ALPHA_ICW_D_ALPHA_TRUE << 4) |
                       NV056_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_9);

        vpp.pPusher->push(2, (NV056_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                      (NV056_SET_COMBINER_ALPHA_ICW_A_ALPHA_TRUE << 28) |
                      (NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_0 << 24) |

                      (NV056_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_INVERT << 21) |
                      (NV056_SET_COMBINER_ALPHA_ICW_B_ALPHA_TRUE << 20) |
                      (NV056_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_0 << 16) |

                      (NV056_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                      (NV056_SET_COMBINER_ALPHA_ICW_C_ALPHA_TRUE << 12) |
                      (NV056_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_0 << 8) |

                      (NV056_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                      (NV056_SET_COMBINER_ALPHA_ICW_D_ALPHA_TRUE << 4) |
                       NV056_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_0);

        vpp.pPusher->push(3, subchannel | NV056_SET_COMBINER_COLOR_ICW(0) | 0x80000);
        vpp.pPusher->push(4, (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                      (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                      (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                      (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                      (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                      (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                      (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_INVERT << 13) |
                      (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                      (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1 << 8) |

                      (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                      (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                       NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9);

        vpp.pPusher->push(5, (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                      (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                      (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_0 << 24) |

                      (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_INVERT << 21) |
                      (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                      (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_0 << 16) |

                      (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                      (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                      (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0 << 8) |

                      (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                      (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                       NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0);

        // Load combination factors
        vpp.pPusher->push(6, subchannel | NV056_SET_COMBINE_FACTOR(0) | 0x80000);
        vpp.pPusher->push(7, 0x80808080); // % of top field
        vpp.pPusher->push(8, 0x80808080); // % of bottom field

        vpp.pPusher->adjust(9);

        // combiner0, RGB portion:      spare0 = A * B + C * D = texture0 + texture1
        // Initialize combiner OCW methods
        vpp.pPusher->push(0, subchannel | NV056_SET_COMBINER_ALPHA_OCW(0) | 0x80000);
        vpp.pPusher->push(1, (NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT << 15) |
                      (NV056_SET_COMBINER_ALPHA_OCW_MUX_ENABLE_FALSE << 14) |
                      (NV056_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_C << 8) |
                      (NV056_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0 << 4) |
                       NV056_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0);

        vpp.pPusher->push(2, (NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT << 15) |
                      (NV056_SET_COMBINER_ALPHA_OCW_MUX_ENABLE_FALSE << 14) |
                      (NV056_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_C << 8) |
                      (NV056_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0 << 4) |
                       NV056_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0);

        vpp.pPusher->push(3, subchannel | NV056_SET_COMBINER0_COLOR_OCW | 0x40000);
        vpp.pPusher->push(4, (NV056_SET_COMBINER0_COLOR_OCW_OPERATION_NOSHIFT << 15) |
                      (NV056_SET_COMBINER0_COLOR_OCW_MUX_ENABLE_FALSE << 14) |
                      (NV056_SET_COMBINER0_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |
                      (NV056_SET_COMBINER0_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |
                      (NV056_SET_COMBINER0_COLOR_OCW_SUM_DST_REG_C << 8) |
                      (NV056_SET_COMBINER0_COLOR_OCW_AB_DST_REG_0 << 4) |
                       NV056_SET_COMBINER0_COLOR_OCW_CD_DST_REG_0);

        vpp.pPusher->push(5, subchannel | NV056_SET_COMBINER1_COLOR_OCW | 0x40000);
        vpp.pPusher->push(6, (NV056_SET_COMBINER1_COLOR_OCW_ITERATION_COUNT_ONE << 28) |    \
                      (NV056_SET_COMBINER1_COLOR_OCW_MUX_SELECT_MSB << 27) |         \
                      (NV056_SET_COMBINER1_COLOR_OCW_OPERATION_NOSHIFT << 15) |      \
                      (NV056_SET_COMBINER1_COLOR_OCW_MUX_ENABLE_FALSE << 14) |       \
                      (NV056_SET_COMBINER1_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |    \
                      (NV056_SET_COMBINER1_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |    \
                      (NV056_SET_COMBINER1_COLOR_OCW_SUM_DST_REG_C << 8) |           \
                      (NV056_SET_COMBINER1_COLOR_OCW_AB_DST_REG_0 << 4) |            \
                       NV056_SET_COMBINER1_COLOR_OCW_CD_DST_REG_0);

        vpp.pPusher->adjust(7);

        // Load final stages with default values
        // Initialize combiner final CW methods
        vpp.pPusher->push(0, subchannel | NV056_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
        vpp.pPusher->push(1, DEFAULT_FINAL_CW0);
        vpp.pPusher->push(2, subchannel | NV056_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
        vpp.pPusher->push(3, DEFAULT_FINAL_CW1);
        vpp.pPusher->adjust(4);

        vpp.pPusher->start(TRUE);
    }
    return TRUE;
}
#endif // #if (NVARCH >= 0x10)


//---------------------------------------------------------------------------
// vppInitKelvin
//      Inits Kelvin object for temporal filter usage.  Kelvin is a shared,
//      cached object, so these methods only get set when we are a new consumer
LOCAL BOOL vppInitKelvin(Vpp_t *pVpp)
{
    DWORD tmpVal;
    float fBuffer[16];
    unsigned long *ftol = (unsigned long *)&fBuffer[0];
    float xTrans, yTrans, zTrans;
    Vpp_t &vpp = *pVpp;
    NvU32 subchannel = SUB_CHANNEL(vpp.threeDClassSubCh);

    nvAssert(vpp.dwFlags & VPP_FLAG_KELVIN_3D);

    nvAssert(vpp.pThreeDClassLastUser);
    *vpp.pThreeDClassLastUser = MODULE_ID_DDRAW_VPP;

    // Let D3D code know that we have touched NV
    vpp.pDriverData->TwoDRenderingOccurred = 1;

    // Reset the celsius context Dmas to use the default video context Dma
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
    vpp.pPusher->push(1, vpp.hToVideoMemContextDma);

    vpp.pPusher->push(2, subchannel +
                         NV097_SET_CONTEXT_DMA_A | 0x40000);
    vpp.pPusher->push(3, vpp.hInVideoMemContextDma);

    vpp.pPusher->push(4, subchannel +
                         NV097_SET_CONTEXT_DMA_B | 0x40000);
    vpp.pPusher->push(5, vpp.hInVideoMemContextDma);

    vpp.pPusher->push(6, subchannel +
                         NV097_SET_CONTEXT_DMA_STATE | 0x40000);
    vpp.pPusher->push(7, vpp.hToVideoMemContextDma);

    vpp.pPusher->push(8, subchannel +
                         NV097_SET_CONTEXT_DMA_COLOR | 0x40000);
    vpp.pPusher->push(9, vpp.hInVideoMemContextDma);

    vpp.pPusher->push(10, subchannel +
                          NV097_SET_CONTEXT_DMA_ZETA | 0x40000);
    vpp.pPusher->push(11, vpp.hInVideoMemContextDma);

    vpp.pPusher->push(12, subchannel +
                          NV097_SET_CONTEXT_DMA_VERTEX_A | 0x40000);
    vpp.pPusher->push(13, vpp.hInVideoMemContextDma);

    vpp.pPusher->push(14, subchannel +
                          NV097_SET_CONTEXT_DMA_VERTEX_B | 0x40000);
    vpp.pPusher->push(15, vpp.hInVideoMemContextDma);

    vpp.pPusher->push(16, subchannel +
                          NV097_SET_CONTEXT_DMA_SEMAPHORE | 0x40000);
    vpp.pPusher->push(17, NV01_NULL_OBJECT);

    vpp.pPusher->push(18, subchannel +
                          NV097_SET_CONTEXT_DMA_REPORT | 0x40000);
    vpp.pPusher->push(19, NV01_NULL_OBJECT);

    vpp.pPusher->adjust(20);

    // Set up one window clip rectangle to be the rendered area
    // All the other rectangles are deactivated
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_WINDOW_CLIP_HORIZONTAL(0) | 0x200000);
    vpp.pPusher->push(1, ((2047 << 16) | 0));  // PG: used to be (-2048 & 0xfff)
    vpp.pPusher->push(2, 0);
    vpp.pPusher->push(3, 0);
    vpp.pPusher->push(4, 0);
    vpp.pPusher->push(5, 0);
    vpp.pPusher->push(6, 0);
    vpp.pPusher->push(7, 0);
    vpp.pPusher->push(8, 0);

    vpp.pPusher->push(9, subchannel +
                         NV097_SET_WINDOW_CLIP_VERTICAL(0) | 0x200000);
    vpp.pPusher->push(10, ((2047 << 16) | 0));  // PG: used to be (-2048 & 0xfff)
    vpp.pPusher->push(11, 0);
    vpp.pPusher->push(12, 0);
    vpp.pPusher->push(13, 0);
    vpp.pPusher->push(14, 0);
    vpp.pPusher->push(15, 0);
    vpp.pPusher->push(16, 0);
    vpp.pPusher->push(17, 0);

    vpp.pPusher->push(18, subchannel +
                          NV097_SET_WINDOW_CLIP_TYPE | 0x40000);
    vpp.pPusher->push(19, NV097_SET_WINDOW_CLIP_TYPE_V_INCLUSIVE);
    vpp.pPusher->adjust(20);

    // Set Control0 defaults
    tmpVal = (NV097_SET_CONTROL0_COLOR_SPACE_CONVERT_PASS << 28) |
             (NV097_SET_CONTROL0_PREMULTIPLIEDALPHA_FALSE << 24) |
             (NV097_SET_CONTROL0_TEXTUREPERSPECTIVE_FALSE << 20) |
             (NV097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE_FALSE << 16) |
             (NV097_SET_CONTROL0_Z_FORMAT_FIXED << 12) |
             (NV097_SET_CONTROL0_STENCIL_WRITE_ENABLE_TRUE);

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_CONTROL0 | 0x40000);
    vpp.pPusher->push(1, tmpVal);
    vpp.pPusher->adjust(2);

    // Make sure to disable any transform program which may be running in D3D
    vpp.pPusher->push(0, subchannel + NV097_SET_TRANSFORM_EXECUTION_MODE | 0x40000);
    vpp.pPusher->push(1, ((NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV << 2) |
                           NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_FIXED));
    vpp.pPusher->adjust(2);

    // Load ModelView matrix and inverse ModelView matrix with identity
    fBuffer[0]  = 1.0; fBuffer[1]  = 0.0; fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
    fBuffer[4]  = 0.0; fBuffer[5]  = 1.0; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
    fBuffer[8]  = 0.0; fBuffer[9]  = 0.0; fBuffer[10] = 1.0; fBuffer[11] = 0.0;
    fBuffer[12] = 0.0; fBuffer[13] = 0.0; fBuffer[14] = 0.0; fBuffer[15] = 1.0;

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_PROJECTION_MATRIX(0) | 0x400000);
    vpp.pPusher->push(1, ftol[0]);
    vpp.pPusher->push(2, ftol[4]);
    vpp.pPusher->push(3, ftol[8]);
    vpp.pPusher->push(4, ftol[12]);
    vpp.pPusher->push(5, ftol[1]);
    vpp.pPusher->push(6, ftol[5]);
    vpp.pPusher->push(7, ftol[9]);
    vpp.pPusher->push(8, ftol[13]);
    vpp.pPusher->push(9, ftol[2]);
    vpp.pPusher->push(10, ftol[6]);
    vpp.pPusher->push(11, ftol[10]);
    vpp.pPusher->push(12, ftol[14]);
    vpp.pPusher->push(13, ftol[3]);
    vpp.pPusher->push(14, ftol[7]);
    vpp.pPusher->push(15, ftol[11]);
    vpp.pPusher->push(16, ftol[15]);
    vpp.pPusher->adjust(17);

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_MODEL_VIEW_MATRIX0(0) | 0x400000);
    vpp.pPusher->push(1, ftol[0]);
    vpp.pPusher->push(2, ftol[4]);
    vpp.pPusher->push(3, ftol[8]);
    vpp.pPusher->push(4, ftol[12]);
    vpp.pPusher->push(5, ftol[1]);
    vpp.pPusher->push(6, ftol[5]);
    vpp.pPusher->push(7, ftol[9]);
    vpp.pPusher->push(8, ftol[13]);
    vpp.pPusher->push(9, ftol[2]);
    vpp.pPusher->push(10, ftol[6]);
    vpp.pPusher->push(11, ftol[10]);
    vpp.pPusher->push(12, ftol[14]);
    vpp.pPusher->push(13, ftol[3]);
    vpp.pPusher->push(14, ftol[7]);
    vpp.pPusher->push(15, ftol[11]);
    vpp.pPusher->push(16, ftol[15]);
    vpp.pPusher->adjust(17);

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_INVERSE_MODEL_VIEW_MATRIX0(0) | 0x400000);
    vpp.pPusher->push(1, ftol[0]);
    vpp.pPusher->push(2, ftol[4]);
    vpp.pPusher->push(3, ftol[8]);
    vpp.pPusher->push(4, ftol[12]);
    vpp.pPusher->push(5, ftol[1]);
    vpp.pPusher->push(6, ftol[5]);
    vpp.pPusher->push(7, ftol[9]);
    vpp.pPusher->push(8, ftol[13]);
    vpp.pPusher->push(9, ftol[2]);
    vpp.pPusher->push(10, ftol[6]);
    vpp.pPusher->push(11, ftol[10]);
    vpp.pPusher->push(12, ftol[14]);
    vpp.pPusher->push(13, ftol[3]);
    vpp.pPusher->push(14, ftol[7]);
    vpp.pPusher->push(15, ftol[11]);
    vpp.pPusher->push(16, ftol[15]);

    vpp.pPusher->adjust(17);

    vpp.pPusher->push(0, subchannel +
                           NV097_NO_OPERATION | 0x40000);
    vpp.pPusher->push(1, 0);

    // PG - Modified 10/31/2000
    vpp.pPusher->push(2, subchannel +
                         NV097_SET_SURFACE_FORMAT | 0x40000);
    vpp.pPusher->push(3, ((NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) |
                          (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) |
                          (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) |
                           NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8));

    vpp.pPusher->adjust(4);

    // Set up viewport
    xTrans = (float)(1.0 / 32.0); // hardware epsilon
    yTrans = (float)(1.0 / 32.0);
    zTrans = 0.0;

    // Load composite matrix assuming full pel scaling of surface coordinates
    fBuffer[0]  = 0.25; // quarter pel xScale
    fBuffer[1]  = 0.0;
    fBuffer[2]  = 0.0;
    fBuffer[3]  = 0.0;
    fBuffer[4]  = 0.0;
    fBuffer[5]  = 0.25; // quarter pel pel yScale
    fBuffer[6]  = 0.0;
    fBuffer[7]  = 0.0;
    fBuffer[8]  = 0.0;
    fBuffer[9]  = 0.0;
    fBuffer[10] = 1.0;
    fBuffer[11] = 0.0;
    fBuffer[12] = 0.0;
    fBuffer[13] = 0.0;
    fBuffer[14] = 0.0;
    fBuffer[15] = 1.0;

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_COMPOSITE_MATRIX(0) | 0x400000);
    vpp.pPusher->push(1, ftol[0]);
    vpp.pPusher->push(2, ftol[4]);
    vpp.pPusher->push(3, ftol[8]);
    vpp.pPusher->push(4, ftol[12]);
    vpp.pPusher->push(5, ftol[1]);
    vpp.pPusher->push(6, ftol[5]);
    vpp.pPusher->push(7, ftol[9]);
    vpp.pPusher->push(8, ftol[13]);
    vpp.pPusher->push(9, ftol[2]);
    vpp.pPusher->push(10, ftol[6]);
    vpp.pPusher->push(11, ftol[10]);
    vpp.pPusher->push(12, ftol[14]);
    vpp.pPusher->push(13, ftol[3]);
    vpp.pPusher->push(14, ftol[7]);
    vpp.pPusher->push(15, ftol[11]);
    vpp.pPusher->push(16, ftol[15]);

    vpp.pPusher->adjust(17);

    // Set viewport offset separately
    fBuffer[0] = xTrans;
    fBuffer[1] = yTrans;
    fBuffer[2] = zTrans;
    fBuffer[3] = 0.0;

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_VIEWPORT_OFFSET(0) | 0x100000);
    vpp.pPusher->push(1, ftol[0]);
    vpp.pPusher->push(2, ftol[1]);
    vpp.pPusher->push(3, ftol[2]);
    vpp.pPusher->push(4, ftol[3]);
    vpp.pPusher->adjust(5);

    // PG - Modified 10/31/2000
    // Set front and back clipping
    fBuffer[0] = 0.0;
    fBuffer[1] = KELVIN_Z_SCALE24;

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_CLIP_MIN | 0x80000);
    vpp.pPusher->push(1, ftol[0]);
    vpp.pPusher->push(2, ftol[1]);
    vpp.pPusher->adjust(3);

    // Set light control
    tmpVal = (NV097_SET_LIGHT_CONTROL_SOUT_ZERO_OUT << 17) |
             (NV097_SET_LIGHT_CONTROL_LOCALEYE_FALSE << 16) |
              NV097_SET_LIGHT_CONTROL_SEPARATE_SPECULAR_EN_FALSE;

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_LIGHT_CONTROL | 0x40000);
    vpp.pPusher->push(1, tmpVal);

    // Disable color material
    tmpVal = (NV097_SET_COLOR_MATERIAL_BACK_SPECULAR_MATERIAL_DISABLE << 14) |
             (NV097_SET_COLOR_MATERIAL_BACK_DIFF_MATERIAL_DISABLE << 12) |
             (NV097_SET_COLOR_MATERIAL_BACK_AMBIENT_MATERIAL_DISABLE << 10) |
             (NV097_SET_COLOR_MATERIAL_BACK_EMISSIVE_MATERIAL_DISABLE << 8) |
             (NV097_SET_COLOR_MATERIAL_SPECULAR_MATERIAL_DISABLE << 6) |
             (NV097_SET_COLOR_MATERIAL_DIFF_MATERIAL_DISABLE << 4) |
             (NV097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL_DISABLE << 2) |
              NV097_SET_COLOR_MATERIAL_EMISSIVE_MATERIAL_DISABLE;

    vpp.pPusher->push(2, subchannel +
                         NV097_SET_COLOR_MATERIAL | 0x40000);
    vpp.pPusher->push(3, tmpVal);
    vpp.pPusher->adjust(4);

    // Set default fog mode
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_FOG_MODE | 0x40000);
    vpp.pPusher->push(1, NV097_SET_FOG_MODE_V_LINEAR);

    vpp.pPusher->push(2, subchannel +
                         NV097_SET_FOG_GEN_MODE | 0x40000);
    vpp.pPusher->push(3, NV097_SET_FOG_GEN_MODE_V_FOG_X);

    // Disable fog
    vpp.pPusher->push(4, subchannel +
                         NV097_SET_FOG_ENABLE | 0x40000);
    vpp.pPusher->push(5, NV097_SET_FOG_ENABLE_V_FALSE);

    // Set default fog color
    vpp.pPusher->push(6, subchannel +
                         NV097_SET_FOG_COLOR | 0x40000);
    vpp.pPusher->push(7, 0xFFFFFFFF);
    vpp.pPusher->adjust(8);

    // Disable almost everything

    // Disable alpha test
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_ALPHA_TEST_ENABLE | 0x40000);
    vpp.pPusher->push(1, NV097_SET_ALPHA_TEST_ENABLE_V_FALSE);

    // Disable blending
    vpp.pPusher->push(2, subchannel +
                         NV097_SET_BLEND_ENABLE | 0x40000);
    vpp.pPusher->push(3, NV097_SET_BLEND_ENABLE_V_FALSE);

    // Disable culling
    vpp.pPusher->push(4, subchannel +
                         NV097_SET_CULL_FACE_ENABLE | 0x40000);
    vpp.pPusher->push(5, NV097_SET_CULL_FACE_ENABLE_V_FALSE);

    // Disable depth test
    vpp.pPusher->push(6, subchannel +
                         NV097_SET_DEPTH_TEST_ENABLE | 0x40000);
    vpp.pPusher->push(7, NV097_SET_DEPTH_TEST_ENABLE_V_FALSE);

    // Disable dither
    vpp.pPusher->push(8, subchannel +
                         NV097_SET_DITHER_ENABLE | 0x40000);
    vpp.pPusher->push(9, NV097_SET_DITHER_ENABLE_V_FALSE);

    // Disable lighting
    vpp.pPusher->push(10, subchannel +
                          NV097_SET_LIGHTING_ENABLE | 0x40000);
    vpp.pPusher->push(11, NV097_SET_LIGHTING_ENABLE_V_FALSE);

    // Disable point parameters computation
    vpp.pPusher->push(12, subchannel +
                          NV097_SET_POINT_PARAMS_ENABLE | 0x40000);
    vpp.pPusher->push(13, NV097_SET_POINT_PARAMS_ENABLE_V_FALSE);

    // Disable point smoothing
    vpp.pPusher->push(14, subchannel +
                          NV097_SET_POINT_SMOOTH_ENABLE | 0x40000);
    vpp.pPusher->push(15, NV097_SET_LINE_SMOOTH_ENABLE_V_FALSE);

    // Disable line smoothing
    vpp.pPusher->push(16, subchannel +
                          NV097_SET_LINE_SMOOTH_ENABLE | 0x40000);
    vpp.pPusher->push(17, NV097_SET_LINE_SMOOTH_ENABLE_V_FALSE);

    // Disable polygon smoothing
    vpp.pPusher->push(18, subchannel +
                          NV097_SET_POLY_SMOOTH_ENABLE | 0x40000);
    vpp.pPusher->push(19, NV097_SET_POLY_SMOOTH_ENABLE_V_FALSE);

    // Disable stippling
    vpp.pPusher->push(20, subchannel +
                          NV097_SET_STIPPLE_CONTROL | 0x40000);
    vpp.pPusher->push(21, NV097_SET_STIPPLE_CONTROL_V_OFF);

    // Disable skinning
    vpp.pPusher->push(22, subchannel +
                          NV097_SET_SKIN_MODE | 0x40000);
    vpp.pPusher->push(23, NV097_SET_SKIN_MODE_V_OFF);

    // Disable stencil test
    vpp.pPusher->push(24, subchannel +
                          NV097_SET_STENCIL_TEST_ENABLE | 0x40000);
    vpp.pPusher->push(25, NV097_SET_STENCIL_TEST_ENABLE_V_FALSE);

    // Disable polygon offset point
    vpp.pPusher->push(26, subchannel +
                          NV097_SET_POLY_OFFSET_POINT_ENABLE | 0x40000);
    vpp.pPusher->push(27, NV097_SET_POLY_OFFSET_POINT_ENABLE_V_FALSE);

    // Disable polygon offset line
    vpp.pPusher->push(28, subchannel +
                          NV097_SET_POLY_OFFSET_LINE_ENABLE | 0x40000);
    vpp.pPusher->push(29, NV097_SET_POLY_OFFSET_LINE_ENABLE_V_FALSE);

    // Disable polygon offset fill
    vpp.pPusher->push(30, subchannel +
                          NV097_SET_POLY_OFFSET_FILL_ENABLE | 0x40000);
    vpp.pPusher->push(31, NV097_SET_POLY_OFFSET_FILL_ENABLE_V_FALSE);
    vpp.pPusher->adjust(32);

    // PG: This part unitl the material emission comment is new
    // Set default alpha function
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_ALPHA_FUNC | 0x40000);
    vpp.pPusher->push(1, NV097_SET_ALPHA_FUNC_V_ALWAYS);

    vpp.pPusher->push(2, subchannel +
                         NV097_SET_ALPHA_REF | 0x40000);
    vpp.pPusher->push(3, 0);

    // Initialize blend factors
    vpp.pPusher->push(4, subchannel +
                         NV097_SET_BLEND_FUNC_SFACTOR | 0x40000);
    vpp.pPusher->push(5, NV097_SET_BLEND_FUNC_SFACTOR_V_ONE);

    // PG - Changed 11/01/2000
    vpp.pPusher->push(6, subchannel +
                         NV097_SET_BLEND_FUNC_DFACTOR | 0x40000);
    vpp.pPusher->push(7, NV097_SET_BLEND_FUNC_DFACTOR_V_ZERO);

    // Set default blend color
    vpp.pPusher->push(8, subchannel +
                         NV097_SET_BLEND_COLOR | 0x40000);
    vpp.pPusher->push(9, 0);

    // PG - Modified 10/31/2000 - SIGNED
    // Set default blend equation
    vpp.pPusher->push(10, subchannel +
                          NV097_SET_BLEND_EQUATION | 0x40000);
    vpp.pPusher->push(11, NV097_SET_BLEND_EQUATION_V_FUNC_ADD_SIGNED);
    vpp.pPusher->adjust(12);

    // Set depth function
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_DEPTH_FUNC | 0x40000);
    vpp.pPusher->push(1, NV097_SET_DEPTH_FUNC_V_ALWAYS);

    // Set color mask
    tmpVal = (NV097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE_TRUE << 24) |
             (NV097_SET_COLOR_MASK_RED_WRITE_ENABLE_TRUE << 16) |
             (NV097_SET_COLOR_MASK_GREEN_WRITE_ENABLE_TRUE << 8) |
              NV097_SET_COLOR_MASK_BLUE_WRITE_ENABLE_TRUE;

    vpp.pPusher->push(2, subchannel +
                         NV097_SET_COLOR_MASK | 0x40000);
    vpp.pPusher->push(3, tmpVal);

    // Set depth mask
    vpp.pPusher->push(4, subchannel +
                         NV097_SET_DEPTH_MASK | 0x40000);
    vpp.pPusher->push(5, NV097_SET_DEPTH_MASK_V_FALSE);
    vpp.pPusher->adjust(6);

    // Initialize stencil state (test already disabled above)
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_STENCIL_MASK | 0x40000);
    vpp.pPusher->push(1, 0x000000FF);

    vpp.pPusher->push(2, subchannel +
                         NV097_SET_STENCIL_FUNC | 0x40000);
    vpp.pPusher->push(3, NV097_SET_STENCIL_FUNC_V_ALWAYS);

    vpp.pPusher->push(4, subchannel +
                         NV097_SET_STENCIL_FUNC_REF | 0x40000);
    vpp.pPusher->push(5, 0);

    vpp.pPusher->push(6, subchannel +
                         NV097_SET_STENCIL_FUNC_MASK | 0x40000);
    vpp.pPusher->push(7, 0x000000FF);

    vpp.pPusher->push(8, subchannel +
                         NV097_SET_STENCIL_OP_FAIL | 0x40000);
    vpp.pPusher->push(9, NV097_SET_STENCIL_OP_FAIL_V_KEEP);

    vpp.pPusher->push(10, subchannel +
                          NV097_SET_STENCIL_OP_ZFAIL | 0x40000);
    vpp.pPusher->push(11, NV097_SET_STENCIL_OP_ZFAIL_V_KEEP);

    vpp.pPusher->push(12, subchannel +
                          NV097_SET_STENCIL_OP_ZPASS | 0x40000);
    vpp.pPusher->push(13, NV097_SET_STENCIL_OP_ZPASS_V_KEEP);
    vpp.pPusher->adjust(14);

    // PG - Modified 10/31/2000 - V_SMOOTH
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_SHADE_MODE | 0x40000);
    vpp.pPusher->push(1, NV097_SET_SHADE_MODE_V_FLAT);

    fBuffer[0] = 0.0;

    vpp.pPusher->push(2, subchannel +
                         NV097_SET_POLYGON_OFFSET_SCALE_FACTOR | 0x40000);
    vpp.pPusher->push(3, ftol[0]);

    vpp.pPusher->push(4, subchannel +
                         NV097_SET_POLYGON_OFFSET_BIAS | 0x40000);
    vpp.pPusher->push(5, ftol[0]);

    vpp.pPusher->push(6, subchannel +
                         NV097_SET_FRONT_POLYGON_MODE | 0x40000);
    vpp.pPusher->push(7, NV097_SET_FRONT_POLYGON_MODE_V_FILL);

    vpp.pPusher->push(8, subchannel +
                         NV097_SET_BACK_POLYGON_MODE | 0x40000);
    vpp.pPusher->push(9, NV097_SET_BACK_POLYGON_MODE_V_FILL);

    vpp.pPusher->push(10, subchannel +
                          NV097_SET_CULL_FACE | 0x40000);
    vpp.pPusher->push(11, NV097_SET_CULL_FACE_V_BACK);

    vpp.pPusher->push(12, subchannel +
                          NV097_SET_FRONT_FACE | 0x40000);
    vpp.pPusher->push(13, NV097_SET_FRONT_FACE_V_CCW);

    vpp.pPusher->push(14, subchannel +
                          NV097_SET_NORMALIZATION_ENABLE | 0x40000);
    vpp.pPusher->push(15, NV097_SET_NORMALIZATION_ENABLE_V_FALSE);

    vpp.pPusher->push(16, subchannel +
                          NV097_SET_EDGE_FLAG | 0x40000);
    vpp.pPusher->push(17, NV097_SET_EDGE_FLAG_V_TRUE);
    vpp.pPusher->adjust(18);

    // Set material emission
    fBuffer[0] = 0.0;

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_MATERIAL_EMISSION(0) | 0xC0000);
    vpp.pPusher->push(1, ftol[0]);
    vpp.pPusher->push(2, ftol[0]);
    vpp.pPusher->push(3, ftol[0]);

    // Disable specular
    vpp.pPusher->push(4, subchannel +
                         NV097_SET_SPECULAR_ENABLE | 0x40000);
    vpp.pPusher->push(5, NV097_SET_SPECULAR_ENABLE_V_FALSE);

    // Disable all lights
    vpp.pPusher->push(6, subchannel +
                         NV097_SET_LIGHT_ENABLE_MASK | 0x40000);
    vpp.pPusher->push(7, 0);
    vpp.pPusher->adjust(8);

    // Disable texgen modes for now (OpenGL default is EYE_LINEAR)
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_TEXGEN_S(0) | 0x40000);
    vpp.pPusher->push(1, NV097_SET_TEXGEN_S_V_DISABLE);

    vpp.pPusher->push(2, subchannel +
                         NV097_SET_TEXGEN_T(0) | 0x40000);
    vpp.pPusher->push(3, NV097_SET_TEXGEN_T_V_DISABLE);

    vpp.pPusher->push(4, subchannel +
                         NV097_SET_TEXGEN_R(0) | 0x40000);
    vpp.pPusher->push(5, NV097_SET_TEXGEN_R_V_DISABLE);

    vpp.pPusher->push(6, subchannel +
                         NV097_SET_TEXGEN_Q(0) | 0x40000);
    vpp.pPusher->push(7, NV097_SET_TEXGEN_Q_V_DISABLE);

    vpp.pPusher->push(8, subchannel +
                         NV097_SET_TEXGEN_S(1) | 0x40000);
    vpp.pPusher->push(9, NV097_SET_TEXGEN_S_V_DISABLE);

    vpp.pPusher->push(10, subchannel +
                          NV097_SET_TEXGEN_T(1) | 0x40000);
    vpp.pPusher->push(11, NV097_SET_TEXGEN_T_V_DISABLE);

    vpp.pPusher->push(12, subchannel +
                          NV097_SET_TEXGEN_R(1) | 0x40000);
    vpp.pPusher->push(13, NV097_SET_TEXGEN_R_V_DISABLE);

    vpp.pPusher->push(14, subchannel +
                          NV097_SET_TEXGEN_Q(1) | 0x40000);
    vpp.pPusher->push(15, NV097_SET_TEXGEN_Q_V_DISABLE);
    vpp.pPusher->adjust(16);

    // PG: This part until the fog parameters is new
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_SWATH_WIDTH | 0x40000);
    vpp.pPusher->push(1, NV097_SET_SWATH_WIDTH_V_OFF);

    vpp.pPusher->push(2, subchannel +
                         NV097_SET_SHADER_STAGE_PROGRAM | 0x40000);
    vpp.pPusher->push(3, ((NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PROGRAM_NONE << 15) |
                          (NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_PROGRAM_NONE << 10) |
                          (NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_2D_PROJECTIVE << 5) |
                           NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_2D_PROJECTIVE));

    vpp.pPusher->push(4, subchannel +
                         NV097_SET_SHADER_CLIP_PLANE_MODE | 0x40000);
    vpp.pPusher->push(5, 0);

    vpp.pPusher->push(6, subchannel +
                         NV097_SET_SHADER_OTHER_STAGE_INPUT | 0x40000);
    vpp.pPusher->push(7, 0);

    // This is set once and forgotten. It puts the class in OGL flat shade mode.
    vpp.pPusher->push(8, subchannel +
                         NV097_SET_FLAT_SHADE_OP | 0x40000);
    vpp.pPusher->push(9, NV097_SET_FLAT_SHADE_OP_V_LAST_VTX);
    vpp.pPusher->adjust(10);

    // Set default fog parameters
    // PG - Modified - 11/03/2000
    fBuffer[0] = 0.0;   // K0
    fBuffer[1] = 1.0;   // K1
    //fBuffer[2] = 0.0;   // K2

    // PG - Modified 10/31/2000 - [0][1][2]
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_FOG_PARAMS(0) | 0xC0000);
    vpp.pPusher->push(1, ftol[1]); // K0
    vpp.pPusher->push(2, ftol[1]); // K1
    vpp.pPusher->push(3, ftol[0]); // K2

    // Set fog plane
    vpp.pPusher->push(4, subchannel +
                         NV097_SET_FOG_PLANE(0) | 0x100000);
    vpp.pPusher->push(5, ftol[0]);
    vpp.pPusher->push(6, ftol[0]);
    vpp.pPusher->push(7, ftol[1]);
    vpp.pPusher->push(8, ftol[0]);

    // Set default fog color
    vpp.pPusher->push(9, subchannel +
                         NV097_SET_FOG_COLOR | 0x40000);
    vpp.pPusher->push(10, 0xFFFFFFFF);
    vpp.pPusher->adjust(11);

    // Set color for MODULATE blend mode
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_DIFFUSE_COLOR4F(0) | 0x100000);
    vpp.pPusher->push(1, ftol[1]);
    vpp.pPusher->push(2, ftol[1]);
    vpp.pPusher->push(3, ftol[1]);
    vpp.pPusher->push(4, ftol[1]);

    vpp.pPusher->push(5, subchannel +
                         NV097_SET_SPECULAR_COLOR3F(0) | 0xC0000);
    vpp.pPusher->push(6, ftol[0]);
    vpp.pPusher->push(7, ftol[0]);
    vpp.pPusher->push(8, ftol[0]);

    vpp.pPusher->push(9, subchannel +
                         NV097_SET_TEXCOORD0_4F(0) | 0x100000);
    vpp.pPusher->push(10, ftol[0]);
    vpp.pPusher->push(11, ftol[0]);
    vpp.pPusher->push(12, ftol[0]);
    vpp.pPusher->push(13, ftol[1]);

    vpp.pPusher->push(14, subchannel +
                          NV097_SET_TEXCOORD1_4F(0) | 0x100000);
    vpp.pPusher->push(15, ftol[0]);
    vpp.pPusher->push(16, ftol[0]);
    vpp.pPusher->push(17, ftol[0]);
    vpp.pPusher->push(18, ftol[1]);

    vpp.pPusher->push(19, subchannel +
                          NV097_SET_NORMAL3F(0) | 0xC0000);
    vpp.pPusher->push(20, ftol[0]);
    vpp.pPusher->push(21, ftol[0]);
    vpp.pPusher->push(22, ftol[1]);

    vpp.pPusher->push(23, subchannel +
                          NV097_SET_FOG1F | 0x40000);
    vpp.pPusher->push(24, ftol[0]);

    vpp.pPusher->push(25, subchannel +
                          NV097_SET_WEIGHT1F | 0x40000);
    vpp.pPusher->push(26, ftol[1]);
    vpp.pPusher->adjust(27);

    // Load texture matrices assuming quarter pel scaling of texture coordinates
    fBuffer[0]  = 0.25; fBuffer[1]  = 0.0;  fBuffer[2]  = 0.0; fBuffer[3]  = 0.0;
    fBuffer[4]  = 0.0;  fBuffer[5]  = 0.25; fBuffer[6]  = 0.0; fBuffer[7]  = 0.0;
    fBuffer[8]  = 0.0;  fBuffer[9]  = 0.0;  fBuffer[10] = 1.0; fBuffer[11] = 0.0;
    fBuffer[12] = 0.0;  fBuffer[13] = 0.0;  fBuffer[14] = 0.0; fBuffer[15] = 1.0;

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_TEXTURE_MATRIX0(0) | 0x400000);
    vpp.pPusher->push(1, ftol[0]);
    vpp.pPusher->push(2, ftol[4]);
    vpp.pPusher->push(3, ftol[8]);
    vpp.pPusher->push(4, ftol[12]);
    vpp.pPusher->push(5, ftol[1]);
    vpp.pPusher->push(6, ftol[5]);
    vpp.pPusher->push(7, ftol[9]);
    vpp.pPusher->push(8, ftol[13]);
    vpp.pPusher->push(9, ftol[2]);
    vpp.pPusher->push(10, ftol[6]);
    vpp.pPusher->push(11, ftol[10]);
    vpp.pPusher->push(12, ftol[14]);
    vpp.pPusher->push(13, ftol[3]);
    vpp.pPusher->push(14, ftol[7]);
    vpp.pPusher->push(15, ftol[11]);
    vpp.pPusher->push(16, ftol[15]);

    vpp.pPusher->push(17, subchannel +
                          NV097_SET_TEXTURE_MATRIX1(0) | 0x400000);
    vpp.pPusher->push(18, ftol[0]);
    vpp.pPusher->push(19, ftol[4]);
    vpp.pPusher->push(20, ftol[8]);
    vpp.pPusher->push(21, ftol[12]);
    vpp.pPusher->push(22, ftol[1]);
    vpp.pPusher->push(23, ftol[5]);
    vpp.pPusher->push(24, ftol[9]);
    vpp.pPusher->push(25, ftol[13]);
    vpp.pPusher->push(26, ftol[2]);
    vpp.pPusher->push(27, ftol[6]);
    vpp.pPusher->push(28, ftol[10]);
    vpp.pPusher->push(29, ftol[14]);
    vpp.pPusher->push(30, ftol[3]);
    vpp.pPusher->push(31, ftol[7]);
    vpp.pPusher->push(32, ftol[11]);
    vpp.pPusher->push(33, ftol[15]);
    vpp.pPusher->adjust(34);

    vpp.pPusher->start(TRUE);

    // Disable logic ops
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_LOGIC_OP_ENABLE | 0x40000);
    vpp.pPusher->push(1, NV097_SET_LOGIC_OP_ENABLE_V_FALSE);

    // Disable Z min/max culling
    vpp.pPusher->push(2, subchannel +
                         NV097_SET_ZMIN_MAX_CONTROL | 0x40000);
    vpp.pPusher->push(3, ((NV097_SET_ZMIN_MAX_CONTROL_CULL_IGNORE_W_TRUE << 8) |
                          (NV097_SET_ZMIN_MAX_CONTROL_ZCLAMP_EN_CLAMP << 4) |
                           NV097_SET_ZMIN_MAX_CONTROL_CULL_NEAR_FAR_EN_FALSE));

    // PG - New 10/31/2000
    // MUST set antialiasing control sample mask even though antialiasing is disabled
    // Disable antialiasing
    vpp.pPusher->push(4, subchannel +
                         NV097_SET_ANTI_ALIASING_CONTROL | 0x40000);
    vpp.pPusher->push(5, ((0xFFFF0000) |
                         NV097_SET_ANTI_ALIASING_CONTROL_ENABLE_FALSE));

    // Disable z buffer compression
    vpp.pPusher->push(6, subchannel +
                         NV097_SET_COMPRESS_ZBUFFER_EN | 0x40000);
    vpp.pPusher->push(7, NV097_SET_COMPRESS_ZBUFFER_EN_V_DISABLE);

    // Disable z stencil occlusion
    vpp.pPusher->push(8, subchannel +
                         NV097_SET_OCCLUDE_ZSTENCIL_EN | 0x40000);
    vpp.pPusher->push(9, NV097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_ZEN_DISABLE);

    // Disable two sided lighting
    vpp.pPusher->push(10, subchannel +
                          NV097_SET_TWO_SIDE_LIGHT_EN | 0x40000);
    vpp.pPusher->push(11, NV097_SET_TWO_SIDE_LIGHT_EN_V_FALSE);

    // Disable z pass pixel counting
    vpp.pPusher->push(12, subchannel +
                          NV097_SET_ZPASS_PIXEL_COUNT_ENABLE | 0x40000);
    vpp.pPusher->push(13, NV097_SET_ZPASS_PIXEL_COUNT_ENABLE_V_FALSE);
    vpp.pPusher->adjust(14);

    vpp.pPusher->start(TRUE);

    // Initialize some vertex attributes
    fBuffer[0] = 0.0;
    fBuffer[1] = 1.0;

    // Enable both texture matrices since these texture coordinates are in quarter pel
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_TEXTURE_MATRIX_ENABLE(0) | 0x100000);
    vpp.pPusher->push(1, NV097_SET_TEXTURE_MATRIX_ENABLE_V_TRUE);
    vpp.pPusher->push(2, NV097_SET_TEXTURE_MATRIX_ENABLE_V_TRUE);
    vpp.pPusher->push(3, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);
    vpp.pPusher->push(4, NV097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE);
    vpp.pPusher->adjust(5);

    tmpVal = (1 << 16) | // 1 MIPMAP level
             (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8 << 8) |
             (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) |
             (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) |
             (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) |
              NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A;

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_TEXTURE_FORMAT(0) | 0x40000);
    vpp.pPusher->push(1, tmpVal);
    vpp.pPusher->push(2, subchannel +
                         NV097_SET_TEXTURE_FORMAT(1) | 0x40000);
    vpp.pPusher->push(3, tmpVal);
    vpp.pPusher->push(4, subchannel +
                         NV097_SET_TEXTURE_FORMAT(2) | 0x40000);
    vpp.pPusher->push(5, tmpVal);
    vpp.pPusher->push(6, subchannel +
                         NV097_SET_TEXTURE_FORMAT(3) | 0x40000);
    vpp.pPusher->push(7, tmpVal);

    // PG - Added 10/31/2000
    tmpVal = (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_Q_FALSE << 24) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_P_FALSE << 20) |
             (NV097_SET_TEXTURE_ADDRESS_P_CLAMP_TO_EDGE << 16) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_V_FALSE << 12) |
             (NV097_SET_TEXTURE_ADDRESS_V_CLAMP_TO_EDGE << 8) |
             (NV097_SET_TEXTURE_ADDRESS_CYLWRAP_U_FALSE << 4) |
              NV097_SET_TEXTURE_ADDRESS_U_CLAMP_TO_EDGE;

    vpp.pPusher->push(8, subchannel +
                         NV097_SET_TEXTURE_ADDRESS(0) | 0x40000);
    vpp.pPusher->push(9, tmpVal);
    vpp.pPusher->push(10, subchannel +
                          NV097_SET_TEXTURE_ADDRESS(1) | 0x40000);
    vpp.pPusher->push(11, tmpVal);
    vpp.pPusher->push(12, subchannel +
                          NV097_SET_TEXTURE_ADDRESS(2) | 0x40000);
    vpp.pPusher->push(13, tmpVal);
    vpp.pPusher->push(14, subchannel +
                          NV097_SET_TEXTURE_ADDRESS(3) | 0x40000);
    vpp.pPusher->push(15, tmpVal);
    vpp.pPusher->adjust(16);

    tmpVal = (NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
             (15 << 18) | // MIN_LOD_CLAMP == 0.9
             (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
             (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
             (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
             (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
              NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_TEXTURE_CONTROL0(0) | 0x40000);
    vpp.pPusher->push(1, tmpVal);

    // PG - Modified 11/02/2000 - ANISO_1
    tmpVal = (NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE << 30) |
        (15 << 18) | // MIN_LOD_CLAMP == 0.9
        (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
        (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_1 << 4) |
        (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
        (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
        NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    vpp.pPusher->push(2, subchannel +
                         NV097_SET_TEXTURE_CONTROL0(1) | 0x40000);
    vpp.pPusher->push(3, tmpVal);

    tmpVal = (NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE << 30) |
             (15 << 18) | // MIN_LOD_CLAMP == 0.9
             (15 << 6) | // default MAX_LOD_CLAMP from OGL == 0.9 (no clamping)
             (NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0 << 4) |
             (NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE << 3) |
             (NV097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE << 2) |
              NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE;

    vpp.pPusher->push(4, subchannel +
                         NV097_SET_TEXTURE_CONTROL0(2) | 0x40000);
    vpp.pPusher->push(5, tmpVal);
    vpp.pPusher->push(6, subchannel +
                         NV097_SET_TEXTURE_CONTROL0(3) | 0x40000);
    vpp.pPusher->push(7, tmpVal);


    tmpVal = (NV097_SET_TEXTURE_FILTER_BSIGNED_BIT_DISABLED << 31) |
             (NV097_SET_TEXTURE_FILTER_GSIGNED_BIT_DISABLED << 30) |
             (NV097_SET_TEXTURE_FILTER_RSIGNED_BIT_DISABLED << 29) |
             (NV097_SET_TEXTURE_FILTER_ASIGNED_BIT_DISABLED << 28) |
             (NV097_SET_TEXTURE_FILTER_MAG_TENT_LOD0 << 24) |
             (NV097_SET_TEXTURE_FILTER_MIN_TENT_LOD0 << 16) |
             (NV097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX << 13);

    vpp.pPusher->push(8, subchannel +
                         NV097_SET_TEXTURE_FILTER(0) | 0x40000);
    vpp.pPusher->push(9, tmpVal);
    vpp.pPusher->push(10, subchannel +
                          NV097_SET_TEXTURE_FILTER(1) | 0x40000);
    vpp.pPusher->push(11, tmpVal);
    vpp.pPusher->push(12, subchannel +
                          NV097_SET_TEXTURE_FILTER(2) | 0x40000);
    vpp.pPusher->push(13, tmpVal);
    vpp.pPusher->push(14, subchannel +
                          NV097_SET_TEXTURE_FILTER(3) | 0x40000);
    vpp.pPusher->push(15, tmpVal);

    tmpVal = (2044 << 16) | 2046; // Must be even and <= 4094

    vpp.pPusher->push(16, subchannel +
                          NV097_SET_TEXTURE_IMAGE_RECT(0) | 0x40000);
    vpp.pPusher->push(17, tmpVal);
    vpp.pPusher->push(18, subchannel +
                          NV097_SET_TEXTURE_IMAGE_RECT(1) | 0x40000);
    vpp.pPusher->push(19, tmpVal);
    vpp.pPusher->push(20, subchannel +
                          NV097_SET_TEXTURE_IMAGE_RECT(2) | 0x40000);
    vpp.pPusher->push(21, tmpVal);
    vpp.pPusher->push(22, subchannel +
                          NV097_SET_TEXTURE_IMAGE_RECT(3) | 0x40000);
    vpp.pPusher->push(23, tmpVal);

    vpp.pPusher->adjust(24);

    // Now setup combiners

    // Default register combiner settings are:
    // A = primary color
    // B = one
    // C = zero
    // D = zero

    // Our temporal filter moComp values
    // combiner0, RGB portion:   A = constantColor0
    // combiner0, RGB portion:   B = texture0
    // combiner0, RGB portion:   C = constantColor1
    // combiner0, RGB portion:   D = texture1

    // Initialize combiner ICW methods
    vpp.pPusher->push(0, subchannel | NV097_SET_COMBINER_ALPHA_ICW(0) | 0x100000);
    vpp.pPusher->push(1, (NV097_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                         (NV097_SET_COMBINER_ALPHA_ICW_A_ALPHA_TRUE << 28) |
                         (NV097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1 << 24) |

                         (NV097_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                         (NV097_SET_COMBINER_ALPHA_ICW_B_ALPHA_TRUE << 20) |
                         (NV097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_8 << 16) |

                         (NV097_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_INVERT << 13) |
                         (NV097_SET_COMBINER_ALPHA_ICW_C_ALPHA_TRUE << 12) |
                         (NV097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_1 << 8) |

                         (NV097_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                         (NV097_SET_COMBINER_ALPHA_ICW_D_ALPHA_TRUE << 4) |
                          NV097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_9);
    vpp.pPusher->push(2, DEFAULT_KELVIN_ALPHA_ICW);
    vpp.pPusher->push(3, DEFAULT_KELVIN_ALPHA_ICW);
    vpp.pPusher->push(4, DEFAULT_KELVIN_ALPHA_ICW);

    vpp.pPusher->push(5, subchannel | NV097_SET_COMBINER_COLOR_ICW(0) | 0x100000);
    vpp.pPusher->push(6, (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                         (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                         (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                         (NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                         (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                         (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                         (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_INVERT << 13) |
                         (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                         (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1 << 8) |

                         (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                         (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                          NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9);
    vpp.pPusher->push(7, DEFAULT_KELVIN_COLOR_ICW);
    vpp.pPusher->push(8, DEFAULT_KELVIN_COLOR_ICW);
    vpp.pPusher->push(9, DEFAULT_KELVIN_COLOR_ICW);
    vpp.pPusher->adjust(10);

    // Load combination factors
    vpp.pPusher->push(0, subchannel | NV097_SET_COMBINER_FACTOR0(0) | 0x40000);
    vpp.pPusher->push(1, 0x80808080); // % of top field

    vpp.pPusher->push(2, subchannel | NV097_SET_COMBINER_FACTOR1(0) | 0x40000);
    vpp.pPusher->push(3, 0x80808080); // % of bottom field
    vpp.pPusher->adjust(4);

    // combiner0, RGB portion:   texture0 + texture1
    // spare0 = A * B + C * D
    // Initialize combiner OCW methods
    // combiner0, RGB portion:      spare0 = A * B + C * D = texture0 + texture1

    // Initialize combiner OCW methods
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_COMBINER_ALPHA_OCW(0) | 0x100000);
    vpp.pPusher->push(1, DEFAULT_KELVIN_ALPHA_OCW);
    vpp.pPusher->push(2, DEFAULT_KELVIN_ALPHA_OCW);
    vpp.pPusher->push(3, DEFAULT_KELVIN_ALPHA_OCW);
    vpp.pPusher->push(4, DEFAULT_KELVIN_ALPHA_OCW);
    vpp.pPusher->adjust(5);

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_COMBINER_COLOR_OCW(0) | 0x100000);
    vpp.pPusher->push(1, DEFAULT_KELVIN_COLOR_OCW);
    vpp.pPusher->push(2, DEFAULT_KELVIN_COLOR_OCW);
    vpp.pPusher->push(3, DEFAULT_KELVIN_COLOR_OCW);
    vpp.pPusher->push(4, DEFAULT_KELVIN_COLOR_OCW);
    vpp.pPusher->adjust(5);

    tmpVal = (NV097_SET_COMBINER_CONTROL_FACTOR1_SAME_FACTOR_ALL << 16) |
             (NV097_SET_COMBINER_CONTROL_FACTOR0_SAME_FACTOR_ALL << 12) |
             (NV097_SET_COMBINER_CONTROL_MUX_SELECT_MSB << 8) |
              NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_ONE;

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_COMBINER_CONTROL | 0x40000);
    vpp.pPusher->push(1, tmpVal);
    vpp.pPusher->adjust(2);

    // Load final stages with default values
    // Initialize combiner final CW methods
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_COMBINER_SPECULAR_FOG_CW0 | 0x40000);
    vpp.pPusher->push(1, DEFAULT_KELVIN_FINAL_CW0);
    vpp.pPusher->push(2, subchannel +
                         NV097_SET_COMBINER_SPECULAR_FOG_CW1 | 0x40000);
    vpp.pPusher->push(3, DEFAULT_KELVIN_FINAL_CW1);
    vpp.pPusher->adjust(4);

    vpp.pPusher->start(TRUE);

    return TRUE;
}


//---------------------------------------------------------------------------
// vppValidateMode
//      Determine whether there is enough memory for this mode
LOCAL BOOL vppValidateMode(Vpp_t *pVpp, DWORD dwWidth, DWORD dwHeight, DWORD dwBytesPerPel,
                            DWORD dwNumSurfaces, DWORD dwHead, DWORD dwRefreshRate, DWORD dwDCHead, DWORD dwDeviceType)
{
    NV_CFGEX_GET_SURFACE_PITCH_PARAMS params = {dwWidth, dwBytesPerPel * 8, 0};
    DWORD dwPitch, dwSize;
    Vpp_t &vpp = *pVpp;

    // get the pitch

    if (NvRmConfigGetEx(vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwDeviceHandle,
                        NV_CFGEX_GET_SURFACE_PITCH,
                        &params, sizeof(params)) == NVOS_CGE_STATUS_SUCCESS) {
        dwPitch = params.Pitch;
    } else {
        // RM call failed, try to figure it out ourselves
        dwPitch  = ((dwWidth * dwBytesPerPel) + vpp.pDriverData->dwSurfaceAlignPad) & ~vpp.pDriverData->dwSurfaceAlignPad;
    }

    // calculate total size
    dwSize = dwPitch * dwHeight * dwNumSurfaces;

    // enough memory?
    if (dwSize > NV_GET_VIDEO_HEAP_FREE()) {
        return FALSE;
    }

#if (IS_WINNT4 || IS_WINNT5)
    MODE_ENTRY *lpMode = vpp.m_ppdev->PhysModeList[dwHead];
    DWORD dwNumModes = vpp.m_ppdev->ulNumPhysModes[dwHead];
    for (DWORD i = 0; i < dwNumModes; i++) {
        if (lpMode->ValidMode && lpMode->Width == dwWidth && lpMode->Height == dwHeight &&
            lpMode->Depth == (dwBytesPerPel * 8) && lpMode->RefreshRate == dwRefreshRate) {
            return TRUE;
        }
        lpMode++;
    }
    return FALSE;
#else
    // validate whether that display mode exists or not
    NVTWINVIEWDATA twinData;
    twinData.dwFlag   = NVTWINVIEW_FLAG_GET;
    twinData.dwAction = NVTWINVIEW_ACTION_VALIDATE_PHYSICAL_MODE;
    twinData.dwCrtc   = dwHead;
    twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceType = dwDeviceType;
    twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwXRes = dwWidth;
    twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwYRes = dwHeight;
    twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwBpp  = dwBytesPerPel * 8;
    twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwRefresh = dwRefreshRate;
    MyExtEscape (dwDCHead, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA),
                 (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);

    if (!twinData.dwSuccess) {
        return FALSE;
    }
#endif

    return TRUE;
}


//---------------------------------------------------------------------------
// vppCreateFSMirror
//      Initialize for fullscreen mirror mode
extern BOOL VppCreateFSMirror(Vpp_t *pVpp,  DWORD dwSrcWidth, DWORD dwSrcHeight)
{
    nvAssert(pVpp);
    nvAssert(dwSrcWidth>0);
    nvAssert(dwSrcHeight>0);

    // During modeset on win9x, this is called before enable (in nvEnable)
    //   Is that really necessary? @mjl@
    if (!(pVpp->dwFlags & VPP_FLAG_ENABLED))
    {
        return FALSE;
    }

    nvAssert(!(pVpp->dwFlags & VPP_FLAG_MIRROR_READY));

#ifdef IS_WINNT4    // No FullScreen mirror on NT4
    return FALSE;
#else
    DWORD i, dwStatus, dwBlockSize;
    NVTWINVIEWDATA twinData = {0};
    DWORD dwCrtcNumber = 0;
    DWORD dwDeviceType = 0;
    DWORD dwDCHead;
    DWORD dwPhysicalHead = 0;
    Vpp_t &vpp = *pVpp;
    LPPROCESSINFO lpProcInfo;

#if IS_WINNT5
    if (vpp.m_ppdev->TwinView_State != NVTWINVIEW_STATE_CLONE) {
        return FALSE;
    }
#elif IS_WIN9X
    if (vpp.pDriverData->dwDesktopState != NVTWINVIEW_STATE_DUALVIEW &&
        vpp.pDriverData->dwDesktopState != NVTWINVIEW_STATE_CLONE) {
        return FALSE;
    }
#endif

    // overlay creation has succeeded, now attempt to create fullscreen mirrors
    if ((vpp.pDriverData->dwHeads > 1) &&       
        (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK) &&
        (vpp.dwOverlayFSNumSurfaces == 0)) {
        DWORD dwAdapterIndex = 0xFFFFFFFF;

        vpp.dwOverlayFSHead = (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK) - 1;

        if (vpp.pDriverData->dwDesktopState == NVTWINVIEW_STATE_DUALVIEW) {
            vpp.dwOverlayFSHead = 14;
            vpp.regOverlayMode2 |= NV4_REG_OVL_MODE2_FSDEVICEMASK;  // set to autoselect
        }

        if (vpp.dwOverlayFSHead >= 14) {
            // auto head selection
            switch (vpp.pDriverData->dwDesktopState) {
            case NVTWINVIEW_STATE_DUALVIEW:
                // multimon mode uses the head which the overlay was not created on
                if (vpp.dwOverlayFSOvlHead == 0) {
                    vpp.dwOverlayFSHead = 1;
                } else if (vpp.dwOverlayFSOvlHead == 1) {
                    vpp.dwOverlayFSHead = 0;
                } else {
                    // no overlay, pick head 1
                    vpp.dwOverlayFSHead = 1;
                }
                break;
            case NVTWINVIEW_STATE_CLONE:
                // clone mode preferentially uses head 1
                vpp.dwOverlayFSHead = 1;
                break;
            default:
                // invalid condition, fail
                vpp.regOverlayMode2 &= ~NV4_REG_OVL_MODE2_FSMASK;
                return FALSE;
            }
        }

#if IS_WIN9X

        switch (vpp.pDriverData->dwDesktopState) {
        case NVTWINVIEW_STATE_DUALVIEW:
            dwCrtcNumber   = 0;       // need to use logical head numbers in multimon mode
            dwDCHead       = vpp.dwOverlayFSHead;
            dwPhysicalHead = vpp.dwOverlayFSHead;
            break;
        case NVTWINVIEW_STATE_CLONE:
            dwCrtcNumber = vpp.dwOverlayFSHead;
            dwDCHead     = pDXShare->dwHeadNumber;
            // get physical head number
            twinData.dwFlag   = NVTWINVIEW_FLAG_GET;
            twinData.dwAction = NVTWINVIEW_ACTION_MAP_LOG_TO_PHYS_HEAD;
            twinData.dwCrtc   = dwCrtcNumber;
            MyExtEscape (dwDCHead, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA),
                         (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);
            dwPhysicalHead = twinData.dwCrtc;
            break;
        default:
            // invalid condition, fail
            vpp.regOverlayMode2 &= ~NV4_REG_OVL_MODE2_FSMASK;
            return FALSE;
        }

        // get the current device type
        vpp.dwOverlayFSTwinData.dwFlag   = NVTWINVIEW_FLAG_GET;
        vpp.dwOverlayFSTwinData.dwAction = NVTWINVIEW_ACTION_SETGET_DEVICE_TYPE;
        vpp.dwOverlayFSTwinData.dwCrtc   = dwCrtcNumber;
        MyExtEscape (dwDCHead, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPCSTR) &vpp.dwOverlayFSTwinData,
                     sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &vpp.dwOverlayFSTwinData);
        twinData = vpp.dwOverlayFSTwinData;
        dwDeviceType = twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceType;

#elif IS_WINNT5
        vpp.dwOverlayFSHead = vpp.m_ppdev->ulDeviceDisplay[vpp.dwOverlayFSHead];
        dwDeviceType = vpp.m_ppdev->ulDeviceType[vpp.dwOverlayFSHead];
        dwDCHead = 0;
#endif

        if (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSSETMODE) {
            // driver will choose the display mode
            DWORD dwEncoderType;
            BOOL isDVDSized, isTV;
            NvRmConfigGet(
                vpp.pDriverData->dwRootHandle,
                vpp.pDriverData->dwDeviceHandle,
                     NV_CFG_VIDEO_ENCODER_TYPE, &dwEncoderType);

    // determine optimal mode
            // TODO: can do some interesting optimizations for anamorphic mode

            isTV = (dwDeviceType == MONITOR_TYPE_NTSC || dwDeviceType == MONITOR_TYPE_PAL);
            isDVDSized = (dwSrcWidth == 720 || dwSrcWidth == 704);

            if (isTV) {
                // if monitor is a TV, then attempt to set the native resolution
                if (dwEncoderType == NV_ENCODER_BROOKTREE_869 ||
                    dwEncoderType == NV_ENCODER_BROOKTREE_868) {
                    // Bt868/9 needs some weird modes because of locked and incorrect aspect ratios on those chips
                    if (dwDeviceType == MONITOR_TYPE_NTSC) {
                        vpp.dwOverlayFSWidth  = 720;
                        vpp.dwOverlayFSHeight = 512;
                        vpp.dwOverlayFSFormat = 4;
                        if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                            goto vppFSMirrorModeFound;
                        }

                        vpp.dwOverlayFSWidth  = 640;
                        vpp.dwOverlayFSHeight = 480;
                        vpp.dwOverlayFSFormat = 4;
                        if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                            goto vppFSMirrorModeFound;
                        }
                    }

                    if (dwDeviceType == MONITOR_TYPE_PAL) {
                        vpp.dwOverlayFSWidth  = 720;
                        vpp.dwOverlayFSHeight = 544;
                        vpp.dwOverlayFSFormat = 4;
                        if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                            goto vppFSMirrorModeFound;
                        }
                        vpp.dwOverlayFSWidth  = 800;
                        vpp.dwOverlayFSHeight = 600;
                        vpp.dwOverlayFSFormat = 4;
                        if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                            goto vppFSMirrorModeFound;
                        }
                    }
                } else if (dwEncoderType == NV_ENCODER_CHRONTEL_7007 ||
                           dwEncoderType == NV_ENCODER_CHRONTEL_7008) {
                    // optimal mode for Chrontel is 720x576
                    vpp.dwOverlayFSWidth  = 720;
                    vpp.dwOverlayFSHeight = 576;
                    vpp.dwOverlayFSFormat = 4;
                    if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                        goto vppFSMirrorModeFound;
                    }
                }
                // Note: if TV encoder is Brooktree/Conexant 871, we fall throught into the following
                // code, which handles more normal video mirror resolutions.
            }

            // Under some circumstances we use the source size, but not for TV.
            // Note 320x240 mode doesn't look right on TV.
            if (!isTV && !((dwDeviceType == MONITOR_TYPE_FLAT_PANEL) && isDVDSized)) {
                // first try a mode exactly equal to source size
                vpp.dwOverlayFSWidth  = dwSrcWidth;
                vpp.dwOverlayFSHeight = dwSrcHeight;
                vpp.dwOverlayFSFormat = 4;
                if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                    goto vppFSMirrorModeFound;
                }
            }

            // select mode based on source size
            // Note 320x240 mode doesn't look right on TV.
            if (dwSrcWidth <= 320 && dwSrcHeight <= 240 && !isTV) {
                vpp.dwOverlayFSWidth  = 320;
                vpp.dwOverlayFSHeight = 240;
                vpp.dwOverlayFSFormat = 4;
                if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                    goto vppFSMirrorModeFound;
                }
            // Note 512x384 mode doesn't look right on TV.
            } else if (dwSrcWidth <= 512 && dwSrcHeight <= 384 && !isTV) {
                vpp.dwOverlayFSWidth  = 512;
                vpp.dwOverlayFSHeight = 384;
                vpp.dwOverlayFSFormat = 4;
                if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                    goto vppFSMirrorModeFound;
                }
            } else if (dwSrcWidth <= 640 && dwSrcHeight <= 480) {
                vpp.dwOverlayFSWidth  = 640;
                vpp.dwOverlayFSHeight = 480;
                vpp.dwOverlayFSFormat = 4;
                if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                    goto vppFSMirrorModeFound;
                }
            } else if (dwSrcWidth <= 800 && dwSrcHeight <= 600) {
                vpp.dwOverlayFSWidth  = 800;
                vpp.dwOverlayFSHeight = 600;
                vpp.dwOverlayFSFormat = 4;
                if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                    goto vppFSMirrorModeFound;
                }
            } else if (dwSrcWidth <= 1024 && dwSrcHeight <= 768) {
                vpp.dwOverlayFSWidth  = 1024;
                vpp.dwOverlayFSHeight = 768;
                vpp.dwOverlayFSFormat = 4;
                if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                    goto vppFSMirrorModeFound;
                }
            } else if (dwSrcWidth <= 1280 && dwSrcHeight <= 960) {
                vpp.dwOverlayFSWidth  = 1280;
                vpp.dwOverlayFSHeight = 960;
                vpp.dwOverlayFSFormat = 4;
                if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                    goto vppFSMirrorModeFound;
                }
            } else if (dwSrcWidth <= 1600 && dwSrcHeight <= 1200) {
                vpp.dwOverlayFSWidth  = 1600;
                vpp.dwOverlayFSHeight = 1200;
                vpp.dwOverlayFSFormat = 4;
                if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                    goto vppFSMirrorModeFound;
                }
            }
            // ok, we must be running into memory problems by now, try modes smaller that overlay source and downscale
            vpp.dwOverlayFSWidth  = 800;
            vpp.dwOverlayFSHeight = 600;
            vpp.dwOverlayFSFormat = 4;
            if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                goto vppFSMirrorModeFound;
            }

            // Try 8x6x32 with 2 buffers.
            if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 2, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                goto vppFSMirrorModeFound;
            }

            vpp.dwOverlayFSWidth  = 640;
            vpp.dwOverlayFSHeight = 480;
            vpp.dwOverlayFSFormat = 4;
            if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                goto vppFSMirrorModeFound;
            }

            // Try 6x4x32 with 2 buffers.
            if (vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 2, dwCrtcNumber, 60, dwDCHead, dwDeviceType)) {
                goto vppFSMirrorModeFound;
            }

            // last resort mode
            vpp.dwOverlayFSWidth  = 640;
            vpp.dwOverlayFSHeight = 480;
            vpp.dwOverlayFSFormat = 2;

            vppFSMirrorModeFound:
#if IS_WINNT5
            RESOLUTION_INFO res;
            BOOL success;

            // WIN2k works a little differently that win9x escape, it won't back down to a lower refresh
            // automatically so we have to do the explicit refresh rate validation
            if (dwDeviceType == MONITOR_TYPE_NTSC) {
                res.ulDisplayRefresh = 60;
            } else if (dwDeviceType == MONITOR_TYPE_PAL) {
                res.ulDisplayRefresh = 50;
            } else {
                res.ulDisplayRefresh = 85;
                if (!vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 85, dwDCHead, dwDeviceType)) {
                    res.ulDisplayRefresh = 75;
                    if (!vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 75, dwDCHead, dwDeviceType)) {
                        res.ulDisplayRefresh = 60;
                    }
                }
            }

            res.ulDisplayWidth      = vpp.dwOverlayFSWidth;
            res.ulDisplayHeight     = vpp.dwOverlayFSHeight;
            switch (vpp.dwOverlayFSFormat) {
            case 1:  res.ulDisplayPixelDepth = BMF_8BPP;  break;
            case 4:  res.ulDisplayPixelDepth = BMF_32BPP; break;
            default: res.ulDisplayPixelDepth = BMF_16BPP; break;
            }
            success = NvSetDac(vpp.m_ppdev, vpp.dwOverlayFSHead, &res);
#elif IS_WIN9X
            // lock the head CRTC
            // do not touch pDriverData->dwOverlayFSTwinData anymore until the CRTC unlock, display driver needs this context for unlocking
            vpp.dwOverlayFSTwinData.dwFlag   = NVTWINVIEW_FLAG_SET;
            vpp.dwOverlayFSTwinData.dwAction = NVTWINVIEW_ACTION_LOCK_HWCRTC;
            vpp.dwOverlayFSTwinData.dwCrtc   = dwPhysicalHead;   // leave this as the physical head number for lock
            MyExtEscape (pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPCSTR) &vpp.dwOverlayFSTwinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &vpp.dwOverlayFSTwinData);

            vpp.dwOverlayFSTwinData.nvtwPhysicalModeImmediate.pHWCrtcCX = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pHWCrtcCX;
            vpp.dwOverlayFSTwinData.nvtwPhysicalModeImmediate.pGNLogdevCX = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pGNLogdevCX;
            vpp.dwOverlayFSTwinData.nvtwPhysicalModeImmediate.dwLogicalCrtcIndex = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.dwLogicalCrtcIndex;
            vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pHWCrtcCX = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pHWCrtcCX;
            vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pGNLogdevCX = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pGNLogdevCX;
            vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.dwLogicalCrtcIndex = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.dwLogicalCrtcIndex;
            twinData = vpp.dwOverlayFSTwinData;

            if (!twinData.dwSuccess) {
                // lock failed, abort
                vpp.dwOverlayFSNumSurfaces = 0;
                return FALSE;
            }

            // set the mode
            twinData.dwFlag   = NVTWINVIEW_FLAG_SET;
            twinData.dwAction = NVTWINVIEW_ACTION_SETGET_PHYSICAL_MODE_IMMEDIATE;
            if (vpp.pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE) {
                twinData.dwCrtc   = dwPhysicalHead;
            } else {
                twinData.dwCrtc   = dwCrtcNumber;
            }
            twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwXRes = vpp.dwOverlayFSWidth;
            twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwYRes = vpp.dwOverlayFSHeight;
            twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwBpp  = vpp.dwOverlayFSFormat * 8;
            if (dwDeviceType == MONITOR_TYPE_NTSC) {
                twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwRefresh = 60;
            } else if (dwDeviceType == MONITOR_TYPE_PAL) {
                twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwRefresh = 50;
            } else {
                twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwRefresh = 85;
            }
            MyExtEscape (dwDCHead, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA),
                         (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);

            vpp.dwOverlayFSWidth  = twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwXRes;
            vpp.dwOverlayFSHeight = twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwYRes;
            vpp.dwOverlayFSFormat = (twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwBpp + 1) / 8;
#endif
        } else {
            // use whatever mode is currently set

#if IS_WINNT5
            vpp.dwOverlayFSWidth  = vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].right -
                                             vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].left;
            vpp.dwOverlayFSHeight = vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].bottom -
                                             vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].top;
            vpp.dwOverlayFSFormat = vpp.m_ppdev->cjPelSize;
            vpp.dwOverlayFSPitch  = vpp.m_ppdev->lDelta;

            if (vpp.dwOverlayFSFormat == 1) {
                RESOLUTION_INFO res;
                BOOL success;

                // if other head was set to 8 bpp, then actually use a 16 bpp FS surface
                vpp.dwOverlayFSFormat = 2;
                // WIN2k works a little differently that win9x escape, it won't back down to a lower refresh
                // automatically so we have to do the explicite refresh rate validation
                if (dwDeviceType == MONITOR_TYPE_NTSC) {
                    res.ulDisplayRefresh = 60;
                } else if (dwDeviceType == MONITOR_TYPE_PAL) {
                    res.ulDisplayRefresh = 50;
                } else {
                    res.ulDisplayRefresh = 85;
                    if (!vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 85, dwDCHead, dwDeviceType)) {
                        res.ulDisplayRefresh = 75;
                        if (!vppValidateMode(&vpp, vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, vpp.dwOverlayFSFormat, 3, dwCrtcNumber, 75, dwDCHead, dwDeviceType)) {
                            res.ulDisplayRefresh = 60;
                        }
                    }
                }

                // promote the mode
                res.ulDisplayWidth      = vpp.dwOverlayFSWidth;
                res.ulDisplayHeight     = vpp.dwOverlayFSHeight;
                res.ulDisplayPixelDepth = BMF_16BPP;
                success = NvSetDac(vpp.m_ppdev, vpp.dwOverlayFSHead, &res);
            }
#elif IS_WIN9X
            // get the physical mode
            twinData.dwFlag   = NVTWINVIEW_FLAG_GET;
            twinData.dwAction = NVTWINVIEW_ACTION_SETGET_PHYSICAL_MODE;
            twinData.dwCrtc   = dwCrtcNumber;
            MyExtEscape (dwDCHead, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA),
                         (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);

            vpp.dwOverlayFSWidth  = twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwXRes;
            vpp.dwOverlayFSHeight = twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwYRes;
            vpp.dwOverlayFSFormat = (twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwBpp + 1) / 8;

            // lock the head CRTC
            // do not touch pDriverData->dwOverlayFSTwinData anymore until the CRTC unlock, display driver needs this context for unlocking
            vpp.dwOverlayFSTwinData.dwFlag   = NVTWINVIEW_FLAG_SET;
            vpp.dwOverlayFSTwinData.dwAction = NVTWINVIEW_ACTION_LOCK_HWCRTC;
            vpp.dwOverlayFSTwinData.dwCrtc   = dwPhysicalHead;   // leave this as the physical head number for lock
            MyExtEscape (pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA),
                         (LPCSTR) &vpp.dwOverlayFSTwinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &vpp.dwOverlayFSTwinData);

            vpp.dwOverlayFSTwinData.nvtwPhysicalModeImmediate.pHWCrtcCX = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pHWCrtcCX;
            vpp.dwOverlayFSTwinData.nvtwPhysicalModeImmediate.pGNLogdevCX = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pGNLogdevCX;
            vpp.dwOverlayFSTwinData.nvtwPhysicalModeImmediate.dwLogicalCrtcIndex = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.dwLogicalCrtcIndex;
            vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pHWCrtcCX = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pHWCrtcCX;
            vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pGNLogdevCX = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pGNLogdevCX;
            vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.dwLogicalCrtcIndex = vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.dwLogicalCrtcIndex;
            twinData = vpp.dwOverlayFSTwinData;

            if (!twinData.dwSuccess) {
                // lock failed, abort
                vpp.dwOverlayFSNumSurfaces = 0;
                return FALSE;
            }

            if (vpp.dwOverlayFSFormat == 1) {
                // if other head was set to 8 bpp, then actually use a 16 bpp FS surface
                vpp.dwOverlayFSFormat = 2;

                twinData.dwFlag   = NVTWINVIEW_FLAG_SET;
                twinData.dwAction = NVTWINVIEW_ACTION_SETGET_PHYSICAL_MODE_IMMEDIATE;
                if (vpp.pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE) {
                    twinData.dwCrtc   = dwPhysicalHead;
                } else {
                    twinData.dwCrtc   = dwCrtcNumber;
                }
                twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwXRes = vpp.dwOverlayFSWidth;
                twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwYRes = vpp.dwOverlayFSHeight;
                twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwBpp  = 16;
                if (dwDeviceType == MONITOR_TYPE_NTSC) {
                    twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwRefresh = 60;
                } else if (dwDeviceType == MONITOR_TYPE_PAL) {
                    twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwRefresh = 50;
                } else {
                    twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwRefresh = 85;
                }
                MyExtEscape (dwDCHead, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA),
                             (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);

                vpp.dwOverlayFSWidth  = twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwXRes;
                vpp.dwOverlayFSHeight = twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwYRes;
                vpp.dwOverlayFSFormat = (twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwBpp + 1) / 8;
            }
#endif
        }

        // Get the pitch from the RM for a given width/bpp
        NV_CFGEX_GET_SURFACE_PITCH_PARAMS params = {vpp.dwOverlayFSWidth, vpp.dwOverlayFSFormat * 8, 0};
        if (
            NvRmConfigGetEx(
                vpp.pDriverData->dwRootHandle,
                vpp.pDriverData->dwDeviceHandle,
                NV_CFGEX_GET_SURFACE_PITCH,
                &params, sizeof(params)) == NVOS_CGE_STATUS_SUCCESS) {
            vpp.dwOverlayFSPitch = params.Pitch;
        } else {
            // RM call failed, try to figure it out ourselves
            vpp.dwOverlayFSPitch  = ((vpp.dwOverlayFSWidth * vpp.dwOverlayFSFormat) + vpp.pDriverData->dwSurfaceAlignPad) & ~vpp.pDriverData->dwSurfaceAlignPad;
        }

        // create offscreen FS mirror surfaces
        dwBlockSize = vpp.dwOverlayFSPitch * vpp.dwOverlayFSHeight;
        for (i=0; i<NV_MAX_EXTRA_FS_SURFACES; i++) {
            NVHEAP_ALLOC (dwStatus, vpp.dwOverlayFSOffset[i], dwBlockSize, TYPE_IMAGE);
            if (dwStatus==0)
            {
                vpp.dwOverlayFSNumSurfaces++;
                vpp.dwOverlayFSOffset[i] = VIDMEM_OFFSET(vpp.dwOverlayFSOffset[i]);

                // strange bug: running vppBltFillBlock to zero out the FS mirror surfaces
                // makes the VPE overlay (in nvpecore.c) nonfunctional. Workaround:
                // don't do the blit if VPE is initialized. -FCF
                if (!vpp.pDriverData->nvpeState.bVPEInitialized) {
                if (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSASPECT) {
                    // will be preserving aspect, need to zero these surfaces
                    vppBltFillBlock(pVpp, 0, vpp.dwOverlayFSOffset[i], vpp.dwOverlayFSPitch,
                        vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, max(vpp.dwOverlayFSFormat, 2));
                }
                }
            }
            else
            {
                dbgError("Failed to allocate offscreen FS-mirror surfaces");
            }
        }

        if (vpp.dwOverlayFSNumSurfaces == 0) {
#if IS_WINNT5
            // we failed restore the display mode
            RESOLUTION_INFO res;
            res.ulDisplayWidth      = vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].right -
                                      vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].left;
            res.ulDisplayHeight     = vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].bottom -
                                      vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].top;
            switch (vpp.m_ppdev->cjPelSize) {
            case 1:  res.ulDisplayPixelDepth = BMF_8BPP;  break;
            case 4:  res.ulDisplayPixelDepth = BMF_32BPP; break;
            default: res.ulDisplayPixelDepth = BMF_16BPP; break;
            }
            res.ulDisplayRefresh    = vpp.m_ppdev->ulRefreshRate[vpp.dwOverlayFSHead];
            NvSetDac(vpp.m_ppdev, vpp.dwOverlayFSHead, &res);
#elif IS_WIN9X
            // we failed, unlock
            vpp.dwOverlayFSTwinData.dwFlag = NVTWINVIEW_FLAG_SET;
            vpp.dwOverlayFSTwinData.dwAction = NVTWINVIEW_ACTION_UNLOCK_HWCRTC;
            vpp.dwOverlayFSTwinData.dwCrtc = dwPhysicalHead;     // leave as physical head
            MyExtEscape (dwDCHead, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA),
                         (LPCSTR) &vpp.dwOverlayFSTwinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &vpp.dwOverlayFSTwinData);
#endif
            return FALSE;
        } else {
/*
            if ((dwDeviceType == MONITOR_TYPE_NTSC || dwDeviceType == MONITOR_TYPE_PAL) ) {
                NV_CFGEX_SET_MACROVISION_MODE_PARAMS    mvParams;
                // To be safe, always turn on Macrovision even though we don't know that it's DVD playing.
                // Unfortunately, this is overkill but we have to be diligent legally. (Until we figure out
                // a better way to make our macrovision more secure.)
                mvParams.head = dwPhysicalHead;
                mvParams.resx = vpp.dwOverlayFSWidth;
                mvParams.resy = vpp.dwOverlayFSHeight;
                mvParams.mode = 3;  // modes are 1-3, arbitrarly use 3 (DVD players will reset it)
                // TODO:  We could define a macro (NvRmConfigSetEX) for this call in WinNT.  Every other RM config call has one.
#ifdef WINNT
                NvConfigSetEx(vpp.m_ppdev->hDriver, vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwDeviceHandle,
                                NV_CFGEX_SET_MACROVISION_MODE, &mvParams, sizeof(mvParams));
#else
                NvRmConfigSetEx(vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwDeviceHandle,
                                NV_CFGEX_SET_MACROVISION_MODE, &mvParams, sizeof(mvParams));
#endif
            }
*/
            // fill in other info
            vpp.dwOverlayFSDeltaX = 0;
            vpp.dwOverlayFSDeltaY = 0;
            vpp.dwOverlayFSIndex = 0;
            switch (vpp.dwOverlayFSFormat) {
            case 1:  vpp.dwOverlayFSFormat = NV062_SET_COLOR_FORMAT_LE_Y8;                break;
            case 4:  vpp.dwOverlayFSFormat = NV062_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8; break;
            default: vpp.dwOverlayFSFormat = NV062_SET_COLOR_FORMAT_LE_R5G6B5;            break;
            }
        }
    } else {
        return FALSE;
    }

#if IS_WINNT5
   DisableHeadCursor(vpp.m_ppdev, vpp.dwOverlayFSHead, TRUE);
#endif

    // Perform a quick flip to clear out the full screen memory.
    lpProcInfo = pmGetProcess(vpp.pDriverData, nvGetCurrentProcessId());
    vppFSFlipSync(pVpp,lpProcInfo, 0, 0);
    vppFSFlip(pVpp,lpProcInfo, 0);

    vpp.dwFlags |= VPP_FLAG_MIRROR_READY;
    return TRUE;
#endif // End if IS_WINNT4
}


//---------------------------------------------------------------------------
// vppDestroyFSMirror
//      Frees all the objects created in vppCreateFSMirror
extern void VppDestroyFSMirror(Vpp_t *pVpp)
{
    nvAssert(pVpp);

    if (!(pVpp->dwFlags & VPP_FLAG_MIRROR_READY))   // It hasn't been created
    {
        return;
    }

#if (IS_WINNT5 || IS_WIN9X)
    DWORD i;
    Vpp_t &vpp = *pVpp;
    LPPROCESSINFO lpProcInfo = pmGetProcess(vpp.pDriverData, nvGetCurrentProcessId());

    // destroy FS mirror surfaces
    if (vpp.dwOverlayFSNumSurfaces > 0) {
#if IS_WINNT5
        RESOLUTION_INFO res;

        vppFSFlipSync(pVpp,lpProcInfo, 0, TRUE);
        vppFSFlipSync(pVpp,lpProcInfo, 1, TRUE);

        // restore the display mode
        res.ulDisplayWidth      = vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].right -
                                  vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].left;
        res.ulDisplayHeight     = vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].bottom -
                                  vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].top;

        switch (vpp.m_ppdev->cjPelSize) {
        case 1:  res.ulDisplayPixelDepth = BMF_8BPP;  break;
        case 4:  res.ulDisplayPixelDepth = BMF_32BPP; break;
        default: res.ulDisplayPixelDepth = BMF_16BPP; break;
        }
        res.ulDisplayRefresh    = vpp.m_ppdev->ulRefreshRate[vpp.dwOverlayFSHead];

        // Get DAC back into the correct resolution
        NvSetDac(vpp.m_ppdev, vpp.dwOverlayFSHead, &res);

        // Re-enable the cursor
        DisableHeadCursor(vpp.m_ppdev, vpp.dwOverlayFSHead, FALSE);

        // Reset the current display rectangle to zero (resets panning values)
        vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].right   = res.ulDisplayWidth;
        vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].bottom  = res.ulDisplayHeight;
        vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].left    = 0;
        vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead].top     = 0;

        // Get DAC pointing back to the proper memory area
        NvSetDacImageOffset(vpp.m_ppdev, vpp.dwOverlayFSHead, 0);

        // Reset panning
        NVMoveDisplayWindow(vpp.m_ppdev, vpp.dwOverlayFSHead,&vpp.m_ppdev->rclCurrentDisplay[vpp.dwOverlayFSHead]);
#elif IS_WIN9X
        // get physical head number
        DWORD dwPhysicalHead;
        if (vpp.pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE) {
            NVTWINVIEWDATA twinData;
            twinData.dwFlag   = NVTWINVIEW_FLAG_GET;
            twinData.dwAction = NVTWINVIEW_ACTION_MAP_LOG_TO_PHYS_HEAD;
            twinData.dwCrtc   = vpp.dwOverlayFSHead;
            MyExtEscape (pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);
            dwPhysicalHead = twinData.dwCrtc;
        } else {
            dwPhysicalHead = vpp.dwOverlayFSHead;
        }

        // unlock the head CRTC
        vpp.dwOverlayFSTwinData.dwFlag = NVTWINVIEW_FLAG_SET;
        vpp.dwOverlayFSTwinData.dwAction = NVTWINVIEW_ACTION_UNLOCK_HWCRTC;
        vpp.dwOverlayFSTwinData.dwCrtc = dwPhysicalHead;
        MyExtEscape (pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPCSTR) &vpp.dwOverlayFSTwinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &vpp.dwOverlayFSTwinData);
#endif

        for (i=0; i<NV_MAX_EXTRA_FS_SURFACES; i++) {
            if (vpp.dwOverlayFSOffset[i]) {
                NVHEAP_FREE(vpp.dwOverlayFSOffset[i] + vpp.pDriverData->BaseAddress);
            }
            vpp.dwOverlayFSOffset[i] = 0;
        }
    }

    vpp.dwOverlayFSWidth = 0;
    vpp.dwOverlayFSHeight = 0;
    vpp.dwOverlayFSFormat = 0;
    vpp.dwOverlayFSPitch = 0;
    vpp.dwOverlayFSIndex = 0;
    vpp.dwOverlayFSNumSurfaces = 0;
    vpp.dwOverlayFSDeltaX = 0;
    vpp.dwOverlayFSDeltaY = 0;
    vpp.dwOverlayFSHead = 0xFFFFFFFF;

    vpp.dwFlags &= ~VPP_FLAG_MIRROR_READY;

#endif // End if (IS_WINNT5 || IS_WIN9X)
}



//---------------------------------------------------------------------------
// VppReadRegistry()
//
//---------------------------------------------------------------------------
extern NvU8 VppReadRegistry(Vpp_t *pVpp, HANDLE hDriver, NvU32 logicalHeadID)
{
    Vpp_t &vpp = *pVpp;

    nvAssert(pVpp);

    if (vpp.pDriverData==NULL || vpp.pSysInfo==NULL)
    {
        nvAssert(FALSE);  // On DBG builds, stop here... need to know if/when this occurs
        return TRUE;      // We were called before VppConstructor -- skip registry read benignly
    }

    if (vpp.regTool.Open(hDriver, logicalHeadID, NV4_REG_DIRECT_DRAW_SUBKEY))
    {
        vpp.regTool.Read(NV4_REG_OVL_MODE,  vpp.regOverlayMode);
        vpp.regTool.Read(NV4_REG_OVL_MODE2, vpp.regOverlayMode2);
        vpp.regTool.Read(NV4_REG_OVL_MODE3, vpp.regOverlayMode3);
        vpp.regTool.Read(NV4_REG_VPP_INV_MASK, vpp.regVPPInvMask);
        vpp.regTool.Read(NV4_REG_VPP_MAX_SURFACES, vpp.regVPPMaxSurfaces);
        vpp.regTool.Read(NV4_REG_OVLCC_ENABLE, vpp.regOverlayColourControlEnable);
#if IS_WIN9X
        vpp.regTool.Read(NV4_REG_VIDEO_BUS_MASTER_MODE, vpp.pDriverData->regTVTunerHack);
        vpp.regTool.Read(NV4_REG_ENUM_REFRESH, vpp.pDriverData->regEnumDDRefresh);
#endif

#if (IS_WIN9X || IS_WINNT5) && !defined(NVPE)
        vpp.regTool.Read(NV4_REG_VPE_ENABLE, vpp.pDriverData->bEnableVPE);  //check for VPE enable bit
#endif

        if (vpp.regTool.Read(NV4_REG_OVLCC_BRIGHTNESS, vpp.colorCtrl.lBrightness))
        {
            if (vpp.pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                // range should be from -10000 to +10000
                vpp.colorCtrl.lBrightness = min(max(vpp.colorCtrl.lBrightness * 10000 / 255, -10000), 10000);
            } else {
                // range should be from -100 to +100, but -256 to +255 is still legal
                vpp.colorCtrl.lBrightness = min(max(vpp.colorCtrl.lBrightness, -256), 255);
            }
        }

        if (vpp.regTool.Read(NV4_REG_OVLCC_CONTRAST, vpp.colorCtrl.lContrast))
        {
            if (vpp.pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                // convert range of 0 to 200% to 0 to 20000
                vpp.colorCtrl.lContrast = min(max(vpp.colorCtrl.lContrast * 20000 / 200, 0), 20000);
            } else {
                // convert range of 0 to 200% to 0 to 511
                vpp.colorCtrl.lContrast = min(max(vpp.colorCtrl.lContrast * 511 / 200, 0), 511);
            }
        }

        if (vpp.regTool.Read(NV4_REG_OVLCC_HUE, vpp.colorCtrl.lHue))
        {
            // range is 0 to 359 degrees
            while (vpp.colorCtrl.lHue < 0)    vpp.colorCtrl.lHue += 360;
            while (vpp.colorCtrl.lHue >= 360) vpp.colorCtrl.lHue -= 360;
        }

        if (vpp.regTool.Read(NV4_REG_OVLCC_SATURATION, vpp.colorCtrl.lSaturation))
        {
            if (vpp.pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                // convert range of 0 to 200% to 0 to 20000
                vpp.colorCtrl.lSaturation = min(max(vpp.colorCtrl.lSaturation * 20000 / 200, 0), 20000);
            } else {
                // convert range of 0 to 200% to 0 to 511
                vpp.colorCtrl.lSaturation = min(max(vpp.colorCtrl.lSaturation * 512 / 200, 0), 512);
            }
        }

        vpp.regTool.Close();
    }

    // can't do this unless more than 4M and MMX instructions available
    if (!(vpp.pSysInfo->dwCPUFeatureSet & CPU_FS_MMX) || (vpp.pDriverData->TotalVRAM <= 0x400000)) {
        vpp.regOverlayColourControlEnable = FALSE;
    }

    // Disable HQVUp on less than 16M
    if (vpp.pDriverData->TotalVRAM < 0x1000000) {
        vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_VUPSCALE;
    }

    if (vpp.pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
        // Disable HQVUp and sw colour controls on NV10/NV20
        vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_VUPSCALE;
        vpp.regOverlayColourControlEnable = FALSE;
        if (vpp.pDriverData->TotalVRAM < 0x800000) {
            // Disable temporal and deinterlace filters on NV10/NV20 if less than 8M
            vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_TFILTER;
            vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_DFILTER;
        }
    } else {
        vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_DFILTER;
        if (vpp.pDriverData->TotalVRAM < 0x1000000) {
            // Disable temporal filter on NV4/5 if less than 16M
            vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_TFILTER;
        }
    }

    // Disable fullscreen mirror on non-dual head devices
    if (vpp.pDriverData->dwHeads < 2) {
        vpp.regOverlayMode2 &= ~NV4_REG_OVL_MODE2_FSDEVICEMASK;
    }

    return TRUE;
}

LOCAL BOOL vppAllocateObjects(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;
    int i;
    BOOL bSuccess = TRUE;

    nvAssert(vpp.hChannel != NULL);

    // Allocate each object in the array; skip OVERLAY (done @ vppEnable)
    //
    for(i=1; bSuccess && i<VPP_OBJECT_COUNT; i++)
    {
        if (vpp.m_obj_a[i].status == VPP_OBJ_FREED)
        {
            vpp.m_obj_a[i].status =
                NvRmAllocObject(vpp.pDriverData->dwRootHandle, vpp.hChannel,vpp.m_obj_a[i].classIID,vpp.m_obj_a[i].classNdx)
                ? VPP_OBJ_ERROR:VPP_OBJ_ALLOCATED;
            bSuccess = (vpp.m_obj_a[i].status == VPP_OBJ_ALLOCATED);
            if (!bSuccess)
            {
                dbgError("Failed object allocation in VPP.");
            }
        }
    }

    // Set the DMA context NOTIFIER on all objects (excluding overlay)
    //   Note: We use the NV07A_SET_CONTEXT_DMA_NOTIFIES member for ALL objects in our array.
    //         This loop assumes all objects have their notifier set method @ that offset (and they must!)
    //
    for(i=1; bSuccess && i<VPP_OBJECT_COUNT; i++)
    {
        if (vpp.m_obj_a[i].status == VPP_OBJ_ALLOCATED)
        {
            vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[i].classIID);
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | NV07A_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
            vpp.pPusher->push(1, vpp.m_obj_a[i].contextIID);         // SetContextDmaNotifies
            vpp.pPusher->adjust(2);
        }
    }
    vpp.pPusher->start(TRUE);

    return bSuccess;
}

LOCAL void vppFreeObjects(Vpp_t *pVpp)
{
    int i;
    Vpp_t &vpp = *pVpp;

    nvAssert(vpp.hChannel != NULL);

    for(i=0; i<VPP_OBJECT_COUNT; i++)
    {
        if (vpp.m_obj_a[i].status == VPP_OBJ_ALLOCATED)
        {
            NvRmFree(vpp.pDriverData->dwRootHandle, vpp.hChannel,vpp.m_obj_a[i].classIID);
            vpp.m_obj_a[i].status = VPP_OBJ_FREED;
        }
    }
}

// Since different objects have different initialization requirements,
// this function is readily 'automated'.
LOCAL void vppInitObjects(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;
    int i;

    // Swizzle
    if (vpp.m_obj_a[SWIZZLE_NDX].status == VPP_OBJ_ALLOCATED)
    {
        vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[SWIZZLE_NDX].classIID);
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | NV077_SET_CONTEXT_DMA_IMAGE | 0x40000);
        vpp.pPusher->push(1, vpp.hInVideoMemContextDma);       // SetContextDmaImage
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV077_SET_CONTEXT_SURFACE | 0x40000);
        vpp.pPusher->push(3, vpp.hContextSurfaceSwizzled);                // SetContextSurface
        vpp.pPusher->push(4, SUB_CHANNEL(vpp.spareSubCh) | NV077_SET_COLOR_FORMAT | 0x80000);
        vpp.pPusher->push(5, NV077_SET_COLOR_FORMAT_LE_A8R8G8B8);          // SetColorFormat
        vpp.pPusher->push(6, NV077_SET_OPERATION_SRCCOPY);                 // SetOperation
        vpp.pPusher->adjust(7);
    }

    // Alpha Blt
    nvAssert(vpp.m_obj_a[ALPHA_BLT_NDX].status == VPP_OBJ_ALLOCATED);
    vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[ALPHA_BLT_NDX].classIID);
    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | NV05F_SET_CONTEXT_BETA4 | 0x40000);
    vpp.pPusher->push(1, vpp.hContextBeta4);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV05F_SET_OPERATION | 0x40000);
    vpp.pPusher->push(3, NV05F_SET_OPERATION_SRCCOPY_PREMULT);
    vpp.pPusher->push(4, SUB_CHANNEL(vpp.spareSubCh) | NV05F_SET_CONTEXT_SURFACES | 0x40000);
    vpp.pPusher->push(5, vpp.hSurfaces2D);
    vpp.pPusher->adjust(6);

    // V2V Format
    nvAssert(vpp.m_obj_a[V2V_FORMAT_NDX].status == VPP_OBJ_ALLOCATED);
    vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[V2V_FORMAT_NDX].classIID);
    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_BUFFER_IN | 0x80000);
    vpp.pPusher->push(1, vpp.hFromVideoMemContextDma);     // SetContextDmaBufferIn
    vpp.pPusher->push(2, vpp.hToVideoMemContextDma);       // SetContextDmaBufferOut
    vpp.pPusher->adjust(3);

    // Scaled Image (2 of them in sequence)
    nvAssert(SCALED_IMAGE2_NDX == SCALED_IMAGE1_NDX+1);
    for(i=0; i<2 ;i++)
    {
        int objNdx = SCALED_IMAGE1_NDX+i;
        if (vpp.m_obj_a[objNdx].status == VPP_OBJ_ALLOCATED)
        {
            vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[objNdx].classIID);

            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | NV063_SET_CONTEXT_PATTERN | 0x80000 );
            vpp.pPusher->push(1, vpp.hContextPattern);         // SetContextPattern
            vpp.pPusher->push(2, vpp.hContextRop);             // SetContextRop

            vpp.pPusher->push(3, SUB_CHANNEL(vpp.spareSubCh) | NV063_SET_OPERATION | 0x40000);
            vpp.pPusher->push(4, NV063_SET_OPERATION_ROP_AND);  // SetOperation

            vpp.pPusher->adjust(5);

            if (vpp.m_obj_a[objNdx].classNdx == NV05_SCALED_IMAGE_FROM_MEMORY)
            {
                vpp.pPusher->push(0,SUB_CHANNEL(vpp.spareSubCh) | NV063_SET_COLOR_CONVERSION | 0x40000);
                vpp.pPusher->push(1,NV063_SET_COLOR_CONVERSION_TRUNCATE);
                vpp.pPusher->adjust(2);
            }
        }
    }

    nvAssert(vpp.m_obj_a[DVDPICT_NDX].status == VPP_OBJ_ALLOCATED);
    vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[DVDPICT_NDX].classIID);
    vpp.pPusher->push(0,SUB_CHANNEL(vpp.spareSubCh) + NV038_SET_CONTEXT_DMA_OVERLAY | 0xC0000);
    vpp.pPusher->push(1,vpp.hFromVideoMemContextDma);
    vpp.pPusher->push(2,vpp.hFromVideoMemContextDma);
    vpp.pPusher->push(3,vpp.hToVideoMemContextDma);
    vpp.pPusher->adjust(4);

    vpp.pDriverData->dDrawSpareSubchannelObject = 0;
    vpp.pPusher->start(TRUE);
}


// NB: We don't yet know (since this is called at constructor time) whether or not
//     any given object is going to be valid.  Therefore we allocate a context DMA
//     for ALL of them.
LOCAL BOOL vppAllocateContextDMAs(Vpp_t *pVpp)
{
    NvU32 status;
    Vpp_t &vpp = *pVpp;
    int i;
    NvU32 contextSize;
    NvU8 *pContextBase;
    int notifierCount;
    NvU32 notifierMemSize;

// This is horrible!  This is so some of the code below which looks like function calls but
//  are actually macros, will get the right value! @mjl@ TBD
#ifdef WINNT
    PDEV *ppdev = pVpp->m_ppdev;
#endif
  
    // Determine how many notifiers total we have
    notifierCount = 0;
    for(i=0; i<VPP_OBJECT_COUNT; i++)
    {
        notifierCount += vpp.m_obj_a[i].notifierCount;
    }

    notifierMemSize = sizeof(NvNotification) * notifierCount;

    // Allocate memory for a bunch of notifiers and the scanline buffer.
    // We must allocate the memory for this structure out of SHARED or
    // SYSTEM space -- not out of App space so don't use GlobalAlloc.

    // TBD: Why doesn't this use AllocIPM? @mjl@
#if (IS_WINNT4 || IS_WINNT5)

    // Under Win2K and WinNT4 this routine is called once when the state is
    // created, and once again after every mode change. Only alloc
    // the notifier buffer and the notfier context DMAs once.
    if (!vpp.m_pContextDMABase)
    {
        vpp.m_pContextDMABase = (DWORD) EngAllocMem(FL_ZERO_MEMORY, (notifierMemSize), 'x_VN');
    }

#else // !WINNT
    {
        NvU32 dwMem[2];
        NvU32 notifierMemoryLimit;

        // Round up to nearest 4k or resman will fail the call
        //   The -1 changes this into a 'limit' which is what the call below requires
        //
        notifierMemoryLimit = ((notifierMemSize + 0xfff) & ~0xfff) - 1;

        if (NvRmAllocMemory (vpp.pDriverData->dwRootHandle,
                             vpp.pDriverData->dwDeviceHandle, NV_VPP_CONTEXT_DMA_MEMORY_IID,
                             NV01_MEMORY_SYSTEM,
                             (NVOS02_FLAGS_PHYSICALITY_NONCONTIGUOUS << 4) |
                             (NVOS02_FLAGS_LOCATION_PCI << 8) |
                             (NVOS02_FLAGS_COHERENCY_CACHED << 12),
                             (PVOID *) dwMem, &notifierMemoryLimit))
        {
            vpp.m_pContextDMABase = NULL;
        }
        else
        {
            vpp.m_pContextDMABase = dwMem[0];
        }
    }

#endif // !WINNT

    if (vpp.m_pContextDMABase == NULL)
    {
        dbgError("Failed to allocate context DMA memory block");
        dbgTracePop();
        return(FALSE);
    }

    for(pContextBase=(NvU8 *)vpp.m_pContextDMABase,i=0; i<VPP_OBJECT_COUNT; i++)
    {
        contextSize = vpp.m_obj_a[i].notifierCount*sizeof(NvNotification);
        
        status = NvRmAllocContextDma(
            vpp.pDriverData->dwRootHandle,
            vpp.m_obj_a[i].contextIID,
            NV01_CONTEXT_DMA,
            DRF_DEF(OS03, _FLAGS, _ACCESS,    _READ_WRITE) |
            DRF_DEF(OS03, _FLAGS, _LOCKED,    _ALWAYS) |
            DRF_DEF(OS03, _FLAGS, _COHERENCY, _CACHED),
            pContextBase,contextSize-1);
        
        vpp.m_obj_a[i].notifier_a = (volatile NvNotification *)pContextBase;   // Each of these context DMAs is used as a notifier
        
        pContextBase += contextSize;
        
        if (status!=0)
        {
            dbgError("VPP: Failed allocateContextDMAs()");
            return(FALSE);
        }
    }
    
    return (TRUE);
}

LOCAL void vppFreeContextDMAs(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;
    int i;
    
    nvAssert(pVpp != NULL);
    nvAssert(vpp.pDriverData != NULL);
    nvAssert(vpp.pDriverData->dwRootHandle != 0);
    nvAssert(vpp.pDriverData->dwDeviceHandle != 0);
    nvAssert(vpp.m_pContextDMABase != NULL);

// This is horrible!  This is so some of the code below which looks like function calls but
//  are actually macros, will get the right value! @mjl@ TBD
#ifdef WINNT
    PDEV *ppdev = pVpp->m_ppdev;
#endif

    for(i=0; i<VPP_OBJECT_COUNT; i++)
    {
        NvRmFree(vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwDeviceHandle,vpp.m_obj_a[i].contextIID);
        vpp.m_obj_a[i].notifier_a = NULL;
    }
    
#if (IS_WINNT4 || IS_WINNT5)
    if (vpp.m_pContextDMABase)
    {
        EngFreeMem((PVOID) vpp.m_pContextDMABase);
        vpp.m_pContextDMABase = NULL;
    }
#else
    NvRmFree(vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwDeviceHandle,NV_VPP_CONTEXT_DMA_MEMORY_IID);
#endif

}

//
// Determine which head the overlay will go on.  Result is stored
// in vpp.dwOverlayFSOvlHead.
//
LOCAL void vppChooseOverlayHead(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;
    DWORD dwOverlayDeviceType;
    NV_CFGEX_GET_DISPLAY_TYPE_MULTI_PARAMS displayParams;

#if (!IS_WINNT4)
    ResetTwinViewState(vpp.pDriverData);
#endif

                                    // Logical index of the requested full screen device
    DWORD dwRequestedFSDevice = (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK) - 1;
    DWORD dwAssignedOvlDevice = 0;  // Logical index of the overlay device, default to first head

    if (vpp.pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE &&
        dwRequestedFSDevice != 0xFFFFFFFF)
    {
        // We are in clone mode AND Video Mirror is enabled
        if (dwRequestedFSDevice < 14) {
            // Not in Auto-Select mode, deduce overlay head from FS head
            dwAssignedOvlDevice = dwRequestedFSDevice ^ 1;
        }
#ifndef WINNT
    }
    else
    {
        // We are NOT in clone mode OR we are in clone mode with Video
        // Mirror disabled
        dwAssignedOvlDevice = pDXShare->dwHeadNumber;
#endif
    }
    
#if (IS_WIN9X)
    // RM needs physical head, map logical to physical
    NVTWINVIEWDATA twinData;
    twinData.dwFlag   = NVTWINVIEW_FLAG_GET;
    twinData.dwAction = NVTWINVIEW_ACTION_MAP_LOG_TO_PHYS_HEAD;
    twinData.dwCrtc   = dwAssignedOvlDevice;
    MyExtEscape (pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA),
                 (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);
    
    // Display device type retrieval will use the physical index of the overlay head
    displayParams.Head = twinData.dwCrtc;

    // Win9x will remember the overlay head with its -logical- index
    vpp.dwOverlayFSOvlHead = twinData.dwCrtc;
#else
    // Device type retrieval will use the physical index of the overlay head
    displayParams.Head = vpp.m_ppdev->ulDeviceDisplay[dwAssignedOvlDevice];
    
    // WinNT/2K/XP will remember the ovelay head with its -physical- index
    vpp.dwOverlayFSOvlHead = displayParams.Head;
#endif

    NvRmConfigGetEx(vpp.pDriverData->dwRootHandle, vpp.pDriverData->dwDeviceHandle,
                    NV_CFGEX_GET_DISPLAY_TYPE_MULTI,
                    &displayParams, sizeof(displayParams));
    dwOverlayDeviceType = displayParams.Type;

    // Set flag to say whether the overlay is on a flat panel.
    // This is used later by UpdateOverlay32.
    if (dwOverlayDeviceType == DISPLAY_TYPE_FLAT_PANEL)
    {
        vpp.dwNVOverlayFlags |= OVERLAY_FLAG_ON_PANEL;
    }
    else
    {
        vpp.dwNVOverlayFlags &= ~OVERLAY_FLAG_ON_PANEL;
    }
}

// The overlay object is SPECIAL since it requires a parameter to specify the
// head on which to create it.
LOCAL NvU8 vppAllocOverlay(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;
    NV07A_ALLOCATION_PARAMETERS nv07aAllocParms;
    nvAssert(vpp.m_obj_a[OVERLAY_NDX].status == VPP_OBJ_FREED);
    nvAssert(vpp.hChannel != NULL);

    nv07aAllocParms.logicalHeadId = vpp.dwOverlayFSOvlHead;
    vpp.m_obj_a[OVERLAY_NDX].status =
        NvRmAlloc(vpp.pDriverData->dwRootHandle,vpp.hChannel,
              vpp.m_obj_a[OVERLAY_NDX].classIID,
              vpp.m_obj_a[OVERLAY_NDX].classNdx,
              &nv07aAllocParms)
        ? VPP_OBJ_ERROR:VPP_OBJ_ALLOCATED;

    if (vpp.m_obj_a[OVERLAY_NDX].status == VPP_OBJ_ERROR)
    {
        dbgError("VPP overlay allocation failed!");
        return FALSE;
    }

    // Special Case -- we've created the overlay and it's notifiers as part of the VPP,
    // but much of the driver [currently] needs direct access to these notifiers.
    // For now, we re-initialize the driver value to the "correct one".  Eventually, all
    // driver components that need to wait for the overlay notifiers to clear should call
    // a vpp function to do it for them.

    // Tell the rest of the driver where the real overlay notifier is located
    vpp.pDriverData->pPioFlipOverlayNotifierFlat = vpp.m_obj_a[OVERLAY_NDX].notifier_a;

    // Don't know if this is necessary here... @mjl@
    // Deal with possible mode change during pending flip

//    if (vpp.m_obj_a[OVERLAY_NDX].notifier_a[1].status == NV_IN_PROGRESS) {
//        long countDown = 0x200000;
//        while ((vpp.m_obj_a[OVERLAY_NDX].notifier_a[1].status == NV_IN_PROGRESS) && (--countDown > 0));
//    }
//    if (vpp.m_obj_a[OVERLAY_NDX].notifier_a[2].status == NV_IN_PROGRESS) {
//        long countDown = 0x200000;
//        while ((vpp.m_obj_a[OVERLAY_NDX].notifier_a[2].status == NV_IN_PROGRESS) && (--countDown > 0));
//    }

    // clear all status flags before first use
    vpp.m_obj_a[OVERLAY_NDX].notifier_a[0].status = 0;
    vpp.m_obj_a[OVERLAY_NDX].notifier_a[1].status = 0;
    vpp.m_obj_a[OVERLAY_NDX].notifier_a[2].status = 0;

    // Initialize the overlay
    vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[OVERLAY_NDX].classIID);
    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | NV07A_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
    vpp.pPusher->push(1, vpp.m_obj_a[OVERLAY_NDX].contextIID);         // SetContextDmaNotifies
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV07A_SET_CONTEXT_DMA_OVERLAY(0) | 0x80000);
    vpp.pPusher->push(3, vpp.hFromVideoMemContextDma);                 // SetContextDmaOverlay(0)
    vpp.pPusher->push(4, vpp.hFromVideoMemContextDma);                 // SetContextDmaOverlay(1)
    vpp.pPusher->adjust(5);
    return TRUE;
}

LOCAL void vppFreeOverlay(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;
    nvAssert(vpp.hChannel != NULL);

    NvRmFree(vpp.pDriverData->dwRootHandle, vpp.hChannel,vpp.m_obj_a[OVERLAY_NDX].classIID);
    vpp.m_obj_a[OVERLAY_NDX].status = VPP_OBJ_FREED;
}


//---------------------------------------------------------------------------
// VppCreateOverlay
//      Initializes some objects used by the VPP (TBD update this comment block @mjl@)
//
// Objects we don't need to create because they already exist:
//      D3D_CELSIUS_PRIMITIVE or D3D_KELVIN_PRIMITIVE (we initialize here)
//      NV_DD_CONTEXT_BETA4
//      NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT
//      NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT
//      NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT
//      NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT
//      NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT
//      NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT
//      NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT
//      NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT
//
// Events (created in nvProcMan):
//      NV_DD_EVENT_VIDEO_TO_SYSTEM_SYNC -> NV_VPP_V2OSH_FORMAT
//      NV_DD_EVENT_SYSTEM_TO_VIDEO_SYNC -> NV_VPP_OSH2V_FORMAT
//      NV_DD_EVENT_COMBINER_SYNC        -> vpp.hDX6MultiTextureTriangle
//
// Notifiers (created in ddDrv):
//      NV_DD_DMA_VPP_NOTIFIER

extern BOOL VppCreateOverlay(Vpp_t *pVpp)
{
    BOOL retCode = TRUE;
    Vpp_t &vpp = *pVpp;

    nvAssert(pVpp);

    // During modeset on win9x, this is called before enable (in nvEnable)
    //   Is that really necessary? @mjl@
    if (!(vpp.dwFlags & VPP_FLAG_ENABLED))
    {
        return FALSE;
    }

    nvAssert(!(vpp.dwFlags & VPP_FLAG_OVERLAY_READY));

    // This is only going to happen if one of the asserts fire... but this gives us
    // a chance to get further in the debug session...
#ifdef DEBUG
    if (vpp.pDriverData == NULL) return TRUE;
#endif

    vppChooseOverlayHead(pVpp);

    if (!vppAllocOverlay(pVpp)) 
    {
        return FALSE;
    }
    vpp.dwFlags |= VPP_FLAG_OVERLAY_READY;  // Set now so VppSetOverlayColourControl will know that overlay obj exists

    //
    // 3d object must already be created outside of the VPP, just fill in some default values
    //
    if (vpp.dwFlags & VPP_FLAG_CELSIUS_3D)
    {
        vppInitCelsius(pVpp);
    }
    else if (vpp.dwFlags & VPP_FLAG_KELVIN_3D)
    {
        vppInitKelvin(pVpp);
    }

    pmAddProcess(vpp.pDriverData, nvGetCurrentProcessId());

    VppSetOverlayColourControl(pVpp);

    vpp.dwOverlayFSOvlLost = FALSE;

    vpp.llDeliveryPeriod = 0;
    vpp.llDeliveryTime = 0;

    return retCode;
}

//---------------------------------------------------------------------------
// VppDestroyOverlay
//
extern void VppDestroyOverlay(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;

    if (!(pVpp->dwFlags & VPP_FLAG_OVERLAY_READY))   // It hasn't been created
    {
        return;
    }

#ifdef IKOS
    // To avoid a race (shows up in IKOS), make sure our push buffer is flushed before calling NvRmFree
    // NOTE:  enable this in non-IKOS builds once it's been tested more
    vpp.pPusher->flush(TRUE);
#endif

    // Delete events tied to objects before freeing the parent objects
    pmDeleteAllProcesses(vpp.pDriverData); // On NT4/2k this only removes all events

    vppFreeOverlay(pVpp);

    vpp.dwOverlayFSOvlHead = 0xFFFFFFFF;
    vpp.dwFlags &= ~VPP_FLAG_OVERLAY_READY;
}

//---------------------------------------------------------------------------
// VppDisable

extern void VppDisable(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;

    // In some shutdown scenarios, disable may be called regardless of current state
    if (!(vpp.dwFlags & VPP_FLAG_ENABLED)) return;

#ifdef IKOS
    // To avoid a race (shows up in IKOS), make sure our push buffer is flushed before calling NvRmFree
    // NOTE:  enable this in non-IKOS builds once it's been tested more
    vpp.pPusher->flush(TRUE);
#endif

    vppFreeObjects(pVpp);

    vpp.dwFlags &= ~VPP_FLAG_ENABLED;
}


//---------------------------------------------------------------------------
//  VppEnable
//
//    Defines and creates various resources needed to operate the VPP.
//
//---------------------------------------------------------------------------
extern NvU8 VppEnable(
    Vpp_t           *pVpp,
    PDEV            *ppdev,
    CPushBuffer     *pPusher,
    NvU32            hChannel,
    NvU32            hThreeDClass,
    NvU32            threeDClassSubCh,
    NvU32            *pThreeDClassLastUser,
    NvU32            hInVideoMemContextDma,
    NvU32            hFromVideoMemContextDma,
    NvU32            hToVideoMemContextDma,
    NvU32            hLutCursorDacBase,
    NvU32            hMiscEventNotifier,
    NvU32            hContextSurfaceSwizzled,
    NvU32            hContextBeta4,
    NvU32            hSurfaces2D,
    NvU32            surfaces2DSubCh,
    NvU32            hVideoMemUtoVideoMemFormat,     // U-conversion object
    NvU32            hVideoMemVtoVideoMemFormat,     // V-conversion object
    NvU32            hVideoMemUVtoVideoMemFormat,    // V-conversion object

    NvU32            unused1,
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
    NvU32            spareSubCh)
{
    Vpp_t &vpp = *pVpp;

    nvAssert(vpp.dwFlags & VPP_FLAG_CONSTRUCTED);
    nvAssert(!(vpp.dwFlags & VPP_FLAG_ENABLED));

    vpp.m_ppdev                            = ppdev;
    vpp.pPusher                            = pPusher;
    vpp.hThreeDClass                       = hThreeDClass;
    vpp.hChannel                           = hChannel;
    vpp.threeDClassSubCh                   = threeDClassSubCh;
    vpp.pThreeDClassLastUser               = pThreeDClassLastUser;
    vpp.hInVideoMemContextDma              = hInVideoMemContextDma;
    vpp.hFromVideoMemContextDma            = hFromVideoMemContextDma;
    vpp.hToVideoMemContextDma              = hToVideoMemContextDma;
    vpp.hLutCursorDacBase                  = hLutCursorDacBase;
    vpp.hMiscEventNotifier                 = hMiscEventNotifier;
    vpp.hContextSurfaceSwizzled            = hContextSurfaceSwizzled;
    vpp.hContextBeta4                      = hContextBeta4;
    vpp.hSurfaces2D                        = hSurfaces2D;
    vpp.surfaces2DSubCh                    = surfaces2DSubCh;
    vpp.hVideoMemUtoVideoMemFormat         = hVideoMemUtoVideoMemFormat;
    vpp.hVideoMemVtoVideoMemFormat         = hVideoMemVtoVideoMemFormat;
    vpp.hVideoMemUVtoVideoMemFormat        = hVideoMemUVtoVideoMemFormat;

    vpp.hContextPattern                    = hContextPattern;
    vpp.hContextRop                        = hContextRop;
    vpp.hContextColorKey                   = hContextColorKey;
    vpp.hFloatingContextDmaInOverlayShadow = hFloatingContextDmaInOverlayShadow;

    vpp.pFlipPrimaryNotifier               = pFlipPrimaryNotifier;
    vpp.pPusherSyncNotifier                = pPusherSyncNotifier;

    vpp.hContextSurfacesARGB_ZS            = hContextSurfacesARGB_ZS;
    vpp.hDX6MultiTextureTriangle           = hDX6MultiTextureTriangle;
    vpp.hImageBlackRect                    = hImageBlackRect;

    vpp.ropRectTextSubCh                   = ropRectTextSubCh;
    vpp.ropSubCh                           = ropSubCh;
    vpp.spareSubCh                         = spareSubCh;

    if (!vppAllocateObjects(pVpp))
    {
        dbgError("VPP object allocation failed!");
        VppDisable(pVpp);
        return FALSE;
    }

    vppInitObjects(pVpp);

    vpp.dwFlags |= VPP_FLAG_ENABLED;

    return TRUE;
}



//---------------------------------------------------------------------------
// vppPreCopy
//      Copies surface from source to destination.  Does not need to follow
//      the notifier protocol because it is not a stage by itself.
LOCAL BOOL vppPreCopy(Vpp_t *pVpp, DWORD dwSrcOffset, DWORD dwSrcPitch,
                DWORD dwDstOffset, DWORD dwDstPitch,
                DWORD dwWidth, DWORD dwHeight)
{
    Vpp_t &vpp = *pVpp;
    DWORD dwMergeSize;

    dwMergeSize = (dwHeight << 16) | (dwWidth >> 1);

    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(1, vpp.hContextBeta4);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | BETA4_SET_BETA_FACTOR_OFFSET | 0x40000);
    vpp.pPusher->push(3, 0xFFFFFFFF);

    // This is sometimes used in it's own channel -- but here DD_SURFACES_2D is assigned to spare? -mjl
    vpp.pPusher->push(4, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(5, vpp.hSurfaces2D);
    vpp.pPusher->push(6, SUB_CHANNEL(vpp.spareSubCh) | SURFACES_2D_PITCH_OFFSET | 0xC0000);
    vpp.pPusher->push(7, (dwDstPitch << 16) | dwSrcPitch);     // SetPitch
    vpp.pPusher->push(8, dwSrcOffset);                         // SetOffsetSource
    vpp.pPusher->push(9, dwDstOffset);                         // SetOffsetDestin
    vpp.pDriverData->bltData.dwLastCombinedPitch = (dwDstPitch << 16) | dwSrcPitch;
    vpp.pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
    vpp.pDriverData->bltData.dwLastDstOffset = dwDstOffset;

    vpp.pPusher->push(10, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(11, vpp.m_obj_a[ALPHA_BLT_NDX].classIID);
    vpp.pPusher->push(12, SUB_CHANNEL(vpp.spareSubCh) | BLIT_POINT_IN_OFFSET | 0xC0000);
    vpp.pPusher->push(13, 0);                                  // control point in
    vpp.pPusher->push(14, 0);                                  // control point out
    vpp.pPusher->push(15, dwMergeSize);                        // size

    vpp.pPusher->adjust(16);
    vpp.pPusher->start(TRUE);
    vpp.pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[ALPHA_BLT_NDX].classIID;
    return TRUE;
}


//---------------------------------------------------------------------------
// vppConvert
//      Converts YUV9 or YUV12 surface to YUV422.  Assume that we always
//      convert to YUY2 and not UYVY. Assume width, pitch, and height are
//      all divisible by the subsampling rate.
LOCAL BOOL vppConvert(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo,
                DWORD dwSrcOffset, DWORD dwSrcPitch,
                DWORD dwDstOffset, DWORD dwDstPitch,
                DWORD dwWidth, DWORD dwHeight,
                DWORD dwFourCC)
{
    DWORD i;
    DWORD dwSrcUOffset, dwSrcVOffset, dwCOffset;
    Vpp_t &vpp = *pVpp;

    dwSrcVOffset = dwSrcOffset + dwSrcPitch * dwHeight;

    // move the chroma data
    switch (dwFourCC) {
    case FOURCC_IF09:
    case FOURCC_YVU9:
    case FOURCC_IV32:
    case FOURCC_IV31:
        dwSrcUOffset = dwSrcVOffset + (dwSrcPitch >> 2) * (dwHeight >> 2);
        dwCOffset    = dwSrcUOffset + (dwSrcPitch >> 2) * (dwHeight >> 2);

        // first weave U and V together in the horizontal direction and double up
        // move U even
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.hVideoMemUtoVideoMemFormat);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
        vpp.pPusher->push(3, dwSrcUOffset);                    // OffsetIn
        vpp.pPusher->push(4, dwCOffset);                       // OffsetOut
        vpp.pPusher->push(5, dwSrcPitch >> 2);                 // PitchIn
        vpp.pPusher->push(6, dwSrcPitch);                      // PitchOut
        vpp.pPusher->push(7, dwWidth >> 2);                    // LineLengthIn
        vpp.pPusher->push(8, dwHeight >> 2);                   // LineCount
        vpp.pPusher->push(9, 0x401);                           // Format
        vpp.pPusher->push(10, NV039_BUFFER_NOTIFY_WRITE_ONLY); // BufferNotify
        vpp.pPusher->adjust(11);

        // move V even
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.hVideoMemVtoVideoMemFormat);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
        vpp.pPusher->push(3, dwSrcVOffset);                    // OffsetIn
        vpp.pPusher->push(4, dwCOffset + 1);                   // OffsetOut
        vpp.pPusher->push(5, dwSrcPitch >> 2);                 // PitchIn
        vpp.pPusher->push(6, dwSrcPitch);                      // PitchOut
        vpp.pPusher->push(7, dwWidth >> 2);                    // LineLengthIn
        vpp.pPusher->push(8, dwHeight >> 2);                   // LineCount
        vpp.pPusher->push(9, 0x401);                           // Format
        vpp.pPusher->push(10, NV039_BUFFER_NOTIFY_WRITE_ONLY); // BufferNotify
        vpp.pPusher->adjust(11);
        vpp.pPusher->start(TRUE);

        // move U odd
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.hVideoMemUtoVideoMemFormat);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
        vpp.pPusher->push(3, dwSrcUOffset);                    // OffsetIn
        vpp.pPusher->push(4, dwCOffset + 2);                   // OffsetOut
        vpp.pPusher->push(5, dwSrcPitch >> 2);                 // PitchIn
        vpp.pPusher->push(6, dwSrcPitch);                      // PitchOut
        vpp.pPusher->push(7, dwWidth >> 2);                    // LineLengthIn
        vpp.pPusher->push(8, dwHeight >> 2);                   // LineCount
        vpp.pPusher->push(9, 0x401);                           // Format
        vpp.pPusher->push(10, NV039_BUFFER_NOTIFY_WRITE_ONLY); // BufferNotify
        vpp.pPusher->adjust(11);

        // move V odd
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.hVideoMemVtoVideoMemFormat);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
        vpp.pPusher->push(3, dwSrcVOffset);                    // OffsetIn
        vpp.pPusher->push(4, dwCOffset + 3);                   // OffsetOut
        vpp.pPusher->push(5, dwSrcPitch >> 2);                 // PitchIn
        vpp.pPusher->push(6, dwSrcPitch);                      // PitchOut
        vpp.pPusher->push(7, dwWidth >> 2);                    // LineLengthIn
        vpp.pPusher->push(8, dwHeight >> 2);                   // LineCount
        vpp.pPusher->push(9, 0x401);                           // Format
        vpp.pPusher->push(10, NV039_BUFFER_NOTIFY_WRITE_ONLY); // BufferNotify
        vpp.pPusher->adjust(11);
        vpp.pPusher->start(TRUE);

        // final move, 4x vertical resample
        for (i=0; i<4; i++) {
            DWORD dwDstAdd = i * dwDstPitch;

            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, vpp.hVideoMemUVtoVideoMemFormat);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
            vpp.pPusher->push(3, dwCOffset);                       // OffsetIn
            vpp.pPusher->push(4, dwDstOffset + dwDstAdd + 1);      // OffsetOut
            vpp.pPusher->push(5, dwSrcPitch);                      // PitchIn
            vpp.pPusher->push(6, dwDstPitch << 2);                 // PitchOut
            vpp.pPusher->push(7, dwWidth);                         // LineLengthIn
            vpp.pPusher->push(8, dwHeight >> 2);                   // LineCount
            vpp.pPusher->push(9, 0x201);                           // Format
            vpp.pPusher->push(10, NV039_BUFFER_NOTIFY_WRITE_ONLY); // BufferNotify
            vpp.pPusher->adjust(11);
        }
        vpp.pPusher->start(TRUE);
        vpp.pDriverData->dDrawSpareSubchannelObject = vpp.hVideoMemUVtoVideoMemFormat;
        break;

    case FOURCC_YV12:
    case FOURCC_420i:
        dwSrcUOffset = dwSrcVOffset + (dwSrcPitch >> 1) * (dwHeight >> 1);

        for (i=0; i<4; i++) {
            DWORD dwDstAdd = (i >> 1) * dwDstPitch + (i & 0x1) * 4;

            // move U
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, vpp.hVideoMemUtoVideoMemFormat);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
            vpp.pPusher->push(3, dwSrcUOffset);                    // OffsetIn
            vpp.pPusher->push(4, dwDstOffset + dwDstAdd + 1);      // OffsetOut
            vpp.pPusher->push(5, dwSrcPitch >> 1);                 // PitchIn
            vpp.pPusher->push(6, dwDstPitch << 1);                 // PitchOut
            vpp.pPusher->push(7, dwWidth >> 1);                    // LineLengthIn
            vpp.pPusher->push(8, dwHeight >> 1);                   // LineCount
            vpp.pPusher->push(9, 0x401);                           // Format
            vpp.pPusher->push(10, NV039_BUFFER_NOTIFY_WRITE_ONLY); // BufferNotify
            vpp.pPusher->adjust(11);

            // move V
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, vpp.hVideoMemVtoVideoMemFormat);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
            vpp.pPusher->push(3, dwSrcVOffset);                    // OffsetIn
            vpp.pPusher->push(4, dwDstOffset + dwDstAdd + 3);      // OffsetOut
            vpp.pPusher->push(5, dwSrcPitch >> 1);                 // PitchIn
            vpp.pPusher->push(6, dwDstPitch << 1);                 // PitchOut
            vpp.pPusher->push(7, dwWidth >> 1);                    // LineLengthIn
            vpp.pPusher->push(8, dwHeight >> 1);                   // LineCount
            vpp.pPusher->push(9, 0x401);                           // Format
            vpp.pPusher->push(10, NV039_BUFFER_NOTIFY_WRITE_ONLY); // BufferNotify
            vpp.pPusher->adjust(11);
            vpp.pPusher->start(TRUE);
        }
        vpp.pDriverData->dDrawSpareSubchannelObject = vpp.hVideoMemVtoVideoMemFormat;
        break;

    default:
        return FALSE;
        break;
    }

//    resetNotification(vpp.pPusherSyncNotifier, lpProcInfo->hConvert3);
    VppResetNotification(vpp.m_obj_a[V2V_FORMAT_NDX].notifier_a, lpProcInfo->hConvert3);

    // move the luma data
    vpp.pPusher->push( 0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push( 1, vpp.m_obj_a[V2V_FORMAT_NDX].classIID);
    vpp.pPusher->push( 2, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
    vpp.pPusher->push( 3, dwSrcOffset);                             // OffsetIn
    vpp.pPusher->push( 4, dwDstOffset);                             // OffsetOut
    vpp.pPusher->push( 5, dwSrcPitch);                              // PitchIn
    vpp.pPusher->push( 6, dwDstPitch);                              // PitchOut
    vpp.pPusher->push( 7, dwWidth);                                 // LineLengthIn
    vpp.pPusher->push( 8, dwHeight);                                // LineCount
    vpp.pPusher->push( 9, 0x201);                                   // Format
    vpp.pPusher->push(10, VPP_NOTIFY_TYPE);                         // BufferNotify
    vpp.pPusher->adjust(11);
    vpp.pPusher->start(TRUE);

    return TRUE;
}


//---------------------------------------------------------------------------
// vppSubPicture
//      Composites DVD subpicture to YUV422 image
LOCAL BOOL vppSubPicture(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo,
                   DWORD dwSrcOffset, DWORD dwSrcPitch,
                   DWORD dwDstOffset, DWORD dwDstPitch,
                   DWORD dwWidth, DWORD dwHeight, DWORD dwFourCC)
{
    DWORD dwMergeSize, dwClipSize;
    DWORD dwOutFormat;
    DWORD dwInPoint, dwOutPoint;
    DWORD dwOverlayPoint;
    DWORD dwSubPictureOffset;
    Vpp_t &vpp = *pVpp;

    if (vpp.subPicture.pitch & NVS2VF_SP_SUBRECT) {
        DWORD dwBytesPerPixel = (vpp.subPicture.format == NV038_OVERLAY_FORMAT_COLOR_LE_A8V8U8Y8 ||
                                 vpp.subPicture.format == NV088_OVERLAY_FORMAT_COLOR_LE_A8CR8CB8Y8) ? 4:2;
        // codec requests in place subrectangle
        dwSubPictureOffset = vpp.subPicture.offset +
                             vpp.subPicture.originY * (vpp.subPicture.pitch & 0xFFFFFFF0) +
                             (vpp.subPicture.originX & ~0x1F) * dwBytesPerPixel;
        dwSrcOffset += vpp.subPicture.originY * dwSrcPitch + (vpp.subPicture.originX & ~0x1F) * 2;
        dwMergeSize = asmMergeCoords(vpp.subPicture.width, vpp.subPicture.height);
        dwOutPoint = asmMergeCoords(vpp.subPicture.originX & 0x1E, 0);
        dwInPoint = dwOutPoint << 4;
        dwOverlayPoint = dwInPoint;
        dwDstOffset = dwSrcOffset;
    } else {
        // otherwise blit the whole thing to next work surface
        dwSubPictureOffset = vpp.subPicture.offset;
        dwMergeSize = asmMergeCoords(dwWidth, dwHeight);
        dwInPoint = 0;
        dwOutPoint = 0;
        dwOverlayPoint = 0;
    }
    dwClipSize = asmMergeCoords(dwWidth, dwHeight);

    if (dwFourCC == FOURCC_UYVY) {
        dwOutFormat = NV038_IMAGE_OUT_FORMAT_COLOR_LE_YB8V8YA8U8;
    } else {
        dwOutFormat = NV038_IMAGE_OUT_FORMAT_COLOR_LE_V8YB8U8YA8;
    }

    VppResetNotification(vpp.m_obj_a[DVDPICT_NDX].notifier_a, lpProcInfo->hSubPicture3);

    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(1, vpp.m_obj_a[DVDPICT_NDX].classIID);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV038_IMAGE_OUT_POINT | 0x400000);
    vpp.pPusher->push(3, dwOutPoint);                              // ImageOutPoint
    vpp.pPusher->push(4, dwMergeSize);                             // ImageOutSize
    vpp.pPusher->push(5, (dwOutFormat << 16) | dwDstPitch);        // ImageOutFormat
    vpp.pPusher->push(6, dwDstOffset);                             // ImageOutOffset
    vpp.pPusher->push(7, 1 << 20);                                 // ImageInDeltaDuDx
    vpp.pPusher->push(8, 1 << 20);                                 // ImageInDeltaDvDy
    vpp.pPusher->push(9, dwClipSize);                              // ImageInSize
    vpp.pPusher->push(10, (dwOutFormat << 16) | dwSrcPitch);       // ImageInFormat
    vpp.pPusher->push(11, dwSrcOffset);                            // ImageInOffset
    vpp.pPusher->push(12, dwInPoint);                              // ImageInPoint
    vpp.pPusher->push(13, 1 << 20);                                // OverlayDeltaDuDx
    vpp.pPusher->push(14, 1 << 20);                                // OverlayDeltaDvDy
    vpp.pPusher->push(15, dwClipSize);                             // OverlaySize
    vpp.pPusher->push(16, (vpp.subPicture.format << 16) |  // OverlayFormat
                   (vpp.subPicture.pitch & 0xFFFFFFF0));
    vpp.pPusher->push(17, dwSubPictureOffset);                     // OverlayOffset
    vpp.pPusher->push(18, dwOverlayPoint);                         // OverlayPoint
    vpp.pPusher->push(19, SUB_CHANNEL(vpp.spareSubCh) | NV038_NOTIFY | 0x40000);
    vpp.pPusher->push(20, VPP_NOTIFY_TYPE);                        // Notify
    vpp.pPusher->push(21, SUB_CHANNEL(vpp.spareSubCh) | NV038_NO_OPERATION | 0x40000);
    vpp.pPusher->push(22, 0);
    vpp.pPusher->adjust(23);
    vpp.pPusher->start(TRUE);

    vpp.pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[DVDPICT_NDX].classIID;
    return TRUE;
}


//---------------------------------------------------------------------------
// vppPreScale
//      Prescales YUV422 surface.  The new width and height are returned.  If
//      this routine does the field prescaling, it flips off the field flags
//      to indicate that the flip should not bob.
LOCAL BOOL vppPreScale(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo,
                 DWORD dwSrcOffset, DWORD dwSrcPitch,
                 DWORD dwDstOffset, DWORD dwDstPitch,
                 LPDWORD pWidth, LPDWORD pHeight,
                 DWORD dwFourCC, LPDWORD pField)
{
    DWORD preScaleDeltaX;
    DWORD preScaleDeltaY;
    DWORD preScaleOverlay;
    DWORD dwDeltaX, dwDeltaY;
    DWORD dwXFactor, dwYFactor;
    DWORD dwSrcWidth, dwDstWidth, dwSrcHeight, dwDstHeight;
    DWORD dwOutSizeX, dwOutSizeY, dwOutFormat;
    DWORD dwInPitch, dwInOffset, dwInPoint, dwInSize;
    BOOL isField;
    Vpp_t &vpp = *pVpp;;

    dwSrcWidth = *pWidth;
    dwSrcHeight = *pHeight;
    dwDstWidth = vpp.dwOverlayDstWidth;
    dwDstHeight = vpp.dwOverlayDstHeight;

    if (dwDstWidth == 0 ||dwDstHeight == 0) {
        return FALSE;
    }

    isField = IS_FIELD(*pField);

    if (dwDstWidth <= 1) {
        dwDeltaX = 0x100000;
        dwXFactor = 1;
    } else {
        dwDeltaX = ((dwSrcWidth - 1) << 20) / (dwDstWidth - 1);
        dwXFactor = dwSrcWidth / dwDstWidth;
    }

    if (dwDstHeight <= 1) {
        dwDeltaY = 0x100000;
        dwYFactor = 1;
    } else {
        dwDeltaY = ((dwSrcHeight - 1) << 20) / (dwDstHeight - 1);
        dwYFactor = dwSrcHeight / dwDstHeight;
    }

    preScaleOverlay = 0;

    if (dwXFactor >= max(vpp.dwOverlayMaxDownScaleX,2) && vpp.extraNumSurfaces >= 4) {
        // use multipass X downscale, one pass reduces by a factor of 2
        preScaleOverlay |= NV_PRESCALE_OVERLAY_X;
        dwOutSizeX = (dwSrcWidth >> 1);
        preScaleDeltaX = ((dwSrcWidth - 1) << 20) / (dwSrcWidth / 2 - 1);
        vpp.dwOverlayDeltaX = ((dwSrcWidth / 2 - 1) << 20) / (dwDstWidth - 1);
    } else if (dwSrcWidth > (dwDstWidth * vpp.dwOverlayMaxDownScale)) {
        // use X prescaler
        preScaleOverlay |= NV_PRESCALE_OVERLAY_X;
        dwOutSizeX = dwDstWidth;
        preScaleDeltaX = dwDeltaX;
        vpp.dwOverlayDeltaX = 0x100000;
    } else {
        // use X overlay scaler
        dwOutSizeX = *pWidth;
        preScaleDeltaX = 0x100000;
        vpp.dwOverlayDeltaX = dwDeltaX;
    }

    if (dwYFactor >= max(vpp.dwOverlayMaxDownScaleY,2) && vpp.extraNumSurfaces >= 4) {
        // use multipass Y downscale, one pass reduces by a factor of 2
        preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
        dwOutSizeY = (dwSrcHeight >> 1) & ~1;
        preScaleDeltaY = ((dwSrcHeight - 1) << 20) / (dwSrcHeight / 2 - 1);
        vpp.dwOverlayDeltaY = ((dwSrcHeight / 2 - 1) << 20) / (dwDstHeight - 1);
    } else if (dwSrcHeight > (dwDstHeight * vpp.dwOverlayMaxDownScale) ||
              (vpp.regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE)) {
        // use Y prescaler
        preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
        dwOutSizeY = dwDstHeight;
        preScaleDeltaY = dwDeltaY;
        vpp.dwOverlayDeltaY = 0x100000;
    } else {
        // use Y overlay scaler
        dwOutSizeY = *pHeight;
        preScaleDeltaY = 0x100000;
        vpp.dwOverlayDeltaY = dwDeltaY;
    }

    if ((vpp.pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) || (preScaleOverlay & NV_PRESCALE_OVERLAY_Y)) {
        if (isField) {
            preScaleOverlay |= NV_PRESCALE_OVERLAY_FIELD;
            if (*pField & VPP_INTERLEAVED) {
                if (dwYFactor >= max(vpp.dwOverlayMaxDownScaleY,2) && vpp.extraNumSurfaces >= 4) {
                    // multipass
                    preScaleDeltaY = ((dwSrcHeight / 2 - 1) << 20) / (dwSrcHeight / 2 - 1);
                } else if (dwSrcHeight > (dwDstHeight * vpp.dwOverlayMaxDownScale) ||
                          (vpp.regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE)) {
                    // use Y prescaler
                    preScaleDeltaY = ((dwSrcHeight / 2 - 1) << 20) / (dwDstHeight - 1);
                } else {
                    // use Y overlay scaler
                    preScaleDeltaY = ((dwSrcHeight / 2 - 1) << 20) / (dwSrcHeight - 1);
                }
            }
        }
    }

    if (preScaleOverlay == 0) {
        return FALSE;
    }

    dwInSize = asmMergeCoords( ((*pWidth+1) & 0xFFFE), *pHeight);
    dwInPoint = 0xFFF8FFF8;
    dwInPitch = dwSrcPitch;
    dwInOffset = dwSrcOffset;
    if (isField && (preScaleOverlay & NV_PRESCALE_OVERLAY_FIELD)) {
        if (*pField & VPP_INTERLEAVED) {
            dwInPitch <<= 1;
            *pHeight >>= 1;
            if (*pField & VPP_ODD) {
                dwInOffset += dwSrcPitch;
            }
        }
        if (*pField & VPP_BOB) {
            // use -1/4, +1/4 biasing for bob fields
            if (*pField & VPP_ODD) {
                dwInPoint += 0x00040000;
            } else {
                dwInPoint += 0x000C0000;
            }
        }
    }
// I believe this code fixes a limit violation error (which should not be occuring so far as I can tell)
// however Sandy reports that visual quality is greatly reduced if we start on aligned boundary instead
// of -1/2 line. That results in a drop of our score on the Video2000 benchmark.  I could not get that
// to run.  Anyway, if no violation errors occur, we can remove this. @mjl@  
#if 0 
    else
    {
        dwInPoint = 0;
    }
#endif

    if (dwFourCC == FOURCC_UYVY) {
        dwOutFormat = NV038_IMAGE_OUT_FORMAT_COLOR_LE_YB8V8YA8U8;
    } else {
        dwOutFormat = NV038_IMAGE_OUT_FORMAT_COLOR_LE_V8YB8U8YA8;
    }

    // HWBUG: Must serialize all usage of DVD subpicture
    // if (IS_EVENT_ERROR(WaitForSingleObject(lpProcInfo->hSubPicture3, VPP_TIMEOUT_TIME))) {
    if (VppWaitForNotification(vpp.m_obj_a[DVDPICT_NDX].notifier_a, lpProcInfo->hSubPicture3, VPP_TIMEOUT_TIME)) {
        dbgError(" *** SUBPICTURE event failure ***");
        EVENT_ERROR;
    }

    VppResetNotification(vpp.m_obj_a[DVDPICT_NDX].notifier_a, lpProcInfo->hSubPicture3);

    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(1, vpp.m_obj_a[DVDPICT_NDX].classIID);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV038_IMAGE_OUT_POINT | 0x400000);
    vpp.pPusher->push(3, 0);                                       // ImageOutPoint
    vpp.pPusher->push(4, asmMergeCoords((dwOutSizeX + 1) & ~1, dwOutSizeY));  // ImageOutSize
    vpp.pPusher->push(5, (dwOutFormat << 16) | dwDstPitch);        // ImageOutFormat
    vpp.pPusher->push(6, dwDstOffset);                             // ImageOutOffset
    vpp.pPusher->push(7, preScaleDeltaX);                          // ImageInDeltaDuDx
    vpp.pPusher->push(8, preScaleDeltaY);                          // ImageInDeltaDvDy
    vpp.pPusher->push(9, dwInSize);                                // ImageInSize
    vpp.pPusher->push(10, (dwOutFormat << 16) | dwInPitch);        // ImageInFormat
    vpp.pPusher->push(11, dwInOffset);                             // ImageInOffset
    vpp.pPusher->push(12, dwInPoint);                              // ImageInPoint
    vpp.pPusher->push(13, preScaleDeltaX);                         // OverlayDeltaDuDx
    vpp.pPusher->push(14, preScaleDeltaY);                         // OverlayDeltaDvDy
    vpp.pPusher->push(15, dwInSize);                               // OverlaySize
    vpp.pPusher->push(16, (NV038_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT << 16) | dwInPitch);  // OverlayFormat
    vpp.pPusher->push(17, dwInOffset);                             // OverlayOffset
    vpp.pPusher->push(18, 0);                                      // OverlayPoint
    vpp.pPusher->push(19, SUB_CHANNEL(vpp.spareSubCh) | NV038_NOTIFY | 0x40000);
    vpp.pPusher->push(20, VPP_NOTIFY_TYPE);                        // Notify
    vpp.pPusher->push(21, SUB_CHANNEL(vpp.spareSubCh) | NV038_NO_OPERATION | 0x40000);
    vpp.pPusher->push(22, 0);
    vpp.pPusher->adjust(23);
    vpp.pPusher->start(TRUE);

    vpp.pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[DVDPICT_NDX].classIID;

    // return changes in state
    if ((vpp.regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE) ||
        (preScaleOverlay & NV_PRESCALE_OVERLAY_FIELD)) {
        // with HQVUp, we no longer use the overlay to bob
        *pField &= ~(VPP_ODD | VPP_EVEN | VPP_BOB | VPP_INTERLEAVED);
    }
    *pHeight = dwOutSizeY;
    *pWidth  = dwOutSizeX;
    return TRUE;
}


//---------------------------------------------------------------------------
// vppFSMirror
//      Scale and colour convert YUV to RGB surface
LOCAL BOOL vppFSMirror(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo,
                 DWORD dwSrcOffset, DWORD dwSrcPitch,
                 DWORD dwWidth, DWORD dwHeight,
                 DWORD dwFourCC, DWORD dwFlags,
                 DWORD dwPrescaleFactorX, DWORD dwPrescaleFactorY)
{
    DWORD dwDeltaX, dwDeltaY;
    DWORD dwDstPitch, dwDstOffset, dwDstSize, dwSrcSize, dwDstPoint;
    DWORD dwSrcWidth, dwDstWidth, dwSrcHeight, dwDstHeight;
    DWORD dwCombinedPitch, dwColourFormat;
    DWORD dwInFormat, dwSrcX, dwSrcY;
    DWORD dwInPitch, dwInOffset, dwInPoint;
    BOOL isField, doZoom;
    DWORD dwBytesPerPel;
    DWORD dwColourFormat2;
    Vpp_t &vpp = *pVpp;

    dwSrcX      = (vpp.dwOverlayFSSrcMinX * dwPrescaleFactorX) >> 20;
    dwSrcY      = (vpp.dwOverlayFSSrcMinY * dwPrescaleFactorY) >> 20;
    dwSrcWidth  = (vpp.dwOverlayFSSrcWidth * dwPrescaleFactorX) >> 20;
    dwSrcHeight = (vpp.dwOverlayFSSrcHeight * dwPrescaleFactorY) >> 20;
    dwDstWidth  = vpp.dwOverlayFSWidth;
    dwDstHeight = vpp.dwOverlayFSHeight;
    dwDstPitch  = vpp.dwOverlayFSPitch;
    dwDstOffset = vpp.dwOverlayFSOffset[vpp.dwOverlayFSIndex];
    dwDstPoint  = 0;
    dwInPoint   = (((dwSrcX << 4) - 8) & 0x0000FFFF) | ((dwSrcY << 20) - 0x80000);

    // scaled image and gdi_rect will silently fail unless we set the pattern (?)
    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(1, vpp.hContextPattern);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) + NV044_SET_PATTERN_SELECT | 0xC0000);
    vpp.pPusher->push(3, NV044_SET_PATTERN_SELECT_MONOCHROME);
    switch (vpp.dwOverlayFSFormat) {
    case NV062_SET_COLOR_FORMAT_LE_Y8:
        dwBytesPerPel = 2;
        dwColourFormat2 = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;
        vpp.pPusher->push(4,NV_ALPHA_1_008);
        vpp.pPusher->push(5,NV_ALPHA_1_008);
        break;
    case NV062_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8:
        dwBytesPerPel = 4;
        dwColourFormat2 = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;
        vpp.pPusher->push(4,NV_ALPHA_1_032);
        vpp.pPusher->push(5,NV_ALPHA_1_032);
        break;
    default:
        dwBytesPerPel = 2;
        dwColourFormat2 = NV04A_SET_COLOR_FORMAT_LE_X16R5G6B5;
        vpp.pPusher->push(4,NV_ALPHA_1_016);
        vpp.pPusher->push(5,NV_ALPHA_1_016);
        break;
    }
    vpp.pPusher->adjust(6);

    isField = IS_FIELD(dwFlags);

    if (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSTRACKOVLZOOM) {
        doZoom = vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_OVLZOOMQUADMASK;
    } else {
        doZoom = vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSZOOMQUADMASK;
    }
    if (doZoom) {
        // zoom
        DWORD dwZoomFactor, dwZoomQuad;
        DWORD dwNewSrcWidth, dwNewSrcHeight;

        if (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSTRACKOVLZOOM) {
            dwZoomFactor   = 384 - ((vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_OVLZOOMFACTORMASK) >> 12);
            dwZoomQuad     = (vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_OVLZOOMQUADMASK) >> 8;
        } else {
            dwZoomFactor   = 384 - ((vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSZOOMFACTORMASK) >> 12);
            dwZoomQuad     = (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSZOOMQUADMASK) >> 8;
        }
        dwNewSrcWidth  = dwSrcWidth  * dwZoomFactor / 384;
        dwNewSrcHeight = dwSrcHeight * dwZoomFactor / 384;

        // get start point
        switch (dwZoomQuad) {
        case 1:  break;
        case 2:  dwSrcX += dwSrcWidth - dwNewSrcWidth;          break;
        case 3:  dwSrcY += dwSrcHeight - dwNewSrcHeight;        break;
        case 4:  dwSrcX += dwSrcWidth - dwNewSrcWidth;
                 dwSrcY += dwSrcHeight - dwNewSrcHeight;        break;
        case 5:  dwSrcX += (dwSrcWidth - dwNewSrcWidth) >> 1;
                 dwSrcY += (dwSrcHeight - dwNewSrcHeight) >> 1; break;
        default: break;
        }
        dwSrcWidth = dwNewSrcWidth;
        dwSrcHeight = dwNewSrcHeight;
    }
    // align source
    dwSrcOffset += (dwSrcY * dwSrcPitch) + ((dwSrcX * 2) & ~NV_BYTE_ALIGNMENT_PAD);
    dwSrcX &= ((NV_BYTE_ALIGNMENT / 2) - 1);
    dwSrcY = 0;
    dwInPoint = (dwInPoint & 0xFFFF0000) | ((dwInPoint & 0x0000FFFF) + (dwSrcX << 4));

    dwDeltaX = ((dwSrcWidth - 1) << 20) / (dwDstWidth - 1);
    dwDeltaY = ((dwSrcHeight - 1) << 20) / (dwDstHeight - 1);
    dwSrcSize  = (dwSrcHeight << 16) | ((dwSrcWidth + dwSrcX + 1) & ~1);

    if (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSASPECT) {
        // preserve aspect
        if (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSTRACKOVLASPECT) {
            // track overlay aspect
            DWORD dwOverlayAspect = (vpp.dwOverlayDeltaX << 8) / vpp.dwOverlayDeltaY;
            DWORD dwNewDeltaY, dwNewDstHeight;

            dwNewDeltaY = (dwDeltaX << 8) / dwOverlayAspect;
            dwNewDstHeight = ((dwSrcHeight - 1) << 20) / dwNewDeltaY + 1;
            if (dwNewDstHeight <= dwDstHeight) {
                DWORD dwHeightPad;
                // X is wider
                dwDeltaY = dwNewDeltaY;
                dwDstHeight = dwNewDstHeight;
                dwHeightPad = ((vpp.dwOverlayFSHeight - dwDstHeight) >> 1);
                dwDstOffset += vpp.dwOverlayFSPitch * dwHeightPad;
//                if (vpp.dwOverlayFSNumSurfaces == 1) {
                    vppBltFillBlock(pVpp, 0, vpp.dwOverlayFSOffset[vpp.dwOverlayFSIndex], vpp.dwOverlayFSPitch,
                                 vpp.dwOverlayFSWidth, dwHeightPad, dwBytesPerPel);
                    vppBltFillBlock(pVpp, 0, vpp.dwOverlayFSOffset[vpp.dwOverlayFSIndex] + (dwHeightPad + dwDstHeight) * vpp.dwOverlayFSPitch,
                                 vpp.dwOverlayFSPitch, dwDstWidth, dwHeightPad, dwBytesPerPel);
                    vpp.dwOverlayFSDeltaX = dwDeltaX;
                    vpp.dwOverlayFSDeltaY = dwDeltaY;
//                }
            } else {
                // Y is higher
                DWORD dwXOffsetPels;

                dwOverlayAspect = (vpp.dwOverlayDeltaY << 8) / vpp.dwOverlayDeltaX;
                dwDeltaX = (dwDeltaY << 8) / dwOverlayAspect;
                dwDstWidth = ((dwSrcWidth - 1) << 20) / dwDeltaX + 1;
                dwXOffsetPels = (vpp.dwOverlayFSWidth - dwDstWidth) >> 1;
                dwDstOffset += (dwXOffsetPels * dwBytesPerPel) & ~NV_BYTE_ALIGNMENT_PAD;
                dwDstPoint = dwXOffsetPels & ((NV_BYTE_ALIGNMENT / dwBytesPerPel) - 1);
                dwSrcSize  = (dwSrcHeight << 16) | ((dwSrcWidth + dwDstPoint + dwSrcX + 1) & ~1);
//                if (vpp.dwOverlayFSNumSurfaces == 1) {
                    vppBltFillBlock(pVpp, 0, vpp.dwOverlayFSOffset[vpp.dwOverlayFSIndex],
                                 vpp.dwOverlayFSPitch, dwXOffsetPels, dwDstHeight, dwBytesPerPel);
                    vppBltFillBlock(pVpp, 0, vpp.dwOverlayFSOffset[vpp.dwOverlayFSIndex] + ((dwXOffsetPels + dwDstWidth) * dwBytesPerPel),
                                 vpp.dwOverlayFSPitch, dwXOffsetPels, dwDstHeight, dwBytesPerPel);
                    vpp.dwOverlayFSDeltaX = dwDeltaX;
                    vpp.dwOverlayFSDeltaY = dwDeltaY;
//                }
            }
        } else if (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSFIXEDANIMORPHIC) {
            // fixed 16:9 aspect
            dwDstHeight = dwDstWidth * 9 / 16;
            dwDeltaY = ((dwSrcHeight - 1) << 20) / (dwDstHeight - 1);
            dwDstOffset += vpp.dwOverlayFSPitch * ((vpp.dwOverlayFSHeight - dwDstHeight) >> 1);
        } else if (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSFIXEDTV) {
            // fixed 4:3 aspect
            if ((dwDstHeight * 4 / 3) >= dwDstWidth) {
                // use X as a reference
                dwDstHeight = dwDstWidth * 3 / 4;
                dwDeltaY = ((dwSrcHeight - 1) << 20) / (dwDstHeight - 1);
                dwDstOffset += vpp.dwOverlayFSPitch * ((vpp.dwOverlayFSHeight - dwDstHeight) >> 1);
            } else {
                DWORD dwXOffsetPels;

                // use Y as a reference
                dwDstWidth = dwDstHeight * 4 / 3;
                dwDeltaX = ((dwSrcWidth - 1) << 20) / (dwDstWidth - 1);
                dwXOffsetPels = (vpp.dwOverlayFSWidth - dwDstWidth) >> 1;
                dwDstOffset += (dwXOffsetPels * dwBytesPerPel) & ~NV_BYTE_ALIGNMENT_PAD;
                dwDstPoint = dwXOffsetPels & ((NV_BYTE_ALIGNMENT / dwBytesPerPel) - 1);
                dwSrcSize  = (dwSrcHeight << 16) | ((dwSrcWidth + dwDstPoint + dwSrcX + 1) & ~1);
            }
        } else {
            // preserve aspect
            DWORD dwNewDstHeight = ((dwSrcHeight - 1) << 20) / dwDeltaX + 1;

            if (dwNewDstHeight <= dwDstHeight) {
                // X is wider
                dwDeltaY = dwDeltaX;
                dwDstHeight = dwNewDstHeight;
                dwDstOffset += vpp.dwOverlayFSPitch * ((vpp.dwOverlayFSHeight - dwDstHeight) >> 1);
            } else {
                //Y is wider
                DWORD dwXOffsetPels;

                dwDeltaX = dwDeltaY;
                dwDstWidth = ((dwSrcWidth - 1) << 20) / dwDeltaX + 1;
                dwXOffsetPels = (vpp.dwOverlayFSWidth - dwDstWidth) >> 1;
                dwDstOffset += (dwXOffsetPels * dwBytesPerPel) & ~NV_BYTE_ALIGNMENT_PAD;
                dwDstPoint = dwXOffsetPels & ((NV_BYTE_ALIGNMENT / dwBytesPerPel) - 1);
                dwSrcSize  = (dwSrcHeight << 16) | ((dwSrcWidth + dwDstPoint + dwSrcX + 1) & ~1);
            }
        }
        if (vpp.dwOverlayFSDeltaX != dwDeltaX || vpp.dwOverlayFSDeltaY != dwDeltaY) {
            static DWORD dwCount = 0;
            // aspect has changed, clear surface first
            vppBltFillBlock(pVpp, 0, vpp.dwOverlayFSOffset[vpp.dwOverlayFSIndex], vpp.dwOverlayFSPitch,
                         vpp.dwOverlayFSWidth, vpp.dwOverlayFSHeight, dwBytesPerPel);
            if (dwCount++ >= (4 * vpp.dwOverlayFSNumSurfaces)) {
                vpp.dwOverlayFSDeltaX = dwDeltaX;
                vpp.dwOverlayFSDeltaY = dwDeltaY;
                dwCount = 0;
            }
        }
    }

    dwInPitch = dwSrcPitch;
    dwInOffset = dwSrcOffset;
    if (isField) {
        // Set flag to say overlay has been in Bob mode.
        vpp.pDriverData->dwDXVAFlags |= DXVA_OVERLAY_WAS_BOBBED;

        if (dwFlags & VPP_INTERLEAVED) {
            dwInPitch <<= 1;
            dwSrcHeight >>= 1;
            dwDeltaY = ((dwSrcHeight - 1) << 20) / (dwDstHeight);
            if (dwFlags & VPP_ODD) {
                dwInOffset += dwSrcPitch;
            }
        }
        if (dwFlags & VPP_BOB) {
            // use -1/4, +1/4 biasing for bob fields
            if (dwFlags & VPP_ODD) {
                dwInPoint += 0x00040000;
            } else {
                dwInPoint += 0x000C0000;
            }
        }
    }
    else if (vpp.pDriverData->dwDXVAFlags & DXVA_OVERLAY_WAS_BOBBED) {
        // For Weave mode: if the overlay has ever been in Bob mode, we adjust the vertical
        // scale factor to match the Bob mode scaling, to improve Bob/Weave switches.
        dwDeltaY = ((dwSrcHeight - 1) << 20) / (dwDstHeight);
    }

    // set ROP
    vpp.pPusher->push(0, SUB_CHANNEL(vpp.ropSubCh) | SET_ROP_OFFSET | 0x40000);
    vpp.pPusher->push(1, SRCCOPYINDEX);
    vpp.pPusher->adjust(2);
    vpp.pDriverData->bltData.dwLastRop = SRCCOPYINDEX;

    // set colour key
    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(1, vpp.hContextColorKey);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | SET_TRANSCOLOR_OFFSET | 0x40000);
    vpp.pPusher->push(3, 0);
    vpp.pPusher->adjust(4);
    vpp.pDriverData->bltData.dwLastColourKey = 0xFFFFFFFF;

    if (dwFourCC == FOURCC_UYVY) {
        dwInFormat = NV089_SET_COLOR_FORMAT_LE_YB8CR8YA8CB8;
    } else {
        dwInFormat = NV089_SET_COLOR_FORMAT_LE_CR8YB8CB8YA8;
    }

    dwCombinedPitch = (dwDstPitch << 16) | dwInPitch;
    dwColourFormat = max(vpp.dwOverlayFSFormat, NV062_SET_COLOR_FORMAT_LE_R5G6B5);

    vpp.pPusher->push(0, SUB_CHANNEL(vpp.surfaces2DSubCh) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
    vpp.pPusher->push(1, dwColourFormat);                  // SetColorFormat
    vpp.pPusher->push(2, dwCombinedPitch);                 // SetPitch
    vpp.pPusher->push(3, dwInOffset);                      // SetSrcOffset
    vpp.pPusher->push(4, dwDstOffset);                     // SetDstOffset
    vpp.pPusher->adjust(5);
    vpp.pDriverData->bltData.dwLastColourFormat = dwColourFormat;
    vpp.pDriverData->bltData.dwLastCombinedPitch = dwCombinedPitch;
    vpp.pDriverData->bltData.dwLastSrcOffset = dwInOffset;
    vpp.pDriverData->bltData.dwLastDstOffset = dwDstOffset;

    dwDstSize  = (dwDstHeight << 16) | dwDstWidth;

// TBD ask ###
    // wait for any backdoor FOURCC blits to complete
    if (VppWaitForNotification(vpp.pPusherSyncNotifier, lpProcInfo->hFSMirror3, VPP_TIMEOUT_TIME)) {
        dbgError(" *** FSMIRROR event failure ***");
        EVENT_ERROR;
    }

    VppResetNotification(vpp.pPusherSyncNotifier, lpProcInfo->hFSMirror3);

    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(1, vpp.m_obj_a[SCALED_IMAGE1_NDX].classIID);
//    vpp.pPusher->push(1, NV_DD_SCALED_IMAGE_IID);

    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000);
    vpp.pPusher->push(3, vpp.hInVideoMemContextDma);   // SetContextDmaImage
    vpp.pPusher->push(4, SUB_CHANNEL(vpp.spareSubCh) | SCALED_IMAGE_CONTEXT_SURFACE_OFFSET | 0x40000);
    vpp.pPusher->push(5, vpp.hSurfaces2D);                       // SetContextSurface
    vpp.pPusher->push(6, SUB_CHANNEL(vpp.spareSubCh) | SCALED_IMAGE_SET_COLOR_FORMAT | 0x40000);
    vpp.pPusher->push(7, dwInFormat);              // SetColorFormat
    vpp.pPusher->push(8, SUB_CHANNEL(vpp.spareSubCh) | SCALED_IMAGE_CLIPPOINT_OFFSET | 0x180000);
    vpp.pPusher->push(9, dwDstPoint);              // ClipPoint
    vpp.pPusher->push(10, dwDstSize);              // ClipSize
    vpp.pPusher->push(11, dwDstPoint);             // ImageOutPoint
    vpp.pPusher->push(12, dwDstSize);              // ImageOutSize
    vpp.pPusher->push(13, dwDeltaX);               // DsDx
    vpp.pPusher->push(14, dwDeltaY);               // DtDy
    vpp.pPusher->push(15, SUB_CHANNEL(vpp.spareSubCh) | SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET | 0x100000);
    vpp.pPusher->push(16, dwSrcSize);              // ImageInSize
    vpp.pPusher->push(17, (NV089_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |     // ImageInFormat
                   (NV089_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                   dwInPitch);
    vpp.pPusher->push(18, dwInOffset);             // ImageInOffset
    vpp.pPusher->push(19, dwInPoint);              // ImageInPoint
    vpp.pPusher->push(20, SUB_CHANNEL(vpp.spareSubCh) | SCALED_IMAGE_NOTIFY_OFFSET | 0x40000);
    vpp.pPusher->push(21, VPP_NOTIFY_TYPE);
    vpp.pPusher->push(22, SUB_CHANNEL(vpp.spareSubCh) | NV089_NO_OPERATION | 0x40000);
    vpp.pPusher->push(23, 0);
    vpp.pPusher->adjust(24);
    vpp.pPusher->start(TRUE);
    vpp.pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[SCALED_IMAGE1_NDX].classIID;
//    vpp.pDriverData->dDrawSpareSubchannelObject = NV_DD_SCALED_IMAGE_IID;

    // restore surfaces2d, pattern, and rect formats
    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(1, vpp.hContextPattern);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) + NV044_SET_PATTERN_SELECT | 0xC0000);
    vpp.pPusher->push(3, NV044_SET_PATTERN_SELECT_MONOCHROME);
    switch ((GET_MODE_BPP() + 1) / 8) {
    case 1:
        dwColourFormat2 = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;
        vpp.pPusher->push(4,NV_ALPHA_1_008);
        vpp.pPusher->push(5,NV_ALPHA_1_008);
        break;
    case 4:
        dwColourFormat2 = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;
        vpp.pPusher->push(4,NV_ALPHA_1_032);
        vpp.pPusher->push(5,NV_ALPHA_1_032);
        break;
    default:
        dwColourFormat2 = NV04A_SET_COLOR_FORMAT_LE_X16R5G6B5;
        vpp.pPusher->push(4,NV_ALPHA_1_016);
        vpp.pPusher->push(5,NV_ALPHA_1_016);
        break;
    }
    vpp.pPusher->push(6, SUB_CHANNEL(vpp.ropRectTextSubCh) | NV04A_SET_COLOR_FORMAT | 0x40000);
    vpp.pPusher->push(7, dwColourFormat2);
    vpp.pPusher->push(8, SUB_CHANNEL(vpp.surfaces2DSubCh) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000);
    vpp.pPusher->push(9, NV062_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8);
    vpp.pPusher->adjust(10);

    vpp.pDriverData->bltData.dwLastColourFormat = NV062_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;

    return TRUE;
}


//---------------------------------------------------------------------------
// vppColourControl
//      Software colour controls for NV4/5.  There is evidence that the
//      Windows kernel uses the floating point stack, which is why there
//      are extra EMMS instructions scattered through the code.
LOCAL BOOL vppColourControl(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo,
                      DWORD dwSrcOffset, DWORD dwSrcPitch,
                      DWORD dwDstOffset, DWORD dwDstPitch,
                      DWORD dwWidth, DWORD dwHeight,
                      DWORD dwFourCC, DWORD dwFlags)
{
    BOOL bOverContrast;
    DWORD dwContrast, dwBrightness, cnt, brt;
    short hue, sat;
    DWORD dwYPhase, dwSysPitch;
    BOOL doContrastBlt, doLuma, doChroma;
    DWORD dwMergeSize;
    BOOL isField;
    Vpp_t &vpp = *pVpp;

    isField = IS_FIELD(dwFlags);

    if (isField) {
        if (dwFlags & VPP_INTERLEAVED) {
            if (dwFlags & VPP_ODD) {
                dwSrcOffset += dwSrcPitch;
                dwDstOffset += dwDstPitch;
            }
            dwSrcPitch <<= 1;
            dwDstPitch <<= 1;
            dwHeight >>= 1;
        }
    }

    if (vpp.colorCtrl.lContrast > 0xFF) {
        cnt = vpp.colorCtrl.lContrast >> 1;
        bOverContrast = TRUE;
    } else {
        cnt = vpp.colorCtrl.lContrast;
        bOverContrast = FALSE;
    }
    if(vpp.colorCtrl.lBrightness  < 0 ) {
      brt = -vpp.colorCtrl.lBrightness;
    } else {
      brt = vpp.colorCtrl.lBrightness;
    }
    hue = (short) vpp.colorCtrl.lHue;
    sat = (short) vpp.colorCtrl.lSaturation;

    dwSysPitch = dwSrcPitch >> 1;
    if (dwFourCC == FOURCC_UYVY || dwFourCC == FOURCC_UYNV) {
        dwContrast   = (cnt << 8) | (cnt << 24) | 0x00FF00FF;
        dwBrightness = brt | (brt << 8) | (brt << 16) | (brt << 24);
        dwYPhase = 1;
    } else if (dwFourCC == FOURCC_YUY2 || dwFourCC == FOURCC_YUNV) {
        dwContrast   = cnt | (cnt << 16) | 0xFF00FF00;
        dwBrightness = brt | (brt << 8) | (brt << 16) | (brt << 24);
        dwYPhase = 0;
    } else {
        dwContrast   = 0xFFFFFFFF;
        dwBrightness = 0;
        bOverContrast = FALSE;
        dwYPhase = 0;
    }
    if (dwFourCC == FOURCC_YV12 ||
        dwFourCC == FOURCC_YVU9 ||
        dwFourCC == FOURCC_IF09 ||
        dwFourCC == FOURCC_IV31 ||
        dwFourCC == FOURCC_IV32) {
        dwSrcPitch <<= 1;
        dwSysPitch <<= 1;
    }

    dwWidth = (dwWidth + 1) & ~1;      // width must be even
    //if (dwWidth > dwSysPitch)
    //   dwWidth -= 2;
    dwMergeSize = (dwHeight << 16) | (dwWidth >> 1);

    doContrastBlt = (dwContrast != 0xFFFFFFFF);
    doLuma        = (dwBrightness != 0 || bOverContrast);
    doChroma      = (hue != 0 || sat != 0x100);

    if (doChroma) {
        // Apply a chroma vector scale and rotation
        // [U'] = [ sat*cos(hue)  sat*sin(hue)] [U]
        // [V']   [-sat*sin(hue)  sat*cos(hue)] [V]
        short sinhue, coshue;
        __int64 c1, c2;
        static const __int64 bOneTwentyEight = 0x8080808080808080;
        static const __int64 evenMask        = 0x00FF00FF00FF00FF;
        static const __int64 oddMask         = 0xFF00FF00FF00FF00;
        static const __int64 zero            = 0;
        DWORD dwChromaOffset, dwAdjWidth, j;
        LPBYTE qPtr;
        // sin table normalized to 256
        static const short sintable[91] = {  0,  4,  9, 13, 18, 22, 27, 31,
                                            36, 40, 44, 49, 53, 58, 62, 66,
                                            71, 75, 79, 83, 88, 92, 96,100,
                                           104,108,112,116,120,124,128,132,
                                           136,139,143,147,150,154,158,161,
                                           165,168,171,175,178,181,184,187,
                                           190,193,196,199,202,204,207,210,
                                           212,215,217,219,222,224,226,228,
                                           230,232,234,236,237,239,241,242,
                                           243,245,246,247,248,249,250,251,
                                           252,253,254,254,255,255,255,256,
                                           256,256,256 };

        dwChromaOffset = dwSysPitch * dwHeight;

        // move chroma data to system memory
        _asm {emms};

        VppResetNotification(vpp.m_obj_a[V2OSH_FORMAT_NDX].notifier_a, lpProcInfo->hColourControl3);

        vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[V2OSH_FORMAT_NDX].classIID);
        vpp.pPusher->push( 0, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_BUFFER_IN | 0x80000);
        vpp.pPusher->push( 1, vpp.hFromVideoMemContextDma);              // SetContextDmaBufferIn
        vpp.pPusher->push( 2, vpp.hFloatingContextDmaInOverlayShadow);   // SetContextDmaBufferOut
        vpp.pPusher->push( 3, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
        vpp.pPusher->push( 4, dwSrcOffset + (dwYPhase ^ 1));             // OffsetIn
        vpp.pPusher->push( 5, dwChromaOffset);                           // OffsetOut
        vpp.pPusher->push( 6, dwSrcPitch);                               // PitchIn
        vpp.pPusher->push( 7, dwSysPitch);                               // PitchOut
        vpp.pPusher->push( 8, dwWidth);                                  // LineLengthIn
        vpp.pPusher->push( 9, dwHeight);                                 // LineCount
        vpp.pPusher->push(10, 0x102);                                    // Format
        vpp.pPusher->push(11, VPP_NOTIFY_TYPE);                          // BufferNotify
        vpp.pPusher->adjust(12);
        vpp.pPusher->start(TRUE);

        // coefficient calculations
        // possible MMX multiply overflow later, so halve the coefficients now
        // and double the result later with saturation
        if (vpp.colorCtrl.lHue <= 90) {
            sinhue = (short)(((long)sat * sintable[hue]) >> 9);
            coshue = (short)(((long)sat * sintable[90 - hue]) >> 9);
        } else if (vpp.colorCtrl.lHue <= 180) {
            sinhue = (short)(((long)sat * sintable[180 - hue]) >> 9);
            coshue = (short)(((long)sat * -sintable[hue - 90]) >> 9);
        } else if (vpp.colorCtrl.lHue <= 270) {
            sinhue = (short)(((long)sat * -sintable[hue - 180]) >> 9);
            coshue = (short)(((long)sat * -sintable[270 - hue]) >> 9);
        } else {
            sinhue = (short)(((long)sat * -sintable[360 - hue]) >> 9);
            coshue = (short)(((long)sat * sintable[hue - 270]) >> 9);
        }

        // c1 = sat*cos(hue), sat*cos(hue), sat*cos(hue), sat*cos(hue)
        c1 = (__int64)coshue & 0xFFFF;
        c1 |= c1 << 16;
        c1 |= c1 << 32;
        // c2 = -sat*sin(hue), sat*sin(hue), -sat*sin(hue), sat*sin(hue)
        c2 = (__int64)sinhue & 0xFFFF;
        c2 |= ((__int64)-sinhue & 0xFFFF) << 16;
        c2 |= c2 << 32;

        // init MMX const data
        qPtr       = (LPBYTE) (vpp.fpOverlayShadow + dwChromaOffset);
        dwAdjWidth = (dwWidth + 7) / 8;
        _asm {
            movq        mm0, zero
            movq        mm1, bOneTwentyEight
        }

        // while we are doing some host MMX things, overlap with some luma operations
        // since we are main memory throughput bound, DO NOT overlap with operations that touch main memory
        if (doContrastBlt) {
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, vpp.hContextBeta4);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | BETA4_SET_BETA_FACTOR_OFFSET | 0x40000);
            vpp.pPusher->push(3, dwContrast);

            vpp.pPusher->push(4, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(5, vpp.hSurfaces2D);
            vpp.pPusher->push(6, SUB_CHANNEL(vpp.spareSubCh) | SURFACES_2D_PITCH_OFFSET | 0xC0000);
            if (doLuma) {
                // more stuff to do later, do this in place
                vpp.pPusher->push(7, (dwSrcPitch << 16) | dwSrcPitch); // set pitch
                vpp.pPusher->push(8, dwSrcOffset);                     // set offset source
                vpp.pPusher->push(9, dwSrcOffset);                     // set offset destin
                vpp.pDriverData->bltData.dwLastCombinedPitch = (dwSrcPitch << 16) | dwSrcPitch;
                vpp.pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
                vpp.pDriverData->bltData.dwLastDstOffset = dwSrcOffset;
            } else {
                vpp.pPusher->push(7, (dwDstPitch << 16) | dwSrcPitch); // set pitch
                vpp.pPusher->push(8, dwSrcOffset);                     // set offset source
                vpp.pPusher->push(9, dwDstOffset);                     // set offset destin
                vpp.pDriverData->bltData.dwLastCombinedPitch = (dwDstPitch << 16) | dwSrcPitch;
                vpp.pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
                vpp.pDriverData->bltData.dwLastDstOffset = dwDstOffset;
            }

            vpp.pPusher->push(10, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(11, vpp.m_obj_a[ALPHA_BLT_NDX].classIID);
            vpp.pPusher->push(12, SUB_CHANNEL(vpp.spareSubCh) | BLIT_POINT_IN_OFFSET | 0xC0000);
            vpp.pPusher->push(13, 0);                                  // control point in
            vpp.pPusher->push(14, 0);                                  // control point out
            vpp.pPusher->push(15, dwMergeSize);                        // size

            vpp.pPusher->adjust(16);
            vpp.pPusher->start(TRUE);
        }

        if (!doContrastBlt && !doLuma) {
            // luma has not and will not move, do that now
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, vpp.hVideoMemUVtoVideoMemFormat);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
            vpp.pPusher->push(3, dwSrcOffset + dwYPhase);          // OffsetIn
            vpp.pPusher->push(4, dwDstOffset + dwYPhase);          // OffsetOut
            vpp.pPusher->push(5, dwSrcPitch);                      // PitchIn
            vpp.pPusher->push(6, dwDstPitch);                      // PitchOut
            vpp.pPusher->push(7, dwWidth);                         // LineLengthIn
            vpp.pPusher->push(8, dwHeight);                        // LineCount
            vpp.pPusher->push(9, 0x202);                           // Format
            vpp.pPusher->push(10, NV039_BUFFER_NOTIFY_WRITE_ONLY); // BufferNotify
            vpp.pPusher->adjust(11);
            vpp.pPusher->start(TRUE);
        }

        // wait for chroma to arrive in system memory
        _asm {emms};
        // if (IS_EVENT_ERROR(WaitForSingleObject(lpProcInfo->hColourControl3, VPP_TIMEOUT_TIME))) {
        if (VppWaitForNotification(vpp.m_obj_a[V2OSH_FORMAT_NDX].notifier_a, lpProcInfo->hColourControl3, VPP_TIMEOUT_TIME)) {
            dbgError(" *** COLOUR CONTROL event failure ***");
            EVENT_ERROR;
        }

/*      // commented out because this appears to cause a cache coherency problem (bug in BX?)
        // preload the first 8 scan lines into the cache
        for (j=0; j<8; j++) {
            _asm {
                        mov     esi, qPtr
                        mov     ecx, dwAdjWidth
                        shr     ecx, 2
                preld1: mov     eax, [esi]
                        mov     eax, 0
                        add     esi, 32
                        dec     ecx
                        jnz     preld1
            }
            qPtr += dwSysPitch;
        }
*/
        // do the chroma vector scale and rotation, 2.25 cycles/pixel
        // mm0 = 0
        // mm1 = 128
        // mm2 = sign mask
        // mm3 = data quad / expanded data / result high
        // mm4 = expanded data / result low
        // mm5 = expanded data, UV swapped
        // mm6 = UV swapped quad / expanded swapped data
        // mm7 = work space
        if (vpp.regOverlayColourControlEnable == 2) {
            // full chroma resolution
            for (j=0; j<dwHeight; j++) {
                _asm {
                            mov         ecx, dwAdjWidth
                            mov         esi, qPtr
                            mov         edi, qPtr
                            mov         ebx, dwSysPitch
                    ;        shl         ebx, 3
                            movq        mm3, [esi]
                            psubb       mm3, mm1
                            movq        mm2, mm0
                            pcmpgtb     mm2, mm3
                            movq        mm4, mm3
                            movq        mm7, mm3
                            punpcklbw   mm4, mm2
                    nextc:  mov         eax, [esi+ebx]  ; U  ; preload cache for next scan line
                            mov         eax, 0          ; V  ; ensure OOE works
                            pmullw      mm4, c1         ; U  ; mm4 = c1 * low(UV)
                            movq        mm6, mm3        ; V  ; mm6 = copy of quad data-128
                            add         esi, 8          ; U  ; next source address
                            psrlq       mm6, 8          ; V  ; mm6 >>= 8
                            pand        mm6, evenMask   ; U  ; mm6 &= 0x00FF00FF00FF00FF
                            psllq       mm7, 8          ; V  ; mm7 <<= 8
                            pand        mm7, oddMask    ; U  ; mm7 &= 0xFF00FF00FF00FF00
                            punpckhbw   mm3, mm2        ; V  ; mm3 = unpacked signed high data
                            pmullw      mm3, c1         ; U  ; mm3 = c1 * high(UV)
                            por         mm6, mm7        ; V  ; mm6 |= mm7, UV are now swapped
                            movq        mm5, mm6        ; U  ; mm5 = quad data with UV swapped
                            movq        mm2, mm0        ; V  ; mm2 = 0
                            add         edi, 8          ; U  ; next destination address
                            pcmpgtb     mm2, mm6        ; V  ; mm2 = sign bits of swapped UV
                            punpcklbw   mm5, mm2        ; U  ; mm5 = unpacked signed low VU data
                            movq        mm7, mm3        ; V  ; move result to mm7 so we can reuse mm3 early
                            movq        mm3, [esi]      ; U  ; mm3 = new data quad
                            punpckhbw   mm6, mm2        ; V  ; mm6 = unpacked signed high VU data
                            pmullw      mm5, c2         ; U  ; mm5 = c2 * low(VU)
                            movq        mm2, mm0        ; V  ; mm2 = 0
                            pmullw      mm6, c2         ; U  ; mm6 = c2 * high(VU)
                            psubb       mm3, mm1        ; V  ; mm3 -= 128
                            paddsw      mm5, mm4        ; U  ; mm5 += mm4
                            paddsw      mm6, mm7        ; V  ; mm6 += mm7
                            dec         ecx             ; U  ; decrement loop count
                            psraw       mm5, 7          ; V  ; downshift result and multiply by 2
                            psraw       mm6, 7          ; U  ; downshift result and multiply by 2
                            pcmpgtb     mm2, mm3        ; V  ; mm2 = sign bits
                            packsswb    mm5, mm6        ; U  ; mm5 = packed(mm5,mm6)
                            paddb       mm5, mm1        ; V  ; mm5 += 128
                            movq        mm7, mm3        ; U  ; mm7 = copy of quad data-128
                            movq        mm4, mm3        ; V  ; mm4 = copy of quad data-128
                            movq        [edi-8], mm5    ; U  ; store data from mm5
                            punpcklbw   mm4, mm2        ; V  ; mm4 = unpacked signed low UV data
                            jnz         nextc
                }
                qPtr += dwSysPitch;
            }
        } else {
            // vertical chroma subsample
            // in this case, ESI is used for source and in-place destination, and EDI is used for next line in destination
            for (j=0; j<dwHeight; j+=2) {
                _asm {
                            mov         ecx, dwAdjWidth
                            mov         esi, qPtr
                            mov         edi, qPtr
                            add         edi, dwSysPitch
                            mov         ebx, dwSysPitch
                            shl         ebx, 1
                    ;        shl         ebx, 3
                            movq        mm3, [esi]
                            psubb       mm3, mm1
                            movq        mm2, mm0
                            pcmpgtb     mm2, mm3
                            movq        mm4, mm3
                            movq        mm7, mm3
                            punpcklbw   mm4, mm2
                    nextc2: mov         eax, [esi+ebx]  ; U  ; preload cache for next scan line
                            mov         eax, 0          ; V  ; ensure OOE works
                            pmullw      mm4, c1         ; U  ; mm4 = c1 * low(UV)
                            movq        mm6, mm3        ; V  ; mm6 = copy of quad data-128
                            add         esi, 8          ; U  ; next source address
                            psrlq       mm6, 8          ; V  ; mm6 >>= 8
                            pand        mm6, evenMask   ; U  ; mm6 &= 0x00FF00FF00FF00FF
                            psllq       mm7, 8          ; V  ; mm7 <<= 8
                            pand        mm7, oddMask    ; U  ; mm7 &= 0xFF00FF00FF00FF00
                            punpckhbw   mm3, mm2        ; V  ; mm3 = unpacked signed high data
                            pmullw      mm3, c1         ; U  ; mm3 = c1 * high(UV)
                            por         mm6, mm7        ; V  ; mm6 |= mm7, UV are now swapped
                            movq        mm5, mm6        ; U  ; mm5 = quad data with UV swapped
                            movq        mm2, mm0        ; V  ; mm2 = 0
                            add         edi, 8          ; U  ; next destination address
                            pcmpgtb     mm2, mm6        ; V  ; mm2 = sign bits of swapped UV
                            movq        mm7, mm3        ; U  ; move result to mm7 so we can reuse mm3 early
                            punpcklbw   mm5, mm2        ; V  ; mm5 = unpacked signed low VU data
                            movq        mm3, [esi]      ; U  ; mm3 = new data quad
                            punpckhbw   mm6, mm2        ; V  ; mm6 = unpacked signed high VU data
                            pmullw      mm5, c2         ; U  ; mm5 = c2 * low(VU)
                            movq        mm2, mm0        ; V  ; mm2 = 0
                            pmullw      mm6, c2         ; U  ; mm6 = c2 * high(VU)
                            psubb       mm3, mm1        ; V  ; mm3 -= 128
                            paddsw      mm5, mm4        ; U  ; mm5 += mm4
                            paddsw      mm6, mm7        ; V  ; mm6 += mm7
                            dec         ecx             ; U  ; decrement loop count
                            psraw       mm5, 7          ; V  ; downshift result and multiply by 2
                            psraw       mm6, 7          ; U  ; downshift result and multiply by 2
                            pcmpgtb     mm2, mm3        ; V  ; mm2 = sign bits
                            packsswb    mm5, mm6        ; U  ; mm5 = packed(mm5,mm6)
                            paddb       mm5, mm1        ; V  ; mm5 += 128
                            movq        mm7, mm3        ; U  ; mm7 = copy of quad data-128
                            movq        mm4, mm3        ; V  ; mm4 = copy of quad data-128
                            movq        [edi-8], mm5    ; U  ; store data from mm5
                            punpcklbw   mm4, mm2        ; V  ; mm4 = unpacked signed low UV data
                            movq        [esi-8], mm5    ; U  ; store same data on next line
                            jnz         nextc2          ; V
                }
                qPtr += dwSysPitch << 1;
            }
        }

        _asm {emms};

        VppResetNotification(vpp.m_obj_a[OSH2V_FORMAT_NDX].notifier_a, lpProcInfo->hColourControl3);

        // move chroma data to back to video memory

        vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[OSH2V_FORMAT_NDX].classIID);
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_BUFFER_IN | 0x80000);
        vpp.pPusher->push(1, vpp.hFloatingContextDmaInOverlayShadow);          // SetContextDmaBufferIn
        vpp.pPusher->push(2, vpp.hToVideoMemContextDma);                       // SetContextDmaBufferOut
        vpp.pPusher->push(3, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
        vpp.pPusher->push(4, dwChromaOffset);                                  // OffsetIn
        vpp.pPusher->push(5, dwDstOffset + (dwYPhase ^ 1));                    // OffsetOut
        vpp.pPusher->push(6, dwSysPitch);                                      // PitchIn
        vpp.pPusher->push(7, dwDstPitch);                                      // PitchOut
        vpp.pPusher->push(8, dwWidth);                                         // LineLengthIn
        vpp.pPusher->push(9, dwHeight);                                        // LineCount
        vpp.pPusher->push(10, 0x201);                                          // Format
        if (!doLuma) {
            vpp.pPusher->push(11, VPP_NOTIFY_TYPE);                            // BufferNotify
        } else {
            vpp.pPusher->push(11, NV039_BUFFER_NOTIFY_WRITE_ONLY);             // BufferNotify
        }
        vpp.pPusher->adjust(12);
        vpp.pPusher->start(TRUE);

        vpp.pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[OSH2V_FORMAT_NDX].classIID;

    } else if (doContrastBlt) {
        // Contrast Blit, since the hardware can only do multiplies up to one (0xFF),
        // and if the contrast is from 100% to 200%, then divide contrast by 2 and worry
        // about left shift and saturation in the MMX routines below
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.hContextBeta4);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | BETA4_SET_BETA_FACTOR_OFFSET | 0x40000);
        vpp.pPusher->push(3, dwContrast);

        vpp.pPusher->push(4, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(5, vpp.hSurfaces2D);
        vpp.pPusher->push(6, SUB_CHANNEL(vpp.spareSubCh) | SURFACES_2D_PITCH_OFFSET | 0xC0000);
        if (doLuma) {
            // more stuff to do later, do this in place
            vpp.pPusher->push(7, (dwSrcPitch << 16) | dwSrcPitch); // set pitch
            vpp.pPusher->push(8, dwSrcOffset);                     // set offset source
            vpp.pPusher->push(9, dwSrcOffset);                     // set offset destin
            vpp.pDriverData->bltData.dwLastCombinedPitch = (dwSrcPitch << 16) | dwSrcPitch;
            vpp.pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
            vpp.pDriverData->bltData.dwLastDstOffset = dwSrcOffset;
        } else {
            vpp.pPusher->push(7, (dwDstPitch << 16) | dwSrcPitch); // set pitch
            vpp.pPusher->push(8, dwSrcOffset);                     // set offset source
            vpp.pPusher->push(9, dwDstOffset);                     // set offset destin
            vpp.pDriverData->bltData.dwLastCombinedPitch = (dwDstPitch << 16) | dwSrcPitch;
            vpp.pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
            vpp.pDriverData->bltData.dwLastDstOffset = dwDstOffset;
        }

        vpp.pPusher->push(10, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(11, vpp.m_obj_a[ALPHA_BLT_NDX].classIID);
        vpp.pPusher->push(12, SUB_CHANNEL(vpp.spareSubCh) | BLIT_POINT_IN_OFFSET | 0xC0000);
        vpp.pPusher->push(13, 0);                                  // control point in
        vpp.pPusher->push(14, 0);                                  // control point out
        vpp.pPusher->push(15, dwMergeSize);                        // size
        vpp.pPusher->adjust(16);

        if (!doLuma) {
            // this is the only operation, be sure to trigger an event
            _asm {emms};

            VppResetNotification(vpp.m_obj_a[V2OSH_FORMAT_NDX].notifier_a, lpProcInfo->hColourControl3);

            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | NV05F_NOTIFY | 0x40000);
            vpp.pPusher->push(1, VPP_NOTIFY_TYPE);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV05F_NO_OPERATION | 0x40000);
            vpp.pPusher->push(3, 0);
            vpp.pPusher->adjust(4);
        }
        vpp.pPusher->start(TRUE);
    }

    // OverContrast and Brightness calculations
    if (doLuma) {
        static const __int64 shiftMask   = 0xFEFEFEFEFEFEFEFE;
        static const __int64 cmpOverflow = 0xFFFFFFFFFFFFFFFF;
        DWORD j, dwAdjWidth;
        LPBYTE qPtr;

        // move Y data to system memory
        _asm {emms};

        VppResetNotification(vpp.m_obj_a[V2OSH_FORMAT_NDX].notifier_a, lpProcInfo->hColourControl3);

        vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[V2OSH_FORMAT_NDX].classIID);
        vpp.pPusher->push( 0, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_BUFFER_IN | 0x80000);
        vpp.pPusher->push( 1, vpp.hFromVideoMemContextDma);           // SetContextDmaBufferIn
        vpp.pPusher->push( 2, vpp.hFloatingContextDmaInOverlayShadow);// SetContextDmaBufferOut
        vpp.pPusher->push( 3, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
        vpp.pPusher->push( 4, dwSrcOffset + dwYPhase);                // OffsetIn
        vpp.pPusher->push( 5, 0);                                     // OffsetOut
        vpp.pPusher->push( 6, dwSrcPitch);                            // PitchIn
        vpp.pPusher->push( 7, dwSysPitch);                            // PitchOut
        vpp.pPusher->push( 8, dwWidth);                               // LineLengthIn
        vpp.pPusher->push( 9, dwHeight);                              // LineCount
        vpp.pPusher->push(10, 0x102);                                 // Format
        vpp.pPusher->push(11, VPP_NOTIFY_TYPE);                       // BufferNotify
        vpp.pPusher->adjust(12);
        vpp.pPusher->start(TRUE);

        if (!doChroma) {
            // chroma has not and will not move, do that now
            vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
            vpp.pPusher->push(1, vpp.hVideoMemUVtoVideoMemFormat);
            vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
            vpp.pPusher->push(3, dwSrcOffset + (dwYPhase ^ 1));    // OffsetIn
            vpp.pPusher->push(4, dwDstOffset + (dwYPhase ^ 1));    // OffsetOut
            vpp.pPusher->push(5, dwSrcPitch);                      // PitchIn
            vpp.pPusher->push(6, dwDstPitch);                      // PitchOut
            vpp.pPusher->push(7, dwWidth);                         // LineLengthIn
            vpp.pPusher->push(8, dwHeight);                        // LineCount
            vpp.pPusher->push(9, 0x202);                           // Format
            vpp.pPusher->push(10, NV039_BUFFER_NOTIFY_WRITE_ONLY); // BufferNotify
            vpp.pPusher->adjust(11);
            vpp.pPusher->start(TRUE);
        }

        // init MMX const data
        qPtr       = (LPBYTE) vpp.fpOverlayShadow;
        dwAdjWidth = (dwWidth + 15) / 16;
        _asm {
            movd        mm0, dwBrightness
            punpckldq   mm0, dwBrightness
            movq        mm2, shiftMask
            movq        mm3, cmpOverflow
        }

        // wait for luma data to arrive in system memory
        _asm {emms};
        // if (IS_EVENT_ERROR(WaitForSingleObject(lpProcInfo->hColourControl3, VPP_TIMEOUT_TIME))) {
        if (VppWaitForNotification(vpp.m_obj_a[V2OSH_FORMAT_NDX].notifier_a, lpProcInfo->hColourControl3, VPP_TIMEOUT_TIME)) {
            dbgError(" *** COLOUR CONTROL event failure ***");
            EVENT_ERROR;
        }
/*
        // preload the first 8 scan lines into the cache
        for (j=0; j<8; j++) {
            _asm {
                        mov     esi, qPtr
                        mov     ecx, dwAdjWidth
                        shr     ecx, 1
                preld2: mov     eax, [esi]
                        mov     eax, 0
                        add     esi, 32
                        dec     ecx
                        jnz     preld2
            }
            qPtr += dwSysPitch;
        }
*/
        // do brightness and/or overcontrast
        // mm0 = brightness
        // mm1 = quad data chunk
        // mm2 = shift mask
        // mm3 = overflow compare value
        // mm4 = compare register
        // mm5 = next quad data chunk
        // mm6 = next compare register
        if (vpp.colorCtrl.lBrightness >= 0) {
            if (bOverContrast) {
                // overcontrast and positive brightness, 0.6875 cycles/pixel
                // AND is commented out for performance -> noise in the lowest bit of precision
                for (j=0; j<dwHeight; j++) {
                    _asm {
                                mov     ecx, dwAdjWidth
                                mov     esi, qPtr
                                mov     edi, qPtr
                                mov     ebx, dwSysPitch
                       ;         shl     ebx, 3
                                movq    mm1, [esi]
                                movq    mm4, mm3
                        nextq1: mov     eax, [esi+ebx]  ; U     ; preload cache for next scan line
                                mov     eax, 0          ; V     ; ensure OOE works
                                movq    mm5, [esi+8]    ; Ui+   ; mm5 = data[i+1]
                                pcmpgtb mm4, mm1        ; Vi    ; mm4 = sign bit of data[i]
                                add     esi, 16         ; U     ; next source address
                                movq    mm6, mm3        ; Vi+   ; mm6 = overflow compare value
                                add     edi, 16         ; U     ; next destination address
                                psll    mm1, 1          ; Vi    ; mm1 <<= 1
                                pcmpgtb mm6, mm5        ; Ui+   ; mm6 = sign bit of data[i+1]
                       ;         pand    mm1, mm2        ; i    ; mm1 &= 0xFE (since there is no PSLLB)
                                por     mm4, mm1        ; Vi    ; mm4 |= mm1 (saturated result[i])

                                psll    mm5, 1          ; Ui+   ; mm5 <<= 1
                                paddusb mm4, mm0        ; Vi    ; mm4 += brightness
                       ;         pand    mm5, mm2        ; i+   ; mm5 &= 0xFE
                                movq    [edi-16], mm4   ; Ui    ; store result[i]
                                por     mm6, mm5        ; Vi+   ; mm6 |= mm5 (saturated result[i+1])
                                movq    mm1, [esi]      ; Ui    ; mm1 = data[i]
                                paddusb mm6, mm0        ; Vi+   ; mm6 += brightness
                                dec     ecx             ; U     ; decrement loop count
                                movq    mm4, mm3        ; Vi    ; mm4 = overflow compare value
                                movq    [edi-8], mm6    ; Ui+   ; store result[i+1]
                                jnz     nextq1          ; V
                    }
                    qPtr += dwSysPitch;
                }
            } else {
                // positive brightness only
                for (j=0; j<dwHeight; j++) {
                    _asm {
                                mov     ecx, dwAdjWidth
                                mov     esi, qPtr
                                mov     edi, qPtr
                                mov     ebx, dwSysPitch
                        ;        shl     ebx, 3
                                movq    mm1, [esi]
                        nextq2: mov     eax, [esi+ebx]  ; U     ; preload cache for next scan line
                                mov     eax, 0          ; V     ; ensure OOE works
                                movq    mm5, [esi+8]    ; Ui+   ; mm5 = data[i+1]
                                paddusb mm1, mm0        ; Vi    ; mm1 += brightness
                                add     esi, 16         ; U     ; next source address
                                add     edi, 16         ; V     ; next destination address
                                dec     ecx             ; U     ; decrement loop count
                                movq    [edi-16], mm1   ; Ui    ; store result[i]
                                paddusb mm5, mm0        ; Vi+   ; mm5 += brightness
                                movq    [edi-8], mm5    ; Ui+   ; store result[i+1]
                                movq    mm1, [esi]      ; Ui    ; mm1 = data[i]
                                jnz     nextq2          ; V
                    }
                    qPtr += dwSysPitch;
                }
            }
        } else {
            if (bOverContrast) {
                // overcontrast and negative brightness
                for (j=0; j<dwHeight; j++) {
                    _asm {
                                mov     ecx, dwAdjWidth
                                mov     esi, qPtr
                                mov     edi, qPtr
                                mov     ebx, dwSysPitch
                        ;        shl     ebx, 3
                                movq    mm1, [esi]
                                movq    mm4, mm3
                        nextq3: mov     eax, [esi+ebx]  ; U     ; preload cache for next scan line
                                mov     eax, 0          ; V     ; ensure OOE works
                                movq    mm5, [esi+8]    ; Ui+   ; mm5 = data[i+1]
                                pcmpgtb mm4, mm1        ; Vi    ; mm4 = sign bit of data[i]
                                add     esi, 16         ; U     ; next source address
                                movq    mm6, mm3        ; Vi+   ; mm6 = overflow compare value
                                add     edi, 16         ; U     ; next destination address
                                psll    mm1, 1          ; Vi    ; mm1 <<= 1
                                pcmpgtb mm6, mm5        ; Ui+   ; mm6 = sign bit of data[i+1]
                       ;         pand    mm1, mm2        ; i    ; mm1 &= 0xFE (since there is no PSLLB)
                                por     mm4, mm1        ; Vi    ; mm4 |= mm1 (saturated result[i])

                                psll    mm5, 1          ; Ui+   ; mm5 <<= 1
                                psubusb mm4, mm0        ; Vi    ; mm4 -= brightness
                       ;         pand    mm5, mm2        ; i+   ; mm5 &= 0xFE
                                movq    [edi-16], mm4   ; Ui    ; store result[i]
                                por     mm6, mm5        ; Vi+   ; mm6 |= mm5 (saturated result[i+1])
                                movq    mm1, [esi]      ; Ui    ; mm1 = data[i]
                                psubusb mm6, mm0        ; Vi+   ; mm6 -= brightness
                                dec     ecx             ; U     ; decrement loop count
                                movq    mm4, mm3        ; Vi    ; mm4 = overflow compare value
                                movq    [edi-8], mm6    ; Ui+   ; store result[i+1]
                                jnz     nextq3          ; V
                    }
                    qPtr += dwSysPitch;
                }
            } else {
                // negative brightness only
                for (j=0; j<dwHeight; j++) {
                    _asm {
                                mov     ecx, dwAdjWidth
                                mov     esi, qPtr
                                mov     edi, qPtr
                                mov     ebx, dwSysPitch
                        ;        shl     ebx, 3
                                movq    mm1, [esi]
                        nextq4: mov     eax, [esi+ebx]  ; U     ; preload cache for next scan line
                                mov     eax, 0          ; V     ; ensure OOE works
                                movq    mm5, [esi+8]    ; Ui+   ; mm5 = data[i+1]
                                psubusb mm1, mm0        ; Vi    ; mm1 -= brightness
                                add     esi, 16         ; U     ; next source address
                                add     edi, 16         ; V     ; next destination address
                                dec     ecx             ; U     ; decrement loop count
                                movq    [edi-16], mm1   ; Ui    ; store result[i]
                                psubusb mm5, mm0        ; Vi+   ; mm5 -= brightness
                                movq    [edi-8], mm5    ; Ui+   ; store result[i+1]
                                movq    mm1, [esi]      ; Ui    ; mm1 = data[i]
                                jnz     nextq4          ; V
                    }
                    qPtr += dwSysPitch;
                }
            }
        }

        // move Y data to back to video memory
        _asm {emms};

        VppResetNotification(vpp.m_obj_a[OSH2V_FORMAT_NDX].notifier_a, lpProcInfo->hColourControl3);

        vpp.pPusher->setObject(vpp.spareSubCh,vpp.m_obj_a[OSH2V_FORMAT_NDX].classIID);
        vpp.pPusher->push( 0, SUB_CHANNEL(vpp.spareSubCh) | NV039_SET_CONTEXT_DMA_BUFFER_IN | 0x80000);
        vpp.pPusher->push( 1, vpp.hFloatingContextDmaInOverlayShadow);// SetContextDmaBufferIn
        vpp.pPusher->push( 2, vpp.hToVideoMemContextDma);             // SetContextDmaBufferOut
        vpp.pPusher->push( 3, SUB_CHANNEL(vpp.spareSubCh) | MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000);
        vpp.pPusher->push( 4, 0);                                     // OffsetIn
        vpp.pPusher->push( 5, dwDstOffset + dwYPhase);                // OffsetOut
        vpp.pPusher->push( 6, dwSysPitch);                            // PitchIn
        vpp.pPusher->push( 7, dwDstPitch);                            // PitchOut
        vpp.pPusher->push( 8, dwWidth);                               // LineLengthIn
        vpp.pPusher->push( 9, dwHeight);                              // LineCount
        vpp.pPusher->push(10, 0x201);                                 // Format
        vpp.pPusher->push(11, VPP_NOTIFY_TYPE);                       // BufferNotify
        vpp.pPusher->adjust(12);
        vpp.pPusher->start(TRUE);

        vpp.pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[OSH2V_FORMAT_NDX].classIID;
    }

    _asm {emms};
    return doContrastBlt || doLuma || doChroma;
}


//---------------------------------------------------------------------------
// vppNv20Deinterlace
//      Deinterlace filter for NV20.  Surface dimensions do not have to be a
//      power of 2.  Texture does not have to be swizzled.
LOCAL  BOOL vppNv20Deinterlace(Vpp_t *pVpp, LPPROCESSINFO lpProcInfo,
                        DWORD dwSrcOffset, DWORD dwSrcPitch,
                        DWORD dwDstOffset, DWORD dwDstPitch,
                        DWORD dwWidth, DWORD dwHeight, DWORD dwFlags)
{
    DWORD dwMergeSize;
    BOOL isField, isOddField;
    DWORD dwCombineFactor;
    int dwOppositeYOffset, dwCurrYOffset, dwDestYOffset;
    DWORD dwOppositeFieldOffset;
    Vpp_t &vpp = *pVpp;
    NvU32 subchannel = SUB_CHANNEL(vpp.threeDClassSubCh);

    nvAssert(vpp.dwFlags & VPP_FLAG_KELVIN_3D);

    // Let D3D code know that we have touched NV
    vpp.pDriverData->TwoDRenderingOccurred = 1;

    isField = IS_FIELD(dwFlags);
    isOddField = isField && (dwFlags & VPP_ODD);

    if (!isField || !(dwFlags & VPP_INTERLEAVED)) {
        return FALSE;
    }

    dwCombineFactor = (vpp.regOverlayMode >> 16) & 0xFF;
    dwCombineFactor |= (dwCombineFactor << 8);
    dwCombineFactor |= (dwCombineFactor << 16);

    dwOppositeFieldOffset = dwSrcOffset;

    if (dwFlags & VPP_ODD) {
        dwSrcOffset += dwSrcPitch;
        dwDstOffset += dwDstPitch;
    } else {
        dwOppositeFieldOffset += dwSrcPitch;
    }

    dwSrcPitch <<= 1;
    dwDstPitch <<= 1;
    dwHeight >>= 1;

    // convert RGB16 to RGB32 width
    dwWidth >>= 1;

    dwMergeSize = (dwWidth << 16) | dwHeight;

    nvAssert(vpp.pThreeDClassLastUser);
    if (*vpp.pThreeDClassLastUser != MODULE_ID_DDRAW_VPP)
    {
        if (!vppInitKelvin(&vpp)) {
            return FALSE;
        }
    }

    vpp.pPusher->push(0, subchannel | NV097_SET_SURFACE_CLIP_HORIZONTAL | 0x40000);
    vpp.pPusher->push(1, ((dwWidth << 16) | 0));
    vpp.pPusher->push(2, subchannel | NV097_SET_SURFACE_CLIP_VERTICAL | 0x40000);
    vpp.pPusher->push(3, ((dwHeight << 16) | 0));
    vpp.pPusher->adjust(4);

    vpp.pPusher->push(0, subchannel | NV097_SET_SURFACE_PITCH | 0x40000);
    vpp.pPusher->push(1, (dwDstPitch << 16) | dwDstPitch);
    vpp.pPusher->adjust(2);

    // Load combination factors
    vpp.pPusher->push(0, subchannel | NV097_SET_COMBINER_FACTOR0(0) | 0x40000);
    vpp.pPusher->push(1, dwCombineFactor);     // % of current field
    vpp.pPusher->push(2, subchannel | NV097_SET_COMBINER_FACTOR1(0) | 0x40000);
    vpp.pPusher->push(3, dwCombineFactor);
    vpp.pPusher->adjust(4);

    // Make sure offset is 128-byte aligned (in debug builds)
    nvAssert(!((dwSrcOffset % 128) || (dwOppositeFieldOffset & ~0x1) % 128));

    vpp.pPusher->push(0, subchannel | NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
    vpp.pPusher->push(1, dwSrcOffset);
    vpp.pPusher->push(2, subchannel | NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
    vpp.pPusher->push(3, dwSrcOffset);//dwOppositeFieldOffset & ~0x1);
    vpp.pPusher->push(4, subchannel | NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
    vpp.pPusher->push(5, dwDstOffset);
    vpp.pPusher->adjust(6);

    // IMPORTANT: Even if we are not using texture units 2 and 3, we need to give
    // them some kind of value other than zero in the following commands.
    // Without these, the hardware will behave incorrectly, probably exhibiting
    // a problem that looks like an invalid surface pitch.
    vpp.pPusher->push(0, subchannel | NV097_SET_TEXTURE_IMAGE_RECT(0) | 0x40000);
    vpp.pPusher->push(1, dwMergeSize);
    vpp.pPusher->push(2, subchannel | NV097_SET_TEXTURE_IMAGE_RECT(1) | 0x40000);
    vpp.pPusher->push(3, dwMergeSize);
    vpp.pPusher->push(4, subchannel | NV097_SET_TEXTURE_IMAGE_RECT(2) | 0x40000);
    vpp.pPusher->push(5, 8 << 16 | 8);
    vpp.pPusher->push(6, subchannel | NV097_SET_TEXTURE_IMAGE_RECT(3) | 0x40000);
    vpp.pPusher->push(7, 8 << 16 | 8);
    vpp.pPusher->push(8, subchannel | NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
    vpp.pPusher->push(9, dwSrcPitch << 16);
    vpp.pPusher->push(10, subchannel | NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
    vpp.pPusher->push(11, dwSrcPitch << 16);
    vpp.pPusher->push(12, subchannel | NV097_SET_TEXTURE_CONTROL1(2) | 0x40000);
    vpp.pPusher->push(13, 8 << 16 | 8);
    vpp.pPusher->push(14, subchannel | NV097_SET_TEXTURE_CONTROL1(3) | 0x40000);
    vpp.pPusher->push(15, 8 << 16 | 8);
    vpp.pPusher->adjust(16);

    vpp.pPusher->push(0, subchannel +
                         NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0) | 0x340000);
    vpp.pPusher->push(1, 0);
    vpp.pPusher->push(2, 0);
    vpp.pPusher->push(3, 0);
    vpp.pPusher->push(4, 0);
    vpp.pPusher->push(5, 0);
    vpp.pPusher->push(6, 0);
    vpp.pPusher->push(7, 0);
    vpp.pPusher->push(8, 0);
    vpp.pPusher->push(9, 0);
    vpp.pPusher->push(10, 4);
    vpp.pPusher->push(11, 8);
    vpp.pPusher->push(12, 0);
    vpp.pPusher->push(13, 0);
    vpp.pPusher->push(14, subchannel +
                          NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0) | 0x340000);
    vpp.pPusher->push(15, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(16, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(17, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(18, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(19, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(20, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(21, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(22, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(23, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(24, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(25, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(26, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(27, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->adjust(28);

    // specify height and width in 1/4 pixel units
    dwHeight <<= 2;
    dwWidth <<= 2;

    if (dwFlags & VPP_ODD) {
        dwOppositeYOffset = +1;
        dwCurrYOffset     = -1;
        dwDestYOffset     = -1;
    } else {
        dwOppositeYOffset = -1;
        dwCurrYOffset     = +1;
        dwDestYOffset     = +1;
    }

    vpp.pPusher->push(0, subchannel | NV097_SET_BEGIN_END | 0x40000);
    vpp.pPusher->push(1, NV097_SET_BEGIN_END_OP_QUADS);
    vpp.pPusher->push(2, subchannel | NV097_INLINE_ARRAY | 0x40300000);

    vpp.pPusher->push(3, (dwDestYOffset << 16) | 0);   // destination
    vpp.pPusher->push(4, (dwCurrYOffset << 16) | 0);   // current field
    vpp.pPusher->push(5, (dwOppositeYOffset << 16) | 0);   // opposite field
    
    vpp.pPusher->push(6, ((dwHeight+dwDestYOffset) << 16) | 0);
    vpp.pPusher->push(7, ((dwHeight+dwCurrYOffset) << 16) | 0);
    vpp.pPusher->push(8, ((dwHeight+dwOppositeYOffset) << 16) | 0);
    
    vpp.pPusher->push(9, ((dwHeight+dwDestYOffset) << 16) | dwWidth);
    vpp.pPusher->push(10, ((dwHeight+dwCurrYOffset) << 16) | dwWidth);
    vpp.pPusher->push(11,  ((dwHeight+dwOppositeYOffset) << 16) | dwWidth);
    
    vpp.pPusher->push(12, (+dwDestYOffset << 16) | dwWidth);
    vpp.pPusher->push(13, (+dwCurrYOffset << 16) | dwWidth);
    vpp.pPusher->push(14, (+dwOppositeYOffset << 16) | dwWidth);
    vpp.pPusher->adjust(15);

    VppResetNotification(NULL, lpProcInfo->hDFilter3);

    vpp.pPusher->push(0, subchannel | NV097_NOTIFY | 0x40000);
    vpp.pPusher->push(1, VPP_NOTIFY_TYPE);
    vpp.pPusher->push(2, subchannel | NV097_SET_BEGIN_END | 0x40000);
    vpp.pPusher->push(3, NV097_SET_BEGIN_END_OP_END);
    vpp.pPusher->adjust(4);

    vpp.pPusher->start(TRUE);

    return TRUE;
}


//---------------------------------------------------------------------------
// vppNv10Deinterlace
//      Deinterlace filter for NV10.  Surface dimensions do not have to be a
//      power of 2.  Texture does not have to be swizzled.
LOCAL BOOL vppNv10Deinterlace(Vpp_t *pVpp, LPPROCESSINFO lpProcInfo,
                        DWORD dwSrcOffset, DWORD dwSrcPitch,
                        DWORD dwDstOffset, DWORD dwDstPitch,
                        DWORD dwWidth, DWORD dwHeight, DWORD dwFlags)
{
    DWORD dwMergeSize;
    BOOL isField, isOddField;
    DWORD dwCombineFactor;
    unsigned long vertexSizeContext;
    int dwOppositeYOffset, dwCurrYOffset, dwDestYOffset;
    DWORD dwOppositeFieldOffset;
    Vpp_t &vpp = *pVpp;
    NvU32 subchannel = SUB_CHANNEL(vpp.threeDClassSubCh);

    nvAssert (NVARCH >= 0x10);

    isField = IS_FIELD(dwFlags);
    isOddField = isField && (dwFlags & VPP_ODD);

    if (!isField || !(dwFlags & VPP_INTERLEAVED)) {
        return FALSE;
    }

    dwCombineFactor = (vpp.regOverlayMode >> 16) & 0xFF;
    dwCombineFactor |= (dwCombineFactor << 8);
    dwCombineFactor |= (dwCombineFactor << 16);

    if (dwFlags & VPP_ODD) {
        dwOppositeFieldOffset = dwSrcOffset;
        dwSrcOffset += dwSrcPitch;
        dwDstOffset += dwDstPitch;
    } else {
        dwOppositeFieldOffset = dwSrcOffset + dwSrcPitch;
    }
    dwSrcPitch <<= 1;
    dwDstPitch <<= 1;
    dwHeight >>= 1;

    // convert RGB16 to RGB32 width
    dwWidth >>= 1;

    dwMergeSize = (dwWidth << 16) | dwHeight;

    nvAssert(vpp.pThreeDClassLastUser);
    if (*vpp.pThreeDClassLastUser != MODULE_ID_DDRAW_VPP)
    {
        if (!vppInitCelsius(&vpp)) {
            return FALSE;
        }
    }

    vpp.pPusher->push(0, subchannel | NV056_SET_SURFACE_CLIP_HORIZONTAL | 0x40000);
    vpp.pPusher->push(1, ((dwWidth << 16) | 0));

    vpp.pPusher->push(2, subchannel +
                   NV056_SET_SURFACE_CLIP_VERTICAL | 0x40000);
    vpp.pPusher->push(3, ((dwHeight << 16) | 0));
    vpp.pPusher->adjust(4);

    vpp.pPusher->push(0, subchannel | NV056_SET_SURFACE_PITCH | 0x40000);
    vpp.pPusher->push(1, (dwDstPitch << 16) | dwDstPitch);
    vpp.pPusher->adjust(2);

    // Load combination factors
    vpp.pPusher->push(0, subchannel | NV056_SET_COMBINE_FACTOR(0) | 0x80000);
    vpp.pPusher->push(1, dwCombineFactor);     // % of current field
    vpp.pPusher->push(2, dwCombineFactor);
    vpp.pPusher->adjust(3);

    vpp.pPusher->push(0, subchannel | NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
    vpp.pPusher->push(1, dwSrcOffset);
    vpp.pPusher->push(2, dwOppositeFieldOffset & ~0x1);
    vpp.pPusher->push(3, subchannel | NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
    vpp.pPusher->push(4, dwDstOffset);
    vpp.pPusher->adjust(5);

    vpp.pPusher->push(0, subchannel | NV056_SET_TEXTURE_IMAGE_RECT(0) | 0x80000);
    vpp.pPusher->push(1, dwMergeSize);
    vpp.pPusher->push(2, dwMergeSize);
    vpp.pPusher->push(3, subchannel | NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
    vpp.pPusher->push(4, dwSrcPitch << 16);
    vpp.pPusher->push(5, dwSrcPitch << 16);
    vpp.pPusher->adjust(6);

    vertexSizeContext = NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_2 | \
                        (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4) | \
                        (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 8) | \
                        (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_2 << 12) | \
                        (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_2 << 16) | \
                        (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 20) | \
                        (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 24) | \
                        (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 28); \
    vpp.pPusher->push(0, subchannel | NV056_SET_INVERSE_MODEL_VIEW_MATRIX1(15) | 0x40000); \
    vpp.pPusher->push(1, vertexSizeContext); \
    vpp.pPusher->push(2, subchannel | NV056_SET_VERTEX_ARRAY_OFFSET | 0x400000); \
    vpp.pPusher->push(3, 0); \
    vpp.pPusher->push(4, (NV056_SET_VERTEX_ARRAY_FORMAT_W_NONE << 24) | \
                  (12 << 8) | \
                  (NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_2 << 4) | \
                   NV056_SET_VERTEX_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->push(5, 0); \
    vpp.pPusher->push(6, (12 << 8) | \
                  (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4) | \
                   NV056_SET_DIFFUSE_ARRAY_FORMAT_TYPE_UNSIGNED_BYTE_BGRA); \
    vpp.pPusher->push(7, 0); \
    vpp.pPusher->push(8, (12 << 8) | \
                  (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 4) | \
                   NV056_SET_SPECULAR_ARRAY_FORMAT_TYPE_UNSIGNED_BYTE_BGRA); \
    vpp.pPusher->push(9, 4); \
    vpp.pPusher->push(10, (12 << 8) | \
                  (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_2 << 4) | \
                   NV056_SET_TEX_COORD0_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->push(11, 8); \
    vpp.pPusher->push(12, (12 << 8) | \
                   (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_2 << 4) | \
                    NV056_SET_TEX_COORD1_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->push(13, 0); \
    vpp.pPusher->push(14, (12 << 8) | \
                   (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 4) | \
                    NV056_SET_NORMAL_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->push(15, 0); \
    vpp.pPusher->push(16, (12 << 8) | \
                   (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 4) | \
                    NV056_SET_WEIGHT_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->push(17, 0); \
    vpp.pPusher->push(18, (12 << 8) | \
                   (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 4) | \
                    NV056_SET_FOG_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->adjust(19); \

    // specify height and width in 1/4 pixel units
    dwHeight <<= 2;
    dwWidth <<= 2;

    if (dwFlags & VPP_ODD) {
        dwOppositeYOffset = +1;
        dwCurrYOffset     = -1;
        dwDestYOffset     = -1;
    } else {
        dwOppositeYOffset = -1;
        dwCurrYOffset     = +1;
        dwDestYOffset     = +1;
    }

    vpp.pPusher->push(0, subchannel | NV056_SET_BEGIN_END | 0x40000);
    vpp.pPusher->push(1, NV056_SET_BEGIN_END_OP_QUADS);
    vpp.pPusher->push(2, subchannel | NV056_INLINE_ARRAY(0) | 0x300000);

    vpp.pPusher->push(3, (dwOppositeYOffset << 16) | 0);   // opposite field
    vpp.pPusher->push(4, (dwCurrYOffset << 16) | 0);   // current field
    vpp.pPusher->push(5, (dwDestYOffset << 16) | 0);   // destination

    vpp.pPusher->push(6, ((dwHeight+dwOppositeYOffset) << 16) | 0);
    vpp.pPusher->push(7, ((dwHeight+dwCurrYOffset) << 16) | 0);
    vpp.pPusher->push(8, ((dwHeight+dwDestYOffset) << 16) | 0);

    vpp.pPusher->push(9,  ((dwHeight+dwOppositeYOffset) << 16) | dwWidth);
    vpp.pPusher->push(10, ((dwHeight+dwCurrYOffset) << 16) | dwWidth);
    vpp.pPusher->push(11, ((dwHeight+dwDestYOffset) << 16) | dwWidth);

    vpp.pPusher->push(12, (+dwOppositeYOffset << 16) | dwWidth);
    vpp.pPusher->push(13, (+dwCurrYOffset << 16) | dwWidth);
    vpp.pPusher->push(14, (+dwDestYOffset << 16) | dwWidth);
    vpp.pPusher->adjust(15);

    VppResetNotification(NULL, lpProcInfo->hDFilter3); // TBD: need de-interlace notifier ptr

    vpp.pPusher->push(0, subchannel | NV056_NOTIFY | 0x40000);
    vpp.pPusher->push(1, VPP_NOTIFY_TYPE);
    vpp.pPusher->push(2, subchannel | NV056_SET_BEGIN_END | 0x40000);
    vpp.pPusher->push(3, NV056_SET_BEGIN_END_OP_END);
    vpp.pPusher->adjust(4);

    vpp.pPusher->start(TRUE);
    return TRUE;
}


//---------------------------------------------------------------------------
// vppNv20Temporal
//      Temporal filter for NV20.  Surface dimensions do not have to be a
//      power of 2.  Texture does not have to be swizzled.
LOCAL BOOL vppNv20Temporal(Vpp_t *pVpp, LPPROCESSINFO lpProcInfo,
                     DWORD dwSrcOffset, DWORD dwSrcPitch,
                     DWORD dwDstOffset, DWORD dwDstPitch,
                     DWORD dwWidth, DWORD dwHeight, DWORD dwFlags,
                     LPDWORD pPrevFrameOffset)
{
    DWORD dwMergeSize;
    BOOL isField, isOddField;
    DWORD dwCombineFactor;
    BOOL isPrevFieldOdd;
    int dwPrevYOffset, dwCurrYOffset, dwDestYOffset;
    Vpp_t &vpp = *pVpp;
    NvU32 subchannel = SUB_CHANNEL(vpp.threeDClassSubCh);

    nvAssert(vpp.dwFlags & VPP_FLAG_KELVIN_3D);

    isField = IS_FIELD(dwFlags);
    isOddField = isField && (dwFlags & VPP_ODD);
    isPrevFieldOdd = (*pPrevFrameOffset & 0x1);

    dwCombineFactor = (vpp.regOverlayMode >> 8) & 0xFF;
    dwCombineFactor |= (dwCombineFactor << 8);
    dwCombineFactor |= (dwCombineFactor << 16);

    if (isField) {
        if (dwFlags & VPP_INTERLEAVED) {
            if (dwFlags & VPP_ODD) {
                dwSrcOffset += dwSrcPitch;
                dwDstOffset += dwDstPitch;
            }
            dwSrcPitch <<= 1;
            dwDstPitch <<= 1;
            dwHeight >>= 1;
        }
    }

    if (!(vpp.regOverlayMode & NV4_REG_OVL_MODE_NOTFIRST)) {
        // if first frame
        vpp.regOverlayMode |= NV4_REG_OVL_MODE_NOTFIRST;
        if (isOddField) {
            // remember field polarity of previous frame
            *pPrevFrameOffset = dwSrcOffset | 0x1;
        } else {
            *pPrevFrameOffset = dwSrcOffset;
        }
        return FALSE;
    }

    // convert RGB16 to RGB32 width
    dwWidth >>= 1;

    dwMergeSize = (dwWidth << 16) | dwHeight;

    nvAssert(vpp.pThreeDClassLastUser);
    if (*vpp.pThreeDClassLastUser != MODULE_ID_DDRAW_VPP)
    {
        if (!vppInitKelvin(&vpp)) {
            return FALSE;
        }
    }

    vpp.pPusher->push(0, subchannel | NV097_SET_SURFACE_CLIP_HORIZONTAL | 0x40000);
    vpp.pPusher->push(1, ((dwWidth << 16) | 0));
    vpp.pPusher->push(2, subchannel | NV097_SET_SURFACE_CLIP_VERTICAL | 0x40000);
    vpp.pPusher->push(3, ((dwHeight << 16) | 0));
    vpp.pPusher->adjust(4);

    vpp.pPusher->push(0, subchannel | NV097_SET_SURFACE_PITCH | 0x40000);
    vpp.pPusher->push(1, (dwDstPitch << 16) | dwDstPitch);
    vpp.pPusher->adjust(2);

    // Load combination factors
    vpp.pPusher->push(0, subchannel | NV097_SET_COMBINER_FACTOR0(0) | 0x40000);
    vpp.pPusher->push(1, dwCombineFactor);     // % of current field
    vpp.pPusher->push(2, subchannel | NV097_SET_COMBINER_FACTOR1(0) | 0x40000);
    vpp.pPusher->push(3, dwCombineFactor);
    vpp.pPusher->adjust(4);

    // Make sure offset is 128-byte aligned (in debug builds)
    nvAssert(!((dwSrcOffset % 128) || (*pPrevFrameOffset & ~0x1) % 128));

    vpp.pPusher->push(0, subchannel | NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
    vpp.pPusher->push(1, dwSrcOffset);
    vpp.pPusher->push(2, subchannel | NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
    vpp.pPusher->push(3, *pPrevFrameOffset & ~0x1);
    vpp.pPusher->push(4, subchannel | NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
    vpp.pPusher->push(5, dwDstOffset);
    vpp.pPusher->adjust(6);

    if (isOddField) {
        // remember field polarity of previous frame
        *pPrevFrameOffset = dwSrcOffset | 0x1;
    } else {
        *pPrevFrameOffset = dwSrcOffset;
    }

    // IMPORTANT: Even if we are not using texture units 2 and 3, we need to give
    // them some kind of value other than zero in the following commands.
    // Without these, the hardware will behave incorrectly, probably exhibiting
    // a problem that looks like an invalid surface pitch.
    vpp.pPusher->push(0, subchannel | NV097_SET_TEXTURE_IMAGE_RECT(0) | 0x40000);
    vpp.pPusher->push(1, dwMergeSize);
    vpp.pPusher->push(2, subchannel | NV097_SET_TEXTURE_IMAGE_RECT(1) | 0x40000);
    vpp.pPusher->push(3, dwMergeSize);
    vpp.pPusher->push(4, subchannel | NV097_SET_TEXTURE_IMAGE_RECT(2) | 0x40000);
    vpp.pPusher->push(5, 8 << 16 | 8);
    vpp.pPusher->push(6, subchannel | NV097_SET_TEXTURE_IMAGE_RECT(3) | 0x40000);
    vpp.pPusher->push(7, 8 << 16 | 8);
    vpp.pPusher->push(8, subchannel | NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
    vpp.pPusher->push(9, dwSrcPitch << 16);
    vpp.pPusher->push(10, subchannel | NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
    vpp.pPusher->push(11, dwSrcPitch << 16);
    vpp.pPusher->push(12, subchannel | NV097_SET_TEXTURE_CONTROL1(2) | 0x40000);
    vpp.pPusher->push(13, 8 << 16 | 8);
    vpp.pPusher->push(14, subchannel | NV097_SET_TEXTURE_CONTROL1(3) | 0x40000);
    vpp.pPusher->push(15, 8 << 16 | 8);
    vpp.pPusher->adjust(16);

    // Inspired by BMAC's SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE macro.
    vpp.pPusher->push(0, subchannel +
                         NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0) | 0x340000);
    vpp.pPusher->push(1, 0);
    vpp.pPusher->push(2, 0);
    vpp.pPusher->push(3, 0);
    vpp.pPusher->push(4, 0);
    vpp.pPusher->push(5, 0);
    vpp.pPusher->push(6, 0);
    vpp.pPusher->push(7, 0);
    vpp.pPusher->push(8, 0);
    vpp.pPusher->push(9, 0);
    vpp.pPusher->push(10, 4);
    vpp.pPusher->push(11, 8);
    vpp.pPusher->push(12, 0);
    vpp.pPusher->push(13, 0);
    vpp.pPusher->push(14, subchannel +
                          NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0) | 0x340000);
    vpp.pPusher->push(15, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(16, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(17, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(18, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(19, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(20, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(21, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(22, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(23, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(24, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(25, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(26, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->push(27, (12 << 8) |
                          (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) |
                           NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K);
    vpp.pPusher->adjust(28);

    // specify height and width in 1/4 pixel units
    dwHeight <<= 2;
    dwWidth <<= 2;

    // PG - Modified 11/07/2000
    // I temporarily disabled the offsets to counter
    // the misalignment of the fields in the next few lines
    if (isField) {
        if (isOddField != isPrevFieldOdd) {
            // current field as opposite polarity as previous field
            if (dwFlags & VPP_ODD) {
                dwPrevYOffset = +1;
                dwCurrYOffset = -1;
                dwDestYOffset = -1;
            } else {
                dwPrevYOffset = -1;
                dwCurrYOffset = +1;
                dwDestYOffset = +1;
            }
        } else {
            // current field as identical polarity as previous field
            if (dwFlags & VPP_ODD) {
                // odd-odd
                dwPrevYOffset = -1;
                dwCurrYOffset = -1;
                dwDestYOffset = -1;
            } else {
                // even-even
                dwPrevYOffset = +1;
                dwCurrYOffset = +1;
                dwDestYOffset = +1;
            }
        }

    } else {    // weave
        dwPrevYOffset = 0;
        dwCurrYOffset = 0;
        dwDestYOffset = 0;
    }

    vpp.pPusher->push(0, subchannel | NV097_SET_BEGIN_END | 0x40000);
    vpp.pPusher->push(1, NV097_SET_BEGIN_END_OP_QUADS);
    vpp.pPusher->push(2, subchannel | NV097_INLINE_ARRAY | 0x40300000);

    vpp.pPusher->push(3, (dwDestYOffset << 16) | 0);   // destination
    vpp.pPusher->push(4, (dwCurrYOffset << 16) | 0);   // current frame
    vpp.pPusher->push(5, (dwPrevYOffset << 16) | 0);   // previous frame
    
    vpp.pPusher->push(6, ((dwHeight+dwDestYOffset) << 16) | 0);
    vpp.pPusher->push(7, ((dwHeight+dwCurrYOffset) << 16) | 0);
    vpp.pPusher->push(8, ((dwHeight+dwPrevYOffset) << 16) | 0);
    
    vpp.pPusher->push(9, ((dwHeight+dwDestYOffset) << 16) | dwWidth);
    vpp.pPusher->push(10, ((dwHeight+dwCurrYOffset) << 16) | dwWidth);
    vpp.pPusher->push(11,  ((dwHeight+dwPrevYOffset) << 16) | dwWidth);
    
    vpp.pPusher->push(12, (+dwDestYOffset << 16) | dwWidth);
    vpp.pPusher->push(13, (+dwCurrYOffset << 16) | dwWidth);
    vpp.pPusher->push(14, (+dwPrevYOffset << 16) | dwWidth);
    vpp.pPusher->adjust(15);

    VppResetNotification(NULL, lpProcInfo->hTFilter3);  // TBD: need temporal filter notifier ptr

    vpp.pPusher->push(0, subchannel | NV097_NOTIFY | 0x40000);
    vpp.pPusher->push(1, VPP_NOTIFY_TYPE);
    vpp.pPusher->push(2, subchannel | NV097_SET_BEGIN_END | 0x40000);
    vpp.pPusher->push(3, NV097_SET_BEGIN_END_OP_END);
    vpp.pPusher->adjust(4);

    vpp.pPusher->start(TRUE);

    return TRUE;
}


//---------------------------------------------------------------------------
// vppNv10Temporal
//      Temporal filter for NV10.  Surface dimensions do not have to be a
//      power of 2.  Texture does not have to be swizzled.
LOCAL BOOL vppNv10Temporal(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo,
                     DWORD dwSrcOffset, DWORD dwSrcPitch,
                     DWORD dwDstOffset, DWORD dwDstPitch,
                     DWORD dwWidth, DWORD dwHeight, DWORD dwFlags,
                     LPDWORD pPrevFrameOffset)
{
    DWORD dwMergeSize;
    BOOL isField, isOddField;
    DWORD dwCombineFactor;
    unsigned long vertexSizeContext;
    BOOL isPrevFieldOdd;
    int dwPrevYOffset, dwCurrYOffset, dwDestYOffset;
    Vpp_t &vpp = *pVpp;
    NvU32 subchannel = SUB_CHANNEL(vpp.threeDClassSubCh);

    nvAssert (NVARCH >= 0x10);

    isField = IS_FIELD(dwFlags);
    isOddField = isField && (dwFlags & VPP_ODD);
    isPrevFieldOdd = (*pPrevFrameOffset & 0x1);

    dwCombineFactor = (vpp.regOverlayMode >> 8) & 0xFF;
    dwCombineFactor |= (dwCombineFactor << 8);
    dwCombineFactor |= (dwCombineFactor << 16);

    if (isField) {
        if (dwFlags & VPP_INTERLEAVED) {
            if (dwFlags & VPP_ODD) {
                dwSrcOffset += dwSrcPitch;
                dwDstOffset += dwDstPitch;
            }
            dwSrcPitch <<= 1;
            dwDstPitch <<= 1;
            dwHeight >>= 1;
        }
    }

    if (!(vpp.regOverlayMode & NV4_REG_OVL_MODE_NOTFIRST)) {
        // if first frame
        vpp.regOverlayMode |= NV4_REG_OVL_MODE_NOTFIRST;
        if (isOddField) {
            // remember field polarity of previous frame
            *pPrevFrameOffset = dwSrcOffset | 0x1;
        } else {
            *pPrevFrameOffset = dwSrcOffset;
        }
        return FALSE;
    }


    // convert RGB16 to RGB32 width
    dwWidth >>= 1;

    dwMergeSize = (dwWidth << 16) | dwHeight;

    nvAssert(vpp.pThreeDClassLastUser);
    if (*vpp.pThreeDClassLastUser != MODULE_ID_DDRAW_VPP)
    {
        if (!vppInitCelsius(&vpp)) {
            return FALSE;
        }
    }

    vpp.pPusher->push(0, subchannel | NV056_SET_SURFACE_CLIP_HORIZONTAL | 0x40000);
    vpp.pPusher->push(1, ((dwWidth << 16) | 0));

    vpp.pPusher->push(2, subchannel +
                   NV056_SET_SURFACE_CLIP_VERTICAL | 0x40000);
    vpp.pPusher->push(3, ((dwHeight << 16) | 0));
    vpp.pPusher->adjust(4);

    vpp.pPusher->push(0, subchannel | NV056_SET_SURFACE_PITCH | 0x40000);
    vpp.pPusher->push(1, (dwDstPitch << 16) | dwDstPitch);
    vpp.pPusher->adjust(2);

    // Load combination factors
    vpp.pPusher->push(0, subchannel | NV056_SET_COMBINE_FACTOR(0) | 0x80000);
    vpp.pPusher->push(1, dwCombineFactor);     // % of current field
    vpp.pPusher->push(2, dwCombineFactor);
    vpp.pPusher->adjust(3);

    vpp.pPusher->push(0, subchannel | NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
    vpp.pPusher->push(1, dwSrcOffset);
    vpp.pPusher->push(2, *pPrevFrameOffset & ~0x1);
    vpp.pPusher->push(3, subchannel | NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
    vpp.pPusher->push(4, dwDstOffset);
    vpp.pPusher->adjust(5);
    if (isOddField) {
        // remember field polarity of previous frame
        *pPrevFrameOffset = dwSrcOffset | 0x1;
    } else {
        *pPrevFrameOffset = dwSrcOffset;
    }

    vpp.pPusher->push(0, subchannel | NV056_SET_TEXTURE_IMAGE_RECT(0) | 0x80000);
    vpp.pPusher->push(1, dwMergeSize);
    vpp.pPusher->push(2, dwMergeSize);

    vpp.pPusher->push(3, subchannel | NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
    vpp.pPusher->push(4, dwSrcPitch << 16);
    vpp.pPusher->push(5, dwSrcPitch << 16);
    vpp.pPusher->adjust(6);

    vertexSizeContext = NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_2 | \
                        (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4) | \
                        (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 8) | \
                        (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_2 << 12) | \
                        (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_2 << 16) | \
                        (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 20) | \
                        (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 24) | \
                        (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 28); \
    vpp.pPusher->push(0, subchannel | NV056_SET_INVERSE_MODEL_VIEW_MATRIX1(15) | 0x40000); \
    vpp.pPusher->push(1, vertexSizeContext); \
    vpp.pPusher->push(2, subchannel | NV056_SET_VERTEX_ARRAY_OFFSET | 0x400000); \
    vpp.pPusher->push(3, 0); \
    vpp.pPusher->push(4, (NV056_SET_VERTEX_ARRAY_FORMAT_W_NONE << 24) | \
                  (12 << 8) | \
                  (NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_2 << 4) | \
                   NV056_SET_VERTEX_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->push(5, 0); \
    vpp.pPusher->push(6, (12 << 8) | \
                  (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4) | \
                   NV056_SET_DIFFUSE_ARRAY_FORMAT_TYPE_UNSIGNED_BYTE_BGRA); \
    vpp.pPusher->push(7, 0); \
    vpp.pPusher->push(8, (12 << 8) | \
                  (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 4) | \
                   NV056_SET_SPECULAR_ARRAY_FORMAT_TYPE_UNSIGNED_BYTE_BGRA); \
    vpp.pPusher->push(9, 4); \
    vpp.pPusher->push(10, (12 << 8) | \
                  (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_2 << 4) | \
                   NV056_SET_TEX_COORD0_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->push(11, 8); \
    vpp.pPusher->push(12, (12 << 8) | \
                   (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_2 << 4) | \
                    NV056_SET_TEX_COORD1_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->push(13, 0); \
    vpp.pPusher->push(14, (12 << 8) | \
                   (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 4) | \
                    NV056_SET_NORMAL_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->push(15, 0); \
    vpp.pPusher->push(16, (12 << 8) | \
                   (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 4) | \
                    NV056_SET_WEIGHT_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->push(17, 0); \
    vpp.pPusher->push(18, (12 << 8) | \
                   (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 4) | \
                    NV056_SET_FOG_ARRAY_FORMAT_TYPE_SHORT); \
    vpp.pPusher->adjust(19); \

    // specify height and width in 1/4 pixel units
    dwHeight <<= 2;
    dwWidth <<= 2;

    if (isField) {
        if (isOddField != isPrevFieldOdd) {
            // current field as opposite polarity as previous field
            if (dwFlags & VPP_ODD) {
                dwPrevYOffset = +1;
                dwCurrYOffset = -1;
                dwDestYOffset = -1;
            } else {
                dwPrevYOffset = -1;
                dwCurrYOffset = +1;
                dwDestYOffset = +1;
            }
        } else {
            // current field as identical polarity as previous field
            if (dwFlags & VPP_ODD) {
                // odd-odd
                dwPrevYOffset = -1;
                dwCurrYOffset = -1;
                dwDestYOffset = -1;
            } else {
                // even-even
                dwPrevYOffset = +1;
                dwCurrYOffset = +1;
                dwDestYOffset = +1;
            }
        }

    } else {    // weave
        dwPrevYOffset = 0;
        dwCurrYOffset = 0;
        dwDestYOffset = 0;
    }

    vpp.pPusher->push(0, subchannel | NV056_SET_BEGIN_END | 0x40000);
    vpp.pPusher->push(1, NV056_SET_BEGIN_END_OP_QUADS);
    vpp.pPusher->push(2, subchannel | NV056_INLINE_ARRAY(0) | 0x300000);

    vpp.pPusher->push(3, (dwPrevYOffset << 16) | 0);   // previous frame
    vpp.pPusher->push(4, (dwCurrYOffset << 16) | 0);   // current frame
    vpp.pPusher->push(5, (dwDestYOffset << 16) | 0);   // destination

    vpp.pPusher->push(6, ((dwHeight+dwPrevYOffset) << 16) | 0);
    vpp.pPusher->push(7, ((dwHeight+dwCurrYOffset) << 16) | 0);
    vpp.pPusher->push(8, ((dwHeight+dwDestYOffset) << 16) | 0);

    vpp.pPusher->push(9,  ((dwHeight+dwPrevYOffset) << 16) | dwWidth);
    vpp.pPusher->push(10, ((dwHeight+dwCurrYOffset) << 16) | dwWidth);
    vpp.pPusher->push(11, ((dwHeight+dwDestYOffset) << 16) | dwWidth);

    vpp.pPusher->push(12, (+dwPrevYOffset << 16) | dwWidth);
    vpp.pPusher->push(13, (+dwCurrYOffset << 16) | dwWidth);
    vpp.pPusher->push(14, (+dwDestYOffset << 16) | dwWidth);
    vpp.pPusher->adjust(15);

    VppResetNotification(NULL, lpProcInfo->hTFilter3);  // TBD: need temporal filter notifier ptr

    vpp.pPusher->push(0, subchannel | NV056_NOTIFY | 0x40000);
    vpp.pPusher->push(1, VPP_NOTIFY_TYPE);
    vpp.pPusher->push(2, subchannel | NV056_SET_BEGIN_END | 0x40000);
    vpp.pPusher->push(3, NV056_SET_BEGIN_END_OP_END);
    vpp.pPusher->adjust(4);

    vpp.pPusher->start(TRUE);
    return TRUE;
}

//---------------------------------------------------------------------------
// vppNv5Temporal
//      Temporal filter for NV5.  Surface dimensions must be a power of 2.
//      Previous frame must be swizzled.
LOCAL BOOL vppNv5Temporal(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo,
                    DWORD dwSrcOffset, DWORD dwSrcPitch,
                    DWORD dwDstOffset, DWORD dwDstPitch,
                    DWORD dwWidth, DWORD dwHeight,
                    LPDWORD pPrevFrameOffset)
{
    DWORD dwLogWidth, dwLogHeight, dwPixWidth, dwLineHeight;
    DWORD dwOffsetA, dwOffsetB;
    NvF32 fWidth, fHeight, fU, fV;
    DWORD dwMergeSize;
    DWORD dwCombineFactor;
    Vpp_t &vpp = *pVpp;
    const NvU32 subchannel = SUB_CHANNEL(vpp.spareSubCh);

    // we shouldn't have a celsius or kelvin class if we're here
    nvAssert(!(vpp.dwFlags & (VPP_FLAG_KELVIN_3D | VPP_FLAG_CELSIUS_3D)));

    dwPixWidth = dwSrcPitch >> 2;
    if (dwPixWidth > 4096)      dwLogWidth = 13;
    else if (dwPixWidth > 2048) dwLogWidth = 12;
    else if (dwPixWidth > 1024) dwLogWidth = 11;
    else if (dwPixWidth > 512)  dwLogWidth = 10;
    else if (dwPixWidth > 256)  dwLogWidth = 9;
    else if (dwPixWidth > 128)  dwLogWidth = 8;
    else if (dwPixWidth > 64)   dwLogWidth = 7;
    else if (dwPixWidth > 32)   dwLogWidth = 6;
    else                        dwLogWidth = 5;

    if (vpp.regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE) {
        dwLineHeight = max(dwHeight, vpp.dwOverlayDstHeight);
    } else {
        dwLineHeight = dwHeight;
    }
    if (dwLineHeight > 4096)      dwLogHeight = 13;
    else if (dwLineHeight > 2048) dwLogHeight = 12;
    else if (dwLineHeight > 1024) dwLogHeight = 11;
    else if (dwLineHeight > 512)  dwLogHeight = 10;
    else if (dwLineHeight > 256)  dwLogHeight = 9;
    else if (dwLineHeight > 128)  dwLogHeight = 8;
    else if (dwLineHeight > 64)   dwLogHeight = 7;
    else if (dwLineHeight > 32)   dwLogHeight = 6;
    else if (dwLineHeight > 16)   dwLogHeight = 5;
    else if (dwLineHeight > 8)    dwLogHeight = 4;
    else if (dwLineHeight > 4)    dwLogHeight = 3;
    else if (dwLineHeight > 2)    dwLogHeight = 2;
    else                          dwLogHeight = 1;

//    _asm {emms};
    fWidth  = (NvF32) (dwWidth >> 1);
    fHeight = (NvF32) dwLineHeight;
    fU = fWidth  / (1 << dwLogWidth);
    fV = fHeight / (1 << dwLogHeight);

    dwCombineFactor = (vpp.regOverlayMode >> 8) & 0xFF;
    dwCombineFactor |= (dwCombineFactor << 8);
    dwCombineFactor |= (dwCombineFactor << 16);

    dwMergeSize = (dwLineHeight << 16) | (dwWidth >> 1);

    // swizzle (used in next pass)
    vpp.pPusher->push(0, subchannel | 0x40000);
    vpp.pPusher->push(1, vpp.hContextSurfaceSwizzled);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV052_SET_FORMAT | 0x80000);
    vpp.pPusher->push(3, NV052_SET_FORMAT_COLOR_LE_A8R8G8B8 |  // SetFormat
                  (dwLogWidth << 16) |
                  (dwLogHeight << 24));
    vpp.pPusher->push(4, dwDstOffset);                         // SetOffset
    vpp.pPusher->adjust(5);

    vpp.pPusher->push(0, subchannel | 0x40000);
    vpp.pPusher->push(1, vpp.m_obj_a[SWIZZLE_NDX].classIID);
    vpp.pPusher->push(2, subchannel | NV077_CLIP_POINT | 0x180000);
    vpp.pPusher->push(3, 0);                               // ClipPoint
    vpp.pPusher->push(4, dwMergeSize);                     // ClipSize
    vpp.pPusher->push(5, 0);                               // ImageOutPoint
    vpp.pPusher->push(6, dwMergeSize);                     // ImageOutSize
    vpp.pPusher->push(7, 1 << 20);                         // DeltaDuDx
    vpp.pPusher->push(8, 1 << 20);                         // DeltaDvDy
    vpp.pPusher->push(9, subchannel | NV077_IMAGE_IN_SIZE | 0x100000);
    vpp.pPusher->push(10, (dwMergeSize + 1) & ~1);         // ImageInSize
    vpp.pPusher->push(11, (dwSrcPitch) |                   // ImageInFormat
                   (NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16) |
                   (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24));
    vpp.pPusher->push(12, dwSrcOffset);                    // ImageInOffset
    vpp.pPusher->push(13, 0);                              // ImageInPoint
    vpp.pPusher->adjust(14);
    vpp.pPusher->start(TRUE);

    // offset A is current frame multiplied by factor
    // offset B is previous frame swizzled
    dwOffsetA = dwSrcOffset;
    dwOffsetB = *pPrevFrameOffset;
    *pPrevFrameOffset = dwDstOffset;

    // only do this part if this is not the first frame
    if (vpp.regOverlayMode & NV4_REG_OVL_MODE_NOTFIRST) {
        DWORD i;
        Tlmtvertex vertex[4] = {
            { 0.0f,   0.0f,    0.0f, 1.0f, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f, 0.0f, 0.0f, 0.0f},
            { 0.0f,   fHeight, 0.0f, 1.0f, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f, fV,   0.0f, fV},
            { fWidth, 0.0f,    0.0f, 1.0f, 0xFFFFFFFF, 0xFFFFFFFF, fU,   0.0f, fU,   0.0f},
            { fWidth, fHeight, 0.0f, 1.0f, 0xFFFFFFFF, 0xFFFFFFFF, fU,   fV,   fU,   fV}
        };

        // multiply current frame with factor, in place
        vpp.pPusher->push(0, subchannel | 0x40000);
        vpp.pPusher->push(1, vpp.hContextBeta4);
        vpp.pPusher->push(2, subchannel | BETA4_SET_BETA_FACTOR_OFFSET | 0x40000);
        vpp.pPusher->push(3, dwCombineFactor);

        vpp.pPusher->push(4, subchannel | 0x40000);
        vpp.pPusher->push(5, vpp.hSurfaces2D);
        vpp.pPusher->push(6, subchannel | SURFACES_2D_PITCH_OFFSET | 0xC0000);
        vpp.pPusher->push(7, (dwSrcPitch << 16) | dwSrcPitch);     // set pitch
        vpp.pPusher->push(8, dwSrcOffset);                         // set offset source
        vpp.pPusher->push(9, dwSrcOffset);                         // set offset destin
        vpp.pDriverData->bltData.dwLastCombinedPitch = (dwSrcPitch << 16) | dwSrcPitch;
        vpp.pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
        vpp.pDriverData->bltData.dwLastDstOffset = dwSrcOffset;

        vpp.pPusher->push(10, subchannel | 0x40000);
        vpp.pPusher->push(11, vpp.m_obj_a[ALPHA_BLT_NDX].classIID);
        vpp.pPusher->push(12, subchannel | BLIT_POINT_IN_OFFSET | 0xC0000);
        vpp.pPusher->push(13, 0);                                  // control point in
        vpp.pPusher->push(14, 0);                                  // control point out
        vpp.pPusher->push(15, dwMergeSize);                        // size
        vpp.pPusher->adjust(16);
        vpp.pPusher->start(TRUE);

        vpp.pPusher->push(0, subchannel | 0x40000);
        vpp.pPusher->push(1, vpp.hContextSurfacesARGB_ZS);
        vpp.pPusher->push(2, subchannel | NV053_SET_FORMAT | 0x80000);
        vpp.pPusher->push(3, NV053_SET_FORMAT_COLOR_LE_A8R8G8B8 |          // SetFormat
                     (NV053_SET_FORMAT_TYPE_PITCH << 8) |
                     (dwLogWidth << 16) |
                     (dwLogHeight << 24));
        vpp.pPusher->push(4, dwMergeSize);                                 // SetClipSize
        vpp.pPusher->push(5, subchannel | NV053_SET_PITCH | 0x80000);
        vpp.pPusher->push(6, ((dwSrcPitch) << 16) | (dwSrcPitch));         // SetPitch
        vpp.pPusher->push(7, dwSrcOffset);                                 // SetOffsetColor
        vpp.pPusher->adjust(8);

        vpp.pPusher->push(0, subchannel | 0x40000);
        vpp.pPusher->push(1, vpp.hDX6MultiTextureTriangle);
        vpp.pPusher->push(2, subchannel | NV055_SET_CONTEXT_DMA_NOTIFIES | 0x100000);
        vpp.pPusher->push(3, vpp.hMiscEventNotifier);     // SetContextDmaNotifies
        vpp.pPusher->push(4, vpp.hInVideoMemContextDma);       // SetContextDmaA
        vpp.pPusher->push(5, vpp.hInVideoMemContextDma);       // SetContextDmaB
        vpp.pPusher->push(6, vpp.hContextSurfacesARGB_ZS);                // SetContextSurfaces
        vpp.pPusher->adjust(7);

        // add current frame with previous frame (multiplied by [1-factor]) in place
        // not using texture unit 0, but need to set everything anyways otherwise RM generates error and doesn't do it
        vpp.pPusher->push(0, subchannel | 0x40000);
        vpp.pPusher->push(1, vpp.hDX6MultiTextureTriangle);
        vpp.pPusher->push(2, subchannel | NV055_OFFSET(0) | 0x200000);
        vpp.pPusher->push(3, dwOffsetA);                                   // Offset(0)
        vpp.pPusher->push(4, dwOffsetB);                                   // Offset(1)
        vpp.pPusher->push(5, NV055_FORMAT_CONTEXT_DMA_A |                  // Format(0)
                      (NV055_FORMAT_ORIGIN_ZOH_CENTER << 4) |
                      (NV055_FORMAT_ORIGIN_FOH_CENTER << 6) |
                      (NV055_FORMAT_COLOR_LE_A8R8G8B8 << 8) |
                      (1 << 12) |
                      (dwLogWidth << 16) |
                      (dwLogHeight << 20) |
                      (NV055_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
                      (NV055_FORMAT_WRAPU_FALSE << 27) |
                      (NV055_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
                      (NV055_FORMAT_WRAPV_FALSE << 31));
        vpp.pPusher->push(6, NV055_FORMAT_CONTEXT_DMA_B |                  // Format(1)
                      (NV055_FORMAT_ORIGIN_ZOH_CENTER << 4) |
                      (NV055_FORMAT_ORIGIN_FOH_CENTER << 6) |
                      (NV055_FORMAT_COLOR_LE_A8R8G8B8 << 8) |
                      (1 << 12) |
                      (dwLogWidth << 16) |
                      (dwLogHeight << 20) |
                      (NV055_FORMAT_TEXTUREADDRESSU_CLAMP << 24) |
                      (NV055_FORMAT_WRAPU_FALSE << 27) |
                      (NV055_FORMAT_TEXTUREADDRESSV_CLAMP << 28) |
                      (NV055_FORMAT_WRAPV_FALSE << 31));
        vpp.pPusher->push(7, (1 << 4) |                                    // Filter(0)
                      (NV055_FILTER_MIPMAP_DITHER_ENABLE_FALSE << 15) |
                      (NV055_FILTER_TEXTUREMIN_NEAREST << 24) |
                      (NV055_FILTER_ANISOTROPIC_MINIFY_ENABLE_FALSE << 27) |
                      (NV055_FILTER_TEXTUREMAG_NEAREST << 28) |
                      (NV055_FILTER_ANISOTROPIC_MAGNIFY_ENABLE_FALSE << 31));
        vpp.pPusher->push(8, (1 << 4) |                                    // Filter(1)
                      (NV055_FILTER_MIPMAP_DITHER_ENABLE_FALSE << 15) |
                      (NV055_FILTER_TEXTUREMIN_NEAREST << 24) |
                      (NV055_FILTER_ANISOTROPIC_MINIFY_ENABLE_FALSE << 27) |
                      (NV055_FILTER_TEXTUREMAG_NEAREST << 28) |
                      (NV055_FILTER_ANISOTROPIC_MAGNIFY_ENABLE_FALSE << 31));
        vpp.pPusher->push(9, NV055_COMBINE_0_ALPHA_INVERSE_0_NORMAL |      // Combine0Alpha
                      (NV055_COMBINE_0_ALPHA_ARGUMENT_0_ZERO << 2) |
                      (NV055_COMBINE_0_ALPHA_INVERSE_1_NORMAL << 8) |
                      (NV055_COMBINE_0_ALPHA_ARGUMENT_1_ZERO << 10) |
                      (NV055_COMBINE_0_ALPHA_INVERSE_2_NORMAL << 16) |
                      (NV055_COMBINE_0_ALPHA_ARGUMENT_2_TEXTURE1 << 18) |
                      (NV055_COMBINE_0_ALPHA_INVERSE_3_INVERSE << 24) |
                      (NV055_COMBINE_0_ALPHA_ARGUMENT_3_ZERO << 26) |
                      (NV055_COMBINE_0_ALPHA_OPERATION_ADD << 29));
        vpp.pPusher->push(10, NV055_COMBINE_0_COLOR_INVERSE_0_NORMAL |     // Combine0Color
                       (NV055_COMBINE_0_COLOR_ALPHA_0_COLOR << 1) |
                       (NV055_COMBINE_0_COLOR_ARGUMENT_0_ZERO << 2) |
                       (NV055_COMBINE_0_COLOR_INVERSE_1_NORMAL << 8) |
                       (NV055_COMBINE_0_COLOR_ALPHA_1_COLOR << 9) |
                       (NV055_COMBINE_0_COLOR_ARGUMENT_1_ZERO << 10) |
                       (NV055_COMBINE_0_COLOR_INVERSE_2_NORMAL << 16) |
                       (NV055_COMBINE_0_COLOR_ALPHA_2_COLOR << 17) |
                       (NV055_COMBINE_0_COLOR_ARGUMENT_2_TEXTURE1 << 18) |
                       (NV055_COMBINE_0_COLOR_INVERSE_3_INVERSE << 24) |
                       (NV055_COMBINE_0_COLOR_ALPHA_3_COLOR << 25) |
                       (NV055_COMBINE_0_COLOR_ARGUMENT_3_ZERO << 26) |
                       (NV055_COMBINE_0_COLOR_OPERATION_ADD << 29));

        vpp.pPusher->push(11, subchannel | NV055_COMBINE_1_ALPHA | 0x200000);
        vpp.pPusher->push(12, NV055_COMBINE_1_ALPHA_INVERSE_0_NORMAL |     // Combine1Alpha
                       (NV055_COMBINE_1_ALPHA_ARGUMENT_0_TEXTURE1 << 2) |
                       (NV055_COMBINE_1_ALPHA_INVERSE_1_INVERSE << 8) |
                       (NV055_COMBINE_1_ALPHA_ARGUMENT_1_FACTOR << 10) |
                       (NV055_COMBINE_1_ALPHA_INVERSE_2_NORMAL << 16) |
                       (NV055_COMBINE_1_ALPHA_ARGUMENT_2_ZERO << 18) |
                       (NV055_COMBINE_1_ALPHA_INVERSE_3_NORMAL << 24) |
                       (NV055_COMBINE_1_ALPHA_ARGUMENT_3_ZERO << 26) |
                       (NV055_COMBINE_1_ALPHA_OPERATION_ADD << 29));
        vpp.pPusher->push(13, NV055_COMBINE_1_COLOR_INVERSE_0_NORMAL |     // Combine1Color
                       (NV055_COMBINE_1_COLOR_ALPHA_0_COLOR << 1) |
                       (NV055_COMBINE_1_COLOR_ARGUMENT_0_TEXTURE1 << 2) |
                       (NV055_COMBINE_1_COLOR_INVERSE_1_INVERSE << 8) |
                       (NV055_COMBINE_1_COLOR_ALPHA_1_COLOR << 9) |
                       (NV055_COMBINE_1_COLOR_ARGUMENT_1_FACTOR << 10) |
                       (NV055_COMBINE_1_COLOR_INVERSE_2_NORMAL << 16) |
                       (NV055_COMBINE_1_COLOR_ALPHA_2_COLOR << 17) |
                       (NV055_COMBINE_1_COLOR_ARGUMENT_2_ZERO << 18) |
                       (NV055_COMBINE_1_COLOR_INVERSE_3_NORMAL << 24) |
                       (NV055_COMBINE_1_COLOR_ALPHA_3_COLOR << 25) |
                       (NV055_COMBINE_1_COLOR_ARGUMENT_3_ZERO << 26) |
                       (NV055_COMBINE_1_COLOR_OPERATION_ADD << 29));
        vpp.pPusher->push(14, dwCombineFactor);                            // CombineFactor
        vpp.pPusher->push(15, NV055_BLEND_MASK_BIT_LSB |                   // Blend
                       (NV055_BLEND_SHADEMODE_FLAT << 6) |
                       (NV055_BLEND_TEXTUREPERSPECTIVE_FALSE << 8) |
                       (NV055_BLEND_SPECULARENABLE_FALSE << 12) |
                       (NV055_BLEND_FOGENABLE_FALSE << 16) |
                       (NV055_BLEND_ALPHABLENDENABLE_TRUE << 20) |
                       (NV055_BLEND_SRCBLEND_ONE << 24) |
                       (NV055_BLEND_DESTBLEND_ONE << 28));
        vpp.pPusher->push(16, (NV055_CONTROL0_ALPHAFUNC_ALWAYS << 8) |     // Control0
                       (NV055_CONTROL0_ALPHATESTENABLE_FALSE << 12) |
                       (NV055_CONTROL0_ORIGIN_CENTER << 13) |
                       (NV055_CONTROL0_ZENABLE_FALSE << 14) |
                       (NV055_CONTROL0_ZFUNC_ALWAYS << 16) |
                       (NV055_CONTROL0_CULLMODE_NONE << 20) |
                       (NV055_CONTROL0_DITHERENABLE_FALSE << 22) |
                       (NV055_CONTROL0_Z_PERSPECTIVE_ENABLE_FALSE << 23) |
                       (NV055_CONTROL0_ZWRITEENABLE_FALSE << 24) |
                       (NV055_CONTROL0_STENCIL_WRITE_ENABLE_FALSE << 25) |
                       (NV055_CONTROL0_ALPHA_WRITE_ENABLE_TRUE << 26) |
                       (NV055_CONTROL0_RED_WRITE_ENABLE_TRUE << 27) |
                       (NV055_CONTROL0_GREEN_WRITE_ENABLE_TRUE << 28) |
                       (NV055_CONTROL0_BLUE_WRITE_ENABLE_TRUE << 29) |
                       (NV055_CONTROL0_Z_FORMAT_FLOAT << 30));
        vpp.pPusher->push(17, NV055_CONTROL1_STENCIL_TEST_ENABLE_FALSE |   // Control1
                       (NV055_CONTROL1_STENCIL_FUNC_NEVER << 4));
        vpp.pPusher->push(18, NV055_CONTROL2_STENCIL_OP_FAIL_KEEP |        // Control2
                       (NV055_CONTROL2_STENCIL_OP_ZFAIL_KEEP << 4) |
                       (NV055_CONTROL2_STENCIL_OP_ZPASS_KEEP << 8));
        vpp.pPusher->push(19, 0);                                          // FogColor

        vpp.pPusher->push(20, subchannel | NV055_TLMTVERTEX(0) | 0xa00000);
        for (i=0; i<40; i++) {
            vpp.pPusher->push(21+i, ((DWORD *)(&vertex[0]))[i]);
        }
        vpp.pPusher->adjust(61);
        vpp.pPusher->start(TRUE);

        VppResetNotification(NULL, lpProcInfo->hTFilter3);  // TBD: need temporal filter notifier ptr

        vpp.pPusher->push(0, subchannel | NV055_NOTIFY | 0x40000);
        vpp.pPusher->push(1, VPP_NOTIFY_TYPE);
        vpp.pPusher->push(2, subchannel | NV055_DRAW_PRIMITIVE(0) | 0x40000);
        vpp.pPusher->push(3, 0 | (1 << 4) | (2 << 8) | (1 << 12) | (2 << 16) | (3 << 20));
        vpp.pPusher->adjust(4);
        vpp.pPusher->start(TRUE);

        vpp.pDriverData->dDrawSpareSubchannelObject = 0;
        vpp.pDriverData->TwoDRenderingOccurred = 1;
    } else {
        // this was the first frame, next time it won't be
        vpp.regOverlayMode |= NV4_REG_OVL_MODE_NOTFIRST;
        return FALSE;
    }

    return TRUE;
}

//---------------------------------------------------------------------------
// vppFlip
//      Flip to surface.  Use the PIO channel in case we do a raw flip with
//      a bunch of 3D going on.
//
LOCAL BOOL vppFlip(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo,
             DWORD dwOffset, DWORD dwPitch, DWORD dwWidth, DWORD dwHeight,
             DWORD dwIndex, DWORD dwFourCC, DWORD dwFlags,
             DWORD dwPrescaleFactorX, DWORD dwPrescaleFactorY)
{
    DWORD dwPointIn, dwSizeIn, dwOverlayFormat;
    DWORD dwDeltaX, dwDeltaY;
    DWORD dwDstX, dwDstY, dwDstWidth, dwDstHeight;
    BOOL doYBiasing;
    Vpp_t &vpp = *pVpp;

    // Remember this in case we get a SetOverlayPosition call
    vpp.dwOverlaySrcOffset = dwOffset;
    vpp.dwOverlaySrcPitch = dwPitch;

    dwDeltaX    = vpp.dwOverlayDeltaX;
    dwDeltaY    = vpp.dwOverlayDeltaY;
    dwDstX      = vpp.dwOverlayDstX;
    dwDstY      = vpp.dwOverlayDstY;
    dwDstWidth  = vpp.dwOverlayDstWidth;
    dwDstHeight = vpp.dwOverlayDstHeight;

    if (vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_OVLZOOMQUADMASK) {
        // zoom
        DWORD dwSrcX         = (vpp.dwOverlayFSSrcMinX * dwPrescaleFactorX) >> 20;
        DWORD dwSrcY         = (vpp.dwOverlayFSSrcMinY * dwPrescaleFactorY) >> 20;
        DWORD dwSrcWidth     = (vpp.dwOverlayFSSrcWidth * dwPrescaleFactorX) >> 20;
        DWORD dwSrcHeight    = (vpp.dwOverlayFSSrcHeight * dwPrescaleFactorY) >> 20;
        DWORD dwZoomFactor   = 384 - ((vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_OVLZOOMFACTORMASK) >> 12);
        DWORD dwZoomQuad     = (vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_OVLZOOMQUADMASK) >> 8;
        DWORD dwNewSrcWidth  = dwSrcWidth  * dwZoomFactor / 384;
        DWORD dwNewSrcHeight = dwSrcHeight * dwZoomFactor / 384;

        // get start point
        switch (dwZoomQuad) {
        case 1:  break;
        case 2:  dwSrcX += dwWidth - dwNewSrcWidth;          break;
        case 3:  dwSrcY += dwHeight - dwNewSrcHeight;        break;
        case 4:  dwSrcX += dwWidth - dwNewSrcWidth;
                 dwSrcY += dwHeight - dwNewSrcHeight;        break;
        case 5:  dwSrcX += (dwWidth - dwNewSrcWidth) >> 1;
                 dwSrcY += (dwHeight - dwNewSrcHeight) >> 1; break;
        default: break;
        }
        dwWidth = dwNewSrcWidth;
        dwHeight = dwNewSrcHeight;

        // if DX clips the source, we need to adjust our new source rectangle to match
        if (vpp.dwOverlaySrcWidth < dwSrcWidth) {
            DWORD xScale = (vpp.dwOverlaySrcWidth << 20) / dwSrcWidth;
            DWORD xRelative = vpp.dwOverlaySrcX - ((vpp.dwOverlayFSSrcMinX * dwPrescaleFactorX) >> 20);
            DWORD xPos   = (xRelative << 20) / dwSrcWidth;
            dwSrcX += (dwWidth * xPos) >> 20;
            dwWidth = (dwWidth * xScale) >> 20;
        }
        if (vpp.dwOverlaySrcHeight < dwSrcHeight) {
            DWORD yScale = (vpp.dwOverlaySrcHeight << 20) / dwSrcHeight;
            DWORD yRelative = vpp.dwOverlaySrcY - ((vpp.dwOverlayFSSrcMinY * dwPrescaleFactorY) >> 20);
            DWORD yPos   = (yRelative << 20) / dwSrcHeight;
            dwSrcY += (dwHeight * yPos) >> 20;
            dwHeight = (dwHeight * yScale) >> 20;
        }

        // calculate new source offset
        dwOffset += (dwSrcY * dwPitch) + (dwSrcX * 2);

        // recompute new scale factors
        dwDeltaX = ((dwWidth - 1) << 20) / (vpp.dwOverlayDstWidth - 1);
        dwDeltaY = ((dwHeight - 1) << 20) / (vpp.dwOverlayDstHeight - 1);
    }

    dwPointIn = (dwOffset & NV_OVERLAY_BYTE_ALIGNMENT_PAD) << 3;
    dwSizeIn = asmMergeCoords((dwWidth & ~1), dwHeight);
    dwSizeIn += dwPointIn >> 4;
    dwOffset &= ~NV_OVERLAY_BYTE_ALIGNMENT_PAD;

    // if downscaling in Y, throw in a Y bias of -1/2 pixel
    // don't do this on NV5 class chips because it doesn't properly bias it
    doYBiasing = (dwDeltaY > 0x100000 && vpp.pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10);
    if (doYBiasing) {
        dwPointIn -= 0x00080000;
    }

    if (IS_FIELD(dwFlags)) {
        // Set flag to say overlay has been in Bob mode.
        vpp.pDriverData->dwDXVAFlags |= DXVA_OVERLAY_WAS_BOBBED;

        if (dwFlags & VPP_INTERLEAVED) {
            if (dwFlags & VPP_ODD) {
                dwOffset += dwPitch;
            }
            dwPitch <<= 1;
            dwHeight >>= 1;
            dwDeltaY = ((dwHeight) << 20) / (vpp.dwOverlayDstHeight);
            dwSizeIn = (dwSizeIn & 0x0000FFFF) | (dwHeight << 16);
        }
        if (dwFlags & VPP_BOB) {
            if (dwFlags & VPP_ODD) {
                if (doYBiasing) {
                    dwPointIn += 0x00040000;
                } else {
                    dwPointIn += 0xFFFC0000;
                }
            } else {
                if (doYBiasing) {
                    dwPointIn += 0x000C0000;
                } else {
                    dwPointIn += 0x00040000;
                }
            }
        }
    }

    // replace old surface pitch value with current pitch value
    dwOverlayFormat = vpp.dwOverlayFormat;
    dwOverlayFormat &= 0xFFFF0000;
    dwOverlayFormat |= dwPitch;
    dwOverlayFormat |= (VPP_NOTIFY_TYPE << 31);

    // reset notifiers
    //   Must clear notifier status even if we're using events.
    if (dwIndex & 0x1) {
        VppResetNotification(&vpp.m_obj_a[OVERLAY_NDX].notifier_a[2], lpProcInfo->hOvlFlipOddEvent3);
        vpp.m_obj_a[OVERLAY_NDX].notifier_a[2].status = NV_IN_PROGRESS;
    } else {
        VppResetNotification(&vpp.m_obj_a[OVERLAY_NDX].notifier_a[1], lpProcInfo->hOvlFlipEvenEvent3);
        vpp.m_obj_a[OVERLAY_NDX].notifier_a[1].status = NV_IN_PROGRESS;
    }

    // frame delivery control
    if (vpp.llDeliveryPeriod != 0)
    {
        VppResetNotification(vpp.m_obj_a[TIMER_NDX].notifier_a, lpProcInfo->hTimer3);
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.m_obj_a[TIMER_NDX].classIID);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV004_SET_ALARM_TIME | 0x80000);
        vpp.pPusher->push(3, (DWORD)(vpp.llDeliveryTime & 0x00000000FFFFFFFF));
        vpp.pPusher->push(4, (DWORD)(vpp.llDeliveryTime >> 32));
        vpp.pPusher->push(5, SUB_CHANNEL(vpp.spareSubCh) | NV004_SET_ALARM_NOTIFY | 0x40000);
        vpp.pPusher->push(6, VPP_NOTIFY_TYPE);
        vpp.pPusher->adjust(7);
        vpp.pPusher->start(TRUE);
        vpp.pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[TIMER_NDX].classIID;
        vpp.llDeliveryTime += vpp.llDeliveryPeriod;
        if (VppWaitForNotification(NULL, lpProcInfo->hTimer3, VPP_TIMEOUT_TIME * 4)) {
            DPF(" *** TIMER event failure ***");
            EVENT_ERROR;
        }
    }

    // HWFIX: NV20 overlay bug, if scale factor >= 0.5 && XPointIn >= 9.4375 then this must be FALSE:
    //        ((XSizeIn - 2) & 0x8) == 0x8
    // This code must not go through on NV < 20
    if ((vpp.pDriverData->NvDeviceVersion > NV_DEVICE_VERSION_10) &&
     ((((dwSizeIn & 0x0000FFFF) - 2) >> 3) & 0x1) &&
       ((dwPointIn & 0x0000FFFF) >= 0x97)) {
        // muck with source width so HWBUG does not happen
        dwSizeIn = (dwSizeIn & 0xFFFF0000) | ((dwSizeIn & 0x0000FFFF) + 8);
        // now muck with dwDeltaX so we don't see 1/2 pixel of crap on the right edge
        dwDeltaX -= (1 << 20) / ((dwSizeIn & 0xFFFF) * 3 / 2);  // should accumulate to 2/3 pixel over the span of the source
    }

    // do the overlay flip
    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(1, vpp.m_obj_a[OVERLAY_NDX].classIID);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV07A_SET_OVERLAY_COLORKEY(dwIndex) | 0x240000);
    vpp.pPusher->push(3, vpp.dwOverlayColorKey);                    // SetOverlayColorKey
    vpp.pPusher->push(4, dwOffset);                                        // SetOverlayOffset
    vpp.pPusher->push(5, dwSizeIn);                                        // SetOverlaySizeIn
    vpp.pPusher->push(6, dwPointIn);                                       // SetOverlayPointIn
    vpp.pPusher->push(7, dwDeltaX);                                        // SetOverlayDuDx
    vpp.pPusher->push(8, dwDeltaY);                                        // SetOverlayDvDy
    vpp.pPusher->push(9, asmMergeCoords(dwDstX, dwDstY));                  // SetOverlayPointOut
    vpp.pPusher->push(10, asmMergeCoords(dwDstWidth, dwDstHeight));        // SetOverlaySizeOut
    vpp.pPusher->push(11, dwOverlayFormat);                                // SetOverlayFormat
    vpp.pPusher->adjust(12);
    vpp.pPusher->start(TRUE);
    vpp.pDriverData->dDrawSpareSubchannelObject = vpp.m_obj_a[OVERLAY_NDX].classIID;

    return TRUE;
}

//---------------------------------------------------------------------------
// vppFSFlip
//      Flip full screen mirrored adapter
LOCAL BOOL vppFSFlip(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwIndex)
{
    DWORD dwDAC, dwLUTFormat;
    Vpp_t &vpp = *pVpp;

#ifndef WINNT
    if (pVpp->dwOverlayFSTwinData.nvtwPhysicalModeImmediate.pHWCrtcCX == 0) {
        // ack! user has suddenly switched from clone mode to multimon
//        Sleep(VPP_TIMEOUT_TIME * 3);                // make sure all flips are flushed
        VppDestroyFSMirror(pVpp);
//        VppDestroyOverlay(pVpp);

        ResetTwinViewState (vpp.pDriverData);

        // special flag to indicate new display driver coming up
        pVpp->dwOverlayFSHead = 0xFFFFFFFE;

        // turn off mode setting bit to minimize impact of turning on FSmirror in the middle of a TwinView change
        vpp.regOverlayMode2 &= ~NV4_REG_OVL_MODE2_FSSETMODE;
        return TRUE;

//        VppCreateOverlay(pVpp);
//        VppCreateFSMirror(pVpp, pDriverData->dwOverlayFSSrcWidth, pDriverData->dwOverlayFSSrcHeight);
    }
#endif

    if (vpp.dwOverlayFSHead == 0) {
        dwDAC = vpp.hLutCursorDacBase;
        if (dwIndex & 0x1) {
            VppResetNotification(NULL, lpProcInfo->hPrimary0FlipOdd3);
            vpp.pFlipPrimaryNotifier[2].status = NV_IN_PROGRESS;
        } else {
            VppResetNotification(NULL, lpProcInfo->hPrimary0FlipEven3);
            vpp.pFlipPrimaryNotifier[1].status = NV_IN_PROGRESS;
        }
    } else {
        dwDAC = vpp.hLutCursorDacBase + 1;
        if (dwIndex & 0x1) {
            VppResetNotification(NULL, lpProcInfo->hPrimary1FlipOdd3);
            vpp.pFlipPrimaryNotifier[2].status = NV_IN_PROGRESS;
        } else {
            VppResetNotification(NULL, lpProcInfo->hPrimary1FlipEven3);
            vpp.pFlipPrimaryNotifier[1].status = NV_IN_PROGRESS;
        }
    }

    dwLUTFormat = vpp.dwOverlayFSPitch |
                 (VPP_NOTIFY_TYPE << 31) |
                 (NV067_SET_IMAGE_FORMAT_WHEN_NOT_WITHIN_BUFFER << 20);

    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(1, dwDAC);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV067_SET_IMAGE_OFFSET(dwIndex) | 0x80000);
    vpp.pPusher->push(3, vpp.dwOverlayFSOffset[vpp.dwOverlayFSIndex]);   // SetImage[i].offset
    vpp.pPusher->push(4, dwLUTFormat);                                                     // SetImage[i].format
    vpp.pPusher->adjust(5);
    vpp.pPusher->start(TRUE);
    vpp.pDriverData->dDrawSpareSubchannelObject = dwDAC;

    return TRUE;
}


//---------------------------------------------------------------------------
// vppSetSurfacesState
//      Sets the pixel format on all the surfaces we will use for video
//      post processing

LOCAL void vppSetSurfacesState(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;
    vpp.pDriverData->dwSharedClipChangeCount++;

    // set surfaces state

    // SURFACES_3D
    if (!(vpp.dwFlags & (VPP_FLAG_CELSIUS_3D | VPP_FLAG_KELVIN_3D))) {
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.hContextSurfacesARGB_ZS);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV053_SET_FORMAT | 0x80000);
        vpp.pPusher->push(3, NV053_SET_FORMAT_COLOR_LE_A8R8G8B8 |      // SetFormat
                     (NV053_SET_FORMAT_TYPE_PITCH << 8) |
                     (9 << 16) |
                     (10 << 24));
        vpp.pPusher->push(4, (2048 << 16) | 512);                      // SetClipSize
        vpp.pPusher->adjust(5);
    }

    // SURFACES_2D
    vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(1, vpp.hSurfaces2D);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000);
    vpp.pPusher->push(3, NV042_SET_COLOR_FORMAT_LE_A8R8G8B8);      // SetColorFormat

    // SURFACE_SWIZZLED
    vpp.pPusher->push(4, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
    vpp.pPusher->push(5, vpp.hContextSurfaceSwizzled);
    vpp.pPusher->push(6, SUB_CHANNEL(vpp.spareSubCh) | NV052_SET_FORMAT | 0x40000);
    vpp.pPusher->push(7, NV052_SET_FORMAT_COLOR_LE_A8R8G8B8 |     // SetFormat
                 (9 << 16) |
                 (10 << 24));

    vpp.pPusher->adjust(8);

    vpp.pDriverData->dDrawSpareSubchannelObject = vpp.hContextSurfaceSwizzled;
    vpp.pDriverData->bltData.dwLastColourFormat = NV042_SET_COLOR_FORMAT_LE_A8R8G8B8;
    vpp.pDriverData->TwoDRenderingOccurred = TRUE;
}

//---------------------------------------------------------------------------
// vppRestoreSurfacesState
//      Restores surfaces to default state.  Swizzled surface state is not
//      restored, D3D sets it before using it all the time

LOCAL void vppRestoreSurfacesState(Vpp_t *pVpp)
{
    DWORD dw2dFormat;
    Vpp_t &vpp = *pVpp;

    // restore surfaces2d
    switch (GET_MODE_BPP()) {
    case 8:
        dw2dFormat = NV062_SET_COLOR_FORMAT_LE_Y8;
        break;
    case 32:
        dw2dFormat = NV062_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;
        break;
    default:
        dw2dFormat = NV062_SET_COLOR_FORMAT_LE_R5G6B5;
        break;
    }
    vpp.pPusher->push(0, SUB_CHANNEL(vpp.surfaces2DSubCh) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000);
    vpp.pPusher->push(1, dw2dFormat);
    vpp.pPusher->adjust(2);

    vpp.pDriverData->bltData.dwLastColourFormat = dw2dFormat;

    vpp.pPusher->start(TRUE);
}

//---------------------------------------------------------------------------
// vppFlipSync
//      Flushes out overlay flips.  Returns true if succeeded

LOCAL BOOL vppFlipSync(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwIndex, BOOL bWait)
{
    HDRVEVENT hEvent;
    __int64 timeNow = 0x7FFFFFFFFFFFFFFF;
    static DWORD dwRetryCount = 0;
    Vpp_t &vpp = *pVpp;

    if (dwIndex) {
        hEvent = lpProcInfo->hOvlFlipOddEvent3;
    } else {
        hEvent = lpProcInfo->hOvlFlipEvenEvent3;
    }

    if (bWait) {
        // if (IS_EVENT_ERROR(WaitForSingleObject(hEvent, VPP_TIMEOUT_TIME))) {
        if (VppWaitForNotification(&(vpp.m_obj_a[OVERLAY_NDX].notifier_a[dwIndex+1]), hEvent, VPP_TIMEOUT_TIME)) {
            // NV11 mobile workaround, sometimes overlay flips are getting lost for unknown reasons
            // check to see if enough time has passed to guarantee this flip has flushed out
            static DWORD dwRetryCount = 0;
            if (dwRetryCount == 0) {
                nvQueryPerformanceCounter(&timeNow);
                if ((timeNow - vpp.pDriverData->flipOverlayRecord.llPreviousFlipTime) >= (vpp.pDriverData->flipPrimaryRecord.dwFlipDuration * 4)) {
                    dwRetryCount++;
                    return TRUE;
                }
            } else {
                dwRetryCount = 0;
            }
            // otherwise this is a real failure
            DPF(" *** OVERLAY FLIP event failure ***");
            EVENT_ERROR;
            vpp.m_obj_a[OVERLAY_NDX].notifier_a[1].status = 0;
            vpp.m_obj_a[OVERLAY_NDX].notifier_a[2].status = 0;
        }
    } else {
        if (vpp.m_obj_a[OVERLAY_NDX].notifier_a[dwIndex + 1].status == NV_IN_PROGRESS) {
            // NV11 mobile workaround, sometimes overlay flips are getting lost for unknown reasons
            // check to see if enough time has passed to guarantee this flip has flushed out
            nvQueryPerformanceCounter(&timeNow);
            if ((timeNow - vpp.pDriverData->flipOverlayRecord.llPreviousFlipTime) >= (vpp.pDriverData->flipPrimaryRecord.dwFlipDuration * 4)) {
                return TRUE;
            }
            // otherwise we really haven't flipped yet
            return FALSE;
        }
    }
    return TRUE;
}


//---------------------------------------------------------------------------
// vppFSFlipSync
//      Flushes out overlay flips.  Returns true if succeeded

LOCAL BOOL vppFSFlipSync(Vpp_t *pVpp,LPPROCESSINFO lpProcInfo, DWORD dwIndex, BOOL bWait)
{
    HDRVEVENT hEvent;
    Vpp_t &vpp = *pVpp;

    if (vpp.dwOverlayFSHead == 0) {
        if (dwIndex) {
            hEvent = lpProcInfo->hPrimary0FlipOdd3;
        } else {
            hEvent = lpProcInfo->hPrimary0FlipEven3;
        }
    } else {
        if (dwIndex) {
            hEvent = lpProcInfo->hPrimary1FlipOdd3;
        } else {
            hEvent = lpProcInfo->hPrimary1FlipEven3;
        }
    }

    if (bWait) {
        // if (IS_EVENT_ERROR(WaitForSingleObject(hEvent, VPP_TIMEOUT_TIME))) {
        if (VppWaitForNotification(&(vpp.pFlipPrimaryNotifier[dwIndex+1]), hEvent, VPP_TIMEOUT_TIME)) {
            DPF(" *** PRIMARY FLIP event failure ***");
            EVENT_ERROR;
            vpp.pFlipPrimaryNotifier[1].status = 0;
            vpp.pFlipPrimaryNotifier[2].status = 0;
        }
    } else {
        if (vpp.pFlipPrimaryNotifier[dwIndex + 1].status == NV_IN_PROGRESS) {
            return FALSE;
        }
    }
    return TRUE;
}


//---------------------------------------------------------------------------
// VppDoFlip
//     Does all the video post processing.  We never know whether the post
//      processing pipe has changed configuration since the last time, so
//      always flip.
//     Requires a source surface and 2 or (usually) 4 work surfaces.  Normal
//      usage leaves the source surface intact (mpeg-2 re-uses them) and
//      operates on each stage of the pipe by leaving the result in the
//      next working surface.  After each stage, the source is made to be
//      the destination of the previous stage.
//     Temporal filtering is special in that it snoops the 2 work surfaces
//      of the previous frame.
#define NEXT_SURFACE { \
            dwSrcOffset = dwDstOffset;  \
            dwSrcPitch = dwDstPitch;    \
            dwDstOffset = dwNextOffset; \
            dwDstPitch = dwNextPitch;   \
            dwNextOffset = dwSrcOffset; \
            dwNextPitch = dwSrcPitch;   \
            dwOpCount++;                \
        }

extern BOOL VppDoFlip(Vpp_t *pVpp,
                   DWORD dwOffset,
                   DWORD dwPitch,
                   DWORD dwWidth,
                   DWORD dwHeight,
                   DWORD dwFourCC,
                   DWORD dwFlags)
{
    DWORD dwSrcOffset, dwSrcPitch;
    DWORD dwDstOffset, dwDstPitch;
    DWORD dwNextOffset, dwNextPitch;
    DWORD dwOpCount;
    DWORD dwWorkSurfaces;
    DWORD dwIndex;
    enum eClassUsed { NV5Class, CelsiusClass, KelvinClass } classUsed;
    BOOL  isYUV9, isYUV12, isYUV422;
    BOOL  doConvert, doSubPicture, doXPreScale, doYPreScale, doPreScale, doCC;
    BOOL  doTemporal, doFSMirror, doDeinterlace = FALSE;
    BOOL  doLateFlipSync, doOvlZoom;
    BOOL  isDownScale;
    LPPROCESSINFO lpProcInfo = NULL;
    DWORD dwRoundX, dwRoundY;
    DWORD vppExec;
    BOOL  isField;
    DWORD dwPrescaleFactorX, dwPrescaleFactorY;
    Vpp_t &vpp = *pVpp;
    HDRVEVENT hLastStage, hLastFSStage;
    NvNotification *pLastStageNotifier, *pLastFSStageNotifier;
    NvU32 processID;

    nvAssert(pVpp);
    nvAssert(vpp.dwFlags & VPP_FLAG_OVERLAY_READY);

    if ((vpp.regVPPInvMask & VPP_MASTER_DISABLE) ||
        (vpp.dwOverlayDstHeight == 0) ||
        (vpp.dwOverlayDstWidth == 0)) {
        return TRUE;
    }

#ifdef VPP_SNOOP
    __int64 qwTime1, qwDelta, qwJitter;
    __int64 qwTime2 = 0;

    vpp.snoop.dwMarker1 = 0x11111111;
    vpp.snoop.qwMarker2 = 0x2222222222222222;

    vpp.snoop.dwLastFlags = dwFlags | 0x80000000;
    nvQueryPerformanceCounter(&qwTime1);
    qwDelta = qwTime1 - vpp.snoop.qwInterFrameTime;
    qwJitter = (qwDelta > vpp.snoop.qwInterFrameDelta) ?
               (qwDelta - vpp.snoop.qwInterFrameDelta) :
               (vpp.snoop.qwInterFrameDelta - qwDelta);
    vpp.snoop.qwInterFrameJitterAcc += qwJitter;
    if (qwJitter > vpp.snoop.qwInterFrameJitterPeak) {
        vpp.snoop.qwInterFrameJitterPeak = qwJitter;
    }
    vpp.snoop.qwInterFrameAcc += qwDelta;
    if (qwDelta > vpp.snoop.qwInterFramePeak) {
        vpp.snoop.qwInterFramePeak = qwDelta;
    }
    vpp.snoop.qwInterFrameDelta = qwDelta;
    vpp.snoop.qwInterFrameTime = qwTime1;
#else   // !VPP_SNOOP
#ifdef DEBUG
    char debugstr[256];
    debugstr[0] = '\0';
    if (dwFlags & VPP_ODD)           nvStrCat (debugstr, "odd  ");
    if (dwFlags & VPP_EVEN)          nvStrCat (debugstr, "even ");
    if (dwFlags & VPP_BOB)           nvStrCat (debugstr, "bob ");
    if (dwFlags & VPP_INTERLEAVED)   nvStrCat (debugstr, "int ");
    if (dwFlags & VPP_VIDEOPORT)     nvStrCat (debugstr, "vp ");
    if (dwFlags & VPP_WAIT)          nvStrCat (debugstr, "wait ");
    if (dwFlags & VPP_OPTIMIZEFLIP)  nvStrCat (debugstr, "opt ");
    nvStrCat (debugstr, "REQ: ");
    if (dwFlags & VPP_CONVERT)       nvStrCat (debugstr, "cvt ");
    if (dwFlags & VPP_SUBPICTURE)    nvStrCat (debugstr, "sp ");
    if (dwFlags & VPP_PRESCALE)      nvStrCat (debugstr, "ps ");
    if (dwFlags & VPP_COLOURCONTROL) nvStrCat (debugstr, "cc ");
    if (dwFlags & VPP_TEMPORAL)      nvStrCat (debugstr, "tf ");
    if (dwFlags & VPP_DEINTERLACE)   nvStrCat (debugstr, "df ");
    if (dwFlags & VPP_FSMIRROR)      nvStrCat (debugstr, "fs ");
#endif  // DEBUG
#endif  // VPP_SNOOP

    dwFlags &= ~(vpp.regVPPInvMask);

    dwIndex = vpp.overlayBufferIndex;

    processID = nvGetCurrentProcessId();

    lpProcInfo = pmGetProcess(vpp.pDriverData, processID);

    // last resort event creation
    if (lpProcInfo == NULL || lpProcInfo->hOvlFlipEvenEvent3 == NULL || lpProcInfo->hOvlFlipOddEvent3 == NULL) {
        pmAddProcess(vpp.pDriverData, processID);
        lpProcInfo = pmGetProcess(vpp.pDriverData, processID);
    }

    // we de-reference the <lpProcInfo> var to get event handles... even when we're not using events
    // so it HAS to point to something!
    nvAssert(lpProcInfo != NULL);
    if (lpProcInfo == NULL)
    {
        return FALSE;
    }

#if (IS_WINNT5 || IS_WIN9X)
    if (dwFourCC == FOURCC_NV12) {
        LPNVMCSURFACEFLAGS lpSurfaceFlags;
        DWORD dwNewOffset;

        lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&(vpp.pDriverData->nvMCSurfaceFlags[0]);

        while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
               (lpSurfaceFlags->dwMCSurfaceBase != (dwOffset + vpp.pDriverData->BaseAddress)))
            lpSurfaceFlags++;

        // NV12 surfaces are preconverted to YUYV, just adjust some parameters
        dwNewOffset = lpSurfaceFlags->dwMCSurface422Offset;

        dwPitch = ((dwWidth + 3) & ~3);
        dwPitch = dwPitch << 1;
        dwPitch = (dwPitch + 127) & ~127;

        if (vpp.pDriverData->bMCOverlaySrcIsSingleField1080i) {
            dwFlags &= ~(VPP_ODD | VPP_EVEN | VPP_INTERLEAVED);
        }

        // Check if we should be displaying the default or the filtered portion of the surface
        if (lpSurfaceFlags->bMCFrameIsFiltered) {
            dwNewOffset += (dwPitch * dwHeight);
        }

        dwOffset = dwNewOffset;
        dwFourCC = FOURCC_YUY2;
    }
#endif

    hLastStage           = NULL;
    hLastFSStage         = NULL;
    pLastStageNotifier   = NULL;
    pLastFSStageNotifier = NULL;

    dwWorkSurfaces = vpp.extraNumSurfaces;
    vppExec = 0;
    dwOpCount = 0;
    dwPrescaleFactorX = 0x100000;
    dwPrescaleFactorY = 0x100000;
    dwSrcOffset = dwOffset;
    dwSrcPitch = dwPitch;
    dwDstOffset = vpp.extraOverlayOffset[vpp.extraIndex];
    dwDstPitch = vpp.extraPitch;
    dwNextOffset = vpp.extraOverlayOffset[vpp.extraIndex + 1];
    dwNextPitch = vpp.extraPitch;

    isYUV9 = (dwFourCC == FOURCC_IF09 ||
              dwFourCC == FOURCC_YVU9 ||
              dwFourCC == FOURCC_IV32 ||
              dwFourCC == FOURCC_IV31);

    isYUV12 = (dwFourCC == FOURCC_YV12 ||
               dwFourCC == FOURCC_420i);

    isYUV422 = (dwFourCC == FOURCC_YUY2 ||
                dwFourCC == FOURCC_UYVY ||
                dwFourCC == FOURCC_YUNV ||
                dwFourCC == FOURCC_UYNV);

    isField = IS_FIELD(dwFlags);

    if (!isYUV9 && !isYUV12 && !isYUV422) {
        return FALSE;
    }

    doConvert = (dwFlags & VPP_CONVERT) &&
                (isYUV9 || isYUV12);

    doSubPicture = (dwFlags & VPP_SUBPICTURE) &&
                   (vpp.subPicture.offset != 0) &&
                 (((vpp.subPicture.pitch & NVS2VF_SP_ODD) && (dwFlags & VPP_ODD)) ||
                  ((vpp.subPicture.pitch & NVS2VF_SP_EVEN) && (dwFlags & VPP_EVEN)) ||
                  ((dwFlags & (VPP_EVEN | VPP_ODD))==0));

    doXPreScale = (vpp.dwOverlaySrcWidth > (vpp.dwOverlayDstWidth * vpp.dwOverlayMaxDownScaleX));
    doYPreScale = (vpp.dwOverlaySrcHeight > (vpp.dwOverlayDstHeight * vpp.dwOverlayMaxDownScaleY));
    if (vpp.pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
        // NV4/5 also prescales:
        //  - all BOBed fields
        //  - vertical upscales when HQVUp is enabled
        doYPreScale = doYPreScale || (vpp.regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE) || isField;
    }

    doPreScale = (dwFlags & VPP_PRESCALE) &&
                 (doXPreScale || doYPreScale);

    isDownScale = FALSE;
    if (doPreScale) {
        DWORD dwSrcHeight, dwSrcArea, dwDstArea;

        dwSrcHeight = vpp.dwOverlaySrcHeight;
        if ((dwFlags & (VPP_ODD | VPP_EVEN)) && (dwFlags & VPP_INTERLEAVED)) {
            // bob mode
            dwSrcHeight >>= 1;
        }
        dwSrcArea = dwSrcHeight * vpp.dwOverlaySrcWidth;
        dwDstArea = vpp.dwOverlayDstHeight * vpp.dwOverlayDstWidth;
        if (dwSrcArea > dwDstArea) {
            isDownScale = TRUE;
        }
    }

    doCC = (dwFlags & VPP_COLOURCONTROL) &&
           (vpp.pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) &&
           (vpp.fpOverlayShadow) &&
           (vpp.regOverlayColourControlEnable) &&
           (vpp.colorCtrl.lContrast != 0xFF ||
            vpp.colorCtrl.lBrightness != 0 ||
            vpp.colorCtrl.lHue != 0 ||
            vpp.colorCtrl.lSaturation != 0x100);

    doTemporal = (dwFlags & VPP_TEMPORAL) &&
                 (vpp.regOverlayMode & NV4_REG_OVL_MODE_TFILTER);

#if (NVARCH >= 10)
    doDeinterlace = (dwFlags & VPP_DEINTERLACE) && isField &&
                    (vpp.regOverlayMode & NV4_REG_OVL_MODE_DFILTER);
#endif

    if (vpp.dwFlags & VPP_FLAG_KELVIN_3D)
        classUsed = KelvinClass;
    else if (vpp.dwFlags & VPP_FLAG_CELSIUS_3D)
        classUsed = CelsiusClass;
    else
        classUsed = NV5Class;

    doFSMirror = (dwFlags & VPP_FSMIRROR) &&
                 (vpp.dwOverlayFSNumSurfaces > 0) &&
                 (vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK);

    if (dwWorkSurfaces < 2) {
        // no work surfaces, can't do anything
        doConvert = FALSE;
        doSubPicture = FALSE;
        doTemporal = FALSE;
        doDeinterlace = FALSE;
        doPreScale = FALSE;
        doCC = FALSE;
    } else if (dwWorkSurfaces < 4) {
        // there are only 2 or 3 work surfaces, we can only do one VPP stage, so prioritize
        if (doConvert) {
            doPreScale = FALSE;
            doSubPicture = FALSE;
            doTemporal = FALSE;
            doDeinterlace = FALSE;
        } else if (doPreScale) {
            doSubPicture = FALSE;
            doTemporal = FALSE;
            doDeinterlace = FALSE;
        } else if (doSubPicture) {
            doTemporal = FALSE;
            doDeinterlace = FALSE;
        }
        doCC = FALSE;   // needs more than 2 work surfaces
        if (classUsed == NV5Class) {
            // NV4/5 requires 4 work surfaces for temporal filtering
            doTemporal = FALSE;
        }
    }

    doOvlZoom = (vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_OVLZOOMQUADMASK);

    doLateFlipSync = (vpp.regOverlayMode & NV4_REG_OVL_MODE_LATEFLIPSYNC);

    // never allow flips to queue up if we are post processing
    if (!doLateFlipSync) {
        // early sync handles all cases
        if (!vppFlipSync(&vpp,lpProcInfo, dwIndex, dwFlags & VPP_WAIT)) {
            return FALSE;
        }
    }

    if (doConvert || doSubPicture || doPreScale || doCC || doDeinterlace || doTemporal || doFSMirror) {
        vpp.pPusher->waitForOtherChannels();
        vppSetSurfacesState(&vpp);
    }

    if (doFSMirror) {
        // if FSMirror is enabled, then we want to process the entire video image, not just the subrectangle
        dwWidth = vpp.dwOverlayFSSrcWidth;
        dwHeight = vpp.dwOverlayFSSrcHeight;
    }

    // always convert the entire surface cuz it saves us alot of subrectangle calculation,
    //  who cares about non-YUV422 performance
    if (doConvert) {
        if (vppConvert(&vpp,lpProcInfo, dwSrcOffset, dwSrcPitch, dwDstOffset,
                       dwDstPitch, dwWidth, dwHeight, dwFourCC)) {
            hLastStage = lpProcInfo->hConvert3;
            pLastStageNotifier = vpp.m_obj_a[V2V_FORMAT_NDX].notifier_a;

            NEXT_SURFACE;
            dwFourCC = FOURCC_YUY2;
            vppExec |= VPP_CONVERT;
        }
    }

    // codec does the subrectangle optimization
    if (doSubPicture) {
        if (vppSubPicture(&vpp,lpProcInfo, dwSrcOffset, dwSrcPitch, dwDstOffset,
                          dwDstPitch, dwWidth, dwHeight, dwFourCC)) {
            hLastStage = lpProcInfo->hSubPicture3;
            pLastStageNotifier = vpp.m_obj_a[DVDPICT_NDX].notifier_a;

            if (vpp.subPicture.pitch & NVS2VF_SP_SUBRECT) {
                // codec has requested in place composition, don't do next_surface
            } else {
                NEXT_SURFACE;
            }
            vppExec |= VPP_SUBPICTURE;
        }
    }

    if (doFSMirror || doOvlZoom) {
        // adjust for the FS mirror subrectangle
        dwRoundX = vpp.dwOverlayFSSrcMinX & 0x1;
        dwRoundY = vpp.dwOverlayFSSrcMinY & 0x1;
        dwSrcOffset += (vpp.dwOverlayFSSrcMinY & ~0x1) * dwSrcPitch;
        dwSrcOffset += (vpp.dwOverlayFSSrcMinX & ~0x1) << 1;   // always YUV422
        // ????? There's a really weird interaction with DVD subpicture class with width set to 8 pixels or less,
        //       and some GDI operation.  Not sure if it's hw or sw, but setting minimum width of 16 pixels makes
        //       the problem go away.  Overlay surfaces should have a minimum pitch greater than this, so very
        //       small overlay surfaces will be unaffected.
        dwWidth = max(vpp.dwOverlayFSSrcWidth, 16);
        dwHeight = vpp.dwOverlayFSSrcHeight;
        if (dwRoundY) {
            dwHeight += 1;
        }
    } else {
        // adjust for the subrectangle
        dwRoundX = vpp.dwOverlaySrcX & 0x1;
        dwRoundY = vpp.dwOverlaySrcY & 0x1;
        dwSrcOffset += (vpp.dwOverlaySrcY & ~0x1) * dwSrcPitch;
        dwSrcOffset += (vpp.dwOverlaySrcX & ~0x1) << 1;   // always YUV422
        // ????? There's a really weird interaction with DVD subpicture class with width set to 8 pixels or less,
        //       and some GDI operation.  Not sure if it's hw or sw, but setting minimum width of 16 pixels makes
        //       the problem go away.  Overlay surfaces should have a minimum pitch greater than this, so very
        //       small overlay surfaces will be unaffected.
        dwWidth = max(vpp.dwOverlaySrcSize & 0xFFFF, 16);
        dwHeight = vpp.dwOverlaySrcSize >> 16;
        if (dwRoundY) {
            dwHeight += 1;
        }
    }

    if (doDeinterlace) {
        if (dwOpCount == 0 && (vpp.regOverlayMode & NV4_REG_OVL_MODE_DF_PRECOPY)) {
            // need to keep a copy of source data
            if (vppPreCopy(&vpp,dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch, dwWidth, dwHeight)) {
                NEXT_SURFACE;
            } else {
                nvAssert(FALSE);
            }
        }

        BOOL bDeinterlaceResult = FALSE;

        switch (classUsed)
        {
        case CelsiusClass:
            bDeinterlaceResult = vppNv10Deinterlace(&vpp, lpProcInfo, dwSrcOffset, dwSrcPitch,
                                                    dwDstOffset, dwDstPitch,
                                                    dwWidth, dwHeight, dwFlags);
            break;

        case KelvinClass:
            bDeinterlaceResult = vppNv20Deinterlace(&vpp, lpProcInfo, dwSrcOffset, dwSrcPitch,
                                                    dwDstOffset, dwDstPitch,
                                                    dwWidth, dwHeight, dwFlags);
            break;
        }

        if (bDeinterlaceResult)
        {
            hLastStage = lpProcInfo->hDFilter3;
            pLastStageNotifier = NULL; // ??? vpp.m_obj_a[???].notifier_a;
            NEXT_SURFACE;
            vppExec |= VPP_DEINTERLACE;
        }
    }

    if (doTemporal) {
        if (dwOpCount == 0 && (vpp.regOverlayMode & NV4_REG_OVL_MODE_TF_PRECOPY)) {
            // need to keep a copy of source data
            if (vppPreCopy(&vpp,dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch, dwWidth, dwHeight)) {
                NEXT_SURFACE;
            } else {
                nvAssert(FALSE);
            }
        }

        BOOL bTemporalResult = FALSE;

        switch(classUsed)
        {
        case CelsiusClass:
            bTemporalResult = vppNv10Temporal(&vpp, lpProcInfo, dwSrcOffset, dwSrcPitch,
                                                dwDstOffset, dwDstPitch,
                                                dwWidth, dwHeight, dwFlags,
                                                &vpp.dwPrevFrameOffset);
            break;

        case KelvinClass:
            bTemporalResult = vppNv20Temporal(&vpp, lpProcInfo, dwSrcOffset, dwSrcPitch,
                                                dwDstOffset, dwDstPitch,
                                                dwWidth, dwHeight, dwFlags,
                                                &vpp.dwPrevFrameOffset);
            break;
        }

        if (bTemporalResult)
        {
            hLastStage = lpProcInfo->hTFilter3;
            pLastStageNotifier = NULL; // ??? vpp.m_obj_a[???].notifier_a;
            NEXT_SURFACE;
            vppExec |= VPP_TEMPORAL;

            // not the first frame any more
            vpp.regOverlayMode |= NV4_REG_OVL_MODE_NOTFIRST;
        }
    }

    if (doFSMirror) {
        if (vpp.dwOverlayFSNumSurfaces == 2) {
            // early flip sync for double buffered case
            vppFSFlipSync(pVpp,lpProcInfo, dwIndex, TRUE);
        } else if (vpp.dwOverlayFSNumSurfaces == 3) {
            // in 60 fps cases at 60 Hz, make sure only one flip is queued, may see tearing if 2 are queued
            vppFSFlipSync(pVpp,lpProcInfo, dwIndex ^ 1, TRUE);
        }
        if (vppFSMirror(pVpp,lpProcInfo, dwSrcOffset, dwSrcPitch, dwWidth, dwHeight, dwFourCC, dwFlags, dwPrescaleFactorX, dwPrescaleFactorY)) {
            hLastFSStage = lpProcInfo->hFSMirror3;
            pLastFSStageNotifier = NULL; // ??? vpp.m_obj_a[???].notifier_a;
            vppExec |= VPP_FSMIRROR;
        }
    }

    // Since OCC is much slower that prescale, arrange it so that OCC operates
    // on the smaller data set.
    if (isDownScale) {
        // do prescale first and then OCC
        if (doPreScale) {
            DWORD dwStartWidth, dwStartHeight;
            dwStartWidth = dwWidth;
            dwStartHeight = dwHeight;
            do {
                if (vppPreScale(pVpp,lpProcInfo, dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch,
                            &dwWidth, &dwHeight, dwFourCC, &dwFlags)) {
                    hLastStage = lpProcInfo->hSubPicture3;
                    pLastStageNotifier = vpp.m_obj_a[DVDPICT_NDX].notifier_a;
                    NEXT_SURFACE;
                    vppExec |= VPP_PRESCALE;
                }
            } while ((dwWidth  > (vpp.dwOverlayDstWidth * vpp.dwOverlayMaxDownScaleX) &&
                      vpp.dwOverlayDstWidth > 4) ||
                     (dwHeight > (vpp.dwOverlayDstHeight * vpp.dwOverlayMaxDownScaleY) &&
                      vpp.dwOverlayDstHeight > 4));
            dwPrescaleFactorX = (dwWidth << 20) / dwStartWidth;
            dwPrescaleFactorY = (dwHeight << 20) / dwStartHeight;
        }
        if (doCC) {
            if (dwOpCount == 0 && vpp.colorCtrl.lContrast != 0xFF) {
                // need to copy source data because contrast may be done in place
                if (vppPreCopy(&vpp,dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch,
                               dwWidth, dwHeight)) {
                    NEXT_SURFACE;
                } else {
                    nvAssert(FALSE);
                }
            }
            if (vppColourControl(&vpp,lpProcInfo, dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch,
                                 dwWidth, dwHeight, dwFourCC, dwFlags)) {
                hLastStage = lpProcInfo->hColourControl3;
                pLastStageNotifier = vpp.m_obj_a[V2OSH_FORMAT_NDX].notifier_a;
                NEXT_SURFACE;
                vppExec |= VPP_COLOURCONTROL;
            }
        }
    } else {
        // do OCC first and then prescale
        if (doCC) {
            if (dwOpCount == 0 && vpp.colorCtrl.lContrast != 0xFF) {
                // need to copy source data because contrast may be done in place
                if (vppPreCopy(&vpp,dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch,
                               dwWidth, dwHeight)) {
                    NEXT_SURFACE;
                } else {
                    nvAssert(FALSE);
                }
            }
            if (vppColourControl(&vpp,lpProcInfo, dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch,
                                 dwWidth, dwHeight, dwFourCC, dwFlags)) {
                hLastStage = lpProcInfo->hColourControl3;
                pLastStageNotifier = vpp.m_obj_a[V2OSH_FORMAT_NDX].notifier_a;
                NEXT_SURFACE;
                vppExec |= VPP_COLOURCONTROL;
            }
        }
        if (doPreScale) {
            DWORD dwStartWidth, dwStartHeight;
            dwStartWidth = dwWidth;
            dwStartHeight = dwHeight;
            do {
                if (vppPreScale(&vpp,lpProcInfo, dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch,
                                &dwWidth, &dwHeight, dwFourCC, &dwFlags)) {
                    hLastStage = lpProcInfo->hSubPicture3;
                    pLastStageNotifier = vpp.m_obj_a[DVDPICT_NDX].notifier_a;
                    NEXT_SURFACE;
                    vppExec |= VPP_PRESCALE;
                }
            } while ((dwWidth  > (vpp.dwOverlayDstWidth * vpp.dwOverlayMaxDownScaleX) &&
                      vpp.dwOverlayDstWidth > 4) ||
                     (dwHeight > (vpp.dwOverlayDstHeight * vpp.dwOverlayMaxDownScaleY) &&
                      vpp.dwOverlayDstHeight > 4));
            dwPrescaleFactorX = (dwWidth << 20) / dwStartWidth;
            dwPrescaleFactorY = (dwHeight << 20) / dwStartHeight;
        }
    }

    if (doTemporal && (classUsed == NV5Class)) {
        if (dwOpCount == 0) {
            // need to copy source data to a work area whose dimensions are a power of 2
            if (vppPreCopy(&vpp,dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch, dwWidth, dwHeight)) {
                NEXT_SURFACE;
            } else {
                nvAssert(FALSE);
            }
        }

        if (vppNv5Temporal(&vpp,lpProcInfo, dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch,
                           dwWidth, dwHeight, &vpp.dwPrevFrameOffset)) {
            hLastStage = lpProcInfo->hTFilter3;
            pLastStageNotifier = NULL; // ??? vpp.m_obj_a[???].notifier_a;
            vppExec |= VPP_TEMPORAL;
            // don't do a NEXT_SURFACE, temporal filter operates in place and leaves
            // a swizzled version at dwDstOffset for use on the next frame (NV4/5)

            // not the first frame any more
            vpp.regOverlayMode |= NV4_REG_OVL_MODE_NOTFIRST;
        }
    }

    if (doConvert || doSubPicture || doPreScale || doCC || doDeinterlace || doTemporal || doFSMirror) {
        vppRestoreSurfacesState(&vpp);
    }

    if (!doTemporal) {
        // next time tfilter is enabled, it will be the first frame
        vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_NOTFIRST;
    }

    if ((dwFlags & VPP_OPTIMIZEFLIP) && dwOpCount == 0) {
        // nothing happened, don't flip
        return FALSE;
    }

    if (doFSMirror && !doOvlZoom) {
        // now do the subrectangle adjustments for FS mirror so things flip properly
        dwSrcOffset += vpp.dwOverlaySrcY * dwSrcPitch;
        dwSrcOffset += (vpp.dwOverlaySrcX & ~0x1) << 1;   // always YUV422
        // ????? There's a really weird interaction with DVD subpicture class with width set to 8 pixels or less,
        //       and some GDI operation.  Not sure if it's hw or sw, but setting minimum width of 16 pixels makes
        //       the problem go away.  Overlay surfaces should have a minimum pitch greater than this, so very
        //       small overlay surfaces will be unaffected.
        dwWidth = max(vpp.dwOverlaySrcSize & 0xFFFF, 16);
        dwHeight = vpp.dwOverlaySrcSize >> 16;
    } else {
        if (dwRoundY) {
            dwSrcOffset += dwSrcPitch;
            dwHeight -= 1;
        }
    }

    //
    // NB: any local variables or parameters used before & after this point must be saved
    //    (excluding lProcinfo)
    //     This SAVE/RESTORE feature is only used by Ravisent decoders
    //
    if (dwFlags & VPP_SAVE_STATE) {
        vpp.pipeState.dwOpCount            = dwOpCount;
        vpp.pipeState.hLastStage           = hLastStage;
        vpp.pipeState.pLastStageNotifier   = pLastStageNotifier;
        vpp.pipeState.hLastFSStage         = hLastFSStage;
        vpp.pipeState.pLastFSStageNotifier = pLastFSStageNotifier;
        vpp.pipeState.doLateFlipSync       = (NvU8)doLateFlipSync;
        vpp.pipeState.dwIndex              = dwIndex;
        vpp.pipeState.dwSrcOffset          = dwSrcOffset;
        vpp.pipeState.dwSrcPitch           = dwSrcPitch;
        vpp.pipeState.dwWidth              = dwWidth;
        vpp.pipeState.dwHeight             = dwHeight;
        vpp.pipeState.dwFourCC             = dwFourCC;
        vpp.pipeState.dwFlags              = dwFlags & ~VPP_SAVE_STATE;
        vpp.pipeState.dwWorkSurfaces       = dwWorkSurfaces;
        vpp.pipeState.dwPrescaleFactorX    = dwPrescaleFactorX;
        vpp.pipeState.dwPrescaleFactorY    = dwPrescaleFactorY;
        vpp.pipeState.doFSMirror           = (NvU8)doFSMirror;
#ifdef VPP_SNOOP
        vpp.pipeState.qwTime1              = qwTime1;
        vpp.pipeState.qwTime2              = qwTime2;
        vpp.pipeState.qwDelta              = qwDelta;
        vpp.pipeState.qwJitter             = qwJitter;
#endif
        return TRUE;
    }

    if (dwFlags & VPP_RESTORE_STATE) {
        dwOpCount            = vpp.pipeState.dwOpCount;
        hLastStage           = vpp.pipeState.hLastStage;
        pLastStageNotifier   = vpp.pipeState.pLastStageNotifier;
        hLastFSStage         = vpp.pipeState.hLastFSStage;
        pLastFSStageNotifier = vpp.pipeState.pLastFSStageNotifier;
        doLateFlipSync       = vpp.pipeState.doLateFlipSync;
        dwIndex              = vpp.pipeState.dwIndex;
        dwSrcOffset          = vpp.pipeState.dwSrcOffset;
        dwSrcPitch           = vpp.pipeState.dwSrcPitch;
        dwWidth              = vpp.pipeState.dwWidth;
        dwHeight             = vpp.pipeState.dwHeight;
        dwFourCC             = vpp.pipeState.dwFourCC;
        dwFlags              = vpp.pipeState.dwFlags;
        dwWorkSurfaces       = vpp.pipeState.dwWorkSurfaces;
        vppExec              = vpp.pipeState.vppExec;
        dwPrescaleFactorX    = vpp.pipeState.dwPrescaleFactorX;
        dwPrescaleFactorY    = vpp.pipeState.dwPrescaleFactorY;
        doFSMirror           = vpp.pipeState.doFSMirror;
#ifdef VPP_SNOOP
        qwTime1              = vpp.pipeState.qwTime1;
        qwTime2              = vpp.pipeState.qwTime2;
        qwDelta              = vpp.pipeState.qwDelta;
        qwJitter             = vpp.pipeState.qwJitter;
#endif
    }

    // wait for all stages to complete
    if (dwOpCount > 0 && hLastStage) {
        if (VppWaitForNotification(pLastStageNotifier, hLastStage, VPP_TIMEOUT_TIME)) {
            DPF(" *** A VPP stage notification failed to return during timeout period ***");
            EVENT_ERROR;
        }

    } else {
        if (vpp.m_obj_a[DMABLT_TO_VID_NDX].notifier_a[1].status == NV_IN_PROGRESS) {
            // wait for any DMA blits to flush out before processing
            // if (IS_EVENT_ERROR(WaitForSingleObject(lpProcInfo->hDMABlitToVid3, VPP_TIMEOUT_TIME))) {
            if (VppWaitForNotification(&(vpp.m_obj_a[DMABLT_TO_VID_NDX].notifier_a[1]), lpProcInfo->hDMABlitToVid3, VPP_TIMEOUT_TIME)) {
                DPF(" *** DMA BLIT event failure ***");
                EVENT_ERROR;
            }
        }
        // wait for any backdoor FOURCC blits to flush out
        if (VppWaitForNotification(vpp.pPusherSyncNotifier, lpProcInfo->hFSMirror3, VPP_TIMEOUT_TIME)) {
            DPF(" *** FOURCC BLIT event failure ***");
            EVENT_ERROR;
        }
    }

    if (doLateFlipSync) {
        // late sync only for decoders that use lots of stages, will not work for
        // fewer than 3 buffers, decoder must not go back and re-use a surface that
        // it just flipped away from
        if (!vppFlipSync(&vpp,lpProcInfo, dwIndex, TRUE)) {
            return FALSE;
        }
    }

    // always do the flip, even if we are not flipping
    if (vppFlip(&vpp,lpProcInfo, dwSrcOffset, dwSrcPitch, dwWidth, dwHeight, dwIndex, dwFourCC, dwFlags, dwPrescaleFactorX, dwPrescaleFactorY)) {
        vpp.pDriverData->dwTVTunerFlipCount = 0;
        vpp.dwOverlayFlipCount++;
        vpp.overlayBufferIndex ^= 1;
        if (dwWorkSurfaces == 0) {
            vpp.extraIndex = 0;
        } else if (dwWorkSurfaces <= 3) {
            vpp.extraIndex = (vpp.extraIndex + 1) % dwWorkSurfaces;
        } else {
            vpp.extraIndex = (vpp.extraIndex + 2) % dwWorkSurfaces;
        }

        // successful overlay flip, now match it with the FS mirror image
        if (doFSMirror) {
            if (hLastFSStage) {
                if (VppWaitForNotification(pLastFSStageNotifier, hLastFSStage, VPP_TIMEOUT_TIME)) {
                    dbgError(" *** FS event failure ***");
                    EVENT_ERROR;
                }
            }
            if (vpp.dwOverlayFSNumSurfaces > 3) {
                // late flip sync for quad buffered case
                vppFSFlipSync(pVpp,lpProcInfo, dwIndex, TRUE);
            }
            vppFSFlip(pVpp,lpProcInfo, dwIndex);
            if (vpp.dwOverlayFSNumSurfaces != 0) {
                vpp.dwOverlayFSIndex = (vpp.dwOverlayFSIndex + 1) % vpp.dwOverlayFSNumSurfaces;
            }
        }
    } else {
        return FALSE;
    }

#ifdef VPP_SNOOP

    vpp.snoop.dwLastExec = vppExec;
    nvQueryPerformanceCounter(&qwTime2);
    qwDelta = qwTime2 - qwTime1;
    qwJitter = (qwDelta > vpp.snoop.qwIntraFrameDelta) ?
               (qwDelta - vpp.snoop.qwIntraFrameDelta) :
               (vpp.snoop.qwIntraFrameDelta - qwDelta);
    vpp.snoop.qwIntraFrameJitterAcc += qwJitter;
    if (qwJitter > vpp.snoop.qwIntraFrameJitterPeak) {
        vpp.snoop.qwIntraFrameJitterPeak = qwJitter;
    }
    vpp.snoop.qwIntraFrameAcc += qwDelta;
    if (qwDelta > vpp.snoop.qwIntraFramePeak) {
        vpp.snoop.qwIntraFramePeak = qwDelta;
    }
    vpp.snoop.qwIntraFrameDelta = qwDelta;
    vpp.snoop.qwCount += 1;
    vpp.snoop.dwLastFlags &= 0x7FFFFFFF;

#else   // !VPP_SNOOP
#ifdef DEBUG
    nvStrCat (debugstr, "ENA: ");
    if (doConvert)                       nvStrCat (debugstr, "cvt ");
    if (doSubPicture)                    nvStrCat (debugstr, "sp ");
    if (doPreScale)                      nvStrCat (debugstr, "ps ");
    if (doCC)                            nvStrCat (debugstr, "cc ");
    if (doTemporal)                      nvStrCat (debugstr, "tf ");
    if (doDeinterlace)                   nvStrCat (debugstr, "df ");
    if (doFSMirror)                      nvStrCat (debugstr, "fs ");
    nvStrCat (debugstr, "EXEC: ");
    if (vppExec & VPP_CONVERT)           nvStrCat (debugstr, "cvt ");
    if (vppExec & VPP_SUBPICTURE)        nvStrCat (debugstr, "sp ");
    if (vppExec & VPP_PRESCALE)          nvStrCat (debugstr, "ps ");
    if (vppExec & VPP_COLOURCONTROL)     nvStrCat (debugstr, "cc ");
    if (vppExec & VPP_TEMPORAL)          nvStrCat (debugstr, "tf ");
    if (vppExec & VPP_DEINTERLACE)       nvStrCat (debugstr, "df ");
    if (vppExec & VPP_FSMIRROR)          nvStrCat (debugstr, "fs ");
    DPF(debugstr);
#endif  // DEBUG
#endif  // VPP_SNOOP

    return TRUE;
}

// -----------------------------------------------------------------------------------------
// VppSetOverlayColourControl
//      Sets the overlay colour controls
//
// Globals:
//      OverlayColourControl

extern NvU8 VppSetOverlayColourControl(Vpp_t *pVpp)
{
    long            newHWContrast;
    long            newHWBrightness;
    long            newHWSaturation;
    long            newHWSine;
    long            newHWCosine;
    float           fRadians;
    Vpp_t &vpp = *pVpp;

    if (!(pVpp->dwFlags & VPP_FLAG_OVERLAY_READY))
    {
       return FALSE;
    }

    if (vpp.pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
        /* Convert to equivalent NV hardware values */
        newHWContrast = vpp.colorCtrl.lContrast;
        if (newHWContrast < 0)
            newHWContrast = 0;
        if (newHWContrast > 19999)
            newHWContrast = 19999;
        newHWContrast *= 8192;
        newHWContrast /= 20000;

        newHWBrightness = vpp.colorCtrl.lBrightness;
        newHWBrightness -= 750;
        newHWBrightness *= 256;
        newHWBrightness /= 10000;

        newHWSaturation = vpp.colorCtrl.lSaturation;
        if (newHWSaturation < 0)
            newHWSaturation = 0;
        if (newHWSaturation > 19999)
            newHWSaturation = 19999;
        newHWSaturation *= 8192;
        newHWSaturation /= 20000;

        fRadians = (float)((vpp.colorCtrl.lHue * 3.1415926) / 180);
        // TBD: using sin/cos in the driver may not be doable on all platforms.  Is the FPU ready in this
        //   cpu mode?  [it is for Windows systems in Ddraw] - @mjl
        newHWSine = (long)(sin(fRadians) * newHWSaturation);
        newHWCosine = (long)(cos(fRadians) * newHWSaturation);

        /* Merge luminance values */
        newHWBrightness <<= 16;
        newHWBrightness |= newHWContrast;

        /* Merge chrominance values */
        newHWCosine &= 0xFFFF;
        newHWSine <<= 16;
        newHWSine |= newHWCosine;

        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.m_obj_a[OVERLAY_NDX].classIID);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | NV07A_SET_OVERLAY_LUMINANCE_A | 0x80000);
        vpp.pPusher->push(3, newHWBrightness);     // SetOverlayLuminanceA
        vpp.pPusher->push(4, newHWSine);           // SetOverlayChrominanceA
        vpp.pPusher->adjust(5);
        vpp.pPusher->start(TRUE);
        vpp.pDriverData->dDrawSpareSubchannelObject = 0;
        return TRUE;
    }
    return FALSE;

}

// These blt support routines should be moved to a shared blt routine when that is made OS independant @mjl@

// -----------------------------------------------------------------------------
// vppBltUpdateClip
//      Updates the NV clipper state if necessary
// -----------------------------------------------------------------------------
LOCAL void vppBltUpdateClip(Vpp_t *pVpp)
{
    Vpp_t &vpp = *pVpp;

    if (vpp.pDriverData->ddClipUpdate ||
        vpp.pDriverData->dwSharedClipChangeCount != vpp.pDriverData->dwDDMostRecentClipChangeCount)
    {
        // Why not use spare here? @mjl@
        vpp.pPusher->push (0,SUB_CHANNEL(vpp.surfaces2DSubCh) | 0x40000);
        vpp.pPusher->push (1,vpp.hImageBlackRect);
        vpp.pPusher->push (2,SUB_CHANNEL(vpp.surfaces2DSubCh) +
                                IMAGE_BLACK_RECTANGLE_POINT_OFFSET | 0x80000);
        vpp.pPusher->push (3,0);
        vpp.pPusher->push (4,asmMergeCoords(NV_MAX_X_CLIP,NV_MAX_Y_CLIP));
        vpp.pPusher->push (5,SUB_CHANNEL(vpp.surfaces2DSubCh) | 0x40000);
        vpp.pPusher->push (6,vpp.hSurfaces2D);

        vpp.pPusher->adjust (7);
        vpp.pPusher->start  (TRUE);

        vpp.pDriverData->dwSharedClipChangeCount++;
        vpp.pDriverData->dwDDMostRecentClipChangeCount = vpp.pDriverData->dwSharedClipChangeCount;
        vpp.pDriverData->ddClipUpdate = FALSE;
    }
}

// -----------------------------------------------------------------------------
// vppBltFillBlock
//      Fill a block of memory.  Self contained, for internal use only
//
// -----------------------------------------------------------------------------
LOCAL void vppBltFillBlock(Vpp_t *pVpp, NvU32 dwColor, NvU32 dwOffset, NvU32 dwPitch, NvU32 dwWidth, NvU32 dwHeight, NvU32 dwBytesPerPel)
{
    NvU32 dwCombinedPitch, dwColourFormat, dwColourFormat2;
    NvU32 dwDstX;
    Vpp_t &vpp = *pVpp;

    vpp.pDriverData->blitCalled = TRUE;
    vpp.pDriverData->TwoDRenderingOccurred = 1;

    // make sure we sync with other channels before writing put
    vpp.pPusher->setSyncChannelFlag();

    vppBltUpdateClip(pVpp);

    // alignment
    dwDstX = (dwOffset & NV_BYTE_ALIGNMENT_PAD) / dwBytesPerPel;
    dwOffset &= ~NV_BYTE_ALIGNMENT_PAD;

    // source key is disabled
    if (vpp.pDriverData->bltData.dwLastColourKey != 0xFFFFFFFF) {
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.hContextColorKey);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | SET_TRANSCOLOR_OFFSET | 0x40000);
        vpp.pPusher->push(3, 0);
        vpp.pPusher->adjust(4);
        vpp.pDriverData->bltData.dwLastColourKey = 0xFFFFFFFF;
        vpp.pDriverData->dDrawSpareSubchannelObject = vpp.hContextColorKey;
    }

    // ROP is always SRCCOPY
    if (vpp.pDriverData->bltData.dwLastRop != SRCCOPYINDEX) {
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.ropSubCh) | SET_ROP_OFFSET | 0x40000);
        vpp.pPusher->push(1, SRCCOPYINDEX);
        vpp.pPusher->adjust(2);
        vpp.pDriverData->bltData.dwLastRop = SRCCOPYINDEX;
    }

    // set surfaces 2d
    switch (dwBytesPerPel) {
    case 1:
        dwColourFormat = NV062_SET_COLOR_FORMAT_LE_Y8;
        dwColourFormat2 = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;
        break;
    case 4:
        dwColourFormat = NV062_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;
        dwColourFormat2 = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;
        break;
    default:
        dwColourFormat = NV062_SET_COLOR_FORMAT_LE_R5G6B5;
        dwColourFormat2 = NV04A_SET_COLOR_FORMAT_LE_X16R5G6B5;
        break;
    }
    dwCombinedPitch = (dwPitch << 16) | dwPitch;
    if (vpp.pDriverData->bltData.dwLastColourFormat != dwColourFormat ||
        vpp.pDriverData->bltData.dwLastCombinedPitch != dwCombinedPitch ||
        vpp.pDriverData->bltData.dwLastSrcOffset != dwOffset ||
        vpp.pDriverData->bltData.dwLastDstOffset != dwOffset) {

        vpp.pPusher->push(0, SUB_CHANNEL(vpp.surfaces2DSubCh) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
        vpp.pPusher->push(1, dwColourFormat);          // SetColorFormat
        vpp.pPusher->push(2, dwCombinedPitch);         // SetPitch
        vpp.pPusher->push(3, dwOffset);                // SetSrcOffset
        vpp.pPusher->push(4, dwOffset);                // SetDstOffset
        vpp.pPusher->adjust(5);
        vpp.pDriverData->bltData.dwLastColourFormat = dwColourFormat;
        vpp.pDriverData->bltData.dwLastCombinedPitch = dwCombinedPitch;
        vpp.pDriverData->bltData.dwLastSrcOffset = dwOffset;
        vpp.pDriverData->bltData.dwLastDstOffset = dwOffset;

        // Also set mono pattern mask when pixel depth changes.
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.hContextPattern);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) + NV044_SET_PATTERN_SELECT | 0xC0000);
        vpp.pPusher->push(3, NV044_SET_PATTERN_SELECT_MONOCHROME);
        switch (dwBytesPerPel) {
        case 1:
            vpp.pPusher->push(4,NV_ALPHA_1_008);
            vpp.pPusher->push(5,NV_ALPHA_1_008);
            break;
        case 4:
            vpp.pPusher->push(4,NV_ALPHA_1_032);
            vpp.pPusher->push(5,NV_ALPHA_1_032);
            break;
        default:
            vpp.pPusher->push(4,NV_ALPHA_1_016);
            vpp.pPusher->push(5,NV_ALPHA_1_016);
            break;
        }
        vpp.pPusher->adjust(6);
        vpp.pDriverData->dDrawSpareSubchannelObject = vpp.hContextPattern;
    }

    vpp.pPusher->push(0, SUB_CHANNEL(vpp.ropRectTextSubCh) | NV04A_SET_COLOR_FORMAT | 0x40000);
    vpp.pPusher->push(1, dwColourFormat2);
    vpp.pPusher->push(2, SUB_CHANNEL(vpp.ropRectTextSubCh) | RECT_AND_TEXT_COLOR1A_OFFSET | 0xC0000);
    vpp.pPusher->push(3, dwColor);
    vpp.pPusher->push(4, asmMergeCoords(0, dwDstX));
    vpp.pPusher->push(5, asmMergeCoords(dwHeight, dwWidth));
    vpp.pPusher->adjust(6);
    vpp.pPusher->start(TRUE);
}

// -----------------------------------------------------------------------------
// vppBltWriteDword
//      Fill a dword with data.  Self contained, for internal use only.  Useful
//      for writing data to video memory in win2k
void VppBltWriteDword(Vpp_t *pVpp, NvU32 dwOffset, NvU32 dwIndex, NvU32 dwData)
{
    Vpp_t &vpp = *pVpp;
    NvU32 dwCombinedPitch;

    vpp.pDriverData->blitCalled = TRUE;             // TBD: see that this is used or ignored properly in NT4 @mjl@
    vpp.pDriverData->TwoDRenderingOccurred = 1;

    // make sure we sync with other channels before writing put
    vpp.pPusher->setSyncChannelFlag();

    vppBltUpdateClip(pVpp);

    // source key is disabled
    if (vpp.pDriverData->bltData.dwLastColourKey != 0xFFFFFFFF) {
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.spareSubCh) | 0x40000);
        vpp.pPusher->push(1, vpp.hContextColorKey);
        vpp.pPusher->push(2, SUB_CHANNEL(vpp.spareSubCh) | SET_TRANSCOLOR_OFFSET | 0x40000);
        vpp.pPusher->push(3, 0);
        vpp.pPusher->adjust(4);
        vpp.pDriverData->bltData.dwLastColourKey = 0xFFFFFFFF;
        vpp.pDriverData->dDrawSpareSubchannelObject = vpp.hContextColorKey;
    }

    // ROP is always SRCCOPY
    if (vpp.pDriverData->bltData.dwLastRop != SRCCOPYINDEX) {
        vpp.pPusher->push(0, SUB_CHANNEL(vpp.ropSubCh) | SET_ROP_OFFSET | 0x40000);
        vpp.pPusher->push(1, SRCCOPYINDEX);
        vpp.pPusher->adjust(2);
        vpp.pDriverData->bltData.dwLastRop = SRCCOPYINDEX;
    }

    // set surfaces2D
    dwCombinedPitch = ((vpp.pDriverData->dwSurfaceAlignPad + 1) << 16) | (vpp.pDriverData->dwSurfaceAlignPad + 1);
    if (vpp.pDriverData->bltData.dwLastColourFormat != NV062_SET_COLOR_FORMAT_LE_Y32 ||
        vpp.pDriverData->bltData.dwLastCombinedPitch != dwCombinedPitch ||
        vpp.pDriverData->bltData.dwLastSrcOffset != dwOffset ||
        vpp.pDriverData->bltData.dwLastDstOffset != dwOffset) {

        vpp.pPusher->push(0, SUB_CHANNEL(vpp.surfaces2DSubCh) | SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000);
        vpp.pPusher->push(1, NV062_SET_COLOR_FORMAT_LE_Y32);   // SetColorFormat
        vpp.pPusher->push(2, dwCombinedPitch);                 // SetPitch
        vpp.pPusher->push(3, dwOffset);                        // SetSrcOffset
        vpp.pPusher->push(4, dwOffset);                        // SetDstOffset
        vpp.pPusher->adjust(5);
        vpp.pDriverData->bltData.dwLastColourFormat = NV062_SET_COLOR_FORMAT_LE_Y32;
        vpp.pDriverData->bltData.dwLastCombinedPitch = dwCombinedPitch;
        vpp.pDriverData->bltData.dwLastSrcOffset = dwOffset;
        vpp.pDriverData->bltData.dwLastDstOffset = dwOffset;
    }

    vpp.pPusher->push(0, SUB_CHANNEL(vpp.ropRectTextSubCh) | RECT_AND_TEXT_COLOR1A_OFFSET | 0xC0000);
    vpp.pPusher->push(1, dwData);
    vpp.pPusher->push(2, asmMergeCoords(0, dwIndex));
    vpp.pPusher->push(3, asmMergeCoords(1, 1));
    vpp.pPusher->adjust(4);

    vpp.pPusher->flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
}

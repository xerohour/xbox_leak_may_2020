 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
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
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
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

//***************************************************************************
//  Module Name:
//
//      nvpecore.c
//
//  Abstract:
//
//      This module contains the core implementation of VideoPort Extension
//      and Kernel mode video transport on both Win2k and Win9x
//
//      The DirectDraw Video Port Extensions (VPE) are the services that
//      control hardware videoports, where video data is streamed into
//      off-screen memory from an external device such as a TV tuner or
//      MPEG decoder, and then displayed on the screen by a video overlay.
//
//      To support VPE, you'll need the following:
//
//          1. A 2-D driver with DirectDraw overlay support;
//          2. A video input device that interfaces with the videoport;
//          3. A bunch of VPE code in the display driver;
//          4. A bunch of VPE code here in the miniport/miniVDD.
//
//
//  Environment:
//
//      Kernel mode
//
//  Copyright (c) 1997 Microsoft Corporation
//
//***************************************************************************

#ifdef NVPE

#include "nvpecore.h" // local definitions

//#define _NVP_DTRACE
//#define _NVP_TEST_NEXT_FIELD
#define FORCE_INTERLEAVE_MEM	//NOTE: If this is defined, DoBobFromInterleaved 
								//		is always called.
								//		Surf is always interleaved even if ddraw
								//		requests for non-interleaved surf. Need to
								//		double dwOverlaySrcHeight. Can't rely on ddraw
								//		interleave caps and our NV_VFM_FORMAT_X caps
								//		that is shared with the overlay code.

#ifdef _NVP_DTRACE
#define DEFINE_GLOBALS
#include "dtrace.h"
#endif

////////// globals
BOOL g_bFlip = FALSE;
DWORD g_nIntCounter = 0;
int DebugLevel=0;               //Debug level for printfs
#ifdef _NVP_TEST_NEXT_FIELD
int g_nNextFieldId = 0;
#endif

//Local Functions
VP_STATUS NVPFlushChannel(PNVP_CONTEXT pVPEContext);

#ifdef VPEFSMIRROR
// extern function not found in one of the header files need to integrate later [XW]
extern U032 rmNVPAlloc(U032 hClient, U032 hChannel,U032 hObject,U032 hClass, PVOID pAllocParms);
#endif


/*++

Routine Description:

    This function flips the video port to the target surface.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    FlipVideoPortInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/

DWORD FlipVideoPort(
    PVOID HwDeviceExtension,
    PNVP_DDFLIPVIDEOPORTINFO pFlipVideoPortInfo,
    PVOID lpOutput)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    DBGPRINT(DBGTRACE,"FlipVideoPort()");
#ifdef _NOT_IMPLEMENTED
    //
    // Get offset (from framebuffer) and add offset from surface:
    //
    ULONG offset;

    offset = pFlipVideoPortInfo->lpTargetSurface->dwSurfaceOffset
           + pFlipVideoPortInfo->lpVideoPortData->dwOriginOffset;

    // flip the actual hw surface here
#endif
    return DX_OK;
}

/*++

Routine Description:

    This function flips the overlay to the target surface.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    FlipOverlayInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/
DWORD FlipOverlay(
    PVOID HwDeviceExtension,
    PNVP_DDFLIPOVERLAYINFO pFlipOverlayInfo,
    PVOID lpOutput)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)HwDeviceExtension;
    DDSURFACEDATA* pCurSurf = (DDSURFACEDATA*) (pFlipOverlayInfo->lpCurrentSurface);
    DDSURFACEDATA* pTargSurf = (DDSURFACEDATA*) (pFlipOverlayInfo->lpTargetSurface);
    PNVP_CONTEXT pVPEContext;
    U032 dwFlags, dwOverlayOffset;
    PNVP_OVERLAY pOvInfo;

    DBGPRINT(DBGTRACE,"FlipOverlay()");

    // get a pointer to the appropriate VPE context (assume VPE 0)
    pVPEContext = &(pHwDevExt->avpContexts[0]);

    // if not initialized
    if (pVPEContext->nvDmaFifo == NULL)
    {
        return DXERR_GENERIC;
    }

    // NVPUpdateOverlay must be called first !
    if (!g_bFlip)
        return NO_ERROR;

    pOvInfo = &(pVPEContext->nvpOverlay);

    // VPP flags
    dwFlags = VPP_PRESCALE;

    if (pTargSurf->dwOverlayFlags & DDOVER_INTERLEAVED)
       dwFlags |= VPP_INTERLEAVED;
      else
       dwFlags |= VPP_BOB;   // non-interleaved

    if (pTargSurf->dwOverlayFlags & DDOVER_BOB)
       dwFlags |= VPP_BOB;

    if (pFlipOverlayInfo->dwFlags & DDFLIP_ODD)
        dwFlags |= VPP_ODD;
    else if (pFlipOverlayInfo->dwFlags & DDFLIP_EVEN)
        dwFlags |= VPP_EVEN;

    // initialize overlay buffer index if necessary
    if (pOvInfo->dwOverlayBufferIndex == NVP_UNDEF)
        pOvInfo->dwOverlayBufferIndex = 0;

    dwOverlayOffset = (U032)pTargSurf->fpLockPtr - pVPEContext->pFrameBuffer;

    // flip overlay here !
    if (NVPPrescaleAndFlip(pVPEContext, pOvInfo, dwOverlayOffset, pTargSurf->lPitch, 0, dwFlags))
        return DX_OK;

    return DXERR_GENERIC;
}

DWORD LockSurface(
    PVOID HwDeviceExtension,
    PNVP_DDLOCKININFO pLockInfoIn,
    PNVP_DDLOCKOUTINFO pLockInfoOut)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)HwDeviceExtension;
    PNVP_CONTEXT pVPEContext;
    U032 *pOvrFlag;

    DBGPRINT(DBGTRACE, "LockSurface()");

    // get a pointer to the appropriate VPE context (assume VPE 0)
    pVPEContext = &(pHwDevExt->avpContexts[0]);

    // this is a shared flag with DDraw...  set flag to indicate to DDraw that VPE kernel takes care of overlay flips !
    pOvrFlag = (U032 *) (pVPEContext->pOvrRing0FlipFlag);
    if (pOvrFlag)
        *pOvrFlag = 1;

    return DX_OK;
}

/*++

Routine Description:

    This function is called when "bob" is used and a VPORT VSYNC occurs
    that does not cause a flip to occur (e.g., bobbing while interleaved).
    When bobbing, the overlay must adjust itself on every VSYNC, so this
    function notifies it of the VSYNCs that it doesn't already know about
    (e.g., VSYNCs that trigger a flip to occur).

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    BobNextFieldInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/

DWORD BobNextField(
    PVOID HwDeviceExtension,
    PNVP_DDBOBNEXTFIELDINFO pBobNextFieldInfo,
    PVOID lpOutput)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    DBGPRINT(DBGTRACE,"BobNextField()");

    return DX_OK;
}

/*++

Routine Description:

    This function is called when the client wants to switch from bob to
    weave.  The overlay flags indicate which state to use.  This is only
    called for interleaved surfaces.

    Note that when this is called, the specified surface may not be
    displaying the overlay (due to a flip).  Instead of failing the call,
    change the bob/weave state for the overlay that would be used if the
    overlay was flipped again to the specified surface.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    SetStateInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/
DWORD SetState(
    PVOID HwDeviceExtension,
    PNVP_DDSETSTATEININFO  pSetStateInInfo,
    PNVP_DDSETSTATEOUTINFO pSetStateOutInfo)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    DBGPRINT(DBGERROR,"SetState() not implemented");

    return DX_OK;
}


/*++

Routine Description:

    This function is called when the client wants to skip the next field,
    usually to undo a 3:2 pulldown but also for decreasing the frame
    rate.  The driver should not lose the VBI lines if dwVBIHeight contains
    a valid value.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    SkipNextFieldInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/

DWORD SkipNextField(
    PVOID HwDeviceExtension,
    PNVP_DDSKIPNEXTFIELDINFO pSkipNextFieldInfo,
    PVOID lpOutput)
{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    DBGPRINT(DBGERROR,"SkipNextField() not implemented");

    return DX_OK;
}


/*++

Routine Description:

    This function returns the polarity of the current field being written
    to the specified video port.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    GetPolarityInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/

DWORD GetPolarity(
    PVOID HwDeviceExtension,
    PNVP_DDGETPOLARITYININFO  pGetPolarityInInfo,
    PNVP_DDGETPOLARITYOUTINFO pGetPolarityOutInfo)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION) HwDeviceExtension;
    PNVP_CONTEXT pVPEContext;

    DBGPRINT(DBGTRACE,"GetPolarity()");

    // get a handle to current VPE context (BUGBUG: assume index 0 for now !)
    pVPEContext = &(pHwDevExt->avpContexts[0]);

    // check video first !
    if (pVPEContext->nNumVidSurf)
    {
        // TRUE for Even, FALSE for Odd
        if (pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx].dwFieldType == NVP_FIELD_ODD)
            pGetPolarityOutInfo->bPolarity = FALSE;
        else
            pGetPolarityOutInfo->bPolarity = TRUE;
    }
    // vbi polarity
    else if (pVPEContext->nNumVBISurf)
    {
        if (pVPEContext->aVBISurfaceInfo[pVPEContext->nVBISurfIdx].dwFieldType == NVP_FIELD_ODD)
            pGetPolarityOutInfo->bPolarity = FALSE;
        else
            pGetPolarityOutInfo->bPolarity = TRUE;
    }
    else
        return DXERR_GENERIC;

    return DX_OK;
}

/*++

Routine Description:

    This function returns the current surface receiving data from the
    video port while autoflipping is taking place.  Only called when
    hardware autoflipping.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    GetCurrentAutoflipInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/
DWORD GetCurrentAutoflip(
    PVOID HwDeviceExtension,
    PNVP_DDGETCURRENTAUTOFLIPININFO  pGetCurrentAutoflipInInfo,
    PNVP_DDGETCURRENTAUTOFLIPOUTINFO pGetAutoFlipInfoOut)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)HwDeviceExtension;
    PNVP_CONTEXT pVPEContext;
    NvNotification *avpNotifiers;
    int nNotifyIdx;

    DBGPRINT(DBGTRACE,"GetCurrentAutoflip()");

    // get a handle to current VPE context (BUGBUG: assume index 0 for now !)
    pVPEContext = &(pHwDevExt->avpContexts[0]);
    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

    pGetAutoFlipInfoOut->dwSurfaceIndex = NVP_UNDEF;
    // check out video
    if (pVPEContext->nNumVidSurf)
    {
        nNotifyIdx = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx].dwFieldType);
        // check progress status of current surface
        if (avpNotifiers[nNotifyIdx].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)
            pGetAutoFlipInfoOut->dwSurfaceIndex = (ULONG)(pVPEContext->nVidSurfIdx);
        else
            // assume it is the next surface then !
            pGetAutoFlipInfoOut->dwSurfaceIndex = (ULONG)(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx].nNextSurfaceIdx);
    }

    pGetAutoFlipInfoOut->dwVBISurfaceIndex = NVP_UNDEF;
    // check out vbi
    if (pVPEContext->nNumVBISurf)
    {
        nNotifyIdx = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(pVPEContext->aVBISurfaceInfo[pVPEContext->nVBISurfIdx].dwFieldType);
        // check progress status of current surface
        if (avpNotifiers[nNotifyIdx].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)
            pGetAutoFlipInfoOut->dwVBISurfaceIndex = (ULONG)(pVPEContext->nVBISurfIdx);
        else
            // assume it is the next surface then !
            pGetAutoFlipInfoOut->dwVBISurfaceIndex = (ULONG)(pVPEContext->aVBISurfaceInfo[pVPEContext->nVBISurfIdx].nNextSurfaceIdx);
    }

    return DX_OK;
}


/*++

Routine Description:

    This function returns the current surface receiving data from the
    video port while autoflipping is taking place.  Only called when
    hardware autoflipping.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    GetPreviousAutoflipInfo - Function parameters.

Return Value:

    DX_OK if successful.  ERROR_INVALID_FUNCTION if the requested
    combination cannot be supported.

--*/
DWORD GetPreviousAutoflip(
    PVOID HwDeviceExtension,
    PNVP_DDGETPREVIOUSAUTOFLIPININFO  pGetAutoflipInInfo,
    PNVP_DDGETPREVIOUSAUTOFLIPOUTINFO pGetAutoFlipInfoOut)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)HwDeviceExtension;
    PNVP_CONTEXT pVPEContext;
    NvNotification *avpNotifiers;
    int nNotifyIdx;

    DBGPRINT(DBGTRACE,"GetPreviousAutoflip()");

    // get a handle to current VPE context (BUGBUG: assume index 0 for now !)
    pVPEContext = &(pHwDevExt->avpContexts[0]);
    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

    pGetAutoFlipInfoOut->dwSurfaceIndex = NVP_UNDEF;
    // check out video
    if (pVPEContext->nNumVidSurf)
    {
        nNotifyIdx = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx].dwFieldType);
        // check progress status of current surface
        if (avpNotifiers[nNotifyIdx].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)
            // if in progress, return index of previous surface
            pGetAutoFlipInfoOut->dwSurfaceIndex = (ULONG)((pVPEContext->nVidSurfIdx == 0) ? (pVPEContext->nNumVidSurf - 1): (pVPEContext->nVidSurfIdx - 1));
        else
            // assume this is the "previous"ly autoflipped surface then !
            pGetAutoFlipInfoOut->dwSurfaceIndex = (ULONG)(pVPEContext->nVidSurfIdx);
    }

    pGetAutoFlipInfoOut->dwVBISurfaceIndex = NVP_UNDEF;
    // check out vbi
    if (pVPEContext->nNumVBISurf)
    {
        nNotifyIdx = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(pVPEContext->aVBISurfaceInfo[pVPEContext->nVBISurfIdx].dwFieldType);
        // check progress status of current surface
        if (avpNotifiers[nNotifyIdx].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)
            // if in progress, return index of previous surface
            pGetAutoFlipInfoOut->dwVBISurfaceIndex = (ULONG)((pVPEContext->nVBISurfIdx == 0) ? (pVPEContext->nNumVBISurf - 1): (pVPEContext->nVBISurfIdx - 1));
        else
            // assume this is the "previous"ly autoflipped surface then !
            pGetAutoFlipInfoOut->dwVBISurfaceIndex = (ULONG)(pVPEContext->nVBISurfIdx);
    }

    return DX_OK;
}

#ifdef VPEFSMIRROR

// ***************************************************************************
// [XW:10/02/2000] - start FS Mirror Functions
// ***************************************************************************
//
// CreateVideoPort -> R3: vppEnableFsMirror + R0: NVPFsMirrorEnable + NVPFsRegSurface
//        |
//        V 
//     VideoField0/1Notify -> R0: NVPProcessCommand -> NVPScheduleOverlayandFlip 
//
//                      -> NVPFsFlipSync -> NVPFsMirror -> NVPFsFlip
//        |
//        V
// DestoryVideoPort32 -> R3: vppDisableFsMirror + R0: NVPFsMirrorDisable

// {

/*
==============================================================================
    
    NVPFsRegSurface

    Description:    register a full screen mirror surface... with
                    attributes

    Date:           September 25,2000
    BUGBUG:         To Fix feature.. Update surfaces.. needs to calc dsdy?
    
    Author:         Xun Wang

==============================================================================
*/
VP_STATUS NVPFsRegSurface(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    
    PNVP_CONTEXT        pVPEContext;
    PNVP_SURFACE_INFO   pSurface;
       
    DWORD               dwSurfId;
        
    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);    
    pSurface = &(pVPEContext->aFSMirrorSurfaceInfo[pCmdObj->dwSurfaceId]);

    dwSurfId = pCmdObj->dwSurfaceId;

    pSurface->pVidMem = pCmdObj->pSurfVidMem;
    pSurface->dwOffset = pCmdObj->dwSurfOffset;    
    pSurface->dwPitch = pCmdObj->dwSurfPitch;
    pSurface->dwHeight = pCmdObj->dwSurfHeight;
    pSurface->dwWidth = pCmdObj->dwSurfWidth;
    
    // point each to previous surface
    if (pCmdObj->dwSurfaceId) {
        pSurface->nPrevSurfaceIdx = dwSurfId - 1;
    }
       
    (pVPEContext->nNumFSSurf)++;

    // update the surface 0's information
    pVPEContext->aFSMirrorSurfaceInfo[0].nPrevSurfaceIdx = pVPEContext->nNumFSSurf - 1; 
        
    return NO_ERROR;
}

/*
==============================================================================
    
    NVPFsMirrorEnable

    Description:    Initialize everything we need for the fsmirror 

    Date:           September 25,2000
    
    Author:         Xun Wang

==============================================================================
*/
VP_STATUS NVPFsMirrorEnable(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    NV07C_ALLOCATION_PARAMETERS nv07cAllocParms;  // object creation params
    NV07A_ALLOCATION_PARAMETERS nv07aAllocParms;
    PNVP_CONTEXT pVPEContext;   // the actual vpe context
    Nv4ControlDma *pDmaChannel; // dma channel stuff
    U032 nvDmaCount;           
    //NVOS10_PARAMETERS evParam; // for event callbacks
    NvNotification *adacNotifiers;
    NvNotification *as2mNotifiers;

    NvNotification *asrfNotifiers;
    U032 bRecreateOverlay = FALSE;
    
    //U032 status;             // some local vars that I need

  
    // ******************************
    // Object Creation  
    // ******************************

    // Create LUT_DAC Object.. most important
    
    //#define NVP_VIDEO_LUT_DAC_SUBCHANNEL 3

    //#define NVP_SCALED_IMAGE_SUBCHANNEL  4

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

    pVPEContext->regOverlayMode2 = pCmdObj->regOverlayMode2;
    pVPEContext->dwOverlayFSHead = pCmdObj->dwOverlayFSHead;
    pVPEContext->dwDesktopState = pCmdObj->dwDesktopState;
    if (pCmdObj->dwOverlayFSOvlHead != pVPEContext->dwOverlayFSOvlHead) {
        bRecreateOverlay = TRUE;
    }
    pVPEContext->dwOverlayFSOvlHead = pCmdObj->dwOverlayFSOvlHead;    
    
    if (bRecreateOverlay) {
    
        // stop overlay?
        // free old overlay
        rmNVPFree(pCmdObj->hClient, NVP_VPE_CHANNEL, NVP_OVERLAY_OBJECT);
        
        // make new overlay        
        nv07aAllocParms.logicalHeadId = pVPEContext->dwOverlayFSOvlHead;
        if (rmNVPAlloc(pCmdObj->hClient,
                  NVP_VPE_CHANNEL,
                  NVP_OVERLAY_OBJECT,
                  NV10_VIDEO_OVERLAY,
                  &nv07aAllocParms) != 0)
        {
            DBG_PRINT_STRING_VALUE(DBGERROR, "cannot allocate Overlay Object ", NVP_OVERLAY_OBJECT);
            return ERROR_INVALID_PARAMETER;
        }        

        // initialize new overlay

    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;


    NVP_DMAPUSH_CHECK_FREE_COUNT(8);

    // set overlay contexts
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OBJECT, NVP_OVERLAY_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_CONTEXT_DMA_NOTIFIES, NVP_OVERLAY_NOTIFIER);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_CONTEXT_DMA_OVERLAY(0), NVP_OVERLAY_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_CONTEXT_DMA_OVERLAY(1), NVP_OVERLAY_BUFFER_CONTEXT);

    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;

    }

    // head selection.  pVPEContext should be pointing to head2
    nv07cAllocParms.logicalHeadId = pVPEContext->dwOverlayFSHead;    

    // use lowest one possible for complete compatibility
    if (rmNVPAlloc(pVPEContext->hClient,
                   NVP_VPE_CHANNEL,
                   NVP_VIDEO_LUT_DAC_OBJ_ID,
                   NV15_VIDEO_LUT_CURSOR_DAC,
                   &nv07cAllocParms) != 0)
    {
        DBG_PRINT_STRING_VALUE(DBGERROR, "cannot allocate video_lut_dac object ", NVP_VIDEO_LUT_DAC_OBJ_ID);
        return ERROR_INVALID_PARAMETER;
    }

    if (rmNVPAllocObject(pVPEContext->hClient,
                   NVP_VPE_CHANNEL,
                   NVP_SCALED_IMAGE_OBJ_ID, 
                   NV10_SCALED_IMAGE_FROM_MEMORY) != 0)
    {
        DBG_PRINT_STRING_VALUE(DBGERROR, "cannot allocate scaled image object ", NVP_SCALED_IMAGE_OBJ_ID);
        return ERROR_INVALID_PARAMETER;
    }

    // Allocate Scaled-image-buffer-context context dmas
    if (rmNVPAllocContextDma(
                        pVPEContext->hClient,
                        NVP_SCALED_IMAGE_BUFFER_CONTEXT,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0, // selector
                        (U032)pVPEContext->pFrameBuffer,
                        pVPEContext->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate notifiers for S2M transfer Object");
        return ERROR_INVALID_PARAMETER;
    }

    // Allocate video_lut_dac_video_buffer0_context context dmas
    if (rmNVPAllocContextDma(
                        pVPEContext->hClient,
                        NVP_VIDEO_LUT_DAC_FIELD0_BUFFER_CONTEXT,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0, // selector
                        (U032)pVPEContext->pFrameBuffer,
                        pVPEContext->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate notifiers for S2M transfer Object");
        return ERROR_INVALID_PARAMETER;
    }
    
    // Allocate video_lut_dac_video_buffer1_context context dmas
    if (rmNVPAllocContextDma(
                        pVPEContext->hClient,
                        NVP_VIDEO_LUT_DAC_FIELD1_BUFFER_CONTEXT,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0, // selector
                        (U032)pVPEContext->pFrameBuffer,
                        pVPEContext->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate notifiers for S2M transfer Object");
        return ERROR_INVALID_PARAMETER;
    }

    // ******************************
    // Notifiers Set, Set Events to VPEContext..
    // ******************************

    //#define NVP_CLASS089_MAX_NOTIFIERS          1
    //#define NVP_CLASS07C_MAX_NOTIFIERS          9

    // Allocate s2m context notifer ID
    if (rmNVPAllocContextDma(
                        pVPEContext->hClient,
                        NVP_SCALED_IMAGE_NOTIFIERS_CONTEXT_ID,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0, // selector
                        (U032)pVPEContext->as2mNotifiers,
                        (sizeof(NvNotification)*NVP_CLASS089_MAX_NOTIFIERS - 1) ) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate notifiers for S2M transfer Object");
        return ERROR_INVALID_PARAMETER;
    }

    // Allocate video_lut_cursor_dac context notifer ID
    if (rmNVPAllocContextDma(
                        pVPEContext->hClient,
                        NVP_VIDEO_LUT_DAC_NOTIFIERS_CONTEXT_ID,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0, // selector
                        (U032)pVPEContext->adacNotifiers,
                        (sizeof(NvNotification)*NVP_CLASS07C_MAX_NOTIFIERS - 1) ) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate notifiers for S2M transfer Object");
        return ERROR_INVALID_PARAMETER;
    }

  
    
    // reset notifiers, scaled image from memory and video_lut_cursor_dac

    adacNotifiers = (NvNotification*)pVPEContext->adacNotifiers;
    as2mNotifiers = (NvNotification*)pVPEContext->as2mNotifiers;
    

    adacNotifiers[NV07C_NOTIFIERS_NOTIFY].status = 0;
    adacNotifiers[NV07C_NOTIFIERS_SET_IMAGE(0)].status = 0;
    adacNotifiers[NV07C_NOTIFIERS_SET_IMAGE(1)].status = 0;
    as2mNotifiers[NV089_NOTIFIERS_NOTIFY].status = 0;

    
    // Context Surface Creation and Setup

    asrfNotifiers = (NvNotification*)(&pVPEContext->asrfNotifiers[0]);
    asrfNotifiers[NV042_NOTIFIERS_NOTIFY].status = 0;

    if (rmNVPAllocObject(pVPEContext->hClient,
                   NVP_VPE_CHANNEL,
                   NVP_CONTEXT_SURFACE_2D_OBJECT, 
                   NV04_CONTEXT_SURFACES_2D) != 0)
    {
        DBGPRINT (DBGERROR, "cannot allocate context surface 2D object");
        return ERROR_INVALID_PARAMETER;
    }

    // context_surface_2d_notifiers_ID
    if (rmNVPAllocContextDma(
                        pVPEContext->hClient,
                        NVP_CONTEXT_SURFACE_2D_NOTIFIERS_CONTEXT_ID,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0, // selector
                        (U032)asrfNotifiers,
                        (sizeof(NvNotification)*NVP_CLASS089_MAX_NOTIFIERS - 1) ) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate notifiers for surf 2 d transfer Object");
        return ERROR_INVALID_PARAMETER;
    } 

    if (rmNVPAllocContextDma(
                        pVPEContext->hClient,
                        NVP_CONTEXT_SURFACE_2D_BUFFER_CONTEXT,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0, // selector
                        (U032)pVPEContext->pFrameBuffer,
                        pVPEContext->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "Cannot Allocate Context for Surface 2D object in memory");
        return ERROR_INVALID_PARAMETER;
    }


    // ******************************
    // Events stuff and Callbacks
    // ******************************
/*  
    // Create 2 callbacks for video_lut_cursor_dac
    evParam.hRoot = pVPEContext->hClient;
    evParam.hObjectParent = NVP_VIDEO_LUT_DAC_OBJ_ID;
    evParam.hObjectNew = NVP_VIDEO_LUT_DAC_EVENT0_ID;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV07C_NOTIFIERS_SET_IMAGE(0);
    NvU64_VALUE(evParam.hEvent) = (ULONG) NVPFsCRTCField0Notify;
    status = rmNVPAllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "cannot create callback for crtc 0");
        return ERROR_INVALID_PARAMETER;
    }

    evParam.hRoot = pVPEContext->hClient;
    evParam.hObjectParent = NVP_VIDEO_LUT_DAC_OBJ_ID;
    evParam.hObjectNew = NVP_VIDEO_LUT_DAC_EVENT1_ID;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV07C_NOTIFIERS_SET_IMAGE(1);
    NvU64_VALUE(evParam.hEvent) = (ULONG) NVPFsCRTCField1Notify;
    status = rmNVPAllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "cannot create callback for crtc 1");
        return ERROR_INVALID_PARAMETER;
    }
  */      
    // ******************************
    // DMA Channel - Object Setup
    // ******************************
    
    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel); // get dma chan    
    nvDmaCount = pVPEContext->nvDmaCount;
    
    NVP_DMAPUSH_CHECK_FREE_COUNT(30); // Adjusted - check    

    // Setup NV04_VIDEO_LUT_CURSOR_DAC object
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07C_SET_OBJECT, NVP_VIDEO_LUT_DAC_OBJ_ID);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07C_SET_CONTEXT_DMA_NOTIFIES, NVP_VIDEO_LUT_DAC_NOTIFIERS_CONTEXT_ID);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07C_SET_CONTEXT_DMA_IMAGE(0), NVP_VIDEO_LUT_DAC_FIELD0_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07C_SET_CONTEXT_DMA_IMAGE(1), NVP_VIDEO_LUT_DAC_FIELD0_BUFFER_CONTEXT);

    // Setup NV04_CONTEXT_SURFACES_2D object
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV042_SET_OBJECT, NVP_CONTEXT_SURFACE_2D_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV042_SET_CONTEXT_DMA_NOTIFIES, NVP_CONTEXT_SURFACE_2D_NOTIFIERS_CONTEXT_ID);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV042_SET_CONTEXT_DMA_IMAGE_SOURCE, NVP_CONTEXT_SURFACE_2D_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV042_SET_CONTEXT_DMA_IMAGE_DESTIN, NVP_CONTEXT_SURFACE_2D_BUFFER_CONTEXT);
    
    // Setup NV05_SCALED_IMAGE_FROM_MEMORY object 
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_SET_OBJECT, NVP_SCALED_IMAGE_OBJ_ID);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_SET_CONTEXT_DMA_NOTIFIES, NVP_SCALED_IMAGE_NOTIFIERS_CONTEXT_ID);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_SET_CONTEXT_DMA_IMAGE, NVP_SCALED_IMAGE_BUFFER_CONTEXT);        
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_SET_COLOR_FORMAT, NV089_SET_COLOR_FORMAT_LE_YB8CR8YA8CB8);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_SET_CONTEXT_SURFACE, NVP_CONTEXT_SURFACE_2D_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_SET_OPERATION,NV089_SET_OPERATION_SRCCOPY_AND);    
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_SET_COLOR_CONVERSION,NV089_SET_COLOR_CONVERSION_DITHER);

    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;

    pVPEContext->nNumFSSurf     = 0;   // will be incremented later
    pVPEContext->nFSFlipSurfIdx = 1;   // should be 1
    pVPEContext->nFSMirrorIdx   = 0;   // should always be one behind
    pVPEContext->nFSFlipIdx     = 0;   // should be 0
    
    // Will only turn on if everything has passed..
    pVPEContext->dwFsMirrorOn = TRUE;  // turn this off at disable time

	// [XW] BUGBUG: 
	// Ideally we should return an Error, so we could deallocate everything Through Ring 3.
	// Right now, we'll just use the dwFsMirrorOn flag to make sure everything is *off* if fs mirror
	// is not turned on.
	
    return NO_ERROR;    
}

/*
==============================================================================
    
    NVPFsMirrorDisable

    Description:    UnInitialize everything we need for the fsmirror 

    Date:           October 2,2000
    
    Author:         Xun Wang

==============================================================================
*/
VP_STATUS NVPFsMirrorDisable(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    // free objects, events etc.
    PNVP_CONTEXT pVPEContext;
    
    DBGPRINT(DBGTRACE, "NVPFsMirrorDisable()");

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

    pVPEContext->dwFsMirrorOn = FALSE;  // turn off at disable time
        
    // FCF: wait for notifiers to stop objects still running?
    rmNVPFree(pVPEContext->hClient,  pCmdObj->dwDeviceId, NVP_SCALED_IMAGE_BUFFER_CONTEXT);
    rmNVPFree(pVPEContext->hClient,  pCmdObj->dwDeviceId, NVP_VIDEO_LUT_DAC_FIELD0_BUFFER_CONTEXT);
    rmNVPFree(pVPEContext->hClient,  pCmdObj->dwDeviceId, NVP_VIDEO_LUT_DAC_FIELD1_BUFFER_CONTEXT);
    rmNVPFree(pVPEContext->hClient,  pCmdObj->dwDeviceId, NVP_SCALED_IMAGE_NOTIFIERS_CONTEXT_ID);
    rmNVPFree(pVPEContext->hClient,  pCmdObj->dwDeviceId, NVP_VIDEO_LUT_DAC_NOTIFIERS_CONTEXT_ID);
    rmNVPFree(pVPEContext->hClient,  pCmdObj->dwDeviceId, NVP_CONTEXT_SURFACE_2D_NOTIFIERS_CONTEXT_ID);
    rmNVPFree(pVPEContext->hClient,  pCmdObj->dwDeviceId, NVP_CONTEXT_SURFACE_2D_BUFFER_CONTEXT);
    
    rmNVPFree(pVPEContext->hClient, NVP_VPE_CHANNEL, NVP_CONTEXT_SURFACE_2D_OBJECT);
    rmNVPFree(pVPEContext->hClient, NVP_VPE_CHANNEL, NVP_VIDEO_LUT_DAC_OBJ_ID);
    rmNVPFree(pVPEContext->hClient, NVP_VPE_CHANNEL, NVP_SCALED_IMAGE_OBJ_ID);
    
    return NO_ERROR;

}

/*
==============================================================================
    
    NVPFsFlip

    Description:    Flips to next FS Surface

    Date:           October 2,2000
    
    Author:         Xun Wang

==============================================================================
*/
VP_STATUS NVPFsFlip(
    PNVP_CONTEXT pVPEContext,
    int nFieldId)
{
    int nFlipIdx;
    NVP_SURFACE_INFO* pSurface;
    DWORD dwOffset;
    DWORD dwFormat;
    Nv4ControlDma *pDmaChannel; // dma channel stuff
    U032 nvDmaCount;           
    NvNotification *adacNotifiers;
        
    nFlipIdx = pVPEContext->nFSFlipSurfIdx;
    nFlipIdx = pVPEContext->aFSMirrorSurfaceInfo[nFlipIdx].nPrevSurfaceIdx;       
    pSurface = &(pVPEContext->aFSMirrorSurfaceInfo[nFlipIdx]);
        
    
    dwOffset = pSurface->dwOffset;
    dwFormat = pSurface->dwPitch |(NV07C_SET_IMAGE_FORMAT_NOTIFY_WRITE_THEN_AWAKEN << 31)| (NV07C_SET_IMAGE_FORMAT_WHEN_NOT_WITHIN_BUFFER << 20);

    // we are ready to start flipping
    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel); // get dma chan    
    nvDmaCount = pVPEContext->nvDmaCount;

    // update the notifiers
    adacNotifiers = (NvNotification*)pVPEContext->adacNotifiers;
    if (adacNotifiers[NV07C_NOTIFIERS_SET_IMAGE(nFieldId)].status == NV07C_NOTIFICATION_STATUS_IN_PROGRESS) {
        return ERROR_INVALID_PARAMETER;
    }
    adacNotifiers[NV07C_NOTIFIERS_SET_IMAGE(nFieldId)].status = NV07C_NOTIFICATION_STATUS_IN_PROGRESS;

    NVP_DMAPUSH_CHECK_FREE_COUNT(6);

    // Setup NV04_VIDEO_LUT_CURSOR_DAC object
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07C_SET_OBJECT, NVP_VIDEO_LUT_DAC_OBJ_ID);
    // context dmas should be set up .. so we just need a offset, and a format to start it going
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07C_SET_IMAGE_OFFSET(nFieldId), dwOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07C_SET_IMAGE_FORMAT(nFieldId), dwFormat);

    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;

    return NO_ERROR;
}


/*
==============================================================================
    
    NVPFsMirror

    Description:    Full Screen Mirror Copy Function from buffer into next surface

    Date:           October 2,2000
    
    Author:         Xun Wang

==============================================================================
*/

VP_STATUS NVPFsMirror(
    PNVP_CONTEXT  pVPEContext,
    PNVP_OVERLAY  pOverlayInfo,
    DWORD         dwOffset,        // source surface offset
    DWORD         dwPitch,         // source pitch
    DWORD         dwWidth,         // source width
    DWORD         dwHeight,        // source height
    DWORD         dwIndex,         // overlay buffer index
    DWORD         dwFlags)         // VPP flags)
{
    // free objects, events etc.

    DWORD dwPointIn, dwSizeIn, dwPointOut, dwSizeOut;
    DWORD dwDeltaX, dwDeltaY;
    DWORD dwInFormat;
    DWORD dwClipPoint, dwClipSize;
    DWORD dwSurfId;
    DWORD dwPitch2d;
    DWORD dwSurfaceColorFormat;
    Nv4ControlDma *pDmaChannel;
    U032 nvDmaCount;

    NVP_SURFACE_INFO* pSurface;
    NvNotification* as2mNotifiers;

	DBGPRINT(DBGTRACE, "NVPFsMirror");
   
    dwSurfId = pVPEContext->nFSFlipSurfIdx;
    pSurface = &(pVPEContext->aFSMirrorSurfaceInfo[dwSurfId]);
    
    dwPointIn = (dwOffset & NV_OVERLAY_BYTE_ALIGNMENT_PAD) << 3;  // << 3 insteed of << 4 because YUYV is 2 bytes wide
    
    if (IS_FIELD(dwFlags) && (dwFlags & VPP_BOB) && (dwFlags & VPP_INTERLEAVED) ) {
        // The problem is scaled_image does not accept -0.5 as a value
        // for Point In, therefore we must adjust dwPointIn to
        // 0 for EVEN fields 
        // 1 for ODD Fields
        // Therefore we must also adjust the SizeIn as well, and this
        // must be done before Delta Y is calculated
        if (dwFlags & VPP_EVEN) 
        {            
            dwPointIn += 0x00040000;
        } else 
        {
            dwPointIn += 0x00140000;
        }
        dwHeight = dwHeight - 1;    
    }
    
    dwSizeIn = asmMergeCoords((dwWidth & ~1), (dwHeight & ~1));        
    
    dwOffset &= ~NV_OVERLAY_BYTE_ALIGNMENT_PAD;

    dwPointOut = asmMergeCoords(0,0); // outpoint on fulscreen mirror should be zero, for top of screen.    
    dwClipPoint = asmMergeCoords(0,0); // clippoint should be zerol;       

    
    // This stuff should be done elsewhere, and saved in the context,
    // In Efficient! [XW]
    // aspect ratio.. should be calculated somewhere... in init,
    // and attached to some kind of info struct
    // calculate the scaling ration in/out.
    

    if (dwFlags & VPP_INTERLEAVED) 
    {            
        if (dwFlags & VPP_EVEN) 
        { dwOffset += dwPitch; }
        dwPitch  <<= 1;
        dwHeight >>= 1;
        dwSizeIn = asmMergeCoords((dwWidth & ~1), (dwHeight & ~1));        
     
    }

    dwInFormat = (ULONG)((ULONG) dwPitch | (ULONG)NV089_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16| (ULONG) NV089_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24);

    if (pSurface->dwWidth <= 1)
        dwDeltaX = 0x100000;
    else
    {
        dwDeltaX =  (dwWidth - 1) << 16;
        dwDeltaX /= (pSurface->dwWidth - 1);
        dwDeltaX <<=4 ;
    }
        
    if (pSurface->dwHeight <= 1)
        dwDeltaY = 0x100000;
    else
    {
        dwDeltaY =  (dwHeight - 1) << 16;
        dwDeltaY /= (pSurface->dwHeight - 1);
        dwDeltaY <<=4 ;
    }
        
    
    dwSizeOut = asmMergeCoords(pSurface->dwWidth, pSurface->dwHeight); // size of fullscreen grab out of surf info in struct
    dwClipSize = dwSizeOut;   
            
    dwSurfaceColorFormat = NV042_SET_COLOR_FORMAT_LE_A8R8G8B8;
    dwPitch2d = ( (pSurface->dwPitch << 16) | dwPitch );

    // program overlay to flip
    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;

    // update the notifiers
    as2mNotifiers = (NvNotification*)pVPEContext->as2mNotifiers;
//    if (as2mNotifiers[NV089_NOTIFIERS_NOTIFY].status == NV089_NOTIFICATION_STATUS_IN_PROGRESS) {
//        return ERROR_INVALID_PARAMETER;
//    } // FCF: Why is the notifier never ready?

    as2mNotifiers[NV089_NOTIFIERS_NOTIFY].status = NV089_NOTIFICATION_STATUS_IN_PROGRESS;

    NVP_DMAPUSH_CHECK_FREE_COUNT(32); // Adjusted - check

    // Setup NV04_CONTEXT_SURFACES_2D object    
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV042_SET_OBJECT, NVP_CONTEXT_SURFACE_2D_OBJECT);    
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV042_SET_COLOR_FORMAT, dwSurfaceColorFormat);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV042_SET_PITCH, dwPitch2d);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV042_SET_OFFSET_SOURCE, dwOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV042_SET_OFFSET_DESTIN, pSurface->dwOffset);
    
    // Setup NV05_SCALED_IMAGE_FROM_MEMORY object
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_SET_OBJECT, NVP_SCALED_IMAGE_OBJ_ID);        
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_CLIP_POINT,dwClipPoint);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_CLIP_SIZE,dwClipSize);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_IMAGE_OUT_POINT,dwPointOut);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_IMAGE_OUT_SIZE,dwSizeOut);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_DS_DX,dwDeltaX);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_DT_DY,dwDeltaY);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_IMAGE_IN_SIZE,dwSizeIn);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_IMAGE_IN_FORMAT,dwInFormat);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_IMAGE_IN_OFFSET,dwOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV089_IMAGE_IN,dwPointIn);

    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;

    return NO_ERROR;
}

// }
// ***************************************************************************
// [XW:10/02/2000] - end FS Mirror Functions
// ***************************************************************************

// ***************************************************************************
// [XW:10/02/2000] - start FS Mirror callbacks section
// ***************************************************************************
// {

#ifdef _WIN32_WINNT



/*
==============================================================================
    
    NVPFsCRTCField0Notify

    Description:    callback function. for crtc buffer 0 

    Date:           October 2, 2000
    
    Author:         Xun Wang

==============================================================================
*/
void NVPFsCRTCField0Notify(PVOID pHwDeviceExtension)
{
    PNVP_CONTEXT pVPEContext;
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;    
    pVPEContext = &(pHwDevExt->avpContexts[0]);

    NVPFsFlip(pVPEContext,1);
}


/*
==============================================================================
    
    NVPFsCRTCField1Notify

    Description:    callback function. for crtc buffer 1 

    Date:           October 2, 2000
    
    Author:         Xun Wang

==============================================================================
*/
void NVPFsCRTCField1Notify(PVOID pHwDeviceExtension)
{
    PNVP_CONTEXT pVPEContext;
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;
    pVPEContext = &(pHwDevExt->avpContexts[0]);
    
    NVPFsFlip(pVPEContext,0);
}

#else


/*
==============================================================================
    
    NVPFsCRTCField0Notify

    Description:    callback function. for crtc buffer 0 

    Date:           October 2, 2000
    
    Author:         Xun Wang

==============================================================================
*/
void NVPFsCRTCField0Notify()
{
    PNVP_CONTEXT pVPEContext;
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)&nvpeDevExtension;    
    pVPEContext = &(pHwDevExt->avpContexts[0]);

    NVPFsFlip(pVPEContext,1);
}


/*
==============================================================================
    
    NVPFsCRTCField1Notify

    Description:    callback function. for crtc buffer 1 

    Date:           October 2, 2000
    
    Author:         Xun Wang

==============================================================================
*/
void NVPFsCRTCField1Notify()
{
    PNVP_CONTEXT pVPEContext;
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)&nvpeDevExtension;
    pVPEContext = &(pHwDevExt->avpContexts[0]);
    
    NVPFsFlip(pVPEContext,0);
}



#endif //_WIN32_WINNT

//#endif // VPE_FSMIRROR

// }
// ***************************************************************************
// [XW:10/02/2000] - end FS Mirror callbacks section
// ***************************************************************************

#endif


/*
==============================================================================
    
    NVP3Initialize

    Description:    Initializes the NV video port. Creates channels and DMA
                    contexts. Create Video and VBI objects.
                    NV3 version uses PIO channel

    Date:           Feb 03, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVP3Initialize(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
#ifdef NVP_NV3
    PNVP_CONTEXT pVPEContext;
    NvNotification *avpNotifiers;
    NVOS10_PARAMETERS evParam;
    int nFreeCount;
    U032 status;
    NVOS09_PARAMETERS ntfParam;
    Nv03ChannelPio *pNVPChannel = (Nv03ChannelPio *) (pCmdObj->pChannel);

//////////// HACK
    //cur_debuglevel = 1;
    g_bFlip = FALSE;
///////////

    DBGPRINT(DBGTRACE,"NVP3Initialize()");

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);
    // initialize context
    pVPEContext->pNVPChannel = (ULONG) pNVPChannel; // keep a copy of the pio channel pointer
    pVPEContext->nvDmaFifo = NULL;
    pVPEContext->dwDeviceId = pCmdObj->dwDeviceId;
    pVPEContext->hClient = pCmdObj->hClient;
      pVPEContext->pFrameBuffer = pCmdObj->pFrameBuffer;
      pVPEContext->dwFBLen = pCmdObj->dwFBLen;
    pVPEContext->pOverlayChannel = pCmdObj->pOvrChannel;
    pVPEContext->dwOvrSubChannel = pCmdObj->dwOvrSubChannel;
    pVPEContext->dwOverlayObjectID = pCmdObj->dwOverlayObjectID;
    pVPEContext->dwScalerObjectID = pCmdObj->dwScalerObjectID;
    pVPEContext->dwDVDSubPicObjectID = pCmdObj->dwDVDObjectID;
    pVPEContext->dwDVDObjectCtxDma = pCmdObj->dwDVDObjectCtxDma;
    pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat = pCmdObj->pNvPioFlipOverlayNotifierFlat;
    pVPEContext->nNumVidSurf = 0;
    pVPEContext->nNumVBISurf = 0;
    pVPEContext->dwVPState = NVP_STATE_STOPPED;
    pVPEContext->dwVPConFlags = pCmdObj->dwVPTypeFlags;
    pVPEContext->dwVPFlags = 0; //pCmdObj->dwVPInfoFlags;
    pVPEContext->nCurTransferIdx = NVP_EOQ;
    pVPEContext->nLastTransferIdx = NVP_EOQ;
    pVPEContext->nRecentTransferIdx = NVP_EOQ;
    pVPEContext->nNumTransfer = 0;
    pVPEContext->bxfContextCreated = FALSE;
    pVPEContext->bxfContextFlushed = FALSE;
    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

    // alloc external decoder object
    if (RmAllocObject(
                pCmdObj->hClient,
                NVP_VPE_CHANNEL,
                NVP_EXTERNAL_DECODER_OBJECT,
                NV03_EXTERNAL_VIDEO_DECODER) != 0)
    {
        DBG_PRINT_STRING_VALUE(DBGERROR, "Cannot allocate External Decoder Object ", NVP_EXTERNAL_DECODER_OBJECT);
        return FALSE;
    }

    // allocate transfer object (used for vbi and video capture)
    if (RmAllocObject(
                pCmdObj->hClient,
                NVP_VPE_CHANNEL,
                NVP_M2M_OBJECT,
                NV03_MEMORY_TO_MEMORY_FORMAT) != 0)
    {
        DBG_PRINT_STRING_VALUE(DBGERROR, "Cannot allocate Mem to mem transfer Object ", NVP_M2M_OBJECT);
        return FALSE;
    }

    // allocate the transfer FROM context
    if (RmAllocContextDma(
                        pCmdObj->hClient,
                        NVP_M2M_FROM_CONTEXT,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0, // selector
                        (U032)pCmdObj->pFrameBuffer,
                        pCmdObj->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "Cannot allocate notifiers for M2M transfer Object");
        return FALSE;
    }

    // allocate a notify context for transfer object
    if (RmAllocContextDma(
                        pCmdObj->hClient,
                        NVP_M2M_NOTIFIER,
                        NV01_CONTEXT_DMA_FROM_MEMORY,
                        0,  //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0,  // selector
                        (U032)(pVPEContext->axfNotifiers),
                        (sizeof( NvNotification)*NVP_CLASS039_MAX_NOTIFIERS  - 1)) != 0)
    {
        DBGPRINT(DBGERROR, "Cannot allocate notifiers for M2M transfer Object");
        return FALSE;
    }
    
    // allocate a notify context for external decoder
    if (RmAllocContextDma(
                    pCmdObj->hClient,
                    NVP_EXTERNAL_DECODER_NOTIFIER,
                    NV01_CONTEXT_DMA_FROM_MEMORY,
                    0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                    0, // selector
                    (U032)(pVPEContext->avpNotifiers),
                    (sizeof(NvNotification)*NVP_CLASS04D_MAX_NOTIFIERS - 1)) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
    {
        DBGPRINT(DBGERROR, "Cannot allocate notifiers for external decoder object");
        return FALSE;
    }

    // context DMA for video field0
    if (RmAllocContextDma(
                pCmdObj->hClient,
                NVP_VIDEO_FIELD0_BUFFER_CONTEXT,
                NV01_CONTEXT_DMA_FROM_MEMORY,
                0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                0, // selector
                (U032) (pCmdObj->pFrameBuffer),
                pCmdObj->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "Cannot allocate context DMA for video field0");
        return FALSE;
    }

    // context DMA for video field1 
    if (RmAllocContextDma(
                pCmdObj->hClient,
                NVP_VIDEO_FIELD1_BUFFER_CONTEXT,
                NV01_CONTEXT_DMA_FROM_MEMORY,
                0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                0, // selector
                (U032) (pCmdObj->pFrameBuffer),
                pCmdObj->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "Cannot allocate context DMA for video field1");
        return FALSE;
    }
    
    // context DMA for video field0
    if (RmAllocContextDma(
                pCmdObj->hClient,
                NVP_VBI_FIELD0_BUFFER_CONTEXT,
                NV01_CONTEXT_DMA_FROM_MEMORY, //NV01_CONTEXT_DMA,
                0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                0, // selector
                (U032) (pCmdObj->pFrameBuffer),
                pCmdObj->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "Cannot allocate context DMA for VBI field0");
        return FALSE;
    }

    // context DMA for video field1
    if (RmAllocContextDma(
                pCmdObj->hClient,
                NVP_VBI_FIELD1_BUFFER_CONTEXT,
                NV01_CONTEXT_DMA_FROM_MEMORY, //NV01_CONTEXT_DMA,
                0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                0, // selector
                (U032) (pCmdObj->pFrameBuffer),
                pCmdObj->dwFBLen - 1) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
    {
        DBGPRINT(DBGERROR, "Cannot allocate context DMA for VBI field1");
        return FALSE;
    }

    //CHECK_FREE_COUNT(pNVPChannel,10*5);
    nFreeCount = NvGetFreeCount(pNVPChannel, 0);
    while (nFreeCount < 13*4)
    {
        //RmInterrupt(&ntfParam);
        nFreeCount = NvGetFreeCount(pNVPChannel, 0);
    }           
    pVPEContext->nFreeCount = nFreeCount - (13*4);

    avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

    // transfer object setup
    pNVPChannel->subchannel[5].SetObject  = NVP_M2M_OBJECT;
    pNVPChannel->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaNotifies = NVP_M2M_NOTIFIER;
    pNVPChannel->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferIn = NVP_M2M_FROM_CONTEXT;
    
    // make sure decoder object is in the channel....
    pNVPChannel->subchannel[0].SetObject = NVP_EXTERNAL_DECODER_OBJECT;
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaNotifies = NVP_EXTERNAL_DECODER_NOTIFIER;

    // plug in our buffer contexts
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaImage[0] = NVP_VIDEO_FIELD0_BUFFER_CONTEXT;
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaImage[1] = NVP_VIDEO_FIELD1_BUFFER_CONTEXT;
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaVbi[0]   = NVP_VBI_FIELD0_BUFFER_CONTEXT;
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetContextDmaVbi[1]   = NVP_VBI_FIELD1_BUFFER_CONTEXT;
    
    // NB: we don't want to launch any captures at this point

    // setup the transfer callback
    evParam.hRoot = pCmdObj->hClient;
    evParam.hObjectParent = NVP_M2M_OBJECT;
    evParam.hObjectNew = NVP_M2M_EVENT;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV039_NOTIFIERS_BUFFER_NOTIFY;
    NvU64_VALUE(evParam.hEvent) = (NvU64_VALUE_T) NVPTransferNotify;
    status = Nv01AllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "Cannot create callback for m2m transfer 0");
        return FALSE;
    }

    // setup fields callbacks
    evParam.hRoot = pCmdObj->hClient;
    evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
    evParam.hObjectNew = NVP_VIDEO_FIELD0_BUFFER_EVENT;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0);
    NvU64_VALUE(evParam.hEvent) = (NvU64_VALUE_T) VideoField0Notify;

    // video field0
    status = Nv01AllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "Cannot create callback for video field 0");
        return FALSE;
    }

    // video field1
    evParam.hRoot = pCmdObj->hClient;
    evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
    evParam.hObjectNew = NVP_VIDEO_FIELD1_BUFFER_EVENT;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1);
    NvU64_VALUE(evParam.hEvent) = (NvU64_VALUE_T) VideoField1Notify;

    status = Nv01AllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "Cannot create callback for video field 1");
        return FALSE;
    }

    // vbi field0
    evParam.hRoot = pCmdObj->hClient;
    evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
    evParam.hObjectNew = NVP_VBI_FIELD0_BUFFER_EVENT;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0);
    NvU64_VALUE(evParam.hEvent) = (NvU64_VALUE_T) VBIField0Notify;

    status = Nv01AllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "Cannot create callback for VBI field 0");
        return FALSE;
    }
            
    // vbi field1
    evParam.hRoot = pCmdObj->hClient;
    evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
    evParam.hObjectNew = NVP_VBI_FIELD1_BUFFER_EVENT;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1);
    NvU64_VALUE(evParam.hEvent) = (NvU64_VALUE_T) VBIField1Notify;

    status = Nv01AllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "Cannot create callback for VBI field 1");
        return FALSE;
    }
#endif // NVP_NV3
    return NO_ERROR;
}


/*
==============================================================================
    
    NVPInitialize

    Description:    Initializes the NV video port. Creates channels and DMA
                    contexts. Create Video and VBI objects.
                    DMA push buffer version (NV ver >= 0x04)

    Date:           Feb 03, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPInitialize(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    PNVP_CONTEXT pVPEContext;
    NvNotification *avpNotifiers;
    NvNotification *aovNotifiers;
    U032 dwOvObj;
    NVOS10_PARAMETERS evParam;
    Nv4ControlDma *pDmaChannel;
    U032 nvDmaCount;
    U032 i, status;

    // NV3 specific code
    if (NVP_GET_CHIP_VER(pCmdObj->dwChipId) <= NVP_CHIP_VER_03)
        return NVP3Initialize(pHwDevExt, pCmdObj);

//////////// HACK
    //cur_debuglevel = 1;
    g_bFlip = FALSE;
///////////

    DBGPRINT(DBGTRACE, "NVPInitialize()");

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

    // already initialized... should rather uninitialize and re-initialize with new data !!!
    //transfer check to ring3 NVPInitialize
#if 0
	if (pVPEContext->nvDmaFifo != NULL)	
    {
          DBGPRINT(DBGERROR, "already initialized !");
          return NO_ERROR;
    }
#endif

    // FS Mirror variable initialization
    pVPEContext->MultiMon = pCmdObj->MultiMon;
    pVPEContext->regOverlayMode = pCmdObj->regOverlayMode;
    pVPEContext->regOverlayMode2 = pCmdObj->regOverlayMode2;
    pVPEContext->regOverlayMode3 = pCmdObj->regOverlayMode3;
    pVPEContext->dwFsMirrorOn = 0;
    pVPEContext->nNumFSSurf = 0;
    pVPEContext->nFSFlipSurfIdx = 0;
    pVPEContext->nFSMirrorIdx = 0;
    pVPEContext->nFSFlipIdx = 0;
    pVPEContext->dwOverlayFSHead = 0;
    pVPEContext->dwOverlayFSOvlHead = 0;

    // initialize context
    //pVPEContext->pNVPChannel = (ULONG) 0;
    pVPEContext->dwChipId = pCmdObj->dwChipId;
    pVPEContext->dwDeviceId = pCmdObj->dwDeviceId;
    pVPEContext->hClient = pCmdObj->hClient;
    pVPEContext->pFrameBuffer = pCmdObj->pFrameBuffer;
    pVPEContext->dwFBLen = pCmdObj->dwFBLen;
    pVPEContext->pOverlayChannel = pCmdObj->pOvrChannel;
    pVPEContext->dwOvrSubChannel = pCmdObj->dwOvrSubChannel;
    pVPEContext->dwOverlayObjectID = pCmdObj->dwOverlayObjectID;
    pVPEContext->dwScalerObjectID = pCmdObj->dwScalerObjectID;
    pVPEContext->dwDVDSubPicObjectID = pCmdObj->dwDVDObjectID;
    pVPEContext->dwDVDObjectCtxDma = pCmdObj->dwDVDObjectCtxDma;
    pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat = pCmdObj->pNvPioFlipOverlayNotifierFlat;
    pVPEContext->nNumVidSurf = 0;
    pVPEContext->nNumVBISurf = 0;
    pVPEContext->dwVPState = NVP_STATE_STOPPED;
    pVPEContext->dwVPConFlags = pCmdObj->dwVPTypeFlags;
    pVPEContext->dwVPFlags = 0; //pCmdObj->dwVPInfoFlags;
    pVPEContext->nCurTransferIdx = NVP_EOQ;
    pVPEContext->nLastTransferIdx = NVP_EOQ;
    pVPEContext->nRecentTransferIdx = NVP_EOQ;
    pVPEContext->nNumTransfer = 0;
    pVPEContext->bxfContextCreated = FALSE;
    pVPEContext->bxfContextFlushed = FALSE;
    for (i = 0; i < NVP_MAX_SYNC_EVENTS; i++)
    {
        pVPEContext->hNVPSyncEvent[i] = (PVOID)(pCmdObj->hNVPSyncEvent[i]);
    }

    pVPEContext->nvDmaFifo = NULL;
    pVPEContext->DmaPushBufTotalSize = 32*1024*4;   // 32k DWORDs

    if (osNVPAllocDmaBuffer((PVOID)pHwDevExt, pVPEContext->DmaPushBufTotalSize, (PVOID *)&(pVPEContext->nvDmaFifo)))
        return ERROR_INVALID_PARAMETER;

    if (rmNVPAllocContextDma(pCmdObj->hClient,
                            NVP_DMA_FROM_MEMORY_PUSHBUF,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_READ_ONLY,
                            0, // selector
                            (U032)(pVPEContext->nvDmaFifo),
                            pVPEContext->DmaPushBufTotalSize - 1) != 0)
    {
        DBGPRINT(DBGERROR, "Cannot allocate Context Dma for PushBuffer Object");
        return ERROR_INVALID_PARAMETER;
    }

    // Allocate the DMA push buffer ERROR notifier
    if (rmNVPAllocContextDma(  pCmdObj->hClient,
                            NVP_DMA_PUSHBUF_ERROR_NOTIFIER,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_WRITE_ONLY,
                            0, // selector
                            (U032)(&(pVPEContext->DmaPushBufErrNotifier[0])),
                            sizeof(NvNotification) - 1) != 0)
    {
        DBGPRINT(DBGERROR, "Cannot allocate PushBuffer notifier");
        return ERROR_INVALID_PARAMETER;
    }

    // allocate DMA channel here !
    if (rmNVPAllocChannelDma(  pCmdObj->hClient,
                            pCmdObj->dwDeviceId,
                            NVP_VPE_CHANNEL,
                            NV4_CHANNEL_DMA,
                            NVP_DMA_PUSHBUF_ERROR_NOTIFIER,
                            NVP_DMA_FROM_MEMORY_PUSHBUF,
                            0,
                            (PVOID)&(pVPEContext->pDmaChannel)) != 0)
    {
        DBG_PRINT_STRING_VALUE(DBGERROR, "cannot allocate Dma Channel ", NVP_VPE_CHANNEL);
        return ERROR_INVALID_PARAMETER;
    }

    // alloc external decoder object
    if (rmNVPAllocObject(
                pCmdObj->hClient,
                NVP_VPE_CHANNEL,
                NVP_EXTERNAL_DECODER_OBJECT,
                NV03_EXTERNAL_VIDEO_DECODER) != 0)
    {
        DBG_PRINT_STRING_VALUE(DBGERROR, "cannot allocate External Decoder Object ", NVP_EXTERNAL_DECODER_OBJECT);
        return ERROR_INVALID_PARAMETER;
    }

    // allocate transfer object (used for vbi and video capture)
    if (rmNVPAllocObject(
                pCmdObj->hClient,
                NVP_VPE_CHANNEL,
                NVP_M2M_OBJECT,
                NV03_MEMORY_TO_MEMORY_FORMAT) != 0)
    {
        DBG_PRINT_STRING_VALUE(DBGERROR, "cannot allocate Mem to mem transfer Object ", NVP_M2M_OBJECT);
        return ERROR_INVALID_PARAMETER;
    }

    dwOvObj = (pCmdObj->dwChipId < NV_DEVICE_VERSION_10) ? NV04_VIDEO_OVERLAY : NV10_VIDEO_OVERLAY;

    // If not MultiMon, use the old way to make an overlay
    // Otherwise, specify which head to use.
    if (!pVPEContext->MultiMon) {
    // alloc overlay object
    if (rmNVPAllocObject(
                pCmdObj->hClient,
                NVP_VPE_CHANNEL,
                NVP_OVERLAY_OBJECT,
                dwOvObj) != 0)
    {
        DBG_PRINT_STRING_VALUE(DBGERROR, "cannot allocate Overlay Object ", NVP_OVERLAY_OBJECT);
        return ERROR_INVALID_PARAMETER;
    }
    } else {
        NV07A_ALLOCATION_PARAMETERS nv07aAllocParms;

        nv07aAllocParms.logicalHeadId = pVPEContext->dwOverlayFSOvlHead;
        if (rmNVPAlloc(pCmdObj->hClient,
                  NVP_VPE_CHANNEL,
                  NVP_OVERLAY_OBJECT,
                  NV10_VIDEO_OVERLAY,
                  &nv07aAllocParms) != 0)
        {
            DBG_PRINT_STRING_VALUE(DBGERROR, "cannot allocate Overlay Object ", NVP_OVERLAY_OBJECT);
            return ERROR_INVALID_PARAMETER;
        }
    }


    // alloc DVD subpicture object
    if (rmNVPAllocObject(
                pCmdObj->hClient,
                NVP_VPE_CHANNEL,
                NVP_DVD_SUBPICTURE_OBJECT,
                NV4_DVD_SUBPICTURE) != 0)
    {
        DBG_PRINT_STRING_VALUE(DBGERROR, "cannot allocate DVD subpicture Object ", NVP_DVD_SUBPICTURE_OBJECT);
        return ERROR_INVALID_PARAMETER;
    }

    // allocate the transfer FROM context
    if (rmNVPAllocContextDma(
                        pCmdObj->hClient,
                        NVP_M2M_FROM_CONTEXT,
                        NV01_CONTEXT_DMA_FROM_MEMORY,       //NV_CONTEXT_DMA_FROM_MEMORY ?
                        0, // (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0, // selector
                        (U032)pCmdObj->pFrameBuffer,
                        pCmdObj->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate notifiers for M2M transfer Object");
        return ERROR_INVALID_PARAMETER;
    }

    // allocate a notify context for transfer object
    if (rmNVPAllocContextDma(
                        pCmdObj->hClient,
                        NVP_M2M_NOTIFIER,
                        NV01_CONTEXT_DMA_FROM_MEMORY,
                        0,  //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0,  // selector
                        (U032)(pVPEContext->axfNotifiers),
                        (sizeof( NvNotification)*NVP_CLASS039_MAX_NOTIFIERS  - 1)) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate notifiers for M2M transfer Object");
        return ERROR_INVALID_PARAMETER;
    }
    
    // allocate a notify context for external decoder
    if (rmNVPAllocContextDma(
                    pCmdObj->hClient,
                    NVP_EXTERNAL_DECODER_NOTIFIER,
                    NV01_CONTEXT_DMA_FROM_MEMORY,
                    0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                    0, // selector
                    (U032)(pVPEContext->avpNotifiers),
                    (sizeof(NvNotification)*NVP_CLASS04D_MAX_NOTIFIERS - 1)) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
    {
        DBGPRINT(DBGERROR, "cannot allocate notifiers for external decoder object");
        return ERROR_INVALID_PARAMETER;
    }

    // context DMA for video field0
    if (rmNVPAllocContextDma(
                pCmdObj->hClient,
                NVP_VIDEO_FIELD0_BUFFER_CONTEXT,
                NV01_CONTEXT_DMA_FROM_MEMORY,
                0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                0, // selector
                (U032) (pCmdObj->pFrameBuffer),
                pCmdObj->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate context DMA for video field0");
        return ERROR_INVALID_PARAMETER;
    }

    // context DMA for video field1 
    if (rmNVPAllocContextDma(
                pCmdObj->hClient,
                NVP_VIDEO_FIELD1_BUFFER_CONTEXT,
                NV01_CONTEXT_DMA_FROM_MEMORY,
                0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                0, // selector
                (U032) (pCmdObj->pFrameBuffer),
                pCmdObj->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "Cannot allocate context DMA for video field1");
        return ERROR_INVALID_PARAMETER;
    }
    
    // context DMA for video field0
    if (rmNVPAllocContextDma(
                pCmdObj->hClient,
                NVP_VBI_FIELD0_BUFFER_CONTEXT,
                NV01_CONTEXT_DMA_FROM_MEMORY, //NV01_CONTEXT_DMA,
                0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                0, // selector
                (U032) (pCmdObj->pFrameBuffer),
                pCmdObj->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate context DMA for VBI field0");
        return ERROR_INVALID_PARAMETER;
    }

    // context DMA for video field1
    if (rmNVPAllocContextDma(
                pCmdObj->hClient,
                NVP_VBI_FIELD1_BUFFER_CONTEXT,
                NV01_CONTEXT_DMA_FROM_MEMORY, //NV01_CONTEXT_DMA,
                0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                0, // selector
                (U032) (pCmdObj->pFrameBuffer),
                pCmdObj->dwFBLen - 1) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
    {
        DBGPRINT(DBGERROR, "cannot allocate context DMA for VBI field1");
        return ERROR_INVALID_PARAMETER;
    }

    // allocate a context for overlay object
    if (rmNVPAllocContextDma(
                        pCmdObj->hClient,
                        NVP_OVERLAY_BUFFER_CONTEXT,
                        NV01_CONTEXT_DMA,
                        0,  //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0,  // selector
                        (U032) (pCmdObj->pFrameBuffer),
                        pCmdObj->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate dma context for overlay buffer0");
        return ERROR_INVALID_PARAMETER;
    }

    // allocate a context for DVD subpicture object
    if (rmNVPAllocContextDma(
                        pCmdObj->hClient,
                        NVP_DVD_SUBPICTURE_CONTEXT,
                        NV01_CONTEXT_DMA,
                        0,  //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                        0,  // selector
                        (U032) (pCmdObj->pFrameBuffer),
                        pCmdObj->dwFBLen - 1) != 0)
    {
        DBGPRINT(DBGERROR, "cannot allocate dma context for DVD subpicture Object");
        return ERROR_INVALID_PARAMETER;
    }

    // allocate a notify context for overlay notifiers
    if (rmNVPAllocContextDma(
                    pCmdObj->hClient,
                    NVP_OVERLAY_NOTIFIER,
                    NV01_CONTEXT_DMA_FROM_MEMORY,
                    0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                    0, // selector
                    (U032)(pVPEContext->aovNotifiers),
                    (sizeof(NvNotification)*NVP_CLASS07A_MAX_NOTIFIERS - 1)) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
    {
        DBGPRINT(DBGERROR, "Cannot allocate notifiers for external decoder object");
        return ERROR_INVALID_PARAMETER;
    }

    // reset media port notifiers
    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);
    avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    // reset overlay notifiers
    aovNotifiers = (NvNotification *) (pVPEContext->aovNotifiers);
    aovNotifiers[NV07A_NOTIFIERS_SET_OVERLAY(0)].status = 0;
    aovNotifiers[NV07A_NOTIFIERS_SET_OVERLAY(1)].status = 0;

    pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat = (U032) (pVPEContext->aovNotifiers);

    // init Dma channel pointers
    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    pDmaChannel->Put = 0;
    nvDmaCount = 0;

    // set m2m object context
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_OBJECT, NVP_M2M_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_CONTEXT_DMA_NOTIFIES, NVP_M2M_NOTIFIER);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_CONTEXT_DMA_BUFFER_IN, NVP_M2M_FROM_CONTEXT);

    // make sure decoder object is in the channel....
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_OBJECT , NVP_EXTERNAL_DECODER_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_NOTIFIES, NVP_EXTERNAL_DECODER_NOTIFIER);
    // plug in our buffer contexts
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_IMAGE(0), NVP_VIDEO_FIELD0_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_IMAGE(1), NVP_VIDEO_FIELD1_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_VBI(0), NVP_VBI_FIELD0_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_VBI(1), NVP_VBI_FIELD1_BUFFER_CONTEXT);

    // set overlay contexts
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OBJECT, NVP_OVERLAY_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_CONTEXT_DMA_NOTIFIES, NVP_OVERLAY_NOTIFIER);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_CONTEXT_DMA_OVERLAY(0), NVP_OVERLAY_BUFFER_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_CONTEXT_DMA_OVERLAY(1), NVP_OVERLAY_BUFFER_CONTEXT);

    // same for DVD subpicture context
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_OBJECT, NVP_DVD_SUBPICTURE_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_CONTEXT_DMA_OVERLAY, NVP_DVD_SUBPICTURE_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_CONTEXT_DMA_IMAGE_IN, NVP_DVD_SUBPICTURE_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_CONTEXT_DMA_IMAGE_OUT, NVP_DVD_SUBPICTURE_CONTEXT);

    pVPEContext->nvDmaCount = nvDmaCount;
    pDmaChannel->Put = nvDmaCount << 2;

    // NB: we don't want to launch any captures at this point

    // setup the transfer callback
    evParam.hRoot = pCmdObj->hClient;
    evParam.hObjectParent = NVP_M2M_OBJECT;
    evParam.hObjectNew = NVP_M2M_EVENT;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV039_NOTIFIERS_BUFFER_NOTIFY;
    NvU64_VALUE(evParam.hEvent) = (ULONG) NVPTransferNotify;
    status = rmNVPAllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "cannot create callback for m2m transfer 0");
        return ERROR_INVALID_PARAMETER;
    }

    // setup fields callbacks
    evParam.hRoot = pCmdObj->hClient;
    evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
    evParam.hObjectNew = NVP_VIDEO_FIELD0_BUFFER_EVENT;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0);
    NvU64_VALUE(evParam.hEvent) = (ULONG) VideoField0Notify;

    // video field0
    status = rmNVPAllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "cannot create callback for video field 0");
        return ERROR_INVALID_PARAMETER;
    }

    // video field1
    evParam.hRoot = pCmdObj->hClient;
    evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
    evParam.hObjectNew = NVP_VIDEO_FIELD1_BUFFER_EVENT;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1);
    NvU64_VALUE(evParam.hEvent) = (ULONG) VideoField1Notify;

    status = rmNVPAllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "cannot create callback for video field 1");
        return ERROR_INVALID_PARAMETER;
    }

    // vbi field0
    evParam.hRoot = pCmdObj->hClient;
    evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
    evParam.hObjectNew = NVP_VBI_FIELD0_BUFFER_EVENT;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0);
    NvU64_VALUE(evParam.hEvent) = (ULONG) VBIField0Notify;

    status = rmNVPAllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "cannot create callback for VBI field 0");
        return ERROR_INVALID_PARAMETER;
    }
            
    // vbi field1
    evParam.hRoot = pCmdObj->hClient;
    evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
    evParam.hObjectNew = NVP_VBI_FIELD1_BUFFER_EVENT;
    evParam.hClass = NV01_EVENT_KERNEL_CALLBACK;
    evParam.index = NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1);
    NvU64_VALUE(evParam.hEvent) = (ULONG) VBIField1Notify;

    status = rmNVPAllocEvent(&evParam);
    if (status)
    {
        DBGPRINT(DBGERROR, "cannot create callback for VBI field 1");
        return ERROR_INVALID_PARAMETER;
    }

    // media port sync event
    if (pCmdObj->hNVPSyncEvent[0])
    {
        // field 0
        evParam.hRoot = pCmdObj->hClient;
        evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
        evParam.hObjectNew = NVP_VSYNC_EVENT0;
        evParam.hClass = NV01_EVENT_WIN32_EVENT;
        evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0);
        NvU64_VALUE(evParam.hEvent) = (ULONG) pCmdObj->hNVPSyncEvent[0];

        status = rmNVPAllocEvent(&evParam);
        if (status)
        {
            DBGPRINT(DBGERROR, "cannot set event for video field 0");
            return ERROR_INVALID_PARAMETER;
        }

        // field 1 (use same event for both fields)
        evParam.hRoot = pCmdObj->hClient;
        evParam.hObjectParent = NVP_EXTERNAL_DECODER_OBJECT;
        evParam.hObjectNew = NVP_VSYNC_EVENT1;
        evParam.hClass = NV01_EVENT_WIN32_EVENT;
        evParam.index = NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1);
        NvU64_VALUE(evParam.hEvent) = (ULONG) pCmdObj->hNVPSyncEvent[0];

        status = rmNVPAllocEvent(&evParam);
        if (status)
        {
            DBGPRINT(DBGERROR, "cannot set event for video field 0");
            return ERROR_INVALID_PARAMETER;
        }

    }

    // media port sync event
    if (pCmdObj->hNVPSyncEvent[1] && pCmdObj->hNVPSyncEvent[2])
    {
        // field 0
        evParam.hRoot = pCmdObj->hClient;
        evParam.hObjectParent = NVP_OVERLAY_OBJECT;
        evParam.hObjectNew = NVP_OV_SYNC_EVENT0;
        evParam.hClass = NV01_EVENT_WIN32_EVENT;
        evParam.index = NV07A_NOTIFIERS_SET_OVERLAY(0);
        NvU64_VALUE(evParam.hEvent) = (ULONG) pCmdObj->hNVPSyncEvent[1];

        status = rmNVPAllocEvent(&evParam);
        if (status)
        {
            DBGPRINT(DBGERROR, "cannot set event for video field 0");
            return ERROR_INVALID_PARAMETER;
        }

        // field 1
        evParam.hRoot = pCmdObj->hClient;
        evParam.hObjectParent = NVP_OVERLAY_OBJECT;
        evParam.hObjectNew = NVP_OV_SYNC_EVENT1;
        evParam.hClass = NV01_EVENT_WIN32_EVENT;
        evParam.index = NV07A_NOTIFIERS_SET_OVERLAY(1);
        NvU64_VALUE(evParam.hEvent) = (ULONG) pCmdObj->hNVPSyncEvent[2];

        status = rmNVPAllocEvent(&evParam);
        if (status)
        {
            DBGPRINT(DBGERROR, "cannot set event for video field 0");
            return ERROR_INVALID_PARAMETER;
        }

    }

	//alloc m2m dst heap for dma xfer -osNVPAllocDmaBuffer naming is ambiguous, should change to allocheap
	if (osNVPAllocDmaBuffer((PVOID)pHwDevExt, BUFSIZE, (PVOID *)&(pVPEContext->pSysBuf)))
        return ERROR_INVALID_PARAMETER;

	if(!(pVPEContext->pSysBuf)){
		DBGPRINT(DBGERROR, "Unable to alloc Sys Buf\n");
		return ERROR_INVALID_PARAMETER;
	}

	//alloc context dma for m2m dst heap
	if (rmNVPAllocContextDma(
                    pVPEContext->hClient,
                    NVP_M2M_CONTEXT,
                    NV01_CONTEXT_DMA_FROM_MEMORY,
                    0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                    0, // selector
                    (U032)(pVPEContext->pSysBuf),
                    BUFSIZE-1) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
    {
        DBGPRINT(DBGERROR, "Cannot allocate mem 2 mem context");
        return DXERR_GENERIC;
    }

    return NO_ERROR;
}

/*
==============================================================================
    
    NVPUninitialize

    Description:    Free channels, DMA contexts and objects created by a
                    previous call to NVPInitialize

    Date:           Feb 03, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPUninitialize(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    PNVP_CONTEXT pVPEContext;

    DBGPRINT(DBGTRACE, "NVPUninitialize()");

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

    //NVPFlushChannel(pVPEContext);

    // free allocated resources
    ////////// shouldn't we wait for completion (either grab of transfer ?!)
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VIDEO_FIELD0_BUFFER_CONTEXT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VIDEO_FIELD1_BUFFER_CONTEXT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VBI_FIELD0_BUFFER_CONTEXT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VBI_FIELD1_BUFFER_CONTEXT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VBI_FIELD0_BUFFER_EVENT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VBI_FIELD1_BUFFER_EVENT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VIDEO_FIELD0_BUFFER_EVENT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VIDEO_FIELD1_BUFFER_EVENT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_EXTERNAL_DECODER_NOTIFIER);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_OVERLAY_BUFFER_CONTEXT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_DVD_SUBPICTURE_CONTEXT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_OVERLAY_NOTIFIER);
    // free transfer object resources
    //if (pVPEContext->bxfContextCreated)
        rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_M2M_CONTEXT);

	// free m2m dst heap
	if(pVPEContext->pSysBuf){
		osNVPReleaseDmaBuffer((PVOID)(pHwDevExt), (PVOID)(pVPEContext->pSysBuf));
	}

    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_M2M_NOTIFIER);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_M2M_FROM_CONTEXT);
    rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_M2M_EVENT);
    rmNVPFree(pCmdObj->hClient, NVP_VPE_CHANNEL, NVP_M2M_OBJECT);

    if (pVPEContext->hNVPSyncEvent[0])
    {
        rmNVPFree(pCmdObj->hClient, NVP_EXTERNAL_DECODER_OBJECT, NVP_VSYNC_EVENT0);
        rmNVPFree(pCmdObj->hClient, NVP_EXTERNAL_DECODER_OBJECT, NVP_VSYNC_EVENT1);
        pVPEContext->hNVPSyncEvent[0] = NULL;
    }

    if (pVPEContext->hNVPSyncEvent[1] || pVPEContext->hNVPSyncEvent[2])
    {
        rmNVPFree(pCmdObj->hClient, NVP_OVERLAY_OBJECT, NVP_OV_SYNC_EVENT0);
        rmNVPFree(pCmdObj->hClient, NVP_OVERLAY_OBJECT, NVP_OV_SYNC_EVENT1);
        pVPEContext->hNVPSyncEvent[1] = NULL;
        pVPEContext->hNVPSyncEvent[2] = NULL;
    }

    rmNVPFree(pCmdObj->hClient, NVP_VPE_CHANNEL, NVP_EXTERNAL_DECODER_OBJECT);
    rmNVPFree(pCmdObj->hClient, NVP_VPE_CHANNEL, NVP_OVERLAY_OBJECT);
    rmNVPFree(pCmdObj->hClient, NVP_VPE_CHANNEL, NVP_DVD_SUBPICTURE_OBJECT);

    if (pVPEContext->nvDmaFifo != NULL)
    {
        rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_VPE_CHANNEL);
        rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_DMA_FROM_MEMORY_PUSHBUF);
        rmNVPFree(pCmdObj->hClient, pCmdObj->dwDeviceId, NVP_DMA_PUSHBUF_ERROR_NOTIFIER);
        osNVPReleaseDmaBuffer((PVOID)(pHwDevExt), (PVOID)(pVPEContext->nvDmaFifo));
    }

    pVPEContext->pOverlayChannel = 0;
    pVPEContext->nvDmaFifo = NULL;
    pVPEContext->pDmaChannel = NULL;

    pVPEContext->nvpOverlay.dwOverlayBufferIndex = NVP_UNDEF;

    return NO_ERROR;
}

/*
==============================================================================
    
    NVPUpdate

    Description:    Updates the NV video port data.

    Date:           Feb 03, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPUpdate(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    PNVP_CONTEXT pVPEContext;
    PNVP_OVERLAY pnvOverlay;
    DWORD i, dwFlipStatus;

    DBGPRINT(DBGTRACE, "NVPUpdate()");

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

	// get decoder height
	pVPEContext->dwDecoderHeight = pCmdObj->dwInHeight;

    pVPEContext->dwVPFlags = pCmdObj->dwVPInfoFlags;
    pVPEContext->pOverlayChannel = pCmdObj->pOvrChannel;

    if (pCmdObj->dwExtra422NumSurfaces > NVP_MAX_EXTRA422)
    {
        DBGPRINT(DBGERROR, "NVPUpdate() exceeded number of extra 422 surfaces");
        return ERROR_INVALID_PARAMETER;
    }

    // get a pointer to the overlay object in VPE context
    pnvOverlay = &(pVPEContext->nvpOverlay);

    // update overlay info: extra 422 surfaces used for prescaling
    pnvOverlay->dwExtra422NumSurfaces = pCmdObj->dwExtra422NumSurfaces;
    pnvOverlay->dwExtra422Index = pCmdObj->dwExtra422Index;
    pnvOverlay->dwExtra422Pitch = pCmdObj->dwExtra422Pitch;            // extra surface pitch

    for (i = 0; i < pCmdObj->dwExtra422NumSurfaces; i++)
    {
        pnvOverlay->aExtra422OverlayOffset[i] = pCmdObj->aExtra422OverlayOffset[i]; // extra memory space used for prescaling
    }

    // just in case flipping was turned on before starting mediaport, turn it off only for the first 2 manual flips
    dwFlipStatus = g_bFlip;
    g_bFlip = FALSE;

    // start video grabbing here if not already done in NVPStart
    if ((pVPEContext->nNumVidSurf > 0) && !(pVPEContext->dwVPState & NVP_STATE_STARTED_VIDEO))
    {
        // start VBI at the same time as video if vbi is not already enabled !
        if (pVPEContext->nNumVBISurf > 0)
        {
            // is it already started, then no need to do it again...
            if (!(pVPEContext->dwVPState & NVP_STATE_STARTED_VBI))
            {
                (pVPEContext->aVBISurfaceInfo[pVPEContext->nNumVBISurf - 1]).nNextSurfaceIdx = 0;
                pVPEContext->nNextVBISurfId = 1;
            }
            pVPEContext->dwVPState = (NVP_STATE_STARTED | NVP_STATE_STARTED_VIDEO | NVP_STATE_STARTED_VBI);
        }
        else
            pVPEContext->dwVPState = (NVP_STATE_STARTED | NVP_STATE_STARTED_VIDEO);
        
        (pVPEContext->aVideoSurfaceInfo[pVPEContext->nNumVidSurf - 1]).nNextSurfaceIdx = 0;
        // kick off VP
        pVPEContext->nNextVideoSurfId = 1;
        pnvOverlay->dwOverlayBufferIndex = NVP_UNDEF;
#ifdef _NVP_TEST_NEXT_FIELD
            g_nIntCounter = 0;
            g_nNextFieldId = 0;
#endif
        // we OR 0x80 to tell function this is not a notifier calling !
        NVPProcessState(pHwDevExt, NVP_FIELD_EVEN | 0x80, FALSE);
        NVPProcessState(pHwDevExt, NVP_FIELD_ODD | 0x80, FALSE);
    }
    // start VBI grabbing here if not already done in NVPStart
    else if ((pVPEContext->nNumVBISurf > 0) && !(pVPEContext->dwVPState & NVP_STATE_STARTED_VBI))
    {
        // if video already started, then only enable VBI state flag
        if (pVPEContext->dwVPState & NVP_STATE_STARTED_VIDEO)
        {
            pVPEContext->dwVPState |= NVP_STATE_STARTED_VBI;
        }
        else
        {
            pVPEContext->dwVPState = (NVP_STATE_STARTED | NVP_STATE_STARTED_VBI);
            (pVPEContext->aVBISurfaceInfo[pVPEContext->nNumVBISurf - 1]).nNextSurfaceIdx = 0;
            pVPEContext->nNextVBISurfId = 1;
            // kick off VP
            NVPProcessState(pHwDevExt, NVP_FIELD_EVEN, TRUE);
            NVPProcessState(pHwDevExt, NVP_FIELD_ODD, TRUE);
        }
    }

    // restore overlay flipping status
    g_bFlip = dwFlipStatus;

    return NO_ERROR;
}

/*
==============================================================================
    
    NVPStart

    Description:    Tells the NV video port to start grabbing video

    Date:           Feb 03, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPStart(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    PNVP_CONTEXT pVPEContext;
    PNVP_OVERLAY pnvOverlay;
    DWORD i, dwFlipStatus;

    DBGPRINT(DBGTRACE, "NVPStart()");

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

    // aleady started ?! it must be an UpdateVideo then...
    if (pVPEContext->dwVPState & NVP_STATE_STARTED)
        return NVPUpdate(pHwDevExt, pCmdObj);

	// get decoder height
	pVPEContext->dwDecoderHeight = pCmdObj->dwInHeight;

    // current
    pVPEContext->nVidSurfIdx = 0;
    pVPEContext->nVBISurfIdx = 0;
    // next in the list
    pVPEContext->nNextVideoSurfId = 0;
    pVPEContext->nNextVBISurfId = 0;

    // update overlay channel pointer !
    /////// NVPUpdate() must be the place for this !?
    pVPEContext->pOverlayChannel = pCmdObj->pOvrChannel;

    pVPEContext->dwVPState = NVP_STATE_STARTED;
    pVPEContext->dwVPFlags = pCmdObj->dwVPInfoFlags;

    if (pCmdObj->dwExtra422NumSurfaces > NVP_MAX_EXTRA422)
    {
        DBGPRINT(DBGERROR, "NVPStart() exceeded number of extra 422 surfaces ");
        return ERROR_INVALID_PARAMETER;
    }

    // get a pointer to the overlay object in VPE context
    pnvOverlay = &(pVPEContext->nvpOverlay);

    // update overlay info: extra 422 surfaces used for prescaling
    pnvOverlay->dwExtra422NumSurfaces = pCmdObj->dwExtra422NumSurfaces;
    pnvOverlay->dwExtra422Index = pCmdObj->dwExtra422Index;
    pnvOverlay->dwExtra422Pitch = pCmdObj->dwExtra422Pitch;            // extra surface pitch

    for (i = 0; i < pCmdObj->dwExtra422NumSurfaces; i++)
    {
        pnvOverlay->aExtra422OverlayOffset[i] = pCmdObj->aExtra422OverlayOffset[i]; // extra memory space used for prescaling
    }

    // just in case flipping was turned on before starting mediaport, turn it off only for the first 2 manual flips
    dwFlipStatus = g_bFlip;
    g_bFlip = FALSE;

    // make a cyclic chain with surfaces
    if (pVPEContext->nNumVidSurf > 0)
    {
        pVPEContext->dwVPState |= NVP_STATE_STARTED_VIDEO;
        (pVPEContext->aVideoSurfaceInfo[pVPEContext->nNumVidSurf - 1]).nNextSurfaceIdx = 0;
        // kick off VP
        pVPEContext->nNextVideoSurfId = 1;
        pnvOverlay->dwOverlayBufferIndex = NVP_UNDEF;
        // before kicking off the media port, check out VBI setting
        if (pVPEContext->nNumVBISurf > 0)
        {
            pVPEContext->dwVPState |= NVP_STATE_STARTED_VBI;
            (pVPEContext->aVBISurfaceInfo[pVPEContext->nNumVBISurf - 1]).nNextSurfaceIdx = 0;
            pVPEContext->nNextVBISurfId = 1;
        }
#ifdef _NVP_TEST_NEXT_FIELD
            g_nIntCounter = 0;
            g_nNextFieldId = 0;
#endif

        // we OR 0x80 to tell function this is not a notifier calling !
        NVPProcessState(pHwDevExt, NVP_FIELD_EVEN | 0x80, FALSE);
        NVPProcessState(pHwDevExt, NVP_FIELD_ODD | 0x80, FALSE);
    }
    else if (pVPEContext->nNumVBISurf > 0)
    {
        pVPEContext->dwVPState |= NVP_STATE_STARTED_VBI;
        (pVPEContext->aVBISurfaceInfo[pVPEContext->nNumVBISurf - 1]).nNextSurfaceIdx = 0;
        pVPEContext->nNextVBISurfId = 1;
        // kick off VP
        NVPProcessState(pHwDevExt, NVP_FIELD_EVEN, TRUE);
        NVPProcessState(pHwDevExt, NVP_FIELD_ODD, TRUE);
    }

    // restore overlay flipping status
    g_bFlip = dwFlipStatus;
    
    return NO_ERROR;
}

/*
==============================================================================
    
    NVP3Stop

    Description:    Tells the NV video port to stop grabbing. PIO channel
                    version

    Date:           Feb 03, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVP3Stop(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
#ifdef NVP_NV3
    Nv03ChannelPio *pNVPChannel = (Nv03ChannelPio *) (pCmdObj->pChannel);
    PNVP_CONTEXT pVPEContext;
    NvNotification *avpNotifiers;
    NVOS09_PARAMETERS ntfParam;
    U032 i = 0;

    DBGPRINT(DBGTRACE, "NVP3Stop()");

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);
    // aleady stopped ?!
    if (pVPEContext->dwVPState & NVP_STATE_STOPPED)
        return NO_ERROR;
    else
        pVPEContext->dwVPState = NVP_STATE_STOPPED;

    g_bFlip = FALSE;

    pVPEContext->pNVPChannel = (ULONG) pNVPChannel; // get a copy of the pio channel pointer
    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

    // tell VP to stop grabbing !
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.StopTransferImage = 0;
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.StopTransferVbi = 0;

    ntfParam.hObjectParent = pCmdObj->dwDeviceId;
#define NVP_NOTIFY_TIMEOUT 100000
    // wait for the last few notifies to show up...
    while ((((avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
             (avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
           ((avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
           (avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
           (i < NVP_NOTIFY_TIMEOUT)))
    {
        i++;
        //RmInterrupt(&ntfParam);
    }

#ifdef _STOP_OVERLAY
    // reset surfaces counters
    pVPEContext->nNumVBISurf = 0;
    pVPEContext->nNumVidSurf = 0;
#endif

#endif // NVP_NV3
    return NO_ERROR;
}

/*
==============================================================================
    
    NVPShutDownMediaPort

    Description:    shuts down media port engine

    Date:           May 24, 2000
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPShutDownMediaPort(PNVP_CONTEXT pVPEContext)
{
    NvNotification *avpNotifiers;
    Nv4ControlDma *pDmaChannel;
    U032 nvDmaCount;
    U032 i;

    DBGPRINT(DBGTRACE, "NVPShutDownMediaPort()");

    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

    // channel
    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;

    // stop grabbing !
    NVP_DMAPUSH_CHECK_FREE_COUNT(6);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_OBJECT , NVP_EXTERNAL_DECODER_OBJECT);
    //if (pCmdObj->dwStopStrm & NVP_STOP_VIDEO)
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_STOP_TRANSFER_IMAGE , 0);
    //if (pCmdObj->dwStopStrm & NVP_STOP_VBI)
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_STOP_TRANSFER_VBI , 0);
    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;

#define NVP_NOTIFY_TIMEOUT 100000
    i = 0;
    // wait for the last few notifies to show up...
    while ((((avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
             (avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
           ((avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
           (avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
           (i < NVP_NOTIFY_TIMEOUT)))
    {
        i++;
    }

    // timed-out...
    if (i >= NVP_NOTIFY_TIMEOUT)
        DBGPRINT(DBGERROR, "waiting for MediaPort notifiers timed-out....");

    avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

    // clear/reset sync event
    osNVPClearEvent((PVOID) pVPEContext->hNVPSyncEvent[0]);

    pVPEContext->dwVPState = NVP_STATE_STOPPED;

    return NO_ERROR;
}

/*
==============================================================================
    
    NVPShutDownOverlay

    Description:    shuts down overlay engine

    Date:           May 24, 2000
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPShutDownOverlay(PNVP_CONTEXT pVPEContext)
{
    Nv4ControlDma *pDmaChannel;
    U032 nvDmaCount;
    NvNotification *pPioFlipOverlayNotifier;
    U032 i;

    DBGPRINT(DBGTRACE, "NVPShutDownOverlay()");

    // channel
    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;

    // stop overlay engine
    if (pDmaChannel && g_bFlip)
    {
        g_bFlip = FALSE;

        if ((pVPEContext->hNVPSyncEvent[1]) && (pVPEContext->hNVPSyncEvent[2]))
        {
            osNVPClearEvent(pVPEContext->hNVPSyncEvent[1]);
            osNVPClearEvent(pVPEContext->hNVPSyncEvent[2]);
        }

        // reset notifiers
        pPioFlipOverlayNotifier = (NvNotification *)pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat;

        pPioFlipOverlayNotifier[1].status = NV_IN_PROGRESS;
        pPioFlipOverlayNotifier[2].status = NV_IN_PROGRESS;

        // stop overlay
        NVP_DMAPUSH_CHECK_FREE_COUNT(6);
        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OBJECT, NVP_OVERLAY_OBJECT); //pOverlayInfo->dwOverlayObjectID);
        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_STOP_OVERLAY(0), NV07A_STOP_OVERLAY_BETWEEN_BUFFERS); //NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE
        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_STOP_OVERLAY(1), NV07A_STOP_OVERLAY_BETWEEN_BUFFERS); //NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE
        pDmaChannel->Put = nvDmaCount << 2;
        pVPEContext->nvDmaCount = nvDmaCount;

        // wait here only if not waiting on events in ring3 !!!
        if ((pVPEContext->hNVPSyncEvent[1] == NULL) || (pVPEContext->hNVPSyncEvent[2] == NULL))
        {
            i = 0;
            while (((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) || (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) && (i < NVP_VPP_TIME_OUT))
                i++;

            // timed-out...
            if (i >= NVP_VPP_TIME_OUT)
                DBGPRINT(DBGERROR, "waiting for overlay flip notifiers timed-out....");
        }

        pPioFlipOverlayNotifier[1].status = 0;
        pPioFlipOverlayNotifier[2].status = 0;
    }
    // don't let ring3 wait for nothing !
    else if ((pVPEContext->hNVPSyncEvent[1]) && (pVPEContext->hNVPSyncEvent[2]))
    {
        osNVPSetEvent(pVPEContext->hNVPSyncEvent[1]);
        osNVPSetEvent(pVPEContext->hNVPSyncEvent[2]);
    }

    return NO_ERROR;
}


/*
==============================================================================
    
    NVPStop

    Description:    Tells the NV video port to stop grabbing

    Date:           Feb 03, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPStop(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    PNVP_CONTEXT pVPEContext;
    //Nv03ChannelPio *pOvChannel;
    Nv4ControlDma *pDmaChannel;
    U032 nvDmaCount;
    NvNotification *pPioFlipOverlayNotifier;

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

    // overlay channel
    //pOvChannel = (Nv03ChannelPio *) (pVPEContext->pOverlayChannel);   // get a pointer to the overlay channel

    // NV3 specific code
    if (NVP_GET_CHIP_VER(pVPEContext->dwChipId) <= NVP_CHIP_VER_03)
        return NVP3Stop(pHwDevExt, pCmdObj);

    DBGPRINT(DBGTRACE, "NVPStop()");

    // aleady stopped ?!
    if (pVPEContext->dwVPState & NVP_STATE_STOPPED)
        return NO_ERROR;
    else
        pVPEContext->dwVPState = NVP_STATE_STOPPED;

    // channel
    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;

#ifdef _STOP_OVERLAY
    // stop overlay engine
    if (pDmaChannel && g_bFlip)
    {
        g_bFlip = FALSE;

        if ((pVPEContext->hNVPSyncEvent[1]) && (pVPEContext->hNVPSyncEvent[2]))
        {
            osNVPClearEvent(pVPEContext->hNVPSyncEvent[1]);
            osNVPClearEvent(pVPEContext->hNVPSyncEvent[2]);
        }

        // reset notifiers
        pPioFlipOverlayNotifier = (NvNotification *)pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat;

        pPioFlipOverlayNotifier[1].status = NV_IN_PROGRESS;
        pPioFlipOverlayNotifier[2].status = NV_IN_PROGRESS;

        // stop overlay
        NVP_DMAPUSH_CHECK_FREE_COUNT(6);
        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OBJECT, NVP_OVERLAY_OBJECT); //pOverlayInfo->dwOverlayObjectID);
        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_STOP_OVERLAY(0), NV07A_STOP_OVERLAY_BETWEEN_BUFFERS); //NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE
        NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_STOP_OVERLAY(1), NV07A_STOP_OVERLAY_BETWEEN_BUFFERS); //NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE
        pDmaChannel->Put = nvDmaCount << 2;
        pVPEContext->nvDmaCount = nvDmaCount;

        // wait here only if not waiting on events in ring3 !!!
        if ((pVPEContext->hNVPSyncEvent[1] == NULL) || (pVPEContext->hNVPSyncEvent[2] == NULL))
        {
            i = 0;
            while (((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) || (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) && (i < NVP_VPP_TIME_OUT))
                i++;

            // timed-out...
            if (i >= NVP_VPP_TIME_OUT)
                DBGPRINT(DBGERROR, "waiting for overlay flip notifiers timed-out....");
        }

        pPioFlipOverlayNotifier[1].status = 0;
        pPioFlipOverlayNotifier[2].status = 0;
    }
    // don't let ring3 wait for nothing !
    else if ((pVPEContext->hNVPSyncEvent[1]) && (pVPEContext->hNVPSyncEvent[2]))
    {
        osNVPSetEvent(pVPEContext->hNVPSyncEvent[1]);
        osNVPSetEvent(pVPEContext->hNVPSyncEvent[2]);
    }

    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

    // tell VP to stop grabbing !
    nvDmaCount = pVPEContext->nvDmaCount;
    NVP_DMAPUSH_CHECK_FREE_COUNT(6);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_OBJECT , NVP_EXTERNAL_DECODER_OBJECT);
    //if (pCmdObj->dwStopStrm & NVP_STOP_VIDEO)
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_STOP_TRANSFER_IMAGE , 0);
    //if (pCmdObj->dwStopStrm & NVP_STOP_VBI)
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_STOP_TRANSFER_VBI , 0);
    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;

#define NVP_NOTIFY_TIMEOUT 100000
    i = 0;
    // wait for the last few notifies to show up...
    while ((((avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
             (avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
           ((avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS) ||
           (avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status == NV04D_NOTIFICATION_STATUS_IN_PROGRESS)) &&
           (i < NVP_NOTIFY_TIMEOUT)))
    {
        i++;
        //RmInterrupt(&ntfParam);
    }

    // timed-out...
    if (i >= NVP_NOTIFY_TIMEOUT)
        DBGPRINT(DBGERROR, "waiting for MediaPort notifiers timed-out....");

    avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(0)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(1)].status = 0; //NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

#else
//    NVPShutDownOverlay(pVPEContext);

    // reset notifiers
    pPioFlipOverlayNotifier = (NvNotification *)pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat;

    pPioFlipOverlayNotifier[1].status = 0;
    pPioFlipOverlayNotifier[2].status = 0;

    NVPShutDownMediaPort(pVPEContext);
#endif


    // reset surfaces counters
    pVPEContext->nNumVBISurf = 0;
    pVPEContext->nNumVidSurf = 0;

#ifdef _STOP_OVERLAY
    // clear/reset sync event
    osNVPClearEvent((PVOID) pVPEContext->hNVPSyncEvent[0]);
#endif

    return NO_ERROR;
}

/*
==============================================================================
    
    NVPRegSurface

    Description:    register a DDraw surface. Insert in list and update
                    attributes

    Date:           Feb 18, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPRegSurface(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    PNVP_CONTEXT pVPEContext;
    PNVP_SURFACE_INFO pSurface;
    BOOL bVideoStarted, bVBIStarted;

    DBGPRINT(DBGTRACE, "NVPRegSurface()");

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

    bVideoStarted = (pVPEContext->dwVPState & NVP_STATE_STARTED_VIDEO);
    bVBIStarted = (pVPEContext->dwVPState & NVP_STATE_STARTED_VBI);

    //resize VP if capturing
    if(pVPEContext->ulVPECaptureFlag){
        
        pCmdObj->dwPreWidth=(pVPEContext->ulVPECapturePrescaleXFactor *
                                    pCmdObj->dwInWidth)/1000;
        pCmdObj->dwPreHeight=(pVPEContext->ulVPECapturePrescaleYFactor *
                                    pCmdObj->dwInHeight)/1000;
    }               

    // update attributes of already existing surfaces ?!
    if (pCmdObj->bSurfUpdate)
    {
        // vbi surfaces ?!
        if (pCmdObj->bSurfVBI)
        {
            if (pCmdObj->dwSurfaceId < (ULONG)pVPEContext->nNumVBISurf)
            {
                pSurface = &(pVPEContext->aVBISurfaceInfo[pCmdObj->dwSurfaceId]);
                // check for mem offset in framebuffer !
                if (pSurface->pVidMem == pCmdObj->pSurfVidMem)
                {
                    pSurface->dwHeight = pCmdObj->dwVBIHeight;
                    pSurface->dwStartLine = pCmdObj->dwVBIStartLine;        // startline for vbi

                }
                else
                {
                    DBGPRINT(DBGERROR, "can't update VBI surface data !");
                    return ERROR_INVALID_PARAMETER;
                }

            }
            else
            {
                // new vbi surfaces
                pSurface = &(pVPEContext->aVBISurfaceInfo[pCmdObj->dwSurfaceId]);
                (pVPEContext->nNumVBISurf)++;
                pSurface->pVidMem = pCmdObj->pSurfVidMem;               // Flat pointer to the surface
                pSurface->dwOffset = pCmdObj->dwSurfOffset;             // Offset from the base of the frame buffer
                pSurface->dwPitch = pCmdObj->dwSurfPitch;
                pSurface->dwHeight = pCmdObj->dwVBIHeight;
                pSurface->dwStartLine = pCmdObj->dwVBIStartLine;        // Programs Y-crop for image, and startline for vbi
                pSurface->dwPreScaleSizeX = pCmdObj->dwInWidth;         // prescale size width == pitch for vbi surfaces
                pSurface->dwPreScaleSizeY = pCmdObj->dwInHeight;        // prescale size height == vbi height 
                pSurface->nNextSurfaceIdx = pCmdObj->dwSurfaceId + 1;   // the next surface idx in the chain
                pSurface->bIsFirstFieldCaptured = FALSE;                // reset value

            }
        }
        else    // video surface
        {
            if (pCmdObj->dwSurfaceId < (ULONG)pVPEContext->nNumVidSurf)
            {
                //////////
                ///// I assume something changing in the following values only !
                pSurface = &(pVPEContext->aVideoSurfaceInfo[pCmdObj->dwSurfaceId]);
                if (pSurface->pVidMem == pCmdObj->pSurfVidMem)
                {
#ifndef _STOP_OVERLAY
                    ///// we need to stop the media port and restart it later because there is a change in the scaling factors
                    if ((pSurface->dwHeight != pCmdObj->dwInHeight) || 
                        (pSurface->dwWidth != pCmdObj->dwInWidth) ||
                        (pSurface->dwPreScaleSizeX != pCmdObj->dwPreWidth) ||
                        (pSurface->dwPreScaleSizeY != pCmdObj->dwPreHeight))
                    {
                        //pVPEContext->bVPScalingFactorChanged = TRUE;
                        // aleady stopped ?!
                        if (!(pVPEContext->dwVPState & NVP_STATE_STOPPED))
                        {
                            NVPShutDownOverlay(pVPEContext);
                            NVPShutDownMediaPort(pVPEContext);
                        }
                    }
#endif
                    pSurface->dwHeight = pCmdObj->dwInHeight;
                    pSurface->dwWidth = pCmdObj->dwInWidth;                 // Ignored for VBI
                    pSurface->dwStartLine = pCmdObj->dwImageStartLine;      // Programs Y-crop for image, and startline for vbi
                    pSurface->dwPreScaleSizeX = pCmdObj->dwPreWidth;        // prescale size width
                    pSurface->dwPreScaleSizeY = pCmdObj->dwPreHeight;       // prescale size height
                }
                else
                {
                    DBGPRINT(DBGTRACE, "can't update video surface data !");
                    return ERROR_INVALID_PARAMETER;
                }
            }
            else
            {
                // new video surfaces
                pSurface = &(pVPEContext->aVideoSurfaceInfo[pCmdObj->dwSurfaceId]);
                (pVPEContext->nNumVidSurf)++;
                pSurface->pVidMem = pCmdObj->pSurfVidMem;               // Flat pointer to the surface
                pSurface->dwOffset = pCmdObj->dwSurfOffset;             // Offset from the base of the frame buffer
                pSurface->dwPitch = pCmdObj->dwSurfPitch;
                pSurface->dwHeight = pCmdObj->dwInHeight;
                pSurface->dwWidth = pCmdObj->dwInWidth;                 // Ignored for VBI
                pSurface->dwStartLine = pCmdObj->dwImageStartLine;      // Programs Y-crop for image, and startline for vbi
                pSurface->dwPreScaleSizeX = pCmdObj->dwPreWidth;        // prescale size width
                pSurface->dwPreScaleSizeY = pCmdObj->dwPreHeight;       // prescale size height
                pSurface->nNextSurfaceIdx = pCmdObj->dwSurfaceId + 1;   // the next surface idx in the chain
                pSurface->bIsFirstFieldCaptured = FALSE;                // reset value

            }
        }
    }
    else    // new surfaces !
    {
        if (pVPEContext->dwVPState & NVP_STATE_STARTED)
        {
            DBGPRINT(DBGERROR, "can't register new DD surfaces while Video Port is running !");
            return ERROR_INVALID_PARAMETER;
        }

        // vbi surfaces ?!
        if (pCmdObj->bSurfVBI)
        {
            pSurface = &(pVPEContext->aVBISurfaceInfo[pCmdObj->dwSurfaceId]);
            (pVPEContext->nNumVBISurf)++;
            pSurface->dwStartLine = pCmdObj->dwVBIStartLine;       // startline for vbi
            pSurface->dwPreScaleSizeX = pCmdObj->dwInWidth;        // shouldn't be any prescaling performed on vbi surfaces
            pSurface->dwPreScaleSizeY = pCmdObj->dwInHeight;
        }
        else    // video surface
        {
            pSurface = &(pVPEContext->aVideoSurfaceInfo[pCmdObj->dwSurfaceId]);
            (pVPEContext->nNumVidSurf)++;
            pSurface->dwStartLine = pCmdObj->dwImageStartLine;      // startline for video
            pSurface->dwPreScaleSizeX = pCmdObj->dwPreWidth;        // prescale size width
            pSurface->dwPreScaleSizeY = pCmdObj->dwPreHeight;       // prescale size height
        }

        pSurface->pVidMem = pCmdObj->pSurfVidMem;               // Flat pointer to the surface
        pSurface->dwOffset = pCmdObj->dwSurfOffset;             // Offset from the base of the frame buffer
        pSurface->dwPitch = pCmdObj->dwSurfPitch;
        pSurface->dwHeight = pCmdObj->dwInHeight;
        pSurface->dwWidth = pCmdObj->dwInWidth;
        pSurface->nNextSurfaceIdx = pCmdObj->dwSurfaceId + 1;   // the next surface idx in the chain
        pSurface->bIsFirstFieldCaptured = FALSE;                // reset value
    }

    return NO_ERROR;
}

/*
==============================================================================
    
    NVPUpdateOverlay

    Description:    update the overlay info structure

    Date:           Mar 01, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPUpdateOverlay(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    PNVP_CONTEXT pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);
    Nv03ChannelPio *pChannel = (Nv03ChannelPio *) (pVPEContext->pOverlayChannel);   // get a pointer to the overlay channel;
    PNVP_OVERLAY pnvOverlay =  &(pVPEContext->nvpOverlay);
    NvNotification *pPioFlipOverlayNotifier;
    Nv4ControlDma *pDmaChannel;
    U032 nvDmaCount, i = 0;
    U032 srcDeltaX, srcDeltaY;
    U032 bRing0FlippingFlag;

    DBGPRINT(DBGTRACE, "NVPUpdateOverlay()");
#define VBI_CAP_OFFSET	0//1	//Hack. When capturing vid +vbi, the last vid line is always corrupted. Use
							//this to offset line and apply for all cases for consistency. ch    

	//Adjust for WDM prescale hack (only when capturing!!)
	//need to zoom back up because of ME prescaling during capture ONLY IF
	//vp is using the overlay engine(bVPNotUseOverSurf)
    if (pVPEContext->ulVPECaptureFlag && !(pVPEContext->ulVPNotUseOverSurf))
    {
        DBGPRINT(DBGTRACE, "NVPUpdateOverlay():Adjusting for VP prescaling...");

        pCmdObj->dwOverlaySrcWidth = (pVPEContext->aVideoSurfaceInfo[0]).dwPreScaleSizeX;    //getting prescale size since it's the same for all surfs.
#ifdef FORCE_INTERLEAVE_MEM
		pCmdObj->dwOverlaySrcHeight = ((pVPEContext->aVideoSurfaceInfo[0]).dwPreScaleSizeY - VBI_CAP_OFFSET)*2;
#else
        if(pVPEContext->dwVPFlags & DDVP_INTERLEAVE){
			pCmdObj->dwOverlaySrcHeight = ((pVPEContext->aVideoSurfaceInfo[0]).dwPreScaleSizeY - VBI_CAP_OFFSET)*2;
		}else{
			pCmdObj->dwOverlaySrcHeight = (pVPEContext->aVideoSurfaceInfo[0]).dwPreScaleSizeY - VBI_CAP_OFFSET;
		}
#endif
    }else{	//not capturing

#ifdef FORCE_INTERLEAVE_MEM
		//since we are forcing interleave and if DDVP_INTERLEAVE is off, 
		//overlay src size is is incorrect. need to double src height
		if (!(pVPEContext->dwVPFlags & DDVP_INTERLEAVE)){
			pCmdObj->dwOverlaySrcHeight = (pCmdObj->dwOverlaySrcHeight - VBI_CAP_OFFSET)*2;
			//pCmdObj->dwOverlaySrcPitch *= 2;
		}
#endif
	}
    // ds/dx
    if (pCmdObj->dwOverlayDstWidth <= 1) {
        srcDeltaX = 0x100000;
    } else {
        srcDeltaX = (pCmdObj->dwOverlaySrcWidth - 1) << 16;
        srcDeltaX /= pCmdObj->dwOverlayDstWidth - 1;
        srcDeltaX <<= 4;
    }

    // dt/dy
    if (pCmdObj->dwOverlayDstHeight <= 1) {
        srcDeltaY = 0x100000;
    } else {
        srcDeltaY = (pCmdObj->dwOverlaySrcHeight - 1) << 16;
        if (pVPEContext->dwChipId <= NV_DEVICE_VERSION_4) {
            srcDeltaY /= pCmdObj->dwOverlayDstHeight + 2;
        } else {
            srcDeltaY /= pCmdObj->dwOverlayDstHeight + 1;
        }
        srcDeltaY <<= 4;
    }

    // sizein
    pCmdObj->dwOverlaySrcSize = (pCmdObj->dwOverlaySrcHeight)<<16;
    pCmdObj->dwOverlaySrcSize |= pCmdObj->dwOverlaySrcWidth;

    pPioFlipOverlayNotifier = (NvNotification *)pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat;

    if ((pCmdObj->dwOverlayDstWidth != 0) && (pCmdObj->dwOverlayDstHeight != 0))
    {
        // I must rather have a structure memcpy !
        pnvOverlay->dwOverlayFormat = pCmdObj->dwOverlayFormat;
        pnvOverlay->dwOverlaySrcX = pCmdObj->dwOverlaySrcX;
        pnvOverlay->dwOverlaySrcY = pCmdObj->dwOverlaySrcY;
        pnvOverlay->dwOverlaySrcPitch = pCmdObj->dwOverlaySrcPitch;
        pnvOverlay->dwOverlaySrcSize = pCmdObj->dwOverlaySrcSize;
        pnvOverlay->dwOverlaySrcWidth = pCmdObj->dwOverlaySrcWidth;
        pnvOverlay->dwOverlaySrcHeight = pCmdObj->dwOverlaySrcHeight;
        pnvOverlay->dwOverlayDstWidth = pCmdObj->dwOverlayDstWidth;
        pnvOverlay->dwOverlayDstHeight = pCmdObj->dwOverlayDstHeight;
        pnvOverlay->dwOverlayDstX = pCmdObj->dwOverlayDstX;
        pnvOverlay->dwOverlayDstY = pCmdObj->dwOverlayDstY;
        pnvOverlay->dwOverlayDeltaX = srcDeltaX;//pCmdObj->dwOverlayDeltaX;
        pnvOverlay->dwOverlayDeltaY = srcDeltaY;//pCmdObj->dwOverlayDeltaY;
        pnvOverlay->dwOverlayColorKey = pCmdObj->dwOverlayColorKey;
        pnvOverlay->dwOverlayMode = pCmdObj->dwOverlayMode;
        pnvOverlay->dwOverlayMaxDownScale = pCmdObj->dwOverlayMaxDownScale;

		DBGPRINTVALUE(DBGINFO, "dwOverlaySrcX=", (pCmdObj->dwOverlaySrcX));
		DBGPRINTVALUE(DBGINFO, "dwOverlaySrcY=", (pCmdObj->dwOverlaySrcY));
		DBGPRINTVALUE(DBGINFO, "dwOverlaySrcPitch=", (pCmdObj->dwOverlaySrcPitch));
		DBGPRINTVALUE(DBGINFO, "dwOverlaySrcSize=", (pCmdObj->dwOverlaySrcSize));
		DBGPRINTVALUE(DBGINFO, "dwOverlaySrcWidth=", (pCmdObj->dwOverlaySrcWidth));
		DBGPRINTVALUE(DBGINFO, "dwOverlaySrcHeight=", (pCmdObj->dwOverlaySrcHeight));
		DBGPRINTVALUE(DBGINFO, "dwOverlayDstWidth=", (pCmdObj->dwOverlayDstWidth));
		DBGPRINTVALUE(DBGINFO, "dwOverlayDstHeight=", (pCmdObj->dwOverlayDstHeight));
		DBGPRINTVALUE(DBGINFO, "dwOverlayDstX=", (pCmdObj->dwOverlayDstX));
		DBGPRINTVALUE(DBGINFO, "dwOverlayDstY=", (pCmdObj->dwOverlayDstY));
		DBGPRINTVALUE(DBGINFO, "srcDeltaX=", (srcDeltaX));
		DBGPRINTVALUE(DBGINFO, "srcDeltaY=", (srcDeltaY));
		DBGPRINTVALUE(DBGINFO, "dwOverlayMode=", (pCmdObj->dwOverlayMode));
		DBGPRINTVALUE(DBGINFO, "dwOverlayMaxDownScale=", (pCmdObj->dwOverlayMaxDownScale));

        // update overlay info: extra 422 surfaces used for prescaling
        pnvOverlay->dwExtra422NumSurfaces = pCmdObj->dwExtra422NumSurfaces;
        pnvOverlay->dwExtra422Index = pCmdObj->dwExtra422Index;
        pnvOverlay->dwExtra422Pitch = pCmdObj->dwExtra422Pitch;            // extra surface pitch

        for (i = 0; i < pCmdObj->dwExtra422NumSurfaces; i++)
        {
            pnvOverlay->aExtra422OverlayOffset[i] = pCmdObj->aExtra422OverlayOffset[i]; // extra memory space used for prescaling
        }

        DBGPRINT(DBGTRACE, "overlay flipping enabled....");
        g_bFlip = TRUE;

        // if we get a 'show overlay' command while MediaPort engine is stopped, this means that we are in a 
        // 'pause state'... unless we are using KMVT to flip (STB-TV-tuner-like app), program the overlay engine to show the last frame captured by Media Port !
        bRing0FlippingFlag = 0;
        if (pVPEContext->pOvrRing0FlipFlag)
            bRing0FlippingFlag = *((U032 *)(pVPEContext->pOvrRing0FlipFlag));
        if ((pVPEContext->dwVPState & NVP_STATE_STOPPED) && !bRing0FlippingFlag)
        {
            NVPScheduleOverlayFlip(pVPEContext, &(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx]), 0, 0, 0);
            // reset notifiers
            pPioFlipOverlayNotifier[1].status = 0;
            pPioFlipOverlayNotifier[2].status = 0;

        }
    }
    else
    {
        if (g_bFlip)
        {
            DBGPRINT(DBGTRACE, "overlay flipping disabled....");

            g_bFlip = FALSE;    // no autoflip !

            // channel
            pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
            nvDmaCount = pVPEContext->nvDmaCount;

            // stop overlay engine
            if (pDmaChannel)
            {
                if ((pVPEContext->hNVPSyncEvent[1]) && (pVPEContext->hNVPSyncEvent[2]))
                {
                    osNVPClearEvent(pVPEContext->hNVPSyncEvent[1]);
                    osNVPClearEvent(pVPEContext->hNVPSyncEvent[2]);
                }

                // reset notifiers
//                pPioFlipOverlayNotifier = (NvNotification *)pVPEContext->nvpOverlay.pNvPioFlipOverlayNotifierFlat;

                pPioFlipOverlayNotifier[1].status = NV_IN_PROGRESS;
                pPioFlipOverlayNotifier[2].status = NV_IN_PROGRESS;

                // stop overlay
                NVP_DMAPUSH_CHECK_FREE_COUNT(6);
                NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OBJECT, NVP_OVERLAY_OBJECT); //pOverlayInfo->dwOverlayObjectID);
                NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_STOP_OVERLAY(0), NV07A_STOP_OVERLAY_BETWEEN_BUFFERS);
                NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_STOP_OVERLAY(1), NV07A_STOP_OVERLAY_BETWEEN_BUFFERS);
                pDmaChannel->Put = nvDmaCount << 2;
                pVPEContext->nvDmaCount = nvDmaCount;

               // wait here only if not waiting on events in ring3 !!!
               if ((pVPEContext->hNVPSyncEvent[1] == NULL) || (pVPEContext->hNVPSyncEvent[2] == NULL))
               {
                    while (((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) || (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) && (i < NVP_VPP_TIME_OUT))
                        i++;
                    // timed-out...
                    if (i >= NVP_VPP_TIME_OUT)
                        DBGPRINT(DBGERROR, "waiting for overlay flip notifiers timed-out....");
                }
    
                pPioFlipOverlayNotifier[1].status = 0;
                pPioFlipOverlayNotifier[2].status = 0;
            }
        }
        // don't let ring3 wait for nothing !
        else if ((pVPEContext->hNVPSyncEvent[1]) && (pVPEContext->hNVPSyncEvent[2]))
        {
            osNVPSetEvent(pVPEContext->hNVPSyncEvent[1]);
            osNVPSetEvent(pVPEContext->hNVPSyncEvent[2]);
        }
    }

    return NO_ERROR;
}

/*
==============================================================================
    
    NVPFlipVideoPort

    Description:    start capturing in specified surface

    Date:           Mar 01, 1999
    
    Author:         H. Azar

==============================================================================
*/
#pragma warning(disable: 4296)

VP_STATUS NVPFlipVideoPort(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pCmdObj)
{
    PNVP_CONTEXT pVPEContext;
    PNVP_SURFACE_INFO pSurface;
    int i;

    DBGPRINT(DBGTRACE, "NVPFlipVideoPort()");

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[pCmdObj->dwPortId]);

    // NVP_UNDEF tells us to lookup the surface in list
    if (pCmdObj->dwSurfaceId == NVP_UNDEF)
    {
        for (i = 0; i < pVPEContext->nNumVidSurf; i++)
        {
                //////////
                ///// I assume nothing changing in the surfaces values
                pSurface = &(pVPEContext->aVideoSurfaceInfo[i]);
                if (pSurface->pVidMem == pCmdObj->pSurfVidMem)
                {
                    // clear/reset sync event
                    osNVPClearEvent((PVOID)pVPEContext->hNVPSyncEvent[0]);
                    // found target surface, so update index of video surface we're currently capturing into
                    pVPEContext->nVidSurfIdx = i;
                    return NO_ERROR;
                }
        }
    }
    else
    {
        // check for weird values !
        if ((pCmdObj->dwSurfaceId >= 0) &&
            (pCmdObj->dwSurfaceId < (DWORD)pVPEContext->nNumVidSurf) &&
            (pVPEContext->aVideoSurfaceInfo[pCmdObj->dwSurfaceId].pVidMem == pCmdObj->pSurfVidMem))
        {
                // clear/reset sync event
                osNVPClearEvent((PVOID)pVPEContext->hNVPSyncEvent[0]);
                // update index of video surface we're currently capturing into
                pVPEContext->nVidSurfIdx = (int)(pCmdObj->dwSurfaceId);
                return NO_ERROR;
        }
    }

    ////// if we got here, it means the target surface wasn't found in list...
    // so add it at the end of the list
    i = pVPEContext->nNumVidSurf;
    pSurface = &(pVPEContext->aVideoSurfaceInfo[i]);
    (pVPEContext->nNumVidSurf)++;
    pSurface->dwStartLine = pCmdObj->dwImageStartLine;      // startline for video
    pSurface->pVidMem = pCmdObj->pSurfVidMem;               // Flat pointer to the surface
    pSurface->dwOffset = pCmdObj->dwSurfOffset;             // Offset from the base of the frame buffer
    pSurface->dwPitch = pCmdObj->dwSurfPitch;
    pSurface->dwHeight = pCmdObj->dwInHeight;
    pSurface->dwWidth = pCmdObj->dwInWidth;                 // Ignored for VBI
    pSurface->dwPreScaleSizeX = pCmdObj->dwPreWidth;        // Ignored for VBI.. prescale size width
    pSurface->dwPreScaleSizeY = pCmdObj->dwPreHeight;       // Ignored for VBI.. prescale size height
    pSurface->nNextSurfaceIdx = 0;                          // the next surface idx in the chain
    pSurface->bIsFirstFieldCaptured = FALSE;                // reset value
    // clear/reset sync event
    osNVPClearEvent((PVOID)pVPEContext->hNVPSyncEvent[0]);
    // flip to surface !
    pVPEContext->nVidSurfIdx = i;                           

    return NO_ERROR;
}

#pragma warning(default: 4296)

/*
==============================================================================
    
    NVP3ProgVideoField

    Description:    programs the vp to capture the specified video field

    Date:           Feb 19, 1999
    
    Author:         H. Azar

==============================================================================
*/
void NVP3ProgVideoField(
    PNVP_CONTEXT pVPEContext,
    PNVP_SURFACE_INFO pSurface,
    int nFieldId,
    U032 dwSurfPitch,
    U032 dwInterleaveOffset,
    BOOL bGrab)
{
#ifdef NVP_NV3
    Nv03ChannelPio *pNVPChannel;
    NvNotification *avpNotifiers;
    int nFreeCount;
    NVOS09_PARAMETERS ntfParam;

    pSurface->dwFieldType = (ULONG)nFieldId;

    // get a pointer to the appropriate VPE context
    pNVPChannel = (Nv03ChannelPio *)(pVPEContext->pNVPChannel) ;    // get a copy of the pio channel pointer

#ifdef _NV_DPC
    //CHECK_FREE_COUNT(pNVPChannel,6*4);
    ntfParam.hObjectParent = pVPEContext->dwDeviceId;
    nFreeCount = pVPEContext->nFreeCount;
    while (nFreeCount < 7*4)
    {
        //RmInterrupt(&ntfParam);
        nFreeCount = NvGetFreeCount(pNVPChannel, 0);
    }           
    pVPEContext->nFreeCount = nFreeCount - (7*4);
#endif

    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);
    // program the VP to grab in the next surface
    avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(nFieldId)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

    pNVPChannel->subchannel[0].SetObject = NVP_EXTERNAL_DECODER_OBJECT;
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetImage[nFieldId].sizeIn  = (pSurface->dwWidth) | (pSurface->dwHeight << 16);
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetImage[nFieldId].sizeOut = (pSurface->dwPreScaleSizeX) | (pSurface->dwPreScaleSizeY << 16);

    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetImageStartLine = pSurface->dwStartLine;
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetImage[nFieldId].offset  = pSurface->dwOffset + dwInterleaveOffset;

    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetImage[nFieldId].format =
            (dwSurfPitch & 0xFFFF)  |
            ((((nFieldId == NVP_FIELD_EVEN) ? NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD : NV04D_SET_IMAGE_FORMAT_FIELD_ODD_FIELD) << 16) & 0xFF0000) |
            ((NV04D_SET_IMAGE_FORMAT_NOTIFY_WRITE_THEN_AWAKEN << 24 ) & 0xFF000000);

#endif // NVP_NV3
}

/*
==============================================================================
    
    NVP3ProgVBIField

    Description:    programs the vp to capture the specified VBI field.
                    PIO channel version.

    Date:           Feb 19, 1999
    
    Author:         H. Azar

==============================================================================
*/
void NVP3ProgVBIField(
    PNVP_CONTEXT pVPEContext,
    PNVP_SURFACE_INFO pSurface,
    int nFieldId)
{
#ifdef NVP_NV3
    Nv03ChannelPio *pNVPChannel;
    int nFreeCount;
    NVOS09_PARAMETERS ntfParam;
    NvNotification *avpNotifiers;

    pSurface->dwFieldType = (ULONG)nFieldId;

    // get a pointer to the appropriate VPE context
    pNVPChannel = (Nv03ChannelPio *)(pVPEContext->pNVPChannel) ;    // get a copy of the pio channel pointer

#ifdef _NV_DPC
    //CHECK_FREE_COUNT(pNVPChannel,5*4);
    ntfParam.hObjectParent = pVPEContext->dwDeviceId;
    nFreeCount = pVPEContext->nFreeCount;
    while (nFreeCount < 5*4)
    {
        //RmInterrupt(&ntfParam);
        nFreeCount = NvGetFreeCount(pNVPChannel, 0);
    }
    pVPEContext->nFreeCount = nFreeCount - (5*4);
#endif
    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);
    avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(nFieldId)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

    pNVPChannel->subchannel[0].SetObject = NVP_EXTERNAL_DECODER_OBJECT;
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetVbi[nFieldId].size  = (pSurface->dwStartLine) | (pSurface->dwHeight << 16);
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetVbi[nFieldId].offset  = pSurface->dwOffset;
    pNVPChannel->subchannel[0].nv03ExternalVideoDecoder.SetVbi[nFieldId].format =
            (pSurface->dwPitch & 0xFFFF)  |
            ((((nFieldId == NVP_FIELD_EVEN) ? NV04D_SET_VBI_FORMAT_FIELD_EVEN_FIELD : NV04D_SET_VBI_FORMAT_FIELD_ODD_FIELD) << 16) & 0xFF0000) |
            ((NV04D_SET_VBI_FORMAT_NOTIFY_WRITE_THEN_AWAKEN << 24) & 0xFF000000);

#endif // NVP_NV3
}

/*
==============================================================================
    
    NVPProgVideoField

    Description:    programs the vp to capture the specified video field

    Date:           Apr 19, 1999
    
    Author:         H. Azar

==============================================================================
*/
void NVPProgVideoField(
    PNVP_CONTEXT pVPEContext,
    PNVP_SURFACE_INFO pSurface,
    int nFieldId,
    U032 dwSurfPitch,
    U032 dwInterleaveOffset,
    BOOL bGrab)
{
    Nv4ControlDma *pDmaChannel;
    NvNotification *avpNotifiers;
    U032 dwFormat;
    U032 nvDmaCount;

    DBGPRINT(DBGTRACE, "NVPProgVideoField");

    //NVPFlushChannel(pVPEContext);
    // NV3 specific code
    if (NVP_GET_CHIP_VER(pVPEContext->dwChipId) <= NVP_CHIP_VER_03)
    {
        NVP3ProgVideoField(pVPEContext, pSurface, nFieldId, dwSurfPitch, dwInterleaveOffset, bGrab);
        return;
    }

#ifdef _NVP_DTRACE
    DTRACE(0xdead0000);
    DTRACE_CYCLE_DELTA();
    DTRACE(g_nIntCounter);
    DTRACE(nFieldId);
    g_nIntCounter++;
#endif

    // update type of field we're capturing
    pSurface->dwFieldType = (ULONG)nFieldId;

    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;
    NVP_DMAPUSH_CHECK_FREE_COUNT(12);

    // reset notifier
    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);

#ifdef _NVP_TEST_NEXT_FIELD
    if (nFieldId != g_nNextFieldId)
        _asm int 3
    g_nNextFieldId = nFieldId ^ 1;
    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);
    dwFormat = avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(nFieldId)].status;
    dwFormat = avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(g_nNextFieldId)].status;
#endif

    avpNotifiers[NV04D_NOTIFIERS_SET_IMAGE_FORMAT_NOTIFY(nFieldId)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

    // program the VP to grab in the next surface
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_OBJECT , NVP_EXTERNAL_DECODER_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_IMAGE_SIZE_IN(nFieldId), ((pSurface->dwWidth) | (pSurface->dwHeight << 16)));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_IMAGE_SIZE_OUT(nFieldId), ((pSurface->dwPreScaleSizeX) | (pSurface->dwPreScaleSizeY << 16)));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_IMAGE_START_LINE, pSurface->dwStartLine);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_IMAGE_OFFSET(nFieldId), (pSurface->dwOffset + dwInterleaveOffset));
    dwFormat = ((dwSurfPitch & 0xFFFF) |
                ((((nFieldId == NVP_FIELD_EVEN) ? NV04D_SET_IMAGE_FORMAT_FIELD_EVEN_FIELD : NV04D_SET_IMAGE_FORMAT_FIELD_ODD_FIELD) << 16) & 0xFF0000) |
                ((NV04D_SET_IMAGE_FORMAT_NOTIFY_WRITE_THEN_AWAKEN << 24 ) & 0xFF000000));

    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_IMAGE_FORMAT(nFieldId), dwFormat);

    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;


}

/*
==============================================================================
    
    NVPProgVBIField

    Description:    programs the vp to capture the specified VBI field

    Date:           Apr 22, 1999
    
    Author:         H. Azar

==============================================================================
*/
void NVPProgVBIField(
    PNVP_CONTEXT pVPEContext,
    PNVP_SURFACE_INFO pSurface,
    int nFieldId)
{
    Nv4ControlDma *pDmaChannel;
    NvNotification *avpNotifiers;
    U032 dwFormat;
    U032 nvDmaCount;

    // NV3 specific code
    if (NVP_GET_CHIP_VER(pVPEContext->dwChipId) <= NVP_CHIP_VER_03)
    {
        NVP3ProgVBIField(pVPEContext, pSurface, nFieldId);
        return;
    }

    // update type of field we're capturing
    pSurface->dwFieldType = (ULONG)nFieldId;

    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;
    NVP_DMAPUSH_CHECK_FREE_COUNT(8);

    // reset notifier
    avpNotifiers = (NvNotification *) (pVPEContext->avpNotifiers);
    avpNotifiers[NV04D_NOTIFIERS_SET_VBI_FORMAT_NOTIFY(nFieldId)].status = NV04D_NOTIFICATION_STATUS_IN_PROGRESS;

    // program the VP to grab in the next surface
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_OBJECT , NVP_EXTERNAL_DECODER_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_VBI_SIZE(nFieldId), ((pSurface->dwStartLine) | (pSurface->dwHeight << 16)));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_VBI_OFFSET(nFieldId), pSurface->dwOffset);
    dwFormat = (pSurface->dwPitch & 0xFFFF)  |
            ((((nFieldId == NVP_FIELD_EVEN) ? NV04D_SET_VBI_FORMAT_FIELD_EVEN_FIELD : NV04D_SET_VBI_FORMAT_FIELD_ODD_FIELD) << 16) & 0xFF0000) |
            ((NV04D_SET_VBI_FORMAT_NOTIFY_WRITE_THEN_AWAKEN << 24) & 0xFF000000);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_VBI_FORMAT(nFieldId), dwFormat);

    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;
}


/*
==============================================================================
    
    NVPProcessState

    Description:    programs the video port grab in a surface and determines
                    which surface is next.

    Date:           Feb 03, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPProcessState(
    PHW_DEVICE_EXTENSION pHwDevExt,
    int nIndex,
    BOOL bVBIField)
{
    PNVP_CONTEXT pVPEContext;
    PNVP_SURFACE_INFO pSurface;
    int nFieldId = nIndex & 1;
    BOOL bIRQCallback;

    DBGPRINT(DBGTRACE, "NVPProcessState()");

    // get a pointer to the appropriate VPE context
    pVPEContext = &(pHwDevExt->avpContexts[0]);

    // setting nIndex to greater than 1 (nField | any_value) means that the caller is NVPStart or NVPUpdate
    // so first time initialize the dwOverlayIndex to match up with the nFieldId (notifier)
    if ((pVPEContext->nvpOverlay.dwOverlayBufferIndex == NVP_UNDEF) && (nIndex <= 1))
       pVPEContext->nvpOverlay.dwOverlayBufferIndex = nFieldId;

    // calling IRQCallback must be done only at the end of video field or at end of vbi when
    // capturing vbi only
    bIRQCallback = (!bVBIField) || (pVPEContext->nNumVidSurf == 0);
    // any transfer request of already captured vbi/video data ?!
//  if (bIRQCallback)
//      NVPCheckForLaunch(pVPEContext, NULL);


    if (pVPEContext->dwVPState != NVP_STATE_STOPPED)
    {
        if (pVPEContext->dwVPState & NVP_STATE_STARTED_VIDEO)
        {
            // program VBI here. This way VBI is synchronized with the video thus we eliminate the need
            // for a seperate VBI notifications routine unless we're streaming VBI only
            if (!bVBIField)
            {
                if (pVPEContext->dwVPState & NVP_STATE_STARTED_VBI)
                {
                    pSurface = &(pVPEContext->aVBISurfaceInfo[pVPEContext->nNextVBISurfId]);
                    // program the VP to grab in the next surface
                    NVPProgVBIField(pVPEContext, pSurface, nFieldId);

                    // next surface in the chain !
                    pVPEContext->nVBISurfIdx = pVPEContext->nNextVBISurfId;
                    pVPEContext->nNextVBISurfId = pSurface->nNextSurfaceIdx;
                }

                if (pVPEContext->dwVPConFlags & DDVPCONNECT_INTERLACED)
                {
                    if (pVPEContext->dwVPFlags & DDVP_INTERLEAVE)
                    {
                        if (pVPEContext->nvpOverlay.dwOverlayMode & NV_VFM_FORMAT_BOBFROMINTERLEAVE)
                        {
                            // Bob from interleaved surfaces (skip even/odd included here too)
                            DoBobFromInterleaved(pVPEContext, nFieldId);
                        }
                        else
                        {
                            // Weave
                            DoWeave(pVPEContext, nFieldId);
                        }
                    }
                    else
                    {
#ifdef FORCE_INTERLEAVE_MEM
                        // Bob from interleaved surfaces (skip even/odd included here too)
                        DoBobFromInterleaved(pVPEContext, nFieldId);
#else
                        // Bob from non-interleaved (separate) surfaces (skip even/odd included here too)
                        DoBob(pVPEContext, nFieldId);
#endif
                    }
                }
                else
                {
                    // Progressive
                    DoProgressive(pVPEContext, nFieldId);
                }
            }
        }
        else if (pVPEContext->dwVPState & NVP_STATE_STARTED_VBI)
        {
            pSurface = &(pVPEContext->aVBISurfaceInfo[pVPEContext->nNextVBISurfId]);
            // program the VP to grab in the next surface
            NVPProgVBIField(pVPEContext, pSurface, nFieldId);

            // next surface in the chain !
            pVPEContext->nVBISurfIdx = pVPEContext->nNextVBISurfId;
            pVPEContext->nNextVBISurfId = pSurface->nNextSurfaceIdx;
        }
    }

    //
    // calling IRQCallback must be done only at the end of video field or at end of vbi when
    // capturing vbi only
    if (bIRQCallback && (pHwDevExt->dwIRQSources & DDIRQ_VPORT0_VSYNC))
    {
        kmvtNVPCallbackDxApi((PVOID)pHwDevExt, DDIRQ_VPORT0_VSYNC);
#if 0
        // can we pass on the interrupt !
        if ((pHwDevExt->IRQCallback) && (pHwDevExt->dwIRQSources & DDIRQ_VPORT0_VSYNC))
        {
            PDX_IRQDATA pIRQData;
            pIRQData = (PDX_IRQDATA)(pHwDevExt->pIRQData);
            pIRQData->dwIrqFlags |= DDIRQ_VPORT0_VSYNC;
            ((PDX_IRQCALLBACK)(pHwDevExt->IRQCallback))(pIRQData);
        }
#endif
    }

    return NO_ERROR;
}

/*
==============================================================================
    
    NVPEProcessCommand

    Description:    dispatches commands sent via IOCTL to the appropriate
                    NVP function

    Date:           Feb 03, 1999
    
    Author:         H. Azar

==============================================================================
*/
#pragma warning(disable: 4296)

VP_STATUS NVPEProcessCommand(
    PVOID pHwDeviceExtension,
    PVIDEO_REQUEST_PACKET pRequestPacket)
{
    U032 status = 0;
    NVP_CMD_OBJECT *pnvCmdObj, *pnvCmdObjOut;
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;

    DBGPRINT(DBGTRACE, "NVPEProcessCommand()");

    pnvCmdObj = (NVP_CMD_OBJECT *)(pRequestPacket->InputBuffer);
    pnvCmdObjOut = (NVP_CMD_OBJECT *)(pRequestPacket->OutputBuffer);
    // some arguments validation
    if ((pnvCmdObj->dwPortId < 0) || (pnvCmdObj->dwPortId >= NVP_MAX_VPORTS))
        return ERROR_INVALID_PARAMETER;

    // dispatch call
    switch (pnvCmdObj->dwSrvId)
    {
        case NVP_SRV_INITIALIZE:
            status = NVPInitialize(pHwDevExt, pnvCmdObj);
            break;
        case NVP_SRV_UNINITIALIZE:
            status = NVPUninitialize(pHwDevExt, pnvCmdObj);
            break;
        case NVP_SRV_START_VIDEO:
            status = NVPStart(pHwDevExt, pnvCmdObj);
            break;
        case NVP_SRV_STOP_VIDEO:
            status = NVPStop(pHwDevExt, pnvCmdObj);
            break;
        case NVP_SRV_UPDATE_VIDEO:
            status = NVPUpdate(pHwDevExt, pnvCmdObj);
            break;
        case NVP_SRV_REG_SURFACE:
            status = NVPRegSurface(pHwDevExt, pnvCmdObj);
            break;
        case NVP_SRV_UPDATE_OVERLAY:
            status = NVPUpdateOverlay(pHwDevExt, pnvCmdObj);
            break;
        case NVP_SRV_FLIP_VIDEOPORT:
            status = NVPFlipVideoPort(pHwDevExt, pnvCmdObj);
            break;
        case NVP_SRV_IS_VPE_ENABLED:
            status = NVPIsVPEEnabled(pHwDevExt, pnvCmdObj, pnvCmdObjOut);
#ifdef _WIN32_WINNT
                  pRequestPacket->StatusBlock->Information = sizeof(NVP_CMD_OBJECT);
#endif
            break;
#ifdef VPEFSMIRROR    
        case NVP_SRV_ENABLE_FSMIRROR:
            status = NVPFsMirrorEnable(pHwDevExt, pnvCmdObj);
            break;
        case NVP_SRV_REG_FSMIRROR_SURFACE:
            status = NVPFsRegSurface(pHwDevExt, pnvCmdObj);
            break;
        case NVP_SRV_DISABLE_FSMIRROR:
            status = NVPFsMirrorDisable(pHwDevExt, pnvCmdObj);
            break;
#endif
        default:
            DBGPRINT(DBGERROR, "invalid VPE command !");
            return ERROR_INVALID_PARAMETER;
    }

    return status;
}

#pragma warning(default: 4296)

#ifdef _WIN32_WINNT // {

/*
==============================================================================
    
    VideoField0Notify

    Description:    callback function. Called after a video buffer gets filled

    Date:           Feb 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
void VideoField0Notify(PVOID pHwDeviceExtension)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;

    NVPProcessState(pHwDevExt, 0, FALSE);
}

/*
==============================================================================
    
    VideoField1Notify

    Description:    callback function. Called after a vbi buffer gets filled

    Date:           Feb 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
void VideoField1Notify(PVOID pHwDeviceExtension)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;

    NVPProcessState(pHwDevExt, 1, FALSE);
}


/*
==============================================================================
    
    VBIField0Notify

    Description:    callback function. Called after a vbi buffer gets filled

    Date:           Feb 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
void VBIField0Notify(PVOID pHwDeviceExtension)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;

    NVPProcessState(pHwDevExt, 0, TRUE);
}

/*
==============================================================================
    
    VBIField1Notify

    Description:    callback function. Called after a vbi buffer gets filled

    Date:           Feb 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
void VBIField1Notify(PVOID pHwDeviceExtension)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pHwDeviceExtension;

    NVPProcessState(pHwDevExt, 1, TRUE);
}

#endif // _WIN32_WINNT }

/*
==============================================================================
    
    NVPScheduleOverlayFlip

    Description:    program overlay object to flip surface

    Date:           Feb 25, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPScheduleOverlayFlip(
    PNVP_CONTEXT pVPEContext,
    PNVP_SURFACE_INFO pSurface,
    int  nImageId,
    U032 dwSrcPitch,
    U032 dwMoveDownOffset)
{
    U032 dwFlags = 0;

    DBGPRINT(DBGTRACE, "NVPScheduleOverlayFlip");

#ifdef VPEFSMIRROR
    	// [XW:10/15/2000] Added so we don't skip the FSMirror for FourCC blits of VPE Port
	    
    if ( (!g_bFlip) && (!pVPEContext->dwFsMirrorOn) )
        return NO_ERROR;

#endif

    if (!g_bFlip)
        return NO_ERROR;

    DBGPRINT(DBGTRACE, "NVPScheduleOverlayFlip()");

    dwFlags = VPP_PRESCALE;

#ifndef FORCE_INTERLEAVE_MEM
    if (pVPEContext->dwVPFlags & DDVP_INTERLEAVE)
       dwFlags |= VPP_INTERLEAVED;
    else
       dwFlags |= VPP_BOB;
#else
	dwFlags |= VPP_INTERLEAVED;
	if (!(pVPEContext->dwVPFlags & DDVP_INTERLEAVE)){
		dwFlags |= VPP_BOB;
	}
#endif
    if (pVPEContext->nvpOverlay.dwOverlayMode & NV_VFM_FORMAT_BOBFROMINTERLEAVE)
       dwFlags |= VPP_BOB;

    if (nImageId == NVP_FIELD_ODD)
       dwFlags |= VPP_ODD;
    else if (nImageId == NVP_FIELD_EVEN)
       dwFlags |= VPP_EVEN;

    NVPPrescaleAndFlip(
        pVPEContext,
        &(pVPEContext->nvpOverlay),
        pSurface->dwOffset,
        pSurface->dwPitch, // source pitch
        nImageId,
        dwFlags);          // VPP flags

    return NO_ERROR;
}


/*
==============================================================================
    
    DoProgressive

    Description:    displays progressive video

    Date:           Mar 08, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS DoProgressive(
    PNVP_CONTEXT pVPEContext,
    int nFieldId)
{
    PNVP_SURFACE_INFO pSurface;
    int nTargetSurfId = pVPEContext->nVidSurfIdx;
    U032 dwSrcPitch;
    int nImage = nFieldId;

    pSurface = &(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx]);
        // if autoflip, CS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
    if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
        nTargetSurfId = pSurface->nNextSurfaceIdx;

    dwSrcPitch = pSurface->dwPitch;

    // Overlay Image n from CS
    NVPScheduleOverlayFlip(pVPEContext, pSurface, nImage, dwSrcPitch, 0);

    pVPEContext->nVidSurfIdx = nTargetSurfId;

    // Grab Image n in TS
    pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfId]);
    NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, TRUE);

    return NO_ERROR;
}

/*
==============================================================================
    
    DoWeave

    Description:    use weave method to display video

    Date:           Mar 08, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS DoWeave(
    PNVP_CONTEXT pVPEContext,
    int nFieldId)
{
    PNVP_SURFACE_INFO pSurface;
    int nTargetSurfId = pVPEContext->nVidSurfIdx;
    U032 dwSrcPitch, dwOffset = 0;
    int nImage = nFieldId;

    pSurface = &(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx]);
    // if autoflip, TS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
    if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
        nTargetSurfId = pSurface->nNextSurfaceIdx;
    // double pitch (interleaved data)
    dwSrcPitch = 2*(pSurface->dwPitch);

    if (nFieldId == NVP_FIELD_ODD)
    {
        // Invert Polarity ?!
        if (pVPEContext->dwVPConFlags & DDVPCONNECT_INVERTPOLARITY)
        {
            // change vp grab offset one line down
            dwOffset = pSurface->dwPitch;
            // Shift Down Image destination by 1 line !
            /////// the following code uses an "undocumented feature" in the RM to achieve the 1 line shift down effect !
            //dwSrcPitch = (dwSrcPitch & 0xFFFFFFFC) | 0x2;
        }
    }
    else
    {
        // not Invert Polarity ?!
        if (!(pVPEContext->dwVPConFlags & DDVPCONNECT_INVERTPOLARITY))
        {
            // offset vp grab one line down
            dwOffset = pSurface->dwPitch;

            // Shift Down Image destination by 1 line !
            /////// the following code uses an "undocumented feature" in the RM to achieve the 1 line shift down effect !
            //dwSrcPitch = (dwSrcPitch & 0xFFFFFFFC) | 0x2;
        }
    }

    // whole frame captured ?!
    if (pSurface->bIsFirstFieldCaptured)
    {
        // Overlay Image n from CS
        NVPScheduleOverlayFlip(pVPEContext, pSurface, NVP_UNDEF, pSurface->dwPitch, 0);
        // NVPScheduleOverlayFlip(pVPEContext, pSurface, nImage ^ 1, dwSrcPitch, dwOffset);

        pSurface->bIsFirstFieldCaptured = FALSE;    // reset flag (for the next time)

        // if autoflip, CS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
        if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
            pVPEContext->nVidSurfIdx = pSurface->nNextSurfaceIdx;
    }
    else
        pSurface->bIsFirstFieldCaptured = TRUE; // set flag (for the next time)

    // Grab Image n in TS
    pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfId]);
    NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, dwOffset, TRUE);

    return NO_ERROR;
}

/*
==============================================================================
    
    DoBobFromInterleaved

    Description:    use bob from interleaved input to display video

    Date:           Mar 08, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS DoBobFromInterleaved(
    PNVP_CONTEXT pVPEContext,
    int nFieldId)
{
    PNVP_SURFACE_INFO pSurface;
    int nTargetSurfId = pVPEContext->nVidSurfIdx;
    U032 dwSrcPitch, dwOffset = 0;
    int nImage = nFieldId;

    DBGPRINT(DBGTRACE, "DoBobFromInterleaved");

    // Current Surface (CS)
    pSurface = &(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx]);
    // NewPitch = OldPitch
    dwSrcPitch = pSurface->dwPitch; // interleaved
    // if autoflip, TS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
    if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
        nTargetSurfId = pSurface->nNextSurfaceIdx;

#if 0
    if (pVPEContext->dwVPFlags & DDVP_SKIPEVENFIELDS)
    {
        if (nFieldId == NVP_FIELD_EVEN)
        {
            // VP minimum programming to issue an interrupt at end of field !
            NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, FALSE);
            return NO_ERROR;
        }
    }
    else if (pVPEContext->dwVPFlags & DDVP_SKIPODDFIELDS)
    {
        if (nFieldId == NVP_FIELD_ODD)
        {
            // VP minimum programming to issue an interrupt at end of field !
            NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, FALSE);
            return NO_ERROR;
        }
    }
    else
#endif
    {
        // double pitch (interleaved data)
        dwSrcPitch = 2*dwSrcPitch;
        if (nFieldId == NVP_FIELD_ODD)
        {
            // Invert Polarity ?!
            if (pVPEContext->dwVPConFlags & DDVPCONNECT_INVERTPOLARITY)
            {
                // change vp grab offset one line down
                dwOffset = pSurface->dwPitch;
            }
        }
        else
        {
            // Invert Polarity ?!
            // for the even field (bottom) shift down overlay destination if invert polarity flag is not set !
            if (!(pVPEContext->dwVPConFlags & DDVPCONNECT_INVERTPOLARITY))
            {
                dwOffset = pSurface->dwPitch;

            }
        }

    }

    if (!((pVPEContext->dwVPFlags & DDVP_SKIPEVENFIELDS) && (nFieldId == NVP_FIELD_EVEN)) &&
        !((pVPEContext->dwVPFlags & DDVP_SKIPODDFIELDS) && (nFieldId == NVP_FIELD_ODD)))
    {
        // Overlay Image n from CS
        NVPScheduleOverlayFlip(pVPEContext, pSurface, nImage, dwSrcPitch, dwOffset);
        // NVPScheduleOverlayFlip(pVPEContext, pSurface, nImage ^ 1, dwSrcPitch, dwOffset);
    }

    // whole frame captured ?!
    if (pSurface->bIsFirstFieldCaptured)
    {
        pSurface->bIsFirstFieldCaptured = FALSE;    // reset (for the next time)
        // if autoflip, CS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
        if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
            pVPEContext->nVidSurfIdx = pSurface->nNextSurfaceIdx;
    }
    else
        pSurface->bIsFirstFieldCaptured = TRUE; // for the next time

    // Grab Image n in TS
    pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfId]);
    NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, dwOffset, TRUE);

    return NO_ERROR;
}

/*
==============================================================================
    
    DoBob

    Description:    use bob from non-interleaved input to display video

    Date:           Mar 08, 1999
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS DoBob(
    PNVP_CONTEXT pVPEContext,
    int nFieldId)
{
    PNVP_SURFACE_INFO pSurface;
    int nTargetSurfId = pVPEContext->nVidSurfIdx;
    U032 dwSrcPitch;
    int nImage = nFieldId;

    pSurface = &(pVPEContext->aVideoSurfaceInfo[pVPEContext->nVidSurfIdx]);
    dwSrcPitch = pSurface->dwPitch;
#if 0
    if (pVPEContext->dwVPFlags & DDVP_SKIPEVENFIELDS)
    {
        if (nFieldId == NVP_FIELD_EVEN)
        {
            // VP minimum programming to issue an interrupt at end of field !
            // TS = next2(CS)
            nTargetSurfId = pVPEContext->aVideoSurfaceInfo[pSurface->nNextSurfaceIdx].nNextSurfaceIdx;
            pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfaceId]);
            NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, TRUE);
            return NO_ERROR;
        }
        else
            // TS = next(CS)
            nTargetSurfId = pSurface->nNextSurfaceIdx;
    }
    else if (pVPEContext->dwVPFlags & DDVP_SKIPODDFIELDS)
    {
        if (nFieldId == NVP_FIELD_ODD)
        {
            // VP minimum programming to issue an interrupt at end of field !
            nTargetSurfId = pVPEContext->aVideoSurfaceInfo[pSurface->nNextSurfaceIdx].nNextSurfaceIdx;
            pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfaceId]);
            NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, TRUE);
            return NO_ERROR;
        }
        else
            // TS = next(CS)
            nTargetSurfId = pSurface->nNextSurfaceIdx;
    }
    else
#endif
    {
        // if autoflip, TS = next2(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
        if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
            nTargetSurfId = pVPEContext->aVideoSurfaceInfo[pSurface->nNextSurfaceIdx].nNextSurfaceIdx;
    }

    if (!((pVPEContext->dwVPFlags & DDVP_SKIPEVENFIELDS) && (nFieldId == NVP_FIELD_EVEN)) &&
        !((pVPEContext->dwVPFlags & DDVP_SKIPODDFIELDS) && (nFieldId == NVP_FIELD_ODD)))
    {
        // Overlay Image n from CS
        NVPScheduleOverlayFlip(pVPEContext, pSurface, nImage, dwSrcPitch, 0);
    }

    // if autoflip, CS = next(CS)    (otherwise stay within the same surface until VideoPortFlip is called !!!)
    if (pVPEContext->dwVPFlags & DDVP_AUTOFLIP)
        pVPEContext->nVidSurfIdx = pSurface->nNextSurfaceIdx;

    // Grab Image n in TS
    pSurface = &(pVPEContext->aVideoSurfaceInfo[nTargetSurfId]);
    NVPProgVideoField(pVPEContext, pSurface, nImage, dwSrcPitch, 0, TRUE);

    return NO_ERROR;
}

/*
==============================================================================
    
    NVPTransfer

    Description:    transfers content of video or vbi field to specified
                    buffer (usually system memory buffer)

    Date:           Mar 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVPTransfer(
    PVOID pContext,
    PNVP_DDTRANSFERININFO  pInInfo,
    PNVP_DDTRANSFEROUTINFO pOutInfo)
{
    PNVP_CONTEXT pVPEContext;
    PNVP_SURFACE_INFO pSurface = NULL;
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pContext;

    DBGPRINT(DBGTRACE, "NVPTransfer()");

    // get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
    pVPEContext = &(pHwDevExt->avpContexts[0]);

    if (pInInfo->dwTransferFlags & DDTRANSFER_NONLOCALVIDMEM)
        return DXERR_UNSUPPORTED;
    
    // we DO handle cancels for transfer buffers not "in progress" !
    if (pInInfo->dwTransferFlags & DDTRANSFER_CANCEL){
		pVPEContext->ulCap=FALSE;
        return NVPCancelTransfer(pVPEContext, pInInfo);
	}

    // determine polarity, TRUE for Even, FALSE for Odd !
    NVPGetSurface(pVPEContext, (U032)(((DDSURFACEDATA*) (pInInfo->lpSurfaceData))->fpLockPtr), &pSurface);
      if (pSurface == NULL)
        return DXERR_GENERIC;
    if (pSurface->dwFieldType == NVP_FIELD_ODD)
        pOutInfo->dwBufferPolarity = FALSE;
    else
        pOutInfo->dwBufferPolarity = TRUE;
    
    // queue request
    if (NVPAddTransfer(pVPEContext, pInInfo, !(pOutInfo->dwBufferPolarity)))
        return DXERR_GENERIC;
#if 0
    if (pVPEContext->nCurTransferIdx != -1)
    {
        pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);
        // Recreate context dma for MDL
        NVPRecreateContext(pVPEContext, (PMDL)(pTransfer->pDestMDL));
    }
#endif

      // launch a transfer here
    NVPCheckForLaunch(pVPEContext, pSurface);

    return DX_OK;
}

/*
==============================================================================
    
    NVPGetSurface

    Description:    looks for surface that has the specified pointer. pSurface
                    is set to NULL if no surface is found.


    Date:           Mar 18, 1999
    
    Author:         H. Azar

==============================================================================
*/
BOOL NVPGetSurface(
    PNVP_CONTEXT pVPEContext,
    U032 dwSurfPtr,
    PNVP_SURFACE_INFO *pSurface)
{
    int i;

    DBGPRINT(DBGTRACE, "NVPGetSurface()");

    *pSurface = NULL;

    // why VBI first ! well, this is most probably what we're transfering... kinda optimization !
    for (i = 0; i < pVPEContext->nNumVBISurf; i++)
    {
        if (pVPEContext->aVBISurfaceInfo[i].pVidMem == dwSurfPtr)
        {
            *pSurface = &(pVPEContext->aVBISurfaceInfo[i]);
            return FALSE;   // FALSE means VBI
        }
    }

    for (i = 0; i < pVPEContext->nNumVidSurf; i++)
    {
        if (pVPEContext->aVideoSurfaceInfo[i].pVidMem == dwSurfPtr)
        {
            *pSurface = &(pVPEContext->aVideoSurfaceInfo[i]);
            return TRUE;    // TRUE means VIDEO
        }
    }

    return FALSE;   // return something ! (pSurface is NULL anyway)
}

/*
==============================================================================
    
    NVP3RecreateContext

    Description:    if necessary, create context DMA for mem to mem object
                    PIO channel version.

    Date:           Mar 18, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVP3RecreateContext(
    PNVP_CONTEXT pVPEContext,
    PMDL pDestMDL)
{
#ifdef NVP_NV3
    Nv03ChannelPio *pChan;
    NVOS09_PARAMETERS ntfParam;
    U032 dwStart;
    U032 dwLimit;
    int nFreeCount;

    pChan = (Nv03ChannelPio *)(pVPEContext->pNVPChannel);

    // first check to see if we can reuse the OLD context... since context creation is expensive.
    dwStart = (U032)(pDestMDL->lpMappedSystemVa);
    dwLimit = (pDestMDL->ByteCount) - 1;
    
    if (pVPEContext->bxfContextCreated)
    {
        // do not recreate if same context
        if ((dwStart == pVPEContext->dwTransferCtxStart) && (dwLimit == pVPEContext->dwTransferCtxSize))
            return DX_OK;
    }

#ifdef _NV_DPC
    // FIFO free count check up
    ntfParam.hObjectParent = pVPEContext->dwDeviceId;
    nFreeCount = pVPEContext->nFreeCount;
    while (nFreeCount < 4*4)
    {
        //RmInterrupt(&ntfParam);
        nFreeCount = NvGetFreeCount(pChan, 5);
    }
    pVPEContext->nFreeCount = nFreeCount - (4*4);
#endif

    // free the old context first
    if (pVPEContext->bxfContextCreated)
    {
        pChan->subchannel[5].SetObject = NVP_M2M_OBJECT;
        pChan->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferOut = 0;
#ifdef _NV_DPC
        // Do a software method flush here
        ntfParam.hObjectParent = pVPEContext->dwDeviceId;
        nFreeCount = pVPEContext->nFreeCount;
        while (nFreeCount < NV06A_FIFO_GUARANTEED_SIZE)
        {
            //RmInterrupt(&ntfParam);
            nFreeCount = NvGetFreeCount(pChan, 5);
        }
        pVPEContext->nFreeCount = nFreeCount - NV06A_FIFO_GUARANTEED_SIZE;
#endif
        RmFree(pVPEContext->hClient, pVPEContext->dwDeviceId, NVP_M2M_CONTEXT);

        pVPEContext->bxfContextCreated = FALSE;
    }
    
    // setup Notify Context
    if (RmAllocContextDma(
                    pVPEContext->hClient,
                    NVP_M2M_CONTEXT,
                    NV01_CONTEXT_DMA_FROM_MEMORY,
                    0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                    0, // selector
                    dwStart,
                    dwLimit) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
    {
        DBGPRINT(DBGERROR, "cannot allocate mem 2 mem context");
        return DXERR_GENERIC;
    }

    // set context of M2M object
    pChan->subchannel[5].SetObject  = NVP_M2M_OBJECT;
    pChan->subchannel[5].nv03MemoryToMemoryFormat.SetContextDmaBufferOut = NVP_M2M_CONTEXT;

    pVPEContext->dwTransferCtxStart = dwStart;
    pVPEContext->dwTransferCtxSize = dwLimit;
    pVPEContext->bxfContextCreated = TRUE;

#endif // NVP_NV3

    return DX_OK;
}

/*
==============================================================================
    
    NVP3CheckForLaunch

    Description:    try to launch another mem transfer if engine not busy
                    (PIO version)

    Date:           Mar 18, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVP3CheckForLaunch(
    PNVP_CONTEXT pVPEContext,
    PNVP_SURFACE_INFO pSurface)
{
#ifdef NVP_NV3
    Nv03ChannelPio *pChan;
    PNVP_TRANSFER_BUFFER pTransfer;
    LPDDSURFACEDATA pSurfaceData;
    NVOS09_PARAMETERS ntfParam;
    NvNotification *aM2MNotifiers;
    PMDL pDestMDL;
    U032 dwOffsetStartByte;
    LONG lLength, lPitch, lCount;
    int nFreeCount;
    PNVP_SURFACE_INFO pSurf = NULL;

    DBGPRINT(DBGTRACE, "NVP3CheckForLaunch()");

    aM2MNotifiers = (NvNotification *)(pVPEContext->axfNotifiers);

    // is transfer in progress ?
    if ((aM2MNotifiers[NV039_NOTIFIERS_BUFFER_NOTIFY].status == NV039_NOTIFICATION_STATUS_IN_PROGRESS) ||
        // or no transfer request !?
        (pVPEContext->nCurTransferIdx == -1))
        return DXERR_GENERIC;

    // get a pointer to the VP pio channel
    pChan = (Nv03ChannelPio *)(pVPEContext->pNVPChannel);
    // which surface are we transferring from ?
    pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);
    pSurfaceData = (LPDDSURFACEDATA) (pTransfer->pSurfaceData);
    if (pSurface == NULL)
    {
        NVPGetSurface(pVPEContext, pSurfaceData->fpLockPtr, &pSurf);
        if (pSurf == NULL)
        {
            DBGPRINT(DBGERROR, "NVPCheckForLaunch() can't get pointer to surface");
            return DXERR_GENERIC;
        }
    }
    else
        pSurf = pSurface;

    // Recreate context dma for MDL
    NVPRecreateContext(pVPEContext, (PMDL)(pTransfer->pDestMDL));
    
    // reset notifier status
    aM2MNotifiers[NV039_NOTIFIERS_BUFFER_NOTIFY].status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;

#ifdef _NV_DPC
    //CHECKFREECOUNT(pMyNvChan,9*4);
    ntfParam.hObjectParent = pVPEContext->dwDeviceId;
    nFreeCount = pVPEContext->nFreeCount;
    while (nFreeCount < 9*4)
    {
        //RmInterrupt(&ntfParam);
        nFreeCount = NvGetFreeCount(pChan, 5);
    }
    pVPEContext->nFreeCount = nFreeCount - (9*4);
#endif

    // start at given line
    dwOffsetStartByte = pSurf->dwOffset + (pSurfaceData->lPitch * pTransfer->lStartLine);

    // lLength and lPitch might not be the same (case of interleaved fields !!!)
    lLength = (pSurfaceData->dwWidth)*(pSurfaceData->dwFormatBitCount >> 3);
    lLength = (pSurface->dwPreScaleSizeX)*(pSurfaceData->dwFormatBitCount >> 3);
    lPitch = pSurfaceData->lPitch;
            
    // SMAC@microsoft.com  :  Yes, you should transfer the end line.
    // Therefore, if we only wanted you to transfer a single line, start line would equal end line. 
    lCount = (pTransfer->lEndLine - pTransfer->lStartLine + 1);

    if (pTransfer->dwTransferFlags & DDTRANSFER_INVERT)
    {
        // if we want to invert things, then we should
        // negate the pitch and put the start offset at the end.
        
        dwOffsetStartByte = dwOffsetStartByte + lPitch * (lCount - 1);
        lPitch = -lPitch;
    }

    pDestMDL = (PMDL)(pTransfer->pDestMDL);
    if (pDestMDL->ByteCount < (ULONG)(lLength * lCount))
    {
        DBGPRINT(DBGERROR, "NVPCheckForLaunch() - Destination memory is not large enough to hold this image, truncating line count");
    //  lCount = (pTransfer->pDestMDL->ByteCount + pTransfer->pDestMDL->ByteOffset) / lLength;
        lCount = pDestMDL->ByteCount / lLength;
    }

#ifdef _NV_DPC
    ntfParam.hObjectParent = pVPEContext->dwDeviceId;
    nFreeCount = pVPEContext->nFreeCount;
    while (nFreeCount < 8*4)
    {
        //RmInterrupt(&ntfParam);
        nFreeCount = NvGetFreeCount(pChan, 5);
    }
    pVPEContext->nFreeCount = nFreeCount - (8*4);
#endif

    // set mem to mem transfer object !
    pChan->subchannel[NVP_M2M_SUBCHANNEL].SetObject = NVP_M2M_OBJECT;
    pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.OffsetIn = dwOffsetStartByte;
    //pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.OffsetOut = (unsigned long)pTheTransfer->DestMDL->ByteOffset;
    pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.OffsetOut = 0;
    pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.PitchIn  = lPitch;   // use the pitch in so that we can do interleaved
    pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.PitchOut = lLength;  // use line length on pitch out so that it squishes together
    pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.LineLengthIn = lLength;
    
    pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.LineCount    = lCount;
    pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.Format       =  ( 1 | (1 << 8));
    pChan->subchannel[NVP_M2M_SUBCHANNEL].nv03MemoryToMemoryFormat.BufferNotify = NV039_NOTIFY_WRITE_THEN_AWAKEN_1;

    pTransfer->dwTransferStatus = NVP_TRANSFER_IN_PROGRESS;

#endif // NVP_NV3

    return DX_OK;
}

/*
==============================================================================
    
    NVPRecreateContext

    Description:    if necessary, create context DMA for mem to mem object
                    (DMA version)

    Date:           Apr 22, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVPRecreateContext(
    PNVP_CONTEXT pVPEContext,
    PMDL pDestMDL)
{
    U032 dwStart;
    U032 dwLimit;

    // NV3 specific code
    if (NVP_GET_CHIP_VER(pVPEContext->dwChipId) <= NVP_CHIP_VER_03)
        return NVP3RecreateContext(pVPEContext, pDestMDL);

    // first check to see if we can reuse the OLD context... since context creation is expensive.
    dwStart = (U032)(pDestMDL->lpMappedSystemVa);
    dwLimit = (pDestMDL->ByteCount) - 1;
    
    if (pVPEContext->bxfContextCreated)
    {
        // do not recreate if same context
        if ((dwStart == pVPEContext->dwTransferCtxStart) && (dwLimit == pVPEContext->dwTransferCtxSize))
            return DX_OK;
    }

    // free the old context first
    if (pVPEContext->bxfContextCreated)
    {
        // free m2m dma context
        rmNVPFree(pVPEContext->hClient, pVPEContext->dwDeviceId, NVP_M2M_CONTEXT);

        pVPEContext->bxfContextCreated = FALSE;
    }
    
    // setup Notify Context
    if (rmNVPAllocContextDma(
                    pVPEContext->hClient,
                    NVP_M2M_CONTEXT,
                    NV01_CONTEXT_DMA_FROM_MEMORY,
                    0, //(ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                    0, // selector
                    dwStart,
                    dwLimit) != 0) //ALLOC_CTX_DMA_STATUS_SUCCESS)
    {
        DBGPRINT(DBGERROR, "Cannot allocate mem 2 mem context");
        return DXERR_GENERIC;
    }

    pVPEContext->dwTransferCtxStart = dwStart;
    pVPEContext->dwTransferCtxSize = dwLimit;
    pVPEContext->bxfContextFlushed = FALSE;
    pVPEContext->bxfContextCreated = TRUE;

    return DX_OK;
}

/*
==============================================================================
    
    NVPCheckForLaunch

    Description:    try to launch another mem transfer if engine not busy
                    (DMA version)

    Date:           Apr 22, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVPCheckForLaunch(
    PNVP_CONTEXT pVPEContext,
    PNVP_SURFACE_INFO pSurface)
{
    PNVP_TRANSFER_BUFFER pTransfer;
    LPDDSURFACEDATA pSurfaceData;
    NvNotification *aM2MNotifiers;
    PMDL pDestMDL;
    U032 dwOffsetStartByte;
    LONG lLength, lPitch, lCount;
    PNVP_SURFACE_INFO pSurf = NULL;
    Nv4ControlDma *pDmaChannel;
    U032 nvDmaCount, bVBICapture;

    // NV3 specific code
    if (NVP_GET_CHIP_VER(pVPEContext->dwChipId) <= NVP_CHIP_VER_03)
        return NVP3CheckForLaunch(pVPEContext, pSurface);

    DBGPRINT(DBGTRACE, "NVPCheckForLaunch()");

    aM2MNotifiers = (NvNotification *)(pVPEContext->axfNotifiers);
	pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);

    // is transfer in progress ?
    if (aM2MNotifiers[NV039_NOTIFIERS_BUFFER_NOTIFY].status == NV039_NOTIFICATION_STATUS_IN_PROGRESS)
        return DXERR_GENERIC;

    // or no transfer request !?
    if (pVPEContext->nCurTransferIdx == -1) //|| !(pVPEContext->bxfContextCreated))
    {
        return DXERR_GENERIC;
    }

	//make sure surf hasn't been DMAed
	if(pTransfer->dwTransferStatus == NVP_TRANSFER_QUEUED){
		//DBGPRINTVALUE(DBGINFO, "Checkforlaunch- xfering=", pTransfer->dwTransferId);
		DBGPRINT(DBGINFO, "Checkforlaunch- xfering=");
	}else{
		//DBGPRINTVALUE(DBGINFO, "Checkforlaunch- surf is already xferred=", pTransfer->dwTransferId);
		DBGPRINT(DBGINFO, "Checkforlaunch- surf is already xferred=");
		return DXERR_GENERIC;
	}

    // which surface are we transferring from ?
    //pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);
    pSurfaceData = (LPDDSURFACEDATA) (pTransfer->pSurfaceData);
    if (pSurface == NULL)
    {
        NVPGetSurface(pVPEContext, pSurfaceData->fpLockPtr, &pSurf);
        if (pSurf == NULL)
        {
            DBGPRINT(DBGERROR, "NVPCheckForLaunch() can't get pointer to surface");
            return DXERR_GENERIC;
        }
    }
    else
        pSurf = pSurface;

    // reset notifier status
    aM2MNotifiers[NV039_NOTIFIERS_BUFFER_NOTIFY].status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;

    // is it a raw8 (vbi) capture ?
    bVBICapture = (pSurfaceData->dwFormatFourCC == mmioFOURCC('R','A','W','8'));

    // start at given line
    dwOffsetStartByte = pSurf->dwOffset + (pSurfaceData->lPitch * pTransfer->lStartLine);

    // lLength and lPitch might not be the same (case of interleaved fields !!!)
    //////// NOTE: this should really be pSurfaceData->dwWidth instead of pSurf->dwPreScaleSizeX but the client
    ///            doesn't know we are prescaling... because of a bug in DShow OverlayMixer, we are using the 
    ///            MediaEngine prescaler behind the back of DirectShow/DxApi... a correction to the surface
    ///            width is needed here !
    //lLength = (pSurfaceData->dwWidth)*(pSurfaceData->dwFormatBitCount >> 3);
    lLength = (bVBICapture) ? pSurfaceData->dwWidth : pSurf->dwPreScaleSizeX;
    lLength *= (pSurfaceData->dwFormatBitCount >> 3); // convert to bytes !
    lPitch = pSurfaceData->lPitch;
            
    // SMAC@microsoft.com  :  Yes, you should transfer the end line.
    // Therefore, if we only wanted you to transfer a single line, start line would equal end line. 
    lCount = (pTransfer->lEndLine - pTransfer->lStartLine + 1);

    /// H.AZAR: 05/02/2000    
    /// NOTE: the following if-statement is a hack since DirectShow is unable of specifying single field capture, 
    ///       so we try to double the pitch to skip the bottom field and always return the top field...
    ///       An alternative to this hack would be for the client app to always capture full frame size (two fields)
    ///       and extract the field it needs...
    ///       PROS of using this hack:
    ///           - transfer only one field so no extra memory/bandwidth required for transfering a full frame
    ///           - no feathering effect or prescaling effect since only transfering the top field
    ///       CONS:
    ///           - it is a hack !
    ///           - if DShow get fixed one day, we will have to put extra logic to deal with old code !
    ///           - always return top field
    ///
/*
    //currently any height < 288 is single field
    //if it's single field, double the pitch (skipping bottom field)
    if (!bVBICapture && (lCount <= 288))
    {
        lPitch *= 2;
    }
*/
    //when capturing, src height is (pVPEContext->nvpOverlay).dwDecoderHeight
	//if capture height <= src height, it is single field capture
	if (!bVBICapture && (lCount <= (LONG)(pVPEContext->dwDecoderHeight)))
    {
        lPitch *= 2;
    }

    ////
    ////


    if (pTransfer->dwTransferFlags & DDTRANSFER_INVERT)
    {
        // if we want to invert things, then we should
        // negate the pitch and put the start offset at the end.
        
        dwOffsetStartByte = dwOffsetStartByte + lPitch * (lCount - 1);
        lPitch = -lPitch;
    }

    pDestMDL = (PMDL)(pTransfer->pDestMDL);

	//recreating a context dma for every captured frame causes weird behavior with ddraw.
	//changed to xfering to a fix surf in sys mem and do a sys 2 sys xfer in nvpchecktransferstatus
/*
    // if necessary, recreate context for new transfer buffer
    if (NVPRecreateContext(pVPEContext, pDestMDL) != DX_OK)
    {
        DBGPRINT(DBGERROR, "NVPCheckForLaunch() - error, can't create context dma for new transfer buffer !");
        return DXERR_GENERIC;
    }
*/


    if (pDestMDL->ByteCount < (ULONG)(lLength * lCount))
    {
        DBGPRINT(DBGERROR, "NVPCheckForLaunch() - Destination memory is not large enough to hold this image, truncating line count");
    //  lCount = (pTransfer->pDestMDL->ByteCount + pTransfer->pDestMDL->ByteOffset) / lLength;
        lCount = pDestMDL->ByteCount / lLength;
    }

    // get a pointer to the VPE dma channel
    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;
    NVP_DMAPUSH_CHECK_FREE_COUNT(20);

    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_OBJECT, NVP_M2M_OBJECT);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_CONTEXT_DMA_BUFFER_OUT, NVP_M2M_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_OFFSET_IN, dwOffsetStartByte);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_OFFSET_OUT, 0);    //(unsigned long)pTransfer->DestMDL->ByteOffset
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_PITCH_IN, lPitch);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_PITCH_OUT, lLength);

    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_LINE_LENGTH_IN, lLength);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_LINE_COUNT, lCount);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_FORMAT, ( 1 | (1 << 8)));
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_BUFFER_NOTIFY, NV039_NOTIFY_WRITE_THEN_AWAKEN_1);

    pTransfer->dwTransferStatus = NVP_TRANSFER_IN_PROGRESS;
    // go !
    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;

    return DX_OK;
}


/*
==============================================================================
    
    NVPGetTransferStatus

    Description:    returns status of buffer transfer (from video mem to
                    system mem)

    Date:           Mar 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVPGetTransferStatus(
    PVOID pContext,
    PVOID pNotUsed,
    PNVP_DDGETTRANSFEROUTINFO pOutInfo)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION)pContext;
    PNVP_TRANSFER_BUFFER pTransfer;
    PNVP_CONTEXT pVPEContext;

    DBGPRINT(DBGTRACE, "NVPGetTransferStatus()");

    // get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
    pVPEContext = &(pHwDevExt->avpContexts[0]);

    if (pVPEContext->nRecentTransferIdx == NVP_EOQ)
        return DXERR_GENERIC;

    pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nRecentTransferIdx]);

    if (pTransfer->dwTransferStatus == NVP_TRANSFER_DONE)
    {
        // return MS transfer ID
        pOutInfo->dwTransferID = pTransfer->dwTransferId;
        NVPRemoveTransfer(pVPEContext, pVPEContext->nRecentTransferIdx);

    }
    else
        return DXERR_GENERIC;

    return DX_OK;
}

/*
==============================================================================
    
    NVPAddTransfer

    Description:    adds transfer buffer to queue

    Date:           Mar 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVPAddTransfer(
    PNVP_CONTEXT pVPEContext,
    PNVP_DDTRANSFERININFO pInInfo,
    BOOL bIsOddField)
{
    PNVP_TRANSFER_BUFFER pNewTransfer;
    int i = pVPEContext->nCurTransferIdx;

    DBGPRINT(DBGTRACE, "NVPAddTransfer()");

    if (pVPEContext->nNumTransfer >= NVP_MAX_TRANSFER_BUFFERS)
        return DXERR_GENERIC;
    
    while (i != NVP_EOQ)
    {
        if (pVPEContext->aTransferBuffers[i].dwTransferId == pInInfo->dwTransferID)
            // a transfer request with same ID is already queued !!!
            return DXERR_GENERIC;
        else
            i = pVPEContext->aTransferBuffers[i].nNextTransferIdx;
    }

        
    // queue new transfer request !
    i = pVPEContext->nLastTransferIdx;
    pVPEContext->nLastTransferIdx = ++(pVPEContext->nLastTransferIdx) % NVP_MAX_TRANSFER_BUFFERS;
    pNewTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nLastTransferIdx]);
    if (i != NVP_EOQ)
        pVPEContext->aTransferBuffers[i].nNextTransferIdx = pVPEContext->nLastTransferIdx;
    pNewTransfer->nNextTransferIdx = NVP_EOQ;
    pNewTransfer->nPrevTransferIdx = i;
    (pVPEContext->nNumTransfer)++;
	if(pVPEContext->nNumTransfer){
		pVPEContext->ulCap=TRUE;
	}
    if (pVPEContext->nCurTransferIdx == NVP_EOQ)
        pVPEContext->nCurTransferIdx = pVPEContext->nLastTransferIdx;

    // DDTRANSFER_HALFLINES indicates that, due to half line issues, the odd field contains an
    // extra line of useless data at the top that the driver must account for
    if ((pInInfo->dwTransferFlags & DDTRANSFER_HALFLINES) && (bIsOddField))
    {
        pNewTransfer->lStartLine = (LONG)(pInInfo->dwStartLine) + 1;
        pNewTransfer->lEndLine = (LONG)(pInInfo->dwEndLine) + 1;
    }
    else
    {
        pNewTransfer->lStartLine = (LONG)(pInInfo->dwStartLine);
        pNewTransfer->lEndLine = (LONG)(pInInfo->dwEndLine);
    }

    // copy input data...
    pNewTransfer->pSurfaceData = (ULONG) (pInInfo->lpSurfaceData);
    pNewTransfer->dwTransferId = pInInfo->dwTransferID;
    pNewTransfer->dwTransferFlags = pInInfo->dwTransferFlags;
    pNewTransfer->pDestMDL = (ULONG) (pInInfo->lpDestMDL);
    pNewTransfer->dwTransferStatus = NVP_TRANSFER_QUEUED;

    return DX_OK;
}


/*
==============================================================================
    
    NVPCancelTransfer

    Description:    cancels transfer request before it gets processed

    Date:           Mar 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVPCancelTransfer(
    PNVP_CONTEXT pVPEContext,
    PNVP_DDTRANSFERININFO pInInfo)
{
    PNVP_TRANSFER_BUFFER pTransfer = NULL;
    int i = pVPEContext->nCurTransferIdx;

    DBGPRINT(DBGTRACE, "NVPCancelTransfer()");

    // look for transfer in queue
    while (i != NVP_EOQ)
    {
        if (pVPEContext->aTransferBuffers[i].dwTransferId == pInInfo->dwTransferID)
        {
            pTransfer = &(pVPEContext->aTransferBuffers[i]);
            break;
        }
        i = pVPEContext->aTransferBuffers[i].nNextTransferIdx;
    }

    if ((pTransfer) && (pTransfer->dwTransferStatus == NVP_TRANSFER_QUEUED))
        return NVPRemoveTransfer(pVPEContext, i);
    else
        return DXERR_GENERIC;
}

/*
==============================================================================
    
    NVPRemoveTransfer

    Description:    removes transfer request from queue

    Date:           Mar 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVPRemoveTransfer(
    PNVP_CONTEXT pVPEContext,
    int nTransferIdx)
{
    PNVP_TRANSFER_BUFFER pTransfer = &(pVPEContext->aTransferBuffers[nTransferIdx]);

    DBGPRINT(DBGTRACE, "NVPRemoveTransfer()");

    // check limits
    if (nTransferIdx == pVPEContext->nCurTransferIdx)
        pVPEContext->nCurTransferIdx = pTransfer->nNextTransferIdx;
    else if (nTransferIdx == pVPEContext->nLastTransferIdx)
        pVPEContext->nLastTransferIdx = pTransfer->nPrevTransferIdx;
    // reset values
    pTransfer->dwTransferId = NVP_UNDEF;
    pTransfer->dwTransferStatus = NVP_TRANSFER_FREE;
    // re-map indices
    if (pTransfer->nPrevTransferIdx != NVP_EOQ)
    {
        pVPEContext->aTransferBuffers[pTransfer->nPrevTransferIdx].nNextTransferIdx = pTransfer->nNextTransferIdx;
        pTransfer->nPrevTransferIdx = NVP_EOQ;
    }
    if (pTransfer->nNextTransferIdx != NVP_EOQ)
    {
        pVPEContext->aTransferBuffers[pTransfer->nNextTransferIdx].nPrevTransferIdx = pTransfer->nPrevTransferIdx;
        pTransfer->nNextTransferIdx = NVP_EOQ;
    }

    // one down !
    (pVPEContext->nNumTransfer)--;

    return DX_OK;
}
    

/*
==============================================================================
    
    NVPCheckTransferStatus

    Description:    checks for transfer status. Calls MS callback.

    Date:           Mar 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVPCheckTransferStatus(
    PHW_DEVICE_EXTENSION pHwDevExt)
{
    NvNotification *aM2MNotifiers;
    PNVP_CONTEXT pVPEContext;
    PNVP_TRANSFER_BUFFER pTransfer;
    U032 status;

    DBGPRINT(DBGTRACE, "nvpecore: NVPCheckTransferStatus()");

    // get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
    pVPEContext = &(pHwDevExt->avpContexts[0]);

    aM2MNotifiers = (NvNotification *)(pVPEContext->axfNotifiers);

    status = aM2MNotifiers[NV039_NOTIFIERS_BUFFER_NOTIFY].status;
    pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);
/*
    if (status == NV039_NOTIFICATION_STATUS_IN_PROGRESS)
        // still in progress
        return FALSE;
    else if (status == NV039_NOTIFICATION_STATUS_DONE_SUCCESS)
    {
        pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);

        // finished ok!
        if (pTransfer->dwTransferStatus == NVP_TRANSFER_IN_PROGRESS)
            pTransfer->dwTransferStatus = NVP_TRANSFER_DONE;

        pVPEContext->nRecentTransferIdx = pVPEContext->nCurTransferIdx;
    }
*/
	if (	status == NV039_NOTIFICATION_STATUS_DONE_SUCCESS ||
		status == NV039_NOTIFICATION_STATUS_IN_PROGRESS){	//notifier may already been reset 
		
		//logging 
		if(status == NV039_NOTIFICATION_STATUS_DONE_SUCCESS){
			//DBGPRINTVALUE(DBGINFO, "ChkXferStatus - m2m status success", pTransfer->dwTransferId);
			DBGPRINT(DBGINFO, "ChkXferStatus - m2m status success");

		}else{
			//DBGPRINTVALUE(DBGINFO, "ChkXferStatus - m2m status in prog", pTransfer->dwTransferId);
			DBGPRINT(DBGINFO, "ChkXferStatus - m2m status in prog");
		}
		//pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);

		//do Sys 2 Sys copy now
		//intermittent page faults were generated when Dst surf is no longer valid.
		//use ulCap=false when DDTRANSFER_CANCEL is sent to cancel sys 2 sys xfer.
		if(pVPEContext->ulCap)		
		{
			DWORD *pSrc;
			DWORD *pDst;
			//DWORD *pdw;
			DWORD dwLimit=((((PMDL)(pTransfer->pDestMDL))->ByteCount) - 1);///3;
			DWORD i;

			if(dwLimit < BUFSIZE){
				pSrc=(DWORD *)pVPEContext->pSysBuf;
				pDst=(DWORD *)(((PMDL)(pTransfer->pDestMDL))->lpMappedSystemVa);
				for(i=0; i<dwLimit; i+=4){
					*pDst=*pSrc;
					++pDst;
					++pSrc;
				}
			}

		
		}else{
			DBGPRINT(DBGERROR, "NOT xfering\n");
		}
        // finished ok!
        if (pTransfer->dwTransferStatus == NVP_TRANSFER_IN_PROGRESS)
            pTransfer->dwTransferStatus = NVP_TRANSFER_DONE;

        pVPEContext->nRecentTransferIdx = pVPEContext->nCurTransferIdx;

	}else{
        
		DBG_PRINT_STRING_VALUE(DBGERROR, "NVPCheckTransferStatus() - memory transfer failed error ", status);

        //pTransfer = &(pVPEContext->aTransferBuffers[pVPEContext->nCurTransferIdx]);

        // finished with an error !
        if (pTransfer->dwTransferStatus == NVP_TRANSFER_IN_PROGRESS)
            pTransfer->dwTransferStatus = NVP_TRANSFER_FAILURE;

    }

    // move to next transfer in queue (*** not sure whether I should do this even when an error had occured ?!?!?!?)
    pVPEContext->nCurTransferIdx = pTransfer->nNextTransferIdx;
/*
    // we pass on the interrupt
#if 0
    if (pHwDevExt->IRQCallback) //&& (pHwDevExt->dwIRQSources & DDIRQ_BUSMASTER))
    {
          PDX_IRQDATA pIRQData;
        pIRQData = (PDX_IRQDATA)(pHwDevExt->pIRQData);
        pIRQData->dwIrqFlags |= DDIRQ_BUSMASTER;
        ((PDX_IRQCALLBACK)(pHwDevExt->IRQCallback))(pIRQData);
    }
#else
      kmvtNVPCallbackDxApi((PVOID)pHwDevExt, DDIRQ_BUSMASTER);
#endif
*/
    return DX_OK;
}


/*
==============================================================================
    
    NVPCompleteTransfer

    Description:    checks for transfer status. Calls MS callback.

    Date:           Mar 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
DWORD NVPCompleteTransfer(PVOID pHwDevExtension)
{
    PHW_DEVICE_EXTENSION pHwDevExt = (PHW_DEVICE_EXTENSION) pHwDevExtension;
    PNVP_CONTEXT pVPEContext;

    // call MS callback if done with transfer
    if (NVPCheckTransferStatus(pHwDevExt) == DX_OK)
    {
        // get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
        pVPEContext = &(pHwDevExt->avpContexts[0]);
        //  transfer next in queue !
        NVPCheckForLaunch(pVPEContext, NULL);

		// we pass on the interrupt
#if 0
		if (pHwDevExt->IRQCallback) //&& (pHwDevExt->dwIRQSources & DDIRQ_BUSMASTER))
		{
			PDX_IRQDATA pIRQData;
			pIRQData = (PDX_IRQDATA)(pHwDevExt->pIRQData);
			pIRQData->dwIrqFlags |= DDIRQ_BUSMASTER;
			((PDX_IRQCALLBACK)(pHwDevExt->IRQCallback))(pIRQData);
		}
#else
		kmvtNVPCallbackDxApi((PVOID)pHwDevExt, DDIRQ_BUSMASTER);
#endif

        return DX_OK;
    }

    return DXERR_GENERIC;
}

#ifdef _WIN32_WINNT
/*
==============================================================================
    
    NVPTransferNotify

    Description:    callback routine for vbi/video transfer code.

    Date:           Mar 16, 1999
    
    Author:         H. Azar

==============================================================================
*/
void NVPTransferNotify(
    PVOID pHwDevExtension)
{
    NVPCompleteTransfer(pHwDevExtension);
}
#endif


/*
==============================================================================
    
    NVPPreScale

    Description:    Prescales YUV422 surface.  The new width and height are
                    returned.  If this routine does the field prescaling, it
                    flips off the field flags to indicate that the flip
                    should not bob.

    Date:           Nov 04, 1999
    
    Author:         H. Azar (based on VPP code from Sandy Lum)

==============================================================================
*/
BOOL NVPPreScale(
    PNVP_CONTEXT  pVPEContext,
    PNVP_OVERLAY  pOverlayInfo,
    DWORD         dwSrcOffset,
    DWORD         dwSrcPitch,
    DWORD         dwDstOffset,
    DWORD         dwDstPitch,
    DWORD         *pWidth,
    DWORD         *pHeight,
    DWORD         *pField)
{
    DWORD preScaleDeltaX;
    DWORD preScaleDeltaY;
    DWORD preScaleOverlay;
    DWORD dwDeltaX, dwDeltaY;
    DWORD dwOutSizeX, dwOutSizeY, dwOutFormat;
    DWORD dwInPitch, dwInOffset, dwInPoint, dwInSize;
    BOOL isField;
    Nv4ControlDma *pDmaChannel;
    U032 nvDmaCount;

    if (pOverlayInfo->dwOverlayDstWidth == 0 ||
        pOverlayInfo->dwOverlayDstHeight == 0)
        return FALSE;

    isField = IS_FIELD(*pField);

    if (pOverlayInfo->dwOverlayDstWidth <= 1)
    {
        dwDeltaX = 0x100000;
    }
    else
    {
        dwDeltaX = (pOverlayInfo->dwOverlaySrcWidth - 1) << 16;
        dwDeltaX /= pOverlayInfo->dwOverlayDstWidth - 1;
        dwDeltaX <<= 4;
    }

    if (pOverlayInfo->dwOverlayDstHeight <= 1)
    {
        dwDeltaY = 0x100000;
    }
    else
    {
        dwDeltaY = (pOverlayInfo->dwOverlaySrcHeight - 1) << 16; //(*pHeight - 1) << 16; //
        // err on the side of caution, need to do this because of the 1/4 pixel offset
        // ????? why is NV4 different from NV5?  NV4 HW must be a little wonky
        if (pVPEContext->dwChipId <= NV_DEVICE_VERSION_4) {
            dwDeltaY /= pOverlayInfo->dwOverlayDstHeight + 2;
        } else {
            dwDeltaY /= pOverlayInfo->dwOverlayDstHeight + 1;
        }
        dwDeltaY <<= 4;
    }

    preScaleOverlay = 0;

    if (pOverlayInfo->dwOverlaySrcWidth > (pOverlayInfo->dwOverlayDstWidth * pOverlayInfo->dwOverlayMaxDownScale))
    {
        // use X prescaler
        preScaleOverlay |= NV_PRESCALE_OVERLAY_X;
        dwOutSizeX = (pOverlayInfo->dwOverlayDstWidth + 2) & ~1;
        preScaleDeltaX = dwDeltaX;
        pOverlayInfo->dwOverlayDeltaX = 0x100000;
    } else {
        // use X overlay scaler
        dwOutSizeX = (*pWidth + 2) & ~1;
        preScaleDeltaX = 0x100000;
        pOverlayInfo->dwOverlayDeltaX = dwDeltaX;
    }

///// Until I get 'pOverlayInfo->regOverlayMode' logic implememted, always use Y prescaler... otherwise video will look aweful on NV4/5 in 'bob from non-interleaved' mode !
#ifndef NVP_CHECK_REGKEY
    // use Y prescaler
    preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
    dwOutSizeY = pOverlayInfo->dwOverlayDstHeight;
    preScaleDeltaY = dwDeltaY;
    pOverlayInfo->dwOverlayDeltaY = 0x100000;
#else
    if ((pOverlayInfo->dwOverlaySrcHeight > (pOverlayInfo->dwOverlayDstHeight * pOverlayInfo->dwOverlayMaxDownScale)) ||
        (pOverlayInfo->regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE))
    {
        // use Y prescaler
        preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
        dwOutSizeY = pOverlayInfo->dwOverlayDstHeight;
        preScaleDeltaY = dwDeltaY;
        pOverlayInfo->dwOverlayDeltaY = 0x100000;
    }
    else
    {
        // use Y overlay scaler
        dwOutSizeY = *pHeight;
        preScaleDeltaY = 0x100000;
        pOverlayInfo->dwOverlayDeltaY = dwDeltaY;
    }
#endif //#ifdef NVP_CHECK_REGKEY

    if ((pVPEContext->dwChipId < NV_DEVICE_VERSION_10) || (preScaleOverlay & NV_PRESCALE_OVERLAY_Y))
    {
        if (isField)
        {
            preScaleOverlay |= NV_PRESCALE_OVERLAY_FIELD;
            if (*pField & VPP_INTERLEAVED)
                preScaleDeltaY >>= 1;
        }
    }

    if (preScaleOverlay == 0)
        return FALSE;

    dwInSize = asmMergeCoords(*pWidth, *pHeight);
    dwInPoint = 0;
    dwInPitch = dwSrcPitch;
    dwInOffset = dwSrcOffset;
    if (isField && (preScaleOverlay & NV_PRESCALE_OVERLAY_FIELD)) {
        if (*pField & VPP_INTERLEAVED) {
            dwInPitch <<= 1;
            *pHeight >>= 1;
            // if (*pField & VPP_ODD) {
            if (*pField & VPP_EVEN) {
                dwInOffset += dwSrcPitch;
            }
        }
        if (*pField & VPP_BOB) {
            // use -1/4, +1/4 biasing for bob fields
            // if (*pField & VPP_ODD) {
            if (*pField & VPP_EVEN) {
                dwInPoint = 0xFFFC0000;
            } else {
                dwInPoint = 0x00040000;
            }
        }
    }

//    if (dwFourCC == FOURCC_UYVY) {
        dwOutFormat = NV038_IMAGE_OUT_FORMAT_COLOR_LE_YB8V8YA8U8;
//    } else {
//        dwOutFormat = NV038_IMAGE_OUT_FORMAT_COLOR_LE_V8YB8U8YA8;
//    }

    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;
    NVP_DMAPUSH_CHECK_FREE_COUNT(38);

    // program the VP to grab in the next surface
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_OBJECT , NVP_DVD_SUBPICTURE_OBJECT); // pVPEContext->dwDVDSubPicObjectID);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_OUT_POINT, 0);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_OUT_SIZE, (asmMergeCoords(dwOutSizeX, dwOutSizeY)));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_OUT_FORMAT, ((dwOutFormat << 16) | dwDstPitch));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_OUT_OFFSET, dwDstOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_DELTA_DU_DX, preScaleDeltaX);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_DELTA_DV_DY, preScaleDeltaY);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_SIZE, dwInSize);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_FORMAT, ((dwOutFormat << 16) | dwInPitch));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_OFFSET, dwInOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_IMAGE_IN_POINT, dwInPoint);

    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_DELTA_DU_DX, preScaleDeltaX);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_DELTA_DV_DY, preScaleDeltaY);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_SIZE, dwInSize);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_FORMAT, ((NV038_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT << 16) | dwInPitch));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_OFFSET, dwInOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_OVERLAY_POINT, 0);

    //NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_NOTIFY, NV038_NOTIFY_WRITE_THEN_AWAKEN);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_NO_OPERATION, 0);

    // this forces the H/W to serialize
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV038_SET_CONTEXT_DMA_OVERLAY, NVP_DVD_SUBPICTURE_CONTEXT);

//    NVP_DMAPUSH_START();
    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;

    // return changes in state
    if (//(pDriverData->regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE) ||
        (preScaleOverlay & NV_PRESCALE_OVERLAY_FIELD)) {
        // with HQVUp, we no longer use the overlay to bob
        *pField &= ~(VPP_ODD | VPP_EVEN | VPP_BOB | VPP_INTERLEAVED);
    }

    *pHeight = dwOutSizeY;
    *pWidth  = dwOutSizeX;

    return TRUE;
}

/*
==============================================================================
    
    NVPFlip

    Description:    Flip YUV422 surface.

    Date:           Nov 04, 1999
    
    Author:         H. Azar (based on VPP code from Sandy Lum)

==============================================================================
*/
BOOL NVPFlip(
    PNVP_CONTEXT  pVPEContext,
    PNVP_OVERLAY  pOverlayInfo,
    DWORD         dwOffset,        // source surface offset
    DWORD         dwPitch,         // source pitch
    DWORD         dwWidth,         // source width
    DWORD         dwHeight,        // source height
    DWORD         dwIndex,         // overlay buffer index
    DWORD         dwFlags)         // VPP flags
{
    NvNotification *pPioFlipOverlayNotifier = (NvNotification *)pOverlayInfo->pNvPioFlipOverlayNotifierFlat;
    DWORD dwPointIn, dwOverlayFormat, dwSizeIn;
    DWORD dwDeltaX, dwDeltaY;
    Nv4ControlDma *pDmaChannel;
    U032 nvDmaCount;

    DBGPRINT(DBGTRACE, "NVPFlip");

    dwPointIn = (dwOffset & NV_OVERLAY_BYTE_ALIGNMENT_PAD) << 3;  // << 3 insteed of << 4 because YUYV is 2 bytes wide
    dwSizeIn = asmMergeCoords((dwWidth & ~1), (dwHeight & ~1));
    dwSizeIn += dwPointIn;
    dwOffset &= ~NV_OVERLAY_BYTE_ALIGNMENT_PAD;

    // Remember this in case we get a SetOverlayPosition call
    //pOverlayInfo->dwOverlaySrcOffset = dwOffset;
    //pOverlayInfo->dwOverlaySrcPitch = dwPitch;

    dwDeltaX = pOverlayInfo->dwOverlayDeltaX;
    dwDeltaY = pOverlayInfo->dwOverlayDeltaY;

    if (IS_FIELD(dwFlags)) {
        if (dwFlags & VPP_INTERLEAVED) {
            // if (dwFlags & VPP_ODD) {
            if (dwFlags & VPP_EVEN) {
                dwOffset += dwPitch;
            }
            dwPitch <<= 1;
            dwHeight >>= 1;
            dwDeltaY >>= 1;
        }
        if (dwFlags & VPP_BOB) {
            // if (dwFlags & VPP_ODD) {
            if (dwFlags & VPP_EVEN) {
                dwPointIn += 0xFFFC0000;
            } else {
                dwPointIn += 0x00040000;
            }
        }
    }

    // replace old surface pitch value with current pitch value
    dwOverlayFormat = pOverlayInfo->dwOverlayFormat;
    dwOverlayFormat &= 0xFFFF0000;
    dwOverlayFormat |= dwPitch;
    dwOverlayFormat |= (1 << 31);

/*

    if (pVPEContext->dwVPConnectFlags & NVP_CONNECT_HDTV) // progressive (non-interlaced) and 16bit VIP
        dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_MATRIX_ITURBT709 << 24);
*/
    if (dwIndex & 0x1) {
        pPioFlipOverlayNotifier[2].status = NV_IN_PROGRESS;
    } else {
        pPioFlipOverlayNotifier[1].status = NV_IN_PROGRESS;
    }

    // program overlay to flip
    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;
    NVP_DMAPUSH_CHECK_FREE_COUNT(20);

    // program the VP to grab in the next surface
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OBJECT, NVP_OVERLAY_OBJECT); //pOverlayInfo->dwOverlayObjectID);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_SIZE_IN(dwIndex), dwSizeIn);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_SIZE_OUT(dwIndex), (asmMergeCoords(pOverlayInfo->dwOverlayDstWidth, pOverlayInfo->dwOverlayDstHeight)));
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_POINT_IN(dwIndex), dwPointIn);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_POINT_OUT(dwIndex), (asmMergeCoords(pOverlayInfo->dwOverlayDstX, pOverlayInfo->dwOverlayDstY)));

    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_OFFSET(dwIndex), dwOffset);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_COLORKEY(dwIndex), pOverlayInfo->dwOverlayColorKey);

    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_DU_DX(dwIndex), dwDeltaX);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_DV_DY(dwIndex), dwDeltaY);

    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV07A_SET_OVERLAY_FORMAT(dwIndex), dwOverlayFormat);

//    NVP_DMAPUSH_START();
    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;

    return TRUE;
}

//---------------------------------------------------------------------------
// vddPrescaleAndFlip
//
//      VPE implementation of selected parts of DDRAW's VPP pipe.  Only
//      prescaling and flip are performed.
//

/*
==============================================================================
    
    NVPPrescaleAndFlip

    Description:    VPE implementation of selected parts of DDRAW's VPP pipe.
                    Only prescaling and flip are performed.

    Date:           Nov 04, 1999
    
    Author:         H. Azar (based on VPP code from Sandy Lum)

==============================================================================
*/

#define NEXT_SURFACE { \
            dwSrcOffset = dwDstOffset;  \
            dwSrcPitch = dwDstPitch;    \
            dwDstOffset = dwNextOffset; \
            dwDstPitch = dwNextPitch;   \
            dwNextOffset = dwSrcOffset; \
            dwNextPitch = dwSrcPitch;   \
            dwOpCount++;                \
        }

BOOL NVPPrescaleAndFlip(
    PNVP_CONTEXT  pVPEContext,
    PNVP_OVERLAY  pOverlayInfo,
    DWORD         dwOffset,        // source surface offset
    DWORD         dwPitch,         // source pitch
    DWORD         dwOvIndex,         // overlay buffer index
    DWORD         dwFlags)         // VPP flags
{
    DWORD dwHeight, dwWidth, i = 0;
    DWORD dwSrcOffset, dwSrcPitch;
    DWORD dwDstOffset, dwDstPitch;
    DWORD dwNextOffset, dwNextPitch;
    DWORD dwOpCount;
    DWORD dwWorkSurfaces;
    BOOL  doXPreScale, doYPreScale, doPreScale;
    BOOL  isDownScale;
    DWORD dwRoundX, dwRoundY;
    DWORD vppExec;
    BOOL  isField;
    NvNotification *pPioFlipOverlayNotifier = (NvNotification *)pOverlayInfo->pNvPioFlipOverlayNotifierFlat;


    

#if 0 //#ifdef DEBUG
    char debugstr[256];

    debugstr[0] = '\0';
    if (dwFlags & VPP_ODD)          strcat(debugstr, "odd  ");
    if (dwFlags & VPP_EVEN)         strcat(debugstr, "even ");
    if (dwFlags & VPP_BOB)          strcat(debugstr, "bob ");
    if (dwFlags & VPP_INTERLEAVED)  strcat(debugstr, "int ");
    if (dwFlags & VPP_VIDEOPORT)    strcat(debugstr, "vp ");
    if (dwFlags & VPP_WAIT)         strcat(debugstr, "wait ");
    if (dwFlags & VPP_OPTIMIZEFLIP) strcat(debugstr, "opt ");
    strcat(debugstr, "REQ: ");
    if (dwFlags & VPP_CONVERT)      strcat(debugstr, "cvt ");
    if (dwFlags & VPP_SUBPICTURE)   strcat(debugstr, "sp ");
    if (dwFlags & VPP_PRESCALE)     strcat(debugstr, "ps ");
    if (dwFlags & VPP_COLOURCONTROL) strcat(debugstr, "cc ");
    if (dwFlags & VPP_TEMPORAL)     strcat(debugstr, "tf ");
    if (dwFlags & VPP_DEINTERLACE)  strcat(debugstr, "df ");
#endif

    //assert(pDriverData);

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // find out if overlay hardware is busy, if so return "overlay busy" error code.
    // Perhaps check a "number of overlay buffers programmed" count.  When this count is less
    // than 2, the overlay hardare is not busy and when it's equal to 2, the hardwar is busy.
    // If it's not busy, then program it up here and increment the count.  Then use a kernel
    // callback to decrement this count upon buffer complete.
    //

    DWORD dwIndex = pOverlayInfo->dwOverlayBufferIndex & 1;

    DBGPRINT(DBGTRACE, "NVPPrescaleAndFlip");

#if 0 //def _NVP_DTRACE
    DTRACE(0xdead0001);
    DTRACE_CYCLE_DELTA();
    DTRACE(dwOffset);
    DTRACE(dwIndex);

//    return TRUE;
#endif

/*
        while ((pPioFlipOverlayNotifier[dwIndex + 1].status == NV_IN_PROGRESS) && (i < VDD_VPP_TIME_OUT))
            i++;
        // timed-out... stop the appropriate buffer and return error
        if (i >= VDD_VPP_TIME_OUT)
*/


    dwWorkSurfaces = pOverlayInfo->dwExtra422NumSurfaces;
    vppExec = 0;
    dwOpCount = 0;
    dwSrcOffset = dwOffset;
    dwSrcPitch = dwPitch;
    dwDstOffset = pOverlayInfo->aExtra422OverlayOffset[pOverlayInfo->dwExtra422Index];
    dwDstPitch = pOverlayInfo->dwExtra422Pitch;
    dwNextOffset = pOverlayInfo->aExtra422OverlayOffset[pOverlayInfo->dwExtra422Index + 1];
    dwNextPitch = pOverlayInfo->dwExtra422Pitch;

    isField = IS_FIELD(dwFlags);

    doXPreScale = (pOverlayInfo->dwOverlaySrcWidth > (pOverlayInfo->dwOverlayDstWidth * pOverlayInfo->dwOverlayMaxDownScale));
    doYPreScale = (pOverlayInfo->dwOverlaySrcHeight > (pOverlayInfo->dwOverlayDstHeight * pOverlayInfo->dwOverlayMaxDownScale));

    if (pVPEContext->dwChipId < NV_DEVICE_VERSION_10) {
        // NV4/5 also prescales:
        //  - all BOBed fields
        //  - vertical upscales when HQVUp is enabled
        doYPreScale = TRUE; //doYPreScale || isField; //|| (pOverlayInfo->regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE)
    }

    doPreScale = (dwFlags & VPP_PRESCALE) &&
                 (doXPreScale || doYPreScale);

    isDownScale = FALSE;
    if (doPreScale)
    {
        DWORD dwSrcArea, dwDstArea;

        dwSrcArea = pOverlayInfo->dwOverlaySrcHeight * pOverlayInfo->dwOverlaySrcWidth;
        if ((dwFlags & (VPP_ODD | VPP_EVEN)) && (dwFlags & VPP_INTERLEAVED))
        {
            // bob mode
            dwSrcArea >>= 1;
        }
        dwDstArea = pOverlayInfo->dwOverlayDstHeight * pOverlayInfo->dwOverlayDstWidth;
        if (dwSrcArea > dwDstArea)
        {
            isDownScale = TRUE;
        }
    }

    if (dwWorkSurfaces < 2) {
        // no work surfaces, can't do anything
        doPreScale = FALSE;
    }

    // adjust for the subrectangle
    dwRoundX = pOverlayInfo->dwOverlaySrcX & 0x1;
    dwRoundY = pOverlayInfo->dwOverlaySrcY & 0x1;
    dwSrcOffset += (pOverlayInfo->dwOverlaySrcY & ~0x1) * dwSrcPitch;
    dwSrcOffset += (pOverlayInfo->dwOverlaySrcX & ~0x1) << 1;   // always YUV422
    dwWidth = pOverlayInfo->dwOverlaySrcSize & 0xFFFF;
    dwHeight = pOverlayInfo->dwOverlaySrcSize >> 16;
    if (dwWidth & 1)
        dwWidth += 1;
    if (dwRoundY)
        dwHeight += 1;

    // do the prescaling now
    if (doPreScale)
    {
        if (NVPPreScale(pVPEContext, pOverlayInfo,
                        dwSrcOffset, dwSrcPitch, dwDstOffset, dwDstPitch, &dwWidth, &dwHeight,
                        &dwFlags)) {
            NEXT_SURFACE;
            vppExec |= VPP_PRESCALE;
        }
    }

    if (dwRoundY)
    {
        dwSrcOffset += dwSrcPitch;
        dwHeight -= 1;
    }

//    if (dwRoundX)
//        dwWidth -= 1;

#ifdef VPEFSMIRROR
	// do FS Mirror
    if (pVPEContext->dwFsMirrorOn)
	{   
        if(NVPFsMirror(pVPEContext, pOverlayInfo, dwSrcOffset, dwSrcPitch, dwWidth, dwHeight, dwIndex, dwFlags)
           == NO_ERROR)
        {
            if (NVPFsFlip(pVPEContext, pVPEContext->nFSFlipIdx) 
                == NO_ERROR)
            {
             pVPEContext->nFSFlipIdx ^= 1;  // success, switch to other buffer            
            }        
        // move to next surface here            
        pVPEContext->nFSFlipSurfIdx = (pVPEContext->nFSFlipSurfIdx + 1) % pVPEContext->nNumFSSurf;
        }
	}
   
#endif

    // drop field if previous one is still being drawn (same overlay buffer) !
    if (pPioFlipOverlayNotifier[dwIndex + 1].status == NV_IN_PROGRESS){
        DBGPRINT(DBGWARN, "     Overlay still busy. Previous one is still being drawn");
    }
    else {
    // always do the flip, even if we are not flipping
    if (NVPFlip(pVPEContext, pOverlayInfo, dwSrcOffset,
                dwSrcPitch, dwWidth, dwHeight, dwIndex, dwFlags))
    {
        pOverlayInfo->dwOverlayBufferIndex ^= 1;
        if (dwWorkSurfaces == 0)
        {
            pOverlayInfo->dwExtra422Index = 0;
        }
        else if (dwWorkSurfaces <= 3)
        {
            pOverlayInfo->dwExtra422Index = (pOverlayInfo->dwExtra422Index + 1) % dwWorkSurfaces;
        }
        else
        {
            pOverlayInfo->dwExtra422Index = (pOverlayInfo->dwExtra422Index + 2) % dwWorkSurfaces;
        }
    }
    else
    {
        return FALSE;
    }
    }



    return TRUE;
}

/*
==============================================================================
    
    NVPIsVPEEnabled

    Description:    checks registry key VPEENABLE if set to 0 (disable)
                    return pCmdObj->dwPortId = 1 if enabled (default)
                           pCmdObj->dwPortId = 0 if disabled

    Date:           Feb 04, 2000
    
    Author:         H. Azar

==============================================================================
*/
VP_STATUS NVPIsVPEEnabled(
    PHW_DEVICE_EXTENSION pHwDevExt,
    NVP_CMD_OBJECT *pInCmdObj,
    NVP_CMD_OBJECT *pOutCmdObj)
{
    PNVP_CONTEXT pVPEContext;
    
    // get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
    pVPEContext = &(pHwDevExt->avpContexts[pInCmdObj->dwPortId]);

    // copy pointer to shared flag
    pVPEContext->pOvrRing0FlipFlag = pInCmdObj->pOvrRing0FlipFlag;

#ifdef _WIN32_WINNT
    // check flag (previously read from registry in function FindAdapter())
    pOutCmdObj->dwPortId = (pHwDevExt->dwVPEEnabled) ? 1 : 0;
#endif

    return NO_ERROR;
}

VP_STATUS NVPFlushChannel(
    PNVP_CONTEXT pVPEContext)
{
    Nv4ControlDma *pDmaChannel;
    U032 nvDmaCount;
    NvNotification *pavpNotifiers;
    U032    count=10000;
    
    DBGPRINT(DBGTRACE, "NVPFlushChannel");

    pDmaChannel = (Nv4ControlDma *)(pVPEContext->pDmaChannel);
    nvDmaCount = pVPEContext->nvDmaCount;


#define USE_M2M_OBJECT 
#ifdef USE_M2M_OBJECT

    pavpNotifiers = (NvNotification *)pVPEContext->axfNotifiers;
    (pavpNotifiers[NV039_NOTIFIERS_NOTIFY]).status=NV039_NOTIFICATION_STATUS_IN_PROGRESS;
    // send NO-OP thru
    NVP_DMAPUSH_CHECK_FREE_COUNT(10);

    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_OBJECT , NVP_M2M_OBJECT);
    pDmaChannel->Put = nvDmaCount << 2;

    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_NOTIFY, NV039_NOTIFY_WRITE_ONLY); 
    pDmaChannel->Put = nvDmaCount << 2;

    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_NO_OPERATION, 0x00000000); 
    pDmaChannel->Put = nvDmaCount << 2;
/*
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_OBJECT , NVP_M2M_OBJECT);
    //NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_CONTEXT_DMA_NOTIFIES, NVP_M2M_NOTIFIER);
    //NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_SET_CONTEXT_DMA_BUFFER_IN, NVP_M2M_FROM_CONTEXT);
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_NOTIFY, NV039_NOTIFY_WRITE_ONLY); 
    NVP_DMAPUSH_WRITE1(NVP_M2M_SUBCHANNEL, NV039_NO_OPERATION, 0x00000000); 

    pDmaChannel->Put = nvDmaCount << 2;
*/
    pVPEContext->nvDmaCount = nvDmaCount;

    while ( pavpNotifiers[NV039_NOTIFIERS_NOTIFY].status == 
                            NV04D_NOTIFICATION_STATUS_IN_PROGRESS && (--count));

    if(!count){
        DBGPRINT(DBGWARN, "Timeout. Unable to flush channel!!!");
    }

#endif

//#define USE_VID_DEC_OBJECT

#ifdef USE_VID_DEC_OBJECT
    ////////////////////////////////////////////////
    // reset notifiers
    //pavpNotifiers = &(((NvNotification *)(pVPEContext->avpNotifiers))[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)]);

    //pavpNotifiers->status = NV04D_NOTIFICATION_INFO16_FIELD_NOT_STARTED;//NV04D_NOTIFICATION_STATUS_IN_PROGRESS;
    pavpNotifiers = (NvNotification *)pVPEContext->avpNotifiers;
    (pavpNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)]).status=NV04D_NOTIFICATION_STATUS_IN_PROGRESS;//NV04D_NOTIFICATION_INFO16_FIELD_NOT_STARTED;
    // send NO-OP thru
    NVP_DMAPUSH_CHECK_FREE_COUNT(6);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_OBJECT , NVP_EXTERNAL_DECODER_OBJECT);
    //NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_NOTIFY, NV04D_NOTIFY_WRITE_ONLY); 
    //NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_NO_OPERATION, 0x00000000); //not implemented
    //NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_SET_CONTEXT_DMA_NOTIFIES, 
    //                                  NVP_EXTERNAL_DECODER_NOTIFIER);
    NVP_DMAPUSH_WRITE1(NVP_DECODER_SUBCHANNEL, NV04D_GET_IMAGE_OFFSET_NOTIFY(0), 
                                        NV04D_GET_IMAGE_OFFSET_NOTIFY_WRITE_ONLY); 
    pDmaChannel->Put = nvDmaCount << 2;
    pVPEContext->nvDmaCount = nvDmaCount;

    while ( ((pavpNotifiers[NV04D_NOTIFIERS_GET_IMAGE_OFFSET_NOTIFY(0)]).status) == 
                            NV04D_NOTIFICATION_STATUS_IN_PROGRESS  && (--count));

    if(!count){
        DBGPRINT(DBGWARN, "Timeout. Unable to flush channel!!!");
    }

#endif

    return NO_ERROR;
}

#endif // #ifdef NVPE

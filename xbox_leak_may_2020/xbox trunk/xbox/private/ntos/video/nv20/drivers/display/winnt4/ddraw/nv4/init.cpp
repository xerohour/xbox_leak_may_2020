//******************************************************************************
//
// Copyright (c) 1995-1996 Microsoft Corporation
//
// Module Name:
//
//     NV4INIT.C
//
// Abstract:
//
//     Initialize DX driver callbacks for NV4.
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//
//******************************************************************************

#include "nvprecomp.h"
#include "nvalpha.h"
#include "ddminint.h"
#include "nv32.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"
#include "nvsubch.h"

#include "nvVPP.h"

///////////////////////////////////////////////////////////////////////////
// Local Function Declarations
///////////////////////////////////////////////////////////////////////////
static NvU8 destroyExtendedDDPatch( PDEV *ppdev );
static NvU8 createExtendedDDPatch( PDEV *ppdev );

#define NvGetFreeCount(NN, CH)\
                 (NN)->subchannel[CH].control.Free

extern void NV4_DdPioSync(PDEV*);

VOID NvDDEnable(PDEV *ppdev)
{
    ppdev->ddCallBacks.WaitForVerticalBlank = DdWaitForVerticalBlank;
    ppdev->ddCallBacks.MapMemory            = DdMapMemory;
    ppdev->ddCallBacks.GetScanLine          = GetScanLine32;
    ppdev->ddCallBacks.dwFlags              = DDHAL_CB32_WAITFORVERTICALBLANK
                                    | DDHAL_CB32_MAPMEMORY
                                    | DDHAL_CB32_GETSCANLINE;

    ppdev->ddSurfaceCallBacks.Blt           = Nv4Blt;
    ppdev->ddSurfaceCallBacks.Flip          = Nv4Flip;
    ppdev->ddSurfaceCallBacks.Lock          = Nv4Lock;
    ppdev->ddSurfaceCallBacks.Unlock        = Nv4Unlock;
    ppdev->ddSurfaceCallBacks.GetBltStatus  = Nv4GetBltStatus;
    ppdev->ddSurfaceCallBacks.GetFlipStatus = DdGetFlipStatus;
    ppdev->ddSurfaceCallBacks.DestroySurface= Nv4DestroySurface;
    ppdev->ddSurfaceCallBacks.dwFlags       = DDHAL_SURFCB32_BLT
                                       | DDHAL_SURFCB32_FLIP
                                       | DDHAL_SURFCB32_UNLOCK
                                       | DDHAL_SURFCB32_LOCK
                                       | DDHAL_SURFCB32_GETBLTSTATUS
                                       | DDHAL_SURFCB32_GETFLIPSTATUS
                                       | DDHAL_SURFCB32_DESTROYSURFACE;

    ppdev->ddCallBacks.CreateSurface             = DdCreateSurface;
    ppdev->ddCallBacks.CanCreateSurface          = DdCanCreateSurface;
    ppdev->ddCallBacks.dwFlags                   |= DDHAL_CB32_CREATESURFACE
                                             | DDHAL_CB32_CANCREATESURFACE;

    ppdev->ddSurfaceCallBacks.SetColorKey        = DdSetColorKey;
    ppdev->ddSurfaceCallBacks.UpdateOverlay      = Nv4UpdateOverlay;
    ppdev->ddSurfaceCallBacks.SetOverlayPosition = Nv4SetOverlayPosition;
    ppdev->ddSurfaceCallBacks.dwFlags           |= DDHAL_SURFCB32_SETCOLORKEY
                                            | DDHAL_SURFCB32_UPDATEOVERLAY
                                            | DDHAL_SURFCB32_SETOVERLAYPOSITION;

    ppdev->pfnUpdateFlipStatus = Nv4UpdateFlipStatus;
}

//
// create the best NV_DD_VIDEO_LUT_CURSOR_DAC object we can
//
static NvU8 createLutCursorDacObj(PPDEV ppdev,NV_SystemInfo_t *pSysInfo)
{
	NvU32 retval;
    NV07C_ALLOCATION_PARAMETERS nv07cAllocParms;

    if (pSysInfo->dwNVClasses & NVCLASS_007C_DAC)
	{
        for (nv07cAllocParms.logicalHeadId=0; 
		     nv07cAllocParms.logicalHeadId < ppdev->pDriverData->dwHeads; 
			 nv07cAllocParms.logicalHeadId++)
		{
			retval = NvAlloc(
				ppdev->hDriver,ppdev->hClient,
				NV_VPP_CHANNEL_IID,
				NV_VPP_LUT_CURSOR_DAC_IID + nv07cAllocParms.logicalHeadId,
				NV15_VIDEO_LUT_CURSOR_DAC,
				&nv07cAllocParms);
            if (retval != NVOS21_STATUS_SUCCESS)
			{
				dbgError("alloc of class 7c should have succeeded, but did not!");
                return FALSE;
            }
        }
    }
    else if (pSysInfo->dwNVClasses & NVCLASS_0067_DAC)
	{
		retval = NvAllocObject(
			ppdev->hDriver,ppdev->hClient,
			NV_VPP_CHANNEL_IID,
			NV_VPP_LUT_CURSOR_DAC_IID,
			NV10_VIDEO_LUT_CURSOR_DAC);
        if (retval != NVOS21_STATUS_SUCCESS)
		{
			dbgError("alloc of class 67 should have succeeded, but did not!");
            return FALSE;
        }
    }
    else if (pSysInfo->dwNVClasses & NVCLASS_0049_DAC)
	{
		retval = NvAllocObject(
			ppdev->hDriver,ppdev->hClient,
			NV_VPP_CHANNEL_IID,
			NV_VPP_LUT_CURSOR_DAC_IID,
			NV05_VIDEO_LUT_CURSOR_DAC);
        if (retval != NVOS21_STATUS_SUCCESS)
		{
			dbgError("alloc of class 49 should have succeeded, but did not!");
            return FALSE;
        }
    }
    else if (pSysInfo->dwNVClasses & NVCLASS_0046_DAC)
	{
		retval = NvAllocObject(
			ppdev->hDriver,ppdev->hClient,
			NV_VPP_CHANNEL_IID,
			NV_VPP_LUT_CURSOR_DAC_IID,
			NV04_VIDEO_LUT_CURSOR_DAC);
        if (retval != NVOS21_STATUS_SUCCESS)
		{
			dbgError("alloc of class 46 should have succeeded, but did not!");
            return FALSE;
        }
    }
    else
	{
        dbgError("hardware doesn't seem to support any flavor of the VIDEO_LUT_CURSOR_DAC class!");
        return FALSE;
    }

#if 0
	    for (DWORD dwHead = 0; dwHead < pDriverData->dwHeads; dwHead ++) {
        // initialize LUT-cursor-DAC object
        nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) | 0x40000);
        nvPushData (1,NV_DD_VIDEO_LUT_CURSOR_DAC + dwHead);
        nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP) + NV067_SET_CONTEXT_DMA_NOTIFIES | 0x1C0000);
        nvPushData (3,NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
        nvPushData (4,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // image a
        nvPushData (5,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // image b
        nvPushData (6,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // LUT a
        nvPushData (7,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // LUT b
        nvPushData (8,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // cursor a
        nvPushData (9,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // cursor b
        nvPushData (10,dDrawSubchannelOffset(NV_DD_ROP) + NV067_SET_DAC(0) | 0x40000);
        nvPushData (11,asmMergeCoords(GET_MODE_WIDTH(), GET_MODE_HEIGHT()));
        nvPusherAdjust (12);
    }

#endif

	return TRUE;
}

//
// Try to create a 3D object (okay if we do not get one though)
//
static NvU8 create3Dobject(PPDEV ppdev, NV_SystemInfo_t *pSysInfo)
{
	NvU32 retval;
	NvU8 bHave3DClass = FALSE;

    if (pSysInfo->dwNVClasses & NVCLASS_0097_KELVIN)
    {
        retval = NvAllocObject(ppdev->hDriver,ppdev->hClient,NV_VPP_CHANNEL_IID,NV_VPP_3D_OBJECT_IID,NV20_KELVIN_PRIMITIVE);
        if (retval != NVOS05_STATUS_SUCCESS)
        {
            dbgError("Failed to get expected class: 0x0097 kelvin");
        }
        else
        {
            bHave3DClass = TRUE;
            DPF_LEVEL(NVDBG_LEVEL_INFO, "VPP using NV20 Kelvin class");
        }
    }
    else if (pSysInfo->dwNVClasses & NVCLASS_1196_CELSIUS)
    {
        retval = NvAllocObject(ppdev->hDriver,ppdev->hClient,NV_VPP_CHANNEL_IID,NV_VPP_3D_OBJECT_IID, NV11_CELSIUS_PRIMITIVE);
        if (retval != NVOS05_STATUS_SUCCESS)
        {
            dbgError("Failed to get expected class: 1196 CELSIUS");
        }
        else
        {
            bHave3DClass = TRUE;
            DPF_LEVEL(NVDBG_LEVEL_INFO, "VPP using NV11 Celsius class");
        }
    }
    else if (pSysInfo->dwNVClasses & NVCLASS_0096_CELSIUS)
    {
        retval = NvAllocObject(ppdev->hDriver,ppdev->hClient,NV_VPP_CHANNEL_IID, NV_VPP_3D_OBJECT_IID, NV15_CELSIUS_PRIMITIVE);
        if (retval != NVOS05_STATUS_SUCCESS)
        {
            dbgError("Failed to get expected class: 0096 CELSIUS");
        }
        else
        {
            bHave3DClass = TRUE;
            DPF_LEVEL(NVDBG_LEVEL_INFO, "VPP using NV15 Celsius class");
        }
    }
    else if (pSysInfo->dwNVClasses & NVCLASS_0056_CELSIUS)
    {
        retval = NvAllocObject(ppdev->hDriver,ppdev->hClient,NV_VPP_CHANNEL_IID, NV_VPP_3D_OBJECT_IID, NV10_CELSIUS_PRIMITIVE);
        if (retval != NVOS05_STATUS_SUCCESS)
        {
            dbgError("Failed to get expected class: 0056 CELSIUS");
        }
        else
        {
            bHave3DClass = TRUE;
            DPF_LEVEL(NVDBG_LEVEL_INFO, "VPP using NV10 Celsius class");
        }
    }
    else
    {
        bHave3DClass = FALSE;
        DPF_LEVEL(NVDBG_LEVEL_INFO, "No 3D super class is available.  Vpp proceeding without one.");
    }
	return bHave3DClass;
}

//**************************************************************************
// Allocate a DMA context which points to all of video memory. The limit
// must be page aligned: i.e. limit = (size in bytes of video mem rounded to the
// closest page boundary) - 1.
//
// TBD: not sure if we can share these.  They do appear to be channel independant.
//
//**************************************************************************
static NvU8 createContextDmas(PPDEV ppdev)
{
	NvU32 retval;

    retval = NvAllocContextDma(ppdev->hDriver,ppdev->hClient,
                        NV_VPP_DMA_WITHIN_VMEM_IID,
                        NV01_CONTEXT_DMA,
                        DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                        DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                        (PVOID) VIDMEM_ADDR(ppdev->pDriverData->BaseAddress),
                        ppdev->pDriverData->TotalVRAM - 1);
    if (retval != NVOS01_STATUS_SUCCESS)
    {
        dbgError("DMA Context <within vmem>: allocation failed");
		return FALSE;
    }

    retval = NvAllocContextDma(ppdev->hDriver,ppdev->hClient,
                        NV_VPP_DMA_FROM_VMEM_IID,
                        NV01_CONTEXT_DMA,
                        DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_ONLY) |
                        DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                        (PVOID) VIDMEM_ADDR(ppdev->pDriverData->BaseAddress),
                        ppdev->pDriverData->TotalVRAM - 1);
    if (retval != NVOS01_STATUS_SUCCESS)
    {
        dbgError("DMA Context <from vmem>: allocation failed");
		return FALSE;
    }

    retval = NvAllocContextDma(ppdev->hDriver,ppdev->hClient,
                        NV_VPP_DMA_TO_VMEM_IID,
                        NV01_CONTEXT_DMA,
                        DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY) |
                        DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                        (PVOID) VIDMEM_ADDR(ppdev->pDriverData->BaseAddress),
                        ppdev->pDriverData->TotalVRAM - 1);
    if (retval != NVOS01_STATUS_SUCCESS)
    {
        dbgError("DMA Context <to vmem>: allocation failed");
		return FALSE;
    }

	// Alloc 3 notifier slots for the OVERLAY notifier
    retval = NvAllocContextDma(ppdev->hDriver,ppdev->hClient,
                        NV_VPP_OVERLAY_NOTIFIER_IID,
                        NV01_CONTEXT_DMA,
                        DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY),
                        ppdev->pDriverData->pCommonNotifierBuffer,
                        3*sizeof(NvNotification) - 1);
    if (retval != NVOS01_STATUS_SUCCESS)
    {
        dbgError("DMA Context for overlay notifier: allocation failed");
		return FALSE;
    }

	return TRUE;
}
static NvU8 allocateCommonNotifierBuffer( PPDEV ppdev )
{
    NvU32   commonBufferSize;

    dbgTracePush("allocateCommonNotifierBuffer");

    commonBufferSize = 0
		+ 3*sizeof(NvNotification)	// for NV_VPP_OVERLAY_NOTIFIER_IID & NV_VPP_OVERLAY_IID
		- 1;

    // Allocate memory for a bunch of notifiers and the scanline buffer.
    // We must allocate the memory for this structure out of SHARED or
    // SYSTEM space -- not out of App space so don't use GlobalAlloc.

	nvAssert(ppdev->pDriverData->pCommonNotifierBuffer == NULL);

    ppdev->pDriverData->pCommonNotifierBuffer = (NvV32 *)EngAllocMem(FL_ZERO_MEMORY, commonBufferSize, ALLOC_TAG);
    if (ppdev->pDriverData->pCommonNotifierBuffer == NULL)
	{
        return FALSE;
    }
	return TRUE;
}

static NvU8 createExtendedDDPatch( PDEV *ppdev )
{
    NvU32 retval;
    NvU8  bHave3DSuperClass;
    CPushBuffer &nvPusher = ppdev->pDriverData->nvPusher;

    dbgTracePush("createExtendedDDPatch");

    nvAssert(ppdev);
    nvAssert(ppdev->pDriverData);

    // Set by VPP code, but otherwise ignored on NT4
    ppdev->pDriverData->dDrawSpareSubchannelObject = 0;
    ppdev->pDriverData->ddClipUpdate               = 0;
    ppdev->pDriverData->TwoDRenderingOccurred      = 0;
    ppdev->pDriverData->blitCalled                 = FALSE;
    ppdev->pDriverData->dwDXVAFlags                = 0;  // Not really used (though VPP sets them)
    ppdev->pDriverData->dwSharedClipChangeCount    = 0;
    ppdev->pDriverData->lpProcessInfoHead          = NULL;

    // allocate a push buffer
    nvPusher.setPdev(ppdev);
    if (!nvPusher.allocate(NV_VPP_CHANNEL_IID))
    {
        dbgError("VPP: failed to allocate push buffer");
		goto fail_extended_patch;
    }
    ppdev->vppChannelNdx = 2; // TBD: compute this as in nvEnable line 1570 @mjl@

	bHave3DSuperClass = create3Dobject(ppdev,&ppdev->pDriverData->sysInfo);

	if (!(    allocateCommonNotifierBuffer(ppdev)
		   && createLutCursorDacObj(ppdev,&ppdev->pDriverData->sysInfo)
	       && createContextDmas(ppdev)
	   ))
	{
		goto fail_extended_patch;
	}

    ppdev->pDriverData->dwMostRecent3dUser = MODULE_ID_NONE;

    if (bHave3DSuperClass)
    {
        nvPusher.setObject(NV_VPP_3D_OBJECT_SUBCH, NV_VPP_3D_OBJECT_IID);
    }
    // Now set DMA context
//    nvPusher.setObject(NV_VPP_3D_OBJECT_SUBCH, NV_VPP_3D_OBJECT_IID);
//    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        // some bare bones initialization: put the thing in its channel and set up for notifers
//        nvglSetObject (NV_DD_KELVIN, D3D_KELVIN_PRIMITIVE);
//        nvglSetNv20KelvinNotifierContextDMA (NV_DD_KELVIN, NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
//    }

    if (0 && !VppEnable(
        &ppdev->pDriverData->vpp,
        ppdev,
        &ppdev->pDriverData->nvPusher,
        NV_VPP_CHANNEL_IID,
        NV_VPP_3D_OBJECT_IID,
        NV_VPP_3D_OBJECT_SUBCH,         // permanent subchannel assignment for 3d object
        &(ppdev->pDriverData->dwMostRecent3dUser),
        NV_VPP_DMA_WITHIN_VMEM_IID,     // In VideoMem DMA (all of VM)
        NV_VPP_DMA_FROM_VMEM_IID,      // From VideoMem DMA (all of VM)
        NV_VPP_DMA_TO_VMEM_IID,        // To VideoMem DMA (all of VM)
        NV_VPP_LUT_CURSOR_DAC_IID ,

        0, /* misceventnotifier (dma) -- used to be: NV_DD_DMA_COLOUR_CONTROL_EVENT_NOTIFIER */
        0, /* contextSurfaceSwizzled -- was: D3D_CONTEXT_SURFACE_SWIZZLED */
        0, //   NV_DD_CONTEXT_BETA4,
        0, //   NV_DD_SURFACES_2D,
        0, // subchannel for SURFACES_2D object
        0, // hVideoMemUtoVideoMemFormat,    // U-conversion object
        0, // hVideoMemVtoVideoMemFormat,    // V-conversion object
        0, // hVideoMemUVtoVideoMemFormat,    // UV-conversion object

        0, // NV_DD_DVD_SUBPICTURE hDvdSubpicture,
        0, // NV_DD_CONTEXT_PATTERN
        0, // NV_CONTEXT_ROP
        0, //  NV_DD_CONTEXT_COLOR_KEY,
        0, //hFloatingContextDmaInOverlayShadow NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_OVERLAY_SHADOW

        0, //(NvNotification  *)pDriverData->DMA version !pFlipPrimaryNotifier,  May be an equivalent to this - @mjl@
        0, //(NvNotification  *)pDriverData->NvDmaPusherSyncNotifierFlat,  May be an equivalent to this - @mjl@
        0, //(NvNotification  *)pDriverData->NvPioColourControlEventNotifierFlat,

        0, // D3D_CONTEXT_SURFACES_ARGB_ZS,
        0, // D3D_DX6_MULTI_TEXTURE_TRIANGLE,
        0, // NV_DD_IMAGE_BLACK_RECTANGLE -- IID

        0, // NV_DD_ROP_RECT_AND_TEXT subchannel
        0, // NV_DD_ROP subchannel
        0 /* spare sub channel */
        ))
    {
        goto fail_extended_patch;
    }

    dbgTracePop();      // Success!
    return TRUE;

fail_extended_patch:    // Failure!  

    dbgTracePop();
    destroyExtendedDDPatch(ppdev);  // Destroy any objects we may have created
    return FALSE;
}

// TBD: release all objects allocated in createExtendedDDPatch @mjl@
static NvU8 destroyExtendedDDPatch( PDEV *ppdev )
{
    NvU32 retval;
    CPushBuffer &pushBuffer = ppdev->pDriverData->nvPusher;

    // flush the push buffer before we start destroying stuff
    if (pushBuffer.isValid())
    {
        pushBuffer.flush(TRUE, CPushBuffer::FLUSH_HEAVY_POLLING);
    }

    // We may or may not have any one or more of these... so it's okay if freeing it fails
	//   (eg: if we fail part way though initial alloc)
    retval = NvFree(ppdev->hDriver,ppdev->hClient,NV_VPP_CHANNEL_IID, NV_VPP_3D_OBJECT_IID);
    retval = NvFree(ppdev->hDriver,ppdev->hClient,NV_VPP_CHANNEL_IID, NV_VPP_LUT_CURSOR_DAC_IID);
    retval = NvFree(ppdev->hDriver,ppdev->hClient,ppdev->hDevice, NV_VPP_DMA_WITHIN_VMEM_IID);
    retval = NvFree(ppdev->hDriver,ppdev->hClient,ppdev->hDevice, NV_VPP_DMA_FROM_VMEM_IID);
    retval = NvFree(ppdev->hDriver,ppdev->hClient,ppdev->hDevice, NV_VPP_DMA_TO_VMEM_IID);

    if (ppdev->pDriverData->pCommonNotifierBuffer) EngFreeMem(ppdev->pDriverData->pCommonNotifierBuffer);
    ppdev->pDriverData->pCommonNotifierBuffer = NULL;

    retval = NvFree(ppdev->hDriver,ppdev->hClient,ppdev->hDevice, NV_VPP_OVERLAY_NOTIFIER_IID);

    if (pushBuffer.isValid())
    {
        pushBuffer.free();
        pushBuffer.setPdev(NULL);
    }

    return TRUE;
}

NvU8 bCreateNVDDPatch(PDEV    *ppdev)
{
    V032    videoFmt, color0, color1;
    LONG   i;
    Nv3ChannelPio   *nv;
    NvNotification *pSyncNotifier = (NvNotification *)ppdev->Notifiers->Sync;
    ULONG   ulScaledImageClass;
    NV07A_ALLOCATION_PARAMETERS nv07aAllocParms;
    PVOID parms;
    ULONG ulHead;
    NV_CREATE_OBJECT_SETUP();

    if(!NvGetSupportedClasses(ppdev))
	{
        return FALSE;
    }

    //**********************************************************************************
    // Open a new channel for ddraw.
    //**********************************************************************************
    ppdev->hDdChannel = ppdev->hPioChannel = NV_DD_DDRAW_PIO_CHANNEL_OBJECT_HANDLE;
    if (NvAllocChannelPio(  ppdev->hDriver,
                            ppdev->hClient,
                            ppdev->hDevice,
                            ppdev->hDdChannel,
                            NV03_CHANNEL_PIO,
                            0,
                            (PVOID *) &(ppdev->ddChannelPtr),
                            0
                            ) != NVOS04_STATUS_SUCCESS)
        {
        DPF("NVDD: Cannot get NV Ddraw PIO channel");
        NvFree(ppdev->hDriver, ppdev->hClient, NV01_NULL_OBJECT, ppdev->hClient);
        NvClose(ppdev->hDriver);
        return FALSE;
        }

    //********************************************************************************
    // Ensure the 2d channel ptr is valid. We need to do this even if 2d is running in
    // DMA PUSH mode, since our waitenginebusy fcts reference the 2d channel ptr.
    //********************************************************************************
    if (ppdev->pjMmBase == NULL)
        {
        ppdev->pjMmBase = ppdev->ddChannelPtr;
        }
    nv = (Nv3ChannelPio *) ppdev->ddChannelPtr;

    //**************************************************************************
    // Allocate a DMA context which points to all of video memory. The limit
    // must be page aligned: i.e. limit = (size in bytes of video mem rounded to the
    // closest page boundary) - 1.
    //**************************************************************************

    if (NvAllocContextDma(  ppdev->hDriver,
                        ppdev->hClient,
                        NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM,
                        NV01_CONTEXT_DMA,
                        NVOS03_FLAGS_ACCESS_READ_WRITE,
                        (PVOID)(ppdev->pjFrameBufbase),
                        ppdev->cbFrameBuf - 1
                        ) != NVOS03_STATUS_SUCCESS )
    {
    DPF("NVDD: Cannot allocate dma in memory context");
    return FALSE;
    }

    if (NvAllocContextDma(  ppdev->hDriver,
                            ppdev->hClient,
                            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_WRITE_ONLY,
                            (PVOID)(ppdev->Notifiers->DmaToMem),
                            5 * sizeof(NvNotification)
                            ) != NVOS03_STATUS_SUCCESS)
        {
        DPF("NVDD: Cannot allocate notifier context");
        return FALSE;
        }

    if (NvAllocContextDma(  ppdev->hDriver,
                            ppdev->hClient,
                            DD_FIFO_SYNC_NOTIFIER,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_WRITE_ONLY,
                            (PVOID)(ppdev->Notifiers->Sync),
                            5 * sizeof(NvNotification)
                            ) != NVOS03_STATUS_SUCCESS)
        {
        DPF("NVDD: Cannot allocate notifier context");
        return FALSE;
        }

    //**************************************************************************
    // Determine black and white colors
    //**************************************************************************

    switch (ppdev->iBitmapFormat)
    {
        case BMF_32BPP:
            color0 =    NV_ALPHA_1_32 | 0x00000000;         // BLACK
            color1 =    NV_ALPHA_1_32 | 0x00ffffff;         // WHITE
            videoFmt = NV_VFM_FORMAT_COLOR_LE_X8R8G8B8;
            break;
        case BMF_16BPP:

            if (ppdev->flGreen == 0x03e0)
                {
                //**************************************************************
                // 5:5:5 format
                //**************************************************************
                videoFmt = NV_VFM_FORMAT_COLOR_LE_X1R5G5B5_P2;
                color0 =    NV_ALPHA_1_16 | 0x00000000;     // BLACK
                color1 =    NV_ALPHA_1_16 | 0x00007fff;     // WHITE
                }
            else
                {

                //**************************************************************
                // 5:6:5 format
                //**************************************************************

                color0 =   (NV_ALPHA_1_565 | 0x00000000);       // BLACK
                color1 =   (NV_ALPHA_1_565 | 0x0000ffff);       // WHITE
                videoFmt = NV_VFM_FORMAT_COLOR_LE_R5G6B5_P2;
                }

            break;
        case BMF_8BPP:
            color0 =    NV_ALPHA_1_08 | 0x00000000;         // BLACK
            color1 =    NV_ALPHA_1_08 | 0x000000ff;         // WHITE
            videoFmt = NV_VFM_FORMAT_COLOR_LE_Y8_P4;

            //*************************************************************************
            // Setup palette for indexed color mode.
            // allocate the colormap context (from system memory)
            //*************************************************************************
            if (NvAllocContextDma(  ppdev->hDriver,
                        ppdev->hClient,
                        NV_DD_WIN_COLORMAP_CONTEXT,
                        NV01_CONTEXT_DMA,
                        NVOS03_FLAGS_ACCESS_READ_ONLY,
                        (PVOID)(ppdev->ajClutData),
                        (256 * sizeof(VIDEO_CLUTDATA)) - 1
                        ) != NVOS03_STATUS_SUCCESS)
                {
                DPF("NVDD: Cannot allocate buffer context");
                return FALSE;
                }
            break;
        default:
            return(FALSE);
    }

    //*************************************************************************
    // allocate the dma notifier context for flip surface
    //*************************************************************************
    if (NvAllocContextDma(  ppdev->hDriver,
                            ppdev->hClient,
                            NV_DD_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_WRITE_ONLY,
                            (PVOID)(ppdev->Notifiers->FlipPrimary),
                            9 * sizeof(NvNotification)
                            ) != NVOS03_STATUS_SUCCESS)
        {
        DPF("NVDD: Cannot allocate context dma notifier for flip surface");
        return FALSE;
        }
    ((NvNotification *) (&(ppdev->Notifiers->FlipPrimary[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status = 0;


    //*************************************************************************
    // allocate the dma notifier context for video overlay flip
    //*************************************************************************
    if (NvAllocContextDma(  ppdev->hDriver,
                        ppdev->hClient,
                        NV_DD_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                        NV01_CONTEXT_DMA,
                        NVOS03_FLAGS_ACCESS_WRITE_ONLY,
                        (PVOID)(ppdev->Notifiers->FlipOverlay),
                        5 * sizeof(NvNotification)
                        ) != NVOS03_STATUS_SUCCESS)
        {
        DPF("NVDD: Cannot allocate notifier context");
        return FALSE;
        }
    ((NvNotification *) (ppdev->Notifiers->FlipOverlay))->status = 0;


    //***********************************************************************************
    // Allocate a buffer for scanlines.
    //***********************************************************************************
    if ((ppdev->NvDmaBufferFlat = EngAllocMem(0, 0x8000, ALLOC_TAG)) != NULL)
        {
        ppdev->NvScanlineBufferFlat = ppdev->NvDmaBufferFlat;
        if (NvAllocContextDma(  ppdev->hDriver,
                        ppdev->hClient,
                        NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
                        NV01_CONTEXT_DMA,
                        NVOS03_FLAGS_ACCESS_READ_WRITE,
                        ppdev->NvDmaBufferFlat,
                        0x8000 - 1
                        ) != NVOS03_STATUS_SUCCESS)
            return(FALSE);
        }


    //******************************************************************************************
    // Create the NV Objects. The video back end objects are identical to NV3.
    //******************************************************************************************
    NV_CREATE_OBJECT_INIT();
    NV_CREATE_OBJECT_PIO2(NV10_CONTEXT_SURFACES_2D, NV04_CONTEXT_SURFACES_2D,
                          NV_DD_PRIMARY_SURFACE);
    if (NV_CREATE_OBJECT_FAIL()) {
        return FALSE;
    } else {
        if (NV_CREATE_OBJECT_CLASS() == NV10_CONTEXT_SURFACES_2D) {
            ulScaledImageClass = NV10_SCALED_IMAGE_FROM_MEMORY;
        } else {
            ulScaledImageClass = NV04_SCALED_IMAGE_FROM_MEMORY;
        }
    }
    
    NV_CREATE_OBJECT_INIT();
    NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_FLOATING0_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_FLOATING1_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV3_CONTEXT_ROP, NV_DD_ROP5_SOLID);
    NV_CREATE_OBJECT_PIO1(NV1_IMAGE_BLACK_RECTANGLE, NV_DD_IMAGE_BLACK_RECTANGLE);
    NV_CREATE_OBJECT_PIO1(NV4_CONTEXT_PATTERN, NV_DD_IMAGE_PATTERN);
    NV_CREATE_OBJECT_PIO1(NV4_CONTEXT_COLOR_KEY, NV_DD_IMAGE_SOLID);
    NV_CREATE_OBJECT_PIO1(NV4_IMAGE_FROM_CPU, NV_DD_IMAGE_FROM_CPU);
    NV_CREATE_OBJECT_PIO1(NV4_IMAGE_BLIT, NV_DD_IMAGE_BLIT);
    NV_CREATE_OBJECT_PIO1(NV4_GDI_RECTANGLE_TEXT, NV_DD_RENDER_RECT_AND_TEXT);
    // video overlay class
    {
        NV_CREATE_OBJECT_SETUP();
        ulHead = ppdev->ulDeviceDisplay[0];
        if (ulHead > 0) {
            nv07aAllocParms.logicalHeadId = ulHead;
            parms = &nv07aAllocParms;
        } else {
            parms = NULL;
        }

        NV_CREATE_OBJECT_INIT();

        NV_CREATE_OBJECT_PARM_PIO2(parms,  NV_DD_VIDEO_OVERLAY, 
                   NV10_VIDEO_OVERLAY, NV04_VIDEO_OVERLAY);

        if (NV_CREATE_OBJECT_FAIL()) {
            return FALSE;
        } else {
            ppdev->CurrentClass.VideoOverlay = NV_CREATE_OBJECT_CLASS();
        }
    }
    
    // if 8bpp, create colormap
    NV_CREATE_OBJECT_INIT();
    if (ppdev->iBitmapFormat == BMF_8BPP) {
        NV_CREATE_OBJECT_PIO1(NV_VIDEO_COLORMAP, NV_DD_SHARED_VIDEO_COLORMAP);
        NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_VIDEO, NV_DD_P_V_SHARED_VIDEO_COLORMAP);
    }

    NV_CREATE_OBJECT_PIO1(NV3_MEMORY_TO_MEMORY_FORMAT, NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT);
    NV_CREATE_OBJECT_PIO1(NV3_MEMORY_TO_MEMORY_FORMAT, NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT);
    NV_CREATE_OBJECT_PIO1(NV4_STRETCHED_IMAGE_FROM_CPU, NV_DD_STRETCHED_UV_IMAGE_FROM_CPU);
    NV_CREATE_OBJECT_PIO1(NV4_STRETCHED_IMAGE_FROM_CPU, NV_DD_STRETCHED_IMAGE_FROM_CPU);
    if (NV_CREATE_OBJECT_FAIL()) {
        return FALSE;
    }    

    //*******************************************************************************
    // Create video overlay objects.
    //*******************************************************************************
    if (ppdev->cBitsPerPel > 8) {
        NV_CREATE_OBJECT_INIT();
        NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY);
        NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY);
        NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY);
        NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(ulScaledImageClass, NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY);
        if (NV_CREATE_OBJECT_FAIL()) {
            return FALSE;
        }    
    }


    {
    ULONG i, ulHead;
    PVOID parms;
    NV07C_ALLOCATION_PARAMETERS nv07cAllocParms;


    	for(i = 0; i < ppdev->ulNumberDacsActive; i++)
	    {
            NV_CREATE_OBJECT_SETUP();

            ulHead = ppdev->ulDeviceDisplay[i];

            if (ulHead > 0) {
                nv07cAllocParms.logicalHeadId = ulHead;
                parms = &nv07cAllocParms;
            } else {
                parms = NULL;
            }
            // NV5 will use NV04_VIDEO_LUT_CURSOR_DAC class because the method offset of NV05_VIDEO_LUT_CURSOR_DAC 
            // is different from other NV??_VIDEO_LUT_CURSOR_DAC classes.
            NV_CREATE_OBJECT_INIT();

            NV_CREATE_OBJECT_PARM_PIO3(parms,NV_DD_VIDEO_LUT_CURSOR_DAC+ulHead, 
                       NV15_VIDEO_LUT_CURSOR_DAC,NV10_VIDEO_LUT_CURSOR_DAC,
                       NV04_VIDEO_LUT_CURSOR_DAC);
            if (NV_CREATE_OBJECT_FAIL()) 
            {
                return (FALSE);
            } 
            else 
            {
                while (NvGetFreeCount(nv, 0) < 3*4);
            
                nv->subchannel[NV_DD_PRIMARY].SetObject = NV_DD_VIDEO_LUT_CURSOR_DAC+ulHead;
                nv->subchannel[NV_DD_PRIMARY].Nv04VideoLutCursorDac.SetContextDmaImage[0] = NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
                nv->subchannel[NV_DD_PRIMARY].Nv04VideoLutCursorDac.SetContextDmaNotifies = NV_DD_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
                
            }
        }
    }

    while (NvGetFreeCount(nv, 0) < 8*4);
    nv->subchannel[NV_DD_PRIMARY].SetObject       = NV_DD_PRIMARY_SURFACE;
    nv->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetContextDmaNotifies = NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetContextDmaImageSource    = NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
    nv->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetContextDmaImageDestin    = NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;

    // Determine the color format
    switch (ppdev->iBitmapFormat)
        {
        case BMF_32BPP:
            nv->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetColorFormat = NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;
            break;
        case BMF_16BPP:
            nv->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetColorFormat =
                NV042_SET_COLOR_FORMAT_LE_R5G6B5;
            break;
        case BMF_8BPP:
            nv->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetColorFormat = NV042_SET_COLOR_FORMAT_LE_Y8;
            break;
        default:
            return(FALSE);
        }

    nv->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetPitch  =
        ppdev->lDelta << 16 | (ppdev->lDelta);
    nv->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetOffsetSource =  ppdev->ulPrimarySurfaceOffset;
    nv->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetOffsetDestin =  ppdev->ulPrimarySurfaceOffset;

    //**********************************************************************
    // Make sure to reset pitches and offsets across modesets!
    //**********************************************************************

    ppdev->DdCurrentSourceOffset = ppdev->DdCurrentDestOffset = ppdev->ulPrimarySurfaceOffset;
    ppdev->DdCurrentSourcePitch  = ppdev->DdCurrentDestPitch = ppdev->lDelta;

    //**************************************************************************
    // Setup ROP5 SOLID
    //**************************************************************************


    while (NvGetFreeCount(nv, 0) < 3*4);
    nv->subchannel[NV_DD_ROP].SetObject      = NV_DD_ROP5_SOLID;
    nv->subchannel[NV_DD_ROP].nv3ContextRop.SetContextDmaNotifies = NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[NV_DD_ROP].nv3ContextRop.SetRop5 = 0xcccc;

    //**************************************************************************
    // Setup clip rectangle
    //**************************************************************************

    while (NvGetFreeCount(nv, 0) < 4*4);
    nv->subchannel[NV_DD_CLIP].SetObject       = NV_DD_IMAGE_BLACK_RECTANGLE;
    nv->subchannel[NV_DD_CLIP].nv1ImageBlackRectangle.SetContextDmaNotifies = DD_FIFO_SYNC_NOTIFIER;
    nv->subchannel[NV_DD_CLIP].nv1ImageBlackRectangle.SetPoint = 0;
    nv->subchannel[NV_DD_CLIP].nv1ImageBlackRectangle.SetSize  = ((0x7fff<<16) | 0x7fff);
    ppdev->DdClipResetFlag = 0;

    //**********************************************************************
    // Setup CONTEXT PATTERN
    //**********************************************************************

    while (NvGetFreeCount(nv, 0) < 10*4);
    nv->subchannel[NV_DD_PATTERN].SetObject      = NV_DD_IMAGE_PATTERN;
    nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetContextDmaNotifies = NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

    // Determine the color format
    switch (ppdev->iBitmapFormat)
        {
        case BMF_32BPP:
            nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetColorFormat = NV044_SET_COLOR_FORMAT_LE_A8R8G8B8;
            break;
        case BMF_16BPP:
            nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetColorFormat = NV044_SET_COLOR_FORMAT_LE_A16R5G6B5;
            break;
        case BMF_8BPP:
            // Must be set to a legal value but hardware ignores it otherwise
            nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetColorFormat = NV044_SET_COLOR_FORMAT_LE_A8R8G8B8;
            break;
        default:
            return(FALSE);
        }

    nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeFormat   = NV044_SET_MONOCHROME_FORMAT_CGA6_M1;
    nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeShape    = NV044_SET_MONOCHROME_SHAPE_64X_1Y ;
    nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternSelect      = NV044_SET_PATTERN_SELECT_MONOCHROME;
    nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeColor0   = color0;
    nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeColor1   = color1;
    nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromePattern0 = 0xffffffff;
    nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromePattern1 = 0x50505050;

    //**********************************************************************
    // Y8 default pattern values
    //**********************************************************************

    while (NvGetFreeCount(nv, 0) < 16*4);
    for (i=0;i<16;i++)
        nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternY8[i] = 0xffffffff;

    //**********************************************************************
    // R5G6B5 default pattern values
    //**********************************************************************

    while (NvGetFreeCount(nv, 0) < 16*4);
    for (i=0;i<16;i++)
        nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternR5G6B5[i] = 0xffffffff;

    while (NvGetFreeCount(nv, 0) < 16*4);
    for (i=16;i<32;i++)
        nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternR5G6B5[i] = 0xffffffff;

    //**********************************************************************
    // X1R5G5B5 default pattern values
    //**********************************************************************

    while (NvGetFreeCount(nv, 0) < 16*4);
    for (i=0;i<16;i++)
        nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternX1R5G5B5[i] = 0xffffffff;

    while (NvGetFreeCount(nv, 0) < 16*4);
    for (i=16;i<32;i++)
        nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternX1R5G5B5[i] = 0xffffffff;

    //**********************************************************************
    // X8R8G8B8 default pattern values
    //**********************************************************************

    while (NvGetFreeCount(nv, 0) < 16*4);
    for (i=0;i<16;i++)
        nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternX8R8G8B8[i] = 0xffffffff;

    while (NvGetFreeCount(nv, 0) < 16*4);
    for (i=16;i<32;i++)
        nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternX8R8G8B8[i] = 0xffffffff;

    while (NvGetFreeCount(nv, 0) < 16*4);
    for (i=32;i<48;i++)
        nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternX8R8G8B8[i] = 0xffffffff;

    while (NvGetFreeCount(nv, 0) < 16*4);
    for (i=48;i<64;i++)
        nv->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternX8R8G8B8[i] = 0xffffffff;

    //**********************************************************************
    // Setup IMAGE SOLID (CONTEXT COLOR KEY for NV4)
    //**********************************************************************

    while (NvGetFreeCount(nv, 0) < 4*4);
    nv->subchannel[NV_DD_SPARE].SetObject = NV_DD_IMAGE_SOLID;
    nv->subchannel[NV_DD_SPARE].nv4ContextColorKey.SetContextDmaNotifies = NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

    // Determine the color format
    switch (ppdev->iBitmapFormat)
        {
        case BMF_32BPP:
            nv->subchannel[NV_DD_SPARE].nv4ContextColorKey.SetColorFormat = NV057_SET_COLOR_FORMAT_LE_A8R8G8B8;
            break;
        case BMF_16BPP:
            nv->subchannel[NV_DD_SPARE].nv4ContextColorKey.SetColorFormat = NV057_SET_COLOR_FORMAT_LE_A16R5G6B5;
            break;
        case BMF_8BPP:
            // Does hardware ignore this value???
            nv->subchannel[NV_DD_SPARE].nv4ContextColorKey.SetColorFormat = NV057_SET_COLOR_FORMAT_LE_A8R8G8B8;
            break;
        default:
            return(FALSE);
        }

    nv->subchannel[NV_DD_SPARE].nv4ContextColorKey.SetColor = 0L; // Disabled

    //**********************************************************************
    // Setup Image From CPU (565 format)
    //
    // NOTE:  We CAN NOT use the COLOR_KEY (IMAGE_SOLID)
    //        if ROP_OPERATION = ROP_AND
    //**********************************************************************

    while (NvGetFreeCount(nv, 0) < 8*4);
    nv->subchannel[NV_DD_SPARE].SetObject         = NV_DD_IMAGE_FROM_CPU;
    nv->subchannel[NV_DD_SPARE].nv4ImageFromCpu.SetContextDmaNotifies = NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[NV_DD_SPARE].nv4ImageFromCpu.SetContextClipRectangle = NV_DD_IMAGE_BLACK_RECTANGLE;
    nv->subchannel[NV_DD_SPARE].nv4ImageFromCpu.SetContextPattern     = NV_DD_IMAGE_PATTERN;
    nv->subchannel[NV_DD_SPARE].nv4ImageFromCpu.SetContextRop         = NV_DD_ROP5_SOLID;
    nv->subchannel[NV_DD_SPARE].nv4ImageFromCpu.SetContextSurface     = NV_DD_PRIMARY_SURFACE;
    nv->subchannel[NV_DD_SPARE].nv4ImageFromCpu.SetOperation          = NV061_SET_OPERATION_ROP_AND;

    // Determine the color format
    switch (ppdev->iBitmapFormat)
        {
        case BMF_32BPP:
            nv->subchannel[NV_DD_SPARE].nv4ImageFromCpu.SetColorFormat = NV061_SET_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        case BMF_16BPP:
            nv->subchannel[NV_DD_SPARE].nv4ImageFromCpu.SetColorFormat = NV061_SET_COLOR_FORMAT_LE_R5G6B5;
            break;
        case BMF_8BPP:
            // Does hardware ignore this value???
            nv->subchannel[NV_DD_SPARE].nv4ImageFromCpu.SetColorFormat = NV061_SET_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        default:
            return(FALSE);
        }

    //**************************************************************************
    // Setup Image Blit
    //**************************************************************************

    while (NvGetFreeCount(nv, 0) < 8*4);
    nv->subchannel[NV_DD_BLIT].SetObject                               = NV_DD_IMAGE_BLIT;
    nv->subchannel[NV_DD_BLIT].nv4ImageBlit.SetContextDmaNotifies      = NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[NV_DD_BLIT].nv4ImageBlit.SetContextClipRectangle    = NV_DD_IMAGE_BLACK_RECTANGLE;
    nv->subchannel[NV_DD_BLIT].nv4ImageBlit.SetContextPattern          = NV_DD_IMAGE_PATTERN;
    nv->subchannel[NV_DD_BLIT].nv4ImageBlit.SetContextRop              = NV_DD_ROP5_SOLID;
    nv->subchannel[NV_DD_BLIT].nv4ImageBlit.SetContextSurfaces         = NV_DD_PRIMARY_SURFACE;
    nv->subchannel[NV_DD_BLIT].nv4ImageBlit.SetOperation               = NV05F_SET_OPERATION_ROP_AND;
    nv->subchannel[NV_DD_BLIT].nv4ImageBlit.SetContextColorKey         = NV_DD_IMAGE_SOLID;

    //**********************************************************************
    // Setup RECT and TEXT  (565 format)
    //**********************************************************************

    while (NvGetFreeCount(nv, 0) < 8*4);
    nv->subchannel[NV_DD_RECT_AND_TEXT].SetObject      = NV_DD_RENDER_RECT_AND_TEXT;
    nv->subchannel[NV_DD_RECT_AND_TEXT].nv4GdiRectangleText.SetContextDmaNotifies = NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[NV_DD_RECT_AND_TEXT].nv4GdiRectangleText.SetContextPattern = NV_DD_IMAGE_PATTERN;
    nv->subchannel[NV_DD_RECT_AND_TEXT].nv4GdiRectangleText.SetContextRop     = NV_DD_ROP5_SOLID;
    nv->subchannel[NV_DD_RECT_AND_TEXT].nv4GdiRectangleText.SetContextSurface = NV_DD_PRIMARY_SURFACE;
    nv->subchannel[NV_DD_RECT_AND_TEXT].nv4GdiRectangleText.SetOperation      = NV04A_SET_OPERATION_ROP_AND;

    // Determine the color format
    switch (ppdev->iBitmapFormat)
        {
        case BMF_32BPP:
            nv->subchannel[NV_DD_RECT_AND_TEXT].nv4GdiRectangleText.SetColorFormat = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        case BMF_16BPP:
            nv->subchannel[NV_DD_RECT_AND_TEXT].nv4GdiRectangleText.SetColorFormat = NV04A_SET_COLOR_FORMAT_LE_X16R5G6B5;
            break;
        case BMF_8BPP:
            // Does hardware ignore this value???
            nv->subchannel[NV_DD_RECT_AND_TEXT].nv4GdiRectangleText.SetColorFormat = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        default:
            return(FALSE);
        }

    nv->subchannel[NV_DD_RECT_AND_TEXT].nv4GdiRectangleText.SetMonochromeFormat = NV04A_SET_MONOCHROME_FORMAT_CGA6_M1;

    // if 8bpp, create colormap
    if (ppdev->iBitmapFormat == BMF_8BPP)
        {
        while (NvGetFreeCount(nv, 0) < 9*4);
        nv->subchannel[NV_DD_SPARE].SetObject = NV_DD_SHARED_VIDEO_COLORMAP;
        nv->subchannel[NV_DD_SPARE].NvVideoColormap.SetVideoOutput =
            NV_DD_P_V_SHARED_VIDEO_COLORMAP;
        nv->subchannel[NV_DD_SPARE].NvVideoColormap.SetVideoInput =
            DD_PATCHCORD_VIDEO;
        nv->subchannel[NV_DD_SPARE].NvVideoColormap.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[NV_DD_SPARE].NvVideoColormap.SetContextDmaColormap =
            NV_DD_WIN_COLORMAP_CONTEXT;
        nv->subchannel[NV_DD_SPARE].NvVideoColormap.SetColormapStart = 0;
        nv->subchannel[NV_DD_SPARE].NvVideoColormap.Notify = 0;
        nv->subchannel[NV_DD_SPARE].NvVideoColormap.SetColormapLength =
            256 * sizeof(VIDEO_CLUTDATA);

        }

    //************************************************************************
    // Create the video memory to system memory format object.
    //************************************************************************

    while (NvGetFreeCount(nv, 0) < 4*4);
    nv->subchannel[NV_DD_SPARE].SetObject = NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT;
    nv->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.SetContextDmaNotifies =
        NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.SetContextDmaBufferIn =
        NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
    nv->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.SetContextDmaBufferOut =
        NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

    //**************************************************************************
    // create memory to memory format object used to reformat overlay surfaces
    //**************************************************************************

    while (NvGetFreeCount(nv, 0) < 4*4);
    nv->subchannel[NV_DD_SPARE].SetObject =
       NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
    nv->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.SetContextDmaNotifies =
       NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.SetContextDmaBufferIn =
       NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
    nv->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.SetContextDmaBufferOut =
       NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;

    // video overlay class
    while (NvGetFreeCount(nv, NV_DD_SPARE) < 4*4);

    nv->subchannel[NV_DD_SPARE].SetObject = NV_DD_VIDEO_OVERLAY;
    nv->subchannel[NV_DD_SPARE].Nv04VideoOverlay.SetContextDmaNotifies =
        NV_DD_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[NV_DD_SPARE].Nv04VideoOverlay.SetContextDmaOverlay[0] =
        NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
    nv->subchannel[NV_DD_SPARE].Nv04VideoOverlay.SetContextDmaOverlay[1] =
        NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;

    if (ppdev->cBitsPerPel > 8)
        {
        //*****************************************************************************
        // Hook up scaled image from mem objects.
        //*****************************************************************************
        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextSurface =
            NV_DD_PRIMARY_SURFACE;
        if (ppdev->cBitsPerPel == 16)
            nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
                NV077_SET_COLOR_FORMAT_LE_R5G6B5;
        else
            nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
                NV077_SET_COLOR_FORMAT_LE_X8R8G8B8;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaImage =
            NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextSurface =
            NV_DD_PRIMARY_SURFACE;
        if (ppdev->cBitsPerPel == 16)
            nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
                NV077_SET_COLOR_FORMAT_LE_R5G6B5;
        else
            nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
                NV077_SET_COLOR_FORMAT_LE_X8R8G8B8;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaImage =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextSurface =
            NV_DD_PRIMARY_SURFACE;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
            NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;

        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaImage =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextSurface =
            NV_DD_PRIMARY_SURFACE;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
            NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaImage =
            NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextSurface =
            NV_DD_PRIMARY_SURFACE;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
            NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaImage =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 7*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextSurface =
            NV_DD_PRIMARY_SURFACE;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
            NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaImage =
            NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
            NV077_SET_COLOR_FORMAT_LE_V8YB8U8YA8;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetOperation =
            NV077_SET_OPERATION_SRCCOPY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextSurface =
            NV_DD_PRIMARY_SURFACE;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
            NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaImage =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextSurface =
            NV_DD_PRIMARY_SURFACE;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
            NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaImage =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextSurface =
            NV_DD_PRIMARY_SURFACE;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
            NV077_SET_COLOR_FORMAT_LE_V8YB8U8YA8;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaImage =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaNotifies =
            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextSurface =
            NV_DD_PRIMARY_SURFACE;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetColorFormat =
            NV077_SET_COLOR_FORMAT_LE_V8YB8U8YA8;
        nv->subchannel[NV_DD_SPARE].nv4ScaledImageFromMemory.SetContextDmaImage =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
#if 0
    // BUGBUG I don't think YUV420 object is needed. BUGBUG
        while (NvGetFreeCount(nv, 0) < 4*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_FLOATING0_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nvScaledYuv420FromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nvScaledYuv420FromMemory.SetImageOutput =
            NV_DD_P_I_FLOATING0_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nvScaledYuv420FromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 4*4);

        nv->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_FLOATING1_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nvScaledYuv420FromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[NV_DD_SPARE].nvScaledYuv420FromMemory.SetImageOutput =
            NV_DD_P_I_FLOATING1_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[NV_DD_SPARE].nvScaledYuv420FromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
#endif
        }

    while (NvGetFreeCount(nv, 0) < 8*4);
    nv->subchannel[NV_DD_STRETCH].SetObject = NV_DD_STRETCHED_IMAGE_FROM_CPU;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetContextDmaNotifies = NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetContextColorKey = NV_DD_IMAGE_SOLID;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetContextPattern = NV_DD_IMAGE_PATTERN;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetContextRop     = NV_DD_ROP5_SOLID;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetOperation =
        NV076_SET_OPERATION_ROP_AND;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetContextSurface =
        NV_DD_PRIMARY_SURFACE;

    switch (ppdev->cBitsPerPel)
        {
        case 8:
            /* A8Y8 color format assumed by hardware when destination surface in 8bpp mode */
            /* Must be set to a legal value but hardware ignores it otherwise */
            nv->subchannel[NV_DD_SPARE].nv4StretchedImageFromCpu.SetColorFormat =
               NV076_SET_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        case 16:

            if (ppdev->flGreen == 0x03e0)
                nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetColorFormat =
                    NV076_SET_COLOR_FORMAT_LE_A1R5G5B5;
            else
                nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetColorFormat =
                    NV076_SET_COLOR_FORMAT_LE_R5G6B5;       // 5:6:5 format

            break;
        case 32:
            nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetColorFormat =
                NV076_SET_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        }
    ppdev->dDrawSpareSubchannelObject = 0;

    while (NvGetFreeCount(nv, 0) < 8*4);
    nv->subchannel[NV_DD_STRETCH].SetObject = NV_DD_STRETCHED_UV_IMAGE_FROM_CPU;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetContextPattern = NV_DD_IMAGE_PATTERN;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetContextRop     = NV_DD_ROP5_SOLID;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetOperation =
        NV076_SET_OPERATION_ROP_AND;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetContextSurface =
        NV_DD_PRIMARY_SURFACE;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetContextDmaNotifies = NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetContextColorKey = NV_DD_IMAGE_SOLID;
    /* A8Y8 color format assumed by hardware when destination surface in 8bpp mode */
    /* Must be set to a legal value but hardware ignores it otherwise */
    nv->subchannel[NV_DD_STRETCH].nv4StretchedImageFromCpu.SetColorFormat =
        NV076_SET_COLOR_FORMAT_LE_X8R8G8B8;

#ifdef MJL_USE_VPP
    if (!createExtendedDDPatch(ppdev))
    {
        return (FALSE);
    }
#endif

    //******************************************************************************************
    // End of video object creation.
    //******************************************************************************************
    ppdev->bDDChannelActive = TRUE;

    pSyncNotifier[NV04A_NOTIFIERS_NOTIFY].status = 0;

    return(TRUE);
}


NvU8 bDestroyNVDDPatch(PDEV    *ppdev)
{
    ULONG status;

    //*********************************************************************************
    // Ensure DD channel processing has completed prior to destroying objects
    // and DMA contexts.
    //*********************************************************************************
    NV4_DdPioSync(ppdev);

    //*********************************************************************************
    // Ensure all processing has completed prior to destroying objects.
    //*********************************************************************************
    ppdev->pfnWaitForChannelSwitch(ppdev);
    ppdev->pfnWaitEngineBusy(ppdev);

    //**********************************************************************************
    // Close ddraw channel.
    //**********************************************************************************
    status = NvFree(ppdev->hDriver,
                ppdev->hClient,
                ppdev->hDevice,
                ppdev->hDdChannel);

    if (status != NVOS00_STATUS_SUCCESS)
        {
        DPF("NVDD: Cannot free NV ddraw PIO channel");
        NvClose(ppdev->hDriver);
        return FALSE;
        }

    //********************************************************************************
    // If the pjMmBase ptr was alloc'd by ddraw, clear it.
    //********************************************************************************
    if (ppdev->pjMmBase == ppdev->ddChannelPtr)
        {
        ppdev->pjMmBase = NULL;
        }

    ppdev->ddChannelPtr = NULL;

    //**************************************************************************
    // Free DMA context which points to all of video memory.
    //**************************************************************************
    if (NvFree(  ppdev->hDriver,ppdev->hClient, ppdev->hDevice, NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM) !=
            NVOS00_STATUS_SUCCESS )
        {
        DPF("NVDD: Cannot free dma in memory context");
        return FALSE;
        }

    //******************************************************************************
    // Free ddraw notifier context
    //******************************************************************************
    if (NvFree(  ppdev->hDriver,
                            ppdev->hClient, ppdev->hDevice, NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY) != NVOS00_STATUS_SUCCESS )
        {
        DPF("NVDD: Cannot free notifier context");
        return FALSE;
        }

    //******************************************************************************
    // Free flip notifier context
    //******************************************************************************
    if (NvFree(  ppdev->hDriver,
                            ppdev->hClient, ppdev->hDevice, NV_DD_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY) != NVOS00_STATUS_SUCCESS )
        {
        DPF("NVDD: Cannot free notifier context");
        return FALSE;
        }

    //******************************************************************************
    // Free flip overlay notifier context
    //******************************************************************************
    if (NvFree(  ppdev->hDriver,
                            ppdev->hClient, ppdev->hDevice, NV_DD_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY) != NVOS00_STATUS_SUCCESS )
        {
        DPF("NVDD: Cannot free notifier context");
        return FALSE;
        }

    //******************************************************************************
    // Free sync notifier context
    //******************************************************************************
    if (NvFree(  ppdev->hDriver,
                            ppdev->hClient, ppdev->hDevice, DD_FIFO_SYNC_NOTIFIER) != NVOS00_STATUS_SUCCESS )
        {
        DPF("NVDD: Cannot free notifier context");
        return FALSE;
        }



    //***********************************************************************************
    // Free scanline buffer.
    //***********************************************************************************
    if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY))
        DPF("NVDD: Cannot free scanline buffer context");
    EngFreeMem(ppdev->NvDmaBufferFlat);
    ppdev->NvScanlineBufferFlat = ppdev->NvDmaBufferFlat = 0;

    //***********************************************************************************
    // Free palette DMA context.
    //***********************************************************************************
    if (ppdev->cBitsPerPel == 8)
        {
        if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_DD_WIN_COLORMAP_CONTEXT))
            DPF("NVDD: Cannot free palette context");
        }

#ifdef MJL_USE_VPP
    destroyExtendedDDPatch(ppdev);
#endif

    ppdev->bDDChannelActive = FALSE;

    return(TRUE);
}



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
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

//---------------------------------------------------------------------------

// attempt allocation and return if unsuccessful

#define NVDD_ALLOCOBJECT(channel, handle, class)                                            \
{                                                                                           \
    DWORD dwStatus;                                                                         \
    dwStatus = NvRmAllocObject (pDriverData->dwRootHandle, (channel), (handle), (class));   \
    if (dwStatus != NVOS05_STATUS_SUCCESS) {                                                \
        dbgTracePop();                                                                      \
        return (FALSE);                                                                     \
    }                                                                                       \
}

//---------------------------------------------------------------------------

void nvCreateVidHeapVarsAndPseudoNotifier (void)
{
    dbgTracePush ("nvCreateVidHeapVarsAndPseudoNotifier");

    // The heap is already consistent. Everyone makes their allocations
    // through the RM. Hence, the heap variables can be initted simply.
    // It is simply all of video memory!
    //
    // Unfortunately, WHQL thinks that right after a DirectDraw init
    // (i.e. a CreateDriverObject) or right after a modeset, the video
    // heap total and free should be equal and complains with a failure
    // if they are not. Hence, I am going to set both the total and the
    // free to be the amount of free memory available for DirectX. This
    // is not perfect, since either people may be making allocations from
    // the video heap directly from the RM and hence video memory
    // "disappears" faster than an app might expect it to, but too bad.
    NVHEAP_INFO ();
    pDriverData->BaseAddress    = HeapParams.address;
    pDriverData->VideoHeapBase  = pDriverData->BaseAddress;
    pDriverData->VideoHeapTotal = HeapParams.total;

#ifdef WINNT
    // For Win2K, include any memory currently in use by the GDI driver as
    // being available for DX usage. The driver defers freeing the GDI alloc'd
    // memory until it is required for a DX memory allocation.
    pDriverData->VideoHeapFree  = HeapParams.free + ppdev->cbGdiHeap;
#else // !WINNT
    pDriverData->VideoHeapFree  = HeapParams.free;
#endif // !WINNT

    // This is what the VideoHeapEnd should be
    // pDriverData->VideoHeapEnd   = HeapParams.address + HeapParams.total - 1;
    // However, for right now, the way other code in this driver works,
    // I have to make the VideoHeapEnd end in FFF, so let's massage it some.
    pDriverData->VideoHeapEnd = ((HeapParams.address + HeapParams.total) & ~0xFFF) - 1;

    dbgTracePop();
}

//---------------------------------------------------------------------------

// Allocate DMA notifiers and pixel/texel buffers
//
// Request 8 pages (NV_DD_COMMON_DMA_BUFFER_SIZE bytes) of memory.
// Allows 2 buffers where 1 buffer can use
// (NV_DD_COMMON_DMA_BUFFER_SIZE - (sizeof(NvNotification) << 5))/2 bytes
// with up to 32 separate notifiers at the beginning of the buffer.
//
// Actual memory allocation done in 16 bit portion of driver.

BOOL nvEnableNotifiers (void)
{
    dbgTracePush ("nvEnableNotifiers");

    NvNotification *pNotifiers = (NvNotification *)pDriverData->NvCommonDmaBufferFlat;

    // Notifier[0]
    pDriverData->pDmaDmaToMemNotifierFlat           = &pNotifiers[0];

    // Notifier[1] actually [0 - 1]
    pDriverData->pDmaBufferNotifierFlat             = &pNotifiers[0];

    // Notifier[2]
    pDriverData->pGenericScaledImageNotifier        = &pNotifiers[2];

    // Notifier[3]
    pDriverData->pDmaPusherSyncNotifierFlat         = &pNotifiers[3];

    // Notifier[5]
    pDriverData->pDmaSyncNotifierFlat               = &pNotifiers[5];

    // Notifier[6-7]
    pDriverData->pPioColourControlEventNotifierFlat = &pNotifiers[6];

    // Notifier[8]
    pDriverData->pPioCombinerEventNotifierFlat      = &pNotifiers[8];

    // Notifier memory [9 - 25] available for use

    // On NV4 Notifier[26 - 30], on NV10 Notifier[26 - 28]
    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
        pDriverData->pPioFlipOverlayNotifierFlat    = (NvNotification*)(pDriverData->NvVideoNotifierBuffer + (sizeof(NvNotification) * (16 + 8 + 2)));
    }
    else {
        pDriverData->pPioFlipOverlayNotifierFlat    = &pNotifiers[16 + 8 + 2];
    }

    // On NV4 Notifier[32 - 36], on NV10 Notifier[32 - 40]
    // multi-head: head0 uses [32 - 40], head1 uses [48 - 56], etc.
    pDriverData->pDmaFlipPrimaryNotifierFlat            = &pNotifiers[32];

    // Scanline buffer starts after array of 32 notifiers
    pDriverData->NvScanlineBufferFlat = pDriverData->NvCommonDmaBufferFlat + NV_DD_NOTIFIER_BLOCK_SIZE;

    NvNotification *pDmaDmaToMemNotifier           = pDriverData->pDmaDmaToMemNotifierFlat;
    NvNotification *pDmaPusherSyncNotifier         = pDriverData->pDmaPusherSyncNotifierFlat;
    NvNotification *pDmaSyncNotifier               = pDriverData->pDmaSyncNotifierFlat;
    NvNotification *pDmaFlipPrimaryNotifier        = pDriverData->pDmaFlipPrimaryNotifierFlat;
    NvNotification *pPioFlipOverlayNotifier        = pDriverData->pPioFlipOverlayNotifierFlat;
    NvNotification *pPioColourControlEventNotifier = pDriverData->pPioColourControlEventNotifierFlat;
    NvNotification *pPioCombinerEventNotifier      = pDriverData->pPioCombinerEventNotifierFlat;

    // Deal with possible mode change during pending flip
    if ((volatile)pDmaFlipPrimaryNotifier[1].status == NV_IN_PROGRESS) {
        long countDown = 0x200000;
        while (((volatile)pDmaFlipPrimaryNotifier[1].status == NV_IN_PROGRESS) && (--countDown > 0));
    }
    // Deal with possible mode change during pending flip
    if ((volatile)pDmaFlipPrimaryNotifier[2].status == NV_IN_PROGRESS) {
        long countDown = 0x200000;
        while (((volatile)pDmaFlipPrimaryNotifier[2].status == NV_IN_PROGRESS) && (--countDown > 0));
    }
    // Deal with possible mode change during pending flip
    if ((volatile)pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) {
        long countDown = 0x200000;
        while (((volatile)pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) && (--countDown > 0));
    }
    // Deal with possible mode change during pending flip
    if ((volatile)pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS) {
        long countDown = 0x200000;
        while (((volatile)pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS) && (--countDown > 0));
    }

    // clear all status flags before first use
    pDmaDmaToMemNotifier->status = 0;
    pDriverData->pGenericScaledImageNotifier->status = 0;
    pDmaSyncNotifier->status = 0;

    pDmaFlipPrimaryNotifier[0].status = 0;
    pDmaFlipPrimaryNotifier[1].status = 0;
    pDmaFlipPrimaryNotifier[2].status = 0;

    pPioFlipOverlayNotifier[0].status = 0;
    pPioFlipOverlayNotifier[1].status = 0;
    pPioFlipOverlayNotifier[2].status = 0;

    pPioColourControlEventNotifier->status = 0;
    pPioCombinerEventNotifier->status = 0;

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

//
//    nvCreateNotifers ()
//    This routine creates all the notifiers to be used by DirectDraw.
//    Basically they are the notifiers and the scanline buffer.
//

DWORD __stdcall nvCreateNotifiers (void)
{
    dbgTracePush ("nvCreateNotifiers");

    DWORD dwSize = NV_DD_COMMON_DMA_BUFFER_SIZE + 0x1000 - 1;

    // Allocate memory for a bunch of notifiers and the scanline buffer.
    if (!pDriverData->NvCommonDmaBufferFlat) {
        pDriverData->NvCommonDmaBufferFlat = (DWORD)AllocIPM(dwSize);
        if (!pDriverData->NvCommonDmaBufferFlat) {
            dbgTracePop();
            return FALSE;
        }
    }

    // NV10 has some extra things
//    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10)
        pDriverData->NvVideoNotifierBuffer = pDriverData->NvCommonDmaBufferFlat + NV_DD_COMMON_DMA_BUFFER_SIZE;

    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY),
                             (PVOID) pDriverData->NvCommonDmaBufferFlat,
                             sizeof(NvNotification) - 1)) {
        dbgTracePop();
        return FALSE;
    }

    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             NV_DD_DMA_CONTEXT_DMA_TO_SYSTEM_MEMORY,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY),
                             (PVOID) pDriverData->NvCommonDmaBufferFlat, NV_DD_COMMON_DMA_BUFFER_SIZE - 1)) {
        dbgTracePop();
        return FALSE;
    }

    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_ONLY),
                             (PVOID) pDriverData->NvCommonDmaBufferFlat, NV_DD_COMMON_DMA_BUFFER_SIZE - 1)) {
        dbgTracePop();
        return FALSE;
    }

    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY),
                             (PVOID) pDriverData->NvCommonDmaBufferFlat,
                             2 * sizeof(NvNotification) - 1)) {
        dbgTracePop();
        return FALSE;
    }

    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             NV_DD_SCALED_IMAGE_CONTEXT_IID,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY),
                             (PVOID) (pDriverData->NvCommonDmaBufferFlat + 2 * sizeof(NvNotification)),
                             sizeof(NvNotification) - 1)) {
        dbgTracePop();
        return FALSE;
    }

    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY),
                             (PVOID) (pDriverData->NvCommonDmaBufferFlat + 3 * sizeof(NvNotification)),
                             2 * sizeof(NvNotification) - 1)) {
        dbgTracePop();
        return FALSE;
    }

    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY),
                             (PVOID) (pDriverData->NvCommonDmaBufferFlat + 5 * sizeof(NvNotification)),
                             sizeof(NvNotification) - 1)) {
        dbgTracePop();
        return FALSE;
    }

    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             NV_DD_PIO_COLOUR_CONTROL_EVENT_NOTIFIER,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY),
                             (PVOID) (pDriverData->NvCommonDmaBufferFlat + 6 * sizeof(NvNotification)),
                             2 * sizeof(NvNotification) - 1)) {
        dbgTracePop();
        return FALSE;
    }

    if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                             NV_DD_PIO_COMBINER_EVENT_NOTIFIER,
                             NV01_CONTEXT_DMA,
                             DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY),
                             (PVOID) (pDriverData->NvCommonDmaBufferFlat + 8 * sizeof(NvNotification)),
                             sizeof(NvNotification) - 1)) {
        dbgTracePop();
        return FALSE;
    }

    for(DWORD dwHead = 0; dwHead < pDriverData->dwHeads; dwHead++) {
        if (NvRmAllocContextDma (pDriverData->dwRootHandle,
                                 NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY + dwHead,
                                 NV01_CONTEXT_DMA,
                                 DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY),
                                 (PVOID) (pDriverData->NvCommonDmaBufferFlat + (32 + 16 * dwHead) * sizeof(NvNotification)),
                                 9 * sizeof(NvNotification) - 1)) {
            dbgTracePop();
            return FALSE;
        }
    }

    dbgTracePop();
    return TRUE;
}

//---------------------------------------------------------------------------

//
//    nvDestroyNotifiers ()
//
//    This routine destroys all the DMA contexts used by DirectDraw
//    Basically they are the notifiers and the scanline buffer
//

DWORD __stdcall nvDestroyNotifiers (void)
{
    dbgTracePush ("nvDestroyNotifiers");

    // free dma contexts
    // free dma contexts
    NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_SCALED_IMAGE_CONTEXT_IID);
    NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_PIO_COLOUR_CONTROL_EVENT_NOTIFIER);
    NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_PIO_COMBINER_EVENT_NOTIFIER);

    for(DWORD dwHead = 0; dwHead < pDriverData->dwHeads; dwHead++)
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY + dwHead);

    NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_CONTEXT_DMA_TO_SYSTEM_MEMORY);
    NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY);

    dbgTracePop();
    return TRUE;
}

//---------------------------------------------------------------------------

BOOL nvCreateDACObjects (DWORD dwChannel, DWORD dwHandle)
{
    dbgTracePush ("nvCreateDAC");

    // create the best NV_DD_VIDEO_LUT_CURSOR_DAC object we can
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_007C_DAC) {
        for (DWORD dwHead = 0; dwHead < pDriverData->dwHeads; dwHead ++) {
            NV07C_ALLOCATION_PARAMETERS nv07cAllocParms;
            nv07cAllocParms.logicalHeadId = dwHead;
#ifndef WINNT
            if (pDriverData->dwDesktopState != NVTWINVIEW_STATE_DUALVIEW) {
                // RM needs physical head, map logical to physical
                NVTWINVIEWDATA twinData;
                twinData.dwFlag   = NVTWINVIEW_FLAG_GET;
                twinData.dwAction = NVTWINVIEW_ACTION_MAP_LOG_TO_PHYS_HEAD;
                twinData.dwCrtc   = nv07cAllocParms.logicalHeadId;
                MyExtEscape (pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);
                nv07cAllocParms.logicalHeadId = twinData.dwCrtc;
            }
#endif
            // Logically, this next line of code should be executed.
            // It maps the logical head number to a physical one.
            // The other pieces of code that deal with this object
            // probably compensate for this irregularity.    PG 04/09/2001
            //nv07cAllocParms.logicalHeadId = ppdev->ulDeviceDisplay[dwHead];
            if (NvRmAlloc (pDriverData->dwRootHandle, dwChannel,
                           dwHandle + dwHead, NV15_VIDEO_LUT_CURSOR_DAC,
                           &nv07cAllocParms) != NVOS21_STATUS_SUCCESS) {
                dbgTracePop();
                return (FALSE);
            }
        }
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0067_DAC) {
        NVDD_ALLOCOBJECT (dwChannel, dwHandle, NV10_VIDEO_LUT_CURSOR_DAC);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0049_DAC) {
        NVDD_ALLOCOBJECT (dwChannel, dwHandle, NV05_VIDEO_LUT_CURSOR_DAC);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0046_DAC) {
        NVDD_ALLOCOBJECT (dwChannel, dwHandle, NV04_VIDEO_LUT_CURSOR_DAC);
    }
    else {
        DPF ("hardware doesn't seem to support any flavor of the VIDEO_LUT_CURSOR_DAC class!");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    // clear the flip counters and the notifiers associated therewith
    for (DWORD dwHead = 0; dwHead < pDriverData->dwHeads; dwHead ++) {
        pDriverData->dwCurrentFlip[dwHead]   = 0;
        pDriverData->dwCompletedFlip[dwHead] = 0;
    }

    // clear the flip-primary notifier memory
    memset((void*)pDriverData->pDmaFlipPrimaryNotifierFlat, 0, pDriverData->dwHeads * 16 * sizeof(NvNotification));

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL nvEnableDDraw (void)
{
    DWORD dwPixelDepth;

    dbgTracePush ("nvAllocateDDrawObjects");

    dwPixelDepth = getDC()->dwEnablePixelDepth;

    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_VIDEO_MEMORY_TO_SYSTEM_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_CONTEXT_ROP, NV3_CONTEXT_ROP);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_CONTEXT_PATTERN, NV4_CONTEXT_PATTERN);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_IMAGE_BLACK_RECTANGLE, NV1_IMAGE_BLACK_RECTANGLE);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_CONTEXT_COLOR_KEY, NV4_CONTEXT_COLOR_KEY);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_CONTEXT_BETA4, NV4_CONTEXT_BETA);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_ROP_GDI_RECT_AND_TEXT, NV4_GDI_RECTANGLE_TEXT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_IMAGE_FROM_CPU, NV4_IMAGE_FROM_CPU);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_GENERIC_STRETCHED_IMAGE, (pDriverData->fDeviceHasDitheringControl ?
                                                                     NV05_STRETCHED_IMAGE_FROM_CPU :
                                                                     NV04_STRETCHED_IMAGE_FROM_CPU));
    DWORD dwScaledImageFromMemClass;
    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
        pDriverData->fDeviceHasDitheringControl = TRUE;
        dwScaledImageFromMemClass = NV10_SCALED_IMAGE_FROM_MEMORY;
    } else if (pDriverData->fDeviceHasDitheringControl) {
        pDriverData->fDeviceHasDitheringControl = TRUE;
        dwScaledImageFromMemClass = NV05_SCALED_IMAGE_FROM_MEMORY;
    } else {
        pDriverData->fDeviceHasDitheringControl = FALSE;
        dwScaledImageFromMemClass = NV4_SCALED_IMAGE_FROM_MEMORY;
    }
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_SCALED_IMAGE_IID, dwScaledImageFromMemClass);

    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_IMAGE_BLIT, NV4_IMAGE_BLIT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_SRCCOPY_IMAGE_BLIT, NV4_IMAGE_BLIT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_SRCCOPY_PREMULT_A8R8G8B8_IMAGE_BLIT, NV4_IMAGE_BLIT);

    // create the best NV_DD_SURFACES_2D we can
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0062_CTXSURF2D) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_SURFACES_2D, NV10_CONTEXT_SURFACES_2D);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0042_CTXSURF2D) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_SURFACES_2D, NV4_CONTEXT_SURFACES_2D);
    }
    else {
        DPF ("hardware doesn't seem to support any CONTEXT_SURFACES_2D class!");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    // create the best NV_DD_INDEXED_IMAGE_FROM_CPU we can
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0064_IDXIMAGE) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_INDEXED_IMAGE_FROM_CPU, NV05_INDEXED_IMAGE_FROM_CPU);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0060_IDXIMAGE) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_INDEXED_IMAGE_FROM_CPU, NV04_INDEXED_IMAGE_FROM_CPU);
    }
    else {
        DPF ("hardware doesn't seem to support any INDEXED_IMAGE class!");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    // make sure we have the correct desktop state, this can change if the user presses the f8 hotkey
    // on mobile systems
#ifdef WINNT
    pDriverData->dwDesktopState = ppdev->TwinView_State;
#else
    NVTWINVIEWDATA twinViewData;
    memset(&twinViewData, 0, sizeof(twinViewData));
    twinViewData.dwFlag   = NVTWINVIEW_FLAG_GET;
    twinViewData.dwAction = NVTWINVIEW_ACTION_SETGET_STATE;
    MyExtEscape(pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, 0, NULL, sizeof(twinViewData), (char*)&twinViewData);
    nvAssert(twinViewData.dwSuccess);
    pDriverData->dwDesktopState = twinViewData.dwState;
#endif

    // initialise and reset the frame tracker
    getDC()->pBlitTracker->create(0, FALSE);
    getDC()->dwCurrentBlit = 0;

    // create the regular set of DAC objects
    if (!nvCreateDACObjects (NV_DD_DEV_DMA, NV_DD_VIDEO_LUT_CURSOR_DAC)) {
        dbgTracePop();
        return (FALSE);
    }

    // create another set of DAC object for the secondary DAC channel used on nv20
#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        if (!nvCreateDACObjects (NV_DD_DEV_DAC, NV_DD_VIDEO_LUT_CURSOR_DAC_2)) {
            dbgTracePop();
            return (FALSE);
        }
    }
#endif
#endif

    if (dwPixelDepth == 8) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_P_V_SHARED_VIDEO_COLORMAP, NV_PATCHCORD_VIDEO);
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_SHARED_VIDEO_COLORMAP, NV_VIDEO_COLORMAP);
    }

    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_GENERIC_MTMF_1, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_GENERIC_MTMF_2, NV3_MEMORY_TO_MEMORY_FORMAT);
    NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, NV_DD_VIDEO_SINK, NV_VIDEO_SINK);

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL nvEnableContextDMAs (void)
{
    dbgTracePush ("nvAllocateContextDMAs");

    // Free any existing dynamic context dma objects
    if (pDriverData->dwVidMemCtxDmaSize != 0) {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY);
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY);
        pDriverData->dwVidMemCtxDmaSize = 0;
    }

    if (pDriverData->dwMCIDCTAGPCtxDmaSize[0] != 0) {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, NV_DD_DMA_FLOATING_MC_IDCT0_CONTEXT_DMA_FROM_MEMORY);
        pDriverData->dwMCIDCTAGPCtxDmaSize[0] = 0;
    }

    if (pDriverData->dwMCIDCTAGPCtxDmaSize[1] != 0) {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, NV_DD_DMA_FLOATING_MC_IDCT1_CONTEXT_DMA_FROM_MEMORY);
        pDriverData->dwMCIDCTAGPCtxDmaSize[1] = 0;
    }

    if (pDriverData->dwMCIDCTAGPCtxDmaSize[2] != 0) {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, NV_DD_DMA_FLOATING_MC_IDCT2_CONTEXT_DMA_FROM_MEMORY);
        pDriverData->dwMCIDCTAGPCtxDmaSize[2] = 0;
    }

    if (pDriverData->dwMCIDCTAGPCtxDmaSize[3] != 0) {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, NV_DD_DMA_FLOATING_MC_IDCT3_CONTEXT_DMA_FROM_MEMORY);
        pDriverData->dwMCIDCTAGPCtxDmaSize[3] = 0;
    }

    // Allocate our video memory context dmas
    pDriverData->dwVidMemCtxDmaSize = (DWORD)(pDriverData->VideoHeapEnd - pDriverData->BaseAddress);

    NvRmAllocContextDma(pDriverData->dwRootHandle,
                        NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,
                        NV01_CONTEXT_DMA,
                        DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                        DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                        (PVOID) VIDMEM_ADDR(pDriverData->BaseAddress),
                        pDriverData->dwVidMemCtxDmaSize);

    NvRmAllocContextDma(pDriverData->dwRootHandle,
                        NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY,
                        NV01_CONTEXT_DMA,
                        DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_ONLY) |
                        DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                        (PVOID) VIDMEM_ADDR(pDriverData->BaseAddress),
                        pDriverData->dwVidMemCtxDmaSize);

    NvRmAllocContextDma(pDriverData->dwRootHandle,
                        NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY,
                        NV01_CONTEXT_DMA,
                        DRF_DEF(OS03, _FLAGS, _ACCESS, _WRITE_ONLY) |
                        DRF_DEF(OS03, _FLAGS, _COHERENCY, _UNCACHED),
                        (PVOID) VIDMEM_ADDR(pDriverData->BaseAddress),
                        pDriverData->dwVidMemCtxDmaSize);

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL nvInitDDrawObjects (void)
{
    dbgTracePush ("nvInitDDrawObjects");

    // set up pointer to DDHALINFO
    LPDDHALINFO pHalInfo = GET_HALINFO();

    DWORD dwPixelDepth = getDC()->dwEnablePixelDepth;

    nvPushData (0,dDrawSubchannelOffset(NV_DD_IMAGE) | 0x40000);
    nvPushData (1,NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_IMAGE) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
    nvPushData (3,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);

    nvPushData (4,dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
    nvPushData (5,NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT);
    nvPushData (6,dDrawSubchannelOffset(NV_DD_STRETCH) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
    nvPushData (7,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);

    nvPushData (8,dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    nvPushData (9,NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT);
    nvPushData (10,dDrawSubchannelOffset(NV_DD_SPARE) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
    nvPushData (11,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);

    nvPushData (12,dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    nvPushData (13,NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT);
    nvPushData (14,dDrawSubchannelOffset(NV_DD_SPARE) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
    nvPushData (15,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);

    nvPusherAdjust (16);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) | 0x40000);
    nvPushData (1,NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
    nvPushData (3,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    nvPushData (4,NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY);
    nvPushData (5,NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY);

    nvPushData (6,dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000);
    nvPushData (7,NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT);
    nvPushData (8,dDrawSubchannelOffset(NV_DD_SURFACES) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
    nvPushData (9,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    nvPushData (10,NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY);
    nvPushData (11,NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY);

    nvPushData (12,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) | 0x40000);
    nvPushData (13,NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT);
    nvPushData (14,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
    nvPushData (15,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    nvPushData (16,NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY);
    nvPushData (17,NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY);

    nvPushData (18,dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000);
    nvPushData (19,NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT);
    nvPushData (20,dDrawSubchannelOffset(NV_DD_BLIT) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
    nvPushData (21,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    nvPushData (22,NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY);
    nvPushData (23,NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY);

    nvPusherAdjust (24);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) | 0x40000);
    nvPushData (1,NV_DD_GENERIC_MTMF_1);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
    nvPushData (3,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);

    nvPushData (4,dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000);
    nvPushData (5,NV_DD_GENERIC_MTMF_2);
    nvPushData (6,dDrawSubchannelOffset(NV_DD_SURFACES) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
    nvPushData (7,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);

    nvPusherAdjust(8);

    for (DWORD dwHead = 0; dwHead < pDriverData->dwHeads; dwHead ++) {
        // initialize LUT-cursor-DAC object
        nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) | 0x40000);
        nvPushData (1,NV_DD_VIDEO_LUT_CURSOR_DAC + dwHead);
        nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP) + NV067_SET_CONTEXT_DMA_NOTIFIES | 0x1C0000);
        nvPushData (3,NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY + dwHead);
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

    /*
     * Connect buffer patch objects
     */
    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) | 0x40000);
    nvPushData (1,NV_DD_SURFACES_2D);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV042_SET_COLOR_FORMAT | 0x40000);

    if (dwPixelDepth == 8) {
        nvPushData (3,NV042_SET_COLOR_FORMAT_LE_Y8);
        pDriverData->bltData.dwLastColourFormat = NV042_SET_COLOR_FORMAT_LE_Y8;
    }
    else if (dwPixelDepth == 16) {
        nvPushData (3,NV042_SET_COLOR_FORMAT_LE_R5G6B5);
        pDriverData->bltData.dwLastColourFormat = NV042_SET_COLOR_FORMAT_LE_R5G6B5;
    }
    else {
        nvAssert (dwPixelDepth == 32);
        nvPushData (3,NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8);
        pDriverData->bltData.dwLastColourFormat = NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;
    }

    nvPushData (4,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV042_SET_CONTEXT_DMA_IMAGE_SOURCE | 0x80000);
    nvPushData (5,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    nvPushData (6,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    nvPushData (7,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV042_SET_PITCH | 0xC0000);
    nvPushData (8,(pHalInfo->vmiData.lDisplayPitch << 16) | pHalInfo->vmiData.lDisplayPitch);
    nvPushData (9,0);
    nvPushData (10,0);
    pDriverData->bltData.dwLastCombinedPitch = (pHalInfo->vmiData.lDisplayPitch << 16) |
                                          pHalInfo->vmiData.lDisplayPitch;
    pDriverData->bltData.dwLastSrcOffset = 0;
    pDriverData->bltData.dwLastDstOffset = 0;

    nvPusherAdjust (11);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) | 0x40000);
    nvPushData (1,NV_DD_IMAGE_BLIT);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP) + NV05F_SET_CONTEXT_COLOR_KEY | 0x40000);
    nvPushData (3,NV_DD_CONTEXT_COLOR_KEY);
//       NV_DD_IMAGE_BLACK_RECTANGLE);  // messes up D3D surface clip
    nvPushData (4,dDrawSubchannelOffset(NV_DD_ROP) + NV05F_SET_CONTEXT_PATTERN | 0x80000);
    nvPushData (5,NV_DD_CONTEXT_PATTERN);
    nvPushData (6,NV_DD_CONTEXT_ROP);
    nvPushData (7,dDrawSubchannelOffset(NV_DD_ROP) + NV05F_SET_OPERATION | 0x40000);
    nvPushData (8,NV05F_SET_OPERATION_ROP_AND);
    nvPushData (9,dDrawSubchannelOffset(NV_DD_ROP) + NV05F_SET_CONTEXT_SURFACES | 0x40000);
    nvPushData (10,NV_DD_SURFACES_2D);

    nvPusherAdjust (11);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) | 0x40000);
    nvPushData (1,NV_DD_SRCCOPY_IMAGE_BLIT);
//   nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP) +
//       NV05F_SET_CONTEXT_COLOR_KEY | 0x40000);   // destination alpha not supported if context color key set
//   nvPushData (3,NV_DD_CONTEXT_COLOR_KEY);
//       NV_DD_IMAGE_BLACK_RECTANGLE);  // messes up D3D surface clip
    nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP) + NV05F_SET_OPERATION | 0x40000);
    nvPushData (3,NV05F_SET_OPERATION_SRCCOPY);
    nvPushData (4,dDrawSubchannelOffset(NV_DD_ROP) + NV05F_SET_CONTEXT_SURFACES | 0x40000);
    nvPushData (5,NV_DD_SURFACES_2D);

    nvPusherAdjust (6);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) | 0x40000);
    nvPushData (1,NV_DD_SCALED_IMAGE_IID);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP) + NV063_SET_CONTEXT_DMA_NOTIFIES | 0x80000);
    nvPushData (3,NV_DD_SCALED_IMAGE_CONTEXT_IID);
    nvPushData (4,NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY); // SetContextDmaImage
    nvPushData (5,dDrawSubchannelOffset(NV_DD_ROP) + NV063_SET_CONTEXT_PATTERN | 0x80000);
    nvPushData (6,NV_DD_CONTEXT_PATTERN);
    nvPushData (7,NV_DD_CONTEXT_ROP);
    nvPushData (8,dDrawSubchannelOffset(NV_DD_ROP) + NV063_SET_OPERATION | 0x40000);
    nvPushData (9,NV063_SET_OPERATION_ROP_AND);
    nvPushData (10,dDrawSubchannelOffset(NV_DD_ROP) + NV063_SET_CONTEXT_SURFACE | 0x40000);
    nvPushData (11,NV_DD_SURFACES_2D);
    nvPushData (12,dDrawSubchannelOffset(NV_DD_ROP) + NV063_SET_COLOR_FORMAT | 0x40000);
    nvPushData (13,((dwPixelDepth==16) ? NV063_SET_COLOR_FORMAT_LE_R5G6B5 : NV063_SET_COLOR_FORMAT_LE_X8R8G8B8));

    if (pDriverData->fDeviceHasDitheringControl) {
        nvPushData (14,dDrawSubchannelOffset(NV_DD_ROP) + NV063_SET_COLOR_CONVERSION | 0x40000);
        nvPushData (15,NV063_SET_COLOR_CONVERSION_TRUNCATE);
        nvPusherAdjust (16);
    }
    else {
        nvPusherAdjust (14);
    }

    nvPushData (0,dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000);
    nvPushData (1,NV_DD_GENERIC_STRETCHED_IMAGE);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_SURFACES) | NV066_SET_CONTEXT_COLOR_KEY | 0xC0000);
    nvPushData (3,NV_DD_CONTEXT_COLOR_KEY);
    nvPushData (4,NV_DD_CONTEXT_PATTERN);
    nvPushData (5,NV_DD_CONTEXT_ROP);
    nvPushData (6,dDrawSubchannelOffset(NV_DD_SURFACES) | NV066_SET_OPERATION | 0x40000);
    nvPushData (7,NV066_SET_OPERATION_ROP_AND);
    nvPushData (8,dDrawSubchannelOffset(NV_DD_SURFACES) | NV066_SET_CONTEXT_SURFACE | 0x40000);
    nvPushData (9,NV_DD_SURFACES_2D);
    nvPushData (10,dDrawSubchannelOffset(NV_DD_SURFACES) | NV066_SET_COLOR_FORMAT | 0x40000);
    /* A8Y8 color format assumed by hardware when destination surface in 8bpp mode */
    /* Must be set to a legal value but hardware ignores it otherwise */
    nvPushData (11,NV066_SET_COLOR_FORMAT_LE_X8R8G8B8);
    nvPusherAdjust (12);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) | 0x40000);
    nvPushData (1,NV_DD_IMAGE_FROM_CPU);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV061_SET_CONTEXT_COLOR_KEY | 0x100000);
    nvPushData (3,NV_DD_CONTEXT_COLOR_KEY);
    nvPushData (4,NV_DD_IMAGE_BLACK_RECTANGLE);
    nvPushData (5,NV_DD_CONTEXT_PATTERN);
    nvPushData (6,NV_DD_CONTEXT_ROP);
    nvPushData (7,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV061_SET_CONTEXT_SURFACE | 0x40000);
    nvPushData (8,NV_DD_SURFACES_2D);
    nvPushData (9,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV061_SET_OPERATION | 0x80000);
    nvPushData (10,NV061_SET_OPERATION_ROP_AND);

    if (dwPixelDepth == 8) {
        // Y8 color format assumed by hardware when destination surface in 8bpp mode
        // Must be set to a legal value but hardware ignores it otherwise
        nvPushData (11,NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);
    }
    else if (dwPixelDepth == 16) {
        nvPushData (11,NV061_SET_COLOR_FORMAT_LE_R5G6B5);
    }
    else {
        nvAssert (dwPixelDepth == 32);
        nvPushData (11,NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);
    }

    nvPusherAdjust (12);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
    nvPushData (1,NV_DD_INDEXED_IMAGE_FROM_CPU);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_STRETCH) + NV064_SET_CONTEXT_DMA_LUT | 0x40000);
    nvPushData (3,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    nvPushData (4,dDrawSubchannelOffset(NV_DD_STRETCH) + NV064_SET_CONTEXT_CLIP_RECTANGLE | 0x40000);
    nvPushData (5,NV_DD_IMAGE_BLACK_RECTANGLE);
    nvPushData (6,dDrawSubchannelOffset(NV_DD_STRETCH) + NV064_SET_CONTEXT_SURFACE | 0x40000);
    nvPushData (7,NV_DD_SURFACES_2D);
    nvPushData (8,dDrawSubchannelOffset(NV_DD_STRETCH) + NV064_SET_COLOR_CONVERSION | 0x40000);
    nvPushData (9,NV064_SET_COLOR_CONVERSION_TRUNCATE);
    nvPushData (10,dDrawSubchannelOffset(NV_DD_STRETCH) + NV064_SET_OPERATION | 0x80000);
    nvPushData (11,NV064_SET_OPERATION_SRCCOPY);
    nvPushData (12,NV064_SET_COLOR_FORMAT_LE_X8R8G8B8);

    nvPusherAdjust (13);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) | 0x40000);
    nvPushData (1,NV_DD_ROP_GDI_RECT_AND_TEXT);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV04A_SET_CONTEXT_DMA_NOTIFIES | 0x40000);
    nvPushData (3,NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    nvPushData (4,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV04A_SET_CONTEXT_PATTERN | 0x80000);
    nvPushData (5,NV_DD_CONTEXT_PATTERN);
    nvPushData (6,NV_DD_CONTEXT_ROP);
    nvPushData (7,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV04A_SET_CONTEXT_SURFACE | 0x40000);
    nvPushData (8,NV_DD_SURFACES_2D);
    nvPushData (9,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV04A_SET_OPERATION | 0xC0000);
    nvPushData (10,NV04A_SET_OPERATION_ROP_AND);

    if (dwPixelDepth == 8) {
        // Y8 color format assumed by hardware when destination surface in 8bpp mode
        // Must be set to a legal value but hardware ignores it otherwise
        nvPushData (11,NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8);
    }
    else if (dwPixelDepth == 16) {
        nvPushData (11,NV04A_SET_COLOR_FORMAT_LE_X16R5G6B5);
    }
    else {
        nvAssert (dwPixelDepth == 32);
        nvPushData (11,NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8);
    }

    nvPushData (12,NV04A_SET_MONOCHROME_FORMAT_CGA6_M1);

    nvPusherAdjust (13);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_IMAGE) | 0x40000);
    nvPushData (1,NV_DD_VIDEO_MEMORY_TO_SYSTEM_MEMORY_FORMAT);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_IMAGE) + NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000);
    nvPushData (3,NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    nvPushData (4,NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY);
    nvPushData (5,NV_DD_DMA_CONTEXT_DMA_TO_SYSTEM_MEMORY);

    nvPusherAdjust (6);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
    nvPushData (1,NV_DD_CONTEXT_COLOR_KEY);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_STRETCH) + NV057_SET_COLOR_FORMAT | 0x80000);

    if (dwPixelDepth == 8) {
        // A8Y8 color format assumed by hardware when destination surface in 8bpp mode
        // Must be set to a legal value but hardware ignores it otherwise
        nvPushData (3,NV057_SET_COLOR_FORMAT_LE_A8R8G8B8);
    }
    else if (dwPixelDepth == 16) {
        nvPushData (3,NV057_SET_COLOR_FORMAT_LE_A16R5G6B5);
    }
    else {
        nvAssert (dwPixelDepth == 32);
        nvPushData (3,NV057_SET_COLOR_FORMAT_LE_A8R8G8B8);
    }

    nvPushData (4,0);

    nvPusherAdjust (5);
    pDriverData->bltData.dwLastColourKey = 0xFFFFFFF0;    // invalidate

    // Pattern is currently only used for overlay video UV masking so
    // always set it's color format to 8 bit indexed.
    nvPushData (0,dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    nvPushData (1,NV_DD_CONTEXT_PATTERN);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_SPARE) + NV044_SET_COLOR_FORMAT | 0x80000);
    // A8Y8 color format assumed by hardware when destination surface in 8bpp mode
    // Must be set to a legal value but hardware ignores it otherwise
    nvPushData (3,NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
    nvPushData (4,NV044_SET_MONOCHROME_FORMAT_CGA6_M1);
    nvPushData (5,dDrawSubchannelOffset(NV_DD_SPARE) + NV044_SET_PATTERN_SELECT | 0xC0000);
    nvPushData (6,NV044_SET_PATTERN_SELECT_MONOCHROME);

    if (dwPixelDepth == 8) {
        nvPushData (7,NV_ALPHA_1_008);
        nvPushData (8,NV_ALPHA_1_008);
    }
    else if (dwPixelDepth == 16) {
        nvPushData (7,NV_ALPHA_1_016);
        nvPushData (8,NV_ALPHA_1_016);
    }
    else {
        nvPushData (7,NV_ALPHA_1_032);
        nvPushData (8,NV_ALPHA_1_032);
    }

    nvPusherAdjust (9);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) | 0x40000);
    nvPushData (1,NV_DD_IMAGE_BLACK_RECTANGLE);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP) + NV019_SET_POINT | 0x80000);
    nvPushData (3,0);
    nvPushData (4,asmMergeCoords(NV_MAX_X_CLIP, NV_MAX_Y_CLIP));

    nvPusherAdjust (5);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) | 0x40000);
    nvPushData (1,NV_DD_CONTEXT_ROP);
    nvPushData (2,dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000);
    nvPushData (3,NV_DD_SURFACES_2D);
    nvPushData (4,dDrawSubchannelOffset(NV_DD_SURFACES) + NV042_SET_PITCH | 0xC0000);
    nvPushData (5,(pHalInfo->vmiData.lDisplayPitch << 16) | pHalInfo->vmiData.lDisplayPitch);
    nvPushData (6,0);
    nvPushData (7,0);

    nvPushData (8,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) | 0x40000);
    nvPushData (9,NV_DD_ROP_GDI_RECT_AND_TEXT);
    nvPushData (10,dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000);
    nvPushData (11,NV_DD_IMAGE_BLIT);
    nvPushData (12,dDrawSubchannelOffset(NV_DD_IMAGE) | 0x40000);
    nvPushData (13,NV_DD_IMAGE_FROM_CPU);
    nvPushData (14,dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000);
    nvPushData (15,NV_DD_GENERIC_STRETCHED_IMAGE);

    nvPusherAdjust (16);

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) + NV043_SET_ROP5 | 0x40000);
    nvPushData (1,0x00000000);
    pDriverData->bltData.dwLastRop = 0;

    // Force rectangle portion of patch to be validated (assures no RM interrupts on Lock calls)
    nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + NV04A_COLOR1_A | 0xC0000);
    nvPushData (3,0);
    nvPushData (4,0);
    nvPushData (5,0);

    // Force Blit to be instantiated
    nvPushData (6,dDrawSubchannelOffset(NV_DD_BLIT) + NV05F_CONTROL_POINT_IN | 0x40000);
    nvPushData (7,0);

    nvPusherAdjust (8);

    // flush cached values
    pDriverData->bltData.dwLastRop = 0xFFFFFFFF;
    pDriverData->bltData.dwLastColourKey = 0xFFFFFFF0;
    pDriverData->bltData.dwLastColourFormat = 0;
    pDriverData->bltData.dwLastCombinedPitch = 0;
    pDriverData->bltData.dwLastSrcOffset = 0xFFFFFFFF;
    pDriverData->bltData.dwLastDstOffset = 0xFFFFFFFF;

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL nvEnableD3D (void)
{
    dbgTracePush ("nvEnableD3D");

    // the push buffer should have already been created at this point
    if (!getDC()->nvPusher.isValid()) {
        dbgTracePop();
        return (FALSE);
    }

    // Create the swizzled surface context.     NV 0x052
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_009E_CTXSURFSWZ) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_CONTEXT_SURFACE_SWIZZLED, NV15_CONTEXT_SURFACE_SWIZZLED);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0052_CTXSURFSWZ){
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_CONTEXT_SURFACE_SWIZZLED, NV04_CONTEXT_SURFACE_SWIZZLED);
    }
    else {
        DPF("Creation of D3D_CONTEXT_SURFACE_SWIZZLED object failed");
        dbgD3DError();
    }

    // create the best 3D surface context we can
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN))) {
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0093_CTXSURF3D) {
            NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_CONTEXT_SURFACES_ARGB_ZS, NV10_CONTEXT_SURFACES_3D);
        }
        else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0053_CTXSURF3D) {
            NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_CONTEXT_SURFACES_ARGB_ZS, NV04_CONTEXT_SURFACES_ARGB_ZS);
        }
        else {
            DPF("Creation of context surface 3d object failed");
            dbgD3DError();
        }
    }

    // create the best 3D rendering class we can
#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0097_KELVIN) {
#ifdef SPOOF_KELVIN
        // allocate a kelvin object for the sake of the trace file. this will fail. ignore return value.
        NvRmAllocObject (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_KELVIN_PRIMITIVE, NV20_KELVIN_PRIMITIVE);
#else
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_KELVIN_PRIMITIVE, NV20_KELVIN_PRIMITIVE);
#endif
        DPF_LEVEL(NVDBG_LEVEL_INFO, "HW supports NV20 Kelvin class");
    } else
#endif
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_1196_CELSIUS) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_CELSIUS_PRIMITIVE, NV11_CELSIUS_PRIMITIVE);
        DPF_LEVEL(NVDBG_LEVEL_INFO, "HW supports NV11 Celsius class");
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0096_CELSIUS) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_CELSIUS_PRIMITIVE, NV15_CELSIUS_PRIMITIVE);
        DPF_LEVEL(NVDBG_LEVEL_INFO, "HW supports NV15 Celsius class");
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0056_CELSIUS) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_CELSIUS_PRIMITIVE, NV10_CELSIUS_PRIMITIVE);
        DPF_LEVEL(NVDBG_LEVEL_INFO, "HW supports NV10 Celsius class");
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_0094_DX5TRI |
                                                       NVCLASS_0095_DX6TRI)) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_DX5_TEXTURED_TRIANGLE, NV10_DX5_TEXTURED_TRIANGLE);
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_DX6_MULTI_TEXTURE_TRIANGLE, NV10_DX6_MULTI_TEXTURE_TRIANGLE);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_0054_DX5TRI |
                                                       NVCLASS_0055_DX6TRI)) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_DX5_TEXTURED_TRIANGLE, NV04_DX5_TEXTURED_TRIANGLE);
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_DX6_MULTI_TEXTURE_TRIANGLE, NV04_DX6_MULTI_TEXTURE_TRIANGLE);
    }
    else {
        DPF ("blech. HW doesn't seem to support any 3D rendering classes");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    // celsius's default state is the OGL view vector
    getDC()->bD3DViewVector = FALSE;

    // create RenderSolid rectangles or GDI rectangles for clearing
    // the z-buffer and rendering surfaces.
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_005E_SOLIDRECT) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_RENDER_SOLID_RECTANGLE, NV4_RENDER_SOLID_RECTANGLE);
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_RENDER_SOLID_RECTANGLE_2, NV4_RENDER_SOLID_RECTANGLE);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_004A_GDIRECT) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_GDI_RECTANGLE_TEXT, NV4_GDI_RECTANGLE_TEXT);
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_GDI_RECTANGLE_TEXT_2, NV4_GDI_RECTANGLE_TEXT);
    }
    else {
        DPF ("hardware supports neither D3D_GDI_RECTANGLE_TEXT nor NV4_RENDER_SOLID_RECTANGLE");
        dbgD3DError();
    }

    // create Scaled Image From Memory object for texture swizzling
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0089_SCALEDIMG) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_SCALED_IMAGE_FROM_MEMORY, NV10_SCALED_IMAGE_FROM_MEMORY);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0063_SCALEDIMG) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_SCALED_IMAGE_FROM_MEMORY, NV05_SCALED_IMAGE_FROM_MEMORY);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0077_SCALEDIMG) {
        NVDD_ALLOCOBJECT (NV_DD_DEV_DMA, D3D_SCALED_IMAGE_FROM_MEMORY, NV04_SCALED_IMAGE_FROM_MEMORY);
    }
    else {
        DPF ("hardware supports no form of SCALED_IMAGE_FROM_MEMORY");
        dbgD3DError();
    }

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL nvEnableHeaps (void)
{
    BOOL bRes;

    dbgTracePush ("nvEnableHeaps");

    if (pDriverData->GARTLinearBase) {
        bRes = nvAGPCreateHeap();
        if (bRes) {
            dbgTracePop();
            return bRes;
        }
        // AGP doesn't seem to be working, disable it
#if defined(WINNT) && !defined(NV_AGP)
        NvWin2KUnmapAgpHeap(pDriverData);
#endif
        pDriverData->GARTLinearBase = 0;
    }

    bRes = nvPCICreateHeap();

    dbgTracePop();
    return bRes;
}

//---------------------------------------------------------------------------

BOOL nvDisableHeaps (void)
{
    BOOL bRes;

    dbgTracePush ("nvDisableHeaps");

#if IS_WIN9X
    // Purge the heap of all DirectX allocations.
    // If things are cleaned up as they should be, there shouldn't be anything
    // to purge and this will therefore generate an "error" message from the RM.
//    NVHEAP_PURGE();
#endif

    if (pDriverData->GARTLinearBase) {
        bRes = nvAGPDestroyHeap();
    } else {
        bRes = nvPCIDestroyHeap();
    }

    dbgTracePop();
    return bRes;
}

//---------------------------------------------------------------------------

BOOL nvInitD3DObjects (void)
{
    dbgTracePush ("nvInitD3DObjects");

    // DO NOT REMOVE - test code to override tiling
    // NvRmConfigSetEx (pDriverData->dwRootHandle,pDriverData->dwDeviceIDNum,NV_CFGEX_DISABLE_TETRIS_TILING,0,0);

    // Hook up the ARGB_ZS surface context.
    if (!(pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN))) {
        nvglSetObject(NV_DD_SPARE, D3D_CONTEXT_SURFACES_ARGB_ZS);
        nvglSetRenderTargetContexts(NV_DD_SPARE,
                                    NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,
                                    NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    }

    if (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_005E_SOLIDRECT)) {
        // Hook up GDI Rectangle.
        nvglSetObject                      (NV_DD_SPARE, D3D_GDI_RECTANGLE_TEXT);
        nvglDrawRopRectAndTextSetSurface   (NV_DD_SPARE, NV_DD_SURFACES_2D);
        nvglDrawRopRectAndTextSetOperation (NV_DD_SPARE, NV04A_SET_OPERATION_SRCCOPY);

        // Hook up GDI Rectangle 2.
        nvglSetObject                        (NV_DD_SPARE, D3D_GDI_RECTANGLE_TEXT_2);
        nvglDrawRopRectAndTextSetSurface     (NV_DD_SPARE, NV_DD_SURFACES_2D);
        nvglDrawRopRectAndTextSetOperation   (NV_DD_SPARE, NV04A_SET_OPERATION_SRCCOPY);
        nvglDrawRopRectAndTextSetColorFormat (NV_DD_SPARE, NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8);
    }
    else {
        // Hook up the Render Solid Rectangle.
        nvglSetObject                     (NV_DD_SPARE, D3D_RENDER_SOLID_RECTANGLE);
        nvglSetNv4SolidRectangleSurface   (NV_DD_SPARE, NV_DD_SURFACES_2D);
        nvglSetNv4SolidRectangleOperation (NV_DD_SPARE, NV05E_SET_OPERATION_SRCCOPY);

        // Hook up Render Solid Rectangle 2.
        nvglSetObject      (NV_DD_SPARE, D3D_RENDER_SOLID_RECTANGLE_2);
        nvglSetNv4SolidRectangleSurface (NV_DD_SPARE, NV_DD_SURFACES_2D);
        nvglSetStartMethod (0,NV_DD_SPARE, NV05E_SET_OPERATION,1);
        nvglSetData        (1,             NV05E_SET_OPERATION_SRCCOPY);
        nvglSetStartMethod (2,NV_DD_SPARE, NV05E_SET_COLOR_FORMAT,1);
        nvglSetData        (3,             NV05E_SET_COLOR_FORMAT_LE_X8R8G8B8);
        nvglAdjust         (4);
    }

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        // some bare bones initialization: put the thing in its channel and set up for notifers
        nvglSetObject (NV_DD_KELVIN, D3D_KELVIN_PRIMITIVE);
        nvglSetNv20KelvinNotifierContextDMA (NV_DD_KELVIN, NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        // some bare bones initialization: put the thing in its channel and set up for notifers
        nvglSetObject (NV_DD_CELSIUS, D3D_CELSIUS_PRIMITIVE);
        nvglSetNv10CelsiusNotifierContextDMA (NV_DD_CELSIUS, NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    }
    else
#endif
    {
        // no celsius or kelvin class. hook up DX5 / DX6 stuff

        // Hook up the Dx5 Textured Triangle.
        nvglSetObject(NV_DD_SPARE, D3D_DX5_TEXTURED_TRIANGLE);
        // Contexts. System/AGP goes to context A, Video Memory goes to context B.
        nvglSetDx5TriangleContexts(NV_DD_SPARE, D3D_CONTEXT_DMA_HOST_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
        // Rendering surface
        nvglSetDx5TriangleSurface(NV_DD_SPARE, D3D_CONTEXT_SURFACES_ARGB_ZS);
        // Notify context.
        nvglSetStartMethod(0, NV_DD_SPARE, NV054_SET_CONTEXT_DMA_NOTIFIES, 1);
        nvglSetData       (1, NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
        nvglAdjust        (2);

        // Hook up the Dx6 Multi Textured Triangle.
        nvglSetObject(NV_DD_SPARE, D3D_DX6_MULTI_TEXTURE_TRIANGLE);
        // System/AGP goes to context A, Video Memory goes to context B.
        nvglSetDx6TriangleContexts(NV_DD_SPARE, D3D_CONTEXT_DMA_HOST_MEMORY, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
        // Rendering surface
        nvglSetDx6TriangleSurface(NV_DD_SPARE, D3D_CONTEXT_SURFACES_ARGB_ZS);
        // Plug in the notify context.
        nvglSetStartMethod(0, NV_DD_SPARE, NV055_SET_CONTEXT_DMA_NOTIFIES, 1);
        nvglSetData       (1, NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
        nvglAdjust        (2);
    }

    pDriverData->dwMostRecentHWUser = MODULE_ID_NONE;

    // Hook up the swizzled surface context.
    nvglSetObject(NV_DD_SPARE, D3D_CONTEXT_SURFACE_SWIZZLED);
    nvglSetSwizzledSurfaceContextImage(NV_DD_SPARE, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);

    // Hook up the scaled image from memory object.
    // - Set the source image context.
    // - Set the destination surface context.
    // - Set operation
    nvglSetObject                     (NV_DD_SPARE, D3D_SCALED_IMAGE_FROM_MEMORY);
    nvglSetScaledImageContextImage    (NV_DD_SPARE, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    nvglSetScaledImageContextSurface  (NV_DD_SPARE, D3D_CONTEXT_SURFACE_SWIZZLED);
    nvglSetScaledImageOperation       (NV_DD_SPARE, NV077_SET_OPERATION_SRCCOPY);

    getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL nvInitRefCounts (void)
{
    DWORD dwIndex;

    dbgTracePush ("nvInitRefCounts");

    // Clear motion comp reference counts
    for (dwIndex = 0; dwIndex < 8; dwIndex++) {
        pDriverData->nvMCSurfaceFlags[dwIndex].dwMCMotionCompReferenceCount = 0;
        pDriverData->nvMCSurfaceFlags[dwIndex].dwMCFormatCnvReferenceCount = 0;
        pDriverData->nvMCSurfaceFlags[dwIndex].dwMCCompositeReferenceCount = 0;
        pDriverData->nvMCSurfaceFlags[dwIndex].dwMCTemporalFilterReferenceCount = 0;
    }
    for (dwIndex = 0; dwIndex < 4; dwIndex++) {
        pDriverData->dwMCNVIDSurfaceReferenceCount[dwIndex] = 0;
    }

    // initialize ref count and frame tracker
    DWORD dwTimeOut = (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) ?
                          50 :
                          (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) ?
                              50 :
                              1;
    getDC()->pRefCount->create (dwTimeOut);

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL nvEnableOverlay (void)
{
    NvU8 bRestoreOverlaySurfaces;
    NvU32 uiSubchannel = NULL;
    NvU32 uiClassID    = NULL;

    dbgTracePush ("nvEnableOverlay");

    bRestoreOverlaySurfaces = (pDriverData->vpp.dwOverlaySurfaces && pDriverData->vpp.dwOverlayFSOvlHeadSaved != 0xFFFFFFFF);

    // Set up default values for device version < NV10

    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_20)
    {
        uiSubchannel = NV_DD_KELVIN;
        uiClassID    = D3D_KELVIN_PRIMITIVE;
    }
    else if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10)
    {
        uiSubchannel = NV_DD_CELSIUS;
        uiClassID    = D3D_CELSIUS_PRIMITIVE;
    }

    if (!VppEnable(
       &pDriverData->vpp,
#ifdef WINNT
       ppdev,
#else
       NULL,
#endif
       &pDriverData->nvPusher,
       NV_DD_DEV_DMA,
       uiClassID,   // Class ID
       uiSubchannel,// Sub channel
       &pDriverData->dwMostRecentHWUser,
       NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,
       NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY,
       NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY,
       NV_DD_VIDEO_LUT_CURSOR_DAC,  // Up to pDriverData->dwHeads of these... @mjl@ pass dwHeads
       NV_DD_DMA_COLOUR_CONTROL_EVENT_NOTIFIER,
       D3D_CONTEXT_SURFACE_SWIZZLED,
       NV_DD_CONTEXT_BETA4,
       NV_DD_SURFACES_2D,
       NV_DD_SURFACES,           // Subchannel holding a 2D surface object
       NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT,
       NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT,
       NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT,

       0,
       NV_DD_CONTEXT_PATTERN,
       NV_DD_CONTEXT_ROP,
       NV_DD_CONTEXT_COLOR_KEY,
       NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_OVERLAY_SHADOW,

       pDriverData->pDmaFlipPrimaryNotifierFlat,
       pDriverData->pDmaPusherSyncNotifierFlat,
       pDriverData->pPioColourControlEventNotifierFlat,

       D3D_CONTEXT_SURFACES_ARGB_ZS,
       D3D_DX6_MULTI_TEXTURE_TRIANGLE,
       NV_DD_IMAGE_BLACK_RECTANGLE,

       NV_DD_ROP_RECT_AND_TEXT, // Subchannel holding RECT_AND_TEXT object
       NV_DD_ROP,               // Subchannel holding ROP object
       NV_DD_SPARE              // spare subchannel
       ))
    {
        dbgTracePop();
        return FALSE;
    }

    if (bRestoreOverlaySurfaces) // we interrupted an app which had open overlays, recreate those objects
    {
#ifdef IS_WIN9X
        DWORD dwSaveCurrentHead = pDXShare->dwHeadNumber;
        pDXShare->dwHeadNumber = pDriverData->vpp.dwOverlayFSOvlHeadSaved;
#endif
        VppCreateOverlay(&pDriverData->vpp);
        VppCreateFSMirror(&pDriverData->vpp, pDriverData->vpp.dwOverlayFSSrcWidth, pDriverData->vpp.dwOverlayFSSrcHeight);

#ifdef IS_WIN9X
        pDXShare->dwHeadNumber = dwSaveCurrentHead;
#endif
    }

//    VppReadRegistry(pVpp);

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL nvEnableSemaphores (void)
{
    dbgTracePush ("nvEnableSemaphores");

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES
    // set up the second DAC and other semaphore-related stuff
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        if (!getDC()->nvSemaphoreManager.create()) {
            DPF ("failed to create semaphore manager");
            dbgTracePop();
            return (FALSE);
        }

        if (!getDC()->nvFlipper.create()) {
            DPF ("failed to create flipper");
            dbgTracePop();
            return (FALSE);
        }

        // set up kelvin
        nvglSetObject (NV_DD_SPARE, D3D_KELVIN_PRIMITIVE);
        nvPushData (0, (0x00040000 | (NV_DD_SPARE << 13) | NV097_SET_CONTEXT_DMA_SEMAPHORE));
        nvPushData (1, CSemaphoreManager::SEMAPHORE_CONTEXT_DMA_IN_MEMORY);    // must be read-write
        nvPusherAdjust (2);

        // set up the LUT-cursor-DAC objects
        for (DWORD dwHead=0; dwHead < pDriverData->dwHeads; dwHead++) {
            getDC()->nvPusherDAC.push ( 0, (0x00040000 | (NV_DD_SUBCH_DAC << 13)));
            getDC()->nvPusherDAC.push ( 1, NV_DD_VIDEO_LUT_CURSOR_DAC_2 + dwHead);
            getDC()->nvPusherDAC.push ( 2, (0x00200000 | (NV_DD_SUBCH_DAC << 13) | NV07C_SET_CONTEXT_DMA_NOTIFIES));
            getDC()->nvPusherDAC.push ( 3, NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
            getDC()->nvPusherDAC.push ( 4, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);        // image a
            getDC()->nvPusherDAC.push ( 5, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);        // image b
            getDC()->nvPusherDAC.push ( 6, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);        // LUT a
            getDC()->nvPusherDAC.push ( 7, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);        // LUT b
            getDC()->nvPusherDAC.push ( 8, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);        // cursor a
            getDC()->nvPusherDAC.push ( 9, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);        // cursor b
            getDC()->nvPusherDAC.push (10, CSemaphoreManager::SEMAPHORE_CONTEXT_DMA_FROM_MEMORY);  // semaphore (must be read only)
            getDC()->nvPusherDAC.push (11, (0x00040000 | (NV_DD_SUBCH_DAC << 13) | NV07C_SET_DAC(0)));
            getDC()->nvPusherDAC.push (12, asmMergeCoords (GET_MODE_WIDTH(), GET_MODE_HEIGHT()));
            getDC()->nvPusherDAC.adjust (13);
        }

        // set up the 3D channel
        nvPushData (0, (0x00040000 | (NV_DD_SPARE << 13) | NV206E_SET_CONTEXT_DMA_SEMAPHORE));
        nvPushData (1, CSemaphoreManager::SEMAPHORE_CONTEXT_DMA_FROM_MEMORY);  // must be read only
        nvPusherAdjust (2);

        // set up the DAC channel
        getDC()->nvPusherDAC.push (0, (0x00040000 | (NV_DD_SUBCH_DAC << 13) | NV206E_SET_CONTEXT_DMA_SEMAPHORE));
        getDC()->nvPusherDAC.push (1, CSemaphoreManager::SEMAPHORE_CONTEXT_DMA_FROM_MEMORY);  // must be read only
        getDC()->nvPusherDAC.adjust (2);
    }
#endif
#endif

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

// nvEnable32()
// create push puffer, channels, and objects

BOOL nvEnable32
(
    LPDDRAWI_DIRECTDRAW_GBL  // not used
)
{
    dbgTracePush ("nvEnable32");

    nvAssert (!global.b16BitCode);

    pDriverData->bltData.dwSystemBltFallback = FALSE;

    if (getDC()->dwEnableStatus == CDriverContext::DC_ENABLESTATUS_PENDINGDISABLE) {
        // we tried to disable before but couldn't. disable now.
        nvDisable32();
    }

    if (getDC()->dwEnableStatus == CDriverContext::DC_ENABLESTATUS_ENABLED) {
        // we're already enabled
        dbgTracePop();
        return (TRUE);
    }

    if ((pDriverData->dwFullScreenDOSStatus & (FSDOSSTATUS_RECOVERYNEEDED_D3D | FSDOSSTATUS_RECOVERYNEEDED_DDRAW)) &&
        (!(pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE))) {
        // currently in full screen DOS. don't allow channel to be enabled
        dbgTracePop();
        return (FALSE);
    }

    getDisplayDuration();

#ifdef CAPTURE
    // move to the next capture file
    captureFileInc();
#endif

#ifdef NV_AGP
    if (ppdev->bAgp) {
        NvRmConfigGet(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFG_AGP_LINEAR_BASE, &pDriverData->GARTLinearBase);
        NvRmConfigGet(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFG_AGP_PHYS_BASE,   &pDriverData->GARTPhysicalBase);
    } else {
        pDriverData->GARTLinearBase   = 0;
        pDriverData->GARTPhysicalBase = 0;
    }
    DPF_LEVEL(NVDBG_LEVEL_NV_AGP, "nvEnable32: GARTLinearBase = %08x, GARTPhysicalBase = %08x", pDriverData->GARTLinearBase, pDriverData->GARTPhysicalBase);
#else
#ifdef WINNT
    if (pDriverData->GARTPhysicalBase && !pDriverData->GARTLinearBase)
    {
        NvWin2KMapAgpHeap(pDriverData);
    }
#endif // WINNT
#endif

    if (pDriverData->bltVidMemInfo.dwLocale == BLTWS_VIDEO) {
        NVHEAP_FREE (pDriverData->bltVidMemInfo.fpVidMem);
        pDriverData->bltVidMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    }

    pmDeleteAllProcesses(pDriverData);

    // we used to purge the heap here - this breaks NV11 running multiple apps since the heap
    // would then be purged whenever the second device is enumerated.
    // a heap purge should not be necessary - any special cleanup should only clean out what is
    // really needed.
    // NVHEAP_PURGE ();

    if (!nvEnableHeaps()) {
        DPF ("could not enable heaps");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    // allocate a push buffer
    if (!getDC()->nvPusher.allocate (NV_DD_DEV_DMA)) {
        DPF ("failed to allocate push buffer");
        dbgTracePop();
        return (FALSE);
    }

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        if (!getDC()->nvPusherDAC.allocate (NV_DD_DEV_DAC)) {
            DPF ("failed to allocate DAC push buffer");
        }
    }
#endif
#endif

    // fetch and cache off the pixel depth so we know what to disable later
    DWORD dwPixelDepth = GET_MODE_BPP();
    getDC()->dwEnablePixelDepth = dwPixelDepth;

    nvCreateVidHeapVarsAndPseudoNotifier();

    if (!nvCreateNotifiers()) {
        DPF ("failed to create notifiers");
        dbgD3DError();
        dbgTracePop();
        return FALSE;
    }

    if (!nvEnableNotifiers()) {
        DPF ("failed to enable notifiers");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    if (!nvEnableContextDMAs()) {
        DPF ("could not enable context DMAs");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    if (!nvEnableDDraw()) {
        DPF ("could not enable ddraw");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    if (!nvInitDDrawObjects()) {
        DPF ("could not initialize ddraw objects");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    if (!nvEnableD3D()) {
        DPF ("could enable D3D");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    if (!nvInitD3DObjects()) {
        DPF ("could not initialize d3d objects");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    if (!nvInitRefCounts()) {
        DPF ("could not initialize ref counts");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    if (!nvEnableOverlay()) {
        DPF ("could not enable overlay");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    dbgFlushType (NVDBG_FLUSH_SETUP);

    DWORD dwHeap = getDC()->nvPusher.getHeap();

    if (getDC()->nvD3DPerfData.dwRecommendedStageBufferSize) {
        if (!getDC()->defaultVB.create (dwHeap, getDC()->nvD3DPerfData.dwRecommendedStageBufferSize)) {
            DPF ("failed to create default vertex buffer");
            dbgD3DError();
        }
    }

    if (!nvEnableSemaphores()) {
        DPF ("could not enable semaphores");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    if (!nvTextureDefaultAlloc()) {
        DPF ("could not create default textures");
        dbgD3DError();
        dbgTracePop();
        return (FALSE);
    }

    // Make final subchannel assignments

    // 0 - NV_DD_ROP
    nvglSetObject (NV_DD_ROP, NV_DD_CONTEXT_ROP);
    // 1 - NV_DD_SURFACES
    nvglSetObject (NV_DD_SURFACES, NV_DD_SURFACES_2D);
    // 2 - NV_DD_ROP_RECT_AND_TEXT
    nvglSetObject (NV_DD_ROP_RECT_AND_TEXT, NV_DD_ROP_GDI_RECT_AND_TEXT);
    // 3 - NV_DD_BLIT
    nvglSetObject (NV_DD_BLIT, NV_DD_IMAGE_BLIT);
    // 4 - NV_DD_IMAGE
    nvglSetObject (NV_DD_IMAGE, NV_DD_IMAGE_FROM_CPU);
    // 5 - NV_DD_STRETCH
    nvglSetObject (NV_DD_STRETCH, NV_DD_GENERIC_STRETCHED_IMAGE); // NOTE! This subchannel is NOT fixed.  Do a set before use!
    // 6 - NV_DD_CELSIUS
    // 6 - NV_DD_KELVIN
#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        nvglSetObject (NV_DD_KELVIN, D3D_KELVIN_PRIMITIVE);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        nvglSetObject (NV_DD_CELSIUS, D3D_CELSIUS_PRIMITIVE);
    }
#endif
    // 7 - NV_DD_SPARE
    pDriverData->dDrawSpareSubchannelObject = 0;

    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        // the RM does not set the celsius defaults reliably. failure to init the
        // eye direction vector here will cause D3D HAL fog to fail (e.g. dolphin)
        nvglSetNv10CelsiusEyeDirection(NV_DD_CELSIUS, 0.f, 0.f, 1.f);
    }

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        // set default objects on the DAC channel
        getDC()->nvPusherDAC.push (0, (0x00040000 | (NV_DD_SUBCH_DAC << 13)));
        getDC()->nvPusherDAC.push (1, NV_DD_VIDEO_LUT_CURSOR_DAC_2);
        getDC()->nvPusherDAC.adjust (2);
    }
#endif
#endif

    // figure out which channel we're on
    getDC()->nvPusher.flush (FALSE, CPushBuffer::FLUSH_WITH_DELAY);
    getDC()->nvPusher.waitForOtherChannels();

    NvNotification *npDmaSyncNotifier = pDriverData->pDmaSyncNotifierFlat;

    DWORD dwIndex = 0xffffffff;
    pDriverData->dwDDDmaPusherChannelIndex = 1;

    while (dwIndex != pDriverData->dwDDDmaPusherChannelIndex) {

        pDriverData->dwDDDmaPusherChannelIndex = dwIndex;
        npDmaSyncNotifier->status = NV_IN_PROGRESS;

        nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000);
        nvPushData (1,0);
        nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000);
        nvPushData (3,0);
        nvPusherAdjust (4);
        nvPusherStart  (TRUE);

        while ((volatile)npDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

        NV_DD_GET_DMA_CHANNEL_INDEX (dwIndex);
    }

    pDriverData->dwDDDmaPusherChannelMask = 1 << pDriverData->dwDDDmaPusherChannelIndex;

    // Perform any re-enabling of D3D.
    nvD3DRecover();

    // Let D3D code know that we have touched NV
    pDriverData->TwoDRenderingOccurred = 1;

    pDriverData->DDrawVideoSurfaceCount = 0;

    pDriverData->ddClipUpdate = TRUE;

    // All active floating DMA contexts have been reset
    pDriverData->fNvActiveFloatingContexts = 0;

    // Make sure overlay owner checking is enabled
    pDriverData->vpp.overlayRelaxOwnerCheck = 0;

    // need to do this again here cuz WINNT doesn't call buildddhalinfo16
    pDriverData->physicalColorMask = (dwPixelDepth == 8) ?
                                         0x000000ff :
                                         (dwPixelDepth == 16) ?
                                             0x0000ffff :
                                             0x00ffffff;

    // Initialize primary surface pointer
    pDriverData->CurrentVisibleSurfaceAddress = GET_PRIMARY_ADDR();

    // No DMA push blit synchronizations currently in progress
    pDriverData->syncDmaRecord.bSyncFlag = FALSE;

    // flush now because if we do a mode change without doing anything, then when
    // the push buffer gets re-enabled, we may attempt to flush crap
    getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_HEAVY_POLLING);
#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        getDC()->nvPusherDAC.flush (TRUE, CPushBuffer::FLUSH_HEAVY_POLLING);
    }
#endif
#endif

    // Initialize the flip history
    //    Cannot be all zeros as that is a valid surface address on NT
    {
        memset(&pDriverData->flipHistory,0xff,sizeof(pDriverData->flipHistory));
        pDriverData->flipHistory.dwIndex = 0;
    }


    getDC()->dwEnableStatus = CDriverContext::DC_ENABLESTATUS_ENABLED;

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

void nvDisableD3D
(
    void
)
{
    dbgTracePush ("nvDisableD3D");

    // Destroy the objects.

    //which of these is preferred? NV20 gets GDIRECT
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_005E_SOLIDRECT) {
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_RENDER_SOLID_RECTANGLE);
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_RENDER_SOLID_RECTANGLE_2);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_004A_GDIRECT) {
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_GDI_RECTANGLE_TEXT);
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_GDI_RECTANGLE_TEXT_2);
    }
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_SCALED_IMAGE_FROM_MEMORY);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_CONTEXT_SURFACE_SWIZZLED);

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_KELVIN_PRIMITIVE);
    }
    else
#endif
#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_CELSIUS_PRIMITIVE);
    }
    else
#endif
    {
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_DX6_MULTI_TEXTURE_TRIANGLE);
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_DX5_TEXTURED_TRIANGLE);
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, D3D_CONTEXT_SURFACES_ARGB_ZS);
    }

    dbgTracePop();
}

//---------------------------------------------------------------------------

// nvDisable32()
// destroy objects, close channels, and free the push buffer

BOOL __stdcall nvDisable32()
{
    DWORD dwPixelDepth, dwHead;

    dbgTracePush ("nvDisable32");

    if (getDC()->dwEnableStatus == CDriverContext::DC_ENABLESTATUS_DISABLED) {
        // we're already disabled
        dbgTracePop();
        return (TRUE);
    }

    // flush the push buffers before we start destroying stuff
    if (getDC()->nvPusher.isValid()) {
        // make sure we sync all other channels before shutdown
        getDC()->nvPusher.waitForOtherChannels();
        getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_HEAVY_POLLING);
    }

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        if (getDC()->nvPusherDAC.isValid()) {
            // make sure we sync all other channels before shutdown
            getDC()->nvPusherDAC.waitForOtherChannels();
            getDC()->nvPusherDAC.flush (TRUE, CPushBuffer::FLUSH_HEAVY_POLLING);
        }
    }
#endif
#endif

    if (global.b16BitCode) {
        // we need to disable things but cannot. we'll do it at the next opportunity
        getDC()->dwEnableStatus = CDriverContext::DC_ENABLESTATUS_PENDINGDISABLE;
        // in the meantime, we're not allowed to use the push buffer
        getDC()->nvPusher.invalidate();
        dbgTracePop();
        return (FALSE);
    }

    if (pDriverData->vpp.dwOverlaySurfaces) {
        // application isn't really done with overlays, something has happened like a mode switch or AGP heap moved
        pDriverData->vpp.dwOverlayFSOvlHeadSaved = pDriverData->vpp.dwOverlayFSOvlHead;
        VppDestroyFSMirror(&pDriverData->vpp);
        VppDestroyOverlay(&pDriverData->vpp);
    } else {
        pDriverData->vpp.dwOverlayFSOvlHeadSaved = 0xFFFFFFFF;
    }

    VppDisable(&pDriverData->vpp);  // Free objects...

    // get the cached value since bi.biBitCount may be different from what it was at enable time
    dwPixelDepth = getDC()->dwEnablePixelDepth;

    // destroy the default VB
    getDC()->defaultVB.destroy();

    getDC()->pRefCount->destroy();
    getDC()->pBlitTracker->destroy();

    nvTextureDefaultFree();

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        getDC()->nvFlipper.destroy();
        getDC()->nvSemaphoreManager.destroy();
    }
#endif
#endif

    // Destroy the D3D objects
    nvDisableD3D();

    for(dwHead = 0; dwHead < pDriverData->dwHeads; dwHead ++) {
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_VIDEO_LUT_CURSOR_DAC + dwHead);
    }

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        for(dwHead = 0; dwHead < pDriverData->dwHeads; dwHead ++) {
            NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DAC, NV_DD_VIDEO_LUT_CURSOR_DAC_2 + dwHead);
        }
    }
#endif
#endif

    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_VIDEO_MEMORY_TO_SYSTEM_MEMORY_FORMAT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_CONTEXT_ROP);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_CONTEXT_PATTERN);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_IMAGE_BLACK_RECTANGLE);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_CONTEXT_COLOR_KEY);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_ROP_GDI_RECT_AND_TEXT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_IMAGE_FROM_CPU);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_INDEXED_IMAGE_FROM_CPU);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_GENERIC_STRETCHED_IMAGE);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_SCALED_IMAGE_IID);

    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_IMAGE_BLIT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_SRCCOPY_IMAGE_BLIT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_SRCCOPY_PREMULT_A8R8G8B8_IMAGE_BLIT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_SURFACES_2D);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_GENERIC_MTMF_1);
    NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_GENERIC_MTMF_2);

    // free the push buffer
    getDC()->nvPusher.free();

#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        getDC()->nvPusherDAC.free();
    }
#endif
#endif

    nvDestroyNotifiers();

    // destroy the texture heap (AGP or PCI) and associated contexts
    nvDisableHeaps();

    // Release all outstanding AGP heap allocations
    //   The DX AGP heap allocations are only valid on 1 head in multiheaded systems.
    //   If we do not free all allocations now, we'll attempt to in nvEnable and that
    //   may be while DX is being enabled on the head WITHOUT the AGP heap -- causes crash.
    nvAGPResetHeap();

    getDC()->dwEnableStatus = CDriverContext::DC_ENABLESTATUS_DISABLED;

    dbgTracePop();
    return (TRUE);
}

#endif  // NVARCH >= 0x04


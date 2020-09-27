//******************************Module*Header***********************************
//
// Module Name: heap.c
//
// Contains offscreen heap manager and create/delete device bitmap functions.
//
// Copyright (c) 1993-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"

#ifdef NVD3D
    #include "dx95type.h"
    #include "nvntd3d.h"
//    #include "nvProcMan.h"
    #include "ddmini.h"
#endif

// Define USE_RM_MMGR RM to use RM heap manager only
//#define USE_RM_MMGR     1

#ifndef USE_RM_MMGR // [
#define MIN_HEAP_SIZE       ( 128 * 1024)
#define FIRST_HEAP_SIZE_16  ( 512 * 1024)
#define FIRST_HEAP_SIZE_32  (1024 * 1024)

void FreeHeap(PDEV* ppdev, HEAP* pHeap);
HEAP* GetHeap(PDEV* ppdev, ULONG ulMinAlloc);

OH* heap_alloc(ULONG blksToAlloc, HEAP* pHeap);
BOOL heap_free(OH* pBlk);
BOOL heap_init(ULONG memBegin, ULONG memSize, HEAP* pHeap);
void heap_clear(HEAP* pHeap);
#endif // !USE_RM_MMGR ]

#ifdef FONT_CACHE // [
// Font cache definitions
//
// NOTE: If values are changed, must update NV4TEXT.ASM
//
#define MAX_FONTS       16
#define MAX_GLYPHS      256
#define GLYPH_PITCH     128
#define FONT_CACHE_SIZE MAX_FONTS*MAX_GLYPHS*GLYPH_PITCH
//
// End of font cache definitions
#endif // FONT_CACHE ]

// macro to clear the given poh with color
// FN: Disabled until someone needs it
#if 0//DBG
#define DBG_CLEARPOH(ppdev, poh, color)                                                                   \
{                                                                                                         \
    RECTL rclTrg = {0};                                                                                   \
    RBRUSH_COLOR rbc = {0};                                                                               \
    ppdev->pfnSetDestBase(ppdev, (ULONG)((BYTE *)(poh->LinearPtr) - ppdev->pjScreen), poh->LinearStride); \
    rbc.iSolidColor = color;                                                                              \
    rclTrg.right    = poh->cx;                                                                            \
    rclTrg.bottom   = poh->cy;                                                                            \
    ppdev->pfnFillSolid(ppdev, 1, &rclTrg, 0xF0F0, rbc, NULL);                                            \
}
#else
#define DBG_CLEARPOH(ppdev, poh, color)
#endif


//******************************Public*Routine**********************************
//
// Function: pohFree()
//
//      Frees an off-screen heap allocation.  The free space will be combined
//      with any adjacent free spaces to avoid segmentation of the heap.
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//     poh   - Pointer to the offscreen memory allocation to free
//
// Return Value:
//
//     None.
//
//******************************************************************************

void pohFree
(
    PDEV*   ppdev,
    OH*     poh
)
{
    if (poh == NULL)
        return;

#ifdef USE_RM_MMGR // [

    //**********************************************************************
    // We now support linear memory management
    //**********************************************************************

    //                       green
    DBG_CLEARPOH(ppdev, poh, 0xff00);
    NVHEAP_FREE(poh->ulOffset);

    //**********************************************************************
    // Remove this node from the allocation list.
    //**********************************************************************

    poh->next->prev = poh->prev;
    poh->prev->next = poh->next;

    //**************************************************************************
    // Free the struct space
    //**************************************************************************

    EngFreeMem(poh);

#else // USE_RM_MMGR ][
    {
        // poh is freed by heap_free so we need to get the heap pointer
        // before calling heap_free
        HEAP* pHeap = poh->pHeap;

        /*
        ** If there is alloc/free hysteresis here, then we have to return heap
        ** space in the following functions:
        **     bAssertModeOffscreenHeap()
        **     bMoveAllOffscreenDfbsToDibs()
        **     vDisableOffscreenHeap()
        */
        if (heap_free(poh) == 1)    // retval 1 means heap is empty
        {
            FreeHeap(ppdev, pHeap);
        }
    }
#endif // USE_RM_MMGR ]

    //**********************************************************************
    // Update the uniqueness to show that space has been freed, so that
    // we may decide to see if some DIBs can be moved back into off-screen
    // memory:
    //**********************************************************************

    ppdev->iHeapUniq++;
}


//******************************Public*Routine**********************************
//
// Function:   OH* pohAllocate
//
// Routine Description:
//
//      Allocates a piece in off-screen memory.
//
// Arguments:
//
//
// Return Value:
//
//        Ptr to OH structure representing offscreen memory
//
//******************************************************************************

OH* pohAllocate
(
    PDEV*   ppdev,
    LONG    cxThis,         // Width of rectangle to be allocated
    LONG    cyThis          // Height of rectangle to be allocated
)
{
    OH*     pohThis;    // Points to found available rectangle we'll use
    OH*     pohRoot;

    ULONG   LinearStride;
    ULONG   ulSizeInBytes;
    ULONG   ulAllocated;
    NvU32   dwStatus;

    //**************************************************************************
    // Align to appropriate boundary.
    //**************************************************************************

    LinearStride  = cxThis * ppdev->cjPelSize;
    LinearStride += ppdev->ulSurfaceAlign;
    LinearStride &= ~ppdev->ulSurfaceAlign;
    ulSizeInBytes = LinearStride * cyThis;

    //**************************************************************************
    // Request offscreen memory from linear memory manager
    //**************************************************************************

#ifdef USE_RM_MMGR // [

    NVHEAP_ALLOC(dwStatus, ulAllocated, ulSizeInBytes, TYPE_IMAGE);

    if (dwStatus != 0)
        return NULL;

    if ((pohThis = EngAllocMem(0, sizeof(OH), ALLOC_TAG)) == NULL)
    {
        NVHEAP_FREE(ulAllocated);
        return NULL;
    }

    //**************************************************************************
    // Fill in the OH structure for this allocation.
    //**************************************************************************

    pohThis->pdsurf         = NULL; // The caller owns and sets this field:
//  pohThis->next           = filled in below
//  pohThis->prev           = filled in below
    pohThis->nextEmpty      = NULL; // not used if RM doing all alloc/free
    pohThis->pHeap          = &ppdev->heap; // not used if RM doing all alloc/free
    pohThis->ulSizeInBytes  = ulSizeInBytes;
    pohThis->ulOffset       = ulAllocated;
    pohThis->ulLinearStride = LinearStride;

    //**************************************************************************
    // Now insert this new node at the head of the OH list:
    //**************************************************************************

    pohRoot = &ppdev->heap.oh;

    pohThis->next = pohRoot->next;
    pohThis->prev = pohRoot;

    pohRoot->next->prev = pohThis;
    pohRoot->next = pohThis;

#else // USE_RM_MMGR ][

    {
        HEAP* pHeap = &ppdev->heap;

        /*
        ** Try to allocate space from one of the heaps.
        */
        do
        {
            pohThis = heap_alloc(ulSizeInBytes, pHeap);
        }
        while ((pohThis == NULL) && ((pHeap = pHeap->pNextHeap) != NULL));

        /*
        ** If we couldn't allocate space allocate another heap and try again.
        */
        if (pohThis == NULL)
        {
            if ((pHeap = GetHeap(ppdev, ulSizeInBytes)) == NULL)
                return NULL;

            /*
            ** This should always succeed if the GetHeap above worked.
            */
            pohThis = heap_alloc(ulSizeInBytes, pHeap);

            /*
            ** This check should not be necessary but is here to be safe in
            ** case GetHeap() is changed to not allocate at least the min
            ** heap space requested in the above GetHeap call.
            */
            if (pohThis == NULL)
            {
                FreeHeap(ppdev, pHeap);
                return NULL;
            }
        }

        /*
        ** The linear memory manager does not have LinearStride so fill it in.
        */
        pohThis->ulLinearStride = LinearStride;


        //                           yellow
        DBG_CLEARPOH(ppdev, pohThis, 0xffff00);

    }
#endif // USE_RM_MMGR ]

    DISPDBG((7, "   Allocated (%li x %li) at offset %li",
                cxThis, cyThis, pohThis->ulOffset));

    return pohThis;
}


//******************************Public*Routine**********************************
//
// Function: BOOL bMoveDibToOffscreenDfbIfRoom
//
// Routine Description:
//
//      Converts the DIB DFB to an off-screen DFB, if there's room for it in
//      off-screen memory.
//
// Arguments:
//
//      ppdev - Pointer to the physical device structure
//
// Return Value:
//
//      FALSE if there wasn't room, TRUE if successfully moved.
//
//******************************************************************************


BOOL bMoveDibToOffscreenDfbIfRoom(
PDEV*   ppdev,
DSURF*  pdsurf)

    {
    OH*         poh;
    SURFOBJ*    pso;
    RECTL       rclDst;
    HSURF       hsurf;

    ASSERTDD(pdsurf->dt == DT_DIB,
             "Can't move a bitmap off-screen when it's already off-screen");

    //**************************************************************************
    // If we're in full-screen mode, or ddraw is currently loaded,
    // we can't move anything to off-screen memory:
    //**************************************************************************

    if (!ppdev->bEnabled || (ppdev->flStatus & STAT_DIRECTDRAW_ENABLED))
        return(FALSE);

    //**************************************************************************
    // Allocate an offscreen region
    //**************************************************************************

    poh = pohAllocate(ppdev, pdsurf->sizl.cx, pdsurf->sizl.cy);
    if (poh == NULL)
        {
        //**********************************************************************
        // There wasn't any free room.
        //**********************************************************************

        return(FALSE);
        }

    //**************************************************************************
    // 'pdsurf->sizl' is the actual bitmap dimension, not 'poh->cx' or
    // 'poh->cy'.
    //**************************************************************************

    rclDst.left   = 0;
    rclDst.top    = 0;
    rclDst.right  = pdsurf->sizl.cx;
    rclDst.bottom = pdsurf->sizl.cy;

    pdsurf->LinearPtr    = ppdev->pjFrameBufbase + poh->ulOffset;
    pdsurf->LinearStride = poh->ulLinearStride;

    vPutBits(ppdev,pdsurf, pdsurf->pso, &rclDst);

    //**************************************************************************
    // Update the data structures to reflect the new off-screen node:
    //**************************************************************************

    pso           = pdsurf->pso;
    pdsurf->dt    = DT_SCREEN;
    pdsurf->poh   = poh;
    poh->pdsurf   = pdsurf;

    //**************************************************************************
    // Now free the DIB.  Get the hsurf from the SURFOBJ before we unlock
    // it (it's not legal to dereference psoDib when it's unlocked):
    //**************************************************************************

    hsurf = pso->hsurf;
    EngUnlockSurface(pso);
    EngDeleteSurface(hsurf);

    return(TRUE);
    }

//******************************Public*Routine**********************************
//
// Function: BOOL pohMoveOffscreenDfbToDib
//
// Routine Description:
//
//       Converts the DFB from being off-screen to being a DIB.
//
//       Note: The caller does NOT have to call 'pohFree' on 'poh' after making
//             this call.
//
// Arguments:
//
//       ppdev - Pointer to the physical device structure
//
// Return Value:
//
//       NULL if the function failed (due to a memory allocation).
//       Otherwise, it returns a pointer to the coalesced off-screen heap
//       node that has been made available for subsequent allocations
//       (useful when trying to free enough memory to make a new
//       allocation).
//
//******************************************************************************

BOOL pohMoveOffscreenDfbToDib
(
    PDEV*       ppdev,
    OH*         poh
)
{
    DSURF*      pdsurf;
    HBITMAP     hbmDib;
    SURFOBJ*    pso;
    RECTL       rclDst;

    if ((pdsurf = poh->pdsurf) == NULL)
        return TRUE;

    DISPDBG((2, "Throwing out %li x %li at offset %li!",
                 pdsurf->sizl.cx, pdsurf->sizl.cy, poh->ulOffset));

    ASSERTDD(pdsurf->dt != DT_DIB,
            "Can't make a DIB into even more of a DIB");

    //**************************************************************************
    // Create the Bitmap for NT GDI to use
    //**************************************************************************

    hbmDib = EngCreateBitmap(pdsurf->sizl, 0, ppdev->iBitmapFormat, BMF_TOPDOWN, NULL);

    if (hbmDib)
        {

        //**********************************************************************
        // Specify flhooks = 0
        //**********************************************************************

        if (EngAssociateSurface((HSURF) hbmDib, ppdev->hdevEng, 0))
            {
            pso = EngLockSurface((HSURF) hbmDib);
            if (pso != NULL)
                {
                rclDst.left   = 0;
                rclDst.top    = 0;
                rclDst.right  = pdsurf->sizl.cx;
                rclDst.bottom = pdsurf->sizl.cy;

                vGetBits(ppdev, pdsurf, pso, &rclDst);

                //**************************************************************
                // sizel field  is already set. Does not need to be modified.
                // Does ppdev field need to be set to NULL here ???
                // No, not really. This is still a DFB and as such is associated
                // with our Device.  A DFB implies that the surface is in a
                // DEVICE format. But for NV, we're just using the standard DIB
                // format.  Essentially, we can just leave the ppdev field alone.
                // Besides, this DFB may get put back in offscreen memory and
                // at that point we may need the ppdev again.
                //**************************************************************

                pdsurf->dt    = DT_DIB;
                pdsurf->pso   = pso;

                //**************************************************************
                // Don't even bother checking to see if this DIB should
                // be put back into off-screen memory until the next
                // heap 'free' occurs:
                //**************************************************************

                pdsurf->iUniq = ppdev->iHeapUniq;
                pdsurf->cBlt  = 0;

                //**************************************************************
                // Remove this node from the off-screen DFB list, and free
                // it.  'pohFree' will never return NULL:
                //**************************************************************

                pohFree(ppdev, poh);
                return TRUE;
                }
            else
                {
                DISPDBG((0,"ERROR in pohMoveOffscreenDfbToDib: EngLockSurface failed on poh:0x%p with 0x%p!",poh,hbmDib));
                }
            }
        else
            {
            DISPDBG((0,"ERROR in pohMoveOffscreenDfbToDib: EngAssociateSurface failed on poh:0x%p with 0x%p!",poh,hbmDib));
            }

        //**********************************************************************
        // Fail case:
        //**********************************************************************

        EngDeleteSurface((HSURF) hbmDib);
        }
    else
        {
        DISPDBG((0,"ERROR in pohMoveOffscreenDfbToDib: EngCreateBitmap failed on poh:0x%p (%d x %d)!",poh,pdsurf->sizl.cx,pdsurf->sizl.cy));
        }
    return FALSE;
}


//******************************Public*Routine**********************************
//
// Function: bMoveAllDfbsFromOffscreenToDibs
//
// Routine Description:
//
//      This function is used when we're about to enter full-screen mode, which
//      would wipe all our off-screen bitmaps.  GDI can ask us to draw on
//      device bitmaps even when we're in full-screen mode, and we do NOT have
//      the option of stalling the call until we switch out of full-screen.
//      We have no choice but to move all the off-screen DFBs to DIBs.
//
// Arguments:
//
//      ppdev - Pointer to the physical device structure
//
// Return Value:
//
//      Returns TRUE if all DSURFs have been successfully moved.
//
//******************************************************************************

BOOL __fastcall bMoveAllDfbsFromOffscreenToDibs(PDEV* ppdev)
{
    DSURF*  pdsurf;
    BOOL    bRet = TRUE;    // Assume success

    pdsurf = ppdev->pdsurf;

    while (pdsurf)
    {
        if (pdsurf->dt == DT_SCREEN)
        {
            //JOEHACK - why would we keep trying if this fails???
            if (!pohMoveOffscreenDfbToDib(ppdev, pdsurf->poh))
                bRet = FALSE;
        }

        pdsurf = pdsurf->next;
    }

#if defined(USE_RM_MMGR) && defined(OLD_WAY) // [
    {
        OH*     poh;
        OH*     pohNext;

        //**************************************************************************
        // Make sure that heap was already initialized in EnableOffscreenHeap.
        //**************************************************************************

        poh = ppdev->heap.oh.next;

        if (poh == NULL)
            return TRUE;

        while (poh != &ppdev->heap.oh)
        {
            pohNext = poh->next;

            //JOEHACK - why would we keep trying if this fails???
            if (!pohMoveOffscreenDfbToDib(ppdev, poh))
                bRet = FALSE;

            poh = pohNext;
        }
    }
#endif // defined(USE_RM_MMGR) && defined(OLD_WAY) ]

    return bRet;
}


//******************************Public*Routine**********************************
//
// Function:    DrvCreateDeviceBitmap
//
// Routine Description:
//
//   Function called by GDI to create a device-format-bitmap (DFB).  We will
//   always try to allocate the bitmap in off-screen; if we can't, we simply
//   fail the call and GDI will create and manage the bitmap itself.
//
//   Note: We do not have to zero the bitmap bits.  GDI will automatically
//         call us via DrvBitBlt to zero the bits (which is a security
//         consideration).
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

HBITMAP DrvCreateDeviceBitmap(
DHPDEV  dhpdev,
SIZEL   sizl,
ULONG   iFormat)

    {
    PDEV*   ppdev;
    OH*     poh;
    DSURF*  pdsurf;
    HBITMAP hbmDevice;

    ppdev = (PDEV*) dhpdev;

    //**************************************************************************
    // If we're in full-screen mode, we hardly have any off-screen memory
    // in which to allocate a DFB.  LATER: We could still allocate an
    // OH node and put the bitmap on the DIB DFB list for later promotion.
    //**************************************************************************

    if (!ppdev->bEnabled)
        return(0);

    //**************************************************************************
    // We only support device bitmaps that are the same colour depth
    // as our display.
    //
    // Actually, those are the only kind GDI will ever call us with,
    // but we may as well check.  Note that this implies you'll never
    // get a crack at 1bpp bitmaps.
    //**************************************************************************

    if (iFormat != ppdev->iBitmapFormat)
        return(0);

    //**************************************************************************
    // Do NOT support device bitmaps when OpenGL or ddraw is enabled !
    // OpenGL MCD and ddraw currently expect to manage all of offscreen memory.
    //**************************************************************************

    //**************************************************************************
    // We don't want anything 8x8 or smaller -- they're typically brush
    // patterns which we don't particularly want to stash in off-screen
    // memory:
    //
    // (Some of the screen savers like 'Dangerous Creatures' have very wide
    // offscreen bmaps).  CorelDream3D and RayDream use very TALL bitmaps (0x20 x 0x4400).
    // So we also need to limit the height of the offscreen bitmaps too.
    //**************************************************************************

    if ( ((sizl.cx <= 8) && (sizl.cy <= 8)) ||
         ((sizl.cx == 0x30) && (sizl.cy == 0x0E)) || // gains ~2% on 32bpp WB99BG
          (sizl.cx > ppdev->cxScreen * 2) ||
          (sizl.cy > ppdev->cyScreen * 2) ||
          (sizl.cx > ppdev->cxSurfaceMax) || // don't exceed h/w limits
          (sizl.cy > MAX_CY_MEMORY_VALUE)    // don't exceed h/w limits
       )
    {
        return 0;
    }


//******************************************************************************
//
//  Remove this code since it hurts OGL performance when running with GDI.
//
//    if (OglIsEnabled(ppdev))
//        return(0);
//
//******************************************************************************

    //**************************************************************************
    // When OpenGL is rendering in FULL screen mode,
    // don't create any offscreen bitmap so that display driver will not perform
    // any render in GDI channel in order to avoid the channel switch from OpenGL.
    //**************************************************************************
    if(globalOpenGLData.oglDrawableClientCount)
        if(ppdev->ulOpenGLinFullScreen)
            return(0);

#if defined(NVD3D)
    if (ppdev->pDriverData && ppdev->pDriverData->DDrawVideoSurfaceCount)
#else
    if (ppdev->DDrawVideoSurfaceCount)
#endif // !NVD3D
        {
        return(0);
        }

    poh = pohAllocate(ppdev, sizl.cx, sizl.cy);

    if (poh != NULL)
        {
        pdsurf = EngAllocMem(FL_ZERO_MEMORY, sizeof(DSURF), ALLOC_TAG);
        if (pdsurf != NULL)
            {
            hbmDevice = EngCreateDeviceBitmap((DHSURF) pdsurf, sizl, iFormat);
            if (hbmDevice != NULL)
                {

                if (EngAssociateSurface((HSURF) hbmDevice, ppdev->hdevEng,
                                        ppdev->flHooksDeviceBitmap))
                    {
                    pdsurf->dt           = DT_SCREEN;
                    pdsurf->sizl         = sizl;
                    pdsurf->ppdev        = ppdev;
                    poh->pdsurf          = pdsurf;
                    pdsurf->poh          = poh;
                    pdsurf->LinearPtr    = ppdev->pjFrameBufbase + poh->ulOffset;
                    pdsurf->LinearStride = poh->ulLinearStride;

                    /*
                    ** Add this pdsurf to the head of our pdsurf list.
                    */
                    pdsurf->prev = NULL;
                    pdsurf->next = ppdev->pdsurf;
                    if (ppdev->pdsurf)
                        ppdev->pdsurf->prev = pdsurf;
                    ppdev->pdsurf = pdsurf;

                    return(hbmDevice);
                    }
                else
                    {
                    DISPDBG((0, "ERROR in DrvCreateDeviceBitmap: EngAssociateSurface on hbmDevice:0x%p", hbmDevice));
                    }

                EngDeleteSurface((HSURF) hbmDevice);
                }
            else
                {
                DISPDBG((0, "ERROR in DrvCreateDeviceBitmap: EngCreateDeviceBitmap failed on pdsurf:0x%p", pdsurf));
                }
            EngFreeMem(pdsurf);
            }
        else
            {
            DISPDBG((0, "ERROR in DrvCreateDeviceBitmap: EngAllocMem failed to alloc %d bytes", sizeof(DSURF)));
            }

        pohFree(ppdev, poh);
        }

    return(0);
    }

//******************************Public*Routine**********************************
//
// Function: DrvDeleteDeviceBitmap
//
// Routine Description:
//
//      Deletes a DFB.
//
//      Since we implement DrvDeriveSurface on W2k, there are two cases to
//      handle here.  One case is the classic case where this DFB to delete
//      was created by an eariler successful call to DrvCreateDeviceBitmap.
//      The other case is where this DFB to delete was created by an eariler
//      successful call to DrvDeriveSurface.  In the latter case, there will
//      never be a non-NULL poh because the offscreen memory had already been
//      allocated by DirectDraw when DrvDeriveSurface was called.
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID DrvDeleteDeviceBitmap
(
    DHSURF      dhsurf
)
{
    DSURF*      pdsurf;
    PDEV*       ppdev;
    SURFOBJ*    psoDib;
    HSURF       hsurfDib;

    pdsurf = (DSURF *)dhsurf;
    ppdev  = pdsurf->ppdev;

    if (pdsurf->dt == DT_SCREEN)
    {
        // If this DFB was created by DrvDeriveSurface, poh will be NULL.
        if (pdsurf->poh)
            pohFree(ppdev, pdsurf->poh);

        // Comment from DDK500\src\video\displays\3dlabs\driver\heap.c:
        // Note: we don't need to call EngDeleteSurface(psurf->hsurf) to delete
        // the HBITMAP we created in DrvCreateDeviceBitmap() or DrvDeriveSurface()
        // because GDI will take care of this when it call DrvDeleteDeviceBitmap
        //

    }
    else
    {
        ASSERTDD(pdsurf->dt == DT_DIB, "Expected DIB type");

        //**********************************************************************
        // Get the hsurf from the SURFOBJ before we unlock it (it's not
        // legal to dereference psoDib when it's unlocked):
        //**********************************************************************

        psoDib = pdsurf->pso;
        hsurfDib = psoDib->hsurf;
        EngUnlockSurface(psoDib);
        EngDeleteSurface(hsurfDib);
    }

    /*
    ** Remove this dsurf from our ppdev->pdsurf list.  DrvDeriveSurface dsurfs
    ** will never be in the ppdev->pdsurf list but DrvDeriveSurface dsurfs
    ** always have (NULL==next==prev) so these DrvDeriveSurface dsurfs don't
    ** cause anything to happen below.
    **
    ** Note that (NULL==next==prev) cannot be used to detect a DrvDeriveSurface
    ** created dsurf because this will also be true for a DrvCreateDeviceBitmap
    ** dsurf when there is only one in a PDEV.
    **
    ** Also note that since DrvDeriveSurface dsurfs always have a NULL prev
    ** pointer we must check that the dsurf being deleted is indeed the head
    ** of the ppdev->pdsurf before unlinking the old head.
    */
    if (pdsurf->next)
        pdsurf->next->prev = pdsurf->prev;

    if (pdsurf->prev)
        pdsurf->prev->next = pdsurf->next;
    else if (ppdev->pdsurf == pdsurf)
        ppdev->pdsurf = pdsurf->next;   // new head of list

    EngFreeMem(pdsurf);
}

//******************************Public*Routine**********************************
//
// Function: bAssertModeOffscreenHeap
//
// Routine Description:
//
//   This function is called whenever we switch in or out of full-screen
//   mode.  We have to convert all the off-screen bitmaps to DIBs when
//   we switch to full-screen (because we may be asked to draw on them even
//   when in full-screen, and the mode switch would probably nuke the video
//   memory contents anyway).
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     True if successful.
//
//******************************************************************************

BOOL bAssertModeOffscreenHeap
(
    PDEV*   ppdev,
    BOOL    bEnable
)
{
    if (!bEnable)
        return bMoveAllDfbsFromOffscreenToDibs(ppdev);

    return TRUE;
}

//******************************Public*Routine**********************************
//
// Function: vDisableOffscreenHeap
//
// Routine Description:
//
//     Frees any resources allocated by the off-screen heap.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

VOID vDisableOffscreenHeap
(
    PDEV*   ppdev
)
{
    SURFOBJ*    psoPunt;
    HSURF       hsurf;

    psoPunt = ppdev->psoPunt;
    if (psoPunt != NULL)
        {
        hsurf = psoPunt->hsurf;
        EngUnlockSurface(psoPunt);
        EngDeleteSurface(hsurf);
        }

    psoPunt = ppdev->psoPunt2;
    if (psoPunt != NULL)
        {
        hsurf = psoPunt->hsurf;
        EngUnlockSurface(psoPunt);
        EngDeleteSurface(hsurf);
        }

    psoPunt = ppdev->psoPunt3;
    if (psoPunt != NULL)
        {
        hsurf = psoPunt->hsurf;
        EngUnlockSurface(psoPunt);
        EngDeleteSurface(hsurf);
        }

#ifndef USE_RM_MMGR // [
    //**************************************************************************
    // Free heap space allocated from from RM.
    //**************************************************************************
    {
        HEAP* pHeap = &ppdev->heap;

        // This should never have to free any heap space unless we are using
        // hysteresis in the alloc/free heap algorithm.  That is if we are
        // freeing heaps as they become empty instead of deferring that work,
        // this function should never be called with any offscreen currently
        // in use.

        /*
        ** Start with the last heap in list.
        */
        while (pHeap->pNextHeap)
            pHeap = pHeap->pNextHeap;

        /*
        ** Return heap space back to RM in reverse order of allocation.
        */
        do
        {
            /*
            ** This check is required because the first heap struct is part of
            ** PDEV instead of being allocated so the only way we know that it
            ** is empty (no space allocated for the heap) is to check its size.
            */
            if (pHeap->ulSizeInBytes)
                FreeHeap(ppdev, pHeap);
        }
        while ((pHeap = pHeap->pPrevHeap) != NULL);
    }
#endif // !USE_RM_MMGR ]

    //**********************************************************************
    // Destroy the offscreen heap.
    //**********************************************************************

    NVHEAP_CLEAR(NULL);
}


//******************************Public*Routine**********************************
//
// Function: bEnableOffscreenHeap
//
// Routine Description:
//
//      Initializes the off-screen heap using all available video memory,
//      accounting for the portion taken by the visible screen.
//
//      Input: ppdev->cxScreen
//             ppdev->cyScreen
//             ppdev->cxMemory
//             ppdev->cyMemory
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

BOOL bEnableOffscreenHeap
(
    PDEV*       ppdev
)
{
    SIZEL       sizl;
    HSURF       hsurf;
    ULONG       xSize;

    //**************************************************************************
    // Initialize offscreen heap manager
    //**************************************************************************
    NVHEAP_INIT(0, ppdev->cbFrameBuf - 1, NULL);

    //**************************************************************************
    // cxMemory is Width of Video Memory (in pixels, not bytes)
    // cyMemory is Height of All of Video Memory (in scanlines)
    //
    //      cx/cyMemory encompasses all of video memory, which includes
    //      any additional 'right-hand' side memory, which may be present if
    //      the Width of the screen != pitch/stride of mode.
    //
    // NOTE:  NV Instance memory exists at the end of offscreen memory.
    //        This has already been compensated for by the miniport.
    //        The Miniport reduces the available amount of offscreen memory
    //        before it returns it to the display driver.  See NVValidateModes
    //        for more information.
    //**************************************************************************

    DISPDBG((1, "Screen: %li x %li  Memory: %li x %li",
        ppdev->cxScreen, ppdev->cyScreen, ppdev->cxMemory, ppdev->cyMemory));

    ASSERTDD((ppdev->cxScreen <= ppdev->cxMemory) &&
             (ppdev->cyScreen <= ppdev->cyMemory),
             "Memory must not have smaller dimensions than visible screen!");

#ifdef USE_RM_MMGR // [
    //**************************************************************************
    // Init the heap.oh structure links.
    //**************************************************************************

    ppdev->heap.oh.pdsurf         = NULL;
    ppdev->heap.oh.next           = &ppdev->heap.oh;
    ppdev->heap.oh.prev           = &ppdev->heap.oh;
    ppdev->heap.oh.nextEmpty      = NULL;
    ppdev->heap.oh.pHeap          = &ppdev->heap;
    ppdev->heap.oh.ulSizeInBytes  = 0;
    ppdev->heap.oh.ulOffset       = 0;
    ppdev->heap.oh.ulLinearStride = 0;
#endif // USE_RM_MMGR ]

    //**************************************************************************
    // Allocate a 'punt' SURFOBJ we'll use when the device-bitmap is in
    // off-screen memory, but we want GDI to draw to it directly as an
    // engine-managed surface:
    //**************************************************************************

    sizl.cx = ppdev->cxMemory;
    sizl.cy = ppdev->cyMemory;

    //**************************************************************************
    // We don't want to hook any drawing functions for the punt surfaces, since
    // they are only there to allow GDI to perform drawing operations which we
    // don't support in hardware. We do need to ask GDI to call us prior to drawing,
    // to ensure the hw graphics engine is idle, since these surfaces represent bitmaps
    // stored in graphics memory.
    //**************************************************************************

    hsurf = (HSURF) EngCreateBitmap(sizl,
                                    0xbadf00d,
                                    ppdev->iBitmapFormat,
                                    BMF_TOPDOWN,
                                    (VOID*) 0xbadf00d);

    if ((hsurf == 0)                                     ||
        (!EngAssociateSurface(hsurf, ppdev->hdevEng, 0)) ||
        (!(ppdev->psoPunt = EngLockSurface(hsurf))))
        {
        DISPDBG((2, "Failed punt surface creation"));

        EngDeleteSurface(hsurf);
        goto ReturnFalse;
        }

    //**************************************************************************
    // We need another for doing DrvBitBlt and DrvCopyBits when both
    // surfaces are off-screen bitmaps:
    //**************************************************************************

    hsurf = (HSURF) EngCreateBitmap(sizl,
                                    0xbadf00d,
                                    ppdev->iBitmapFormat,
                                    BMF_TOPDOWN,
                                    (VOID*) 0xbadf00d);

    //**************************************************************************
    // These 'punt' services are necessary for when we use GDI to
    // draw on a 'Device' format bitmap.  Instead of passing the SURFOBJ
    // with the Device Surface in it, we use these temporary wrapper surfaces.
    //**************************************************************************

    if ((hsurf == 0)                                     ||
        (!EngAssociateSurface(hsurf, ppdev->hdevEng, 0)) ||
        (!(ppdev->psoPunt2 = EngLockSurface(hsurf))))
        {
        DISPDBG((2, "Failed punt surface creation"));

        EngDeleteSurface(hsurf);
        goto ReturnFalse;
        }


    //**************************************************************************
    // We need another punt surface for translating multi-colored brushes
    //**************************************************************************

    hsurf = (HSURF) EngCreateBitmap(sizl,
                                    ppdev->lDelta,
                                    ppdev->iBitmapFormat,
                                    BMF_TOPDOWN,
                                    ppdev->pjFrameBufbase);

    //**************************************************************************
    // These 'punt' services are necessary for when we use GDI to
    // draw on a 'Device' format bitmap.  Instead of passing the SURFOBJ
    // with the Device Surface in it, we use these temporary wrapper surfaces.
    //**************************************************************************

    if ((hsurf == 0)                                     ||
        (!EngAssociateSurface(hsurf, ppdev->hdevEng, 0)) ||
        (!(ppdev->psoPunt3 = EngLockSurface(hsurf))))
        {
        DISPDBG((2, "Failed punt surface creation"));

        EngDeleteSurface(hsurf);
        goto ReturnFalse;
        }

    //**************************************************************************
    // Point the punt surface back to the ppdev
    //**************************************************************************
    ppdev->psoPunt->dhpdev = (DHPDEV) ppdev;
    ppdev->psoPunt2->dhpdev = (DHPDEV) ppdev;
    ppdev->psoPunt3->dhpdev = (DHPDEV) ppdev;


    DISPDBG((5, "Passed bEnableOffscreenHeap"));

#ifdef FONT_CACHE // [
    /*
    ** JOEHACK
    ** If we go back to glyph caching we should create and Enable, Disable and
    ** AssertMode GlyphCache() funtions and call them as appropiate from Enable
    ** and Disable Surface and AssertMode().
    */
#ifndef NV3
    //**************************************************************************
    // Check if screen rectangle allocation succeeded
    //**************************************************************************

    if (poh != NULL) {

    //**************************************************************************
    // Reserve the font cache area in offscreen memory.
    //**************************************************************************

        if (ppdev->fontCacheEnabled) {
            xSize = ppdev->cxScreen;
            poh = pohAllocate  (ppdev,
                                xSize/ppdev->cjPelSize,
                                (FONT_CACHE_SIZE+xSize-1)/xSize,
                                FLOH_MAKE_PERMANENT);
            if (poh == NULL)
                ppdev->fontCacheEnabled = FALSE;
            else {
                ppdev->fontCacheOffset = (ULONG)((BYTE *)poh->LinearPtr - ppdev->pjFrameBufbase);
            }

        }
    }
#endif // NV3
#endif // FONT_CACHE ]

    return(TRUE);

ReturnFalse:

    DISPDBG((2, "Failed bEnableOffscreenHeap"));

    return(FALSE);
}


#if _WIN32_WINNT >= 0x0500 // [
/*****************************************************************************\
*  HBITMAP DrvDeriveSurface
*
*   This function derives and creates a GDI surface from
*   specified DirectDraw surface.
*
*   Parameters:
*   pDirectDraw     Points to a DD_DIRECTDRAW_GLOBAL structure that
*                   describes the DirectDraw obect.
*   pSurface        Points to a DD_SURFACE_LOCAL structure that
*                   describes DirectDraw surface around which to wrap
*                   a GDI surface.
*
*   Return Value:
*   DrvDeriveSurface returns a handle to the created GDI surface
*   upon succeed. It returns NULL if the call fails or
*   if if the river cannot accelerate GDI drawing to
*   the specified DirectDraw surface.
*
*   Comments:
*   DrvDeriveSurface allows the driver to create a GDI surface around a
*   DirectDraw video memory or AGP surface object in order to allow
*   acceleration of GDI drawing to the surface. If the driver does not
*   hook this call, all drawing to DirectDRaw surfaces is done in software
*   using DIB engine.
*   GDI calls DrvDeriveSurface with RGB surfaces only.
*   The driver should call DrvCreateDeviceBitmap to create a GDI surface
*   of the same size and format as that of the DirectDraw surface. Space
*   for the actual pixels need not be allocated since it already exists.
*
*   Note:
*
\*****************************************************************************/

HBITMAP DrvDeriveSurface(
    DD_DIRECTDRAW_GLOBAL    *pDirectDraw,
    DD_SURFACE_LOCAL        *pSurface
)
{
    PPDEV               ppdev   = (PPDEV)pDirectDraw->dhpdev;
    SIZEL               sizel;
    HBITMAP             hbmDevice;
    DSURF               *pdsurf;
    DD_SURFACE_GLOBAL   *pSurfaceGlobal = pSurface->lpGbl;

    //  Conditions that can't create GDI surface on DirectDraw surface.

    if(pSurface->ddsCaps.dwCaps & (DDSCAPS_NONLOCALVIDMEM | DDSCAPS_TEXTURE))
    {
        //  Currently, we don't support create GDI surface over AGP
        //  memory, or textures (swizzled).

        return(0);
    }

    pdsurf  = (DSURF *)EngAllocMem(
        FL_ZERO_MEMORY, sizeof(DSURF), ALLOC_TAG);

    if(pdsurf != NULL)
    {
        sizel.cx    = pSurfaceGlobal->wWidth;
        sizel.cy    = pSurfaceGlobal->wHeight;

        hbmDevice   = EngCreateDeviceBitmap(
            (DHSURF)ppdev, sizel, ppdev->iBitmapFormat);

        if(hbmDevice != NULL)
        {
            //  HOOK_SYNCHRONIZE must always be hooked when we
            //  give GDI a pointer to the bitmap bits. (flHook
            //  already has this bit set.)

            if(EngModifySurface(
                (HSURF)hbmDevice, ppdev->hdevEng, ppdev->flHooks,
                MS_NOTSYSTEMMEMORY, (DHSURF)pdsurf,
                ppdev->pjFrameBufbase + pSurfaceGlobal->fpVidMem,
                pSurfaceGlobal->lPitch, NULL))
            {
                pdsurf->dt  = DT_SCREEN;
                pdsurf->sizl= sizel;
                pdsurf->ppdev= ppdev;
                pdsurf->LinearPtr   = ppdev->pjFrameBufbase + pSurfaceGlobal->fpVidMem;
                pdsurf->LinearStride= pSurfaceGlobal->lPitch;

                return(hbmDevice);
            }

            //  Delete the surface handle if failed.

            EngDeleteSurface((HSURF)hbmDevice);
        }

        //  Can't create handle to the bitmap, free allocated resources.

        EngFreeMem(pdsurf);
    }

    //  We can't allocate memory for the device surface object, we must
    //  return failure.

    return(0);
}
#endif // _WIN32_WINNT >= 0x0500 ]

#ifndef USE_RM_MMGR // [
//******************************Public*Routine**********************************
//
// Function:   void FreeHeap
//
// Routine Description:
//
//     Calls RM to free a heap.
//
// Arguments:
//
//     pointer to current PDEV
//     poniter to heap to free
//
// Return Value:
//
//******************************************************************************

void FreeHeap
(
    PDEV*   ppdev,
    HEAP*   pHeap
)
{
    /*
    ** Free structs used to manage this heap space.
    */
    heap_clear(pHeap);

    /*
    ** Call RM to return this heap space to its pool.
    */
    NVHEAP_FREE(pHeap->ulOffset);

    /*
    ** If this is not the root heap unlink it and free the heap struct.
    */
    if (pHeap != &ppdev->heap)
    {
        if (pHeap->pNextHeap)
            pHeap->pNextHeap->pPrevHeap = pHeap->pPrevHeap;

        if (pHeap->pPrevHeap)
            pHeap->pPrevHeap->pNextHeap = pHeap->pNextHeap;

        EngFreeMem(pHeap);
    }
    else // this is the root heap so just mark it as empty
    {
        pHeap->ulSizeInBytes = 0;
        pHeap->ulOffset      = 0;
    }
}

//******************************Public*Routine**********************************
//
// Function:   HEAP* GetHeap
//
// Routine Description:
//
//     Calls RM to allocates another heap.
//
// Arguments:
//
//     pointer to current PDEV
//     size in bytes of minimum allocation requested
//
// Return Value:
//
//     Ptr to HEAP structure (success) or NULL
//
//******************************************************************************

HEAP* GetHeap
(
    PDEV*   ppdev,
    ULONG   ulMinAlloc
)
{
    ULONG   ulAllocated = 0;
    ULONG   ulAllocSize;
    HEAP*   pNewHeap;
    HEAP*   pHeap;
    NvU32   dwStatus;

    /*
    ** Don't allocate heap space smaller than MIN_HEAP_SIZE
    */
    if (ulMinAlloc < MIN_HEAP_SIZE)
        ulMinAlloc = MIN_HEAP_SIZE;

    /*
    ** First heap will contain the persistent DFBS allocated right after a mode
    ** switch and some extra space so we won't usually have to allocate a big
    ** chunk.  If we do allocate a big chunk, it will be after Windows has
    ** already allocated its persistent DFBs so that allocation shouldn't last
    ** long.
    **
    ** We allocate TYPE_CURSOR from them RM because it will high-justify these
    ** allocations.  We do this so that when we free one of these chunks to
    ** make room for Dx/Ogl the freed space should be adjacent the current
    ** free memory since Dx/Ogl allocations are low-justified.  This leaves a
    ** single large free block to work with instead of 2 smaller free blocks.
    */

    if (ppdev->heap.ulSizeInBytes == 0)
    {
        if (ppdev->cjPelSize == 4)
            ulAllocSize = FIRST_HEAP_SIZE_32;
        else
            ulAllocSize = FIRST_HEAP_SIZE_16;  // same for 8bpp
    }
    else
    {
        // RoundUp(2 * screensize, 2^n)
        ulAllocSize = 4 * MIN_HEAP_SIZE;
        while (ulAllocSize < (ULONG)(2 * ppdev->cyScreen * ppdev->cxScreen * ppdev->cjPelSize))
        {
            ulAllocSize *= 2;
        }
    }

    /*
    ** Allocate a heap chunk from the RM memmgr.  Start with a base size and
    ** if that fails, try to allocate something half that size.
    */
    do
    {
        NVHEAP_ALLOC(dwStatus, ulAllocated, ulAllocSize, TYPE_CURSOR);
    }
    while ((dwStatus!=0) && (ulAllocSize /= 2) >= ulMinAlloc);

    if (dwStatus!=0)
    {
        DISPDBG((0, "GetHeap failed to initialize heap. Last try: %d byte", ulAllocSize));
        return NULL;
    }

    /*
    ** This is kind of a hack but rather than changing the USE_RM_MMGR case
    ** which handles only a single heap, here we deal with the mess associated
    ** with the first heap struct allocated within ppdev and subsequent heap
    ** structs being allocated.
    */

    if (ppdev->heap.ulSizeInBytes == 0)
    {
        memset(&ppdev->heap, 0, sizeof(ppdev->heap));

        pNewHeap = &ppdev->heap;
    }
    else
    {
        /*
        ** Running out of system memory sould be a rare event compared to running
        ** out of offscreen memory so we allocate the space for the heap structure
        ** after we know we have the offscreen space.
        */
        pNewHeap = EngAllocMem(FL_ZERO_MEMORY, sizeof(HEAP), ALLOC_TAG);
        if (pNewHeap == NULL)
        {
            NVHEAP_FREE(ulAllocated);
            return NULL;
        }

        /*
        ** Walk to last heap in list.
        */
        pHeap = &ppdev->heap;
        while (pHeap->pNextHeap)
            pHeap = pHeap->pNextHeap;

        pHeap->pNextHeap = pNewHeap;
        pNewHeap->pPrevHeap = pHeap;
    }

    /*
    ** Initialize size info for this heap.
    */
    pNewHeap->ulSizeInBytes = ulAllocSize;
    pNewHeap->ulOffset      = ulAllocated;

    /*
    ** Initialize memory manager structs for this heap.
    */
    if (heap_init(ulAllocated, pNewHeap->ulSizeInBytes, pNewHeap) == 0)
    {
        FreeHeap(ppdev, pNewHeap);
        return NULL;
    }

    return pNewHeap;
}

/*
** This linear heap manager is massaged from the one that was previously in
** this file.  It has been changed to use OH structs and the OH struct was
** changed to incorporate the memory management components in the memory_t
** structure that the original version used.
*/

// Don't use the memstruc.h versions of these macros.
#undef EMPTY
#undef IS_EMPTY
#undef SET_EMPTY
#undef SET_BUSY
#define EMPTY           0x1
#define IS_EMPTY(pBlk)  ((ULONG_PTR)(pBlk)->prev &  EMPTY)
#define SET_EMPTY(pBlk) ((pBlk)->prev = (OH *)((ULONG_PTR)(pBlk)->prev |  EMPTY))
#define SET_BUSY(pBlk)  ((pBlk)->prev = (OH *)((ULONG_PTR)(pBlk)->prev & ~EMPTY))

//----------------------------------------------------------------------------
//
//  Function:   heap_alloc
//
//  Routine Description:
//
//      Allocates a block of memory and cleans up the heap
//
//  Arguments:
//
//  Return Value:
//
//----------------------------------------------------------------------------
OH* heap_alloc
(
    ULONG   ulBytesToAlloc,
    HEAP*   pHeap
)
{
    OH*     pBlk;
    OH*     pPrevEmpty;
    OH*     pNextEmpty;
    ULONG   ulBytesAvail;

    /*
    ** Check for a valid initialized heap.
    */
    if ((pHeap == NULL) || (pHeap->ulSizeInBytes == 0))
        return NULL;

    //**************************************************************************
    // Scan through the free list for a block large enough
    //
    // pHeap->oh.next  (Initial look at heap)
    //        |                                               -------
    //        |  ------- nextEmpty --------   --- nextEmpty--| EMPTY |
    //        | |                          | |                -------
    //        v |                          v |
    //       1st Blk      2nd Blk       3rd Blk       Last Blk
    //       ----          ----          ----          ----
    //      |    |  --->  |    |  --->  |    |  --->  |    | ---
    //      |    |  <---  |    |  <---  |    |  <---  |    |    |
    //       ----          ----          ----          ----     |
    //         ^                                                |
    //         |                                                |
    //          ------------------------------------------------
    //
    //      EMPTY          BUSY          EMPTY         BUSY
    //
    //     No Space       Contains      Contains       Contains
    //     Available         no          video           no
    //                      data         memory         data
    //
    //  ------------------------------------------------------------------------
    // First time heap_alloc is called:
    //
    //                                  pBlk
    //                                     |
    //                                     v
    //       1st Blk      2nd Blk       3rd Blk       Last Blk
    //       ----          ----          ----          ----
    //      |    |  --->  |    |  --->  |    |  --->  |    | ---
    //      |    |  <---  |    |  <---  |    |  <---  |    |    |
    //       ----          ----          ----          ----     |
    //         ^                                                |
    //         |                                                |
    //          ------------------------------------------------
    //**************************************************************************

    pBlk = pHeap->oh.next;   // points to 1st block in heap

    do
    {
        pPrevEmpty = pBlk ;

        pBlk = pBlk->nextEmpty;

        if (pBlk == (void *)EMPTY)
            return 0;

        ulBytesAvail = pBlk->ulSizeInBytes;
    }
    while (ulBytesAvail < ulBytesToAlloc);

    //**************************************************************************
    // At this point, pBlk points to an entry which contains enough memory
    // to satisfy the allocation request.
    //
    // Note: We search thru the 'EMPTY' (UNUSED) entries, not the 'BUSY' (USED) entries
    //**************************************************************************

    if (ulBytesAvail == ulBytesToAlloc)
    {
        //**********************************************************************
        // We're giving the caller the whole thing
        // So all we need to do is set pNextEmpty
        //**********************************************************************
        pNextEmpty = pBlk->nextEmpty;
    }
    else
    {
        //**********************************************************************
        // Create a new 'EMPTY' entry which will hold the currently unused
        // portion of memory (EMPTY essentially means UNUSED in this
        // heap manager). pBlk will hold the currently allocated portion of memory
        //**********************************************************************

        pNextEmpty = EngAllocMem(FL_ZERO_MEMORY, sizeof(OH), ALLOC_TAG);

        //  if pNextEmpty is NULL ptr, we can't do anything but fail the call.

        if(pNextEmpty == NULL)
            return 0;

//      pNextEmpty->pdsurf        = we don't have this information
        pNextEmpty->next          = pBlk->next;
        pNextEmpty->prev          = pBlk;
        pNextEmpty->nextEmpty     = pBlk->nextEmpty;
        pNextEmpty->pHeap         = pHeap;
        pNextEmpty->ulSizeInBytes = ulBytesAvail - ulBytesToAlloc;
        pNextEmpty->ulOffset      = pBlk->ulOffset + ulBytesToAlloc;
//      pNextEmpty->ulLinearStride= we don't have this information

        SET_EMPTY(pNextEmpty);

        //**********************************************************************
        // Now patch its neighbors
        //**********************************************************************

        pBlk->next = pNextEmpty;
        pNextEmpty->next->prev = pNextEmpty;
    }

    pPrevEmpty->nextEmpty = pNextEmpty;

    //**************************************************************************
    // Mark as BUSY (BUSY essentially means USED in this heap manager)
    //**************************************************************************

    pBlk->ulSizeInBytes = ulBytesToAlloc;

    SET_BUSY(pBlk);

    //**************************************************************************
    //
    // pHeap->oh.next  (UPDATED HEAP)
    //        |                                                           -------
    //        |  ------- nextEmpty --------------------   --- nextEmpty--| EMPTY |
    //        | |                       pBlk           | |                -------
    //        v |                                      v |
    //       1st Blk      2nd Blk        NEW           NEW         Last Blk
    //       ----          ----         ----          ----          ----
    //      |    |  --->  |    |  ---> |    |  --->  |    |  --->  |    | ---
    //      |    |  <---  |    |  <--- |    |  <---  |    |  <---  |    |    |
    //       ----          ----         ----          ----          ----     |
    //         ^                                                             |
    //         |                                                             |
    //          -------------------------------------------------------------
    //
    //                                  NEW           NEW
    //      EMPTY          BUSY         BUSY         EMPTY          BUSY
    //
    //     No Space       Contains    Contains      Contains        Contains
    //     Available         no       current         all             no
    //                      data      requested     of video         data
    //                                 video         memory
    //                                 memory
    //
    //                  |<---------------------->|
    //                   This is where new nodes
    //                          get added
    //**************************************************************************

    return pBlk;
}

//----------------------------------------------------------------------------
//
//  Function:   heap_free
//
//  Routine Description:
//
//      Frees up a block of memory and cleans up the heap
//
//  Arguments:
//
//  Return Value:
//      0 - heap is not empty
//      1 - heap is empty
//
//----------------------------------------------------------------------------
BOOL heap_free
(
    OH*     pBlk
)
{
    HEAP*   pHeap = pBlk->pHeap;

    OH*     pChk;
    OH*     pNext;
    OH*     pNextEmpty;
    OH*     freeCurrent;
    OH*     freeAhead;

    //**************************************************************************
    // Check and see if we can coalesce memory blocks
    //
    //
    //              Previous Block    Block to     Next Block
    //                                be freed
    //
    //                   |              |               |
    //                   |              |               |
    //                   v              v               v
    //
    //               EMPTY/BUSY?       BUSY         EMPTY/BUSY?
    //
    //**************************************************************************

    pNext = pBlk->next;
    if (IS_EMPTY(pNext))
    {
        //**********************************************************************
        // Coalesce forward , next block is EMPTY
        //**********************************************************************

        freeAhead   = pNext;
        pNextEmpty  = pNext->nextEmpty;
        pNext       = pNext->next;

        //**********************************************************************
        // Current State of Variables:
        //
        //                            Block to     Next Block
        //                            be freed
        //
        //                            (pBlk)
        //                              |               |
        //                              |               |  ----- pNextEmpty -->
        //                              v               v |
        //                                                |
        //                             BUSY  ------>  EMPTY ---- pNext ------->
        //                                  freeAhead
        //
        //                          |<---------------------->|
        //                             These two blocks will
        //                             be merged below
        //
        //          pBlk        = current block
        //          freeAhead   = points to the block that will be deleted
        //          pNextEmpty  = points to the new NextEmpty block
        //          pNext       = points to the new Next block
        //**********************************************************************

    }
    else
    {
        //**********************************************************************
        // Coalesce forward , next block is BUSY
        //**********************************************************************

        pNextEmpty = 0 ;
        freeAhead = (OH *)0;

        //**********************************************************************
        // Current State of Variables:
        //
        //                            Block to     Next Block
        //                            be freed
        //
        //                            (pBlk)
        //                              |               |
        //                              |               |
        //                              v               v       pNextEmpty = 0
        //                                                      freeAhead  = 0
        //                             BUSY  ------>  BUSY
        //                                    pNext
        //
        //
        //                            The above two blocks
        //                            will NOT be merged
        //
        //          pBlk        = current block
        //          freeAhead   = 0 (means we will NOT delete/merge a block)
        //          pNextEmpty  = 0 (means we will need to obtain the pNextEmpty
        //                           block from the PREVIOUS Empty block)
        //          pNext       = unchanged, already points to the next block
        //**********************************************************************

    }

    //**************************************************************************
    // Now check and see if we can coalesce previous memory blocks
    //**************************************************************************

    if (IS_EMPTY(pBlk->prev))
    {
        //**********************************************************************
        // Coalesce backward, previous block is EMPTY
        //**********************************************************************

        freeCurrent     = pBlk;
        pBlk            = pBlk->prev;
        pNext->prev     = pBlk;
        if (pNextEmpty)
            pBlk->nextEmpty = pNextEmpty;

        //**********************************************************************
        // Current State of Variables:
        //
        //          Previous Block    Block to
        //                            be freed
        //
        //               |              |
        //               |              |
        //               v              v       --> pNextEmpty (already set up above)
        //                                      --> pNext (already set up above)
        //             EMPTY <------  BUSY
        //
        //               ^ |            ^
        //               | |            |
        //             pBlk|        freeCurrent
        //                 |
        //                  ------------------------> pNextEmpty
        //
        //          |<------------------------>|
        //              The above two blocks
        //                WILL be merged
        //
        //          pBlk            = updated to the new 'current' block
        //
        //          freeCurrent     = points to block that will be deleted
        //          pNext->prev     = updated to be linked to its new previous block
        //          pBlk->nextEmpty = updated to point to next empty block
        //                            (unless the pNextEmpty block is to be obtained
        //                             from the previous empty block, then
        //                             this field is left UNMODIFIED, because
        //                             it ALREADY points to the next empty block)
        //          pBlk->next      = is initialized after this if statement because
        //                            it doesn't depend on this if statement
        //**********************************************************************

    }
    else
    {
        //**********************************************************************
        // Coalesce backward, previous block is BUSY
        //**********************************************************************

        SET_EMPTY( pBlk ) ;
        freeCurrent = (OH *)0;

        //**********************************************************************
        // Search this heap for the previous empty block
        //**********************************************************************

        for (pChk = pHeap->oh.next;
                (pChk->nextEmpty != (OH *)EMPTY) && (pChk->nextEmpty->ulOffset < pBlk->ulOffset);
                pChk = pChk->nextEmpty)
        {
            ;
        }

        //**********************************************************************
        // If we already have a pNextEmpty block, then we know that we just need to
        // update the pNext->prev block to point back to this current block, because
        // it pNext->prev currently points to the block which will be deleted.
        //
        // However, if pNextEmpty == 0, then pNext->prev is already setup correctly.
        //
        // The following if statement would be easier to understand in the following
        // format:
        //
        //  if (pNextEmpty)
        //      pNext->prev = pBlk;
        //
        //  if (!pNextEmpty)
        //      pNextEmpty = pChk->nextEmpty
        //
        //**********************************************************************

        if (pNextEmpty)
            pNext->prev = pBlk;
        else
            pNextEmpty = pChk->nextEmpty ;

        //**********************************************************************
        // pNextEmpty has now been correctly determined.
        // Insert the block in the EMPTY list and fixup the pointers.
        //**********************************************************************

        pBlk->nextEmpty = pNextEmpty ;
        pChk->nextEmpty = pBlk ;

        //**********************************************************************
        // Current State of Variables:
        //
        //              Previous Block    Block to
        //                                be freed
        //
        //                   |              |
        //                   |              |
        //                   v              v     --> pNextEmpty (already set up above)
        //                                        --> pNext (already set up above)
        //                 BUSY           BUSY
        //                                ^ ^
        //      Previous                  | |
        //       Empty <------------------   -- pBlk
        //       Block
        //
        //
        //       pBlk            = current block
        //       freeCurrent     = 0 (means that we will NOT delete a block)
        //       pNext->prev     = updated to be linked to its new previous block
        //                         (if necessary)
        //       pBlk->nextEmpty = updated to point to the next empty block.
        //       pChk->nextEmpty = previous next empty block is also correctly updated
        //       pBlk->next      = is initialized after this if statement because
        //                         it doesn't depend on this if statement
        //**********************************************************************
    }


    //**************************************************************************
    // freeAhead is NONZERO if we merged 2 empty blocks into 1
    // freeCurrent is also NONZERO if we merged 2 empty blocks into 1
    //**************************************************************************

    if (freeCurrent)
        EngFreeMem(freeCurrent);

    if (freeAhead)
        EngFreeMem(freeAhead);

    //**************************************************************************
    // Update the next block link.
    // Also, update the size of the current block, due to freeing up one block.
    //**************************************************************************

    pBlk->next = pNext;
    pBlk->ulSizeInBytes = pNext->ulOffset - pBlk->ulOffset;

    return pBlk->ulSizeInBytes == pHeap->ulSizeInBytes;
}

//----------------------------------------------------------------------------
//
//  Function:   heap_init
//
//  Routine Description:
//
//      Initializes the linear heap manager for OpenGL
//
//  Arguments:
//
//  Return Value:
//      0 - failed
//      1 - success
//
//----------------------------------------------------------------------------
BOOL heap_init
(
    ULONG   memBegin,
    ULONG   memSize,
    HEAP*   pHeap
)
{
    OH*     pFirstBlk;
    OH*     pSecondBlk;
    OH*     pThirdBlk;
    OH*     pLastBlk;
    OH*     pNextBlk;
    OH*     pBlk;

    //**************************************************************************
    // We initialize the memory system, by setting up three special
    // blocks and a fourth block containing all of free memory.
    // The first block is "EMPTY", but has no "spaceAvailable".
    // The second block is "BUSY", but uses no space.
    // The last block is "BUSY" and also contains no memory.
    // Finally, the third block contains the rest of memory!
    //
    // pHeap->oh.next
    //        |                                               -----------
    //        |  ------- nextEmpty --------   --- nextEmpty--| 0 + EMPTY |
    //        | |                          | |                -----------
    //        v |                          v |
    //       1st Blk      2nd Blk       3rd Blk       Last Blk
    //       ----          ----          ----          ----
    //      |    |  --->  |    |  --->  |    |  --->  |    | ---
    //      |    |  <---  |    |  <---  |    |  <---  |    |    |
    //       ----          ----          ----          ----     |
    //         ^                                                |
    //         |                                                |
    //          ------------------------------------------------
    //
    //      EMPTY          BUSY          EMPTY         BUSY
    //
    //     No Space       Contains      Contains       Contains
    //     Available         no           all            no
    //                      data        of video        data
    //                                   memory
    //
    //**************************************************************************

    if ((pFirstBlk = EngAllocMem(FL_ZERO_MEMORY, 4*sizeof(OH), ALLOC_TAG)) == NULL)
    {
        pHeap->oh.next = NULL;
        return 0;
    }

    pHeap->oh.next = pFirstBlk;
    pSecondBlk = pFirstBlk + 1;
    pThirdBlk = pSecondBlk + 1;
    pLastBlk = pThirdBlk + 1;

    //**************************************************************************
    // FirstBlk is "EMPTY" but has no space available
    //**************************************************************************

    pFirstBlk->prev          = pLastBlk;
    pFirstBlk->next          = pSecondBlk;
    pFirstBlk->nextEmpty     = pThirdBlk;
    pFirstBlk->pHeap         = pHeap;
    pFirstBlk->ulSizeInBytes = 0;
    pFirstBlk->ulOffset      = memBegin;
    SET_EMPTY(pFirstBlk);

    //**************************************************************************
    // Second is "BUSY", but contains no data
    //**************************************************************************

    pSecondBlk->prev          = pFirstBlk;
    pSecondBlk->next          = pThirdBlk;
    pSecondBlk->nextEmpty     = NULL;
    pSecondBlk->pHeap         = pHeap;
    pSecondBlk->ulSizeInBytes = 0;
    pSecondBlk->ulOffset      = memBegin;
    SET_BUSY(pSecondBlk);

    //**************************************************************************
    // ThirdBlk is "EMPTY" and contains all of memory!
    //**************************************************************************

    pThirdBlk->prev          = pSecondBlk;
    pThirdBlk->next          = pLastBlk ;
    pThirdBlk->nextEmpty     = (OH *)EMPTY;
    pThirdBlk->pHeap         = pHeap;
    pThirdBlk->ulSizeInBytes = memSize;
    pThirdBlk->ulOffset      = memBegin;
    SET_EMPTY(pThirdBlk);

    //**************************************************************************
    // LastBlk is "BUSY", but contains no data
    // LastBlk starts 1 past end of available memory
    //**************************************************************************

    pLastBlk->prev          = pThirdBlk;
    pLastBlk->next          = pFirstBlk;
    pLastBlk->nextEmpty     = NULL;
    pLastBlk->pHeap         = pHeap;
    pLastBlk->ulSizeInBytes = 0;
    pLastBlk->ulOffset      = memBegin + memSize;
    SET_BUSY(pLastBlk);

    return 1;
}


//----------------------------------------------------------------------------
//
//  Function:   heap_clear
//
//  Routine Description:
//
//      Frees up the 4 base blocks used to manage a a heap
//
//  Arguments:
//
//  Return Value:
//
//----------------------------------------------------------------------------
void heap_clear
(
    HEAP*   pHeap
)
{
    /*
    ** Note that this is NOT an assert case.  If GetHeap successfully allocates
    ** a new heap but heap_init() fails (presumeably because the allocation of
    ** the oh struct space failed) then GetHeap will call FreeHeap which calls
    ** heap_clear() and pHeap->oh.next will definitely be NULL in that case.
    */
    if (pHeap && pHeap->oh.next)
    {
        EngFreeMem(pHeap->oh.next);
        pHeap->oh.next = NULL;
    }
}
#endif // !USE_RM_MMGR ]

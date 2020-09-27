//**************************** Module Header ***********************************
//
// Module Name: bitblt.c
//
// Contains the high-level DrvBitBlt and DrvCopyBits functions.  The low-
// level stuff lives in the 'blt??.c' files.
//
// Note: Since we've implemented device-bitmaps, any surface that GDI passes
//       to us can have 3 values for its 'iType': STYPE_BITMAP, STYPE_DEVICE
//       or STYPE_DEVBITMAP.  We filter device-bitmaps that we've stored
//       as DIBs fairly high in the code, so after we adjust its 'pptlSrc',
//       we can treat STYPE_DEVBITMAP surfaces the same as STYPE_DEVICE
//       surfaces (e.g., a blt from an off-screen device bitmap to the screen
//       gets treated as a normal screen-to-screen blt).
//
//       Unfortunately, if we've created our primary surface as a device-
//       managed surface, it has an 'iType' of STYPE_BITMAP and not
//       STYPE_DEVICE.  So throughout this code, we will determine if a
//       surface is one of ours by checking 'dhsurf' -- a NULL value means
//       that it's a GDI-created DIB, otherwise it's one of our surfaces and
//       'dhsurf' points to our DSURF structure.
//
// Copyright (c) 1992-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"
#include "nvapi.h"
#include "oglDD.h"
#include "oglOverlay.h"
#include "oglsync.h"

VOID CopyDIBBitmaps (DSURF*, BYTE*, BYTE*, ULONG, ULONG); // forward declaration

typedef enum {
    NONE=0,
    COPYBLIT,
    IMAGEFROMCPU
} ALPHA_SAVE;


extern VOID NV4DmaPushAlphaPreserve(PPDEV, BOOL, BOOL);
#if _WIN32_WINNT >= 0x0500
extern VOID NV4DmaPushSetColorKey(PPDEV, ULONG, ULONG);
extern BOOL NV4CheckAlphaBlendOK(PPDEV);
extern BOOL NV4DmaPushCopyAlphaBlt(PPDEV, LONG, RECTL*, ULONG, POINTL*, RECTL*, BLENDOBJ*);
#endif _WIN32_WINNT >= 0x0500
    
//******************************************************************************
//
//  Function:   PrepareSurfacesForPunt
//
//  Routine Description:
//
//      GDI only knows how to write to standard DIB surfaces.
//      That means, if the SOURCE or DEST is a DFB, we need to create a standard
//      DIB SurfObj in place of the device managed surface (DFB) object in order
//      to let GDI do the blit.
//
//      Essentially, there are 4 types of SURFOBJS we'll encounter.
//
//              1) System Memory SurfObj (Standard DIB)
//                  We can pass this surfobj to GDI as is.
//
//              2) DFB - exists in offscreen VRAM memory
//                  Need to create a standard DIB SurfObj to let GDI draw to this surface
//
//              3) DFB - exists in system memory
//                  Need to create a standard DIB surfObj to let GDI draw to this surface
//
//              4) Primary Surface VRAM (Standard DIB format)
//                  We can pass this surfobj to GDI as is.
//
//              CONDITION                                           SURFOBJ TYPE
//              ---------                                           ------------
//
//      (pdsurf != NULL)  &&  (pdsurf->dt == DT_DIB)         --> This is a DFB in system memory
//      (pdsurf != NULL)  &&  (pdsurf->dt == DT_SCREEN)      --> This is a DFB in offscreen VRAM memory
//
//      (pdsurf == NULL)  &&  (pvBits != ppdev->pjScreen)    --> Standard DIB
//      (pdsurf == NULL)  &&  (pvBits == ppdev->pjScreen)    --> Primary Screen Surface
//
//  Arguments:
//
//      Src, Dst SURFOBJ pointers
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID PrepareSurfacesForPunt(
    SURFOBJ*    psoSrc,
    SURFOBJ*    psoDst,
    SURFOBJ**   pTempSrcSurfObjPtr,
    SURFOBJ**   pTempDstSurfObjPtr,
    PDEV*       ppdev
)
{
    DSURF*          pdsurfDst;
    DSURF*          pdsurfSrc;
    SURFOBJ*        SrcPuntPtr;
    SURFOBJ*        DstPuntPtr;

    //**************************************************************************
    // Default to Standard DIB, and get handles to surfaces if they exist
    //**************************************************************************

    (VOID *)pdsurfSrc = NULL;
    (VOID *)pdsurfDst = NULL;

    if ((VOID *)psoSrc != NULL)
        pdsurfSrc = (DSURF*) psoSrc->dhsurf;

    if ((VOID *)psoDst != NULL)
        pdsurfDst = (DSURF*) psoDst->dhsurf;

    //**************************************************************************
    // Default to using the original, un-modified SURFOBJ's that were passed to us.
    //**************************************************************************

    *pTempSrcSurfObjPtr = psoSrc;
    *pTempDstSurfObjPtr = psoDst;

    //**************************************************************************
    // Get pointers to the 'wrapper' surfaces that we use to draw to the
    // screen surface if necessary.  These surfaces are locked.
    // (The surfaces that we pass to GDI MUST be locked).
    // See EnableOffscreenHeap for more information.
    //**************************************************************************

    SrcPuntPtr = ppdev->psoPunt;
    DstPuntPtr = ppdev->psoPunt2;

    //**************************************************************************
    // Check if source bitmap is a DFB.  If so, create a temporary surfobj
    // (mapped to the screen) that GDI can read from
    //**************************************************************************

    if (pdsurfSrc !=0 )
        {
        SrcPuntPtr->dhsurf      = 0;                    // Behave like a standard DIB
        if (pdsurfSrc->dt == DT_SCREEN)
            {
            SrcPuntPtr->sizlBitmap  = pdsurfSrc->sizl;
            SrcPuntPtr->cjBits  = pdsurfSrc->LinearStride * pdsurfSrc->sizl.cy;
            SrcPuntPtr->pvBits  = pdsurfSrc->LinearPtr;
            SrcPuntPtr->pvScan0 = pdsurfSrc->LinearPtr;
            SrcPuntPtr->lDelta  = pdsurfSrc->LinearStride;
            // adi we cannot use the ppdev->iBitmapFormat because we may have to do
            // with a surface wrapped around a DD surface by DrvDeriveSurface
            SrcPuntPtr->iBitmapFormat = psoSrc->iBitmapFormat;
            }
        else
            {
            //**********************************************************************
            // This is a DFB which is stored in main memory as a DIB
            //**********************************************************************
            SrcPuntPtr->sizlBitmap  = pdsurfSrc->pso->sizlBitmap;
            SrcPuntPtr->cjBits      = pdsurfSrc->pso->lDelta * pdsurfSrc->pso->sizlBitmap.cy;
            SrcPuntPtr->pvBits  = pdsurfSrc->pso->pvBits;
            SrcPuntPtr->pvScan0 = pdsurfSrc->pso->pvScan0;
            SrcPuntPtr->lDelta  = pdsurfSrc->pso->lDelta;
            SrcPuntPtr->iBitmapFormat = pdsurfSrc->pso->iBitmapFormat;
            }

        //**********************************************************************
        // Use the 'wrapper' surface, instead of the original DFB surfobj for GDI
        //**********************************************************************

        *pTempSrcSurfObjPtr       = SrcPuntPtr;
        }


    //**************************************************************************
    // Check if destination bitmap is a DFB.  If so, create a temporary surfobj
    // (mapped to the screen) that GDI can write on
    //**************************************************************************

    if (pdsurfDst !=0 )
        {
        DstPuntPtr->dhsurf      = 0;                    // Behave like a standard DIB
        if (pdsurfDst->dt == DT_SCREEN)
            {
            DstPuntPtr->sizlBitmap  = pdsurfDst->sizl;
            DstPuntPtr->cjBits  = pdsurfDst->LinearStride * pdsurfDst->sizl.cy;
            DstPuntPtr->pvBits  = pdsurfDst->LinearPtr;
            DstPuntPtr->pvScan0 = pdsurfDst->LinearPtr;
            DstPuntPtr->lDelta  = pdsurfDst->LinearStride;
            // adi we cannot use the ppdev->iBitmapFormat because we may have to do
            // with a surface wrapped around a DD surface by DrvDeriveSurface
            DstPuntPtr->iBitmapFormat = psoDst->iBitmapFormat;
            }
        else
            {
            //**********************************************************************
            // This is a DFB which is stored in main memory as a DIB
            //**********************************************************************
            DstPuntPtr->sizlBitmap  = pdsurfDst->pso->sizlBitmap;
            DstPuntPtr->cjBits      = pdsurfDst->pso->lDelta * pdsurfDst->pso->sizlBitmap.cy;
            DstPuntPtr->pvBits  = pdsurfDst->pso->pvBits;
            DstPuntPtr->pvScan0 = pdsurfDst->pso->pvScan0;
            DstPuntPtr->lDelta  = pdsurfDst->pso->lDelta;
            DstPuntPtr->iBitmapFormat = pdsurfDst->pso->iBitmapFormat;
            }

        //**********************************************************************
        // Use the 'wrapper' surface, instead of the original DFB surfobj for GDI
        //**********************************************************************

        *pTempDstSurfObjPtr       = DstPuntPtr;
        }
}


//******************************************************************************
//
//  Macro:   TRY_MOVE_SRC_TO_OFFSCREEN_MEM
//
//  Macro Description:
//
//      check and see if we can move system memory DFB's back into                  
//      offscreen VRAM for better performance. That is, at some point, we put a DFB 
//      in system memory. Now we determine that if this DFB is getting              
//      used a lot, put it back in offscreen VRAM so we can regain performance.     
//
//*****************************************************************************
#define TRY_MOVE_SRC_TO_OFFSCREEN_MEM(psoSrc, pdsurfSrc, pxlo, ppdev)                           \
{                                                                                               \
    if ( (pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_DIB) )                                     \
        {                                                                                       \
        /***********************************************************************/               \
        /* Here we consider putting a DIB DFB back into off-screen             */               \
        /* memory.  If there's a translate, it's probably not worth            */               \
        /* moving it since we won't be able to use the hardware to do          */               \
        /* the blt (a similar argument could be made for weird rops            */               \
        /* and stuff that we'll only end up having GDI simulate, but           */               \
        /* those should happen infrequently enough that I don't care).         */               \
        /***********************************************************************/               \
        if ((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL))                                     \
            {                                                                                   \
            ASSERTDD(pdsurfSrc->pso->iBitmapFormat == ppdev->iBitmapFormat, "oops");            \
                                                                                                \
            if (pdsurfSrc->iUniq == ppdev->iHeapUniq)                                           \
                {                                                                               \
                /*****************************************************************/             \
                /* Attempt to move DIB back to offscreen VRAM.                   */             \
                /* This won't necessarily succeed.                               */             \
                /*                                                               */             \
                /* NOTE: If space doesn't get freed up for a while (heapUniq     */             \
                /*       remains the same), and this call fails, then cBlt will  */             \
                /*       wrap around to 0xffffffff, meaning we won't consider    */             \
                /*       putting this DFB DIB back into offscreen VRAM for a     */             \
                /*       LONG time, until someone frees up some offscreen memory,*/             \
                /*       and causes cBlt to be reset to HEAP_COUNT_DOWN.         */             \
                /*****************************************************************/             \
                if (--pdsurfSrc->cBlt == 0)                                                     \
                    if (bMoveDibToOffscreenDfbIfRoom(ppdev, pdsurfSrc))                         \
                        {                                                                       \
                            /* Now in offscreen, ptrs OK as is so return */                     \
                            goto MoveComplete;                                                  \
                        }                                                                       \
                }                                                                               \
                else                                                                            \
                {                                                                               \
                /***************************************************************/               \
                /* Some space was freed up in off-screen memory,               */               \
                /* so reset the counter for this DFB:                          */               \
                /***************************************************************/               \
                pdsurfSrc->iUniq = ppdev->iHeapUniq;                                            \
                pdsurfSrc->cBlt  = HEAP_COUNT_DOWN;                                             \
                }                                                                               \
            }                                                                                   \
                                                                                                \
        psoSrc = pdsurfSrc->pso;                                                                \
        pdsurfSrc = (DSURF *) psoSrc->dhsurf;                                                   \
                                                                                                \
        }                                                                                       \
MoveComplete: ;                                                                                 \
}


//******************************************************************************
//
//  Function:   GenericBitBltAlpha
//
//  Routine Description:
//
//      Implements the workhorse routine of a display driver.
//      Called by DrvBitBlt and DrvAlphaBlend to do bit block transfers
//      Called by DrvCopyBits in some cases.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL GenericBitBltAlpha(
    SURFOBJ*    psoDst,
    SURFOBJ*    psoSrc,
    SURFOBJ*    psoMsk,
    CLIPOBJ*    pco,
    XLATEOBJ*   pxlo,
    RECTL*      prclDst,
    RECTL*      prclSrc,
    POINTL*     pptlSrc,
    POINTL*     pptlMsk,
    BRUSHOBJ*   pbo,
    POINTL*     pptlBrush,
    ROP4        rop4,
    BLENDOBJ*   pBlendObj,
    PDEV*       ppdev
)

    {
    DSURF*          pdsurfDst;
    DSURF*          pdsurfSrc;
    POINTL          ptlSrc;
    BYTE            jClip;
    BOOL            bMore;
    CLIPENUM        ce;
    LONG            c;
    RECTL           rcl;
    BYTE            rop3;
    FNFILL*         pfnFill;
    RBRUSH_COLOR    rbc;        // Realized brush or solid colour
    FNXFER*         pfnXfer;
    FNSCRNTOMEM*    pfnStoM;
    ULONG           iSrcBitmapFormat;
    ULONG           iDstBitmapFormat;
    ULONG           iDir;
    BOOL            bRet;
    ULONG*          CheckSurfPtr;
    ULONG*          CheckPdevPtr;
    ULONG*          ScreenBitmap;
    ULONG*          DstBits;
    ULONG*          SrcBits;
    ULONG           PatternColor;

    SURFOBJ*        SrcPuntPtr;
    SURFOBJ*        DstPuntPtr;
    SURFOBJ*        TempDstSurfObjPtr;
    SURFOBJ*        TempSrcSurfObjPtr;

    ULONG           SrcOffset;
    ULONG           DstOffset;
    ULONG           SrcStride;
    ULONG           DstStride;

    ALPHA_SAVE      alphaPreserve=NONE;

    //**************************************************************************
    // Assume successful blit
    //**************************************************************************

    bRet = TRUE;

    // Alpha only supported on Win2k
#if _WIN32_WINNT >= 0x0500

    if (pBlendObj)
        {
        // Trivially reject AlphaBlends we can't do
        if ((pxlo && pxlo->flXlate && !(pxlo->flXlate & XO_TRIVIAL) ) ||
            ((ppdev->iBitmapFormat != BMF_32BPP) && (ppdev->iBitmapFormat != BMF_16BPP)))
            {
            // Can't do translations with blends
            // Can't hw accelerate anything except 16,32 bpp desktop
            goto EngFallBack;
            }
        }    
    //**************************************************************************
    //  Fixes case where driver is called in full screen DOS during STRESS tests.
    //  In this case, GDI is allowed to call us to update DFBs that are stored in
    //  system memory, or offscreen memory. We need to wrap the surface and call
    //  EngBitBlt.
    //**************************************************************************

    if(!ppdev->bEnabled)
        goto EngFallBack;

#endif

    //**************************************************************************
    // Trivially reject calls we don't handle. We don't blt between host memory bitmaps.
    //**************************************************************************

    if (!IS_DEV_SURF(psoDst))
        {
        if (psoSrc == NULL)
            goto EngFallBack;
        else if (!IS_DEV_SURF(psoSrc))
            goto EngFallBack;
        }

    //**************************************************************************
    // This is a good place to examine offscreen memory visually for debugging.
    // purposes.  That's because there are difficulties when trying to output
    // DWORD values in the WinDbg debugger to NV hardware registers.
    // Apparently, when using WinDbg, it doesn't output dwords as dwords.
    // It seems to output them as 4 bytes. This can cause problems
    // with NV hardware since NV architecture expects all operations to be done
    // as dwords. As a result, in order to flip the buffer, we need to do it in
    // real 32-bit code, instead of manually in the debugger.
    //
    //
    //**************************************************************************

    //**************************************************************************
    // GDI will never give us a Rop4 with the bits in the high-word set
    // (so that we can check if it's actually a Rop3 via the expression
    // (rop4 >> 8) == (rop4 & 0xff)):
    //**************************************************************************

    ASSERTDD((rop4 >> 16) == 0, "Didn't expect a rop4 with high bits set");

    //**************************************************************************
    // For NV, our primary screen surface is a GDI-Managed bitmap, instead of a
    // DEVICE MANAGED surface.  This allows us to punt any call back to GDI
    // if necessary.  Therefore, pdsurfDst will be NULL when the destination
    // is the SCREEN.  pdsurfDst will be Non-NULL when the destination is a DFB.
    // As a result, we determine if the SURFOBJ is the primary surface by taking
    // a look at the 'pvBits' value.  If pvBits == ppdev->pjScreen, then the
    // surfobj is the primary surface.
    //**************************************************************************

    pdsurfDst = (DSURF*) psoDst->dhsurf;    // May be NULL since we created the
                                            // primary surface as a GDI Managed bitmap

    //**************************************************************************
    // Check to see if the dest is a device bitmap which has been moved to main
    // memory
    //**************************************************************************
    if ((pdsurfDst != NULL) && (pdsurfDst->dt == DT_DIB))
        {
        psoDst = pdsurfDst->pso;
        pdsurfDst = (DSURF *) psoDst->dhsurf;
        }

    //**************************************************************************
    // Check if this blit only involves a destination
    //**************************************************************************

    if (psoSrc == NULL)
        {
        //**********************************************************************
        // Since there is no source bitmap, the destination can be either a
        // DIB , DFB (DT_SCREEN), DFB(DT_DIB) or SCREEN surface.  Since we don't
        // handle blits to DIBs or system memory, we'll only handle blits
        // to the primary screen surface (SCREEN), or blits to DFB (DT_SCREEN).
        //
        //  1) DESTINATION = SCREEN
        //
        //        (pdsurfDst    == 0)               &&
        //        (DstBits      == ScreenBitmap)
        //
        //  2) DESTINATION = DFB (DT_SCREEN)
        //
        //        (pdsurfDst     != 0)              &&
        //        (pdsurfDst->dt == DT_SCREEN)      &&
        //        (DstBits       != ScreenBitmap)
        //
        //**********************************************************************

        //**********************************************************************
        // AlphaBlends need a source so assert if we don't have one. -paul
        //**********************************************************************
        ASSERTDD(!pBlendObj ,"AlphaBlend with no source bitmap!");

        //**********************************************************************
        // Fills are this function's "raison d'etre", so we handle them
        // as quickly as possible:
        //**********************************************************************

        //**********************************************************************
        // There will always be a destination so we don't have to check
        // for psoDst == NULL.
        //**********************************************************************

        DstBits         = (ULONG *)(psoDst->pvBits);
        ScreenBitmap    = (ULONG *)(ppdev->pjScreen);

        //**********************************************************************
        // Only handle blits to VRAM.  Send blits to system memory back to GDI.
        //**********************************************************************

        if (  ((pdsurfDst == 0) && (DstBits == ScreenBitmap))   ||
              ((pdsurfDst != 0) && (pdsurfDst->dt == DT_SCREEN))  )
            {
            //******************************************************************
            // Make sure it doesn't involve a mask (i.e., it's really a
            // Rop3):
            //******************************************************************

            rop3 = (BYTE) rop4;

            if ((BYTE) (rop4 >> 8) == rop3)

                {
                //**************************************************************
                // Since 'psoSrc' is NULL, the rop3 had better not indicate
                // that we need a source.
                //
                // Truth table for a Rop 3.           R
                //                                    O
                //                        (SD)        P
                //                  11  10  01  00    3
                //                  --------------
                //          (P) 1  | x   x   x   x    High nibble
                //              0  | x   x   x   x    Low nibble
                //
                //**************************************************************

                ASSERTDD((((rop4 >> 2) ^ (rop4)) & 0x33) == 0,
                             "Need source but GDI gave us a NULL 'psoSrc'");

                //**************************************************************
                // Default to solid fill
                //**************************************************************

                pfnFill = ppdev->pfnFillSolid;

                //**************************************************************
                // This will be a SOLID fill (No Source, No Pattern
                // unless the ROP says a pattern is required).
                // So check if Pattern is needed.  See truth table up above.
                //**************************************************************

                if(pbo == NULL)  // Black and White Rops do not need color
                    rbc.iSolidColor = (ULONG)-1;
                else
                    rbc.iSolidColor = pbo->iSolidColor;

                if ((((rop3 >> 4) ^ (rop3)) & 0xf) != 0)

                    {
                    //**********************************************************
                    // The rop says that a pattern is truly required
                    // (blackness, for instance, doesn't need one):
                    //**********************************************************

                    if (rbc.iSolidColor == -1)

                        {
                        //******************************************************
                        // Try and realize the pattern brush; by doing
                        // this call-back, GDI will eventually call us
                        // again through DrvRealizeBrush:
                        //******************************************************

                        rbc.prb = pbo->pvRbrush;
                        if (rbc.prb == NULL)

                            {
                            rbc.prb = BRUSHOBJ_pvGetRbrush(pbo);
                            if (rbc.prb == NULL)

                                {
                                //**********************************************
                                // If we couldn't realize the brush, let GDI handle
                                // the call (it may have been a non 8x8
                                // brush or something, which we can't be
                                // bothered to handle, so let GDI do the
                                // drawing):
                                //**********************************************

                                goto EngFallBack;
                                }
                            }

                        //******************************************************
                        // We've now determined that this a Pattern blit.
                        // Pattern will come from the Pattern brush.
                        //******************************************************

                        pfnFill = ppdev->pfnFillPat;

                        }

                    }


                //**************************************************************
                // Linear memory management
                // Set linear offset and stride for destination buffer
                //**************************************************************

                if (pdsurfDst != 0)
                    {
                    DstOffset = (ULONG)((BYTE *)(pdsurfDst->LinearPtr) - ppdev->pjFrameBufbase);
                    DstStride = pdsurfDst->LinearStride;
                    }

                else

                    {
                    //**********************************************************
                    // Use pitch settings of visible screen
                    //**********************************************************

                    DstOffset = ppdev->ulPrimarySurfaceOffset;
                    DstStride = ppdev->lDelta;
                    }

                ppdev->pfnSetDestBase(ppdev, DstOffset , DstStride);


                //**************************************************************
                // Solid and Pattern fills
                //**************************************************************

                //**************************************************************
                // Note that these 2 'if's are more efficient than
                // a switch statement:
                //**************************************************************

                if ((pco == NULL) || (pco->iDComplexity == DC_TRIVIAL))

                    {
                    //**********************************************************
                    // No clipping required
                    //**********************************************************
                    pfnFill(ppdev, 1, prclDst, rop4, rbc, pptlBrush);
                    goto All_Done;
                    }

                else if (pco->iDComplexity == DC_RECT)

                    {
                    //**********************************************************
                    // Clip to a single rectangle
                    //**********************************************************
                    if (bIntersect(prclDst, &pco->rclBounds, &rcl))
                        {
                        pfnFill(ppdev, 1, &rcl, rop4, rbc, pptlBrush);
                        }

                    goto All_Done;
                    }

                else

                    {
                    //**********************************************************
                    // Clip region must be enumerated
                    // For NV, do we need to worry about direction???
                    //**********************************************************
                    CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

                    do  {
                        bMore = CLIPOBJ_bEnum(pco, sizeof(ce), (ULONG*) &ce);

                        c = cIntersect(prclDst, ce.arcl, ce.c);

                        if (c != 0)
                            {
                            pfnFill(ppdev, c, ce.arcl, rop4, rbc, pptlBrush);
                            }

                        } while (bMore);
                    goto All_Done;
                    }

                } // if (BYTE) Rop4 >> 8) ...
                else
                    //*****************************************************
                    // This is a true Rop4, pass this call to GDI.
                    //*****************************************************
                    goto EngFallBack;

            }  // if (destination == VRAM)

        else

            {

            //******************************************************************
            // At this point, we know that there was no source, and that the
            // destination was a standard DIB or a DFB that was moved from
            // offscreen VRAM to system memory.  We won't handle these so just pass
            // them on to EngBitBlt
            //
            // We will also get to this point if the blit involved a MASK.
            //******************************************************************

            goto EngFallBack;
            }

        } // if (psoSrc == NULL)...

    //**************************************************************************
    // Before we handle blits which involve a source and destination,
    // check and see if we can move system memory DFB's back into
    // offscreen VRAM for better performance. That is, at some point, we put a DFB
    // in system memory. Now we determine that if this DFB is getting
    // used a lot, put it back in offscreen VRAM so we can regain performance.
    //
    // We know that at this point, psoSrc and psoDst will not be null.
    //**************************************************************************

    //**************************************************************************
    // Here we know psoSrc != NULL and psoDst should NOT be NULL !
    //**************************************************************************

    ASSERTDD(((VOID *)psoSrc != NULL), "Didn't expect a NULL source");
    ASSERTDD(((VOID *)psoDst != NULL), "Didn't expect a NULL destination");

    pdsurfDst = (DSURF*) psoDst->dhsurf;
    pdsurfSrc = (DSURF*) psoSrc->dhsurf;

    if ( (pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_DIB) )

        {
        //**********************************************************************
        // Here we consider putting a DIB DFB back into off-screen
        // memory.  If there's a translate, it's probably not worth
        // moving it since we won't be able to use the hardware to do
        // the blt (a similar argument could be made for weird rops
        // and stuff that we'll only end up having GDI simulate, but
        // those should happen infrequently enough that I don't care).
        //**********************************************************************

        if ((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL)) {
            ASSERTDD(pdsurfSrc->pso->iBitmapFormat == ppdev->iBitmapFormat, "oops");
            //******************************************************************
            // See 'DrvCopyBits' for some more comments on how this
            // moving-it-back-into-off-screen-memory thing works:
            //******************************************************************

            if (pdsurfSrc->iUniq == ppdev->iHeapUniq) {
                //**************************************************************
                // Attempt to move DIB back to offscreen VRAM.
                // This won't necessarily succeed.
                //
                // NOTE: If space doesn't get freed up for a while (heapUniq
                //       remains the same), and this call fails, then cBlt will
                //       wrap around to 0xffffffff, meaning we won't consider
                //       putting this DFB DIB back into offscreen VRAM for a
                //       LONG time, until someone frees up some offscreen memory,
                //       and causes cBlt to be reset to HEAP_COUNT_DOWN.
                //**************************************************************

                if (--pdsurfSrc->cBlt == 0)
                    if (bMoveDibToOffscreenDfbIfRoom(ppdev, pdsurfSrc))
                              goto BltDstDevBmp;
            } else {
                
                //**************************************************************
                // Some space was freed up in off-screen memory,
                // so reset the counter for this DFB:
                //**************************************************************
                ASSERTDD(pdsurfSrc->pso->iBitmapFormat == ppdev->iBitmapFormat, "oops");

                pdsurfSrc->iUniq = ppdev->iHeapUniq;
                pdsurfSrc->cBlt  = HEAP_COUNT_DOWN;
            }

        }
        
        psoSrc = pdsurfSrc->pso;
        pdsurfSrc = (DSURF *) psoSrc->dhsurf;

    }

BltDstDevBmp:
    //**************************************************************************
    // Get pointers to the Destination and Source 'Bitmaps'
    // We'll use these to determine if the bitmap is ONSCREEN or not.
    //**************************************************************************

    DstBits = (ULONG *)(psoDst->pvBits);
    SrcBits = (ULONG *)(psoSrc->pvBits);
    ScreenBitmap = (ULONG *)(ppdev->pjScreen);

    //**************************************************************************
    // Bitmaps can be one of 4 types, resulting in 16 ways to do blits as follows
    //
    //
    // 1) DIB,  2) DFB (OFFSCREEN),  3) DFB (SYS MEMORY),   4) SCREEN
    //
    //
    //    DIB               to DIB                -> Will not handle - Punt to GDI
    //    DFB (OFFSCREEN)   to DIB                -> Will not handle - Punt to GDI
    //    DFB (SYS MEMORY)  to DIB                -> Will not handle - Punt to GDI
    // SCREEN               to DIB                -> Will not handle - Punt to GDI
    //
    //    DIB               to DFB (OFFSCREEN)    -> Handled
    //    DFB (OFFSCREEN)   to DFB (OFFSCREEN)    -> Handled (rare?)
    //    DFB (SYS MEMORY)  to DFB (OFFSCREEN)    -> Handled (rare?)
    // SCREEN               to DFB (OFFSCREEN)    -> Handled
    //
    //    DIB               to DFB (SYS MEMORY)   -> Will not handle - Punt to GDI
    //    DFB (OFFSCREEN)   to DFB (SYS MEMORY)   -> Will not handle - Punt to GDI ??
    //    DFB (SYS MEMORY)  to DFB (SYS MEMORY)   -> Will not handle - Punt to GDI ??
    // SCREEN               to DFB (SYS MEMORY)   -> Will not handle - Punt to GDI
    //
    //    DIB               to SCREEN             -> Handled
    //    DFB (OFFSCREEN)   to SCREEN             -> Handled
    //    DFB (SYS MEMORY)  to SCREEN             -> Handled
    // SCREEN               to SCREEN             -> Handled
    //
    //
    //      We'll only need to handle the following cases:  (Punt the rest to GDI)
    //      The reason we need to check against ScreenBitmap is that
    //      we've mapped the primary surface using EngCreateBitmap,
    //      so that we can let GDI draw directly to it.  Because of this,
    //      the primary surface is NOT a device managed surface, but rather
    //      a GDI-managed surface.  This means that pdsurf for the
    //      primary surface will be NULL.  So in order to determine
    //      if a surface corresponds to the primary screen surface,
    //      we simply check it's pointer against the ScreenBitmap pointer
    //
    //      Optional:  Since we started with a Dumb Frame Buffer driver using
    //                 EngCreateBitmap (primary surface is GDI-managed, not device
    //                 managed), we check if a standard DIB is the primary surface
    //                 by checking if pvBits == ScreenBitmap.  We can avoid
    //                 this by converting the primary surface to be device managed.
    //                 (EngCreateDeviceSurface). This would result in a more consistent
    //                 driver. (This is the original way it was done in the S3 driver).
    //                 However, we started with a Dumb Frame buffer driver, so
    //                 just leave it for now. It doesn't seem to make that much
    //                 of a difference except for a couple of extra 'if' statements.
    //
    //
    //      We only handle blits where the destination is VRAM, as follows:
    //
    //
    //  1) SCREEN to SCREEN             (CopyBlit)
    //
    //        pdsurfSrc == 0 && SrcBits == ScreenBitmap
    //        pdsurfDst == 0 && DstBits == ScreenBitmap
    //
    //  2) DFB (SYS MEMORY) to SCREEN   (Memory to Screen)
    //
    //        pdsurfSrc != 0 && pdsurfSrc->dt == DT_DIB     && SrcBits != ScreenBitmap;
    //        pdsurfDst == 0 && DstBits == ScreenBitmap
    //
    //  3) DFB (OFFSCREEN) to SCREEN    (CopyBlit)
    //
    //        pdsurfSrc != 0 && pdsurfSrc->dt == DT_SCREEN  &&  SrcBits != ScreenBitmap
    //        pdsurfDst == 0 && DstBits == ScreenBitmap
    //
    //  4) DIB to SCREEN                (Memory to Screen)
    //
    //        pdsurfSrc == 0 && SrcBits != ScreenBitmap;
    //        pdsurfDst == 0 && DstBits == ScreenBitmap
    //
    //  5) SCREEN to DFB (OFFSCREEN)    (CopyBlit)
    //
    //        pdsurfSrc == 0 && SrcBits == ScreenBitmap
    //        pdsurfDst != 0 && pdsurfDst->dt == DT_SCREEN  && DstBits != ScreenBitmap
    //
    //  6) DFB (SYS MEMORY) to DFB (OFFSCREEN)   (Memory to Screen)
    //
    //        pdsurfSrc != 0 && pdsurfSrc->dt == DT_DIB     && SrcBits != ScreenBitmap
    //        pdsurfDst != 0 && pdsurfDst->dt == DT_SCREEN  && DstBits != ScreenBitmap
    //
    //  7) DFB (OFFSCREEN) to DFB (OFFSCREEN)    (CopyBlit)
    //
    //        pdsurfSrc != 0 && pdsurfSrc->dt == DT_SCREEN  && SrcBits != ScreenBitmap
    //        pdsurfDst != 0 && pdsurfDst->dt == DT_SCREEN  && DstBits != ScreenBitmap
    //
    //  8) DIB to DFB (OFFSCREEN)       (Memory to Screen)
    //
    //        pdsurfSrc == 0 && SrcBits != ScreenBitmap
    //        pdsurfDst != 0 && pdsurfDst->dt == DT_SCREEN  DstBits != ScreenBitmap
    //
    //
    //      If the destination is system memory, then we send it back to GDI
    //
    //**************************************************************************

    //**************************************************************************
    // Determine the clipping type
    //**************************************************************************

    if (pco == NULL)
        jClip = DC_TRIVIAL;             // Don't have to worry about clipping
    else
        jClip = pco->iDComplexity;      // Use the CLIPOBJ-provided clipping


    //**************************************************************************
    // Check if DEST is to VRAM or SYSTEM MEMORY
    //**************************************************************************

    if ((DstBits == ScreenBitmap) ||
        ((pdsurfDst !=NULL) && (pdsurfDst->dt == DT_SCREEN)))

        {

        //**********************************************************************
        // Set linear offset and stride for destination buffer
        //**********************************************************************

        if (pdsurfDst != 0)
            {
            DstOffset = (ULONG)((BYTE *)(pdsurfDst->LinearPtr) - ppdev->pjFrameBufbase);
            DstStride = pdsurfDst->LinearStride;
            }
        else
            {
            DstOffset = ppdev->ulPrimarySurfaceOffset;
            DstStride = ppdev->lDelta;
            }

        //**********************************************************************
        // Check to see if this is an 8BPP VRAM to 8BPP VRAM with translation
        // If so, kick out the bitmap to system memory.
        //**********************************************************************
        
        if (ppdev->iBitmapFormat  == BMF_8BPP &&
            psoSrc->iBitmapFormat == BMF_8BPP &&
            pxlo != NULL                      && 
            (pxlo->flXlate & XO_TABLE)        &&
            pdsurfSrc != NULL                 && 
            (pdsurfSrc->dt == DT_SCREEN))

            {
                //
                // We can not flush the primary screen
                //
                if (psoSrc->iType != STYPE_DEVICE)
                { 
                    DISPDBG((3,"Kick out to system memory"));
                    pohMoveOffscreenDfbToDib(ppdev,pdsurfSrc->poh);
                }
            }

        //**********************************************************************
        // Set pitch and stride of destination buffer
        //**********************************************************************

        ppdev->pfnSetDestBase(ppdev, DstOffset , DstStride);

        //**********************************************************************
        // DEST is VRAM
        // Check if the ROP4 is really a ROP3
        //**********************************************************************


        if (((rop4 >> 8) & 0xff) == (rop4 & 0xff))
            {
            //******************************************************************
            // Since we've already handled the cases where the ROP4 is really
            // a ROP3 and no source is required, we can assert...
            //******************************************************************

            ASSERTDD((psoSrc != NULL) && (pptlSrc != NULL),
                   "Expected no-source case to already have been handled");

            //******************************************************************
            // Since the foreground and background ROPs are the same, we
            // don't have to worry about no stinking masks (it's a simple
            // Rop3).
            //******************************************************************

            rop3 = (BYTE) rop4;     // Make it into a Rop3 (we keep the rop4
                                    //  around in case we decide to punt)

            //******************************************************************
            // Check if a pattern is required
            //******************************************************************

            if ((rop3 >> 4) == (rop3 & 0xf))
                {
                //**************************************************************
                // The ROP3 doesn't require a pattern:
                // Check if the source is from VRAM or SYSTEM MEMORY
                //**************************************************************

                if ((SrcBits == ScreenBitmap)   ||
                    ((pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_SCREEN)))

                    {
#if (_WIN32_WINNT >= 0x0500)
                    if ( ((rop3 == 0xcc) || pBlendObj) && 
                         (ppdev->iBitmapFormat == BMF_32BPP) )
                        {
                        alphaPreserve = COPYBLIT;
                        NV4DmaPushAlphaPreserve(ppdev, TRUE, FALSE); // Start=TRUE, FromCpu=FALSE
                        }
#endif // NT5
                    //**********************************************************
                    // Linear memory management
                    // Set linear offset and stride for source buffer
                    //**********************************************************

                    if (pdsurfSrc != 0)
                        {
                        SrcOffset = (ULONG)((BYTE *)(pdsurfSrc->LinearPtr) - ppdev->pjFrameBufbase);
                        SrcStride = pdsurfSrc->LinearStride;
                        }
                    else
                        {
                        SrcOffset = ppdev->ulPrimarySurfaceOffset;
                        SrcStride = ppdev->lDelta;
                        }

                    ppdev->pfnSetSourceBase(ppdev,SrcOffset,SrcStride);

                    //**********************************************************
                    // DEST is VRAM
                    // SOURCE is VRAM  (on-screen or off-screen)
                    //
                    // SCREEN TO SCREEN blt with no color translate
                    //**********************************************************

                    if ((pxlo == NULL) || (pBlendObj && !pxlo->flXlate) || (pxlo->flXlate & XO_TRIVIAL))
                        {
                        
                        if (jClip == DC_TRIVIAL)

                            {
                            //**************************************************
                            // No clipping required
                            //**************************************************
#if _WIN32_WINNT >= 0x0500
                            if (pBlendObj)
                                {
                                if (!NV4DmaPushCopyAlphaBlt(ppdev, 1, prclDst, rop4, pptlSrc, prclDst, pBlendObj))
                                    {
                                    goto EngFallBack; // Routine returned failure - must punt
                                    }
                                goto All_Done;
                                }
#endif _WIN32_WINNT >= 0x0500

                            ppdev->pfnCopyBlt(ppdev, 1, prclDst, rop4, pptlSrc, prclDst, pBlendObj);
                            goto All_Done;
                            }

                        else if (jClip == DC_RECT)

                            {
                            //**************************************************
                            // Clip to a single rectangle
                            //**************************************************

                            if (bIntersect(prclDst, &pco->rclBounds, &rcl))
                                {
#if _WIN32_WINNT >= 0x0500
                                if (pBlendObj)
                                    {
                                    if (!NV4DmaPushCopyAlphaBlt(ppdev, 1, &rcl, rop4, pptlSrc, prclDst, pBlendObj))
                                        {
                                        goto EngFallBack; // Routine returned failure - must punt
                                        }
                                    goto All_Done;
                                    }
#endif _WIN32_WINNT >= 0x0500
                                ppdev->pfnCopyBlt(ppdev, 1, &rcl, rop4, pptlSrc, prclDst, pBlendObj);
                                }

                            goto All_Done;
                            }

                        else

                            {
                            //**************************************************
                            // Clip region must be enumerated
                            //**************************************************

                            if (pptlSrc->y >= (prclDst->top))
                                {
                                if (pptlSrc->x >= (prclDst->left))
                                    iDir = CD_RIGHTDOWN;
                                else
                                    iDir = CD_LEFTDOWN;
                                }
                            else
                                {
                                if (pptlSrc->x >= (prclDst->left))
                                    iDir = CD_RIGHTUP;
                                else
                                    iDir = CD_LEFTUP;
                                }

                            CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES,
                                                 iDir, 0);

                            do  {
                                bMore = CLIPOBJ_bEnum(pco, sizeof(ce),
                                                        (ULONG*) &ce);

                                c = cIntersect(prclDst, ce.arcl, ce.c);

                                if (c != 0)
                                    {
#if _WIN32_WINNT >= 0x0500
                                    if (pBlendObj)
                                        {
                                        if (!NV4DmaPushCopyAlphaBlt(ppdev, c, ce.arcl, rop4, pptlSrc, prclDst, pBlendObj))
                                            {
                                            goto EngFallBack; // Routine returned failure - must punt
                                            }
                                        }
                                        else
                                        {
#endif _WIN32_WINNT >= 0x0500
                                        ppdev->pfnCopyBlt(ppdev, c, ce.arcl, rop4, pptlSrc, prclDst, pBlendObj);
#if _WIN32_WINNT >= 0x0500
                                        }
#endif _WIN32_WINNT >= 0x0500


                                    }

                                } while (bMore);
                             goto All_Done;

                            }

                        } // if (pxlo == NULL)...

                    } // if (psoSrc !=NULL) ....

                else

                    {

                    //**********************************************************
                    // DEST is VRAM
                    // SOURCE is SYSTEM MEMORY
                    //**********************************************************

                    iSrcBitmapFormat = psoSrc->iBitmapFormat;
                    iDstBitmapFormat = ppdev->iBitmapFormat;

                    if (iSrcBitmapFormat == BMF_1BPP)

                        {
                        //******************************************************
                        // Transfer from a 1bpp bitmap
                        //******************************************************
                        pfnXfer = ppdev->pfnXfer1bpp;
                        goto Xfer_It;
                        }

                    else if (((iSrcBitmapFormat == iDstBitmapFormat) ||
                              (pBlendObj && (iDstBitmapFormat == BMF_16BPP) && (iSrcBitmapFormat == BMF_32BPP)) )
                             &&
                             ((pxlo == NULL) || (pBlendObj && !pxlo->flXlate) || (pxlo->flXlate & XO_TRIVIAL)))
                        {

                        //******************************************************
                        // Transfer from bitmap with SAME pixel depth
                        //******************************************************

#if (_WIN32_WINNT >= 0x0500)
                        // effectively an RGB bitmap, so preserve alpha
                        if ( ((rop3 == 0xcc) || pBlendObj) && 
                             (ppdev->iBitmapFormat == BMF_32BPP) )
                            {
                            alphaPreserve = IMAGEFROMCPU;
                            NV4DmaPushAlphaPreserve(ppdev, TRUE, TRUE); // Start=TRUE, FromCpu=TRUE
                            }
#endif // NT5

                        pfnXfer = ppdev->pfnXferNative;
                        goto Xfer_It;

                        }

                    else if ((iSrcBitmapFormat == BMF_4BPP) &&
                             (iDstBitmapFormat == BMF_8BPP))

                        {
                        //******************************************************
                        // Transfer from 4bpp Source Bitmap
                        // to 8bpp Destination Bitmap
                        //******************************************************

                        pfnXfer = ppdev->pfnXfer4bpp;
                        goto Xfer_It;
                        }

                    else if (((iSrcBitmapFormat == BMF_8BPP) ||
                              (iSrcBitmapFormat == BMF_4BPP)) &&
                             ((iDstBitmapFormat == BMF_32BPP) ||
                              (iDstBitmapFormat == BMF_16BPP)) &&
                              (pxlo!=NULL) &&
                              (pxlo->flXlate & XO_TABLE))
                        {

                        //******************************************************
                        // Use hardware Indexed Image to transfer from 4bpp or 8bpp
                        // source bitmap to 16bpp or 32bpp destination bitmap
                        //******************************************************

                        pfnXfer = ppdev->pfnIndexedImage;

                        if (pfnXfer != NULL)
                            goto Xfer_It;
                        }

                    } // DEST is VRAM, SRC is SYSTEM MEMORY

                } // ...if (pattern NOT required)

            else

                {
                //**************************************************************
                // Blit is to VRAM, we have a source bitmap in system memory,
                // AND a pattern is required
                //**************************************************************

                ASSERTDD(!pBlendObj ,"No AlphaBlends with patterns");

                if (  ((pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_DIB)) ||
                       (pdsurfSrc == NULL) && (SrcBits !=ScreenBitmap))
                    {

                    //**********************************************************
                    // Need to check clipping here too !!!  We'll only handle simple clipping for now...
                    //**********************************************************

                    //**********************************************************
                    // If iSolidColor == 0xffffffff and pvRbrush == NULL, that means
                    // the brush has not yet been realized.  This is our cue to call
                    // BRUSHOBJ_pvGetRBrush which will call our DrvRealizeBrush function
                    //**********************************************************

                    rbc.iSolidColor = pbo->iSolidColor;

                    //**********************************************************
                    // Prepare to use solid pattern color
                    //**********************************************************

                    PatternColor = rbc.iSolidColor;

                    if (rbc.iSolidColor == -1)

                        {
                        //******************************************************
                        // Try and realize the pattern brush; by doing
                        // this call-back, GDI will eventually call us
                        // again through DrvRealizeBrush:
                        //******************************************************

                        //******************************************************
                        // TODO:  ... Implement ROP3 Pattern,Source,Destination
                        //            blits for non-solid brushes for better
                        //            performance.  We currently don't support
                        //            Colored Brushes.  We only support
                        //            monochrome brushes right now...
                        //******************************************************

                        goto EngFallBack;

                        //******************************************************
                        // Need to test if this works...
                        //******************************************************

                        rbc.prb = pbo->pvRbrush;
                        if (rbc.prb == NULL)

                            {
                            rbc.prb = BRUSHOBJ_pvGetRbrush(pbo);
                            if (rbc.prb == NULL)

                                {
                                //**********************************************
                                // If we couldn't realize the brush, punt
                                // the call (it may have been a non 8x8
                                // brush or something, which we can't be
                                // bothered to handle, so let GDI do the
                                // drawing):
                                //**********************************************

                                goto EngFallBack;
                                }
                            }

                        }

                    //**********************************************************
                    // At this point, let's check if it's a monochrome bitmap that's being passed
                    // to us.  If so, we can do it using IMAGE_MONO_FROM_CPU.
                    //**********************************************************

                    iSrcBitmapFormat = psoSrc->iBitmapFormat;
                    if (iSrcBitmapFormat == BMF_1BPP)

                        {
                        //******************************************************
                        // Transfer from a 1bpp bitmap
                        //******************************************************

                        pfnXfer = ppdev->pfnXfer1bpp;
                        goto Xfer_It;
                        }

                    //**********************************************************
                    // Be careful!  When swithing modes with the display applet,
                    // GDI will NOT update the SrcBitmap formats to agree with the
                    // new resolution.  If you started NT at 16bpp, the source bitmap format
                    // will be 16bpp.  But if you mode switch to 8bpp, the source bitmap
                    // format will remain at 16bpp. Beware....performance tests will run
                    // better if you do NOT do mode switches!
                    //**********************************************************

                    if (iSrcBitmapFormat == ppdev->iBitmapFormat)
                        {

                        //******************************************************
                        // DEST is SCREEN
                        // SOURCE is MEMORY
                        // PATTERN is required as well
                        //******************************************************

#if _WIN32_WINNT >= 0x0500
                        if(iSrcBitmapFormat == BMF_16BPP)
                        {
                            // On WIN2K Multi-Monitors support,  the GDI passed the
                            // 15BPP format source bitmap to us.  Make additional call
                            // to check whether it is 15BPP format bitmap
                        
                            ULONG   RGBFields[3] = { 0, 1, 2};
                            ULONG   ulXret;
                        
                            ulXret = XLATEOBJ_cGetPalette(pxlo, XO_SRCBITFIELDS, 3, RGBFields);
                            
                            if(ulXret && RGBFields[1] == 0x03E0)
                            {
                                // It is 15BPP format bitmap; our driver only handle 16Bpp bitmap
                                // punt back to GDI.
                                DISPDBG((5, "XLATEOBJ_cGetPalette returned %ld and { %x, %x, %x }",
                                    ulXret, RGBFields[0], RGBFields[1], RGBFields[2]));
                            
                                goto EngFallBack;
                            
                            }
                            
                        }
#endif                        
                        rop3 = (BYTE) rop4;

                        if (jClip == DC_TRIVIAL)
                        {
                            ppdev->pfnMemToScreenPatternBlt(ppdev,
                                                          psoSrc,
                                                          pptlSrc,
                                                          prclDst,
                                                          (ULONG)rop3,
                                                          &rbc,
                                                          NULL);
                        }
                        else if (jClip == DC_RECT)
                        {
                            if (bIntersect(prclDst, &pco->rclBounds, &rcl))
                            {
                                    ppdev->pfnMemToScreenPatternBlt(ppdev,
                                                          psoSrc,
                                                          pptlSrc,
                                                          prclDst,
                                                          (ULONG)rop3,
                                                          &rbc,
                                                          &rcl);
                            }
                        }
                        else
                        {
                                //**********************************************************************
                                // Clip region must be enumerated
                                //**********************************************************************

                                CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

                                do  {
                                    bMore = CLIPOBJ_bEnum(pco, sizeof(ce),
                                        (ULONG*) &ce);

                                    c = cIntersect(prclDst, ce.arcl, ce.c);

                                    if (c != 0)
                                    {
                                        ppdev->pfnMemToScreenPatternBlt(ppdev,
                                                          psoSrc,
                                                          pptlSrc,
                                                          prclDst,
                                                          (ULONG)rop3,
                                                          &rbc,
                                                          ce.arcl);
                                    }
                                } while (bMore);
                        }

                        goto All_Done;
                        }

                    } // Make sure source is from system memory
                 
                

                else  // else (Source is from VRAM)    ( dest is to vram and we need a pattern )
                    {
                        // VRAM->VRAM + PATTERN BLITS

                        // Check if brush is needed ( StoS + Non-Solid Pat. )
    
                        if (pbo->iSolidColor == 0xFFFFFFFF )
                        {
                            //******************************************************
                            // Realize the pattern brush if it's ptr is NULL
                            //******************************************************
    
                            if (pbo->pvRbrush == NULL)
                            {
                                pbo->pvRbrush = BRUSHOBJ_pvGetRbrush(pbo);
                                if (pbo->pvRbrush == NULL)
                                {
                                    //**********************************************
                                    // If we couldn't realize the brush, let GDI handle
                                    // the call (it may have been a non 8x8
                                    // brush or something, which we can't be
                                    // bothered to handle, so let GDI do the
                                    // drawing):
                                    //**********************************************
                                    goto EngFallBack;
                                }
                            }
                        }
                        
                        //**********************************************************************
                        // Linear memory management
                        // Set linear offset and stride for source buffer
                        //**********************************************************************
            
                        if (pdsurfSrc != 0)
                            {
                            SrcOffset = (ULONG)( (BYTE *)(pdsurfSrc->LinearPtr) - ppdev->pjFrameBufbase);
                            SrcStride = pdsurfSrc->LinearStride;
                            }
                        else           
                            {
                            SrcOffset = ppdev->ulPrimarySurfaceOffset;
                            SrcStride = ppdev->lDelta;
                            }
            
                        (ppdev->pfnSetSourceBase)(ppdev,SrcOffset,SrcStride);
            
                        // pfnSetDestBase was already called at that point
            
                        if( 
                            (ppdev->pfnScreenToScreenWithPatBlt != NULL )             &&  // function exist
                            ( (pxlo==NULL) || (pxlo->flXlate & XO_TRIVIAL) )          &&  // no translation
                            ( jClip == DC_TRIVIAL )                                       // trivial clipping
                            
                          ) 
                        {
            
                            (ppdev->pfnScreenToScreenWithPatBlt)(      ppdev, 
                                                                           1, 
                                                                     prclDst, 
                                                                     pptlSrc, 
                                                                     prclDst,
                                                                         pbo,
                                                                   pptlBrush,
                                                                        rop4  );
            
                            goto All_Done;
            
            
                        }
                    
                    } // ...else (Source is from VRAM)    ( dest is to vram and we need a pattern )

                } // ...else( pattern IS required)

            } // ...if (not a Rop4)

        } // ...if (DstBits == ScreenBitmap)

    else

        {

        //**********************************************************************
        // DEST is SYSTEM MEMORY
        //**********************************************************************

        //**********************************************************************
        // Can't support AlphaBlend to system memory
        // Only supported on Win2k
        //**********************************************************************

#if _WIN32_WINNT >= 0x0500
        
        if (pBlendObj)
            {
            goto EngFallBack;
            }

#endif //_WIN32_WINNT >= 0x0500
         
            

            if ((SrcBits == ScreenBitmap) ||
                ((pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_SCREEN)))
            {
                //******************************************************************
                // DEST is SYSTEM MEMORY
                // SOURCE is VRAM
                //******************************************************************
                
                // Check to see if we have trivial clipping and source copy rop...
                //
                
                if (
                     ( jClip == DC_TRIVIAL )                               &&
                     ( ( (rop4 >> 8) & 0xff  ) == (rop4 & 0xff) )          &&
                     ( (rop4 & 0xff) == 0xcc   )                                
                   )
                {
    
                    //******************************************************************
                    // Check if this is a color to monochrome bitmap conversion
                    //******************************************************************
        
                    if ( (psoDst->iBitmapFormat == BMF_1BPP)                    &&
                         (ppdev->iBitmapFormat != BMF_24BPP)                    &&
                         ( (pxlo!=NULL) && (pxlo->flXlate & XO_TO_MONO)  )
                       )
                    {
                       //**************************************************************
                       // ScreenTo1Bpp will return TRUE is successful.
                       // Otherwise, the call will be punted (this probably means
                       // the temporary buffer wasn't large enough to handle the blit)
                       //**************************************************************
        
                       if (ppdev->pfnScreenTo1BppMemBlt != NULL)
                       {
                           if ( (ppdev->pfnScreenTo1BppMemBlt)(ppdev, 1, prclDst, psoSrc, psoDst, pptlSrc, prclDst, pxlo ) == TRUE)
                               goto All_Done;
                       }
                       
                    } //... if ScreenTo1Bpp (Screen->SysMem)
    
                    //
                    // Else check to see if formats are equal...
                    //
                    else if(
                               ( psoDst->iBitmapFormat == ppdev->iBitmapFormat )     && // if
                               ( (pxlo==NULL) || (pxlo->flXlate & XO_TRIVIAL) )         // no translation
                           )
                    {
                        // Formats are equal, we don't need to do any xlate,
                        // so use the general ScreenToMemBlt
    
                        if (ppdev->pfnScreenToMemBlt != NULL )
                        {
                    
                           if ( (ppdev->pfnScreenToMemBlt)(   ppdev, 
                                                                  1, 
                                                            prclDst, 
                                                             psoSrc, 
                                                             psoDst, 
                                                            pptlSrc, 
                                                            prclDst, 
                                                               pxlo  ) == TRUE )
                               goto All_Done;
                        }
                    }
                    
                    else // else color translation is needed.
                    {
    
                        // Formats are not equal, handle 32,16 -> 4 or 8 bpp blits
                        // 
                        switch(ppdev->cjPelSize)  // All the frame buffer is the same format at this time so
                        {                         // we use cjPelSize in ppdev to see what's the dest format
                        case 4: 
                            switch(psoDst->iBitmapFormat)
                            {
                            case BMF_4BPP:
                                pfnStoM = ppdev->pfnScreenToMem32to4bppBlt;
                                break;
                      
                            case BMF_8BPP:
                                pfnStoM = ppdev->pfnScreenToMem32to8bppBlt;
                                break;
                      
                            default:
                                goto EngFallBack;
                            }
                      
                            break; // if SRC == 32 bit/pixel
                      
                        case 2:
                      
                            //goto EngFallBack; // UNTESTED !! (the code is there tough)
                      
                            switch(psoDst->iBitmapFormat)
                            {
                            case BMF_4BPP:
                      
                                pfnStoM = ppdev->pfnScreenToMem16to4bppBlt;
                                break;
                      
                            case BMF_8BPP:
                      
                                pfnStoM = ppdev->pfnScreenToMem16to8bppBlt;
                                break;
                      
                            default:
                                goto EngFallBack;
                            }
                      
                            break; // if SRC == 16 bit/pixel
                      
                        default:

                            // Source is indexed, don't need the fancy nonindexed to indexed
                            // color translate blit routines.  Translation is more
                            // simple in that case, and we could do 2 other ScreenToMem*to*
                            // (just need a slight modification in the core macros of CpuXfers,
                            //  see nv4bltc.c)  This is not added because "who sets their UI
                            // in 8bpp anyway ? "
                        case 1:
                            goto EngFallBack;
                        
                        } // ...swith( ppdev->cjPelSize ) ( type of source)
                        
                        //
                        // Now that the proper function is chosen, do the blt
                        //
                        
                        if (pfnStoM != NULL )
                        {
                    
                           if ( (pfnStoM)(   ppdev, 
                                                 1, 
                                           prclDst, 
                                            psoSrc, 
                                            psoDst, 
                                           pptlSrc, 
                                           prclDst, 
                                              pxlo  ) == TRUE )
                               goto All_Done;
                        }
                      
    
                    } // ...else color translation is needed.
                
                } // ...if (without clipping and with source copy rop)  (in ScreenToMem)
                             
                
            } // ...if (Src is screen)  (Dest is system mem)

        else
            {
            //******************************************************************
            // DEST is SYSTEM MEMORY
            // SOURCE is SYSTEM MEMORY
            //******************************************************************

            }


        }


    //**************************************************************************
    // Let GDI handle all other blits for now....
    //
    // Destination = System Memory
    // True ROP 4's com here (masks required)
    //**************************************************************************

    goto EngFallBack;


    //**************************************************************************
    // Common bitmap transfer
    //**************************************************************************

Xfer_It:


#if _WIN32_WINNT >= 0x0500
    if (pBlendObj &&
        ((pfnXfer != ppdev->pfnXferNative) || !NV4CheckAlphaBlendOK(ppdev)))
        {
        //**************************************************************************
        // Only native xfer routines can handle an alpha blend.  If we get here with 
        // anything but a native xfer we are likely blending between different depths.
        // Call hw specific routine to check if hw capable of blending in current state
        //**************************************************************************

        goto EngFallBack;
        }

#endif _WIN32_WINNT >= 0x0500

    if (jClip == DC_TRIVIAL)

        {
        //**********************************************************************
        // No clipping required
        //**********************************************************************

        pfnXfer(ppdev, 1, prclDst, rop4, psoSrc, pptlSrc, prclDst, pxlo,PatternColor,pBlendObj);

        goto All_Done;
        }

    else if (jClip == DC_RECT)

        {

        //**********************************************************************
        // Clip to a single rectangle
        //**********************************************************************
        if (bIntersect(prclDst, &pco->rclBounds, &rcl))
            pfnXfer(ppdev, 1, &rcl, rop4, psoSrc, pptlSrc, prclDst, pxlo,PatternColor,pBlendObj);

        goto All_Done;
        }

    else

        {

        //**********************************************************************
        // Clip region must be enumerated
        //**********************************************************************

        CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

        do  {
            bMore = CLIPOBJ_bEnum(pco, sizeof(ce),
                                    (ULONG*) &ce);

            c = cIntersect(prclDst, ce.arcl, ce.c);

            if (c != 0)
                {
                pfnXfer(ppdev, c, ce.arcl, rop4, psoSrc,
                        pptlSrc, prclDst, pxlo,PatternColor,pBlendObj);
                }

            } while (bMore);
        goto All_Done;
        }



EngFallBack:

    //**************************************************************************
    // GDI only knows how to write to standard DIB surfaces.
    // That means, if the SOURCE or DEST is a DFB, we need to create a standard
    // DIB SurfObj in place of the device managed surface (DFB) object in order
    // to let GDI do the blit.
    //
    // Essentially, there are 4 types of SURFOBJS we'll encounter.
    //
    //      1) System Memory SurfObj (Standard DIB)
    //          We can pass this surfobj to GDI as is.
    //
    //      2) DFB - exists in offscreen VRAM memory
    //          Need to create a standard DIB SurfObj to let GDI draw to this surface
    //
    //      3) DFB - exists in system memory
    //          Need to create a standard DIB surfObj to let GDI draw to this surface
    //
    //      4) Primary Surface VRAM (Standard DIB format)
    //          We can pass this surfobj to GDI as is.
    //
    //          CONDITION                                           SURFOBJ TYPE
    //          ---------                                           ------------
    //
    // (pdsurf != NULL)  &&  (pdsurf->dt == DT_DIB)         --> This is a DFB in system memory
    // (pdsurf != NULL)  &&  (pdsurf->dt == DT_SCREEN)      --> This is a DFB in offscreen VRAM memory
    //
    // (pdsurf == NULL)  &&  (pvBits != ppdev->pjScreen)    --> Standard DIB
    // (pdsurf == NULL)  &&  (pvBits == ppdev->pjScreen)    --> Primary Screen Surface
    //**************************************************************************


    //**************************************************************************
    // Default to Standard DIB, and get handles to surfaces if they exist
    //**************************************************************************

    (VOID *)pdsurfSrc = NULL;
    (VOID *)pdsurfDst = NULL;

    if ((VOID *)psoSrc != NULL)
        pdsurfSrc = (DSURF*) psoSrc->dhsurf;

    if ((VOID *)psoDst != NULL)
        pdsurfDst = (DSURF*) psoDst->dhsurf;

    //**************************************************************************
    // Default to using the original, un-modified SURFOBJ's that were passed to us.
    //**************************************************************************

    TempSrcSurfObjPtr = psoSrc;
    TempDstSurfObjPtr = psoDst;

    //**************************************************************************
    // Get pointers to the 'wrapper' surfaces that we use to draw to the
    // screen surface if necessary.  These surfaces are locked.
    // (The surfaces that we pass to GDI MUST be locked).
    // See EnableOffscreenHeap for more information.
    //**************************************************************************

    SrcPuntPtr = ppdev->psoPunt;
    DstPuntPtr = ppdev->psoPunt2;

    //**************************************************************************
    // Check if source bitmap is a DFB.  If so, create a temporary surfobj
    // (mapped to the screen) that GDI can read from
    //**************************************************************************

    if (pdsurfSrc !=0 )
        {
        SrcPuntPtr->dhsurf      = 0;                    // Behave like a standard DIB
        if (pdsurfSrc->dt == DT_SCREEN)
            {
            SrcPuntPtr->sizlBitmap  = pdsurfSrc->sizl;
            SrcPuntPtr->cjBits  = pdsurfSrc->LinearStride * pdsurfSrc->sizl.cy;
            SrcPuntPtr->pvBits  = pdsurfSrc->LinearPtr;
            SrcPuntPtr->pvScan0 = pdsurfSrc->LinearPtr;
            SrcPuntPtr->lDelta  = pdsurfSrc->LinearStride;
            SrcPuntPtr->iBitmapFormat = ppdev->iBitmapFormat;
            }
        else
            {
            //**********************************************************************
            // This is a DFB which is stored in main memory as a DIB
            //**********************************************************************
            SrcPuntPtr->sizlBitmap  = pdsurfSrc->pso->sizlBitmap;
            SrcPuntPtr->cjBits      = pdsurfSrc->pso->lDelta * pdsurfSrc->pso->sizlBitmap.cy;
            SrcPuntPtr->pvBits  = pdsurfSrc->pso->pvBits;
            SrcPuntPtr->pvScan0 = pdsurfSrc->pso->pvScan0;
            SrcPuntPtr->lDelta  = pdsurfSrc->pso->lDelta;
            SrcPuntPtr->iBitmapFormat = pdsurfSrc->pso->iBitmapFormat;
            }

        //**********************************************************************
        // Use the 'wrapper' surface, instead of the original DFB surfobj for GDI
        //**********************************************************************

        TempSrcSurfObjPtr       = SrcPuntPtr;
        }


    //**************************************************************************
    // Check if destination bitmap is a DFB.  If so, create a temporary surfobj
    // (mapped to the screen) that GDI can write on
    //**************************************************************************

    if (pdsurfDst !=0 )
        {
        DstPuntPtr->dhsurf      = 0;                    // Behave like a standard DIB
        if (pdsurfDst->dt == DT_SCREEN)
            {
            DstPuntPtr->sizlBitmap  = pdsurfDst->sizl;
            DstPuntPtr->cjBits  = pdsurfDst->LinearStride * pdsurfDst->sizl.cy;
            DstPuntPtr->pvBits  = pdsurfDst->LinearPtr;
            DstPuntPtr->pvScan0 = pdsurfDst->LinearPtr;
            DstPuntPtr->lDelta  = pdsurfDst->LinearStride;
            DstPuntPtr->iBitmapFormat = ppdev->iBitmapFormat;
            }
        else
            {
            //**********************************************************************
            // This is a DFB which is stored in main memory as a DIB
            //**********************************************************************
            DstPuntPtr->sizlBitmap  = pdsurfDst->pso->sizlBitmap;
            DstPuntPtr->cjBits      = pdsurfDst->pso->lDelta * pdsurfDst->pso->sizlBitmap.cy;
            DstPuntPtr->pvBits  = pdsurfDst->pso->pvBits;
            DstPuntPtr->pvScan0 = pdsurfDst->pso->pvScan0;
            DstPuntPtr->lDelta  = pdsurfDst->pso->lDelta;
            DstPuntPtr->iBitmapFormat = pdsurfDst->pso->iBitmapFormat;
            }

        //**********************************************************************
        // Use the 'wrapper' surface, instead of the original DFB surfobj for GDI
        //**********************************************************************

        TempDstSurfObjPtr       = DstPuntPtr;
        }

    ppdev->pfnWaitEngineBusy(ppdev);

    //**********************************************************************
    // Only supported on Win2k
    //**********************************************************************

#if _WIN32_WINNT >= 0x0500
    
    if (pBlendObj) {
        //**********************************************************************
        // This was a call from DrvAlphaBlend so call EngAlphaBlend
        //**********************************************************************
        bRet = ppdev->pfnEngAlphaBlend(TempDstSurfObjPtr, TempSrcSurfObjPtr, pco, pxlo, prclDst,
                                       prclSrc, pBlendObj);
    } else {
        //**********************************************************************
        // This was a call from DrvBitBlt so call EngBitBlt
        //**********************************************************************
        bRet = ppdev->pfnEngBitBlt(TempDstSurfObjPtr, TempSrcSurfObjPtr, psoMsk, pco, pxlo, prclDst,
                                   pptlSrc, pptlMsk, pbo, pptlBrush, rop4);
    }

#else // NT 4.0
    
    //**********************************************************************
    // This was a call from DrvBitBlt so call EngBitBlt
    //**********************************************************************
    bRet = ppdev->pfnEngBitBlt(TempDstSurfObjPtr, TempSrcSurfObjPtr, psoMsk, pco, pxlo, prclDst,
                               pptlSrc, pptlMsk, pbo, pptlBrush, rop4);

#endif //_WIN32_WINNT >= 0x0500



All_Done:

    if (alphaPreserve != NONE)
        {
        NV4DmaPushAlphaPreserve(ppdev, FALSE, alphaPreserve==IMAGEFROMCPU); // Start=FALSE; TRUE if FromCPU
        }

    return(bRet);

    }


#if _WIN32_WINNT >= 0x0500

//******************************************************************************
//
//  Function:   TransparentBitBlt
//
//  Routine Description:
//
//      Simpler version of GenericBitBltAlpha, used only by DrvTransparentBlt
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL TransparentBitBlt(
    SURFOBJ*    psoDst,
    SURFOBJ*    psoSrc,
    CLIPOBJ*    pco,
    XLATEOBJ*   pxlo,
    RECTL*      prclDst,
    RECTL*      prclSrc,
    POINTL*     pptlSrc,
    ULONG       iTransColor,
    ULONG       Reserved,
    PDEV*       ppdev
)

    {
    DSURF*          pdsurfDst;
    DSURF*          pdsurfSrc;
    POINTL          ptlSrc;
    BYTE            jClip;
    BOOL            bMore;
    CLIPENUM        ce;
    LONG            c;
    RECTL           rcl;
    FNXFER*         pfnXfer = ppdev->pfnXferNative;
    ULONG           iDir;
    BOOL            bRet;
    ULONG*          ScreenBitmap;
    ULONG*          DstBits;
    ULONG*          SrcBits;
    
    SURFOBJ*        TempDstSurfObjPtr;
    SURFOBJ*        TempSrcSurfObjPtr;

    ULONG           SrcOffset;
    ULONG           DstOffset;
    ULONG           SrcStride;
    ULONG           DstStride;

    // Assume success
    bRet = TRUE;

    // Trivially reject TransparentBlts we can't do
    if ((pxlo && pxlo->flXlate && !(pxlo->flXlate & XO_TRIVIAL) ) ||
        ((ppdev->iBitmapFormat != BMF_32BPP) && (ppdev->iBitmapFormat != BMF_16BPP)) ||
        (psoSrc == NULL) ||
        (!ppdev->bEnabled))
        {
        // Can't do translations
        // Can't hw accelerate anything except 16,32 bpp desktop
        // Must have a source bitmap
        // ppdev must be enabled (may be disable in Stress test)
        goto EngFallBack;
        }

    // Trivially reject blts to host memory bitmaps.
    if (!IS_DEV_SURF(psoDst))
        {
        goto EngFallBack;
        }
    

    //**************************************************************************
    // For NV, our primary screen surface is a GDI-Managed bitmap, instead of a
    // DEVICE MANAGED surface.  This allows us to punt any call back to GDI
    // if necessary.  Therefore, pdsurfDst will be NULL when the destination
    // is the SCREEN.  pdsurfDst will be Non-NULL when the destination is a DFB.
    // As a result, we determine if the SURFOBJ is the primary surface by taking
    // a look at the 'pvBits' value.  If pvBits == ppdev->pjScreen, then the
    // surfobj is the primary surface.
    //**************************************************************************

    pdsurfDst = (DSURF*) psoDst->dhsurf;    // May be NULL since we created the
                                            // primary surface as a GDI Managed bitmap

    //**************************************************************************
    // Check to see if the dest is a device bitmap which has been moved to main
    // memory
    //**************************************************************************
    if ((pdsurfDst != NULL) && (pdsurfDst->dt == DT_DIB))
        {
        psoDst = pdsurfDst->pso;
        pdsurfDst = (DSURF *) psoDst->dhsurf;
        }

    //**************************************************************************
    // Here we know psoDst should NOT be NULL! (already punted if psoSrc=NULL)
    //**************************************************************************

    ASSERTDD(((VOID *)psoDst != NULL), "Didn't expect a NULL destination");

    pdsurfSrc = (DSURF*) psoSrc->dhsurf;

    TRY_MOVE_SRC_TO_OFFSCREEN_MEM(psoSrc, pdsurfSrc, pxlo, ppdev);

    //**************************************************************************
    // Get pointers to the Destination and Source 'Bitmaps'
    // We'll use these to determine if the bitmap is ONSCREEN or not.
    //**************************************************************************

    DstBits = (ULONG *)(psoDst->pvBits);
    SrcBits = (ULONG *)(psoSrc->pvBits);
    ScreenBitmap = (ULONG *)(ppdev->pjScreen);

    //**************************************************************************
    // Determine the clipping type
    //**************************************************************************

    if (pco == NULL)
        jClip = DC_TRIVIAL;             // Don't have to worry about clipping
    else
        jClip = pco->iDComplexity;      // Use the CLIPOBJ-provided clipping

    //**************************************************************************
    // Check if DEST is to VRAM or SYSTEM MEMORY
    //**************************************************************************

    if ((DstBits == ScreenBitmap) ||
        ((pdsurfDst !=NULL) && (pdsurfDst->dt == DT_SCREEN)))
        {
        //**********************************************************************
        // DEST is VRAM
        //**********************************************************************

        // Set linear offset and stride for destination buffer
        if (pdsurfDst != 0)
            {
            DstOffset = (ULONG)((BYTE *)(pdsurfDst->LinearPtr) - ppdev->pjFrameBufbase);
            DstStride = pdsurfDst->LinearStride;
            }
        else
            {
            DstOffset = ppdev->ulPrimarySurfaceOffset;
            DstStride = ppdev->lDelta;
            }

        // Set pitch and stride of destination buffer
        ppdev->pfnSetDestBase(ppdev, DstOffset , DstStride);

        // Check if the source is from VRAM or SYSTEM MEMORY
        if ((SrcBits == ScreenBitmap)   ||
            ((pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_SCREEN)))
            {
            //**********************************************************
            // DEST is VRAM
            // SOURCE is VRAM  (on-screen or off-screen)
            //
            // SCREEN TO SCREEN blt with no color translate
            //**********************************************************

            //**********************************************************
            // Linear memory management
            // Set linear offset and stride for source buffer
            //**********************************************************
    
            if (pdsurfSrc != 0)
                {
                SrcOffset = (ULONG)((BYTE *)(pdsurfSrc->LinearPtr) - ppdev->pjFrameBufbase);
                SrcStride = pdsurfSrc->LinearStride;
                }
            else
                {
                SrcOffset = ppdev->ulPrimarySurfaceOffset;
                SrcStride = ppdev->lDelta;
                }
    
            ppdev->pfnSetSourceBase(ppdev,SrcOffset,SrcStride);
    
    
            NV4DmaPushSetColorKey(ppdev, ppdev->iBitmapFormat, iTransColor);

            if (jClip == DC_TRIVIAL)
                {
                //**************************************************
                // No clipping required
                //**************************************************
                ppdev->pfnCopyBlt(ppdev, 1, prclDst, 0xcccc, pptlSrc, prclDst, NULL);
                
                goto All_Done;
                }
            else if (jClip == DC_RECT)
                {
                //**************************************************
                // Clip to a single rectangle
                //**************************************************

                if (bIntersect(prclDst, &pco->rclBounds, &rcl))
                    {

                    ppdev->pfnCopyBlt(ppdev, 1, &rcl, 0xcccc, pptlSrc, prclDst, NULL);
                    
                    }

                goto All_Done;
                }
            else
                {
                //**************************************************
                // Clip region must be enumerated
                //**************************************************

                if (pptlSrc->y >= (prclDst->top))
                    {
                    if (pptlSrc->x >= (prclDst->left))
                        iDir = CD_RIGHTDOWN;
                    else
                        iDir = CD_LEFTDOWN;
                    }
                else
                    {
                    if (pptlSrc->x >= (prclDst->left))
                        iDir = CD_RIGHTUP;
                    else
                        iDir = CD_LEFTUP;
                    }

                CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES,
                                     iDir, 0);

                do  {
                    bMore = CLIPOBJ_bEnum(pco, sizeof(ce),
                                            (ULONG*) &ce);

                    c = cIntersect(prclDst, ce.arcl, ce.c);

                    if (c != 0)
                        {
                        
                        ppdev->pfnCopyBlt(ppdev, c, ce.arcl, 0xcccc, pptlSrc, prclDst, NULL);

                        }

                    } while (bMore);
                 goto All_Done;
                 }
            }
        else
            {
            //**********************************************************
            // DEST is VRAM
            // SOURCE is SYSTEM MEMORY
            //**********************************************************

            // Transfer from bitmap with SAME pixel depth, or 32bpp source w/ 16bpp dest
            if ((psoSrc->iBitmapFormat == ppdev->iBitmapFormat) ||
                (psoSrc->iBitmapFormat == BMF_32BPP) && (ppdev->iBitmapFormat == BMF_16BPP))
                {
                
                pfnXfer = ppdev->pfnXferNative;
                goto Xfer_It;

                }
            } // DEST is VRAM, SRC is SYSTEM MEMORY
        } // ...if (DstBits == ScreenBitmap)

    //**************************************************************************
    // GDI handles all other blits
    //
    // Destination = System Memory
    //**************************************************************************
    goto EngFallBack;

    //**************************************************************************
    // Common bitmap transfer
    //**************************************************************************

Xfer_It:

    NV4DmaPushSetColorKey(ppdev, psoSrc->iBitmapFormat, iTransColor);

    if (jClip == DC_TRIVIAL)
        {
        //**********************************************************************
        // No clipping required
        //**********************************************************************

        pfnXfer(ppdev, 1, prclDst, 0xcccc, psoSrc, pptlSrc, prclDst, pxlo,0,NULL);

        }
    else if (jClip == DC_RECT)
        {
        //**********************************************************************
        // Clip to a single rectangle
        //**********************************************************************
        if (bIntersect(prclDst, &pco->rclBounds, &rcl))
            pfnXfer(ppdev, 1, &rcl, 0xcccc, psoSrc, pptlSrc, prclDst, pxlo,0,NULL);
        
        }
    else
        {

        //**********************************************************************
        // Clip region must be enumerated
        //**********************************************************************

        CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

        do  {
            bMore = CLIPOBJ_bEnum(pco, sizeof(ce),
                                    (ULONG*) &ce);

            c = cIntersect(prclDst, ce.arcl, ce.c);

            if (c != 0)
                {
                pfnXfer(ppdev, c, ce.arcl, 0xcccc, psoSrc, pptlSrc, prclDst, pxlo,0,NULL);
                }

            } while (bMore);
        }
    
    goto All_Done;

EngFallBack:

    PrepareSurfacesForPunt(psoSrc,psoDst,(SURFOBJ**)&TempSrcSurfObjPtr,(SURFOBJ**)&TempDstSurfObjPtr,ppdev);

    ppdev->pfnWaitEngineBusy(ppdev);
    bRet = ppdev->pfnEngTransparentBlt(TempDstSurfObjPtr,TempSrcSurfObjPtr,pco,pxlo,prclDst,prclSrc,iTransColor,Reserved); 

All_Done:

    NV4DmaPushSetColorKey(ppdev, 0, 0);
    
    return(bRet);

    }

#endif // _WIN32_WINNT >= 0x0500


//******************************************************************************
//
//  Function:   DrvBitBlt
//
//  Routine Description:
//
//      Implements Bit Block Transfers without alpha blend support -paul.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL DrvBitBlt(
    SURFOBJ*    psoDst,
    SURFOBJ*    psoSrc,
    SURFOBJ*    psoMsk,
    CLIPOBJ*    pco,
    XLATEOBJ*   pxlo,
    RECTL*      prclDst,
    POINTL*     pptlSrc,
    POINTL*     pptlMsk,
    BRUSHOBJ*   pbo,
    POINTL*     pptlBrush,
    ROP4        rop4)
{
    PDEV*           ppdev;
    
    ppdev = (PDEV*) psoDst->dhpdev;
    
    //**************************************************************************
    // ppdev may not be present in the Dst surface object
    // This can happen if the Dst surface object is a standard DIB
    //**************************************************************************

    if (ppdev == NULL || (ppdev->hdevEng != psoDst->hdev)) {

        if (psoSrc != NULL)
            ppdev = (PDEV *)psoSrc->dhpdev;
        if (ppdev == NULL || (ppdev->hdevEng != psoSrc->hdev))
            //******************************************************************
            // This blit call is either screwed up, or we got a
            // memory to memory blit (with no physical device associated).
            // Send the call back to GDI.
            //******************************************************************
                return(EngBitBlt(psoDst, psoSrc, psoMsk, pco, pxlo, prclDst,
                                pptlSrc, pptlMsk, pbo, pptlBrush, rop4));

    }

    //************************************************************************** 
    // Need to do smart sync between OpenGL and GDI depending on 
    // clip intersection.
    //**************************************************************************
    if (OglIsEnabled(ppdev)) // fast ogl check
    {
        bOglSyncIfGdiInOglWindowBlt(ppdev,psoDst,psoSrc,pco,prclDst,pptlSrc);        
    }


    return GenericBitBltAlpha(psoDst,
                              psoSrc,
                              psoMsk,
                              pco,
                              pxlo,
                              prclDst,
                              NULL,        // RECTL *prclSrc
                              pptlSrc,
                              pptlMsk,
                              pbo,
                              pptlBrush,
                              rop4,
                              NULL,         // BLENDOBJ *pBlendObj
                              ppdev
                              );

}

#if _WIN32_WINNT >= 0x0500

//******************************************************************************
//
//  Function:   DrvAlphaBlend
//
//  Routine Description:
//
//      Implements Bit Block Transfers with alpha blend support. -paul
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL DrvAlphaBlend(
    SURFOBJ  *psoDst,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ  *pxlo,
    RECTL  *prclDst,
    RECTL  *prclSrc,
    BLENDOBJ  *pBlendObj) 
{
    PDEV*           ppdev;
    POINTL          ptlSrc;
    LONG            SrcWidth;
    LONG            SrcHeight;


    //**************************************************************************
    // Either the source or the destination should have the physical device
    // associated with it. (We should never get a memory to memory blit)
    // Default to getting the PPDEV from the dst.  Otherwise, get it from source
    //**************************************************************************

    ppdev = (PDEV*) psoDst->dhpdev;
    //**************************************************************************
    // ppdev may not be present in the Dst surface object
    // This can happen if the Dst surface object is a standard DIB
    //**************************************************************************
    if (ppdev == NULL || (ppdev->hdevEng != psoDst->hdev) || (!ppdev->bEnabled)) 
        {

        if (psoSrc != NULL)
            ppdev = (PDEV *)psoSrc->dhpdev;
        if (ppdev == NULL || (ppdev->hdevEng != psoSrc->hdev) || (!ppdev->bEnabled))
            //******************************************************************
            // This blit call is either screwed up, or we got a
            // memory to memory blit (with no physical device associated).
            // Send the call back to GDI.
            //******************************************************************
            {
            return (EngAlphaBlend(psoDst,psoSrc,pco,pxlo,prclDst,prclSrc,pBlendObj)); 
            }
        }

    // NOTE: GDI does not pass the call if no per-pixel alpha and SourceConstantAlpha is 0xFF. 

    // We get called if SourceConstantAlpha == 0x00, but no visual change for this, so return ASAP
    if(!(pBlendObj->BlendFunction.SourceConstantAlpha))
        {
        return TRUE; // nothing to blend.
        }


    //************************************************************************** 
    // Need to do smart sync between OpenGL and GDI depending on 
    // clip intersection.
    //**************************************************************************
    if (OglIsEnabled(ppdev)) // fast ogl check
    {
        bOglSyncIfGdiInOglWindowStretch(ppdev,psoDst,psoSrc,pco,prclDst,prclSrc);        
    }


    //**********************************************************************
    // Source and Dest rects may be different size.
    // If they are a different size, we need to stretch/squash 'em. 
    // Currently we don't support StretchBlt, which would be the function we'd
    // feed into.
    //**********************************************************************
    SrcWidth  = prclSrc->right  - prclSrc->left;
    SrcHeight = prclSrc->bottom - prclSrc->top;

    if ((SrcWidth != prclDst->right - prclDst->left) ||
        (SrcHeight!= prclDst->bottom - prclDst->top))
        {
        // SCALED_IMAGE class will support this, when we switch to it eventually.
        ppdev->pfnWaitEngineBusy(ppdev);
        return (ppdev->pfnEngAlphaBlend(psoDst,psoSrc,pco,pxlo,prclDst,prclSrc,pBlendObj)); 
        }

    //**********************************************************************
    // Create the src point struct based on the source rect
    //**********************************************************************

    ptlSrc.x = prclSrc->left;
    ptlSrc.y = prclSrc->top;

    DISPDBG((4,"DrvAlphablend:x=%d,y=%d,w=%d,h=%d",ptlSrc.x, ptlSrc.y, SrcWidth, SrcHeight));
    return GenericBitBltAlpha(psoDst,
                              psoSrc,
                              NULL,        // SURFOBJ  *psoMsk
                              pco,
                              pxlo,
                              prclDst,
                              prclSrc,
                              &ptlSrc,
                              NULL,        // POINTL   *pptlMsk
                              NULL,        // BRUSHOBJ *pbo
                              NULL,        // POINTL   *pptlBrush
                              0,           // ROP4     rop4
                              pBlendObj,
                              ppdev
                              );

}


//******************************************************************************
//
//  Function:   DrvTransparentBlt
//
//  Routine Description:
//
//      Implements Bit Block Transfers with transparency (color key)
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL DrvTransparentBlt(
    SURFOBJ     *psoDst,
    SURFOBJ     *psoSrc,
    CLIPOBJ     *pco,
    XLATEOBJ    *pxlo,
    RECTL       *prclDst,
    RECTL       *prclSrc,
    ULONG       iTransColor,
    ULONG       Reserved) 
{
    PDEV*           ppdev;
    POINTL          ptlSrc;
    LONG            SrcWidth;
    LONG            SrcHeight;

    ppdev = (PDEV*) psoDst->dhpdev;
    
    //**************************************************************************
    // ppdev may not be present in the Dst surface object
    // This can happen if the Dst surface object is a standard DIB
    //**************************************************************************

    if (ppdev == NULL || (ppdev->hdevEng != psoDst->hdev)) {

        if (psoSrc != NULL)
            ppdev = (PDEV *)psoSrc->dhpdev;
        if (ppdev == NULL || (ppdev->hdevEng != psoSrc->hdev))
            //******************************************************************
            // This blit call is either screwed up, or we got a
            // memory to memory blit (with no physical device associated).
            // Send the call back to GDI.
            //******************************************************************
                return(EngTransparentBlt(psoDst, psoSrc, pco, pxlo, prclDst, prclSrc, iTransColor, Reserved));

    }


    //************************************************************************** 
    // Need to do smart sync between OpenGL and GDI depending on 
    // clip intersection.
    //**************************************************************************
    if (OglIsEnabled(ppdev)) // fast ogl check
    {
        bOglSyncIfGdiInOglWindowStretch(ppdev,psoDst,psoSrc,pco,prclDst,prclSrc);        
    }


    //**********************************************************************
    // Source and Dest rects may be different size.
    // If they are a different size, we need to stretch/squash 'em. 
    // Currently we don't support StretchBlt, which would be the function we'd
    // feed into.
    //**********************************************************************
    SrcWidth  = prclSrc->right  - prclSrc->left;
    SrcHeight = prclSrc->bottom - prclSrc->top;

    if ((SrcWidth != prclDst->right - prclDst->left) ||
        (SrcHeight!= prclDst->bottom - prclDst->top))
        {
        // SCALED_IMAGE class will support this, when we switch to it eventually.
        ppdev->pfnWaitEngineBusy(ppdev);
        return (ppdev->pfnEngTransparentBlt(psoDst,psoSrc,pco,pxlo,prclDst,prclSrc,iTransColor,Reserved)); 
        }

    //**********************************************************************
    // Create the src point struct based on the source rect
    //**********************************************************************

    ptlSrc.x = prclSrc->left;
    ptlSrc.y = prclSrc->top;

    DISPDBG((4,"DrvTransparentBlt:x=%d,y=%d,w=%d,h=%d",ptlSrc.x, ptlSrc.y, SrcWidth, SrcHeight));
    
    return TransparentBitBlt(psoDst,
                              psoSrc,
                              pco,
                              pxlo,
                              prclDst,
                              prclSrc,
                              &ptlSrc,
                              iTransColor,
                              Reserved,
                              ppdev
                              );
}

#endif // _WIN32_WINNT >= 0x0500

//******************************************************************************
//
//  Function:   DrvCopyBits
//
//  Routine Description:
//
//      Do fast bitmap copies.
//
//      Note that GDI will (usually) automatically adjust the blt extents to
//      adjust for any rectangular clipping, so we'll rarely see DC_RECT
//      clipping in this routine (and as such, we don't bother special casing
//      it).
//
//      I'm not sure if the performance benefit from this routine is actually
//      worth the increase in code size, since SRCCOPY BitBlts are hardly the
//      most common drawing operation we'll get.  But what the heck.
//
//      On the S3 it's faster to do straight SRCCOPY bitblt's through the
//      memory aperture than to use the data transfer register; as such, this
//      routine is the logical place to put this special case.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


BOOL DrvCopyBits(
SURFOBJ*  psoDst,
SURFOBJ*  psoSrc,
CLIPOBJ*  pco,
XLATEOBJ* pxlo,
RECTL*    prclDst,
POINTL*   pptlSrc)

    {
    BOOL        bMore;
    CLIPENUM    ce;
    LONG        c;
    PDEV*       ppdev;
    DSURF*      pdsurfSrc;
    DSURF*      pdsurfDst;
    RECTL       rcl;
    POINTL      ptlSrc;
    ULONG*      ScreenBitmap;
    ULONG*      DstBits;
    ULONG*      SrcBits;
    FNXFER*     pfnXfer;
    BYTE        jClip;
    ULONG       jXlo;

    ULONG       iDstBitmapFormat;
    ULONG       iSrcBitmapFormat;

    SURFOBJ*    SrcPuntPtr;
    SURFOBJ*    DstPuntPtr;
    SURFOBJ*    TempDstSurfObjPtr;
    SURFOBJ*    TempSrcSurfObjPtr;
    BOOL        bret                = FALSE;

    ULONG       bltHeight;
    ULONG       bltWidth;
    ULONG       bltDelta;
    ULONG       SrcOffset;
    ULONG       DstOffset;
    ULONG       SrcStride;
    ULONG       DstStride;

    BYTE*       SrcOffsetPtr;
    BYTE*       DstOffsetPtr;

    ALPHA_SAVE  alphaPreserve=NONE;

    //**************************************************************************
    //  Src and Dst surfaces better be valid.
    //**************************************************************************

    ASSERTDD(((VOID *)psoSrc != NULL), "Didn't expect a NULL source");
    ASSERTDD(((VOID *)psoDst != NULL), "Didn't expect a NULL destination");

    //**************************************************************************
    // Either the source or the destination should have the physical device
    // associated with it. Default to getting the PPDEV from the dst.
    // Otherwise, get it from source
    //**************************************************************************

    ppdev = (PDEV*) psoDst->dhpdev;

    //**************************************************************************
    // ppdev may not be present in the Dst surface object
    // This can happen if the Dst surface object is a standard DIB
    //**************************************************************************

    if (ppdev == NULL)
        {
        //******************************************************************
        // If the PDEV ptr is still NULL, then just kick it back to DIB engine
        //******************************************************************
        ppdev = (PDEV *)psoSrc->dhpdev;
        if (ppdev == NULL)
            //*******************************************
            // No device managed surfaces in the blit.
            //*******************************************

            return(EngCopyBits(psoDst, psoSrc, pco, pxlo, prclDst, pptlSrc));
        }


    //************************************************************************** 
    // Need to do smart sync between OpenGL and GDI depending on 
    // clip intersection.
    //**************************************************************************
    if (OglIsEnabled(ppdev)) // fast ogl check
    {
        bOglSyncIfGdiInOglWindowBlt(ppdev,psoDst,psoSrc,pco,prclDst,pptlSrc);        
    }


    //**************************************************************************
    // Get ptrs to the drivers private device surface structs.
    //**************************************************************************

    pdsurfDst = (DSURF*) psoDst->dhsurf;
    pdsurfSrc = (DSURF*) psoSrc->dhsurf;

    //**************************************************************************
    // Try Signature of ppdev....Only accept PPDEV if it matches our signature
    // (NetMeeting seems to be passing in bogus ppdev values)
    //**************************************************************************

    if (ppdev->NVSignature != NV_SIGNATURE)

        {
        //**********************************************************************
        // The ppdev from the DST did not match...try the ppdev from psosrc
        //**********************************************************************

        ppdev = (PDEV*) psoSrc->dhpdev;

        if (ppdev->NVSignature != NV_SIGNATURE)

            {
            //*******************************************************************
            // We're not able to determine the correct ppdev....just punt
            //*******************************************************************

            goto EngCopyBits_It;
            }

        }

#if _WIN32_WINNT >= 0x0500
    
    //**************************************************************************
    //  Fixes case where driver is called in full screen DOS during STRESS tests.
    //  In this case, GDI is allowed to call us to update DFBs that are stored in
    //  system memory, or offscreen memory. We need to wrap the surface and call
    //  EngBitBlt.
    //**************************************************************************

    if(!ppdev->bEnabled)
        goto EngCopyBits_It;
#endif

    //**************************************************************************
    // Trivially reject calls we don't handle. We don't do color translation or
    // clipping here, nor do we blt between host memory bitmaps.
    //**************************************************************************

    if (pco == NULL)
        jClip = DC_TRIVIAL;
    else
        jClip = pco->iDComplexity;

    if (pxlo == NULL)
        jXlo = XO_TRIVIAL;
    else
        jXlo = pxlo->flXlate;

    //**************************************************************************
    // Check to see if the dest is a device bitmap which has been moved to main
    // memory
    //**************************************************************************

    if ((pdsurfDst != NULL) && (pdsurfDst->dt == DT_DIB))
        {
        psoDst = pdsurfDst->pso;
        if ((pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_DIB))
            {
            //*************************************************************
            // No Device surfaces involved in the blt - check if overlap to
            // avoid GDI bug.
            //*************************************************************
            psoSrc = pdsurfSrc->pso;
            if (pdsurfSrc == pdsurfDst)
                {
                goto CheckDibToDibOverlap;
                }
            }
        goto EngCopyBits_It;     // destination in DIB, no overlap so just punt to GDI
        }

    //**************************************************************************
    // Check and see if we can move system memory DFB's back into
    // offscreen VRAM for better performance. That is, at some point, we put a
    // DFB in system memory. Now we determine that if this DFB is getting
    // used a lot, put it back in offscreen VRAM so we can regain performance.
    //**************************************************************************

    if ( (pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_DIB) )

        {
        //**********************************************************************
        // Here we consider putting a DIB DFB back into off-screen
        // memory. See 'DrvCopyBits' for some more comments on how this
        // moving-it-back-into-off-screen-memory thing works:
        //******************************************************************
        if (pdsurfSrc->iUniq == ppdev->iHeapUniq)

            {
            ASSERTDD(pdsurfSrc->pso->iBitmapFormat == ppdev->iBitmapFormat, "oops");
            //**************************************************************
            // Attempt to move DIB back to offscreen VRAM.
            // This won't necessarily succeed.
            //
            // NOTE: If space doesn't get freed up for a while (heapUniq
            //       remains the same), and this call fails, then cBlt will
            //       wrap around to 0xffffffff, meaning we won't consider
            //       putting this DFB DIB back into offscreen VRAM for a
            //       LONG time, until someone frees up some offscreen memory,
            //       and causes cBlt to be reset to HEAP_COUNT_DOWN.
            //**************************************************************

            if (--pdsurfSrc->cBlt == 0)
                if (bMoveDibToOffscreenDfbIfRoom(ppdev, pdsurfSrc))
                    goto DstDevBmp;

            }
        else
            {
            //**************************************************************
            // Some space was freed up in off-screen memory,
            // so reset the counter for this DFB:
            //**************************************************************
            ASSERTDD(pdsurfSrc->pso->iBitmapFormat == ppdev->iBitmapFormat, "oops");

            pdsurfSrc->iUniq = ppdev->iHeapUniq;
            pdsurfSrc->cBlt  = HEAP_COUNT_DOWN;
            }
        psoSrc = pdsurfSrc->pso;
        }

    //**************************************************************************
    // Get pointers to the Destination and Source 'Bitmaps'
    // We'll use these to determine if the bitmap is ONSCREEN or not.
    //**************************************************************************
DstDevBmp:
    DstBits = (ULONG *)(psoDst->pvBits);
    SrcBits = (ULONG *)(psoSrc->pvBits);
    ScreenBitmap = (ULONG *)(ppdev->pjScreen);

    //**************************************************************************
    // Bitmaps can be one of 4 types, resulting in 16 ways to do blits as follows
    //
    //
    // 1) DIB,  2) DFB (OFFSCREEN),  3) DFB (SYS MEMORY),   4) SCREEN
    //
    //
    //    DIB               to DIB                -> Will not handle - Punt to GDI
    //    DFB (OFFSCREEN)   to DIB                -> Will not handle - Punt to GDI
    //    DFB (SYS MEMORY)  to DIB                -> Will not handle - Punt to GDI
    // SCREEN               to DIB                -> Will not handle - Punt to GDI
    //
    //    DIB               to DFB (OFFSCREEN)    -> Handled
    //    DFB (OFFSCREEN)   to DFB (OFFSCREEN)    -> Handled (rare?)
    //    DFB (SYS MEMORY)  to DFB (OFFSCREEN)    -> Handled (rare?)
    // SCREEN               to DFB (OFFSCREEN)    -> Handled
    //
    //    DIB               to DFB (SYS MEMORY)   -> Will not handle - Punt to GDI
    //    DFB (OFFSCREEN)   to DFB (SYS MEMORY)   -> Will not handle - Punt to GDI ??
    //    DFB (SYS MEMORY)  to DFB (SYS MEMORY)   -> Will not handle - Punt to GDI ??
    // SCREEN               to DFB (SYS MEMORY)   -> Will not handle - Punt to GDI
    //
    //    DIB               to SCREEN             -> Handled
    //    DFB (OFFSCREEN)   to SCREEN             -> Handled
    //    DFB (SYS MEMORY)  to SCREEN             -> Handled
    // SCREEN               to SCREEN             -> Handled
    //
    //
    //      We'll only need to handle the following cases:  (Punt the rest to GDI)
    //      The reason we need to check against ScreenBitmap is that
    //      we've mapped the primary surface using EngCreateBitmap,
    //      so that we can let GDI draw directly to it.  Because of this,
    //      the primary surface is NOT a device managed surface, but rather
    //      a GDI-managed surface.  This means that pdsurf for the
    //      primary surface will be NULL.  So in order to determine
    //      if a surface corresponds to the primary screen surface,
    //      we simply check it's pointer against the ScreenBitmap pointer
    //
    //      Optional:  Since we started with a Dumb Frame Buffer driver using
    //                 EngCreateBitmap (primary surface is GDI-managed, not device
    //                 managed), we check if a standard DIB is the primary surface
    //                 by checking if pvBits == ScreenBitmap.  We can avoid
    //                 this by converting the primary surface to be device managed.
    //                 (EngCreateDeviceSurface). This would result in a more consistent
    //                 driver. (This is the original way it was done in the S3 driver).
    //                 However, we started with a Dumb Frame buffer driver, so
    //                 just leave it for now. It doesn't seem to make that much
    //                 of a difference except for a couple of extra 'if' statements.
    //
    //
    //      We only handle blits where the destination is VRAM, as follows:
    //
    //
    //  1) SCREEN to SCREEN             (CopyBlit)
    //
    //        pdsurfSrc == 0 && SrcBits == ScreenBitmap
    //        pdsurfDst == 0 && DstBits == ScreenBitmap
    //
    //  2) DFB (SYS MEMORY) to SCREEN   (Memory to Screen)
    //
    //        pdsurfSrc != 0 && pdsurfSrc->dt == DT_DIB     && SrcBits != ScreenBitmap;
    //        pdsurfDst == 0 && DstBits == ScreenBitmap
    //
    //  3) DFB (OFFSCREEN) to SCREEN    (CopyBlit)
    //
    //        pdsurfSrc != 0 && pdsurfSrc->dt == DT_SCREEN  &&  SrcBits != ScreenBitmap
    //        pdsurfDst == 0 && DstBits == ScreenBitmap
    //
    //  4) DIB to SCREEN                (Memory to Screen)
    //
    //        pdsurfSrc == 0 && SrcBits != ScreenBitmap;
    //        pdsurfDst == 0 && DstBits == ScreenBitmap
    //
    //  5) SCREEN to DFB (OFFSCREEN)    (CopyBlit)
    //
    //        pdsurfSrc == 0 && SrcBits == ScreenBitmap
    //        pdsurfDst != 0 && pdsurfDst->dt == DT_SCREEN  && DstBits != ScreenBitmap
    //
    //  6) DFB (SYS MEMORY) to DFB (OFFSCREEN)   (Memory to Screen)
    //
    //        pdsurfSrc != 0 && pdsurfSrc->dt == DT_DIB     && SrcBits != ScreenBitmap
    //        pdsurfDst != 0 && pdsurfDst->dt == DT_SCREEN  && DstBits != ScreenBitmap
    //
    //  7) DFB (OFFSCREEN) to DFB (OFFSCREEN)    (CopyBlit)
    //
    //        pdsurfSrc != 0 && pdsurfSrc->dt == DT_SCREEN  && SrcBits != ScreenBitmap
    //        pdsurfDst != 0 && pdsurfDst->dt == DT_SCREEN  && DstBits != ScreenBitmap
    //
    //  8) DIB to DFB (OFFSCREEN)       (Memory to Screen)
    //
    //        pdsurfSrc == 0 && SrcBits != ScreenBitmap
    //        pdsurfDst != 0 && pdsurfDst->dt == DT_SCREEN  DstBits != ScreenBitmap
    //
    //
    //      If the destination is system memory, then we send it back to GDI
    //
    //**************************************************************************

    //**********************************************************************
    // Check if DEST is the SCREEN or MEMORY
    //**********************************************************************
    if (IS_DEV_SURF(psoDst))

        {
        //******************************************************************
        // DEST is SCREEN
        //******************************************************************

        if (IS_DEV_SURF(psoSrc))
            {
            //**************************************************************
            // DEST is SCREEN
            // SOURCE is SCREEN
            // Handle simple SCREEN to SCREEN blit with ROP=SRCCOPY
            //**************************************************************

            if ((jClip != DC_TRIVIAL) || (jXlo != XO_TRIVIAL))
                goto DrvBlt_It;

            //**************************************************************
            // Linear memory management
            // Set linear offset and stride for source buffer
            //**************************************************************

            if (pdsurfSrc != 0)     // source is offscreen bitmap
                {
                SrcOffset = (ULONG)((BYTE *)(pdsurfSrc->LinearPtr) - ppdev->pjFrameBufbase);
                SrcStride = pdsurfSrc->LinearStride;
                }
            else
                {
                SrcOffset = ppdev->ulPrimarySurfaceOffset;
                SrcStride = ppdev->lDelta;
                }

            (ppdev->pfnSetSourceBase)(ppdev,SrcOffset,SrcStride);

            //**************************************************************
            // Set linear offset and stride for destination buffer
            //**************************************************************

            if (pdsurfDst != 0)     // dest is offscreen bitmap
                {
                DstOffset = (ULONG)((BYTE *)(pdsurfDst->LinearPtr) - ppdev->pjFrameBufbase);
                DstStride = pdsurfDst->LinearStride;
                }
            else
                {
                DstOffset = ppdev->ulPrimarySurfaceOffset;
                DstStride = ppdev->lDelta;
                }

            (ppdev->pfnSetDestBase)(ppdev, DstOffset , DstStride);

            //**************************************************************
            // Do the blit
            //**************************************************************
#if (_WIN32_WINNT >= 0x0500)
            if (ppdev->iBitmapFormat == BMF_32BPP)
                {
                // 32bpp src copy - upper 8 bits may be alpha
                // Preserve alpha since this bitmap may be the source on a future DrvAlphaBlend
                alphaPreserve = COPYBLIT;
                NV4DmaPushAlphaPreserve(ppdev, TRUE, FALSE); // Start=TRUE, FromCpu=TRUE
                }
#endif // NT5
            ppdev->pfnCopyBlt(ppdev, 1, prclDst, 0xcccc, pptlSrc, prclDst, NULL);

            bret = TRUE;
            goto Exit;
            }

        else

            {

            //**************************************************************
            // DEST is SCREEN
            // SOURCE is MEMORY
            // Handle simple MEMORY to SCREEN blit with ROP=SRCCOPY
            //**************************************************************

            //**************************************************************
            // Set linear offset and stride for destination buffers
            //**************************************************************

            if (pdsurfDst != 0)     // dest is offscreen bitmap
                {
                DstOffset = (ULONG)((BYTE *)(pdsurfDst->LinearPtr) - ppdev->pjFrameBufbase);
                DstStride = pdsurfDst->LinearStride;
                }
            else
                {
                DstOffset = ppdev->ulPrimarySurfaceOffset;
                DstStride = ppdev->lDelta;
                }

            (ppdev->pfnSetDestBase)(ppdev, DstOffset , DstStride);

            iSrcBitmapFormat = psoSrc->iBitmapFormat;
            iDstBitmapFormat = ppdev->iBitmapFormat;
            bltHeight = prclDst->bottom - prclDst->top;

            if ((jXlo & XO_TABLE)               &&
                (iSrcBitmapFormat == BMF_8BPP)  &&
                (jClip == DC_TRIVIAL)           &&
                (ppdev->DmaPushEnabled2D)       &&
                (bltHeight == 1))
                {

                if ((iDstBitmapFormat == BMF_32BPP))
                    {
                    ppdev->pfnFastXfer8to32(ppdev, prclDst, psoSrc, pptlSrc, pxlo);
                    bret = TRUE;
                    goto Exit;
                    }

                if ((iDstBitmapFormat == BMF_16BPP))
                    {
                    ppdev->pfnFastXfer8to16(ppdev, prclDst, psoSrc, pptlSrc, pxlo);
                    bret = TRUE;
                    goto Exit;
                    }

                goto DrvBlt_It;
                }

            if (((iSrcBitmapFormat == BMF_8BPP)  ||
                 (iSrcBitmapFormat == BMF_4BPP)) &&
                ((iDstBitmapFormat == BMF_32BPP) ||
                 (iDstBitmapFormat == BMF_16BPP)) &&
                 (jXlo & XO_TABLE))
                {
                //******************************************************
                // For small bitmaps, it is faster to do a manual indexed
                // image transfer.
                //******************************************************

                bltWidth = prclDst->right - prclDst->left;

                //
                // pfnIndexedImage will download 256 dwords palette and the image itself
                // 16x16 in 8bpp = 64 dwords
                // 16x16 in 4bpp = 32 dwords
                // pfnXferXtoYY will xlate in sw and put the xlated data to the pushbuffer
                // We need to count the palette overhead to make a decision
                // suggestion: if ((bltWidth * bltHeight) <= 320)
                if ((bltWidth <= 16) && (bltHeight <= 16))
                    {
                    if (iDstBitmapFormat == BMF_16BPP)
                        {
                        if (iSrcBitmapFormat == BMF_4BPP)
                            pfnXfer = ppdev->pfnXfer4to16bpp;
                        else
                            pfnXfer = ppdev->pfnXfer8to16bpp;
                        }
                    else
                        {
                        if (iSrcBitmapFormat == BMF_4BPP)
                            pfnXfer = ppdev->pfnXfer4to32bpp;
                        else
                            pfnXfer = ppdev->pfnXfer8to32bpp;
                        }
                    }
                else
                    {
                    //******************************************************
                    // Use hardware Indexed Image to transfer from 4bpp or 8bpp
                    // source bitmap to 16bpp or 32bpp destination bitmap
                    //******************************************************

                    pfnXfer = ppdev->pfnIndexedImage;
                    }

                if (pfnXfer == NULL)
                    goto DrvBlt_It;

                if (jClip == DC_TRIVIAL)
                    pfnXfer(ppdev, 1, prclDst, 0xcccc, psoSrc, pptlSrc, prclDst, pxlo, 0, NULL);
                else if (jClip == DC_RECT)
                    {
                    if (bIntersect(prclDst, &pco->rclBounds, &rcl))
                        pfnXfer(ppdev, 1, &rcl, 0xcccc, psoSrc, pptlSrc, prclDst, pxlo, 0, NULL);
                    }
                else
                    {

            //************************************************************
            // Clip region must be enumerated
            //************************************************************

                    CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

                    do  {
                        bMore = CLIPOBJ_bEnum(pco, sizeof(ce),
                                           (ULONG*) &ce);

                        c = cIntersect(prclDst, ce.arcl, ce.c);

                        if (c != 0)
                            {
                            pfnXfer(ppdev, c, ce.arcl, 0xcccc, psoSrc,
                                    pptlSrc, prclDst, pxlo, 0, NULL);
                            }

                        } while (bMore);
                    }
                }
            //**************************************************************
            // If source format == dest format, do straight mem to screen BLT.
            //**************************************************************

            else
                {
                if ((jClip != DC_TRIVIAL) ||
                    (iSrcBitmapFormat != iDstBitmapFormat) ||
                    (jXlo != XO_TRIVIAL))

                    goto DrvBlt_It;

#if (_WIN32_WINNT >= 0x0500)
                if (iDstBitmapFormat == BMF_32BPP)
                    {
                    // Preserve alpha since this bitmap may be the source on a future DrvAlphaBlend
                    alphaPreserve = IMAGEFROMCPU;
                    NV4DmaPushAlphaPreserve(ppdev, TRUE, TRUE); // Start=TRUE, FromCpu=TRUE
                    }
#endif // NT5

                (ppdev->pfnMemToScreenBlt)(ppdev, psoSrc, pptlSrc, prclDst);
                }

            bret = TRUE;
            goto Exit;

            }

        }

    else

        {

        //******************************************************************
        // DEST is MEMORY
        //******************************************************************

        if (IS_DEV_SURF(psoSrc))
            {
            //**************************************************************
            // DEST is MEMORY
            // SOURCE is SCREEN
            // Handle simple SCREEN to MEMORY blit with ROP=SRCCOPY
            //**************************************************************
            goto DrvBlt_It;
            }

        else
            {
            //**************************************************************
            // DEST is MEMORY
            // SOURCE is MEMORY
            // Should never get here since we trivially rejected this case.
            // But if we do get here, punt.
            //**************************************************************
            goto EngCopyBits_It;
            }

        }

    //**************************************************************************
    // If DrvCopyBits couldn't handle it, just send it to DrvBitBlt.
    // A DrvCopyBits is after all just a simplified DrvBitBlt:
    //**************************************************************************

DrvBlt_It:
    bret = GenericBitBltAlpha(psoDst,
                              psoSrc,
                              NULL,
                              pco,
                              pxlo,
                              prclDst,
                              NULL,        // RECTL *prclSrc
                              pptlSrc,
                              NULL,
                              NULL,
                              NULL,
                              0x0000CCCC,
                              NULL,         // BLENDOBJ *pBlendObj
                              ppdev
                              );
    goto Exit;

EngCopyBits_It:

    //**************************************************************************
    // GDI only knows how to write to standard DIB surfaces.
    // That means, if the SOURCE or DEST is a DFB, we need to create a standard
    // DIB SurfObj in place of the device managed surface (DFB) object in order
    // to let GDI do the blit.
    //
    // Essentially, there are 4 types of SURFOBJS we'll encounter.
    //
    //      1) System Memory SurfObj (Standard DIB)
    //          We can pass this surfobj to GDI as is.
    //
    //      2) DFB - exists in offscreen VRAM memory
    //          Need to create a standard DIB SurfObj to let GDI draw to this surface
    //
    //      3) DFB - exists in system memory
    //          Need to create a standard DIB surfObj to let GDI draw to this surface
    //
    //      4) Primary Surface VRAM (Standard DIB format)
    //          We can pass this surfobj to GDI as is.
    //
    //          CONDITION                                           SURFOBJ TYPE
    //          ---------                                           ------------
    //
    // (pdsurf != NULL)  &&  (pdsurf->dt == DT_DIB)         --> This is a DFB in system memory
    // (pdsurf != NULL)  &&  (pdsurf->dt == DT_SCREEN)      --> This is a DFB in offscreen VRAM memory
    //
    // (pdsurf == NULL)  &&  (pvBits != ppdev->pjScreen)    --> Standard DIB
    // (pdsurf == NULL)  &&  (pvBits == ppdev->pjScreen)    --> Primary Screen Surface
    //**************************************************************************


    //**************************************************************************
    // Default to using the original, un-modified SURFOBJ's that were passed to us.
    //**************************************************************************

    TempSrcSurfObjPtr = psoSrc;
    TempDstSurfObjPtr = psoDst;

    //**************************************************************************
    // Get pointers to the 'wrapper' surfaces that we use to draw to the
    // screen surface if necessary.  These surfaces are locked.
    // (The surfaces that we pass to GDI MUST be locked).
    // See EnableOffscreenHeap for more information.
    //**************************************************************************

    SrcPuntPtr = ppdev->psoPunt;
    DstPuntPtr = ppdev->psoPunt2;

    //**************************************************************************
    // Check if source bitmap is a DFB.  If so, create a temporary surfobj
    // (mapped to the screen) that GDI can read from
    //**************************************************************************

    if (pdsurfSrc !=0 )
        {
        SrcPuntPtr->dhsurf      = 0;                    // Behave like a standard DIB
        if (pdsurfSrc->dt == DT_SCREEN)
            {
            SrcPuntPtr->sizlBitmap  = pdsurfSrc->sizl;
            SrcPuntPtr->cjBits  = pdsurfSrc->LinearStride * pdsurfSrc->sizl.cy;
            SrcPuntPtr->pvBits  = pdsurfSrc->LinearPtr;
            SrcPuntPtr->pvScan0 = pdsurfSrc->LinearPtr;
            SrcPuntPtr->lDelta  = pdsurfSrc->LinearStride;
            SrcPuntPtr->iBitmapFormat = ppdev->iBitmapFormat;
            }
        else
            {
            //**********************************************************************
            // This is a DFB which is stored in main memory as a DIB
            //**********************************************************************
            SrcPuntPtr->sizlBitmap  = pdsurfSrc->pso->sizlBitmap;
            SrcPuntPtr->cjBits      = pdsurfSrc->pso->lDelta * pdsurfSrc->pso->sizlBitmap.cy;
            SrcPuntPtr->pvBits  = pdsurfSrc->pso->pvBits;
            SrcPuntPtr->pvScan0 = pdsurfSrc->pso->pvScan0;
            SrcPuntPtr->lDelta  = pdsurfSrc->pso->lDelta;
            SrcPuntPtr->iBitmapFormat = pdsurfSrc->pso->iBitmapFormat;
            }

        //**********************************************************************
        // Use the 'wrapper' surface, instead of the original DFB surfobj for GDI
        //**********************************************************************

        TempSrcSurfObjPtr       = SrcPuntPtr;
        }


    //**************************************************************************
    // Check if destination bitmap is a DFB.  If so, create a temporary surfobj
    // (mapped to the screen) that GDI can write on
    //**************************************************************************

    if (pdsurfDst !=0 )
        {
        DstPuntPtr->dhsurf      = 0;                    // Behave like a standard DIB
        if (pdsurfDst->dt == DT_SCREEN)
            {
            DstPuntPtr->sizlBitmap  = pdsurfDst->sizl;
            DstPuntPtr->cjBits  = pdsurfDst->LinearStride * pdsurfDst->sizl.cy;
            DstPuntPtr->pvBits  = pdsurfDst->LinearPtr;
            DstPuntPtr->pvScan0 = pdsurfDst->LinearPtr;
            DstPuntPtr->lDelta  = pdsurfDst->LinearStride;
            DstPuntPtr->iBitmapFormat = ppdev->iBitmapFormat;
            }
        else
            {
            //**********************************************************************
            // This is a DFB which is stored in main memory as a DIB
            //**********************************************************************
            DstPuntPtr->sizlBitmap  = pdsurfDst->pso->sizlBitmap;
            DstPuntPtr->cjBits      = pdsurfDst->pso->lDelta * pdsurfDst->pso->sizlBitmap.cy;
            DstPuntPtr->pvBits  = pdsurfDst->pso->pvBits;
            DstPuntPtr->pvScan0 = pdsurfDst->pso->pvScan0;
            DstPuntPtr->lDelta  = pdsurfDst->pso->lDelta;
            DstPuntPtr->iBitmapFormat = pdsurfDst->pso->iBitmapFormat;
            }

        //**********************************************************************
        // Use the 'wrapper' surface, instead of the original DFB surfobj for GDI
        //**********************************************************************

        TempDstSurfObjPtr       = DstPuntPtr;
        }
    
    //**************************************************************************
    // Let GDI do the blit, even our DFB surfobjs!
    //**************************************************************************
    ppdev->pfnWaitEngineBusy(ppdev);
    bret = (ppdev->pfnEngCopyBits(TempDstSurfObjPtr, TempSrcSurfObjPtr, pco, pxlo, prclDst, pptlSrc));
    goto Exit;

    //**************************************************************************
    //
    // There is a bug in GDI that shows up with MapView (a Japanese app). The
    // problem ends up occuring where an offscreen bitmap is punted to system
    // memory. Then the app does a copy from one part of the bitmap to the other.
    // In some cases, there is overlap and the copy to the destination stomps on
    // the source. Working around that problem here.
    //
    // On entry, pdsurfDst = psoDst->dhsurf
    //           pdsurfSrc = psoSrc->dhsurf
    //
    //**************************************************************************

CheckDibToDibOverlap:

    bltDelta = pdsurfDst->pso->lDelta;

    DstOffsetPtr = SrcOffsetPtr = pdsurfSrc->pso->pvScan0;

    SrcOffsetPtr += (((pptlSrc->x) << (ppdev->cjPelSize/2)) + (pptlSrc->y * bltDelta)) ;
    DstOffsetPtr += (((prclDst->left) << (ppdev->cjPelSize/2)) + (prclDst->top * bltDelta)) ;

    //*****************************************************************
    // Check if Src DIB offsetis before Dest DIB. If so, then we might
    // need to copy this DIB from bottom to top to take care of the
    // overlapping case.
    //*****************************************************************

    if ((SrcOffsetPtr < DstOffsetPtr) &&
        (jClip == DC_TRIVIAL) && (jXlo == XO_TRIVIAL))
        {
        ULONG       bltHeight;
        ULONG       bltWidth;
        ULONG       bitmapEnd;

        bltHeight = prclDst->bottom - prclDst->top;
        bltWidth  = (prclDst->right  - prclDst->left) << (ppdev->cjPelSize/2);
        bitmapEnd = ((bltHeight-1)*bltDelta) + bltWidth;

        //*****************************************************************
        // Check if end of Src DIB overlaps start of Dest DIB. If so, then
        // we handle this case since there is a bug in GDI.
        //*****************************************************************

        if ((SrcOffsetPtr+bitmapEnd) > DstOffsetPtr)
            {
            //*****************************************************************
            // Copy from Src DIB to Dest DIB taking care of overlapping
            //*****************************************************************
            SrcOffsetPtr += bitmapEnd;                 // point to end of src bitmap
            DstOffsetPtr += bitmapEnd;                 // point to end of dest bitmap

            CopyDIBBitmaps (pdsurfDst,
                            DstOffsetPtr,
                            SrcOffsetPtr,
                            bltHeight,
                            bltWidth);

            bret = TRUE;
            goto Exit;
            }
        }
    goto EngCopyBits_It;


Exit:

#if (NVARCH >= 0x4)
    // MSchwarzer 10/04/2000 need this extrypoint for fixes in overlaymode
    if(   (bret)                                            // CopyBit was successful
#if _WIN32_WINNT >= 0x0500
        &&(ppdev->bEnabled)                                 // see above
#endif
        &&(ppdev->bOglOverlaySurfacesCreated)               // OverlayMode is enabled
        &&(XO_TRIVIAL == jXlo )                             // no color translation
        &&(psoDst->iBitmapFormat == ppdev->iBitmapFormat)   // Dst has same format than Primary
      )
    {
        // 1st check for primary to mem/offscreen blit
        if(   (STYPE_DEVICE == psoSrc->iType)
            &&(STYPE_DEVICE != psoDst->iType)
            &&(NULL         == pco  )          // no clipping => need to check this for W2K fading !!
           )
        {
            DISPDBG((20, "PM:psoSrc: 0x%x, psoDst: 0x%x, pco: 0x%x",psoSrc,psoDst,pco));                

            // Check for intersection between source and any overlaywindow and
            // copy mainplanefront of these areas into destination bitmap
            if( bCopyIntersectingMainPlaneFrontToPSO( ppdev, psoSrc, psoDst, prclDst, pptlSrc) )
            {
                // remember Destination Bitmap for restauration purposes
                if(!bAddPointerToList( &ppdev->plSavedPopupMenus, (PULONG)psoDst ))
                {
                    if( bRemoveFirstElementFromList( &ppdev->plSavedPopupMenus ) )
                    {
                        if(!bAddPointerToList( &ppdev->plSavedPopupMenus, (PULONG)psoDst ))
                            DISPDBG((0, "too many popup windows at one time to store !"));                
                    }
                }
            }

        }
        // 2nd check if it was a restauration blit ( mem/offsreen to primary )
        else if(   (STYPE_DEVICE == psoDst->iType)
                 &&(bIsPointerInList(&ppdev->plSavedPopupMenus,(PULONG)psoSrc))
               )
        {
            DISPDBG((20, "MP:psoSrc: 0x%x, psoDst: 0x%x, pco: 0x%x",psoSrc,psoDst,pco));                

            // Restore color format before entering the merge blit. The Colorkey blit 
            // needs the right color format set for the primary surface.
            if (alphaPreserve != NONE)
            {
                NV4DmaPushAlphaPreserve(ppdev, FALSE, alphaPreserve!=COPYBLIT); // Start=FALSE; TRUE if FromCPU else FALSE
                alphaPreserve = NONE;
            }

            // we need to to a mergeblit on any parts of OverlayWindows
            // affected by this blit
            if( !bDoMergeBlitOnIntersectingRectangle(ppdev, prclDst) )
                DISPDBG((0, "bDoMergeBlitOnIntersectingRectangle failed "));                
            if(!bRemovePointerFromList( &ppdev->plSavedPopupMenus, (PULONG)psoSrc ))
               DISPDBG((0, "Could not remove popupmenue from psolist! !"));                
        }
    }
#endif// #if (NVARCH >= 0x4)

    if (alphaPreserve != NONE)
        {
        NV4DmaPushAlphaPreserve(ppdev, FALSE, alphaPreserve!=COPYBLIT); // Start=FALSE; TRUE if FromCPU else FALSE
        }


    return bret;
    }


//*****************************************************************************
//
// There is a bug in GDI that shows up with MapView (a Japanese app). The
// problem ends up occuring where an offscreen bitmap is punted to system
// memory. Then the app does a copy from one part of the bitmap to the other.
// In some cases, there is overlap and the copy to the destination stomps on
// the source. The ntoskrnl!movemem routine checks for overlapping one scanline
// at a time, but the calling code does not take care of the bitmaps overlapping
// correctly. This routine copies the bitmap when a source offset is less than
// the destination offset and takes care of the overlapping.
//
// Note: Performance is not an issue with this bug - this code will almost never
//       execute. The only way I can reproduce this is with MapView on NV3 (with
//       4M or less offscreen memory) at 800x600x32 or 1024x768x32.
//*****************************************************************************

VOID CopyDIBBitmaps (
DSURF*      pdsurf,
BYTE*       DstOffset,      // points to end of DIB bitmap
BYTE*       SrcOffset,      // points to end of DIB bitmap
ULONG       bltHeight,
ULONG       bltWidth)

    {

    LONG    i;
    LONG    cjMiddle;
    LONG    culMiddle;
    LONG    cjStartPhase;
    LONG    cjEndPhase;
    LONG    lDelta;

    lDelta    = pdsurf->pso->lDelta;

    //******************************************************************
    // Align the copy according to the DEST.
    //
    // Careful:  Phase is 0 if on a byte 0 boundary.
    //           Phase is 3 if on a byte 1 boundary.
    //           Phase is 2 if on a byte 2 boundary.
    //           Phase is 1 if on a byte 3 boundary.
    //
    // cjStartPhase is the number of bytes we need to output first, before
    // we can start outputting the rest of the bytes.
    //******************************************************************

    // We ignore upper dword since we only care about bottom 2 bits
    cjStartPhase = (ULONG)((ULONG_PTR)DstOffset & 3);

    //******************************************************************
    // Calculate Number of bytes remaing after outputting the 'starting'
    // phase bytes. We'll later calculate the final culMiddle and cjEndPhase
    // from cjMiddle.
    //**************************************************************************

    cjMiddle = bltWidth - cjStartPhase;

    //**************************************************************************
    // The following condition always holds true:
    //
    //      cjStartPhase + (culMiddle * 4) +cjEndPhase  ==  cjScan
    //
    // EXCEPT for the cases where cjMiddle < 0.  So we must special case them.
    // Here, we'll just get rid of the starting phase.  The end result is that
    // for these case, bytes will only be output in the 'cjEndPhase'.
    //
    //**************************************************************************

    if (cjMiddle < 0)
        {
        cjStartPhase = 0;
        cjMiddle     = bltWidth;
        }

    //**************************************************************************
    // Update the number of bytes we need to add to get to the next scanline,
    // after copying 'cjScan' bytes.
    //**************************************************************************

    lDelta -= bltWidth;

    //**************************************************************************
    // EndPhase will be copied in bytes.
    // MiddlePhase will always be copied in DWORDS.
    // See description up above.
    //**************************************************************************

    cjEndPhase = cjMiddle & 3;
    culMiddle  = cjMiddle >> 2;

    //**********************************************************************
    // Align to the destination (implying that the source may be
    // unaligned).
    // Copy scanlines at a time.
    //**********************************************************************

    for (; bltHeight > 0; bltHeight--)

        {
        //******************************************************************
        // Copy Bytes at a time
        //******************************************************************

        for (i = cjStartPhase; i > 0; i--)
            {
            *(--DstOffset) = *(--SrcOffset);
            }

        //******************************************************************
        // Copy Dwords at a time
        //******************************************************************

        for (i = culMiddle; i > 0; i--)
            {
            *(--((ULONG*)DstOffset)) = *(--((ULONG UNALIGNED *)SrcOffset));
            }

        //******************************************************************
        // Copy Bytes at a time
        //******************************************************************

        for (i = cjEndPhase; i > 0; i--)
            {
            *(--DstOffset) = *(--SrcOffset);
            }

        SrcOffset -= lDelta;
        DstOffset -= lDelta;
        }

    }

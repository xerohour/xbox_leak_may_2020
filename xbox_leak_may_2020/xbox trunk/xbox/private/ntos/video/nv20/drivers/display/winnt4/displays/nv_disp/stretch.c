//******************************Module*Header***********************************
//
// Module Name: stretch.c
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
#include "oglsync.h"

#define STRETCH_MAX_EXTENT 32767

typedef DWORDLONG ULONGLONG;

BOOL DrvStretchBlt(
SURFOBJ*            psoDst,
SURFOBJ*            psoSrc,
SURFOBJ*            psoMsk,
CLIPOBJ*            pco,
XLATEOBJ*           pxlo,
COLORADJUSTMENT*    pca,
POINTL*             pptlHTOrg,
RECTL*              prclDst,
RECTL*              prclSrc,
POINTL*             pptlMsk,
ULONG               iMode)

    {
    DSURF*  pdsurfSrc;
    DSURF*  pdsurfDst;
    PDEV*   ppdev;

    RECTL       rclClip;
    RECTL*      prclClip;
    ULONG       DstWidth;
    ULONG       DstHeight;
    ULONG       SrcWidth;
    ULONG       SrcHeight;
    BOOL        bMore;
    CLIPENUM    ce;
    LONG        c;
    LONG        i;
    FNXFER*         pfnXfer;
    POINTL      ptlSrc;

    //**************************************************************************
    // GDI guarantees us that for a StretchBlt the destination surface
    // will always be a device surface, and not a DIB:
    //**************************************************************************

    //**************************************************************************
    // For NV: Our primary surface is NOT a device surface.
    // It is a GDI managed, standard DIB surface...
    //**************************************************************************

    pdsurfSrc = NULL;
    pdsurfDst = NULL;

    if ((VOID *)psoSrc != NULL)
        pdsurfSrc = (DSURF*) psoSrc->dhsurf;

    if ((VOID *)psoDst != NULL)
        pdsurfDst = (DSURF*) psoDst->dhsurf;


    ppdev = (PDEV*) psoDst->dhpdev;


    //************************************************************************** 
    // Need to do smart sync between OpenGL and GDI depending on 
    // clip intersection.
    //**************************************************************************
    if (OglIsEnabled(ppdev)) // fast ogl check
    {
        bOglSyncIfGdiInOglWindowStretch(ppdev,psoDst,psoSrc,pco,prclDst,prclSrc);        
    }


    //**************************************************************************
    // NV: This function is NOT currently implemented/hooked
    // TODO:  Add punt/wrapper functionality same as BitBlt!
    //**************************************************************************

    return(TRUE);




    //**************************************************************************
    // It's quicker for GDI to do a StretchBlt when the source surface
    // is NOT a device-managed surface, because then it can directly
    // read the source bits without having to allocate a temporary
    // buffer and call DrvCopyBits to get a copy that it can use.
    //
    // NV:  This works much better with our driver since we use GDI-MANAGED
    //      surfaces.  No need to call DrvCopyBits so performance is better.
    //      (We can directly read the source bits, no problem)
    //**************************************************************************

    //**************************************************************************
    // Currently, we only support the following type of stretch DIB
    //      - No mask
    //      - Source is a DIB (STYPE_BITMAP), 1 bpp with pxlo->flXlate = XO_TABLE
    //          which means we get 1 colors, one for foreground and one for background
    //      - Destination is screen
    //      - 1 to 1 stretching (no stretching)
    //      - Clipping is DC_TRIVIAL
    //**************************************************************************

    if (    (psoSrc->iType == STYPE_BITMAP) &&
            (psoMsk == NULL) &&
            (pxlo != NULL) &&
            (pxlo->flXlate & XO_TABLE) &&
            (pxlo->cEntries == 2) &&
            (psoSrc->iBitmapFormat == BMF_1BPP) &&
            (pco !=NULL) &&
            (pco->iDComplexity == DC_TRIVIAL)
        )

        {
        DstWidth  = prclDst->right - prclDst->left;
        DstHeight = prclDst->bottom - prclDst->top;

        SrcWidth  = prclSrc->right - prclSrc->left;
        SrcHeight = prclSrc->bottom - prclSrc->top;

        //**********************************************************************
        // Right now, we don't really do stretching.
        // We only support 1 to 1 copies with a monochrome source bitmap.
        // DeviceBitmapBit calls come thru here.
        //**********************************************************************

        if ( (DstWidth == SrcWidth) &&
             (DstHeight == SrcHeight))
            {

            pfnXfer = ppdev->pfnXfer1bpp;

            //******************************************************************
            // Specify 1 'clip' rectangle (same as original destination rectangle)
            // Manually specify 0xCCCC for Source Copy ROP
            // PatternColor doesn't matter in this case since this is a SRCCOPY rop
            //   (no pattern involved) so we'll just send 0x00000000
            // Send the origin of the source rectangle in ptlSrc..for now
            // we just care about the origin since we're not doing stretching
            //******************************************************************

            ptlSrc.x = prclSrc->left;
            ptlSrc.y = prclSrc->top;

            pfnXfer(ppdev, 1, prclDst, 0xCCCC, psoSrc, &ptlSrc, prclDst, pxlo,0x00000000,NULL);
            return(TRUE);
            }

        else
            {
            //******************************************************************
            // Eventually, we'll add the stretch functionality
            // For now, just fall through to GDI.
            //******************************************************************
            }

        }

    //**************************************************************************
    // Send call back to GDI, we didn't handle it.
    //**************************************************************************

    //**************************************************************************
    // TODO:  Add punt/wrapper functionality same as BitBlt!
    //**************************************************************************

    return(EngStretchBlt(psoDst, psoSrc, psoMsk, pco, pxlo, pca, pptlHTOrg,
                         prclDst, prclSrc, pptlMsk, iMode));
    }




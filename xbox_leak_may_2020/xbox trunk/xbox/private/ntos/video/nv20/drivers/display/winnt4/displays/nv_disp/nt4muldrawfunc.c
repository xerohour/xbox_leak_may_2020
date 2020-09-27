//************************** Module Header *************************************
//                                                                             *
//  Module Name: nt4MulDrawFunc.c                                                   *
//                                                                             *
//  This module contains the functions that belong to the multi device wrapper *
//                                                                             *
//   Copyright (C) 2000 NVidia Corporation. All Rights Reserved.               *
//                                                                             *
//******************************************************************************


#include "precomp.h"
#include "driver.h"
#include "RectUtils.h"

#include "nt4multidev.h"

#ifdef NT4_MULTI_DEV

//******************************************************************************
//
//  MulRealizeBrush
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulRealizeBrush(BRUSHOBJ* pbo, SURFOBJ*  psoTarget,
                    SURFOBJ*  psoPattern,   SURFOBJ*  psoMask,
                    XLATEOBJ* pxlo, ULONG     iHatch)
{
    PPDEV ppdev = (PPDEV)psoTarget->dhpdev; 
    BOOL  bRet  = FALSE;

    DISPDBG((100, "MulRealizeBrush >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvRealizeBrush(pbo, psoTarget, psoPattern, psoMask, pxlo, iHatch);
    }
    else
    {
        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            ASSERT(pmdev->abdDevices[ulDev].pso);
            bRet = DrvRealizeBrush(pbo, pmdev->abdDevices[ulDev].pso, psoPattern,   psoMask, pxlo,  iHatch);
        }
    }

    DISPDBG((100, "<<< MulRealizeBrush"));
    return bRet;
}


//******************************************************************************
//
//  MulSaveScreenBits
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//  For now we don't have implemented SaveScreenBits for Multimon case
//  We may need to do it if quadbuffered stereo and opengl overlay are
//  needed in multimon case.
//  That means to return an array of all collected ulRets and to take thís array
//  as input parameter for the restore cases
//
//******************************************************************************
ULONG_PTR APIENTRY MulSaveScreenBits(
    SURFOBJ   *pso,
    ULONG      iMode,
    ULONG_PTR  ident,
    RECTL     *prcl )
{
    PPDEV      ppdev = (PPDEV)pso->dhpdev; 
    ULONG_PTR  ulRet = FALSE;

    DISPDBG((100, "MulSaveScreenBits >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        ulRet = DrvSaveScreenBits(pso, iMode, ident, prcl);
    }
#if 0 // not implemented yet
    else
    {
        switch(iMode) 
        {
        case SS_SAVE:
            // create array to contain all ulRets, call all DrvSaveScreenBits and return array
            break;
        case SS_RESTORE:
            // free array ( which is given by ident )
            break;
        case SS_FREE:
            // free array ( which is given by ident )
            break;
        default:
            ASSERT(FALSE);
            break;
        }
    }
#endif// #if 0

    DISPDBG((100, "<<< MulSaveScreenBits"));
    return ulRet;
}


//******************************************************************************
//
//  MulPaint
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulPaint(  SURFOBJ*  pso,
                CLIPOBJ*  pco,
                BRUSHOBJ* pbo,
                POINTL*   pptlBrushOrg,
                MIX       mix)
{
    PPDEV ppdev = (PPDEV)pso->dhpdev; 
    BOOL  bRet  = FALSE;

    DISPDBG((100, "MulPaint >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvPaint(pso, pco, pbo, pptlBrushOrg, mix);
    }
    else
    {
        PMDEV       pmdev = ppdev->pmdev;  
        ULONG       ulDev;

        for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
        {
            BOARDDESC *pBoardDesc;
            ASSERT(pmdev->abdDevices[ulDev].pso);
            ASSERT(pco);

            // adjust clipobject for each single board and pass it to the coresponding board
            pBoardDesc = &pmdev->abdDevices[ulDev];

            if (DC_COMPLEX == pco->iDComplexity)
            {
                BOOL        bMoreClip;
                CLIPENUM    ce;
                LONG        l;

                CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

                do  
                {
                    bMoreClip = CLIPOBJ_bEnum(pco, sizeof(ce), (ULONG*) &ce);
                    for (l = 0; l < ce.c; l++)
                    {
                        if (bRclIntersect(&pBoardDesc->pco->rclBounds, &ce.arcl[l], &pBoardDesc->rclBoard))
                        {
                            vRclSubOffset(&pBoardDesc->pco->rclBounds,
                                          &pBoardDesc->pco->rclBounds,
                                          pBoardDesc->rclBoard.left, 
                                          pBoardDesc->rclBoard.top);
                            pBoardDesc->pco->iDComplexity = DC_RECT;
                            bRet = DrvPaint( pBoardDesc->pso, pBoardDesc->pco, pbo, pptlBrushOrg, mix);
                            ASSERT(bRet);
                        }
                    }
                }
                while (bMoreClip);
            }
            else
            {
                if (bRclIntersect(&pBoardDesc->pco->rclBounds, &pco->rclBounds, &pBoardDesc->rclBoard))
                {
                    vRclSubOffset(&pBoardDesc->pco->rclBounds,
                                  &pBoardDesc->pco->rclBounds,
                                  pBoardDesc->rclBoard.left, 
                                  pBoardDesc->rclBoard.top);
                    pBoardDesc->pco->iDComplexity = DC_RECT;
                    bRet = DrvPaint( pBoardDesc->pso, pBoardDesc->pco, pbo, pptlBrushOrg, mix);
                    ASSERT(bRet);
                }
            }
        }//for (ulDev = 0; ulDev < pmdev->ulNumDevicesActive; ulDev++)
    }

    DISPDBG((100, "<<< MulPaint"));
    return bRet;
}


//******************************************************************************
//
//  bTranslateBitBltParams
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL _inline bTranslateBitBltParams(IN  RECTL           *prclDstBoard,
                                    IN  RECTL           *prclSrcBoard,
                                    IN  ROP4            rop4,
                                    IN  RECTL           *prclClip,
                                    IN  RECTL           *prclDst,
                                    IN  POINTL          *pptlSrc,
                                    IN  POINTL          *pptlMask,
                                    IN  POINTL          *pptlBrush,
                                    OUT BLT_PARAM       *pBltParam)
{
    BOOL bRet = FALSE;
    SIZEL   sizSrcMinusDst;
    SIZEL   sizMaskMinusDst;
    RECTL   rclSrc;
    RECTL   rclMask;

    ASSERT(pBltParam);
    ASSERT(prclClip);
    ASSERT(prclDst);

    pBltParam->rclClip  = *prclClip;
    pBltParam->prclClip = &pBltParam->rclClip;

    pBltParam->rclDst   = *prclDst;
    pBltParam->prclDst  = &pBltParam->rclDst;

    pBltParam->pptlSrc  = pptlSrc;
    if (pptlSrc)
    {
        pBltParam->ptlSrc = *pptlSrc;
        pBltParam->pptlSrc = &pBltParam->ptlSrc;
    }

    pBltParam->pptlMask = pptlMask;
    if (pptlMask)
    {
        pBltParam->ptlMask = *pptlMask;
        pBltParam->pptlMask = &pBltParam->ptlMask;
    }

    pBltParam->pptlBrush= pptlBrush;
    if (pptlBrush)
    {
        pBltParam->ptlBrush = *pptlBrush;
        pBltParam->pptlBrush = &pBltParam->ptlBrush;
    }

    if (pptlMask)
    {
        sizMaskMinusDst.cx = pptlMask->x  - prclDst->left;
        sizMaskMinusDst.cy = pptlMask->y  - prclDst->top;
    }

    if (pptlSrc)
    {
        ASSERT(prclSrcBoard);

        sizSrcMinusDst.cx = pptlSrc->x  - prclDst->left;
        sizSrcMinusDst.cy = pptlSrc->y  - prclDst->top;

        if (bRclIntersect(&pBltParam->rclClip, prclClip, prclDstBoard))
        {
            if (bRclIntersect(&pBltParam->rclDst, prclDst, &pBltParam->rclClip))
            {
                pBltParam->ptlSrc.x = pBltParam->rclDst.left + sizSrcMinusDst.cx;
                pBltParam->ptlSrc.y = pBltParam->rclDst.top  + sizSrcMinusDst.cy;
                rclSrc.left         = pBltParam->ptlSrc.x;
                rclSrc.top          = pBltParam->ptlSrc.y;
                rclSrc.right        = pBltParam->ptlSrc.x + pBltParam->rclDst.right - pBltParam->rclDst.left;
                rclSrc.bottom       = pBltParam->ptlSrc.y + pBltParam->rclDst.bottom - pBltParam->rclDst.top;

                if (bRclIntersect(&rclSrc, &rclSrc, prclSrcBoard))
                {
                    vRclSubOffset(&pBltParam->rclDst, &rclSrc, sizSrcMinusDst.cx, sizSrcMinusDst.cy); 

                    vRclSubOffset(&pBltParam->rclClip, &pBltParam->rclClip, prclDstBoard->left, prclDstBoard->top); 
                    vRclSubOffset(&pBltParam->rclDst, &pBltParam->rclDst, prclDstBoard->left, prclDstBoard->top); 
                    vRclSubOffset(&rclSrc, &rclSrc, prclSrcBoard->left, prclSrcBoard->top); 
                    pBltParam->ptlSrc.x = rclSrc.left;
                    pBltParam->ptlSrc.y = rclSrc.top;

                    if (pptlMask)
                    {
                        pBltParam->ptlMask.x = pBltParam->rclDst.left + prclDstBoard->left + sizMaskMinusDst.cx;
                        pBltParam->ptlMask.y = pBltParam->rclDst.top + prclDstBoard->top + sizMaskMinusDst.cy;
                    }
                    if (pptlBrush)
                    {
                        pBltParam->ptlBrush.x = pptlBrush->x - prclDstBoard->left;
                        pBltParam->ptlBrush.y = pptlBrush->y - prclDstBoard->top;
                    }
                    bRet = TRUE;
                }
            }
        }

    }
    else
    {
        if (bRclIntersect(&pBltParam->rclClip, prclClip, prclDstBoard))
        {
            if (bRclIntersect(&pBltParam->rclDst, prclDst, &pBltParam->rclClip))
            {
                vRclSubOffset(&pBltParam->rclClip, &pBltParam->rclClip, prclDstBoard->left, prclDstBoard->top); 
                vRclSubOffset(&pBltParam->rclDst, &pBltParam->rclDst, prclDstBoard->left, prclDstBoard->top); 

                if (pptlMask)
                {
                    pBltParam->ptlMask.x = pBltParam->rclDst.left + prclDstBoard->left + sizMaskMinusDst.cx;
                    pBltParam->ptlMask.y = pBltParam->rclDst.top + prclDstBoard->top + sizMaskMinusDst.cy;
                }

                if (pptlBrush)
                {
                    pBltParam->ptlBrush.x = pptlBrush->x - prclDstBoard->left;
                    pBltParam->ptlBrush.y = pptlBrush->y - prclDstBoard->top;
                }
                bRet = TRUE;
            }
        }


    }



    return bRet;
}

//******************************************************************************
//
//  vPatchGlyphs
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
void vPatchGlyphs(GLYPHPOS*  pgp, ULONG cGlyphs, LONG xOffset, LONG yOffset)
{
    ULONG ul;
    ASSERT(pgp);

    if (xOffset != 0 || yOffset != 0)
    {
        for (ul = 0; ul < cGlyphs; ul++)
        {
            pgp[ul].ptl.x += xOffset;
            pgp[ul].ptl.y += yOffset;
        }
    }
}


//******************************************************************************
//
//  MulTextOut
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulTextOut(SURFOBJ*  pso, STROBJ*   pstro, FONTOBJ*  pfo,
                CLIPOBJ*  pco, RECTL*    prclExtra, RECTL*    prclOpaque,
                BRUSHOBJ* pboFore, BRUSHOBJ* pboOpaque, POINTL*   pptlOrg, MIX mix)
{
    PPDEV ppdev = (PPDEV)pso->dhpdev; 
    BOOL  bRet  = FALSE;

    DISPDBG((100, "MulTextOut >>>"));

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvTextOut(pso, pstro, pfo, pco, prclExtra, prclOpaque, pboFore, pboOpaque, pptlOrg, mix);
    }
    else
    {
        ULONG       cDevices;
        BOOL        bMore;
        BOOL        bMoreClip;
        ENUMDEV16   enumDev16;    
        ULONG       ulDevID;
        DEVOBJ      DevObj;
        BOARDDESC  *pBoardDesc;
        RECTL       rclClip;
        CLIPENUM    ce;
        LONG        l;
        POINTL     *pptlBoardOrg;
        POINTL      ptlOrg;
        RECTL       rclBoard;
        SURFOBJ    *psoBoard;
        RECTL       rclOpaque;
        RECTL      *prclBoardOpaque;
        STROBJ      SaveStrO;
        GLYPHPOS   *pgpOriginal;
        ULONG       cGlyphOriginal;
        BOOL        bMoreGlyphs;
        ULONG       ulGlyph;
        ULONG       iDComplexity;
        BOOL        bIntersectString;
        BOOL        bIntersectOpaque;

#if DBG
        DWORD dwLvl = 100;
        DISPDBG((dwLvl, "pco    :0x%x", pco));
        DISPDBG((dwLvl, "rclBkGd: (%d, %d) - (%d, %d)", pstro->rclBkGround.left, pstro->rclBkGround.top, pstro->rclBkGround.right, pstro->rclBkGround.bottom));
        if (prclOpaque)
            DISPDBG((dwLvl, "rclOpaque: (%d, %d) - (%d, %d)", prclOpaque->left, prclOpaque->top, prclOpaque->right, prclOpaque->bottom));
        if (prclExtra)
            DISPDBG((dwLvl, "rclExtra: (%d, %d) - (%d, %d)", prclExtra->left, prclExtra->top, prclExtra->right, prclExtra->bottom));
        if (pboFore)
            DISPDBG((dwLvl, "Fore Color: 0x%x", pboFore->iSolidColor));
        if (pboOpaque)
            DISPDBG((dwLvl, "Opaque Color: 0x%x", pboOpaque->iSolidColor));
#endif

        cDevices = DEVOBJ_cEnumStart(&DevObj, pso, NULL, TRUE, CD_ANY);
        if (cDevices)
        {
            do 
            {
                bMore = DEVOBJ_bEnum(&DevObj, &enumDev16);
                for (ulDevID = 0; ulDevID < enumDev16.c; ulDevID++)
                {
                    pBoardDesc = enumDev16.apbdDevices[ulDevID];
                    psoBoard = psoGetBoardRectAndSync( IN pso, IN pBoardDesc, IN NULL, OUT &rclBoard);

                    prclBoardOpaque = prclOpaque;
                    if (prclOpaque)
                    {
                        if (bRclIntersect(&rclOpaque, prclOpaque, &rclBoard))
                        {
                            vRclSubOffset(&rclOpaque, &rclOpaque, rclBoard.left, rclBoard.top);                            
                            prclBoardOpaque = &rclOpaque;
                        }

                    }

                    pptlBoardOrg = pptlOrg;
                    if (pptlBoardOrg)
                    {
                        ptlOrg.x = pptlOrg->x - rclBoard.left;
                        ptlOrg.y = pptlOrg->y - rclBoard.top;
                        pptlBoardOrg = &ptlOrg;
                    }


                    iDComplexity = NULL == pco ? DC_TRIVIAL : pco->iDComplexity;

                    if (DC_TRIVIAL == iDComplexity || DC_RECT == iDComplexity)
                    {
                        if (DC_RECT == iDComplexity)
                        {
                            if (!bRclIntersect(&pBoardDesc->pco->rclBounds, &pco->rclBounds, &rclBoard))
                                continue;
                        }
                        else
                        {
                            pBoardDesc->pco->rclBounds        = rclBoard;    
                        }

                        // string is completely outside of clip or board rect, so go haead next board
                        bIntersectOpaque = prclOpaque ? bRclIntersect(NULL, &pBoardDesc->pco->rclBounds, prclOpaque) : FALSE;
                        bIntersectString = bRclIntersect(NULL, &pBoardDesc->pco->rclBounds, &pstro->rclBkGround);

                        if (!bIntersectString && !bIntersectOpaque)
                            continue;

                        vRclSubOffset(&pBoardDesc->pco->rclBounds, &pBoardDesc->pco->rclBounds, rclBoard.left, rclBoard.top);

                        if (!pstro->pgp)
                            STROBJ_vEnumStart(pstro);

                        do {
                            SaveStrO   = *pstro;

                            if (pstro->pgp)
                            {
                                bMoreGlyphs     = FALSE;
                                pgpOriginal     = pstro->pgp;
                                cGlyphOriginal  = pstro->cGlyphs;
                            }
                            else
                            {
                                bMoreGlyphs = STROBJ_bEnum(pstro, &cGlyphOriginal, &pgpOriginal);
                            }

                            vPatchGlyphs(pgpOriginal, cGlyphOriginal, -rclBoard.left, -rclBoard.top);

                            pstro->pgp      = pgpOriginal;
                            pstro->cGlyphs  = cGlyphOriginal;

                            vRclSubOffset(&pstro->rclBkGround, &pstro->rclBkGround, rclBoard.left, rclBoard.top);

                            bRet = DrvTextOut(psoBoard, pstro, pfo, pBoardDesc->pco, prclExtra, prclBoardOpaque, pboFore, pboOpaque, pptlBoardOrg, mix);

                            vPatchGlyphs(pgpOriginal, cGlyphOriginal, rclBoard.left, rclBoard.top);

                            *pstro = SaveStrO;
                        }
                        while (bMoreGlyphs);
                    }
                    else
                    {
                        if (!bRclIntersect(&pBoardDesc->pco->rclBounds, &pco->rclBounds, &pBoardDesc->rclBoard))
                            continue;

                        // string is completely outside of clip or board rect, so go haead next board
                        bIntersectOpaque = prclOpaque ? bRclIntersect(NULL, &pBoardDesc->pco->rclBounds, prclOpaque) : FALSE;
                        bIntersectString = bRclIntersect(NULL, &pBoardDesc->pco->rclBounds, &pstro->rclBkGround);

                        if (!bIntersectString && !bIntersectOpaque)
                            continue;

                        if (!pstro->pgp)
                            STROBJ_vEnumStart(pstro);

                        do {
                            SaveStrO   = *pstro;
                            if (pstro->pgp)
                            {
                                bMoreGlyphs     = FALSE;
                                pgpOriginal     = pstro->pgp;
                                cGlyphOriginal  = pstro->cGlyphs;
                            }
                            else
                            {
                                bMoreGlyphs = STROBJ_bEnum(pstro, &cGlyphOriginal, &pgpOriginal);
                            }

                            vRclSubOffset(&pstro->rclBkGround, &pstro->rclBkGround, rclBoard.left, rclBoard.top);

                            vPatchGlyphs(pgpOriginal, cGlyphOriginal, -rclBoard.left, -rclBoard.top);

                            pstro->pgp      = pgpOriginal;
                            pstro->cGlyphs  = cGlyphOriginal;

                            CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

                            do  
                            {
                                bMoreClip = CLIPOBJ_bEnum(pco, sizeof(ce), (ULONG*) &ce);
                                for (l = 0; l < ce.c; l++)
                                {
                                    if (bRclIntersect(&pBoardDesc->pco->rclBounds, &ce.arcl[l], &pBoardDesc->rclBoard))
                                    {
                                        vRclSubOffset(&pBoardDesc->pco->rclBounds, &pBoardDesc->pco->rclBounds, pBoardDesc->rclBoard.left, pBoardDesc->rclBoard.top);

                                        bRet = DrvTextOut(psoBoard, pstro, pfo, pBoardDesc->pco, prclExtra, prclBoardOpaque, pboFore, pboOpaque, pptlBoardOrg, mix);
                                    }
                                }
                            }
                            while (bMoreClip);

                            vPatchGlyphs(pgpOriginal, cGlyphOriginal, rclBoard.left, rclBoard.top);

                            *pstro = SaveStrO;
                        }
                        while (bMoreGlyphs);
                    }
                }
            }
            while (bMore && bRet);
        }
    }
    
    DISPDBG((100,"<<< MulTextOut %d", bRet));
    return bRet;
}


//******************************************************************************
//
//  ulGetBlitDirection
//
//  NT4 multiboard helper function which calculates the direction of a blit
//  to get the right clip enumeration
//
//******************************************************************************
ULONG ulGetBlitDirection( POINTL* pptlSrc, RECTL* prclDst)
{
    ULONG ulDir = CD_ANY;

    ASSERT(prclDst);

    if (pptlSrc)
    {
        if (prclDst->left < pptlSrc->x)
        {
            if (prclDst->top < pptlSrc->y)
            {
                ulDir = CD_RIGHTDOWN;
            }
            else if (prclDst->top == pptlSrc->y)
            {
                ulDir = CD_RIGHTDOWN;
            }
            else
            {
                ulDir = CD_RIGHTUP;
            }
        }
        else if (prclDst->left == pptlSrc->x)
        {
            if (prclDst->top < pptlSrc->y)
            {
                ulDir = CD_RIGHTDOWN;
            }
            else if (prclDst->top == pptlSrc->y)
            {
                ulDir = CD_ANY;
            }
            else
            {
                ulDir = CD_RIGHTUP;
            }
        }
        else
        {
            if (prclDst->top < pptlSrc->y)
            {
                ulDir = CD_LEFTDOWN;
            }
            else if (prclDst->top == pptlSrc->y)
            {
                ulDir = CD_LEFTDOWN;
            }
            else
            {
                ulDir = CD_LEFTUP;
            }
        }
    }

    return ulDir;
}


//******************************************************************************
//
//  MulBitBlt
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulBitBlt(SURFOBJ*  psoDst, SURFOBJ*  psoSrc, SURFOBJ*  psoMask,
                CLIPOBJ*  pco, XLATEOBJ* pxlo, RECTL*    prclDst,
                POINTL*   pptlSrc, POINTL*   pptlMask, BRUSHOBJ* pbo,
                POINTL*   pptlBrush, ROP4      rop4)
{
    BOOL  bRet  = FALSE;
    PPDEV ppdev = NULL;

    DISPDBG((100, "MulBitBlt >>>"));

    if(psoDst)
        ppdev = (PPDEV)psoDst->dhpdev; 
    if(!ppdev)
    {
        ASSERT(psoSrc);
        ppdev = (PPDEV)psoSrc->dhpdev; 
    }

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
         bRet = DrvBitBlt(psoDst, psoSrc, psoMask, pco, pxlo, prclDst, pptlSrc, pptlMask, pbo, pptlBrush, rop4);
    }
    else
    {
        ULONG   ulDev;
        BOOL    bNeedToEnumTrg;
        BOOL    bNeedToEnumSrc;
        ULONG   cDstDevices;
        ULONG   cSrcDevices;
        BOOL    bDstMore;
        BOOL    bSrcMore;
        ENUMDEV16 enumDstDev16; 
        ENUMDEV16 enumSrcDev16; 
        ULONG   ulDstDevID;
        ULONG   ulSrcDevID;
        DEVOBJ  DstDevObj;
        DEVOBJ  SrcDevObj;
        BOARDDESC *pDstBoardDesc;
        BOARDDESC *pSrcBoardDesc;
        RECTL   rclDst;
        POINTL  ptlSrc;
        POINTL  ptlMask;
        POINTL  ptlBrush;
        BOOL    bMoreClip;
        LONG    l;
        CLIPENUM    ce;
        RECTL       rclDstBoard;
        RECTL       rclSrcBoard;
        BLT_PARAM   BltParam;
        ULONG       ulDir;

#if DBG
        DWORD dwLvl = 10;

        DISPDBG((dwLvl, "MulBitBlt >>>"));
        DISPDBG((dwLvl, "prclDst:0x%x", prclDst));
        DISPDBG((dwLvl, "pco    :0x%x", pco));
        DISPDBG((dwLvl, "rclDst: (%d, %d) - (%d, %d)", prclDst->left, prclDst->top, prclDst->right, prclDst->bottom));
        if (pbo)
            DISPDBG((dwLvl, "Color: 0x%x", pbo->iSolidColor));
#endif

        bNeedToEnumTrg = bNeedToEnumerate(psoDst);
        bNeedToEnumSrc = bNeedToEnumerate(psoSrc);

        if (bNeedToEnumTrg || bNeedToEnumSrc)
        {
            ulDir = ulGetBlitDirection(pptlSrc,prclDst);

            rclDst  = *prclDst;
            prclDst = &rclDst;

            if (pptlSrc)
            {
                ptlSrc  = *pptlSrc;
                pptlSrc = &ptlSrc;
            }

            if (pptlMask)
            {
                ptlMask  = *pptlMask;
                pptlMask = &ptlMask;
            }
            if (pptlBrush)
            {
                ptlBrush  = *pptlBrush;
                pptlBrush = &ptlBrush;
            }

            cDstDevices = DEVOBJ_cEnumStart(&DstDevObj, psoDst, NULL, TRUE, ulDir);
            if (cDstDevices)
            {
                bDstMore = TRUE;
                bRet  = TRUE;
                while (bDstMore && bRet)
                {
                    bDstMore = DEVOBJ_bEnum(&DstDevObj, &enumDstDev16);
                    for (ulDstDevID = 0; ulDstDevID < enumDstDev16.c; ulDstDevID++)
                    {
                        pDstBoardDesc = enumDstDev16.apbdDevices[ulDstDevID]; // maybe NULL on Memory bitmaps
                        BltParam.psoDst = psoGetBoardRectAndSync( IN psoDst, IN pDstBoardDesc, IN NULL, OUT &rclDstBoard);

                        cSrcDevices = DEVOBJ_cEnumStart(&SrcDevObj, psoSrc, NULL, TRUE, ulDir);
                        do
                        {
                            bSrcMore    = DEVOBJ_bEnum(&SrcDevObj, &enumSrcDev16);

                            for (ulSrcDevID = 0; ulSrcDevID < enumSrcDev16.c; ulSrcDevID++)
                            {
                                pSrcBoardDesc = enumSrcDev16.apbdDevices[ulSrcDevID];       // maybe NULL on Memory bitmaps

                                BltParam.psoSrc = psoGetBoardRectAndSync( IN psoSrc, IN pSrcBoardDesc, IN pDstBoardDesc, OUT &rclSrcBoard);
                                if (!BltParam.psoSrc)
                                    rclSrcBoard = rclDstBoard;

                                bRet = TRUE;
                                if (NULL == pco || DC_TRIVIAL == pco->iDComplexity)
                                {
                                    if (bTranslateBitBltParams( IN &rclDstBoard,
                                                                IN &rclSrcBoard,
                                                                rop4,
                                                                &rclDstBoard,  // as rclClip
                                                                prclDst, pptlSrc, pptlMask, pptlBrush, &BltParam))
                                        bRet = DrvBitBlt(BltParam.psoDst, BltParam.psoSrc, psoMask, NULL, pxlo, 
                                                        BltParam.prclDst, BltParam.pptlSrc, BltParam.pptlMask, pbo, BltParam.pptlBrush, rop4);
                                }
                                else if (DC_RECT == pco->iDComplexity)
                                {
                                    pDstBoardDesc->pco->rclBounds = pco->rclBounds;

                                    if (bTranslateBitBltParams( IN &rclDstBoard,
                                                                IN &rclSrcBoard,
                                                                rop4,
                                                                &pDstBoardDesc->pco->rclBounds,  // as rclClip
                                                                prclDst, pptlSrc, pptlMask, pptlBrush, &BltParam))
                                        bRet = DrvBitBlt(BltParam.psoDst, BltParam.psoSrc, psoMask, NULL, pxlo, 
                                                        BltParam.prclDst, BltParam.pptlSrc, BltParam.pptlMask, pbo, BltParam.pptlBrush, rop4);

                                }
                                else
                                {
                                    CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, ulDir, 0);
                                    do  
                                    {
                                        bMoreClip = CLIPOBJ_bEnum(pco, sizeof(CLIPENUM), (ULONG*) &ce);
                                        for (l = 0; l < ce.c; l++)
                                        {
                                            if (bTranslateBitBltParams( IN &rclDstBoard,
                                                                        IN &rclSrcBoard,
                                                                        rop4,
                                                                        &ce.arcl[l],  // as rclClip
                                                                        prclDst, pptlSrc, pptlMask, pptlBrush, &BltParam))
                                            bRet = DrvBitBlt(BltParam.psoDst, BltParam.psoSrc, psoMask, NULL, pxlo, 
                                                            BltParam.prclDst, BltParam.pptlSrc, BltParam.pptlMask, pbo, BltParam.pptlBrush, rop4);
                                        }
                                    }
                                    while (bMoreClip);
                                }
                            }
                        }
                        while (bSrcMore);
                    }
                }

            }
        }
        else
        {
            ASSERT(FALSE);
            bRet = EngBitBlt(psoDst, psoSrc, psoMask, pco, pxlo, prclDst, pptlSrc, pptlMask, pbo, pptlBrush, rop4);
        }
    }

    ASSERT(bRet);

    DISPDBG((100, "<<< MulBitBlt - %d", bRet));

    return bRet;
}


//******************************************************************************
//
//  MulCopyBits
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulCopyBits(SURFOBJ*  psoDst, SURFOBJ*  psoSrc, CLIPOBJ*  pco,
                XLATEOBJ* pxlo, RECTL*    prclDst, POINTL*   pptlSrc)
{
    BOOL  bRet  = FALSE;
    PPDEV ppdev = NULL;

    DISPDBG((100, "MulCopyBits >>>"));

    if(psoDst)
        ppdev = (PPDEV)psoDst->dhpdev; 
    if(!ppdev)
    {
        ASSERT(psoSrc);
        ppdev = (PPDEV)psoSrc->dhpdev; 
    }

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvCopyBits(psoDst, psoSrc, pco, pxlo, prclDst, pptlSrc);
    }
    else
    {

        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;
        BOOL    bNeedToEnumTrg;
        BOOL    bNeedToEnumSrc;
        ULONG   cDstDevices;
        ULONG   cSrcDevices;
        BOOL    bDstMore;
        BOOL    bSrcMore;
        ENUMDEV16 enumDstDev16; 
        ENUMDEV16 enumSrcDev16; 
        ULONG   ulDstDevID;
        ULONG   ulSrcDevID;
        DEVOBJ  DstDevObj;
        DEVOBJ  SrcDevObj;
        BOARDDESC *pDstBoardDesc;
        BOARDDESC *pSrcBoardDesc;
        RECTL   rclDst;
        POINTL  ptlSrc;
        BOOL    bMoreClip;
        LONG    l;
        CLIPENUM    ce;
        RECTL       rclDstBoard;
        RECTL       rclSrcBoard;
        BLT_PARAM   BltParam;
        ULONG       ulDir;

#if DBG
        DWORD dwLvl = 10;
        DISPDBG((dwLvl, "MulCopyBits >>>"));
        DISPDBG((dwLvl, "prclDst:0x%x", prclDst));
        DISPDBG((dwLvl, "pco    :0x%x", pco));
        DISPDBG((dwLvl, "rclDst: (%d, %d) - (%d, %d)", prclDst->left, prclDst->top, prclDst->right, prclDst->bottom));
#endif

        bNeedToEnumTrg = bNeedToEnumerate(psoDst);
        bNeedToEnumSrc = bNeedToEnumerate(psoSrc);

        if (bNeedToEnumTrg || bNeedToEnumSrc)
        {
            ulDir = ulGetBlitDirection(pptlSrc,prclDst);

            rclDst  = *prclDst;
            prclDst = &rclDst;

            if (pptlSrc)
            {
                ptlSrc  = *pptlSrc;
                pptlSrc = &ptlSrc;
            }

            cDstDevices = DEVOBJ_cEnumStart(&DstDevObj, psoDst, NULL, TRUE, ulDir);
            if (cDstDevices)
            {
                bDstMore = TRUE;
                bRet  = TRUE;
                while (bDstMore && bRet)
                {
                    bDstMore = DEVOBJ_bEnum(&DstDevObj, &enumDstDev16);
                    for (ulDstDevID = 0; ulDstDevID < enumDstDev16.c; ulDstDevID++)
                    {
                        pDstBoardDesc = enumDstDev16.apbdDevices[ulDstDevID]; // maybe NULL on Memory bitmaps
                        BltParam.psoDst = psoGetBoardRectAndSync( IN psoDst, IN pDstBoardDesc, IN NULL, OUT &rclDstBoard);

                        cSrcDevices = DEVOBJ_cEnumStart(&SrcDevObj, psoSrc, NULL, TRUE, ulDir);
                        do
                        {
                            bSrcMore    = DEVOBJ_bEnum(&SrcDevObj, &enumSrcDev16);

                            for (ulSrcDevID = 0; ulSrcDevID < enumSrcDev16.c; ulSrcDevID++)
                            {
                                pSrcBoardDesc = enumSrcDev16.apbdDevices[ulSrcDevID];       // maybe NULL on Memory bitmaps

                                BltParam.psoSrc = psoGetBoardRectAndSync( IN psoSrc, IN pSrcBoardDesc, IN pDstBoardDesc, OUT &rclSrcBoard);
                                if (!BltParam.psoSrc)
                                    rclSrcBoard = rclDstBoard;

                                bRet = TRUE;
                                if (NULL == pco || DC_TRIVIAL == pco->iDComplexity)
                                {
                                    if (bTranslateBitBltParams( IN &rclDstBoard,
                                                                IN &rclSrcBoard,
                                                                0xcccc,
                                                                &rclDstBoard,  // as rclClip
                                                                prclDst, pptlSrc, NULL, NULL, &BltParam))

                                        bRet = DrvCopyBits(BltParam.psoDst, BltParam.psoSrc, NULL, pxlo, BltParam.prclDst, BltParam.pptlSrc);
                                }
                                else if (DC_RECT == pco->iDComplexity)
                                {
                                    pDstBoardDesc->pco->rclBounds = pco->rclBounds;

                                    if (bTranslateBitBltParams( IN &rclDstBoard,
                                                                IN &rclSrcBoard,
                                                                0xcccc,
                                                                &pDstBoardDesc->pco->rclBounds,  // as rclClip
                                                                prclDst, pptlSrc, NULL, NULL, &BltParam))
                                        bRet = DrvCopyBits(BltParam.psoDst, BltParam.psoSrc, NULL, pxlo, BltParam.prclDst, BltParam.pptlSrc);

                                }
                                else
                                {
                                    CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, ulDir, 0);
                                    do  
                                    {
                                        bMoreClip = CLIPOBJ_bEnum(pco, sizeof(CLIPENUM), (ULONG*) &ce);
                                        for (l = 0; l < ce.c; l++)
                                        {
                                            if (bTranslateBitBltParams( IN &rclDstBoard,
                                                                        IN &rclSrcBoard,
                                                                        0xcccc,
                                                                        &ce.arcl[l],  // as rclClip
                                                                        prclDst, pptlSrc, NULL, NULL, &BltParam))
                                                bRet = DrvCopyBits(BltParam.psoDst, BltParam.psoSrc, NULL, pxlo, BltParam.prclDst, BltParam.pptlSrc);
                                        }
                                    }
                                    while (bMoreClip);
                                }
                            }
                        }
                        while (bSrcMore);
                    }
                }

            }
        }
        else
        {
            ASSERT(FALSE);
            bRet = DrvCopyBits(psoDst, psoSrc, pco, pxlo, prclDst, pptlSrc);
        }
    }

    ASSERT(bRet);

    DISPDBG((100, "<<< MulCopyBits - %d", bRet));
    return bRet;
}


//******************************************************************************
//
//  MulStretchBlt
//
//  NT4 multiboard wrapper function which redirects all calls to singeboard
//  functions of all devices
//
//******************************************************************************
BOOL MulStretchBlt( SURFOBJ*            psoDst,
                    SURFOBJ*            psoSrc,
                    SURFOBJ*            psoMask,
                    CLIPOBJ*            pco,
                    XLATEOBJ*           pxlo,
                    COLORADJUSTMENT*    pca,
                    POINTL*             pptlHTOrg,
                    RECTL*              prclDst,
                    RECTL*              prclSrc,
                    POINTL*             pptlMask,
                    ULONG               iMode)
{
    BOOL  bRet  = FALSE;
    PPDEV ppdev = NULL;

    DISPDBG((100, "MulStretchBlt >>>"));

    if(psoDst)
        ppdev = (PPDEV)psoDst->dhpdev; 
    if(!ppdev)
    {
        ASSERT(psoSrc);
        ppdev = (PPDEV)psoSrc->dhpdev; 
    }

    ASSERT(ppdev);

    if( !ppdev->pmdev )
    {
        bRet = DrvStretchBlt(psoDst, psoSrc, psoMask, pco, pxlo, pca, pptlHTOrg, prclDst, prclSrc, pptlMask, iMode);
    }
    else
    {

        PMDEV   pmdev = ppdev->pmdev;  
        ULONG   ulDev;

        // have to implement this if needed
        ASSERT( FALSE );
    }
    DISPDBG((100, "<<< MulStretchBlt"));
    return TRUE;
}


#endif // NT4_MULTI_DEV

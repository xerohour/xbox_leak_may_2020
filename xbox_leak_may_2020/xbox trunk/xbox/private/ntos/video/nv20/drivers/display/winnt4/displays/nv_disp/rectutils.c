/*
** $Header$
**
** MODULE:   rectutils.c
**
** AUTHOR:   FNicklisch, (c) nVidia GmbH
**
** DESCR:    Basic rectangle functions for Top Down coordinates
**           Bottom Up coordinates have BU at end of name
**
** History:
**   FNicklis 28.03.1996: created      
**   db       23.09.1996: RectDrawSingle added
**   KLux     05.11.1997: New enumeration functions for complex lists
**   FNicklis 11.12.1997: moved prototypes from enum.h
**   FNicklis 13.05.1998: remaming of inlines completed
**   FNicklis 19.06.1999: used in nVidia single and multiboard driver branches
**   FNicklis 10.08.1999: ported to OpenGL project
**   FNicklis 25.08.1999: new functions for Bottom Up coordinate space
**   FNicklisch 05.07.2001: ported to nvidia
*/

#include "precomp.h"
#include "driver.h"

// export
#include "rectutils.h"

/*
** ************************************************************************ 
** * main routines, chip independent                                      *
** ************************************************************************
*/

/*
** lRclMinusRcl
**
** calculates rclBounds - rclMinus, which may have 0 to 4 
** resulting rectangles stored in the prclResult array.
**
** return:
**   1-4 number of resulting rects stored in prclResult
**   0   nothing left from rclBounds, no rectangle stored.
**
** FNicklis 11.07.97: New
** FNicklis 21.10.98: simplified interface
*/
LONG lRclMinusRcl(OUT RECTL prclResult[],
                    IN  const RECTL *prclBounds, 
                    IN  const RECTL *prclMinus)
{
    RECTL rclWork;          // Copy of rclBounds we work on.
    RECTL rclHelp = {0};    // Intersecting rect between bounds and minus (part of minus within bounds)
    LONG  lRects  = 0;      // default exit
    
    ASSERTDD(NULL!=prclResult, "lRclMinusRcl: NULL==prclResult");
    ASSERTDD(NULL!=prclBounds, "lRclMinusRcl: NULL==prclBounds");
    //ASSERTDD(NULL!=prclMinus,   "lRclMinusRcl: NULL==prclMinus");
    
    // Minus is a non recangle (point) inside Bounds?
    if (   (NULL==prclMinus)
        || ( bRclIsEmpty(prclMinus) && bRclIsIn(prclBounds, prclMinus) ) ) 
    { 
        // e.g. where pwo->coClient.rclBounds is a nul-rect on OGL-startup
        // Nothing to substract
        DISPDBG((DBG_LVL_RECT, "minus is 0 and inside prclBounds"));
        *prclResult = *prclBounds;
        lRects        = 1;
        goto Exit;
    }
    
    // if bounds is completely inside minus -> trivial case
    if (bRclIsIn (prclMinus, prclBounds)) 
    { // Bounds inside Minus?
        DISPDBG((DBG_LVL_RECT, "bounds is inside minus, nothing left"));
        *prclResult = *prclBounds;
        lRects        = 0;
        goto Exit;
    }
    
    // if bounds isn't intersecting minus -> trivial case
    if (   !bRclIntersect(&rclHelp, prclBounds, prclMinus)
        || bRclIsEmpty(&rclHelp) )
    {
        DISPDBG((DBG_LVL_RECT, "not intersection (trivial case)"));
        *prclResult = *prclBounds;
        lRects        = 1;
        goto Exit;
    }
    
    // No we have the rect that has to be cut form bounds inside 
    // rclHelp. We make a copy of bounds (work) and run around help cutting
    // of overlapping parts from work. These parts are stored to the array.
    // The last remaing rect in work has to be equal help!
    
    //              H=minus rect
    //   111111111               11 111 111 11
    //   111111111               2H hhh 2H3 H2
    //   222HHH333               
    //   222HHH333               11      1  11
    //   222444333               2H 1H2  H  H2
    //   222444333               23      2  32
    //                           
    //
    rclWork = *prclBounds;
    
    // take rect above rclHelp
    prclResult[lRects].top    = rclWork.top;    
    prclResult[lRects].left   = rclWork.left;   
    prclResult[lRects].right  = rclWork.right;
    prclResult[lRects].bottom = rclHelp.top;  // ***
    
    if (!bRclIsEmpty(&prclResult[lRects])) 
    {
        // shrink rclWork and count rect
        rclWork.top = rclHelp.top;
        lRects++;
    }
    
    // take rect on the left side of rclHelp
    prclResult[lRects].top    = rclWork.top;
    prclResult[lRects].left   = rclWork.left;
    prclResult[lRects].right  = rclHelp.left; // ***
    prclResult[lRects].bottom = rclWork.bottom;
    
    if (!bRclIsEmpty(&prclResult[lRects])) 
    {
        // shrink rclWork and count rect
        rclWork.left = rclHelp.left;
        lRects++;
    }
    
    // take rect on the right side of rclHelp
    prclResult[lRects].top    = rclWork.top;
    prclResult[lRects].left   = rclHelp.right; // ***
    prclResult[lRects].right  = rclWork.right;
    prclResult[lRects].bottom = rclWork.bottom;
    
    if (!bRclIsEmpty(&prclResult[lRects])) 
    {
        // shrink rclWork and count rect
        rclWork.right = rclHelp.right;
        lRects++;
    }
    
    // take remaing rect below rclHelp
    prclResult[lRects].top    = rclHelp.bottom; // ***
    prclResult[lRects].left   = rclWork.left;
    prclResult[lRects].right  = rclWork.right;
    prclResult[lRects].bottom = rclWork.bottom;
    
    if (!bRclIsEmpty(&prclResult[lRects])) 
    {
        // shrink rclWork and count rect
        rclWork.bottom = rclHelp.bottom;
        lRects++;
    }
    
Exit:
    return(lRects);
} // lRclMinusRcl


/*
** lRclIntersectExclude
**
** stores a In rect to a Out rect cliped against bounds minus Exclude.
** This can result in up to four rectangles!
**
** out = (In - Exclude) & Bounds
**
** bounds, 
**
** Returns number of resulting rects.
**
** FNicklis 08.10.98: New
*/
LONG lRclIntersectExclude(
  OUT RECTL *prclOut,        // array of up to four resulting rectangles
  IN const RECTL  *prclIn,         // rect stored to Out
  IN const RECTL  *prclBounds,     // Has to be clipped against In
  IN const RECTL  *prclExclude)    // exclude must not be part of Out
{
  LONG lRet = 0;

  if (prclBounds)
  {
    // calculate intersection
    if (bRclIntersect(prclOut, prclIn, prclBounds))
    {
      lRet++;
    }
  }
  else
  {
    *prclOut = *prclIn;
    lRet++;
  }

  if ( (0 != lRet) && (NULL!=prclExclude) )
  {
    RECTL rclTemp = *prclOut; // Make a copy!

    // exclude prclExclude this rectangle from prclOut
    // this gives up to four new rectangles!

    // substract exclude from out
    lRet = lRclMinusRcl(prclOut, &rclTemp, prclExclude);
  }

#ifdef DEBUG
  // preset the unused rectangles to show a failure in screen!
  if ( 0 == lRet )
  {
    LONG l;
    for (l=0;l<4;l++)
      prclRclSetXYWH(prclOut+l, 0,0, 1024,768); // don't have ppdev, hardcoded rectangle
  }
#endif

  return (lRet);
}


// functions tests if prclSubject is left or above the prclReference
BOOL bIsLeftOrAbove(RECTL *prclSubject, RECTL *prclReference)
{
  BOOL bRet = FALSE;

  DBG_TRACE_IN(DBG_LVL_RECT, bIsLeftOrAbove);
  ASSERT(NULL!=prclSubject);
  ASSERT(NULL!=prclReference);

  if (prclSubject->right <= prclReference->left)
    bRet = TRUE;
  else
    if (prclSubject->bottom <= prclReference->top)
      bRet = TRUE;

  DBG_TRACE_OUT(DBG_LVL_RECT, bRet);
  return bRet;
}

// functions tests if prclSubject is right or above the prclReference
BOOL bIsRightOrAbove(RECTL *prclSubject, RECTL *prclReference)
{
  BOOL bRet = FALSE;

  DBG_TRACE_IN(DBG_LVL_RECT, bIsRightOrAbove);
  ASSERT(NULL!=prclSubject);
  ASSERT(NULL!=prclReference);

  if (prclSubject->left >= prclReference->right)
    bRet = TRUE;
  else
    if (prclSubject->bottom <= prclReference->top)
      bRet = TRUE;

  DBG_TRACE_OUT(DBG_LVL_RECT, bRet);
  return bRet;
}

// functions tests if prclSubject is left or below the prclReference
BOOL bIsLeftOrBelow(RECTL *prclSubject, RECTL *prclReference)
{
  BOOL bRet = FALSE;

  DBG_TRACE_IN(DBG_LVL_RECT, bIsLeftOrBelow);
  ASSERT(NULL!=prclSubject);
  ASSERT(NULL!=prclReference);

  if (prclSubject->right <= prclReference->left)
    bRet = TRUE;
  else
    if (prclSubject->top >= prclReference->bottom)
      bRet = TRUE;

  DBG_TRACE_OUT(DBG_LVL_RECT, bRet);
  return bRet;
}

// functions tests if prclSubject is right or below the prclReference
BOOL bIsRightOrBelow(RECTL *prclSubject, RECTL *prclReference)
{
  BOOL bRet = FALSE;

  DBG_TRACE_IN(DBG_LVL_RECT, bIsRightOrBelow);
  ASSERT(NULL!=prclSubject);
  ASSERT(NULL!=prclReference);

  if (prclSubject->left >= prclReference->right)
    bRet = TRUE;
  else
    if (prclSubject->top >= prclReference->bottom)
      bRet = TRUE;

  DBG_TRACE_OUT(DBG_LVL_RECT, bRet);
  return bRet;
}


#if 0 //disabled

/*
** bRclDrawSingle_SW
**
** handles all rop2 with P and D, but only Solid brushes
**
** 290797 FNicklis new
*/
BOOL bRclDrawSingle_SW (SURFOBJ  *pso, 
                         RECTL    *prcl, 
                         BRUSHOBJ *pbo, 
                         POINTL   *pptlBrushOrg, 
                         ROP4     rop4) 
{
  PPDEV ppdev;
  UNALIGNED BYTE *pjData;
  BOOL  bRet=FALSE;
  BOOL  bReadAndModify=FALSE;
  ULONG ulSolidColor;
  ULONG ulColor;
  BYTE  *pjColor= (BYTE *)&ulColor;
  WORD  *pwColor= (WORD *)&ulColor;
  LONG  x,y;
  ULONG rop3;
  BOOL  bValidGDI;

  DISPDBG((DBG_LVL_SUBENTRY, "  bRclDrawSingle_SW {"));
  ASSERTDD(NULL != pso,          "bRclDrawSingle_SW: pso == NULL");
  ASSERTDD(NULL != pso->dhpdev,  "bRclDrawSingle_SW: pso->dhpdev == NULL");
  ASSERTDD(NULL != prcl,         "bRclDrawSingle_SW: prcl == NULL");
  ASSERTDD(!bRectIsEmpty(prcl), "bRclDrawSingle_SW: prcl is empty rect");

  ppdev = (PPDEV)pso->dhpdev;

  ulSolidColor = pbo->iSolidColor;
  if ((ULONG)-1 == ulSolidColor) {
    DISPDBG((DBG_LVL_PUNT, "    ERROR in bRclDrawSingle_SW: Not a solid brush -> leaving"));
    goto Exit;
  }

  rop3 = (rop4>>8) & 0xff;

  if ((rop4 & 0xff) != rop3) {
    DISPDBG((DBG_LVL_PUNT, "    ERROR in bRclDrawSingle_SW: FG!=BG ROP:0x%x",rop4));
    goto Exit;
  }

  // Check basic 16 rops, 5 do not need to read destination -> set ulColor
  if (rop3==ROP3_P) {
    // Copy
    DISPDBG((DBG_LVL_FLOW1,    "    rop3:P"));
    ulColor=ulSolidColor;

  } else if (rop3==ROP3_0) {
    DISPDBG((DBG_LVL_FLOW1,    "    rop3:0"));
    ulColor=0;

  } else if (rop3==ROP3_1) {
    DISPDBG((DBG_LVL_FLOW1,    "    rop3:1"));
    ulColor=1;

  } else if (rop3==ROP3_D) {
    DISPDBG((DBG_LVL_FLOW1,    "    rop3:D(nop)"));
    bRet = TRUE; // NOP
    goto Exit;

  } else if (rop3==ROP3_Pn) {
    DISPDBG((DBG_LVL_FLOW1,    "    rop3:Pn"));
    ulColor=~ulSolidColor;

  } else {
    // Have to read destination
    DISPDBG((DBG_LVL_FLOW1,    "    rop3:0x%x", rop3));
    if (rop3==ROP3_DPna ||
        rop3==ROP3_DPno){
      DISPDBG((DBG_LVL_FLOW1,    " ?Pn?"));
      ulSolidColor=~ulSolidColor; // remember inverted solid color
    }
    bReadAndModify=TRUE;
  }

  pjData = ((BYTE *)pso->pvScan0) + prcl->top*pso->lDelta + prcl->left*ppdev->cjPelSize;

  DISPDBG((DBG_LVL_FLOW1,    " ulSolidColor:0x%x",ulSolidColor));
  vDbg_PrintRECTL(DBG_LVL_FLOW1, "    prcl:",prcl);
  DISPDBG((DBG_LVL_FLOW1,    "    iBytePerPixel:%d, ulBitCount:%d",iBytePerPixel,ppdev->ulBitCount));
  DISPDBG((DBG_LVL_FLOW1,    "    pvScan0:0x%x, lDelta:0x%x", pso->pvScan0,pso->lDelta));

  for (y=0; y<lRectHeight(prcl); y++) {
    UNALIGNED BYTE *pujWork  = (BYTE *)pjData;
    UNALIGNED WORD *puwWork =  (WORD *)pjData;
    ULONG *pulWork = (ULONG *)pjData;

    //DISPDBG((DBG_LVL_FLOW1, "    %d: pjData:0x%x", y, pjData));
    for (x=0; x<lRectWidth(prcl); x++) {

      // handle rops that need to read the destination?
      if (bReadAndModify) {
        // Read Destination
        UCDISPDBG(("WARNING bRclDrawSingle_SW called with rop, but not testet! - remove this line when OK"));
        switch (iBytePerPixel) {
          case 1:
            pjColor[0] = *pujWork;
            break;
          case 2:
            pwColor[0] = *puwWork;
            break;
          case 3:
            pjColor[0] = pujWork[0];
            pjColor[1] = pujWork[1];
            pjColor[2] = pujWork[2];
            break;
          case 4:
            ulColor = *pulWork;
            break;
          default:
            DISPDBG((DBG_LVL_ERROR, "    ERROR in bRclDrawSingle_SW: unknown iBytePerPixel=%d",iBytePerPixel));
            break;
        }
        // Destination Data from framebuffer now in ulColor

        if (ROP3_PDna==rop3 ||  
            ROP3_PDno==rop3 ||
            ROP3_Dn  ==rop3) {
          // Invert Destination
          ulColor = ~ulColor; 
        }

        // Now combine D and P
        if (ROP3_DPna==rop3 ||
            ROP3_PDna==rop3 ||
            ROP3_DPan==rop3 ||
            ROP3_DPa==rop3) {
          // D and P
          ulColor = ulColor & ulSolidColor; 
        } else if (ROP3_DPno==rop3 ||
                   ROP3_PDno==rop3 ||
                   ROP3_DPo ==rop3 ||
                   ROP3_DPon==rop3) {
          // D or P
          ulColor = ulColor | ulSolidColor; 
        } else if (ROP3_DPx ==rop3 ||
                   ROP3_PDxn==rop3) {
          // D xor P
          ulColor = ulColor ^ ulSolidColor;
        } 

        // Now if necesssary invert the result
        if (ROP3_DPan ==rop3 ||
            ROP3_PDxn==rop3 ||
            ROP3_DPon==rop3) {
          ulColor = ~ulColor; 
        }
      } // if ..bReadAndModify (ROPS)

      // Come here and copy the resulting color to the destination
      switch (iBytePerPixel) {
        case 1:
          *pujWork++ = pjColor[0];
          break;
        case 2:
          *puwWork++ = pwColor[0];
          break;
        case 3:
          *pujWork++ = pjColor[0];
          *pujWork++ = pjColor[1];
          *pujWork++ = pjColor[2];
          break;
        case 4:
          *pulWork++ = ulColor;
          break;
        default:
          DISPDBG((DBG_LVL_ERROR, "    ERROR in bRclDrawSingle_SW: unknown iBytePerPixel=%d",iBytePerPixel));
          break;
      }
    }
    pjData += pso->lDelta;
  }
  if (bValidGDI) ppdev->ReenableHwAccelAccess(ppdev); 

  bRet = TRUE;

Exit:
  DISPDBG((DBG_LVL_SUBENTRY, "  } RDS_SW: %s", bRet?"TRUE":"FALSE"));
  return bRet;
} // end of bRclDrawSingle_SW

#endif

/*
** same as vRclAddOffset but working on array
*/
VOID vRclListAddOffset(
       OUT RECTL *pTrg,     // output array long enough to store c rects
  const IN RECTL *pSrc,     // input array with c rects
        IN ULONG  c,        // number of rects to transform
        IN LONG   lXOffset, // x offset to add 
        IN LONG   lYOffset) // x offste to add
{
  ULONG cRect;
  ASSERT(NULL!=pTrg);
  ASSERT(NULL!=pSrc);

  for (cRect=0; cRect<c; cRect++)
  {
    vRclAddOffset(pTrg+cRect, pSrc+cRect, lXOffset, lYOffset);
  }
}

/*
** same as vRclSubOffset but working on array
*/
VOID vRclListSubOffset(
       OUT RECTL *pTrg,     // output array long enough to store c rects
  const IN RECTL *pSrc,     // input array with c rects
        IN ULONG  c,        // number of rects to transform
        IN LONG   lXOffset, // x offset to add 
        IN LONG   lYOffset) // x offste to add
{
  ULONG cRect;
  ASSERT(NULL!=pTrg);
  ASSERT(NULL!=pSrc);

  for (cRect=0; cRect<c; cRect++)
  {
    vRclSubOffset(pTrg+cRect, pSrc+cRect, lXOffset, lYOffset);
  }
}



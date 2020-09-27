#ifndef _RECT_UTILS_H
#define _RECT_UTILS_H

/*
** $Header$
**
** MODULE:   Rect_Utils.h
**
** AUTHOR:   FNicklis, (c) nVidia GmbH
**
** DESCR:    Basic rectangle functions for Top Down coordinates
**           Bottom Up coordinates have BU at end of name
**
** History:
**   MSchwarzer 10/17/2000: integrated from other codebase
*/

/* *** Basic Rectangle Functions *** */

static RECTL rclEmpty   = { 0,0,0,0 };
static RECTL rclOutside = {-1,-1,-1,-1};

// returns TRUE if Src1 intersects Src2 and intersection rect is stored to Trg
BOOL _inline bRclIntersect (
        OUT RECTL *Trg,  // can be NULL
  const IN  RECTL *Src1, // NULL not allowed
  const IN  RECTL *Src2) // NULL not allowed
{
  BOOL bRet=FALSE;

  ASSERT(NULL!=Src1);
  ASSERT(NULL!=Src2);

  if (   (Src1->left   >= Src2->right ) 
      || (Src1->right  <= Src2->left  ) 
      || (Src1->top    >= Src2->bottom) 
      || (Src1->bottom <= Src2->top   ) ) 
  {
    //Skuklik : If NO intersection, the resulting rectangle should be empty (0,0,0,0)
    if (Trg)
    {
      *Trg = rclEmpty;
    }
    goto Exit;
  }

  bRet = TRUE;

  if (NULL == Trg) 
  {
    goto Exit;
  }

  Trg->left     = max(Src1->left,   Src2->left);
  Trg->right    = min(Src1->right,  Src2->right);
  Trg->top      = max(Src1->top,    Src2->top);
  Trg->bottom   = min(Src1->bottom, Src2->bottom);

Exit:
  return(bRet);
}

// NULL params not allowed
BOOL _inline bRclIntersectBU (RECTL *Trg, const RECTL *Src1, const RECTL *Src2)
{
  BOOL bRet=FALSE;

  ASSERT(NULL!=Src1);
  ASSERT(NULL!=Src2);

  if (   (Src1->left   >= Src2->right ) 
      || (Src1->right  <= Src2->left  ) 
      || (Src1->bottom >= Src2->top   ) 
      || (Src1->top    <= Src2->bottom) ) 
  {
    goto Exit;
  }

  bRet = TRUE;

  if (NULL == Trg) 
  {
    goto Exit;
  }

  Trg->left     = max(Src1->left,   Src2->left);
  Trg->right    = min(Src1->right,  Src2->right);
  Trg->top      = min(Src1->top,    Src2->top);
  Trg->bottom   = max(Src1->bottom, Src2->bottom);

Exit:
  return(bRet);
}

// NULL params not allowed
BOOL _inline bRclIsEmpty(const RECTL *Src)
{
  BOOL bRet=FALSE;

  ASSERT(NULL!=Src);
  if ( ((Src->right  - Src->left) <= 0) || 
       ((Src->bottom - Src->top) <= 0) ) 
  {
    bRet = TRUE;
  }

  return(bRet);
}

// NULL params not allowed
LONG _inline lRclWidth(const RECTL *pSrc)   
{
  ASSERT(NULL!=pSrc);
  return (pSrc->right - pSrc->left); 
}

// NULL params not allowed
LONG _inline lRclHeight(const RECTL *pSrc)   
{
  ASSERT(NULL!=pSrc);
  return (pSrc->bottom - pSrc->top); 
}

// NULL params not allowed
LONG _inline lRclHeightBU(const RECTL *pSrc)   
{
  ASSERT(NULL!=pSrc);
  return (pSrc->top - pSrc->bottom); 
}

// NULL params not allowed
void _inline vRclSubOffset(OUT RECTL *pTrg, const IN RECTL *pSrc, IN LONG lXOffset, IN LONG lYOffset)
{

  ASSERT(NULL!=pSrc);
  ASSERT(NULL!=pTrg);

  pTrg->left     = pSrc->left   - lXOffset;
  pTrg->right    = pSrc->right  - lXOffset;
  pTrg->top      = pSrc->top    - lYOffset;
  pTrg->bottom   = pSrc->bottom - lYOffset;

  return;
}

//
// add the origin of the src rect to the dest rect
//
void __inline vRclAddOrigin(IN OUT PRECTL prclDest, const IN PRECTL prclSrc)
{
  LONG xOrigin, yOrigin;

  ASSERT(NULL != prclDest);
  ASSERT(NULL != prclSrc);
  
  xOrigin = prclSrc->left;
  yOrigin = prclSrc->top;

  prclDest->left   += xOrigin;
  prclDest->top    += yOrigin;
  prclDest->right  += xOrigin;
  prclDest->bottom += yOrigin;
}

// NULL params not allowed
void _inline vRclAddOffset (OUT RECTL *pTrg, const IN RECTL *pSrc, IN LONG lXOffset, IN LONG lYOffset)
{

  ASSERT(NULL!=pSrc);
  ASSERT(NULL!=pTrg);

  pTrg->left     = pSrc->left   + lXOffset;
  pTrg->right    = pSrc->right  + lXOffset;
  pTrg->top      = pSrc->top    + lYOffset;
  pTrg->bottom   = pSrc->bottom + lYOffset;

  return;
}


// NULL params not allowed
void _inline vRclBounds (OUT RECTL *Trg, IN const RECTL *Src1, IN const RECTL *Src2) 
{

  ASSERT(NULL!=Src1);
  ASSERT(NULL!=Src2);
  ASSERT(NULL!=Trg );

  Trg->left     = min(Src1->left,   Src2->left);
  Trg->right    = max(Src1->right,  Src2->right);
  Trg->top      = min(Src1->top,    Src2->top);
  Trg->bottom   = max(Src1->bottom, Src2->bottom);

  return;
}

// NULL params not allowed
BOOL _inline bRclCombine(IN OUT RECTL *prclInOut, IN const RECTL *prclNew)  
{
  BOOL bRet=TRUE;


  ASSERT(NULL != prclInOut);
  ASSERT(NULL != prclNew);

  if (prclNew->top == prclInOut->bottom)
  {
    if (prclNew->left == prclInOut->left && prclNew->right == prclInOut->right)
    {
      prclInOut->bottom = prclNew->bottom;
      goto Exit;
    }
  }
  if (prclNew->bottom == prclInOut->top)
  {
    if (prclNew->left == prclInOut->left && prclNew->right == prclInOut->right)
    {
      prclInOut->top = prclNew->top;
      goto Exit;
    }
  }
  if (prclNew->left == prclInOut->right)
  { 
    if (prclNew->top == prclInOut->top && prclNew->bottom == prclInOut->bottom)
    {
      prclInOut->right = prclNew->right;
      goto Exit;
    }
  }
  if (prclNew->right == prclInOut->left)
  {
    if (prclNew->top == prclInOut->top && prclNew->bottom == prclInOut->bottom)
    {
      prclInOut->left = prclNew->left;
      goto Exit;
    }
  }
  bRet=FALSE;

Exit:
  return(bRet);
}

/*
** Checks if inner rectangle is completly inside or equal 
** the outer rectangle.
** TRUE:  inner is completly surrounded by outer.
** FALSE: inner intersects outer or is outside
**
** 20.06.96 fi
*/ 
//FN it was time enough to use the new name: #define E_RclIsIn bRclIsIn 
// NULL params not allowed
_inline BOOL bRclIsIn (IN const RECTL *outer, IN const RECTL *inner) 
{
  BOOL bRet;

  ASSERT(NULL != outer);
  ASSERT(NULL != inner);
  bRet = ((outer->left <= inner->left) && (outer->right  >= inner->right) &&  // x-check
          (outer->top  <= inner->top)  && (outer->bottom >= inner->bottom));   // y-check

  return bRet;
} // end of bRclIsIn


/*
** Checks if inner rectangle is completly inside or equal 
** the outer rectangle.
** TRUE:  inner is completly surrounded by outer.
** FALSE: inner intersects outer or is outside
**
** 20.06.96 fi
*/ 
//FN it was time enough to use the new name: #define E_RclIsIn bRclIsIn 
// NULL params not allowed
_inline BOOL bRclIsInBU (IN const RECTL *outer, IN const RECTL *inner) 
{
  BOOL bRet;

  ASSERT(NULL != outer);
  ASSERT(NULL != inner);
  bRet = ((outer->left <= inner->left) && (outer->right  >= inner->right) &&  // x-check
          (outer->top  >= inner->top)  && (outer->bottom <= inner->bottom));   // y-check

  return bRet;
} // end of bRclIsInBU


// Checks whether a point is inside the rect
// NULL params not allowed
BOOL _inline bRclPtlInside (IN const RECTL *prcl, IN const POINTL *pptl)
{
  BOOL bRet;

  ASSERT(NULL!=prcl);
  ASSERT(NULL!=pptl);

  if ((pptl->x      < prcl->left)    || 
	    (pptl->x      >= prcl->right)  ||  // right border is outside the rect
      (pptl->y      < prcl->top)     || 
	    (pptl->y      >= prcl->bottom) )   // bottom border is outside
  { 
    // bottom border is outside the rect
    bRet = FALSE;
  }
  else
  {
    // Point is inside
    bRet = TRUE;
  }

  return(bRet);
}

// Checks whether a point is inside the rect
// NULL params not allowed
BOOL _inline bRclPtlInsideBU(IN const RECTL *prcl, IN const POINTL *pptl)
{
  BOOL bRet;

  ASSERT(NULL!=prcl);
  ASSERT(NULL!=pptl);

  if ((pptl->x      < prcl->left)    || 
	    (pptl->x      >= prcl->right)  ||  // right border is outside the rect
      (pptl->y      >= prcl->top)    ||  // top border is outside
	    (pptl->y      < prcl->bottom) ) 
  { 
    // bottom border is outside the rect
    bRet = FALSE;
  }
  else
  {
    // Point is inside
    bRet = TRUE;
  }

  return(bRet);
}

// Sets all members to 0
// NULL param allowed
VOID _inline vRclClear (OUT RECTL *prcl)
{
  if (prcl) {
    prcl->left   =
    prcl->right  =
    prcl->top    =
    prcl->bottom = 0;
  }
}

// **FN** 100697 Compares two rects either by pointer or members
// NULL params allowed
_inline BOOL bRclIsEqual (IN const RECTL *prcl1, IN const RECTL *prcl2)
{
  BOOL bRet;
 
  if (prcl1==prcl2) 
  {
    // both are equal pointers or NULL
    bRet = TRUE;

  } 
  else if (prcl1 && prcl2) 
  {
    // both point to a rect, compare members
    bRet =  (prcl1->left   == prcl2->left   &&
            prcl1->right  == prcl2->right  &&
            prcl1->top    == prcl2->top    &&
            prcl1->bottom == prcl2->bottom);
  } 
  else 
  {
    // One is NULL
    bRet = FALSE;
  }
  return (bRet);
} // end of bRclIsEqual 

__inline RECTL *prclRclOrder(OUT RECTL *prclOrdered, IN RECTL *prclIn)
{
  *prclOrdered = *prclIn; // Make a copy of the src rect

  ASSERT(prclOrdered!=prclIn);
  ASSERT(prclOrdered!=NULL);
  ASSERT(prclIn!=NULL);

  // Then check for ordering
  if (prclIn->left>prclIn->right)  {
    prclOrdered->left  = prclIn->right;
    prclOrdered->right = prclIn->left;
  }
  if (prclIn->top>prclIn->bottom)  {
    prclOrdered->top = prclIn->bottom;
    prclOrdered->bottom = prclIn->top;
  }

  return (prclOrdered);
}


// FN 031197
// Exchanges data of both input structs and returns a pointer to the first param if successful or NULL
__inline RECTL *prclRclSwap(IN OUT RECTL *prcl1, IN OUT RECTL *prcl2)
{
  RECTL rclTemp;

  if (NULL==prcl1 || NULL==prcl2) 
  {
    DISPDBG((0, "  ERROR in prclRclSwap: NULL rect - prcl1:0x%x, prcl2:0x%x",prcl1, prcl2));
    prcl1 = NULL;
  } 
  else 
  {
    rclTemp = *prcl1;
    *prcl1  = *prcl2;
    *prcl2  = rclTemp;
  }
  return (prcl1);
} // end of prclRclSwap


// FN 031197
// Exchanges data of both input structs and returns a pointer to the first param if successful or NULL
__inline SIZEL *psizSizeSwap(IN OUT SIZEL *psiz1, IN OUT SIZEL *psiz2)
{
  SIZEL sizTemp;

  if (NULL==psiz1 || NULL==psiz2) {
    DISPDBG((0, "  ERROR in psizSizeSwap: NULL size - psiz1:0x%x, psiz2:0x%x",psiz1, psiz2));
    psiz1 = NULL;
  } else {
    sizTemp = *psiz1;
    *psiz1  = *psiz2;
    *psiz2  = sizTemp;
  }
  return (psiz1);
} // end of psizSizeSwap

__inline RECTL *prclRclSetXYWH(OUT RECTL *prcl, IN LONG left, LONG top, 
                                   LONG width, LONG height)
{
  if (NULL!=prcl) {
    prcl->left   = left;
    prcl->right  = left + width;
    prcl->top    = top;
    prcl->bottom = top + height;
  }
  return prcl;
}

// FNicklis 01.11.98: counts the pixels in a rect (w*h)
__inline LONG lRclPixelCount(IN const RECTL *prcl)
{
  return (lRclWidth(prcl) * lRclHeight(prcl));
}

/*
** vRclTopDownToBottomUp
** transformation from TopDown to BottomUp coordinate space
*/
__inline VOID vRclTopDownToBottomUp(
  OUT RECTL *prclBottomUp,
  IN  RECTL *prclTopDown,
  IN  RECTL *prclReference) // taken as reference to perform transformation (need height)
{
  LONG lReferenceHeight;

  ASSERT(NULL!=prclBottomUp);
  ASSERT(NULL!=prclTopDown);

  lReferenceHeight = lRclHeight(prclReference);

  prclBottomUp->left   = prclTopDown->left;
  prclBottomUp->right  = prclTopDown->right;

  prclBottomUp->bottom = lReferenceHeight - prclTopDown->bottom;
  prclBottomUp->top    = lReferenceHeight - prclTopDown->top;
}

// vRclBottomUpToTopDown is simply the same as vRclTopDownToBottomUp
#define vRclBottomUpToTopDown vRclTopDownToBottomUp


// prclSrc copies contents of prclSrc to prclDst
__inline VOID vRclCopy(OUT RECTL *prclDst, IN RECTL *prclSrc)
{
    ASSERT(prclDst);
    ASSERT(prclSrc);
    *prclDst = *prclSrc; // Make a copy of the src rect
}


LONG lRclMinusRcl(OUT RECTL prclDest[], IN const RECTL *prclSrc, IN const RECTL *prclMinus);

LONG lRclIntersectExclude(
  OUT RECTL *prclOut,              // array of up to four resulting rectangles
  IN const RECTL  *prclIn,         // rect stored to Out
  IN const RECTL  *prclBounds,     // Has to be clipped against In
  IN const RECTL  *prclExclude);   // exclude must not be part of Out

BOOL bIsLeftOrAbove(RECTL *prclSubject, RECTL *prclReference);
BOOL bIsRightOrAbove(RECTL *prclSubject, RECTL *prclReference);
BOOL bIsLeftOrBelow(RECTL *prclSubject, RECTL *prclReference);
BOOL bIsRightOrBelow(RECTL *prclSubject, RECTL *prclReference);
VOID vRclListAddOffset(OUT RECTL *pTrg, const IN RECTL *pSrc, IN ULONG c, IN LONG lXOffset, IN LONG lYOffset);
VOID vRclListSubOffset(OUT RECTL *pTrg, const IN RECTL *pSrc, IN ULONG c, IN LONG lXOffset, IN LONG lYOffset);

#endif //#define _RECT_SUP_H


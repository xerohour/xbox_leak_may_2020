//******************************Module*Header***********************************
// Module Name: cliplist.c
//
// management functions for CLIPLIST
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************
#include "precomp.h"
#include "driver.h"
#include "RectUtils.h"

#include "cliplist.h"


//*************************************************************************
// bClipListAlloc
//
// allocates a CLIPLIST that can store cMax clip rects.
// All members are initilized to 0 if we don't fail.
//
// return: TRUE  - Success, cMax and array set to new values
//         FALSE - Failure, pClip isn't touched
//
// FNicklis 24.11.98: New
//*************************************************************************
BOOL bClipListAlloc(
  IN OUT CLIPLIST *pClip, // structure to fill out
  IN     LONG      cMax)  // number of rects to allocate
{
  BOOL   bRet = FALSE;
  RECTL *prcl = NULL;

  DBG_TRACE_IN(DBG_LVL_SUBENTRY, bClipListAlloc);

  ASSERTDD(bClipListCheck(pClip),"function isn't used correctly!");
  ASSERTDD(NULL==pClip->prcl,    "cliplist not empty!");
  ASSERTDD(0==pClip->cMax,       "cliplist not empty!");

  prcl = EngAllocMem(FL_ZERO_MEMORY, cMax*sizeof(RECTL), ALLOC_TAG);

  if (NULL == prcl)
  {
    // don't touch structure and exit
    DBG_ERROR("EngAllocMem failed for clip list");
    goto Exit;
  }

  // clear all members as we start a new clip list
  RtlZeroMemory(pClip, sizeof(CLIPLIST));

  pClip->cMax = cMax;
  pClip->prcl = prcl;

  bRet = TRUE;

Exit:
  DBG_TRACE_OUT(DBG_LVL_SUBENTRY, bRet);

  return (bRet);
}


//*************************************************************************
// bClipListFree
//
// Free a cliplist 
//
// return: TRUE=OK, FALSE=error.
//
// FNicklis 01.11.98: New from bFreeNegativeClipList
//*************************************************************************
BOOL bClipListFree(
  IN OUT CLIPLIST *pClip)
{
  BOOL      bRet = FALSE;

  DBG_TRACE_IN(DBG_LVL_SUBENTRY, bClipListFree);

  ASSERTDD(bClipListCheck(pClip),"function isn't used correctly!");

  // is there a cliplist to free?
  if ( (pClip->prcl != NULL) )
  {
    DISPDBG((10,"free %d rects in %d sized buffer",pClip->c,pClip->cMax));
    // all rects should have been counted too then
    // free and invalidate list
    EngFreeMem(pClip->prcl);

    // clear entire structure
    RtlZeroMemory(pClip, sizeof(CLIPLIST));
  }
  else
  {
    DISPDBG((10,"nothing to do"));
  }
  bRet = TRUE;

  DBG_TRACE_OUT(DBG_LVL_SUBENTRY, bRet);

  return (bRet);
}


#define CLIPLIST_GRANULARITY 8

//*************************************************************************
// bClipListReAlloc
//
// Realloc on clip list. Either shrink or grow a cliplist, keeping
// existing data.
//
// return: TRUE  - Success, cMax and array set to new values
//         FALSE - Failure, pClip isn't touched
//
// FNicklis 24.11.98: New
//*************************************************************************
BOOL bClipListReAlloc(
  IN OUT CLIPLIST *pClip,  // structure to fill out
  IN     LONG      cNew)   // number of rects to allocate
{
    BOOL      bRet = FALSE;
    RECTL    *prcl = NULL;
    LONG      cNewMax;
    LONG      c;
    CLIPLIST  NewClip = {0};

    ASSERTDD(bClipListCheck(pClip),"function isn't used correctly!");
    ASSERTDD(NULL!=pClip->prcl,    "cliplist empty!");
    ASSERTDD(0!=pClip->cMax,       "cliplist empty!");

    cNewMax = max(CLIPLIST_GRANULARITY, ((cNew + CLIPLIST_GRANULARITY - 1) / CLIPLIST_GRANULARITY) * CLIPLIST_GRANULARITY);

    if (cNewMax != pClip->cMax)
    {
        if (!bClipListAlloc(&NewClip, cNewMax))
          goto Exit;

        c = min(pClip->c, cNew);

        // copy current rects (if available) to new array
        if (c > 0)
        {
            RtlCopyMemory(NewClip.prcl, pClip->prcl, (c * sizeof(RECTL)) );
            NewClip.c = c;
        }

        bClipListFree(pClip);

        *pClip = NewClip;
    }
    // else nothing to do

    bRet = TRUE;

Exit:
    ASSERTDD(bClipListCheck(pClip),"clip list is now destroyed!");
    return (bRet);
}


//*************************************************************************
// bClipListCopy
//
// Copy max of cOutMax rects out of CLIPLIST to prclOut
//
// return: count of rects copied to prclOut
//
// FNicklis 01.11.98: New
//*************************************************************************
LONG lClipListCopy(
  OUT       RECTL    *prclOut,
  IN        LONG      cOutMax,
  IN  const CLIPLIST *pClip)
{
  LONG lRects = 0; // default is "nothing done"

  DBG_TRACE_IN(DBG_LVL_SUBENTRY, lClipListCopy);
  
  ASSERTDD(bClipListCheck(pClip),"function isn't used correctly!");
  ASSERTDD(cOutMax>0,               "function isn't used correctly!");
  ASSERTDD(NULL!=prclOut,           "function isn't used correctly!");
  
  if (pClip->c > 0)
  {
    ASSERTDD(NULL!=prclOut, "");

    // cannot copy more than room in output buffer!
    lRects = min(pClip->c,cOutMax);

    RtlCopyMemory(
      prclOut,              // dst
      pClip->prcl,          // src
      lRects*sizeof(RECTL));// count
  }

  // check if outputbuffer was to small
  DISPDBGCOND(lRects!=pClip->c, (1, "WARNING in lClipListCopy: cOutMax:%d for pClip->c:%d to small?",cOutMax, pClip->c));

  DBG_TRACE_OUT(DBG_LVL_SUBENTRY, lRects);
  return (lRects);
}

LONG lClipListPixelCount(CLIPLIST *pClip)
{
  LONG      lRects;

  pClip->cPixel = 0;
  for (lRects = 0; lRects <pClip->c; lRects++)
  {
    pClip->cPixel += lRclPixelCount(&pClip->prcl[lRects]);
  }
  return (pClip->cPixel);
}


BOOL bClipListCheck(
  IN  const CLIPLIST *pClip)
{
  BOOL bRet = FALSE;

  if (NULL==pClip)
    bRet = FALSE; // invalid pointer
  else
  {
    if ( (pClip->prcl!=NULL) &&          // if allocated
         (pClip->cMax   >  0) &&         // we need a max amount of rects
         (pClip->cMax   >= pClip->c) &&  // cannot store more than max
         (pClip->c      >= 0) &&         // rects should be stored, or 0 -> reason to free a buffer!
         (pClip->cPixel >= 0) )          // pixels should be counted
    {
     bRet = TRUE;
    }
    else if ( (pClip->prcl   == NULL) && // not allocated
              (pClip->cMax   == 0) &&    // max must be null
              (pClip->c      == 0) &&    // nothing stored
              (pClip->cPixel == 0) )     // no pixels
    {
     bRet = TRUE;
    }
    else
    {
      bRet = FALSE;                     // error
    }
  }
  return (bRet);
}


// End of cliplist.c
